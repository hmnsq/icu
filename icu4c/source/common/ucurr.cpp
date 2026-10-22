// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_FORMATTING

#అన్తర్భావ <utility>

#అన్తర్భావ "unicode/ucurr.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/parsepos.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/usetiter.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "static_unicode_sets.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uenumimp.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "hash.h"
#అన్తర్భావ "uinvchar.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "ulist.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "ureslocs.h"
#అన్తర్భావ "ulocimp.h"

ఉపయోగించి నామాకాశ icu;

//#define UCURR_DEBUG_EQUIV 1
#నిర్వచితమైతే UCURR_DEBUG_EQUIV
#అన్తర్భావ "stdio.h"
#అయితే_ముగింపు
//#define UCURR_DEBUG 1
#నిర్వచితమైతే UCURR_DEBUG
#అన్తర్భావ "stdio.h"
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం Iso_సంకేతం_నమోదు {
    స్థిరరాశి char16_t *iso_సంకేతం; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate గమ్యం;
} Iso_సంకేతం_నమోదు;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
స్థిర స్థిరరాశి int32_t చివరి_RESORT_దత్తాంశం[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
స్థిర స్థిరరాశి int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

స్థిర స్థిరరాశి int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#నిర్వచించు ISO_CURRENCY_సంకేతం_నిడివి 3

//------------------------------------------------------------
// Resource tags
//

స్థిర స్థిరరాశి అక్షరం CURRENCY_దత్తాంశం[] = "supplementalData";
// Tag for meta-data, in root.
స్థిర స్థిరరాశి అక్షరం CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
స్థిర స్థిరరాశి అక్షరం CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
స్థిర స్థిరరాశి అక్షరం DEFAULT_META[] = "DEFAULT";

// Variant delimiter
స్థిర స్థిరరాశి అక్షరం VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
స్థిర స్థిరరాశి అక్షరం CURRENCIES[] = "Currencies";
స్థిర స్థిరరాశి అక్షరం CURRENCIES_NARROW[] = "Currencies%narrow";
స్థిర స్థిరరాశి అక్షరం CURRENCIES_FORMAL[] = "Currencies%formal";
స్థిర స్థిరరాశి అక్షరం CURRENCIES_VARIANT[] = "Currencies%variant";
స్థిర స్థిరరాశి అక్షరం CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
స్థిర స్థిరరాశి UHashtable* gIsoCodes = శూన్యసూచిక;
స్థిర icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
స్థిర స్థిరరాశి icu::Hashtable* gCurrSymbolsEquiv = శూన్యసూచిక;
స్థిర icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
తరగతి EquivIterator : బహిరంగ icu::UMemory {
బహిరంగ:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    అంతఃవరుస EquivIterator(స్థిరరాశి icu::Hashtable& hash, స్థిరరాశి icu::UnicodeString& s)
        : _hash(hash) { 
        ప్రారంభం_2 = _current = &s;
    }
    అంతఃవరుస ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    స్థిరరాశి icu::UnicodeString *next();
అంతరంగ:
    స్థిరరాశి icu::Hashtable& _hash;
    స్థిరరాశి icu::UnicodeString* ప్రారంభం_2;
    స్థిరరాశి icu::UnicodeString* _current;
};

స్థిరరాశి icu::UnicodeString *
EquivIterator::next() {
    స్థిరరాశి icu::UnicodeString* తదుపరి_2 = static_cast<స్థిరరాశి icu::UnicodeString*>(_hash.get(*_current));
    ఒకవేళ (తదుపరి_2 == శూన్యసూచిక) {
        U_ASSERT(_current == ప్రారంభం_2);
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (*తదుపరి_2 == *ప్రారంభం_2) {
        తిరిగి శూన్యసూచిక;
    }
    _current = తదుపరి_2;
    తిరిగి తదుపరి_2;
}

U_NAMESPACE_చివర

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
స్థిర శూన్య makeEquivalent(
    స్థిరరాశి icu::UnicodeString &lhs,
    స్థిరరాశి icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (lhs == rhs) {
        // already equivalent
        తిరిగి;
    }
    icu::EquivIterator ఎడమ_Iter(*hash, lhs);
    icu::EquivIterator కుడి_Iter(*hash, rhs);
    స్థిరరాశి icu::UnicodeString *మొదటి_ఎడమ = ఎడమ_Iter.next();
    స్థిరరాశి icu::UnicodeString *మొదటి_కుడి = కుడి_Iter.next();
    స్థిరరాశి icu::UnicodeString *తదుపరి_ఎడమ = మొదటి_ఎడమ;
    స్థిరరాశి icu::UnicodeString *తదుపరి_కుడి = మొదటి_కుడి;
    అయితే (తదుపరి_ఎడమ != శూన్యసూచిక && తదుపరి_కుడి != శూన్యసూచిక) {
        ఒకవేళ (*తదుపరి_ఎడమ == rhs || *తదుపరి_కుడి == lhs) {
            // Already equivalent
            తిరిగి;
        }
        తదుపరి_ఎడమ = ఎడమ_Iter.next();
        తదుపరి_కుడి = కుడి_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_మొదటి_ఎడమ;
    icu::UnicodeString *new_మొదటి_కుడి;
    ఒకవేళ (మొదటి_కుడి == శూన్యసూచిక && మొదటి_ఎడమ == శూన్యసూచిక) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_మొదటి_ఎడమ = కొత్త icu::UnicodeString(rhs);
        new_మొదటి_కుడి = కొత్త icu::UnicodeString(lhs);
    } లేకపోతే ఒకవేళ (మొదటి_కుడి == శూన్యసూచిక) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_మొదటి_ఎడమ = కొత్త icu::UnicodeString(rhs);
        new_మొదటి_కుడి = కొత్త icu::UnicodeString(*మొదటి_ఎడమ);
    } లేకపోతే ఒకవేళ (మొదటి_ఎడమ == శూన్యసూచిక) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_మొదటి_ఎడమ = కొత్త icu::UnicodeString(*మొదటి_కుడి);
        new_మొదటి_కుడి = కొత్త icu::UnicodeString(lhs);
    } లేకపోతే {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_మొదటి_ఎడమ = కొత్త icu::UnicodeString(*మొదటి_కుడి);
        new_మొదటి_కుడి = కొత్త icu::UnicodeString(*మొదటి_ఎడమ);
    }
    ఒకవేళ (new_మొదటి_ఎడమ == శూన్యసూచిక || new_మొదటి_కుడి == శూన్యసూచిక) {
        తొలగించు new_మొదటి_ఎడమ;
        తొలగించు new_మొదటి_కుడి;
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    hash->put(lhs, (శూన్య *) new_మొదటి_ఎడమ, స్థితి_2);
    hash->put(rhs, (శూన్య *) new_మొదటి_కుడి, స్థితి_2);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
స్థిర int32_t లెక్క_Equivalent(స్థిరరాశి icu::Hashtable &hash, స్థిరరాశి icu::UnicodeString &s) {
    int32_t ఫలితం = 0;
    icu::EquivIterator iter(hash, s);
    అయితే (iter.next() != శూన్యసూచిక) {
        ++ఫలితం;
    }
#నిర్వచితమైతే UCURR_DEBUG_EQUIV
 {
   అక్షరం tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   రాయి("CountEquivalent('%s') = %d\n", tmp, ఫలితం);
 }
#అయితే_ముగింపు
    తిరిగి ఫలితం;
}

స్థిర స్థిరరాశి icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//సంకేతం

/**
 * Cleanup callback func
 */
స్థిర UBool U_CALLCONV 
isoCodes_cleanup()
{
    ఒకవేళ (gIsoCodes != శూన్యసూచిక) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = శూన్యసూచిక;
    }
    gIsoCodesInitOnce.reset();
    తిరిగి నిజం;
}

/**
 * Cleanup callback func
 */
స్థిర UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    తొలగించు const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = శూన్యసూచిక;
    gCurrSymbolsEquivInitOnce.reset();
    తిరిగి నిజం;
}

/**
 * Deleter for IsoCodeEntry
 */
స్థిర శూన్య U_CALLCONV
delete_Iso_సంకేతం_నమోదు(శూన్య *obj) {
    Iso_సంకేతం_నమోదు* నమోదు_2 = static_cast<Iso_సంకేతం_నమోదు*>(obj);
    uprv_free(నమోదు_2);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
స్థిర శూన్య U_CALLCONV
deleteUnicode(శూన్య *obj) {
    icu::UnicodeString* నమోదు_2 = static_cast<icu::UnicodeString*>(obj);
    తొలగించు నమోదు_2;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
స్థిర అంతఃవరుస అక్షరం*
my_UChars_గమ్యం_Chars(అక్షరం* ఫలితం_యొక్క_Len4, స్థిరరాశి char16_t* currency) {
    u_UChars_గమ్యం_Chars(currency, ఫలితం_యొక్క_Len4, ISO_CURRENCY_సంకేతం_నిడివి);
    ఫలితం_యొక్క_Len4[ISO_CURRENCY_సంకేతం_నిడివి] = 0;
    తిరిగి ఫలితం_యొక్క_Len4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
స్థిర స్థిరరాశి int32_t*
కనుగొను_Meta_దత్తాంశం(స్థిరరాశి char16_t* currency, UError_సంకేతం& ec) {

    ఒకవేళ (currency == శూన్యసూచిక || *currency == 0) {
        ఒకవేళ (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_వాదం_ERROR;
        }
        తిరిగి చివరి_RESORT_దత్తాంశం;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_దత్తాంశం = ures_open_ప్రత్యక్ష(U_ICUDATA_CURR, CURRENCY_దత్తాంశం, &ec);
    Local_UResource_Bundle_సూచిక currencyMeta(ures_get_By_కీలకం(currency_దత్తాంశం, CURRENCY_META, currency_దత్తాంశం, &ec));

    ఒకవేళ (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        తిరిగి చివరి_RESORT_దత్తాంశం;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    అక్షరం buf[ISO_CURRENCY_సంకేతం_నిడివి+1];
    UError_సంకేతం ec2 = U_ZERO_ERROR; // local error code: soft failure
    Local_UResource_Bundle_సూచిక rb(ures_get_By_కీలకం(currencyMeta.getAlias(), my_UChars_గమ్యం_Chars(buf, currency), శూన్యసూచిక, &ec2));
      ఒకవేళ (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_కీలకం(currencyMeta.getAlias(),DEFAULT_META, శూన్యసూచిక, &ec));
        ఒకవేళ (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            తిరిగి చివరి_RESORT_దత్తాంశం;
        }
    }

    int32_t len;
    స్థిరరాశి int32_t *దత్తాంశం = ures_get_Int_సదిశ(rb.getAlias(), &len, &ec);
    ఒకవేళ (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        ఒకవేళ (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        తిరిగి చివరి_RESORT_దత్తాంశం;
    }

    తిరిగి దత్తాంశం;
}

// -------------------------------------

స్థిర CharString
idForLocale(స్థిరరాశి అక్షరం* locale, UError_సంకేతం* ec)
{
    తిరిగి ulocimp_get_Region_For_Supplemental_దత్తాంశం(locale, అబద్ధం, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
స్థిర UBool U_CALLCONV currency_cleanup();
U_CDECL_చివర

#ఒకవేళ !UCONFIG_NO_SERVICE
నిర్మాణం CReg;

స్థిర UMutex g_CReg_తాళం;
స్థిర CReg* g_CReg_తల = శూన్యసూచిక;

నిర్మాణం CReg : బహిరంగ icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_సంకేతం_నిడివి+1];
    అక్షరం  id[ULOC_FULLNAME_సామర్థ్యం];

    CReg(స్థిరరాశి char16_t* _iso, స్థిరరాశి అక్షరం* _id)
        : next(శూన్యసూచిక)
    {
        uprv_strncpy(id, _id, పరిమాణం(id)-1);
        id[పరిమాణం(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_సంకేతం_నిడివి);
        iso[ISO_CURRENCY_సంకేతం_నిడివి] = 0;
    }

    స్థిర UCurr_Registry_కీలకం reg(స్థిరరాశి char16_t* _iso, స్థిరరాశి అక్షరం* _id, UError_సంకేతం* స్థితి_2)
    {
        ఒకవేళ (స్థితి_2 && U_SUCCESS(*స్థితి_2) && _iso && _id) {
            CReg* n = కొత్త CReg(_iso, _id);
            ఒకవేళ (n) {
                umtx_తాళం(&g_CReg_తాళం);
                ఒకవేళ (!g_CReg_తల) {
                    /* register for the first time */
                    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_తల;
                g_CReg_తల = n;
                umtx_unlock(&g_CReg_తాళం);
                తిరిగి n;
            }
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
        తిరిగి శూన్యసూచిక;
    }

    స్థిర UBool unreg(UCurr_Registry_కీలకం కీలకం) {
        UBool దొరికింది = అబద్ధం;
        umtx_తాళం(&g_CReg_తాళం);

        CReg** p = &g_CReg_తల;
        అయితే (*p) {
            ఒకవేళ (*p == కీలకం) {
                *p = ((CReg*)కీలకం)->next;
                తొలగించు (CReg*)కీలకం;
                దొరికింది = నిజం;
                ఆపు;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_తాళం);
        తిరిగి దొరికింది;
    }

    స్థిర స్థిరరాశి char16_t* get(స్థిరరాశి అక్షరం* id) {
        స్థిరరాశి char16_t* ఫలితం = శూన్యసూచిక;
        umtx_తాళం(&g_CReg_తాళం);
        CReg* p = g_CReg_తల;

        /* register cleanup of the mutex */
        ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        అయితే (p) {
            ఒకవేళ (uprv_strcmp(id, p->id) == 0) {
                ఫలితం = p->iso;
                ఆపు;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_తాళం);
        తిరిగి ఫలితం;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    స్థిర శూన్య cleanup() {
        అయితే (g_CReg_తల) {
            CReg* n = g_CReg_తల;
            g_CReg_తల = g_CReg_తల->next;
            తొలగించు n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_కీలకం U_EXPORT2
ucurr_రిజిస్టరు(స్థిరరాశి char16_t* iso_సంకేతం, స్థిరరాశి అక్షరం* locale, UError_సంకేతం *స్థితి_2)
{
    ఒకవేళ (స్థితి_2 && U_SUCCESS(*స్థితి_2)) {
        CharString id = idForLocale(locale, స్థితి_2);
        తిరిగి CReg::reg(iso_సంకేతం, id.దత్తాంశం(), స్థితి_2);
    }
    తిరిగి శూన్యసూచిక;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_కీలకం కీలకం, UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (స్థితి_2 && U_SUCCESS(*స్థితి_2)) {
        తిరిగి CReg::unreg(కీలకం);
    }
    తిరిగి అబద్ధం;
}
#అయితే_ముగింపు /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
స్థిర UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
స్థిర UBool U_CALLCONV currency_cleanup() {
#ఒకవేళ !UCONFIG_NO_SERVICE
    CReg::cleanup();
#అయితే_ముగింపు
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    తిరిగి నిజం;
}
U_CDECL_చివర

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(స్థిరరాశి అక్షరం* locale,
                char16_t* buff,
                int32_t buff_సామర్థ్యం,
                UError_సంకేతం* ec) {
    ఒకవేళ (U_FAILURE(*ec)) { తిరిగి 0; }
    ఒకవేళ (buff_సామర్థ్యం < 0 || (buff == శూన్యసూచిక && buff_సామర్థ్యం > 0)) {
        *ec = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    UError_సంకేతం local_స్థితి = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_విలువ(locale, "currency", local_స్థితి);
    int32_t resLen = currency.length();

    ఒకవేళ (U_SUCCESS(local_స్థితి) && resLen == 3 && uprv_isInvariantString(currency.దత్తాంశం(), resLen)) {
        ఒకవేళ (resLen < buff_సామర్థ్యం) {
            T_CString_గమ్యం_పై_Case(currency.దత్తాంశం());
            u_chars_గమ్యం_UChars(currency.దత్తాంశం(), buff, resLen);
        }
        తిరిగి u_terminateUChars(buff, buff_సామర్థ్యం, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    ఒకవేళ (U_FAILURE(*ec)) {
        తిరిగి 0;
    }

#ఒకవేళ !UCONFIG_NO_SERVICE
    స్థిరరాశి char16_t* ఫలితం = CReg::get(id.దత్తాంశం());
    ఒకవేళ (ఫలితం) {
        ఒకవేళ(buff_సామర్థ్యం > u_strlen(ఫలితం)) {
            u_strcpy(buff, ఫలితం);
        }
        resLen = u_strlen(ఫలితం);
        తిరిగి u_terminateUChars(buff, buff_సామర్థ్యం, resLen, ec);
    }
#అయితే_ముగింపు
    // Remove variants, which is only needed for registration.
    అక్షరం *idDelim = uprv_strchr(id.దత్తాంశం(), VAR_DELIM);
    ఒకవేళ (idDelim) {
        id.truncate(idDelim - id.దత్తాంశం());
    }

    స్థిరరాశి char16_t* s = శూన్యసూచిక;  // Currency code from data file.
    ఒకవేళ (id.is_ఖాళీలు()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_స్థితి = U_MISSING_RESOURCE_ERROR;
    } లేకపోతే {
        // Look up the CurrencyMap element in the root bundle.
        local_స్థితి = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_ప్రత్యక్ష(U_ICUDATA_CURR, CURRENCY_దత్తాంశం, &local_స్థితి);
        UResourceBundle *cm = ures_get_By_కీలకం(rb, CURRENCY_MAP, rb, &local_స్థితి);
        Local_UResource_Bundle_సూచిక countryArray(ures_get_By_కీలకం(rb, id.దత్తాంశం(), cm, &local_స్థితి));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        ఒకవేళ (U_SUCCESS(local_స్థితి)) {
            int32_t array_నిడివి = ures_get_కొలత(countryArray.getAlias());
            కొరకు (int32_t i = 0; i < array_నిడివి; ++i) {
                Local_UResource_Bundle_సూచిక currencyReq(
                    ures_get_By_సూచీ(countryArray.getAlias(), i, శూన్యసూచిక, &local_స్థితి));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_సంకేతం tender_స్థితి = local_స్థితి;
                స్థిరరాశి char16_t *tender =
                    ures_get_String_By_కీలకం(currencyReq.getAlias(), "tender", శూన్యసూచిక, &tender_స్థితి);
                తార్కిక isTender = U_FAILURE(tender_స్థితి) || u_strcmp(tender, u"false") != 0;
                ఒకవేళ (!isTender && s != శూన్యసూచిక) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    కొనసాగు;
                }
                // Fetch the currency code.
                s = ures_get_String_By_కీలకం(currencyReq.getAlias(), "id", &resLen, &local_స్థితి);
                ఒకవేళ (isTender) {
                    ఆపు;
                }
            }
            ఒకవేళ (U_SUCCESS(local_స్థితి) && s == శూన్యసూచిక) {
                local_స్థితి = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    ఒకవేళ ((U_FAILURE(local_స్థితి)) && strchr(id.దత్తాంశం(), '_') != శూన్యసూచిక) {
        // We don't know about it.  Check to see if we support the variant.
        CharString మాతృ = ulocimp_get_మాతృ(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        తిరిగి ucurr_forLocale(మాతృ.దత్తాంశం(), buff, buff_సామర్థ్యం, ec);
    }
    ఒకవేళ (*ec == U_ZERO_ERROR || local_స్థితి != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_స్థితి;
    }
    ఒకవేళ (U_SUCCESS(*ec)) {
        ఒకవేళ(buff_సామర్థ్యం > resLen) {
            u_strcpy(buff, s);
        }
    }
    తిరిగి u_terminateUChars(buff, buff_సామర్థ్యం, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
స్థిర UBool fallback(CharString& loc) {
    ఒకవేళ (loc.is_ఖాళీలు()) {
        తిరిగి అబద్ధం;
    }
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    ఒకవేళ (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", స్థితి_2);
    } లేకపోతే {
        loc = ulocimp_get_మాతృ(loc.దత్తాంశం(), స్థితి_2);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    తిరిగి నిజం;
}


U_CAPI స్థిరరాశి char16_t* U_EXPORT2
ucurr_getName(స్థిరరాశి char16_t* currency U_LIFETIME_BOUND,
              స్థిరరాశి అక్షరం* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UError_సంకేతం* ec) {

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

    ఒకవేళ (U_FAILURE(*ec)) {
        తిరిగి శూన్యసూచిక;
    }

    int32_t choice = (int32_t) nameStyle;
    ఒకవేళ (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
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
    UError_సంకేతం ec2 = U_ZERO_ERROR;

    ఒకవేళ (locale == శూన్యసూచిక) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ఒకవేళ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    అక్షరం buf[ISO_CURRENCY_సంకేతం_నిడివి+1];
    my_UChars_గమ్యం_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_గమ్యం_పై_Case(buf);
    
    స్థిరరాశి char16_t* s = శూన్యసూచిక;
    ec2 = U_ZERO_ERROR;
    Local_UResource_Bundle_సూచిక rb(ures_open(U_ICUDATA_CURR, loc.దత్తాంశం(), &ec2));

    ఒకవేళ (nameStyle == UCURR_NARROW_చిహ్నం_NAME || nameStyle == UCURR_FORMAL_చిహ్నం_NAME || nameStyle == UCURR_VARIANT_చిహ్నం_NAME) {
        CharString కీలకం;
        ఎంపిక (nameStyle) {
        స్థితి UCURR_NARROW_చిహ్నం_NAME:
            కీలకం.append(CURRENCIES_NARROW, ec2);
            ఆపు;
        స్థితి UCURR_FORMAL_చిహ్నం_NAME:
            కీలకం.append(CURRENCIES_FORMAL, ec2);
            ఆపు;
        స్థితి UCURR_VARIANT_చిహ్నం_NAME:
            కీలకం.append(CURRENCIES_VARIANT, ec2);
            ఆపు;
        అప్రమేయ:
            *ec = U_UNSUPPORTED_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        కీలకం.append("/", ec2);
        కీలకం.append(buf, ec2);
        s = ures_get_String_By_కీలకం_With_Fallback(rb.getAlias(), కీలకం.దత్తాంశం(), len, &ec2);
        ఒకవేళ (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_చిహ్నం_NAME;
        }
    }
    ఒకవేళ (s == శూన్యసూచిక) {
        ures_get_By_కీలకం(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_కీలకం_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_సూచీ(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ఒకవేళ (U_SUCCESS(ec2)) {
        ఒకవేళ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    ఒకవేళ (isChoiceFormat != శూన్యసూచిక) {
        *isChoiceFormat = అబద్ధం;
    }
    ఒకవేళ (U_SUCCESS(ec2)) {
        U_ASSERT(s != శూన్యసూచిక);
        తిరిగి s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    తిరిగి currency;
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2
ucurr_getPluralName(స్థిరరాశి char16_t* currency U_LIFETIME_BOUND,
                    స్థిరరాశి అక్షరం* locale,
                    UBool* isChoiceFormat,
                    స్థిరరాశి అక్షరం* plural_లెక్క,
                    int32_t* len, // fillin
                    UError_సంకేతం* ec) {
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

    ఒకవేళ (U_FAILURE(*ec)) {
        తిరిగి శూన్యసూచిక;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_సంకేతం ec2 = U_ZERO_ERROR;

    ఒకవేళ (locale == శూన్యసూచిక) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ఒకవేళ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    అక్షరం buf[ISO_CURRENCY_సంకేతం_నిడివి+1];
    my_UChars_గమ్యం_Chars(buf, currency);

    స్థిరరాశి char16_t* s = శూన్యసూచిక;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.దత్తాంశం(), &ec2);

    rb = ures_get_By_కీలకం(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    Local_UResource_Bundle_సూచిక curr(ures_get_By_కీలకం_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_కీలకం_With_Fallback(curr.getAlias(), plural_లెక్క, len, &ec2);
    ఒకవేళ (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_కీలకం_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        ఒకవేళ (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            తిరిగి ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ఒకవేళ (U_SUCCESS(ec2)) {
        ఒకవేళ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != శూన్యసూచిక);
        తిరిగి s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    తిరిగి currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#నిర్వచించు NEED_గమ్యం_BE_DELETED 0x1

// TODO: a better way to define this?
#నిర్వచించు MAX_CURRENCY_NAME_LEN 100

రకనిర్వచనం నిర్మాణం {
    స్థిరరాశి అక్షరం* Iso_సంకేతం;  //కీలకం
    char16_t* currencyName;  //విలువ
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#నిర్వచితం_కాకపోతే MIN
#నిర్వచించు MIN(a,b) (((a)<(b)) ? (a) : (b))
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే MAX
#నిర్వచించు MAX(a,b) (((a)<(b)) ? (b) : (a))
#అయితే_ముగింపు


// Comparison function used in quick sort.
స్థిర పూర్ణాంకం U_CALLCONV currencyNameComparator(స్థిరరాశి శూన్య* a, స్థిరరాశి శూన్య* b) {
    స్థిరరాశి CurrencyNameStruct* currName_1 = static_cast<స్థిరరాశి CurrencyNameStruct*>(a);
    స్థిరరాశి CurrencyNameStruct* currName_2 = static_cast<స్థిరరాశి CurrencyNameStruct*>(b);
    కొరకు (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        ఒకవేళ (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            తిరిగి -1;
        }
        ఒకవేళ (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            తిరిగి 1;
        }
    }
    ఒకవేళ (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        తిరిగి -1;
    } లేకపోతే ఒకవేళ (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        తిరిగి 1;
    }
    తిరిగి 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
స్థిర శూన్య
get_Currency_Name_లెక్క(స్థిరరాశి అక్షరం* loc, int32_t* మొత్తం_currency_name_లెక్క, int32_t* మొత్తం_currency_చిహ్నం_లెక్క) {
    U_NAMESPACE_USE
    *మొత్తం_currency_name_లెక్క = 0;
    *మొత్తం_currency_చిహ్నం_లెక్క = 0;
    స్థిరరాశి char16_t* s = శూన్యసూచిక;
    CharString locale;
    {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        locale.append(loc, స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    }
    స్థిరరాశి icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    కొరకు (;;) {
        UError_సంకేతం ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        Local_UResource_Bundle_సూచిక rb(ures_open(U_ICUDATA_CURR, locale.దత్తాంశం(), &ec2));
        Local_UResource_Bundle_సూచిక curr(ures_get_By_కీలకం(rb.getAlias(), CURRENCIES, శూన్యసూచిక, &ec2));
        int32_t n = ures_get_కొలత(curr.getAlias());
        కొరకు (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_సూచిక పేర్లు(ures_get_By_సూచీ(curr.getAlias(), i, శూన్యసూచిక, &ec2));
            int32_t len;
            s = ures_get_String_By_సూచీ(పేర్లు.getAlias(), UCURR_చిహ్నం_NAME, &len, &ec2);
            ++(*మొత్తం_currency_చిహ్నం_లెక్క);  // currency symbol
            ఒకవేళ (currencySymbolsEquiv != శూన్యసూచిక) {
                *మొత్తం_currency_చిహ్నం_లెక్క += లెక్క_Equivalent(*currencySymbolsEquiv, UnicodeString(నిజం, s, len));
            }
            ++(*మొత్తం_currency_చిహ్నం_లెక్క); // iso code
            ++(*మొత్తం_currency_name_లెక్క); // long name
        }

        // currency plurals
        UError_సంకేతం ec3 = U_ZERO_ERROR;
        Local_UResource_Bundle_సూచిక curr_p(ures_get_By_కీలకం(rb.getAlias(), CURRENCYPLURALS, శూన్యసూచిక, &ec3));
        n = ures_get_కొలత(curr_p.getAlias());
        కొరకు (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_సూచిక పేర్లు(ures_get_By_సూచీ(curr_p.getAlias(), i, శూన్యసూచిక, &ec3));
            *మొత్తం_currency_name_లెక్క += ures_get_కొలత(పేర్లు.getAlias());
        }

        ఒకవేళ (!fallback(locale)) {
            ఆపు;
        }
    }
}

స్థిర char16_t*
గమ్యం_పై_Case(స్థిరరాశి char16_t* ఆధారం, int32_t len, స్థిరరాశి అక్షరం* locale) {
    char16_t* dest = శూన్యసూచిక;
    UError_సంకేతం ec = U_ZERO_ERROR;
    int32_t destLen = u_str_గమ్యం_పై(dest, 0, ఆధారం, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(పరిమాణం(char16_t) * MAX(destLen, len)));
    ఒకవేళ (dest == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    u_str_గమ్యం_పై(dest, destLen, ఆధారం, len, locale, &ec);
    ఒకవేళ (U_FAILURE(ec)) {
        u_memcpy(dest, ఆధారం, len);
    } 
    తిరిగి dest;
}


స్థిర శూన్య delete_Currency_పేర్లు(CurrencyNameStruct* currency_పేర్లు, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
స్థిర శూన్య
collect_Currency_పేర్లు(స్థిరరాశి అక్షరం* locale, 
                     CurrencyNameStruct** currency_పేర్లు, 
                     int32_t* మొత్తం_currency_name_లెక్క, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* మొత్తం_currency_చిహ్నం_లెక్క, 
                     UError_సంకేతం& ec) {
    ఒకవేళ (U_FAILURE(ec)) {
        *currency_పేర్లు = *currencySymbols = శూన్యసూచిక;
        *మొత్తం_currency_name_లెక్క = *మొత్తం_currency_చిహ్నం_లెక్క = 0;
        తిరిగి;
    }
    U_NAMESPACE_USE
    స్థిరరాశి icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_సంకేతం ec2 = U_ZERO_ERROR;

    ఒకవేళ (locale == శూన్యసూచిక) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ఒకవేళ (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_వాదం_ERROR;
        *currency_పేర్లు = *currencySymbols = శూన్యసూచిక;
        *మొత్తం_currency_name_లెక్క = *మొత్తం_currency_చిహ్నం_లెక్క = 0;
        తిరిగి;
    }

    // Get maximum currency name count first.
    get_Currency_Name_లెక్క(loc.దత్తాంశం(), మొత్తం_currency_name_లెక్క, మొత్తం_currency_చిహ్నం_లెక్క);

    *currency_పేర్లు = static_cast<CurrencyNameStruct*>(
        uprv_malloc(పరిమాణం(CurrencyNameStruct) * (*మొత్తం_currency_name_లెక్క)));
    ఒకవేళ(*currency_పేర్లు == శూన్యసూచిక) {
        *currencySymbols = శూన్యసూచిక;
        *మొత్తం_currency_name_లెక్క = *మొత్తం_currency_చిహ్నం_లెక్క = 0;
        ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(పరిమాణం(CurrencyNameStruct) * (*మొత్తం_currency_చిహ్నం_లెక్క)));

    ఒకవేళ(*currencySymbols == శూన్యసూచిక) {
        uprv_free(*currency_పేర్లు);
        *currency_పేర్లు = శూన్యసూచిక;
        *మొత్తం_currency_name_లెక్క = *మొత్తం_currency_చిహ్నం_లెక్క = 0;
        ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    స్థిరరాశి char16_t* s = శూన్యసూచిక;  // currency name
    అక్షరం* iso = శూన్యసూచిక;  // currency ISO code

    *మొత్తం_currency_name_లెక్క = 0;
    *మొత్తం_currency_చిహ్నం_లెక్క = 0;

    UError_సంకేతం ec3 = U_ZERO_ERROR;
    UError_సంకేతం ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    Local_UHashtable_సూచిక currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, శూన్యసూచిక, &ec3));
    Local_UHashtable_సూచిక currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, శూన్యసూచిక, &ec4));
    కొరకు (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        Local_UResource_Bundle_సూచిక rb(ures_open(U_ICUDATA_CURR, loc.దత్తాంశం(), &ec2));
        Local_UResource_Bundle_సూచిక curr(ures_get_By_కీలకం(rb.getAlias(), CURRENCIES, శూన్యసూచిక, &ec2));
        int32_t n = ures_get_కొలత(curr.getAlias());
        కొరకు (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_సూచిక పేర్లు(ures_get_By_సూచీ(curr.getAlias(), i, శూన్యసూచిక, &ec2));
            int32_t len;
            s = ures_get_String_By_సూచీ(పేర్లు.getAlias(), UCURR_చిహ్నం_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<అక్షరం*>(ures_get_కీలకం(పేర్లు.getAlias()));
            ఒకవేళ (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != శూన్యసూచిక) {
                కొనసాగు;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*మొత్తం_currency_చిహ్నం_లెక్క)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            ఒకవేళ (currencySymbolsEquiv != శూన్యసూచిక) {
                UnicodeString str(నిజం, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                స్థిరరాశి UnicodeString *చిహ్నం;
                అయితే ((చిహ్నం = iter.next()) != శూన్యసూచిక) {
                    (*currencySymbols)[(*మొత్తం_currency_చిహ్నం_లెక్క)++]
                        = {iso, const_cast<char16_t*>(చిహ్నం->get_బఫరు()), చిహ్నం->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_సూచీ(పేర్లు.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* పై_Name = గమ్యం_పై_Case(s, len, locale);
            ఒకవేళ (పై_Name == శూన్యసూచిక) {
                ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                వెళ్ళు error;
            }
            (*currency_పేర్లు)[(*మొత్తం_currency_name_లెక్క)++] = {iso, పై_Name, len, NEED_గమ్యం_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_సంకేతం = static_cast<char16_t*>(uprv_malloc(పరిమాణం(char16_t) * 3));
            ఒకవేళ (iso_సంకేతం == శూన్యసూచిక) {
                ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                వెళ్ళు error;
            }
            // Must convert iso[] into Unicode
            u_chars_గమ్యం_UChars(iso, iso_సంకేతం, 3);
            (*currencySymbols)[(*మొత్తం_currency_చిహ్నం_లెక్క)++] = {iso, iso_సంకేతం, 3, NEED_గమ్యం_BE_DELETED};
        }

        // currency plurals
        UError_సంకేతం ec5 = U_ZERO_ERROR;
        Local_UResource_Bundle_సూచిక curr_p(ures_get_By_కీలకం(rb.getAlias(), CURRENCYPLURALS, శూన్యసూచిక, &ec5));
        n = ures_get_కొలత(curr_p.getAlias());
        కొరకు (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_సూచిక పేర్లు(ures_get_By_సూచీ(curr_p.getAlias(), i, శూన్యసూచిక, &ec5));
            iso = const_cast<అక్షరం*>(ures_get_కీలకం(పేర్లు.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            ఒకవేళ (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != శూన్యసూచిక) {
                కొనసాగు;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_కొలత(పేర్లు.getAlias());
            int32_t len;
            కొరకు (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_సూచీ(పేర్లు.getAlias(), j, &len, &ec5);
                char16_t* పై_Name = గమ్యం_పై_Case(s, len, locale);
                ఒకవేళ (పై_Name == శూన్యసూచిక) {
                    ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    వెళ్ళు error;
                }
                (*currency_పేర్లు)[(*మొత్తం_currency_name_లెక్క)++] = {iso, పై_Name, len, NEED_గమ్యం_BE_DELETED};
            }
        }

        ఒకవేళ (!fallback(loc)) {
            ఆపు;
        }
    }

    // quick sort the struct
    qsort(*currency_పేర్లు, *మొత్తం_currency_name_లెక్క, 
          పరిమాణం(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *మొత్తం_currency_చిహ్నం_లెక్క, 
          పరిమాణం(CurrencyNameStruct), currencyNameComparator);

#నిర్వచితమైతే UCURR_DEBUG
    రాయి("currency name count: %d\n", *మొత్తం_currency_name_లెక్క);
    కొరకు (int32_t సూచీ = 0; సూచీ < *మొత్తం_currency_name_లెక్క; ++సూచీ) {
        రాయి("index: %d\n", సూచీ);
        రాయి("iso: %s\n", (*currency_పేర్లు)[సూచీ].Iso_సంకేతం);
        అక్షరం curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_పేర్లు)[సూచీ].currencyName, (*currency_పేర్లు)[సూచీ].currencyNameLen);
        రాయి("currencyName: %s\n", curNameBuf);
        రాయి("len: %d\n", (*currency_పేర్లు)[సూచీ].currencyNameLen);
    }
    రాయి("currency symbol count: %d\n", *మొత్తం_currency_చిహ్నం_లెక్క);
    కొరకు (int32_t సూచీ = 0; సూచీ < *మొత్తం_currency_చిహ్నం_లెక్క; ++సూచీ) {
        రాయి("index: %d\n", సూచీ);
        రాయి("iso: %s\n", (*currencySymbols)[సూచీ].Iso_సంకేతం);
        అక్షరం curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[సూచీ].currencyName, (*currencySymbols)[సూచీ].currencyNameLen);
        రాయి("currencySymbol: %s\n", curNameBuf);
        రాయి("len: %d\n", (*currencySymbols)[సూచీ].currencyNameLen);
    }
#అయితే_ముగింపు
    // fail on hashtable errors
    ఒకవేళ (U_FAILURE(ec3)) {
      ec = ec3;
    } లేకపోతే ఒకవేళ (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    ఒకవేళ (U_FAILURE(ec)) {
        delete_Currency_పేర్లు(*currency_పేర్లు, *మొత్తం_currency_name_లెక్క);
        delete_Currency_పేర్లు(*currencySymbols, *మొత్తం_currency_చిహ్నం_లెక్క);
        *currency_పేర్లు = *currencySymbols = శూన్యసూచిక;
        *మొత్తం_currency_name_లెక్క = *మొత్తం_currency_చిహ్నం_లెక్క = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
స్థిర int32_t
ద్విమాన_శోధన(స్థిరరాశి CurrencyNameStruct* currency_పేర్లు, 
             int32_t సూచీ_లోపల_Currency_పేర్లు,
             స్థిరరాశి char16_t కీలకం,
             int32_t* begin, int32_t* చివర) {
#నిర్వచితమైతే UCURR_DEBUG
    రాయి("key = %x\n", కీలకం);
#అయితే_ముగింపు
   int32_t first = *begin;
   int32_t చివరి = *చివర;
   అయితే (first <= చివరి) {
       int32_t mid = (first + చివరి) / 2;  // compute mid point.
       ఒకవేళ (సూచీ_లోపల_Currency_పేర్లు >= currency_పేర్లు[mid].currencyNameLen) {
           first = mid + 1;
       } లేకపోతే {
           ఒకవేళ (కీలకం > currency_పేర్లు[mid].currencyName[సూచీ_లోపల_Currency_పేర్లు]) {
               first = mid + 1;
           }
           లేకపోతే ఒకవేళ (కీలకం < currency_పేర్లు[mid].currencyName[సూచీ_లోపల_Currency_పేర్లు]) {
               చివరి = mid - 1;
           }
           లేకపోతే {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#నిర్వచితమైతే UCURR_DEBUG
                రాయి("mid = %d\n", mid);
#అయితే_ముగింపు
                అయితే (L < R) {
                    int32_t M = (L + R) / 2;
#నిర్వచితమైతే UCURR_DEBUG
                    రాయి("L = %d, R = %d, M = %d\n", L, R, M);
#అయితే_ముగింపు
                    ఒకవేళ (సూచీ_లోపల_Currency_పేర్లు >= currency_పేర్లు[M].currencyNameLen) {
                        L = M + 1;
                    } లేకపోతే {
                        ఒకవేళ (currency_పేర్లు[M].currencyName[సూచీ_లోపల_Currency_పేర్లు] < కీలకం) {
                            L = M + 1;
                        } లేకపోతే {
#నిర్వచితమైతే UCURR_DEBUG
                            U_ASSERT(currency_పేర్లు[M].currencyName[సూచీ_లోపల_Currency_పేర్లు] == కీలకం);
#అయితే_ముగింపు
                            R = M;
                        }
                    }
                }
#నిర్వచితమైతే UCURR_DEBUG
                U_ASSERT(L == R);
#అయితే_ముగింపు
                *begin = L;
#నిర్వచితమైతే UCURR_DEBUG
                రాయి("begin = %d\n", *begin);
                U_ASSERT(currency_పేర్లు[*begin].currencyName[సూచీ_లోపల_Currency_పేర్లు] == కీలకం);
#అయితే_ముగింపు

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *చివర;
                అయితే (L < R) {
                    int32_t M = (L + R) / 2;
#నిర్వచితమైతే UCURR_DEBUG
                    రాయి("L = %d, R = %d, M = %d\n", L, R, M);
#అయితే_ముగింపు
                    ఒకవేళ (currency_పేర్లు[M].currencyNameLen < సూచీ_లోపల_Currency_పేర్లు) {
                        L = M + 1;
                    } లేకపోతే {
                        ఒకవేళ (currency_పేర్లు[M].currencyName[సూచీ_లోపల_Currency_పేర్లు] > కీలకం) {
                            R = M;
                        } లేకపోతే {
#నిర్వచితమైతే UCURR_DEBUG
                            U_ASSERT(currency_పేర్లు[M].currencyName[సూచీ_లోపల_Currency_పేర్లు] == కీలకం);
#అయితే_ముగింపు
                            L = M + 1;
                        }
                    }
                }
#నిర్వచితమైతే UCURR_DEBUG
                U_ASSERT(L == R);
#అయితే_ముగింపు
                ఒకవేళ (currency_పేర్లు[R].currencyName[సూచీ_లోపల_Currency_పేర్లు] > కీలకం) {
                    *చివర = R - 1;
                } లేకపోతే {
                    *చివర = R;
                }
#నిర్వచితమైతే UCURR_DEBUG
                రాయి("end = %d\n", *చివర);
#అయితే_ముగింపు

                // now, found the range. check whether there is exact match
                ఒకవేళ (currency_పేర్లు[*begin].currencyNameLen == సూచీ_లోపల_Currency_పేర్లు + 1) {
                    తిరిగి *begin;  // find range and exact match.
                }
                తిరిగి -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *చివర = -1;
   తిరిగి -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
స్థిర శూన్య
రేఖీయ_శోధన(స్థిరరాశి CurrencyNameStruct* currency_పేర్లు, 
             int32_t begin, int32_t చివర,
             స్థిరరాశి char16_t* పాఠ్యం, int32_t పాఠ్యం_Len,
             int32_t *partial_సరిపోలిక_Len,
             int32_t *max_సరిపోలిక_Len, int32_t* max_సరిపోలిక_సూచీ) {
    int32_t initial_Partial_సరిపోలిక_Len = *partial_సరిపోలిక_Len;
    కొరకు (int32_t సూచీ = begin; సూచీ <= చివర; ++సూచీ) {
        int32_t len = currency_పేర్లు[సూచీ].currencyNameLen;
        ఒకవేళ (len > *max_సరిపోలిక_Len && len <= పాఠ్యం_Len &&
            uprv_memcmp(currency_పేర్లు[సూచీ].currencyName, పాఠ్యం, len * పరిమాణం(char16_t)) == 0) {
            *partial_సరిపోలిక_Len = MAX(*partial_సరిపోలిక_Len, len);
            *max_సరిపోలిక_సూచీ = సూచీ;
            *max_సరిపోలిక_Len = len;
#నిర్వచితమైతే UCURR_DEBUG
            రాయి("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_సరిపోలిక_సూచీ, *max_సరిపోలిక_Len);
#అయితే_ముగింపు
        } లేకపోతే {
            // Check for partial matches.
            కొరకు (int32_t i=initial_Partial_సరిపోలిక_Len; i<MIN(len, పాఠ్యం_Len); i++) {
                ఒకవేళ (currency_పేర్లు[సూచీ].currencyName[i] != పాఠ్యం[i]) {
                    ఆపు;
                }
                *partial_సరిపోలిక_Len = MAX(*partial_సరిపోలిక_Len, i + 1);
            }
        }
    }
}

#నిర్వచించు రేఖీయ_శోధన_పరిమితి 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
స్థిర శూన్య
శోధన_Currency_Name(స్థిరరాశి CurrencyNameStruct* currency_పేర్లు, 
                   int32_t మొత్తం_currency_లెక్క,
                   స్థిరరాశి char16_t* పాఠ్యం, int32_t పాఠ్యం_Len,
                   int32_t *partial_సరిపోలిక_Len,
                   int32_t* max_సరిపోలిక_Len, int32_t* max_సరిపోలిక_సూచీ) {
    *max_సరిపోలిక_సూచీ = -1;
    *max_సరిపోలిక_Len = 0;
    int32_t సరిపోలిక_సూచీ = -1;
    int32_t ద్విమాన_శోధన_Begin = 0;
    int32_t ద్విమాన_శోధన_చివర = మొత్తం_currency_లెక్క - 1;
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
    కొరకు (int32_t సూచీ = 0; సూచీ < పాఠ్యం_Len; ++సూచీ) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        సరిపోలిక_సూచీ = ద్విమాన_శోధన(currency_పేర్లు, సూచీ,
                                  పాఠ్యం[సూచీ],
                                  &ద్విమాన_శోధన_Begin, &ద్విమాన_శోధన_చివర);
        ఒకవేళ (ద్విమాన_శోధన_Begin == -1) { // did not find the range
            ఆపు;
        }
        *partial_సరిపోలిక_Len = MAX(*partial_సరిపోలిక_Len, సూచీ + 1);
        ఒకవేళ (సరిపోలిక_సూచీ != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_సరిపోలిక_Len = సూచీ + 1;
            *max_సరిపోలిక_సూచీ = సరిపోలిక_సూచీ;
        }
        ఒకవేళ (ద్విమాన_శోధన_చివర - ద్విమాన_శోధన_Begin < రేఖీయ_శోధన_పరిమితి) {
            // linear search if within threshold.
            రేఖీయ_శోధన(currency_పేర్లు, ద్విమాన_శోధన_Begin, ద్విమాన_శోధన_చివర,
                         పాఠ్యం, పాఠ్యం_Len,
                         partial_సరిపోలిక_Len,
                         max_సరిపోలిక_Len, max_సరిపోలిక_సూచీ);
            ఆపు;
        }
    }
}

//========================= currency name cache =====================
రకనిర్వచనం నిర్మాణం {
    అక్షరం locale[ULOC_FULLNAME_సామర్థ్యం];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_పేర్లు;  //విలువ
    int32_t మొత్తం_Currency_Name_లెక్క;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //విలువ
    int32_t మొత్తం_Currency_చిహ్నం_లెక్క;  //లెక్క
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_లెక్క;
} Currency_Name_Cache_నమోదు;


#నిర్వచించు CURRENCY_NAME_CACHE_అంకె 10

// Reserve 10 cache entries.
స్థిర Currency_Name_Cache_నమోదు* currCache[CURRENCY_NAME_CACHE_అంకె] = {శూన్యసూచిక};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
స్థిర int8_t current_Cache_నమోదు_సూచీ = 0;

స్థిర UMutex gCurrencyCacheMutex;

// Cache deletion
స్థిర శూన్య
delete_Currency_పేర్లు(CurrencyNameStruct* currency_పేర్లు, int32_t count) {
    కొరకు (int32_t సూచీ = 0; సూచీ < count; ++సూచీ) {
        ఒకవేళ ( (currency_పేర్లు[సూచీ].flag & NEED_గమ్యం_BE_DELETED) ) {
            uprv_free(currency_పేర్లు[సూచీ].currencyName);
        }
    }
    uprv_free(currency_పేర్లు);
}


స్థిర శూన్య
delete_Cache_నమోదు(Currency_Name_Cache_నమోదు* నమోదు_2) {
    delete_Currency_పేర్లు(నమోదు_2->currency_పేర్లు, నమోదు_2->మొత్తం_Currency_Name_లెక్క);
    delete_Currency_పేర్లు(నమోదు_2->currencySymbols, నమోదు_2->మొత్తం_Currency_చిహ్నం_లెక్క);
    uprv_free(నమోదు_2);
}


// Cache clean up
స్థిర UBool U_CALLCONV
currency_cache_cleanup() {
    కొరకు (int32_t i = 0; i < CURRENCY_NAME_CACHE_అంకె; ++i) {
        ఒకవేళ (currCache[i]) {
            delete_Cache_నమోదు(currCache[i]);
            currCache[i] = శూన్యసూచిక;
        }
    }
    తిరిగి నిజం;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
స్థిర Currency_Name_Cache_నమోదు*
get_Cache_నమోదు(స్థిరరాశి అక్షరం* locale, UError_సంకేతం& ec) {

    int32_t మొత్తం_currency_name_లెక్క = 0;
    CurrencyNameStruct* currency_పేర్లు = శూన్యసూచిక;
    int32_t మొత్తం_currency_చిహ్నం_లెక్క = 0;
    CurrencyNameStruct* currencySymbols = శూన్యసూచిక;
    Currency_Name_Cache_నమోదు* cache_నమోదు = శూన్యసూచిక;

    umtx_తాళం(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t దొరికింది = -1;
    కొరకు (int8_t i = 0; i < CURRENCY_NAME_CACHE_అంకె; ++i) {
        ఒకవేళ (currCache[i]!= శూన్యసూచిక &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            దొరికింది = i;
            ఆపు;
        }
    }
    ఒకవేళ (దొరికింది != -1) {
        cache_నమోదు = currCache[దొరికింది];
        ++(cache_నమోదు->ref_లెక్క);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    ఒకవేళ (దొరికింది == -1) {
        collect_Currency_పేర్లు(locale, &currency_పేర్లు, &మొత్తం_currency_name_లెక్క, &currencySymbols, &మొత్తం_currency_చిహ్నం_లెక్క, ec);
        ఒకవేళ (U_FAILURE(ec)) {
            తిరిగి శూన్యసూచిక;
        }
        umtx_తాళం(&gCurrencyCacheMutex);
        // check again.
        కొరకు (int8_t i = 0; i < CURRENCY_NAME_CACHE_అంకె; ++i) {
            ఒకవేళ (currCache[i]!= శూన్యసూచిక &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                దొరికింది = i;
                ఆపు;
            }
        }
        ఒకవేళ (దొరికింది == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_నమోదు = currCache[current_Cache_నమోదు_సూచీ];
            ఒకవేళ (cache_నమోదు) {
                --(cache_నమోదు->ref_లెక్క);
                // delete if the ref count is zero
                ఒకవేళ (cache_నమోదు->ref_లెక్క == 0) {
                    delete_Cache_నమోదు(cache_నమోదు);
                }
            }
            cache_నమోదు = static_cast<Currency_Name_Cache_నమోదు*>(uprv_malloc(పరిమాణం(Currency_Name_Cache_నమోదు)));
            ఒకవేళ (cache_నమోదు == శూన్యసూచిక) {
                delete_Currency_పేర్లు(currency_పేర్లు, మొత్తం_currency_name_లెక్క);
                delete_Currency_పేర్లు(currencySymbols, మొత్తం_currency_చిహ్నం_లెక్క);
                ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి శూన్యసూచిక;
            }
            currCache[current_Cache_నమోదు_సూచీ] = cache_నమోదు;
            uprv_strcpy(cache_నమోదు->locale, locale);
            cache_నమోదు->currency_పేర్లు = currency_పేర్లు;
            cache_నమోదు->మొత్తం_Currency_Name_లెక్క = మొత్తం_currency_name_లెక్క;
            cache_నమోదు->currencySymbols = currencySymbols;
            cache_నమోదు->మొత్తం_Currency_చిహ్నం_లెక్క = మొత్తం_currency_చిహ్నం_లెక్క;
            cache_నమోదు->ref_లెక్క = 2; // one for cache, one for reference
            current_Cache_నమోదు_సూచీ = (current_Cache_నమోదు_సూచీ + 1) % CURRENCY_NAME_CACHE_అంకె;
            ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } లేకపోతే {
            delete_Currency_పేర్లు(currency_పేర్లు, మొత్తం_currency_name_లెక్క);
            delete_Currency_పేర్లు(currencySymbols, మొత్తం_currency_చిహ్నం_లెక్క);
            cache_నమోదు = currCache[దొరికింది];
            ++(cache_నమోదు->ref_లెక్క);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    తిరిగి cache_నమోదు;
}

స్థిర శూన్య release_Cache_నమోదు(Currency_Name_Cache_నమోదు* cache_నమోదు) {
    umtx_తాళం(&gCurrencyCacheMutex);
    --(cache_నమోదు->ref_లెక్క);
    ఒకవేళ (cache_నమోదు->ref_లెక్క == 0) {  // remove
        delete_Cache_నమోదు(cache_నమోదు);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI శూన్య
uprv_parseCurrency(స్థిరరాశి అక్షరం* locale,
                   స్థిరరాశి icu::UnicodeString& పాఠ్యం,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_సరిపోలిక_Len,
                   char16_t* ఫలితం,
                   UError_సంకేతం& ec) {
    U_NAMESPACE_USE
    ఒకవేళ (U_FAILURE(ec)) {
        తిరిగి;
    }
    Currency_Name_Cache_నమోదు* cache_నమోదు = get_Cache_నమోదు(locale, ec);
    ఒకవేళ (U_FAILURE(ec)) {
        తిరిగి;
    }

    int32_t మొత్తం_currency_name_లెక్క = cache_నమోదు->మొత్తం_Currency_Name_లెక్క;
    CurrencyNameStruct* currency_పేర్లు = cache_నమోదు->currency_పేర్లు;
    int32_t మొత్తం_currency_చిహ్నం_లెక్క = cache_నమోదు->మొత్తం_Currency_చిహ్నం_లెక్క;
    CurrencyNameStruct* currencySymbols = cache_నమోదు->currencySymbols;

    int32_t ప్రారంభం = pos.get_సూచీ();

    char16_t ఆదానం_పాఠ్యం[MAX_CURRENCY_NAME_LEN];
    char16_t పై_పాఠ్యం[MAX_CURRENCY_NAME_LEN];
    int32_t పాఠ్యం_Len = MIN(MAX_CURRENCY_NAME_LEN, పాఠ్యం.length() - ప్రారంభం);
    పాఠ్యం.extract(ప్రారంభం, పాఠ్యం_Len, ఆదానం_పాఠ్యం);
    UError_సంకేతం ec1 = U_ZERO_ERROR;
    పాఠ్యం_Len = u_str_గమ్యం_పై(పై_పాఠ్యం, MAX_CURRENCY_NAME_LEN, ఆదానం_పాఠ్యం, పాఠ్యం_Len, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_సరిపోలిక_Len = 0;

    int32_t max = 0;
    int32_t సరిపోలిక_సూచీ = -1;
    // case in-sensitive comparison against currency names
    శోధన_Currency_Name(currency_పేర్లు, మొత్తం_currency_name_లెక్క, 
                       పై_పాఠ్యం, పాఠ్యం_Len, partial_సరిపోలిక_Len, &max, &సరిపోలిక_సూచీ);

#నిర్వచితమైతే UCURR_DEBUG
    రాయి("search in names, max = %d, matchIndex = %d\n", max, సరిపోలిక_సూచీ);
#అయితే_ముగింపు

    int32_t max_లోపల_చిహ్నం = 0;
    int32_t సరిపోలిక_సూచీ_లోపల_చిహ్నం = -1;
    ఒకవేళ (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        శోధన_Currency_Name(currencySymbols, మొత్తం_currency_చిహ్నం_లెక్క, 
                           ఆదానం_పాఠ్యం, పాఠ్యం_Len,
                           partial_సరిపోలిక_Len,
                           &max_లోపల_చిహ్నం, &సరిపోలిక_సూచీ_లోపల_చిహ్నం);
    }

#నిర్వచితమైతే UCURR_DEBUG
    రాయి("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_లోపల_చిహ్నం, సరిపోలిక_సూచీ_లోపల_చిహ్నం);
    ఒకవేళ(సరిపోలిక_సూచీ_లోపల_చిహ్నం != -1) {
      రాయి("== ISO=%s\n", currencySymbols[సరిపోలిక_సూచీ_లోపల_చిహ్నం].Iso_సంకేతం);
    }
#అయితే_ముగింపు

    ఒకవేళ (max >= max_లోపల_చిహ్నం && సరిపోలిక_సూచీ != -1) {
        u_chars_గమ్యం_UChars(currency_పేర్లు[సరిపోలిక_సూచీ].Iso_సంకేతం, ఫలితం, 4);
        pos.సమితి_సూచీ(ప్రారంభం + max);
    } లేకపోతే ఒకవేళ (max_లోపల_చిహ్నం >= max && సరిపోలిక_సూచీ_లోపల_చిహ్నం != -1) {
        u_chars_గమ్యం_UChars(currencySymbols[సరిపోలిక_సూచీ_లోపల_చిహ్నం].Iso_సంకేతం, ఫలితం, 4);
        pos.సమితి_సూచీ(ప్రారంభం + max_లోపల_చిహ్నం);
    }

    // decrease reference count
    release_Cache_నమోదు(cache_నమోదు);
}

శూన్య uprv_currencyLeads(స్థిరరాశి అక్షరం* locale, icu::Unicode_సమితి& ఫలితం, UError_సంకేతం& ec) {
    U_NAMESPACE_USE
    ఒకవేళ (U_FAILURE(ec)) {
        తిరిగి;
    }
    Currency_Name_Cache_నమోదు* cache_నమోదు = get_Cache_నమోదు(locale, ec);
    ఒకవేళ (U_FAILURE(ec)) {
        తిరిగి;
    }

    కొరకు (int32_t i=0; i<cache_నమోదు->మొత్తం_Currency_చిహ్నం_లెక్క; i++) {
        స్థిరరాశి CurrencyNameStruct& info = cache_నమోదు->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        ఫలితం.కూడు(cp);
    }

    కొరకు (int32_t i=0; i<cache_నమోదు->మొత్తం_Currency_Name_లెక్క; i++) {
        స్థిరరాశి CurrencyNameStruct& info = cache_నమోదు->currency_పేర్లు[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        ఫలితం.కూడు(cp);
    }

    // decrease reference count
    release_Cache_నమోదు(cache_నమోదు);
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
U_CAPI శూన్య
uprv_getStaticCurrencyName(స్థిరరాశి char16_t* iso, స్థిరరాశి అక్షరం* loc,
                           icu::UnicodeString& ఫలితం, UError_సంకేతం& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    స్థిరరాశి char16_t* currname = ucurr_getName(iso, loc, UCURR_చిహ్నం_NAME,
                                          శూన్యసూచిక /* isChoiceFormat */, &len, &ec);
    ఒకవేళ (U_SUCCESS(ec)) {
        ఫలితం.సమితి_గమ్యం(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(స్థిరరాశి char16_t* currency, UError_సంకేతం* ec) {
    తిరిగి ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(స్థిరరాశి char16_t* currency, స్థిరరాశి UCurrencyUsage usage, UError_సంకేతం* ec) {
    int32_t fracDigits = 0;
    ఒకవేళ (U_SUCCESS(*ec)) {
        ఎంపిక (usage) {
            స్థితి UCURR_USAGE_STANDARD:
                fracDigits = (కనుగొను_Meta_దత్తాంశం(currency, *ec))[0];
                ఆపు;
            స్థితి UCURR_USAGE_CASH:
                fracDigits = (కనుగొను_Meta_దత్తాంశం(currency, *ec))[2];
                ఆపు;
            అప్రమేయ:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    తిరిగి fracDigits;
}

U_CAPI ద్విగుణ U_EXPORT2
ucurr_getRoundingIncrement(స్థిరరాశి char16_t* currency, UError_సంకేతం* ec) {
    తిరిగి ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI ద్విగుణ U_EXPORT2
ucurr_getRoundingIncrementForUsage(స్థిరరాశి char16_t* currency, స్థిరరాశి UCurrencyUsage usage, UError_సంకేతం* ec) {
    ద్విగుణ ఫలితం = 0.0;

    స్థిరరాశి int32_t *దత్తాంశం = కనుగొను_Meta_దత్తాంశం(currency, *ec);
    ఒకవేళ (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        ఎంపిక (usage) {
            స్థితి UCURR_USAGE_STANDARD:
                fracDigits = దత్తాంశం[0];
                increment = దత్తాంశం[1];
                ఆపు;
            స్థితి UCURR_USAGE_CASH:
                fracDigits = దత్తాంశం[2];
                increment = దత్తాంశం[3];
                ఆపు;
            అప్రమేయ:
                *ec = U_UNSUPPORTED_ERROR;
                తిరిగి ఫలితం;
        }

        // If the meta data is invalid, return 0.0
        ఒకవేళ (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } లేకపోతే {
            // A rounding value of 0 or 1 indicates no rounding.
            ఒకవేళ (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                ఫలితం = ద్విగుణ(increment) / POW10[fracDigits];
            }
        }
    }

    తిరిగి ఫలితం;
}

U_CDECL_BEGIN

రకనిర్వచనం నిర్మాణం UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
స్థిర స్థిరరాశి నిర్మాణం CurrencyList {
    స్థిరరాశి అక్షరం *currency;
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
    { శూన్యసూచిక, 0 } // Leave here to denote the end of the list.
};

#నిర్వచించు UCURR_MATCHES_BITMASK(variable, type_గమ్యం_సరిపోలిక) \
    ((type_గమ్యం_సరిపోలిక) == UCURR_ALL || ((variable) & (type_గమ్యం_సరిపోలిక)) == (type_గమ్యం_సరిపోలిక))

స్థిర int32_t U_CALLCONV
ucurr_లెక్క_Currency_List(UEnumeration *enumerator, UError_సంకేతం * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    కొరకు (int32_t idx = 0; gCurrencyList[idx].currency != శూన్యసూచిక; idx++) {
        ఒకవేళ (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    తిరిగి count;
}

స్థిర స్థిరరాశి అక్షరం* U_CALLCONV
ucurr_తదుపరి_Currency_List(UEnumeration *enumerator,
                        int32_t* ఫలితం_నిడివి,
                        UError_సంకేతం * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    అయితే (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        స్థిరరాశి నిర్మాణం CurrencyList *curr_అంశం = &gCurrencyList[myContext->listIdx++];
        ఒకవేళ (UCURR_MATCHES_BITMASK(curr_అంశం->currType, myContext->currType))
        {
            ఒకవేళ (ఫలితం_నిడివి) {
                *ఫలితం_నిడివి = 3; /* Currency codes are only 3 chars long */
            }
            తిరిగి curr_అంశం->currency;
        }
    }
    /* We enumerated too far. */
    ఒకవేళ (ఫలితం_నిడివి) {
        *ఫలితం_నిడివి = 0;
    }
    తిరిగి శూన్యసూచిక;
}

స్థిర శూన్య U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_సంకేతం * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

స్థిర శూన్య U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

స్థిర శూన్య U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_సంకేతం* స్థితి_2){
    UError_సంకేతం local_స్థితి = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_ప్రత్యక్ష(U_ICUDATA_CURR, CURRENCY_దత్తాంశం, &local_స్థితి);
    Local_UResource_Bundle_సూచిక currencyMapArray(ures_get_By_కీలకం(rb, CURRENCY_MAP, rb, &local_స్థితి));

    ఒకవేళ (U_SUCCESS(local_స్థితి)) {
        // process each entry in currency map
        కొరకు (int32_t i=0; i<ures_get_కొలత(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            Local_UResource_Bundle_సూచిక currencyArray(ures_get_By_సూచీ(currencyMapArray.getAlias(), i, శూన్యసూచిక, &local_స్థితి));
            // process each currency
            ఒకవేళ (U_SUCCESS(local_స్థితి)) {
                కొరకు (int32_t j=0; j<ures_get_కొలత(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    Local_UResource_Bundle_సూచిక currencyRes(ures_get_By_సూచీ(currencyArray.getAlias(), j, శూన్యసూచిక, &local_స్థితి));
                    Iso_సంకేతం_నమోదు *నమోదు_2 = (Iso_సంకేతం_నమోదు*)uprv_malloc(పరిమాణం(Iso_సంకేతం_నమోదు));
                    ఒకవేళ (నమోదు_2 == శూన్యసూచిక) {
                        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        తిరిగి;
                    }

                    // get the ISO code
                    int32_t iso_నిడివి = 0;
                    Local_UResource_Bundle_సూచిక idRes(ures_get_By_కీలకం(currencyRes.getAlias(), "id", శూన్యసూచిక, &local_స్థితి));
                    ఒకవేళ (idRes.isNull()) {
                        కొనసాగు;
                    }
                    స్థిరరాశి char16_t *iso_సంకేతం = ures_getString(idRes.getAlias(), &iso_నిడివి, &local_స్థితి);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    Local_UResource_Bundle_సూచిక fromRes(ures_get_By_కీలకం(currencyRes.getAlias(), "from", శూన్యసూచిక, &local_స్థితి));

                    ఒకవేళ (U_SUCCESS(local_స్థితి)) {
                        int32_t from_నిడివి = 0;
                        స్థిరరాశి int32_t *fromArray = ures_get_Int_సదిశ(fromRes.getAlias(), &from_నిడివి, &local_స్థితి);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate గమ్యం_Date = U_DATE_MAX;
                    local_స్థితి = U_ZERO_ERROR;
                    Local_UResource_Bundle_సూచిక గమ్యం_Res(ures_get_By_కీలకం(currencyRes.getAlias(), "to", శూన్యసూచిక, &local_స్థితి));

                    ఒకవేళ (U_SUCCESS(local_స్థితి)) {
                        int32_t గమ్యం_నిడివి = 0;
                        స్థిరరాశి int32_t *గమ్యం_Array = ures_get_Int_సదిశ(గమ్యం_Res.getAlias(), &గమ్యం_నిడివి, &local_స్థితి);
                        int64_t currDate64 = (uint64_t)గమ్యం_Array[0] << 32;
                        currDate64 |= ((int64_t)గమ్యం_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        గమ్యం_Date = (UDate)currDate64;
                    }

                    నమోదు_2->iso_సంకేతం = iso_సంకేతం;
                    నమోదు_2->from = fromDate;
                    నమోదు_2->గమ్యం = గమ్యం_Date;

                    local_స్థితి = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_సంకేతం, నమోదు_2, &local_స్థితి);
                }
            } లేకపోతే {
                *స్థితి_2 = local_స్థితి;
            }
        }
    } లేకపోతే {
        *స్థితి_2 = local_స్థితి;
    }
}

స్థిర స్థిరరాశి UEnumeration gEnumCurrencyList = {
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucurr_closeCurrencyList,
    ucurr_లెక్క_Currency_List,
    uenum_unextDefault,
    ucurr_తదుపరి_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_చివర


స్థిర శూన్య U_CALLCONV initIsoCodes(UError_సంకేతం &స్థితి_2) {
    U_ASSERT(gIsoCodes == శూన్యసూచిక);
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    Local_UHashtable_సూచిక isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, శూన్యసూచిక, &స్థితి_2));
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    uhash_సమితి_విలువ_Deleter(isoCodes.getAlias(), delete_Iso_సంకేతం_నమోదు);

    ucurr_createCurrencyList(isoCodes.getAlias(), &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

స్థిర శూన్య populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    కొరకు (స్థిరరాశి స్వయంచాలక& నమోదు_2 : unisets::kCurrencyEntries) {
        UnicodeString exemplar(నమోదు_2.exemplar);
        స్థిరరాశి Unicode_సమితి* సమితి = unisets::get(నమోదు_2.కీలకం);
        ఒకవేళ (సమితి == శూన్యసూచిక) { తిరిగి; }
        Unicode_సమితి_Iterator it(*సమితి);
        అయితే (it.next()) {
            UnicodeString value = it.getString();
            ఒకవేళ (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                కొనసాగు;
            }
            makeEquivalent(exemplar, value, hash, స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        }
    }
}

స్థిర శూన్య U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == శూన్యసూచిక);
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *తాత్కాలిక = కొత్త icu::Hashtable(స్థితి_2);
    ఒకవేళ (తాత్కాలిక == శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు తాత్కాలిక;
        తిరిగి;
    }
    తాత్కాలిక->సమితి_విలువ_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(తాత్కాలిక, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు తాత్కాలిక;
        తిరిగి;
    }
    gCurrSymbolsEquiv = తాత్కాలిక;
}

U_CAPI UBool U_EXPORT2
ucurr_is_అందుబాటు(స్థిరరాశి char16_t* iso_సంకేతం, UDate from, UDate గమ్యం, UError_సంకేతం* e_Error_సంకేతం) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_సంకేతం);
    ఒకవేళ (U_FAILURE(*e_Error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }

    Iso_సంకేతం_నమోదు* ఫలితం = (Iso_సంకేతం_నమోదు *) uhash_get(gIsoCodes, iso_సంకేతం);
    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    } లేకపోతే ఒకవేళ (from > గమ్యం) {
        *e_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    } లేకపోతే ఒకవేళ  ((from > ఫలితం->గమ్యం) || (గమ్యం < ఫలితం->from)) {
        తిరిగి అబద్ధం;
    }
    తిరిగి నిజం;
}

స్థిర స్థిరరాశి icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    తిరిగి gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_సంకేతం *p_Error_సంకేతం) {
    UEnumeration *myEnum = శూన్యసూచిక;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(పరిమాణం(UEnumeration));
    ఒకవేళ (myEnum == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, పరిమాణం(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(పరిమాణం(UCurrencyContext));
    ఒకవేళ (myContext == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        uprv_free(myEnum);
        తిరిగి శూన్యసూచిక;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    తిరిగి myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_లెక్క_Currencies(స్థిరరాశి అక్షరం* locale, 
                 UDate date, 
                 UError_సంకేతం* ec)
{
    int32_t curr_లెక్క = 0;

    ఒకవేళ (ec != శూన్యసూచిక && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_సంకేతం local_స్థితి = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        ఒకవేళ (U_FAILURE(*ec))
        {
            తిరిగి 0;
        }

        // Remove variants, which is only needed for registration.
        అక్షరం *idDelim = strchr(id.దత్తాంశం(), VAR_DELIM);
        ఒకవేళ (idDelim)
        {
            id.truncate(idDelim - id.దత్తాంశం());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_open_ప్రత్యక్ష(U_ICUDATA_CURR, CURRENCY_దత్తాంశం, &local_స్థితి);
        UResourceBundle *cm = ures_get_By_కీలకం(rb, CURRENCY_MAP, rb, &local_స్థితి);

        // Using the id derived from the local, get the currency data
        Local_UResource_Bundle_సూచిక countryArray(ures_get_By_కీలకం(rb, id.దత్తాంశం(), cm, &local_స్థితి));

        // process each currency to see which one is valid for the given date
        ఒకవేళ (U_SUCCESS(local_స్థితి))
        {
            కొరకు (int32_t i=0; i<ures_get_కొలత(countryArray.getAlias()); i++)
            {
                // get the currency resource
                Local_UResource_Bundle_సూచిక currencyRes(ures_get_By_సూచీ(countryArray.getAlias(), i, శూన్యసూచిక, &local_స్థితి));

                // get the from date
                int32_t from_నిడివి = 0;
                Local_UResource_Bundle_సూచిక fromRes(ures_get_By_కీలకం(currencyRes.getAlias(), "from", శూన్యసూచిక, &local_స్థితి));
                స్థిరరాశి int32_t *fromArray = ures_get_Int_సదిశ(fromRes.getAlias(), &from_నిడివి, &local_స్థితి);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                ఒకవేళ (ures_get_కొలత(currencyRes.getAlias())> 2)
                {
                    int32_t గమ్యం_నిడివి = 0;
                    Local_UResource_Bundle_సూచిక గమ్యం_Res(ures_get_By_కీలకం(currencyRes.getAlias(), "to", శూన్యసూచిక, &local_స్థితి));
                    స్థిరరాశి int32_t *గమ్యం_Array = ures_get_Int_సదిశ(గమ్యం_Res.getAlias(), &గమ్యం_నిడివి, &local_స్థితి);

                    currDate64 = (int64_t)గమ్యం_Array[0] << 32;
                    currDate64 |= ((int64_t)గమ్యం_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate గమ్యం_Date = (UDate)currDate64;

                    ఒకవేళ ((fromDate <= date) && (date < గమ్యం_Date))
                    {
                        curr_లెక్క++;
                    }
                }
                లేకపోతే
                {
                    ఒకవేళ (fromDate <= date)
                    {
                        curr_లెక్క++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        ఒకవేళ (*ec == U_ZERO_ERROR || local_స్థితి != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = local_స్థితి;
        }

        ఒకవేళ (U_SUCCESS(*ec))
        {
            // no errors
            తిరిగి curr_లెక్క;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    తిరిగి 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(స్థిరరాశి అక్షరం* locale, 
                UDate date, 
                int32_t సూచీ,
                char16_t* buff,
                int32_t buff_సామర్థ్యం, 
                UError_సంకేతం* ec)
{
    int32_t resLen = 0;
	int32_t curr_సూచీ = 0;
    స్థిరరాశి char16_t* s = శూన్యసూచిక;

    ఒకవేళ (ec != శూన్యసూచిక && U_SUCCESS(*ec))
    {
        // check the arguments passed
        ఒకవేళ ((buff && buff_సామర్థ్యం) || !buff_సామర్థ్యం )
        {
            // local variables
            UError_సంకేతం local_స్థితి = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            ఒకవేళ (U_FAILURE(*ec))
            {
                తిరిగి 0;
            }

            // Remove variants, which is only needed for registration.
            అక్షరం *idDelim = strchr(id.దత్తాంశం(), VAR_DELIM);
            ఒకవేళ (idDelim)
            {
                id.truncate(idDelim - id.దత్తాంశం());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_open_ప్రత్యక్ష(U_ICUDATA_CURR, CURRENCY_దత్తాంశం, &local_స్థితి);
            UResourceBundle *cm = ures_get_By_కీలకం(rb, CURRENCY_MAP, rb, &local_స్థితి);

            // Using the id derived from the local, get the currency data
            Local_UResource_Bundle_సూచిక countryArray(ures_get_By_కీలకం(rb, id.దత్తాంశం(), cm, &local_స్థితి));

            // process each currency to see which one is valid for the given date
            తార్కిక సరిపోలిక_దొరికింది = అబద్ధం;
            ఒకవేళ (U_SUCCESS(local_స్థితి))
            {
                ఒకవేళ ((సూచీ <= 0) || (సూచీ> ures_get_కొలత(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    తిరిగి 0;
                }

                కొరకు (int32_t i=0; i<ures_get_కొలత(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    Local_UResource_Bundle_సూచిక currencyRes(ures_get_By_సూచీ(countryArray.getAlias(), i, శూన్యసూచిక, &local_స్థితి));
                    s = ures_get_String_By_కీలకం(currencyRes.getAlias(), "id", &resLen, &local_స్థితి);

                    // get the from date
                    int32_t from_నిడివి = 0;
                    Local_UResource_Bundle_సూచిక fromRes(ures_get_By_కీలకం(currencyRes.getAlias(), "from", శూన్యసూచిక, &local_స్థితి));
                    స్థిరరాశి int32_t *fromArray = ures_get_Int_సదిశ(fromRes.getAlias(), &from_నిడివి, &local_స్థితి);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    ఒకవేళ (ures_get_కొలత(currencyRes.getAlias()) > 2)
                    {
                        int32_t గమ్యం_నిడివి = 0;
                        Local_UResource_Bundle_సూచిక గమ్యం_Res(ures_get_By_కీలకం(currencyRes.getAlias(), "to", శూన్యసూచిక, &local_స్థితి));
                        స్థిరరాశి int32_t *గమ్యం_Array = ures_get_Int_సదిశ(గమ్యం_Res.getAlias(), &గమ్యం_నిడివి, &local_స్థితి);

                        currDate64 = (int64_t)గమ్యం_Array[0] << 32;
                        currDate64 |= ((int64_t)గమ్యం_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate గమ్యం_Date = (UDate)currDate64;

                        ఒకవేళ ((fromDate <= date) && (date < గమ్యం_Date))
                        {
                            curr_సూచీ++;
                            ఒకవేళ (curr_సూచీ == సూచీ)
                            {
                                సరిపోలిక_దొరికింది = నిజం;
                            }
                        }
                    }
                    లేకపోతే
                    {
                        ఒకవేళ (fromDate <= date)
                        {
                            curr_సూచీ++;
                            ఒకవేళ (curr_సూచీ == సూచీ)
                            {
                                సరిపోలిక_దొరికింది = నిజం;
                            }
                        }
                    }
                    // check for loop exit
                    ఒకవేళ (సరిపోలిక_దొరికింది)
                    {
                        ఆపు;
                    }

                } // end For loop
            }

            // Check for errors
            ఒకవేళ (*ec == U_ZERO_ERROR || local_స్థితి != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = local_స్థితి;
            }

            ఒకవేళ (U_SUCCESS(*ec))
            {
                // no errors
                ఒకవేళ((buff_సామర్థ్యం> resLen) && సరిపోలిక_దొరికింది)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                లేకపోతే
                {
                    తిరిగి 0;
                }
            }

            // return null terminated currency string
            తిరిగి u_terminateUChars(buff, buff_సామర్థ్యం, resLen, ec);
        }
        లేకపోతే
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_వాదం_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    తిరిగి resLen;
}

స్థిర స్థిరరాశి UEnumeration default_Keyword_విలువలు = {
    శూన్యసూచిక,
    శూన్యసూచిక,
    ulist_close_keyword_విలువలు_iterator,
    ulist_లెక్క_keyword_విలువలు,
    uenum_unextDefault,
    ulist_తదుపరి_keyword_విలువ, 
    ulist_reset_keyword_విలువలు_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_విలువలు_For_Locale(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *locale, UBool commonly_వాడిన, UError_సంకేతం* స్థితి_2) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_దత్తాంశం(locale, నిజం, *స్థితి_2);

    // Read value from supplementalData
    UList *విలువలు = ulist_create_ఖాళీలు_List(స్థితి_2);
    UList *other_విలువలు = ulist_create_ఖాళీలు_List(స్థితి_2);
    UEnumeration *en = (UEnumeration *)uprv_malloc(పరిమాణం(UEnumeration));
    ఒకవేళ (U_FAILURE(*స్థితి_2) || en == శూన్యసూచిక) {
        ఒకవేళ (en == శూన్యసూచిక) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            uprv_free(en);
        }
        ulist_deleteList(విలువలు);
        ulist_deleteList(other_విలువలు);
        తిరిగి శూన్యసూచిక;
    }
    జ్ఞాపకం_నకలు(en, &default_Keyword_విలువలు, పరిమాణం(UEnumeration));
    en->context = విలువలు;
    
    UResourceBundle* rb = ures_open_ప్రత్యక్ష(U_ICUDATA_CURR, "supplementalData", స్థితి_2);
    Local_UResource_Bundle_సూచిక bundle(ures_get_By_కీలకం(rb, "CurrencyMap", rb, స్థితి_2));
    దొంతర_UResource_Bundle bundlekey, regbndl, curbndl, గమ్యం;
    
    అయితే (U_SUCCESS(*స్థితి_2) && ures_has_తదుపరి(bundle.getAlias())) {
        ures_get_తదుపరి_Resource(bundle.getAlias(), bundlekey.getAlias(), స్థితి_2);
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            ఆపు;
        }
        స్థిరరాశి అక్షరం *region = ures_get_కీలకం(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        ఒకవేళ (!isPrefRegion && commonly_వాడిన) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            కొనసాగు;
        }
        ures_get_By_కీలకం(bundle.getAlias(), region, regbndl.getAlias(), స్థితి_2);
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            ఆపు;
        }
        అయితే (U_SUCCESS(*స్థితి_2) && ures_has_తదుపరి(regbndl.getAlias())) {
            ures_get_తదుపరి_Resource(regbndl.getAlias(), curbndl.getAlias(), స్థితి_2);
            ఒకవేళ (ures_getType(curbndl.getAlias()) != URES_పట్టిక) {
                // Currently, an empty ARRAY is mixed in.
                కొనసాగు;
            }
            అక్షరం *curID = (అక్షరం *)uprv_malloc(పరిమాణం(అక్షరం) * ULOC_KEYWORDS_సామర్థ్యం);
            ఒకవేళ (curID == శూన్యసూచిక) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                ఆపు;
            }
            int32_t curIDLength = ULOC_KEYWORDS_సామర్థ్యం;

#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_కీలకం(curbndl.getAlias(), "id", curID, &curIDLength, నిజం, స్థితి_2);
            /* optimize - use the utf-8 string */
#లేకపోతే
            {
                       స్థిరరాశి char16_t* defString = ures_get_String_By_కీలకం(curbndl.getAlias(), "id", &curIDLength, స్థితి_2);
                       ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
			   ఒకవేళ(curIDLength+1 > ULOC_KEYWORDS_సామర్థ్యం) {
				*స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
			   } లేకపోతే {
                           	u_UChars_గమ్యం_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#అయితే_ముగింపు	

            ఒకవేళ (U_FAILURE(*స్థితి_2)) {
                ఆపు;
            }
            UBool has_గమ్యం = అబద్ధం;
            ures_get_By_కీలకం(curbndl.getAlias(), "to", గమ్యం.getAlias(), స్థితి_2);
            ఒకవేళ (U_FAILURE(*స్థితి_2)) {
                // Do nothing here...
                *స్థితి_2 = U_ZERO_ERROR;
            } లేకపోతే {
                has_గమ్యం = నిజం;
            }
            ఒకవేళ (isPrefRegion && !has_గమ్యం && !ulist_కలిగియుంది_String(విలువలు, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_కూడు_అంశం_చివర_List(విలువలు, curID, నిజం, స్థితి_2);
            } లేకపోతే ఒకవేళ (!ulist_కలిగియుంది_String(other_విలువలు, curID, (int32_t)uprv_strlen(curID)) && !commonly_వాడిన) {
                ulist_కూడు_అంశం_చివర_List(other_విలువలు, curID, నిజం, స్థితి_2);
            } లేకపోతే {
                uprv_free(curID);
            }
        }
        
    }
    ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
        ఒకవేళ (commonly_వాడిన) {
            ఒకవేళ (ulist_get_List_కొలత(విలువలు) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_విలువలు_For_Locale(కీలకం, "und", నిజం, స్థితి_2);
            }
        } లేకపోతే {
            // Consolidate the list
            అక్షరం *value = శూన్యసూచిక;
            ulist_resetList(other_విలువలు);
            అయితే ((value = (అక్షరం *)ulist_get_తదుపరి(other_విలువలు)) != శూన్యసూచిక) {
                ఒకవేళ (!ulist_కలిగియుంది_String(విలువలు, value, (int32_t)uprv_strlen(value))) {
                    అక్షరం *tmp_విలువ = (అక్షరం *)uprv_malloc(పరిమాణం(అక్షరం) * ULOC_KEYWORDS_సామర్థ్యం);
                    ఒకవేళ (tmp_విలువ == శూన్యసూచిక) {
                        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        ఆపు;
                    }
                    uprv_memcpy(tmp_విలువ, value, uprv_strlen(value) + 1);
                    ulist_కూడు_అంశం_చివర_List(విలువలు, tmp_విలువ, నిజం, స్థితి_2);
                    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
                        ఆపు;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } లేకపోతే {
        ulist_deleteList(విలువలు);
        uprv_free(en);
        విలువలు = శూన్యసూచిక;
        en = శూన్యసూచిక;
    }
    ulist_deleteList(other_విలువలు);
    తిరిగి en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_సంకేతం(స్థిరరాశి char16_t* currency) {
    int32_t సంకేతం_2 = 0;
    ఒకవేళ (currency && u_strlen(currency) == ISO_CURRENCY_సంకేతం_నిడివి) {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_ప్రత్యక్ష(శూన్యసూచిక, "currencyNumericCodes", &స్థితి_2);
        Local_UResource_Bundle_సూచిక సంకేతం_Map(ures_get_By_కీలకం(bundle, "codeMap", bundle, &స్థితి_2));
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            అక్షరం alpha_సంకేతం[ISO_CURRENCY_సంకేతం_నిడివి+1];
            my_UChars_గమ్యం_Chars(alpha_సంకేతం, currency);
            T_CString_గమ్యం_పై_Case(alpha_సంకేతం);
            ures_get_By_కీలకం(సంకేతం_Map.getAlias(), alpha_సంకేతం, సంకేతం_Map.getAlias(), &స్థితి_2);
            పూర్ణాంకం tmp_సంకేతం = ures_getInt(సంకేతం_Map.getAlias(), &స్థితి_2);
            ఒకవేళ (U_SUCCESS(స్థితి_2)) {
                సంకేతం_2 = tmp_సంకేతం;
            }
        }
    }
    తిరిగి సంకేతం_2;
}
#అయితే_ముగింపు /* #if !UCONFIG_NO_FORMATTING */

//eof
