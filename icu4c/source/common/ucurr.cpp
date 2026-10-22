// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

#شامل <utility>

#شامل "unicode/ucurr.h"
#شامل "unicode/locid.h"
#شامل "unicode/ures.h"
#شامل "unicode/ustring.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/uniset.h"
#شامل "unicode/usetiter.h"
#شامل "unicode/utf16.h"
#شامل "ustr_imp.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "static_unicode_sets.h"
#شامل "uassert.h"
#شامل "umutex.h"
#شامل "ucln_cmn.h"
#شامل "uenumimp.h"
#شامل "uhash.h"
#شامل "hash.h"
#شامل "uinvchar.h"
#شامل "uresimp.h"
#شامل "ulist.h"
#شامل "uresimp.h"
#شامل "ureslocs.h"
#شامل "ulocimp.h"

استعمال نامخلا icu;

//#define UCURR_DEBUG_EQUIV 1
#اگر_تعریف_شدہ UCURR_DEBUG_EQUIV
#شامل "stdio.h"
#اگر_ختم
//#define UCURR_DEBUG 1
#اگر_تعریف_شدہ UCURR_DEBUG
#شامل "stdio.h"
#اگر_ختم

قسمتعریف ساخت Iso_رمز_اندراج {
    ثابت char16_t *iso_رمز; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate مقصد;
} Iso_رمز_اندراج;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ساکن ثابت int32_t آخری_RESORT_ڈیٹا[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ساکن ثابت int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ساکن ثابت int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#تعریف_کرو ISO_CURRENCY_رمز_لمبائی 3

//------------------------------------------------------------
// Resource tags
//

ساکن ثابت حرف CURRENCY_ڈیٹا[] = "supplementalData";
// Tag for meta-data, in root.
ساکن ثابت حرف CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
ساکن ثابت حرف CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
ساکن ثابت حرف DEFAULT_META[] = "DEFAULT";

// Variant delimiter
ساکن ثابت حرف VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
ساکن ثابت حرف CURRENCIES[] = "Currencies";
ساکن ثابت حرف CURRENCIES_NARROW[] = "Currencies%narrow";
ساکن ثابت حرف CURRENCIES_FORMAL[] = "Currencies%formal";
ساکن ثابت حرف CURRENCIES_VARIANT[] = "Currencies%variant";
ساکن ثابت حرف CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
ساکن ثابت UHashtable* gIsoCodes = خالی_اشاریہ;
ساکن icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ساکن ثابت icu::Hashtable* gCurrSymbolsEquiv = خالی_اشاریہ;
ساکن icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
جماعت EquivIterator : عوامی icu::UMemory {
عوامی:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    اندرونی EquivIterator(ثابت icu::Hashtable& hash, ثابت icu::UnicodeString& s)
        : _hash(hash) { 
        آغاز_2 = _current = &s;
    }
    اندرونی ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ثابت icu::UnicodeString *next();
نجی:
    ثابت icu::Hashtable& _hash;
    ثابت icu::UnicodeString* آغاز_2;
    ثابت icu::UnicodeString* _current;
};

ثابت icu::UnicodeString *
EquivIterator::next() {
    ثابت icu::UnicodeString* اگلا_2 = static_cast<ثابت icu::UnicodeString*>(_hash.get(*_current));
    اگر (اگلا_2 == خالی_اشاریہ) {
        U_ASSERT(_current == آغاز_2);
        واپس خالی_اشاریہ;
    }
    اگر (*اگلا_2 == *آغاز_2) {
        واپس خالی_اشاریہ;
    }
    _current = اگلا_2;
    واپس اگلا_2;
}

U_NAMESPACE_انتہا

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ساکن خالی makeEquivalent(
    ثابت icu::UnicodeString &lhs,
    ثابت icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (lhs == rhs) {
        // already equivalent
        واپس;
    }
    icu::EquivIterator بایاں_Iter(*hash, lhs);
    icu::EquivIterator دایاں_Iter(*hash, rhs);
    ثابت icu::UnicodeString *پہلا_بایاں = بایاں_Iter.next();
    ثابت icu::UnicodeString *پہلا_دایاں = دایاں_Iter.next();
    ثابت icu::UnicodeString *اگلا_بایاں = پہلا_بایاں;
    ثابت icu::UnicodeString *اگلا_دایاں = پہلا_دایاں;
    جب (اگلا_بایاں != خالی_اشاریہ && اگلا_دایاں != خالی_اشاریہ) {
        اگر (*اگلا_بایاں == rhs || *اگلا_دایاں == lhs) {
            // Already equivalent
            واپس;
        }
        اگلا_بایاں = بایاں_Iter.next();
        اگلا_دایاں = دایاں_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_پہلا_بایاں;
    icu::UnicodeString *new_پہلا_دایاں;
    اگر (پہلا_دایاں == خالی_اشاریہ && پہلا_بایاں == خالی_اشاریہ) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_پہلا_بایاں = نیا icu::UnicodeString(rhs);
        new_پہلا_دایاں = نیا icu::UnicodeString(lhs);
    } ورنہ اگر (پہلا_دایاں == خالی_اشاریہ) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_پہلا_بایاں = نیا icu::UnicodeString(rhs);
        new_پہلا_دایاں = نیا icu::UnicodeString(*پہلا_بایاں);
    } ورنہ اگر (پہلا_بایاں == خالی_اشاریہ) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_پہلا_بایاں = نیا icu::UnicodeString(*پہلا_دایاں);
        new_پہلا_دایاں = نیا icu::UnicodeString(lhs);
    } ورنہ {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_پہلا_بایاں = نیا icu::UnicodeString(*پہلا_دایاں);
        new_پہلا_دایاں = نیا icu::UnicodeString(*پہلا_بایاں);
    }
    اگر (new_پہلا_بایاں == خالی_اشاریہ || new_پہلا_دایاں == خالی_اشاریہ) {
        حذف new_پہلا_بایاں;
        حذف new_پہلا_دایاں;
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    hash->put(lhs, (خالی *) new_پہلا_بایاں, حالت_2);
    hash->put(rhs, (خالی *) new_پہلا_دایاں, حالت_2);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ساکن int32_t گنتی_Equivalent(ثابت icu::Hashtable &hash, ثابت icu::UnicodeString &s) {
    int32_t نتیجہ = 0;
    icu::EquivIterator iter(hash, s);
    جب (iter.next() != خالی_اشاریہ) {
        ++نتیجہ;
    }
#اگر_تعریف_شدہ UCURR_DEBUG_EQUIV
 {
   حرف tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   لکھو("CountEquivalent('%s') = %d\n", tmp, نتیجہ);
 }
#اگر_ختم
    واپس نتیجہ;
}

ساکن ثابت icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//رمز

/**
 * Cleanup callback func
 */
ساکن UBool U_CALLCONV 
isoCodes_cleanup()
{
    اگر (gIsoCodes != خالی_اشاریہ) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = خالی_اشاریہ;
    }
    gIsoCodesInitOnce.reset();
    واپس سچ;
}

/**
 * Cleanup callback func
 */
ساکن UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    حذف const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = خالی_اشاریہ;
    gCurrSymbolsEquivInitOnce.reset();
    واپس سچ;
}

/**
 * Deleter for IsoCodeEntry
 */
ساکن خالی U_CALLCONV
delete_Iso_رمز_اندراج(خالی *obj) {
    Iso_رمز_اندراج* اندراج_2 = static_cast<Iso_رمز_اندراج*>(obj);
    uprv_free(اندراج_2);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ساکن خالی U_CALLCONV
deleteUnicode(خالی *obj) {
    icu::UnicodeString* اندراج_2 = static_cast<icu::UnicodeString*>(obj);
    حذف اندراج_2;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ساکن اندرونی حرف*
my_UChars_مقصد_Chars(حرف* نتیجہ_کا_Len4, ثابت char16_t* currency) {
    u_UChars_مقصد_Chars(currency, نتیجہ_کا_Len4, ISO_CURRENCY_رمز_لمبائی);
    نتیجہ_کا_Len4[ISO_CURRENCY_رمز_لمبائی] = 0;
    واپس نتیجہ_کا_Len4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
ساکن ثابت int32_t*
ڈھونڈو_Meta_ڈیٹا(ثابت char16_t* currency, UError_رمز& ec) {

    اگر (currency == خالی_اشاریہ || *currency == 0) {
        اگر (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_دلیل_ERROR;
        }
        واپس آخری_RESORT_ڈیٹا;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_ڈیٹا = ures_open_براہ_راست(U_ICUDATA_CURR, CURRENCY_ڈیٹا, &ec);
    Local_UResource_Bundle_اشارہ_گر currencyMeta(ures_get_By_کلید(currency_ڈیٹا, CURRENCY_META, currency_ڈیٹا, &ec));

    اگر (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        واپس آخری_RESORT_ڈیٹا;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    حرف buf[ISO_CURRENCY_رمز_لمبائی+1];
    UError_رمز ec2 = U_ZERO_ERROR; // local error code: soft failure
    Local_UResource_Bundle_اشارہ_گر rb(ures_get_By_کلید(currencyMeta.getAlias(), my_UChars_مقصد_Chars(buf, currency), خالی_اشاریہ, &ec2));
      اگر (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_کلید(currencyMeta.getAlias(),DEFAULT_META, خالی_اشاریہ, &ec));
        اگر (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            واپس آخری_RESORT_ڈیٹا;
        }
    }

    int32_t len;
    ثابت int32_t *ڈیٹا = ures_get_Int_سمتیہ(rb.getAlias(), &len, &ec);
    اگر (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        اگر (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        واپس آخری_RESORT_ڈیٹا;
    }

    واپس ڈیٹا;
}

// -------------------------------------

ساکن CharString
idForLocale(ثابت حرف* locale, UError_رمز* ec)
{
    واپس ulocimp_get_Region_For_Supplemental_ڈیٹا(locale, جھوٹ, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ساکن UBool U_CALLCONV currency_cleanup();
U_CDECL_انتہا

#اگر !UCONFIG_NO_SERVICE
ساخت CReg;

ساکن UMutex g_CReg_تالا;
ساکن CReg* g_CReg_سر = خالی_اشاریہ;

ساخت CReg : عوامی icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_رمز_لمبائی+1];
    حرف  id[ULOC_FULLNAME_استعداد];

    CReg(ثابت char16_t* _iso, ثابت حرف* _id)
        : next(خالی_اشاریہ)
    {
        uprv_strncpy(id, _id, حجم(id)-1);
        id[حجم(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_رمز_لمبائی);
        iso[ISO_CURRENCY_رمز_لمبائی] = 0;
    }

    ساکن UCurr_Registry_کلید reg(ثابت char16_t* _iso, ثابت حرف* _id, UError_رمز* حالت_2)
    {
        اگر (حالت_2 && U_SUCCESS(*حالت_2) && _iso && _id) {
            CReg* n = نیا CReg(_iso, _id);
            اگر (n) {
                umtx_تالا(&g_CReg_تالا);
                اگر (!g_CReg_سر) {
                    /* register for the first time */
                    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_سر;
                g_CReg_سر = n;
                umtx_unlock(&g_CReg_تالا);
                واپس n;
            }
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
        }
        واپس خالی_اشاریہ;
    }

    ساکن UBool unreg(UCurr_Registry_کلید کلید) {
        UBool ملا = جھوٹ;
        umtx_تالا(&g_CReg_تالا);

        CReg** p = &g_CReg_سر;
        جب (*p) {
            اگر (*p == کلید) {
                *p = ((CReg*)کلید)->next;
                حذف (CReg*)کلید;
                ملا = سچ;
                توڑو;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_تالا);
        واپس ملا;
    }

    ساکن ثابت char16_t* get(ثابت حرف* id) {
        ثابت char16_t* نتیجہ = خالی_اشاریہ;
        umtx_تالا(&g_CReg_تالا);
        CReg* p = g_CReg_سر;

        /* register cleanup of the mutex */
        ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        جب (p) {
            اگر (uprv_strcmp(id, p->id) == 0) {
                نتیجہ = p->iso;
                توڑو;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_تالا);
        واپس نتیجہ;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ساکن خالی cleanup() {
        جب (g_CReg_سر) {
            CReg* n = g_CReg_سر;
            g_CReg_سر = g_CReg_سر->next;
            حذف n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_کلید U_EXPORT2
ucurr_رجسٹر(ثابت char16_t* iso_رمز, ثابت حرف* locale, UError_رمز *حالت_2)
{
    اگر (حالت_2 && U_SUCCESS(*حالت_2)) {
        CharString id = idForLocale(locale, حالت_2);
        واپس CReg::reg(iso_رمز, id.ڈیٹا(), حالت_2);
    }
    واپس خالی_اشاریہ;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_کلید کلید, UError_رمز* حالت_2)
{
    اگر (حالت_2 && U_SUCCESS(*حالت_2)) {
        واپس CReg::unreg(کلید);
    }
    واپس جھوٹ;
}
#اگر_ختم /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
ساکن UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
ساکن UBool U_CALLCONV currency_cleanup() {
#اگر !UCONFIG_NO_SERVICE
    CReg::cleanup();
#اگر_ختم
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    واپس سچ;
}
U_CDECL_انتہا

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ثابت حرف* locale,
                char16_t* buff,
                int32_t buff_استعداد,
                UError_رمز* ec) {
    اگر (U_FAILURE(*ec)) { واپس 0; }
    اگر (buff_استعداد < 0 || (buff == خالی_اشاریہ && buff_استعداد > 0)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    UError_رمز local_حالت = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_قدر(locale, "currency", local_حالت);
    int32_t resLen = currency.length();

    اگر (U_SUCCESS(local_حالت) && resLen == 3 && uprv_isInvariantString(currency.ڈیٹا(), resLen)) {
        اگر (resLen < buff_استعداد) {
            T_CString_مقصد_بالائی_Case(currency.ڈیٹا());
            u_chars_مقصد_UChars(currency.ڈیٹا(), buff, resLen);
        }
        واپس u_terminateUChars(buff, buff_استعداد, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    اگر (U_FAILURE(*ec)) {
        واپس 0;
    }

#اگر !UCONFIG_NO_SERVICE
    ثابت char16_t* نتیجہ = CReg::get(id.ڈیٹا());
    اگر (نتیجہ) {
        اگر(buff_استعداد > u_strlen(نتیجہ)) {
            u_strcpy(buff, نتیجہ);
        }
        resLen = u_strlen(نتیجہ);
        واپس u_terminateUChars(buff, buff_استعداد, resLen, ec);
    }
#اگر_ختم
    // Remove variants, which is only needed for registration.
    حرف *idDelim = uprv_strchr(id.ڈیٹا(), VAR_DELIM);
    اگر (idDelim) {
        id.truncate(idDelim - id.ڈیٹا());
    }

    ثابت char16_t* s = خالی_اشاریہ;  // Currency code from data file.
    اگر (id.is_خالی_خانے()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_حالت = U_MISSING_RESOURCE_ERROR;
    } ورنہ {
        // Look up the CurrencyMap element in the root bundle.
        local_حالت = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_براہ_راست(U_ICUDATA_CURR, CURRENCY_ڈیٹا, &local_حالت);
        UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_حالت);
        Local_UResource_Bundle_اشارہ_گر countryArray(ures_get_By_کلید(rb, id.ڈیٹا(), cm, &local_حالت));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        اگر (U_SUCCESS(local_حالت)) {
            int32_t array_لمبائی = ures_get_سائز(countryArray.getAlias());
            برائے (int32_t i = 0; i < array_لمبائی; ++i) {
                Local_UResource_Bundle_اشارہ_گر currencyReq(
                    ures_get_By_اشاریہ(countryArray.getAlias(), i, خالی_اشاریہ, &local_حالت));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_رمز tender_حالت = local_حالت;
                ثابت char16_t *tender =
                    ures_get_String_By_کلید(currencyReq.getAlias(), "tender", خالی_اشاریہ, &tender_حالت);
                منطقی isTender = U_FAILURE(tender_حالت) || u_strcmp(tender, u"false") != 0;
                اگر (!isTender && s != خالی_اشاریہ) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    جاری;
                }
                // Fetch the currency code.
                s = ures_get_String_By_کلید(currencyReq.getAlias(), "id", &resLen, &local_حالت);
                اگر (isTender) {
                    توڑو;
                }
            }
            اگر (U_SUCCESS(local_حالت) && s == خالی_اشاریہ) {
                local_حالت = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    اگر ((U_FAILURE(local_حالت)) && strchr(id.ڈیٹا(), '_') != خالی_اشاریہ) {
        // We don't know about it.  Check to see if we support the variant.
        CharString والد = ulocimp_get_والد(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        واپس ucurr_forLocale(والد.ڈیٹا(), buff, buff_استعداد, ec);
    }
    اگر (*ec == U_ZERO_ERROR || local_حالت != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_حالت;
    }
    اگر (U_SUCCESS(*ec)) {
        اگر(buff_استعداد > resLen) {
            u_strcpy(buff, s);
        }
    }
    واپس u_terminateUChars(buff, buff_استعداد, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
ساکن UBool fallback(CharString& loc) {
    اگر (loc.is_خالی_خانے()) {
        واپس جھوٹ;
    }
    UError_رمز حالت_2 = U_ZERO_ERROR;
    اگر (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", حالت_2);
    } ورنہ {
        loc = ulocimp_get_والد(loc.ڈیٹا(), حالت_2);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    واپس سچ;
}


U_CAPI ثابت char16_t* U_EXPORT2
ucurr_getName(ثابت char16_t* currency U_LIFETIME_BOUND,
              ثابت حرف* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UError_رمز* ec) {

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

    اگر (U_FAILURE(*ec)) {
        واپس خالی_اشاریہ;
    }

    int32_t choice = (int32_t) nameStyle;
    اگر (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
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
    UError_رمز ec2 = U_ZERO_ERROR;

    اگر (locale == خالی_اشاریہ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    اگر (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    حرف buf[ISO_CURRENCY_رمز_لمبائی+1];
    my_UChars_مقصد_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_مقصد_بالائی_Case(buf);
    
    ثابت char16_t* s = خالی_اشاریہ;
    ec2 = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارہ_گر rb(ures_open(U_ICUDATA_CURR, loc.ڈیٹا(), &ec2));

    اگر (nameStyle == UCURR_NARROW_علامت_NAME || nameStyle == UCURR_FORMAL_علامت_NAME || nameStyle == UCURR_VARIANT_علامت_NAME) {
        CharString کلید;
        انتخاب (nameStyle) {
        حالت UCURR_NARROW_علامت_NAME:
            کلید.append(CURRENCIES_NARROW, ec2);
            توڑو;
        حالت UCURR_FORMAL_علامت_NAME:
            کلید.append(CURRENCIES_FORMAL, ec2);
            توڑو;
        حالت UCURR_VARIANT_علامت_NAME:
            کلید.append(CURRENCIES_VARIANT, ec2);
            توڑو;
        طےشدہ:
            *ec = U_UNSUPPORTED_ERROR;
            واپس خالی_اشاریہ;
        }
        کلید.append("/", ec2);
        کلید.append(buf, ec2);
        s = ures_get_String_By_کلید_With_Fallback(rb.getAlias(), کلید.ڈیٹا(), len, &ec2);
        اگر (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_علامت_NAME;
        }
    }
    اگر (s == خالی_اشاریہ) {
        ures_get_By_کلید(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_کلید_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_اشاریہ(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    اگر (U_SUCCESS(ec2)) {
        اگر (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    اگر (isChoiceFormat != خالی_اشاریہ) {
        *isChoiceFormat = جھوٹ;
    }
    اگر (U_SUCCESS(ec2)) {
        U_ASSERT(s != خالی_اشاریہ);
        واپس s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    واپس currency;
}

U_CAPI ثابت char16_t* U_EXPORT2
ucurr_getPluralName(ثابت char16_t* currency U_LIFETIME_BOUND,
                    ثابت حرف* locale,
                    UBool* isChoiceFormat,
                    ثابت حرف* plural_گنتی,
                    int32_t* len, // fillin
                    UError_رمز* ec) {
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

    اگر (U_FAILURE(*ec)) {
        واپس خالی_اشاریہ;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_رمز ec2 = U_ZERO_ERROR;

    اگر (locale == خالی_اشاریہ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    اگر (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    حرف buf[ISO_CURRENCY_رمز_لمبائی+1];
    my_UChars_مقصد_Chars(buf, currency);

    ثابت char16_t* s = خالی_اشاریہ;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.ڈیٹا(), &ec2);

    rb = ures_get_By_کلید(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    Local_UResource_Bundle_اشارہ_گر curr(ures_get_By_کلید_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_کلید_With_Fallback(curr.getAlias(), plural_گنتی, len, &ec2);
    اگر (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_کلید_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        اگر (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            واپس ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    اگر (U_SUCCESS(ec2)) {
        اگر (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != خالی_اشاریہ);
        واپس s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    واپس currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#تعریف_کرو NEED_مقصد_BE_DELETED 0x1

// TODO: a better way to define this?
#تعریف_کرو MAX_CURRENCY_NAME_LEN 100

قسمتعریف ساخت {
    ثابت حرف* Iso_رمز;  //کلید
    char16_t* currencyName;  //قدر
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#اگر_غیر_تعریف_شدہ MIN
#تعریف_کرو MIN(a,b) (((a)<(b)) ? (a) : (b))
#اگر_ختم

#اگر_غیر_تعریف_شدہ MAX
#تعریف_کرو MAX(a,b) (((a)<(b)) ? (b) : (a))
#اگر_ختم


// Comparison function used in quick sort.
ساکن صحیح U_CALLCONV currencyNameComparator(ثابت خالی* a, ثابت خالی* b) {
    ثابت CurrencyNameStruct* currName_1 = static_cast<ثابت CurrencyNameStruct*>(a);
    ثابت CurrencyNameStruct* currName_2 = static_cast<ثابت CurrencyNameStruct*>(b);
    برائے (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        اگر (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            واپس -1;
        }
        اگر (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            واپس 1;
        }
    }
    اگر (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        واپس -1;
    } ورنہ اگر (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        واپس 1;
    }
    واپس 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
ساکن خالی
get_Currency_Name_گنتی(ثابت حرف* loc, int32_t* کل_currency_name_گنتی, int32_t* کل_currency_علامت_گنتی) {
    U_NAMESPACE_USE
    *کل_currency_name_گنتی = 0;
    *کل_currency_علامت_گنتی = 0;
    ثابت char16_t* s = خالی_اشاریہ;
    CharString locale;
    {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        locale.append(loc, حالت_2);
        اگر (U_FAILURE(حالت_2)) { واپس; }
    }
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    برائے (;;) {
        UError_رمز ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        Local_UResource_Bundle_اشارہ_گر rb(ures_open(U_ICUDATA_CURR, locale.ڈیٹا(), &ec2));
        Local_UResource_Bundle_اشارہ_گر curr(ures_get_By_کلید(rb.getAlias(), CURRENCIES, خالی_اشاریہ, &ec2));
        int32_t n = ures_get_سائز(curr.getAlias());
        برائے (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارہ_گر نام(ures_get_By_اشاریہ(curr.getAlias(), i, خالی_اشاریہ, &ec2));
            int32_t len;
            s = ures_get_String_By_اشاریہ(نام.getAlias(), UCURR_علامت_NAME, &len, &ec2);
            ++(*کل_currency_علامت_گنتی);  // currency symbol
            اگر (currencySymbolsEquiv != خالی_اشاریہ) {
                *کل_currency_علامت_گنتی += گنتی_Equivalent(*currencySymbolsEquiv, UnicodeString(سچ, s, len));
            }
            ++(*کل_currency_علامت_گنتی); // iso code
            ++(*کل_currency_name_گنتی); // long name
        }

        // currency plurals
        UError_رمز ec3 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشارہ_گر curr_p(ures_get_By_کلید(rb.getAlias(), CURRENCYPLURALS, خالی_اشاریہ, &ec3));
        n = ures_get_سائز(curr_p.getAlias());
        برائے (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارہ_گر نام(ures_get_By_اشاریہ(curr_p.getAlias(), i, خالی_اشاریہ, &ec3));
            *کل_currency_name_گنتی += ures_get_سائز(نام.getAlias());
        }

        اگر (!fallback(locale)) {
            توڑو;
        }
    }
}

ساکن char16_t*
مقصد_بالائی_Case(ثابت char16_t* ماخذ, int32_t len, ثابت حرف* locale) {
    char16_t* dest = خالی_اشاریہ;
    UError_رمز ec = U_ZERO_ERROR;
    int32_t destLen = u_str_مقصد_بالائی(dest, 0, ماخذ, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * MAX(destLen, len)));
    اگر (dest == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    u_str_مقصد_بالائی(dest, destLen, ماخذ, len, locale, &ec);
    اگر (U_FAILURE(ec)) {
        u_memcpy(dest, ماخذ, len);
    } 
    واپس dest;
}


ساکن خالی delete_Currency_نام(CurrencyNameStruct* currency_نام, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ساکن خالی
collect_Currency_نام(ثابت حرف* locale, 
                     CurrencyNameStruct** currency_نام, 
                     int32_t* کل_currency_name_گنتی, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* کل_currency_علامت_گنتی, 
                     UError_رمز& ec) {
    اگر (U_FAILURE(ec)) {
        *currency_نام = *currencySymbols = خالی_اشاریہ;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
        واپس;
    }
    U_NAMESPACE_USE
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_رمز ec2 = U_ZERO_ERROR;

    اگر (locale == خالی_اشاریہ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    اگر (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_دلیل_ERROR;
        *currency_نام = *currencySymbols = خالی_اشاریہ;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
        واپس;
    }

    // Get maximum currency name count first.
    get_Currency_Name_گنتی(loc.ڈیٹا(), کل_currency_name_گنتی, کل_currency_علامت_گنتی);

    *currency_نام = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*کل_currency_name_گنتی)));
    اگر(*currency_نام == خالی_اشاریہ) {
        *currencySymbols = خالی_اشاریہ;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
        ec = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*کل_currency_علامت_گنتی)));

    اگر(*currencySymbols == خالی_اشاریہ) {
        uprv_free(*currency_نام);
        *currency_نام = خالی_اشاریہ;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
        ec = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    ثابت char16_t* s = خالی_اشاریہ;  // currency name
    حرف* iso = خالی_اشاریہ;  // currency ISO code

    *کل_currency_name_گنتی = 0;
    *کل_currency_علامت_گنتی = 0;

    UError_رمز ec3 = U_ZERO_ERROR;
    UError_رمز ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    Local_UHashtable_اشارہ_گر currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, خالی_اشاریہ, &ec3));
    Local_UHashtable_اشارہ_گر currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, خالی_اشاریہ, &ec4));
    برائے (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        Local_UResource_Bundle_اشارہ_گر rb(ures_open(U_ICUDATA_CURR, loc.ڈیٹا(), &ec2));
        Local_UResource_Bundle_اشارہ_گر curr(ures_get_By_کلید(rb.getAlias(), CURRENCIES, خالی_اشاریہ, &ec2));
        int32_t n = ures_get_سائز(curr.getAlias());
        برائے (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارہ_گر نام(ures_get_By_اشاریہ(curr.getAlias(), i, خالی_اشاریہ, &ec2));
            int32_t len;
            s = ures_get_String_By_اشاریہ(نام.getAlias(), UCURR_علامت_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<حرف*>(ures_get_کلید(نام.getAlias()));
            اگر (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != خالی_اشاریہ) {
                جاری;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*کل_currency_علامت_گنتی)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            اگر (currencySymbolsEquiv != خالی_اشاریہ) {
                UnicodeString str(سچ, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ثابت UnicodeString *علامت;
                جب ((علامت = iter.next()) != خالی_اشاریہ) {
                    (*currencySymbols)[(*کل_currency_علامت_گنتی)++]
                        = {iso, const_cast<char16_t*>(علامت->get_بفر()), علامت->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_اشاریہ(نام.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* بالائی_Name = مقصد_بالائی_Case(s, len, locale);
            اگر (بالائی_Name == خالی_اشاریہ) {
                ec = U_حافظہ_تخصیص_ERROR;
                جاؤ error;
            }
            (*currency_نام)[(*کل_currency_name_گنتی)++] = {iso, بالائی_Name, len, NEED_مقصد_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_رمز = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * 3));
            اگر (iso_رمز == خالی_اشاریہ) {
                ec = U_حافظہ_تخصیص_ERROR;
                جاؤ error;
            }
            // Must convert iso[] into Unicode
            u_chars_مقصد_UChars(iso, iso_رمز, 3);
            (*currencySymbols)[(*کل_currency_علامت_گنتی)++] = {iso, iso_رمز, 3, NEED_مقصد_BE_DELETED};
        }

        // currency plurals
        UError_رمز ec5 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشارہ_گر curr_p(ures_get_By_کلید(rb.getAlias(), CURRENCYPLURALS, خالی_اشاریہ, &ec5));
        n = ures_get_سائز(curr_p.getAlias());
        برائے (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارہ_گر نام(ures_get_By_اشاریہ(curr_p.getAlias(), i, خالی_اشاریہ, &ec5));
            iso = const_cast<حرف*>(ures_get_کلید(نام.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            اگر (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != خالی_اشاریہ) {
                جاری;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_سائز(نام.getAlias());
            int32_t len;
            برائے (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_اشاریہ(نام.getAlias(), j, &len, &ec5);
                char16_t* بالائی_Name = مقصد_بالائی_Case(s, len, locale);
                اگر (بالائی_Name == خالی_اشاریہ) {
                    ec = U_حافظہ_تخصیص_ERROR;
                    جاؤ error;
                }
                (*currency_نام)[(*کل_currency_name_گنتی)++] = {iso, بالائی_Name, len, NEED_مقصد_BE_DELETED};
            }
        }

        اگر (!fallback(loc)) {
            توڑو;
        }
    }

    // quick sort the struct
    qsort(*currency_نام, *کل_currency_name_گنتی, 
          حجم(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *کل_currency_علامت_گنتی, 
          حجم(CurrencyNameStruct), currencyNameComparator);

#اگر_تعریف_شدہ UCURR_DEBUG
    لکھو("currency name count: %d\n", *کل_currency_name_گنتی);
    برائے (int32_t اشاریہ = 0; اشاریہ < *کل_currency_name_گنتی; ++اشاریہ) {
        لکھو("index: %d\n", اشاریہ);
        لکھو("iso: %s\n", (*currency_نام)[اشاریہ].Iso_رمز);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_نام)[اشاریہ].currencyName, (*currency_نام)[اشاریہ].currencyNameLen);
        لکھو("currencyName: %s\n", curNameBuf);
        لکھو("len: %d\n", (*currency_نام)[اشاریہ].currencyNameLen);
    }
    لکھو("currency symbol count: %d\n", *کل_currency_علامت_گنتی);
    برائے (int32_t اشاریہ = 0; اشاریہ < *کل_currency_علامت_گنتی; ++اشاریہ) {
        لکھو("index: %d\n", اشاریہ);
        لکھو("iso: %s\n", (*currencySymbols)[اشاریہ].Iso_رمز);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[اشاریہ].currencyName, (*currencySymbols)[اشاریہ].currencyNameLen);
        لکھو("currencySymbol: %s\n", curNameBuf);
        لکھو("len: %d\n", (*currencySymbols)[اشاریہ].currencyNameLen);
    }
#اگر_ختم
    // fail on hashtable errors
    اگر (U_FAILURE(ec3)) {
      ec = ec3;
    } ورنہ اگر (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    اگر (U_FAILURE(ec)) {
        delete_Currency_نام(*currency_نام, *کل_currency_name_گنتی);
        delete_Currency_نام(*currencySymbols, *کل_currency_علامت_گنتی);
        *currency_نام = *currencySymbols = خالی_اشاریہ;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ساکن int32_t
ثنائی_تلاش(ثابت CurrencyNameStruct* currency_نام, 
             int32_t اشاریہ_اندر_Currency_نام,
             ثابت char16_t کلید,
             int32_t* begin, int32_t* انتہا) {
#اگر_تعریف_شدہ UCURR_DEBUG
    لکھو("key = %x\n", کلید);
#اگر_ختم
   int32_t first = *begin;
   int32_t آخری = *انتہا;
   جب (first <= آخری) {
       int32_t mid = (first + آخری) / 2;  // compute mid point.
       اگر (اشاریہ_اندر_Currency_نام >= currency_نام[mid].currencyNameLen) {
           first = mid + 1;
       } ورنہ {
           اگر (کلید > currency_نام[mid].currencyName[اشاریہ_اندر_Currency_نام]) {
               first = mid + 1;
           }
           ورنہ اگر (کلید < currency_نام[mid].currencyName[اشاریہ_اندر_Currency_نام]) {
               آخری = mid - 1;
           }
           ورنہ {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#اگر_تعریف_شدہ UCURR_DEBUG
                لکھو("mid = %d\n", mid);
#اگر_ختم
                جب (L < R) {
                    int32_t M = (L + R) / 2;
#اگر_تعریف_شدہ UCURR_DEBUG
                    لکھو("L = %d, R = %d, M = %d\n", L, R, M);
#اگر_ختم
                    اگر (اشاریہ_اندر_Currency_نام >= currency_نام[M].currencyNameLen) {
                        L = M + 1;
                    } ورنہ {
                        اگر (currency_نام[M].currencyName[اشاریہ_اندر_Currency_نام] < کلید) {
                            L = M + 1;
                        } ورنہ {
#اگر_تعریف_شدہ UCURR_DEBUG
                            U_ASSERT(currency_نام[M].currencyName[اشاریہ_اندر_Currency_نام] == کلید);
#اگر_ختم
                            R = M;
                        }
                    }
                }
#اگر_تعریف_شدہ UCURR_DEBUG
                U_ASSERT(L == R);
#اگر_ختم
                *begin = L;
#اگر_تعریف_شدہ UCURR_DEBUG
                لکھو("begin = %d\n", *begin);
                U_ASSERT(currency_نام[*begin].currencyName[اشاریہ_اندر_Currency_نام] == کلید);
#اگر_ختم

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *انتہا;
                جب (L < R) {
                    int32_t M = (L + R) / 2;
#اگر_تعریف_شدہ UCURR_DEBUG
                    لکھو("L = %d, R = %d, M = %d\n", L, R, M);
#اگر_ختم
                    اگر (currency_نام[M].currencyNameLen < اشاریہ_اندر_Currency_نام) {
                        L = M + 1;
                    } ورنہ {
                        اگر (currency_نام[M].currencyName[اشاریہ_اندر_Currency_نام] > کلید) {
                            R = M;
                        } ورنہ {
#اگر_تعریف_شدہ UCURR_DEBUG
                            U_ASSERT(currency_نام[M].currencyName[اشاریہ_اندر_Currency_نام] == کلید);
#اگر_ختم
                            L = M + 1;
                        }
                    }
                }
#اگر_تعریف_شدہ UCURR_DEBUG
                U_ASSERT(L == R);
#اگر_ختم
                اگر (currency_نام[R].currencyName[اشاریہ_اندر_Currency_نام] > کلید) {
                    *انتہا = R - 1;
                } ورنہ {
                    *انتہا = R;
                }
#اگر_تعریف_شدہ UCURR_DEBUG
                لکھو("end = %d\n", *انتہا);
#اگر_ختم

                // now, found the range. check whether there is exact match
                اگر (currency_نام[*begin].currencyNameLen == اشاریہ_اندر_Currency_نام + 1) {
                    واپس *begin;  // find range and exact match.
                }
                واپس -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *انتہا = -1;
   واپس -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ساکن خالی
خطی_تلاش(ثابت CurrencyNameStruct* currency_نام, 
             int32_t begin, int32_t انتہا,
             ثابت char16_t* متن, int32_t متن_Len,
             int32_t *partial_مطابقت_Len,
             int32_t *max_مطابقت_Len, int32_t* max_مطابقت_اشاریہ) {
    int32_t initial_Partial_مطابقت_Len = *partial_مطابقت_Len;
    برائے (int32_t اشاریہ = begin; اشاریہ <= انتہا; ++اشاریہ) {
        int32_t len = currency_نام[اشاریہ].currencyNameLen;
        اگر (len > *max_مطابقت_Len && len <= متن_Len &&
            uprv_memcmp(currency_نام[اشاریہ].currencyName, متن, len * حجم(char16_t)) == 0) {
            *partial_مطابقت_Len = MAX(*partial_مطابقت_Len, len);
            *max_مطابقت_اشاریہ = اشاریہ;
            *max_مطابقت_Len = len;
#اگر_تعریف_شدہ UCURR_DEBUG
            لکھو("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_مطابقت_اشاریہ, *max_مطابقت_Len);
#اگر_ختم
        } ورنہ {
            // Check for partial matches.
            برائے (int32_t i=initial_Partial_مطابقت_Len; i<MIN(len, متن_Len); i++) {
                اگر (currency_نام[اشاریہ].currencyName[i] != متن[i]) {
                    توڑو;
                }
                *partial_مطابقت_Len = MAX(*partial_مطابقت_Len, i + 1);
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
ساکن خالی
تلاش_Currency_Name(ثابت CurrencyNameStruct* currency_نام, 
                   int32_t کل_currency_گنتی,
                   ثابت char16_t* متن, int32_t متن_Len,
                   int32_t *partial_مطابقت_Len,
                   int32_t* max_مطابقت_Len, int32_t* max_مطابقت_اشاریہ) {
    *max_مطابقت_اشاریہ = -1;
    *max_مطابقت_Len = 0;
    int32_t مطابقت_اشاریہ = -1;
    int32_t ثنائی_تلاش_Begin = 0;
    int32_t ثنائی_تلاش_انتہا = کل_currency_گنتی - 1;
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
    برائے (int32_t اشاریہ = 0; اشاریہ < متن_Len; ++اشاریہ) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        مطابقت_اشاریہ = ثنائی_تلاش(currency_نام, اشاریہ,
                                  متن[اشاریہ],
                                  &ثنائی_تلاش_Begin, &ثنائی_تلاش_انتہا);
        اگر (ثنائی_تلاش_Begin == -1) { // did not find the range
            توڑو;
        }
        *partial_مطابقت_Len = MAX(*partial_مطابقت_Len, اشاریہ + 1);
        اگر (مطابقت_اشاریہ != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_مطابقت_Len = اشاریہ + 1;
            *max_مطابقت_اشاریہ = مطابقت_اشاریہ;
        }
        اگر (ثنائی_تلاش_انتہا - ثنائی_تلاش_Begin < خطی_تلاش_حد) {
            // linear search if within threshold.
            خطی_تلاش(currency_نام, ثنائی_تلاش_Begin, ثنائی_تلاش_انتہا,
                         متن, متن_Len,
                         partial_مطابقت_Len,
                         max_مطابقت_Len, max_مطابقت_اشاریہ);
            توڑو;
        }
    }
}

//========================= currency name cache =====================
قسمتعریف ساخت {
    حرف locale[ULOC_FULLNAME_استعداد];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_نام;  //قدر
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
ساکن Currency_Name_Cache_اندراج* currCache[CURRENCY_NAME_CACHE_عدد] = {خالی_اشاریہ};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ساکن int8_t current_Cache_اندراج_اشاریہ = 0;

ساکن UMutex gCurrencyCacheMutex;

// Cache deletion
ساکن خالی
delete_Currency_نام(CurrencyNameStruct* currency_نام, int32_t count) {
    برائے (int32_t اشاریہ = 0; اشاریہ < count; ++اشاریہ) {
        اگر ( (currency_نام[اشاریہ].flag & NEED_مقصد_BE_DELETED) ) {
            uprv_free(currency_نام[اشاریہ].currencyName);
        }
    }
    uprv_free(currency_نام);
}


ساکن خالی
delete_Cache_اندراج(Currency_Name_Cache_اندراج* اندراج_2) {
    delete_Currency_نام(اندراج_2->currency_نام, اندراج_2->کل_Currency_Name_گنتی);
    delete_Currency_نام(اندراج_2->currencySymbols, اندراج_2->کل_Currency_علامت_گنتی);
    uprv_free(اندراج_2);
}


// Cache clean up
ساکن UBool U_CALLCONV
currency_cache_cleanup() {
    برائے (int32_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
        اگر (currCache[i]) {
            delete_Cache_اندراج(currCache[i]);
            currCache[i] = خالی_اشاریہ;
        }
    }
    واپس سچ;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ساکن Currency_Name_Cache_اندراج*
get_Cache_اندراج(ثابت حرف* locale, UError_رمز& ec) {

    int32_t کل_currency_name_گنتی = 0;
    CurrencyNameStruct* currency_نام = خالی_اشاریہ;
    int32_t کل_currency_علامت_گنتی = 0;
    CurrencyNameStruct* currencySymbols = خالی_اشاریہ;
    Currency_Name_Cache_اندراج* cache_اندراج = خالی_اشاریہ;

    umtx_تالا(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t ملا = -1;
    برائے (int8_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
        اگر (currCache[i]!= خالی_اشاریہ &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            ملا = i;
            توڑو;
        }
    }
    اگر (ملا != -1) {
        cache_اندراج = currCache[ملا];
        ++(cache_اندراج->ref_گنتی);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    اگر (ملا == -1) {
        collect_Currency_نام(locale, &currency_نام, &کل_currency_name_گنتی, &currencySymbols, &کل_currency_علامت_گنتی, ec);
        اگر (U_FAILURE(ec)) {
            واپس خالی_اشاریہ;
        }
        umtx_تالا(&gCurrencyCacheMutex);
        // check again.
        برائے (int8_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
            اگر (currCache[i]!= خالی_اشاریہ &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                ملا = i;
                توڑو;
            }
        }
        اگر (ملا == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_اندراج = currCache[current_Cache_اندراج_اشاریہ];
            اگر (cache_اندراج) {
                --(cache_اندراج->ref_گنتی);
                // delete if the ref count is zero
                اگر (cache_اندراج->ref_گنتی == 0) {
                    delete_Cache_اندراج(cache_اندراج);
                }
            }
            cache_اندراج = static_cast<Currency_Name_Cache_اندراج*>(uprv_malloc(حجم(Currency_Name_Cache_اندراج)));
            اگر (cache_اندراج == خالی_اشاریہ) {
                delete_Currency_نام(currency_نام, کل_currency_name_گنتی);
                delete_Currency_نام(currencySymbols, کل_currency_علامت_گنتی);
                ec = U_حافظہ_تخصیص_ERROR;
                واپس خالی_اشاریہ;
            }
            currCache[current_Cache_اندراج_اشاریہ] = cache_اندراج;
            uprv_strcpy(cache_اندراج->locale, locale);
            cache_اندراج->currency_نام = currency_نام;
            cache_اندراج->کل_Currency_Name_گنتی = کل_currency_name_گنتی;
            cache_اندراج->currencySymbols = currencySymbols;
            cache_اندراج->کل_Currency_علامت_گنتی = کل_currency_علامت_گنتی;
            cache_اندراج->ref_گنتی = 2; // one for cache, one for reference
            current_Cache_اندراج_اشاریہ = (current_Cache_اندراج_اشاریہ + 1) % CURRENCY_NAME_CACHE_عدد;
            ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } ورنہ {
            delete_Currency_نام(currency_نام, کل_currency_name_گنتی);
            delete_Currency_نام(currencySymbols, کل_currency_علامت_گنتی);
            cache_اندراج = currCache[ملا];
            ++(cache_اندراج->ref_گنتی);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    واپس cache_اندراج;
}

ساکن خالی release_Cache_اندراج(Currency_Name_Cache_اندراج* cache_اندراج) {
    umtx_تالا(&gCurrencyCacheMutex);
    --(cache_اندراج->ref_گنتی);
    اگر (cache_اندراج->ref_گنتی == 0) {  // remove
        delete_Cache_اندراج(cache_اندراج);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI خالی
uprv_parseCurrency(ثابت حرف* locale,
                   ثابت icu::UnicodeString& متن,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_مطابقت_Len,
                   char16_t* نتیجہ,
                   UError_رمز& ec) {
    U_NAMESPACE_USE
    اگر (U_FAILURE(ec)) {
        واپس;
    }
    Currency_Name_Cache_اندراج* cache_اندراج = get_Cache_اندراج(locale, ec);
    اگر (U_FAILURE(ec)) {
        واپس;
    }

    int32_t کل_currency_name_گنتی = cache_اندراج->کل_Currency_Name_گنتی;
    CurrencyNameStruct* currency_نام = cache_اندراج->currency_نام;
    int32_t کل_currency_علامت_گنتی = cache_اندراج->کل_Currency_علامت_گنتی;
    CurrencyNameStruct* currencySymbols = cache_اندراج->currencySymbols;

    int32_t آغاز = pos.get_اشاریہ();

    char16_t داخلہ_متن[MAX_CURRENCY_NAME_LEN];
    char16_t بالائی_متن[MAX_CURRENCY_NAME_LEN];
    int32_t متن_Len = MIN(MAX_CURRENCY_NAME_LEN, متن.length() - آغاز);
    متن.extract(آغاز, متن_Len, داخلہ_متن);
    UError_رمز ec1 = U_ZERO_ERROR;
    متن_Len = u_str_مقصد_بالائی(بالائی_متن, MAX_CURRENCY_NAME_LEN, داخلہ_متن, متن_Len, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_مطابقت_Len = 0;

    int32_t max = 0;
    int32_t مطابقت_اشاریہ = -1;
    // case in-sensitive comparison against currency names
    تلاش_Currency_Name(currency_نام, کل_currency_name_گنتی, 
                       بالائی_متن, متن_Len, partial_مطابقت_Len, &max, &مطابقت_اشاریہ);

#اگر_تعریف_شدہ UCURR_DEBUG
    لکھو("search in names, max = %d, matchIndex = %d\n", max, مطابقت_اشاریہ);
#اگر_ختم

    int32_t max_اندر_علامت = 0;
    int32_t مطابقت_اشاریہ_اندر_علامت = -1;
    اگر (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        تلاش_Currency_Name(currencySymbols, کل_currency_علامت_گنتی, 
                           داخلہ_متن, متن_Len,
                           partial_مطابقت_Len,
                           &max_اندر_علامت, &مطابقت_اشاریہ_اندر_علامت);
    }

#اگر_تعریف_شدہ UCURR_DEBUG
    لکھو("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_اندر_علامت, مطابقت_اشاریہ_اندر_علامت);
    اگر(مطابقت_اشاریہ_اندر_علامت != -1) {
      لکھو("== ISO=%s\n", currencySymbols[مطابقت_اشاریہ_اندر_علامت].Iso_رمز);
    }
#اگر_ختم

    اگر (max >= max_اندر_علامت && مطابقت_اشاریہ != -1) {
        u_chars_مقصد_UChars(currency_نام[مطابقت_اشاریہ].Iso_رمز, نتیجہ, 4);
        pos.سیٹ_اشاریہ(آغاز + max);
    } ورنہ اگر (max_اندر_علامت >= max && مطابقت_اشاریہ_اندر_علامت != -1) {
        u_chars_مقصد_UChars(currencySymbols[مطابقت_اشاریہ_اندر_علامت].Iso_رمز, نتیجہ, 4);
        pos.سیٹ_اشاریہ(آغاز + max_اندر_علامت);
    }

    // decrease reference count
    release_Cache_اندراج(cache_اندراج);
}

خالی uprv_currencyLeads(ثابت حرف* locale, icu::Unicode_سیٹ& نتیجہ, UError_رمز& ec) {
    U_NAMESPACE_USE
    اگر (U_FAILURE(ec)) {
        واپس;
    }
    Currency_Name_Cache_اندراج* cache_اندراج = get_Cache_اندراج(locale, ec);
    اگر (U_FAILURE(ec)) {
        واپس;
    }

    برائے (int32_t i=0; i<cache_اندراج->کل_Currency_علامت_گنتی; i++) {
        ثابت CurrencyNameStruct& info = cache_اندراج->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        نتیجہ.جوڑو(cp);
    }

    برائے (int32_t i=0; i<cache_اندراج->کل_Currency_Name_گنتی; i++) {
        ثابت CurrencyNameStruct& info = cache_اندراج->currency_نام[i];
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
U_CAPI خالی
uprv_getStaticCurrencyName(ثابت char16_t* iso, ثابت حرف* loc,
                           icu::UnicodeString& نتیجہ, UError_رمز& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ثابت char16_t* currname = ucurr_getName(iso, loc, UCURR_علامت_NAME,
                                          خالی_اشاریہ /* isChoiceFormat */, &len, &ec);
    اگر (U_SUCCESS(ec)) {
        نتیجہ.سیٹ_مقصد(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ثابت char16_t* currency, UError_رمز* ec) {
    واپس ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_رمز* ec) {
    int32_t fracDigits = 0;
    اگر (U_SUCCESS(*ec)) {
        انتخاب (usage) {
            حالت UCURR_USAGE_STANDARD:
                fracDigits = (ڈھونڈو_Meta_ڈیٹا(currency, *ec))[0];
                توڑو;
            حالت UCURR_USAGE_CASH:
                fracDigits = (ڈھونڈو_Meta_ڈیٹا(currency, *ec))[2];
                توڑو;
            طےشدہ:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    واپس fracDigits;
}

U_CAPI دوہرا U_EXPORT2
ucurr_getRoundingIncrement(ثابت char16_t* currency, UError_رمز* ec) {
    واپس ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI دوہرا U_EXPORT2
ucurr_getRoundingIncrementForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_رمز* ec) {
    دوہرا نتیجہ = 0.0;

    ثابت int32_t *ڈیٹا = ڈھونڈو_Meta_ڈیٹا(currency, *ec);
    اگر (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        انتخاب (usage) {
            حالت UCURR_USAGE_STANDARD:
                fracDigits = ڈیٹا[0];
                increment = ڈیٹا[1];
                توڑو;
            حالت UCURR_USAGE_CASH:
                fracDigits = ڈیٹا[2];
                increment = ڈیٹا[3];
                توڑو;
            طےشدہ:
                *ec = U_UNSUPPORTED_ERROR;
                واپس نتیجہ;
        }

        // If the meta data is invalid, return 0.0
        اگر (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } ورنہ {
            // A rounding value of 0 or 1 indicates no rounding.
            اگر (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                نتیجہ = دوہرا(increment) / POW10[fracDigits];
            }
        }
    }

    واپس نتیجہ;
}

U_CDECL_BEGIN

قسمتعریف ساخت UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
ساکن ثابت ساخت CurrencyList {
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
    { خالی_اشاریہ, 0 } // Leave here to denote the end of the list.
};

#تعریف_کرو UCURR_MATCHES_BITMASK(variable, type_مقصد_مطابقت) \
    ((type_مقصد_مطابقت) == UCURR_ALL || ((variable) & (type_مقصد_مطابقت)) == (type_مقصد_مطابقت))

ساکن int32_t U_CALLCONV
ucurr_گنتی_Currency_List(UEnumeration *enumerator, UError_رمز * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    برائے (int32_t idx = 0; gCurrencyList[idx].currency != خالی_اشاریہ; idx++) {
        اگر (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    واپس count;
}

ساکن ثابت حرف* U_CALLCONV
ucurr_اگلا_Currency_List(UEnumeration *enumerator,
                        int32_t* نتیجہ_لمبائی,
                        UError_رمز * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    جب (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ثابت ساخت CurrencyList *curr_شے = &gCurrencyList[myContext->listIdx++];
        اگر (UCURR_MATCHES_BITMASK(curr_شے->currType, myContext->currType))
        {
            اگر (نتیجہ_لمبائی) {
                *نتیجہ_لمبائی = 3; /* Currency codes are only 3 chars long */
            }
            واپس curr_شے->currency;
        }
    }
    /* We enumerated too far. */
    اگر (نتیجہ_لمبائی) {
        *نتیجہ_لمبائی = 0;
    }
    واپس خالی_اشاریہ;
}

ساکن خالی U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_رمز * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ساکن خالی U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ساکن خالی U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_رمز* حالت_2){
    UError_رمز local_حالت = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_براہ_راست(U_ICUDATA_CURR, CURRENCY_ڈیٹا, &local_حالت);
    Local_UResource_Bundle_اشارہ_گر currencyMapArray(ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_حالت));

    اگر (U_SUCCESS(local_حالت)) {
        // process each entry in currency map
        برائے (int32_t i=0; i<ures_get_سائز(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            Local_UResource_Bundle_اشارہ_گر currencyArray(ures_get_By_اشاریہ(currencyMapArray.getAlias(), i, خالی_اشاریہ, &local_حالت));
            // process each currency
            اگر (U_SUCCESS(local_حالت)) {
                برائے (int32_t j=0; j<ures_get_سائز(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    Local_UResource_Bundle_اشارہ_گر currencyRes(ures_get_By_اشاریہ(currencyArray.getAlias(), j, خالی_اشاریہ, &local_حالت));
                    Iso_رمز_اندراج *اندراج_2 = (Iso_رمز_اندراج*)uprv_malloc(حجم(Iso_رمز_اندراج));
                    اگر (اندراج_2 == خالی_اشاریہ) {
                        *حالت_2 = U_حافظہ_تخصیص_ERROR;
                        واپس;
                    }

                    // get the ISO code
                    int32_t iso_لمبائی = 0;
                    Local_UResource_Bundle_اشارہ_گر idRes(ures_get_By_کلید(currencyRes.getAlias(), "id", خالی_اشاریہ, &local_حالت));
                    اگر (idRes.isNull()) {
                        جاری;
                    }
                    ثابت char16_t *iso_رمز = ures_getString(idRes.getAlias(), &iso_لمبائی, &local_حالت);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    Local_UResource_Bundle_اشارہ_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", خالی_اشاریہ, &local_حالت));

                    اگر (U_SUCCESS(local_حالت)) {
                        int32_t from_لمبائی = 0;
                        ثابت int32_t *fromArray = ures_get_Int_سمتیہ(fromRes.getAlias(), &from_لمبائی, &local_حالت);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate مقصد_Date = U_DATE_MAX;
                    local_حالت = U_ZERO_ERROR;
                    Local_UResource_Bundle_اشارہ_گر مقصد_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", خالی_اشاریہ, &local_حالت));

                    اگر (U_SUCCESS(local_حالت)) {
                        int32_t مقصد_لمبائی = 0;
                        ثابت int32_t *مقصد_Array = ures_get_Int_سمتیہ(مقصد_Res.getAlias(), &مقصد_لمبائی, &local_حالت);
                        int64_t currDate64 = (uint64_t)مقصد_Array[0] << 32;
                        currDate64 |= ((int64_t)مقصد_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        مقصد_Date = (UDate)currDate64;
                    }

                    اندراج_2->iso_رمز = iso_رمز;
                    اندراج_2->from = fromDate;
                    اندراج_2->مقصد = مقصد_Date;

                    local_حالت = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_رمز, اندراج_2, &local_حالت);
                }
            } ورنہ {
                *حالت_2 = local_حالت;
            }
        }
    } ورنہ {
        *حالت_2 = local_حالت;
    }
}

ساکن ثابت UEnumeration gEnumCurrencyList = {
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucurr_closeCurrencyList,
    ucurr_گنتی_Currency_List,
    uenum_unextDefault,
    ucurr_اگلا_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_انتہا


ساکن خالی U_CALLCONV initIsoCodes(UError_رمز &حالت_2) {
    U_ASSERT(gIsoCodes == خالی_اشاریہ);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    Local_UHashtable_اشارہ_گر isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, خالی_اشاریہ, &حالت_2));
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    uhash_سیٹ_قدر_Deleter(isoCodes.getAlias(), delete_Iso_رمز_اندراج);

    ucurr_createCurrencyList(isoCodes.getAlias(), &حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ساکن خالی populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس; }
    برائے (ثابت خودکار& اندراج_2 : unisets::kCurrencyEntries) {
        UnicodeString exemplar(اندراج_2.exemplar);
        ثابت Unicode_سیٹ* سیٹ = unisets::get(اندراج_2.کلید);
        اگر (سیٹ == خالی_اشاریہ) { واپس; }
        Unicode_سیٹ_Iterator it(*سیٹ);
        جب (it.next()) {
            UnicodeString value = it.getString();
            اگر (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                جاری;
            }
            makeEquivalent(exemplar, value, hash, حالت_2);
            اگر (U_FAILURE(حالت_2)) { واپس; }
        }
    }
}

ساکن خالی U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == خالی_اشاریہ);
    UError_رمز حالت_2 = U_ZERO_ERROR;
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *عارضی = نیا icu::Hashtable(حالت_2);
    اگر (عارضی == خالی_اشاریہ) {
        واپس;
    }
    اگر (U_FAILURE(حالت_2)) {
        حذف عارضی;
        واپس;
    }
    عارضی->سیٹ_قدر_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(عارضی, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        حذف عارضی;
        واپس;
    }
    gCurrSymbolsEquiv = عارضی;
}

U_CAPI UBool U_EXPORT2
ucurr_is_دستیاب(ثابت char16_t* iso_رمز, UDate from, UDate مقصد, UError_رمز* e_Error_رمز) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_رمز);
    اگر (U_FAILURE(*e_Error_رمز)) {
        واپس جھوٹ;
    }

    Iso_رمز_اندراج* نتیجہ = (Iso_رمز_اندراج *) uhash_get(gIsoCodes, iso_رمز);
    اگر (نتیجہ == خالی_اشاریہ) {
        واپس جھوٹ;
    } ورنہ اگر (from > مقصد) {
        *e_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    } ورنہ اگر  ((from > نتیجہ->مقصد) || (مقصد < نتیجہ->from)) {
        واپس جھوٹ;
    }
    واپس سچ;
}

ساکن ثابت icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    واپس gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_رمز *p_Error_رمز) {
    UEnumeration *myEnum = خالی_اشاریہ;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(حجم(UEnumeration));
    اگر (myEnum == خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, حجم(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(حجم(UCurrencyContext));
    اگر (myContext == خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        uprv_free(myEnum);
        واپس خالی_اشاریہ;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    واپس myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_گنتی_Currencies(ثابت حرف* locale, 
                 UDate date, 
                 UError_رمز* ec)
{
    int32_t curr_گنتی = 0;

    اگر (ec != خالی_اشاریہ && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_رمز local_حالت = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        اگر (U_FAILURE(*ec))
        {
            واپس 0;
        }

        // Remove variants, which is only needed for registration.
        حرف *idDelim = strchr(id.ڈیٹا(), VAR_DELIM);
        اگر (idDelim)
        {
            id.truncate(idDelim - id.ڈیٹا());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_open_براہ_راست(U_ICUDATA_CURR, CURRENCY_ڈیٹا, &local_حالت);
        UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_حالت);

        // Using the id derived from the local, get the currency data
        Local_UResource_Bundle_اشارہ_گر countryArray(ures_get_By_کلید(rb, id.ڈیٹا(), cm, &local_حالت));

        // process each currency to see which one is valid for the given date
        اگر (U_SUCCESS(local_حالت))
        {
            برائے (int32_t i=0; i<ures_get_سائز(countryArray.getAlias()); i++)
            {
                // get the currency resource
                Local_UResource_Bundle_اشارہ_گر currencyRes(ures_get_By_اشاریہ(countryArray.getAlias(), i, خالی_اشاریہ, &local_حالت));

                // get the from date
                int32_t from_لمبائی = 0;
                Local_UResource_Bundle_اشارہ_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", خالی_اشاریہ, &local_حالت));
                ثابت int32_t *fromArray = ures_get_Int_سمتیہ(fromRes.getAlias(), &from_لمبائی, &local_حالت);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                اگر (ures_get_سائز(currencyRes.getAlias())> 2)
                {
                    int32_t مقصد_لمبائی = 0;
                    Local_UResource_Bundle_اشارہ_گر مقصد_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", خالی_اشاریہ, &local_حالت));
                    ثابت int32_t *مقصد_Array = ures_get_Int_سمتیہ(مقصد_Res.getAlias(), &مقصد_لمبائی, &local_حالت);

                    currDate64 = (int64_t)مقصد_Array[0] << 32;
                    currDate64 |= ((int64_t)مقصد_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate مقصد_Date = (UDate)currDate64;

                    اگر ((fromDate <= date) && (date < مقصد_Date))
                    {
                        curr_گنتی++;
                    }
                }
                ورنہ
                {
                    اگر (fromDate <= date)
                    {
                        curr_گنتی++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        اگر (*ec == U_ZERO_ERROR || local_حالت != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = local_حالت;
        }

        اگر (U_SUCCESS(*ec))
        {
            // no errors
            واپس curr_گنتی;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    واپس 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ثابت حرف* locale, 
                UDate date, 
                int32_t اشاریہ,
                char16_t* buff,
                int32_t buff_استعداد, 
                UError_رمز* ec)
{
    int32_t resLen = 0;
	int32_t curr_اشاریہ = 0;
    ثابت char16_t* s = خالی_اشاریہ;

    اگر (ec != خالی_اشاریہ && U_SUCCESS(*ec))
    {
        // check the arguments passed
        اگر ((buff && buff_استعداد) || !buff_استعداد )
        {
            // local variables
            UError_رمز local_حالت = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            اگر (U_FAILURE(*ec))
            {
                واپس 0;
            }

            // Remove variants, which is only needed for registration.
            حرف *idDelim = strchr(id.ڈیٹا(), VAR_DELIM);
            اگر (idDelim)
            {
                id.truncate(idDelim - id.ڈیٹا());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_open_براہ_راست(U_ICUDATA_CURR, CURRENCY_ڈیٹا, &local_حالت);
            UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_حالت);

            // Using the id derived from the local, get the currency data
            Local_UResource_Bundle_اشارہ_گر countryArray(ures_get_By_کلید(rb, id.ڈیٹا(), cm, &local_حالت));

            // process each currency to see which one is valid for the given date
            منطقی مطابقت_ملا = جھوٹ;
            اگر (U_SUCCESS(local_حالت))
            {
                اگر ((اشاریہ <= 0) || (اشاریہ> ures_get_سائز(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    واپس 0;
                }

                برائے (int32_t i=0; i<ures_get_سائز(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    Local_UResource_Bundle_اشارہ_گر currencyRes(ures_get_By_اشاریہ(countryArray.getAlias(), i, خالی_اشاریہ, &local_حالت));
                    s = ures_get_String_By_کلید(currencyRes.getAlias(), "id", &resLen, &local_حالت);

                    // get the from date
                    int32_t from_لمبائی = 0;
                    Local_UResource_Bundle_اشارہ_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", خالی_اشاریہ, &local_حالت));
                    ثابت int32_t *fromArray = ures_get_Int_سمتیہ(fromRes.getAlias(), &from_لمبائی, &local_حالت);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    اگر (ures_get_سائز(currencyRes.getAlias()) > 2)
                    {
                        int32_t مقصد_لمبائی = 0;
                        Local_UResource_Bundle_اشارہ_گر مقصد_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", خالی_اشاریہ, &local_حالت));
                        ثابت int32_t *مقصد_Array = ures_get_Int_سمتیہ(مقصد_Res.getAlias(), &مقصد_لمبائی, &local_حالت);

                        currDate64 = (int64_t)مقصد_Array[0] << 32;
                        currDate64 |= ((int64_t)مقصد_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate مقصد_Date = (UDate)currDate64;

                        اگر ((fromDate <= date) && (date < مقصد_Date))
                        {
                            curr_اشاریہ++;
                            اگر (curr_اشاریہ == اشاریہ)
                            {
                                مطابقت_ملا = سچ;
                            }
                        }
                    }
                    ورنہ
                    {
                        اگر (fromDate <= date)
                        {
                            curr_اشاریہ++;
                            اگر (curr_اشاریہ == اشاریہ)
                            {
                                مطابقت_ملا = سچ;
                            }
                        }
                    }
                    // check for loop exit
                    اگر (مطابقت_ملا)
                    {
                        توڑو;
                    }

                } // end For loop
            }

            // Check for errors
            اگر (*ec == U_ZERO_ERROR || local_حالت != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = local_حالت;
            }

            اگر (U_SUCCESS(*ec))
            {
                // no errors
                اگر((buff_استعداد> resLen) && مطابقت_ملا)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                ورنہ
                {
                    واپس 0;
                }
            }

            // return null terminated currency string
            واپس u_terminateUChars(buff, buff_استعداد, resLen, ec);
        }
        ورنہ
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_دلیل_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    واپس resLen;
}

ساکن ثابت UEnumeration default_Keyword_اقدار = {
    خالی_اشاریہ,
    خالی_اشاریہ,
    ulist_close_keyword_اقدار_iterator,
    ulist_گنتی_keyword_اقدار,
    uenum_unextDefault,
    ulist_اگلا_keyword_قدر, 
    ulist_reset_keyword_اقدار_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_اقدار_For_Locale(ثابت حرف *کلید, ثابت حرف *locale, UBool commonly_مستعمل, UError_رمز* حالت_2) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_ڈیٹا(locale, سچ, *حالت_2);

    // Read value from supplementalData
    UList *اقدار = ulist_create_خالی_خانے_List(حالت_2);
    UList *other_اقدار = ulist_create_خالی_خانے_List(حالت_2);
    UEnumeration *en = (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    اگر (U_FAILURE(*حالت_2) || en == خالی_اشاریہ) {
        اگر (en == خالی_اشاریہ) {
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
        } ورنہ {
            uprv_free(en);
        }
        ulist_deleteList(اقدار);
        ulist_deleteList(other_اقدار);
        واپس خالی_اشاریہ;
    }
    حافظہ_نقل(en, &default_Keyword_اقدار, حجم(UEnumeration));
    en->context = اقدار;
    
    UResourceBundle* rb = ures_open_براہ_راست(U_ICUDATA_CURR, "supplementalData", حالت_2);
    Local_UResource_Bundle_اشارہ_گر bundle(ures_get_By_کلید(rb, "CurrencyMap", rb, حالت_2));
    ڈھیر_UResource_Bundle bundlekey, regbndl, curbndl, مقصد;
    
    جب (U_SUCCESS(*حالت_2) && ures_has_اگلا(bundle.getAlias())) {
        ures_get_اگلا_Resource(bundle.getAlias(), bundlekey.getAlias(), حالت_2);
        اگر (U_FAILURE(*حالت_2)) {
            توڑو;
        }
        ثابت حرف *region = ures_get_کلید(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        اگر (!isPrefRegion && commonly_مستعمل) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            جاری;
        }
        ures_get_By_کلید(bundle.getAlias(), region, regbndl.getAlias(), حالت_2);
        اگر (U_FAILURE(*حالت_2)) {
            توڑو;
        }
        جب (U_SUCCESS(*حالت_2) && ures_has_اگلا(regbndl.getAlias())) {
            ures_get_اگلا_Resource(regbndl.getAlias(), curbndl.getAlias(), حالت_2);
            اگر (ures_getType(curbndl.getAlias()) != URES_جدول) {
                // Currently, an empty ARRAY is mixed in.
                جاری;
            }
            حرف *curID = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_استعداد);
            اگر (curID == خالی_اشاریہ) {
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                توڑو;
            }
            int32_t curIDLength = ULOC_KEYWORDS_استعداد;

#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_کلید(curbndl.getAlias(), "id", curID, &curIDLength, سچ, حالت_2);
            /* optimize - use the utf-8 string */
#ورنہ
            {
                       ثابت char16_t* defString = ures_get_String_By_کلید(curbndl.getAlias(), "id", &curIDLength, حالت_2);
                       اگر(U_SUCCESS(*حالت_2)) {
			   اگر(curIDLength+1 > ULOC_KEYWORDS_استعداد) {
				*حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
			   } ورنہ {
                           	u_UChars_مقصد_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#اگر_ختم	

            اگر (U_FAILURE(*حالت_2)) {
                توڑو;
            }
            UBool has_مقصد = جھوٹ;
            ures_get_By_کلید(curbndl.getAlias(), "to", مقصد.getAlias(), حالت_2);
            اگر (U_FAILURE(*حالت_2)) {
                // Do nothing here...
                *حالت_2 = U_ZERO_ERROR;
            } ورنہ {
                has_مقصد = سچ;
            }
            اگر (isPrefRegion && !has_مقصد && !ulist_شامل_ہے_String(اقدار, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_جوڑو_شے_انتہا_List(اقدار, curID, سچ, حالت_2);
            } ورنہ اگر (!ulist_شامل_ہے_String(other_اقدار, curID, (int32_t)uprv_strlen(curID)) && !commonly_مستعمل) {
                ulist_جوڑو_شے_انتہا_List(other_اقدار, curID, سچ, حالت_2);
            } ورنہ {
                uprv_free(curID);
            }
        }
        
    }
    اگر (U_SUCCESS(*حالت_2)) {
        اگر (commonly_مستعمل) {
            اگر (ulist_get_List_سائز(اقدار) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_اقدار_For_Locale(کلید, "und", سچ, حالت_2);
            }
        } ورنہ {
            // Consolidate the list
            حرف *value = خالی_اشاریہ;
            ulist_resetList(other_اقدار);
            جب ((value = (حرف *)ulist_get_اگلا(other_اقدار)) != خالی_اشاریہ) {
                اگر (!ulist_شامل_ہے_String(اقدار, value, (int32_t)uprv_strlen(value))) {
                    حرف *tmp_قدر = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_استعداد);
                    اگر (tmp_قدر == خالی_اشاریہ) {
                        *حالت_2 = U_حافظہ_تخصیص_ERROR;
                        توڑو;
                    }
                    uprv_memcpy(tmp_قدر, value, uprv_strlen(value) + 1);
                    ulist_جوڑو_شے_انتہا_List(اقدار, tmp_قدر, سچ, حالت_2);
                    اگر (U_FAILURE(*حالت_2)) {
                        توڑو;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } ورنہ {
        ulist_deleteList(اقدار);
        uprv_free(en);
        اقدار = خالی_اشاریہ;
        en = خالی_اشاریہ;
    }
    ulist_deleteList(other_اقدار);
    واپس en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_رمز(ثابت char16_t* currency) {
    int32_t رمز = 0;
    اگر (currency && u_strlen(currency) == ISO_CURRENCY_رمز_لمبائی) {
        UError_رمز حالت_2 = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_براہ_راست(خالی_اشاریہ, "currencyNumericCodes", &حالت_2);
        Local_UResource_Bundle_اشارہ_گر رمز_Map(ures_get_By_کلید(bundle, "codeMap", bundle, &حالت_2));
        اگر (U_SUCCESS(حالت_2)) {
            حرف alpha_رمز[ISO_CURRENCY_رمز_لمبائی+1];
            my_UChars_مقصد_Chars(alpha_رمز, currency);
            T_CString_مقصد_بالائی_Case(alpha_رمز);
            ures_get_By_کلید(رمز_Map.getAlias(), alpha_رمز, رمز_Map.getAlias(), &حالت_2);
            صحیح tmp_رمز = ures_getInt(رمز_Map.getAlias(), &حالت_2);
            اگر (U_SUCCESS(حالت_2)) {
                رمز = tmp_رمز;
            }
        }
    }
    واپس رمز;
}
#اگر_ختم /* #if !UCONFIG_NO_FORMATTING */

//eof
