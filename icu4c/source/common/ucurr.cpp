// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_FORMATTING

#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/ucurr.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/usetiter.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "static_unicode_sets.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "hash.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "ulist.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "ureslocs.h"
#अन्तर्भाव "ulocimp.h"

उपयुज्य नामाकाश icu;

//#define UCURR_DEBUG_EQUIV 1
#यदि_परिभाषितम् UCURR_DEBUG_EQUIV
#अन्तर्भाव "stdio.h"
#यदि_अन्तः
//#define UCURR_DEBUG 1
#यदि_परिभाषितम् UCURR_DEBUG
#अन्तर्भाव "stdio.h"
#यदि_अन्तः

प्रकारनिर्देश संरचना Iso_संहिता_प्रविष्टिः {
    ध्रुव char16_t *iso_संहिता; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate गन्तव्यम्;
} Iso_संहिता_प्रविष्टिः;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
स्थिर ध्रुव int32_t चरमः_RESORT_दत्त[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
स्थिर ध्रुव int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

स्थिर ध्रुव int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#परिभाषय ISO_CURRENCY_संहिता_दीर्घता 3

//------------------------------------------------------------
// Resource tags
//

स्थिर ध्रुव वर्ण CURRENCY_दत्त[] = "supplementalData";
// Tag for meta-data, in root.
स्थिर ध्रुव वर्ण CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
स्थिर ध्रुव वर्ण CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
स्थिर ध्रुव वर्ण DEFAULT_META[] = "DEFAULT";

// Variant delimiter
स्थिर ध्रुव वर्ण VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
स्थिर ध्रुव वर्ण CURRENCIES[] = "Currencies";
स्थिर ध्रुव वर्ण CURRENCIES_NARROW[] = "Currencies%narrow";
स्थिर ध्रुव वर्ण CURRENCIES_FORMAL[] = "Currencies%formal";
स्थिर ध्रुव वर्ण CURRENCIES_VARIANT[] = "Currencies%variant";
स्थिर ध्रुव वर्ण CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
स्थिर ध्रुव UHashtable* gIsoCodes = शून्यसूचकः;
स्थिर icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
स्थिर ध्रुव icu::Hashtable* gCurrSymbolsEquiv = शून्यसूचकः;
स्थिर icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
वर्ग EquivIterator : सार्वजनिक icu::UMemory {
सार्वजनिक:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    अन्तःस्थ EquivIterator(ध्रुव icu::Hashtable& hash, ध्रुव icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    अन्तःस्थ ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ध्रुव icu::UnicodeString *next();
गुप्त:
    ध्रुव icu::Hashtable& _hash;
    ध्रुव icu::UnicodeString* _start;
    ध्रुव icu::UnicodeString* _current;
};

ध्रुव icu::UnicodeString *
EquivIterator::next() {
    ध्रुव icu::UnicodeString* अग्र_2 = static_cast<ध्रुव icu::UnicodeString*>(_hash.get(*_current));
    यदि (अग्र_2 == शून्यसूचकः) {
        U_ASSERT(_current == _start);
        प्रत्यागम शून्यसूचकः;
    }
    यदि (*अग्र_2 == *_start) {
        प्रत्यागम शून्यसूचकः;
    }
    _current = अग्र_2;
    प्रत्यागम अग्र_2;
}

U_NAMESPACE_अन्तः

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
स्थिर शून्य makeEquivalent(
    ध्रुव icu::UnicodeString &lhs,
    ध्रुव icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (lhs == rhs) {
        // already equivalent
        प्रत्यागम;
    }
    icu::EquivIterator वामः_Iter(*hash, lhs);
    icu::EquivIterator दक्षिणः_Iter(*hash, rhs);
    ध्रुव icu::UnicodeString *first_वामः = वामः_Iter.next();
    ध्रुव icu::UnicodeString *first_दक्षिणः = दक्षिणः_Iter.next();
    ध्रुव icu::UnicodeString *अग्र_वामः = first_वामः;
    ध्रुव icu::UnicodeString *अग्र_दक्षिणः = first_दक्षिणः;
    यावत् (अग्र_वामः != शून्यसूचकः && अग्र_दक्षिणः != शून्यसूचकः) {
        यदि (*अग्र_वामः == rhs || *अग्र_दक्षिणः == lhs) {
            // Already equivalent
            प्रत्यागम;
        }
        अग्र_वामः = वामः_Iter.next();
        अग्र_दक्षिणः = दक्षिणः_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_वामः;
    icu::UnicodeString *new_First_दक्षिणः;
    यदि (first_दक्षिणः == शून्यसूचकः && first_वामः == शून्यसूचकः) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_वामः = नूतन icu::UnicodeString(rhs);
        new_First_दक्षिणः = नूतन icu::UnicodeString(lhs);
    } अन्यथा यदि (first_दक्षिणः == शून्यसूचकः) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_वामः = नूतन icu::UnicodeString(rhs);
        new_First_दक्षिणः = नूतन icu::UnicodeString(*first_वामः);
    } अन्यथा यदि (first_वामः == शून्यसूचकः) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_वामः = नूतन icu::UnicodeString(*first_दक्षिणः);
        new_First_दक्षिणः = नूतन icu::UnicodeString(lhs);
    } अन्यथा {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_वामः = नूतन icu::UnicodeString(*first_दक्षिणः);
        new_First_दक्षिणः = नूतन icu::UnicodeString(*first_वामः);
    }
    यदि (new_First_वामः == शून्यसूचकः || new_First_दक्षिणः == शून्यसूचकः) {
        विलोपय new_First_वामः;
        विलोपय new_First_दक्षिणः;
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    hash->put(lhs, (शून्य *) new_First_वामः, स्थितिः);
    hash->put(rhs, (शून्य *) new_First_दक्षिणः, स्थितिः);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
स्थिर int32_t गणनम्_Equivalent(ध्रुव icu::Hashtable &hash, ध्रुव icu::UnicodeString &s) {
    int32_t परिणाम = 0;
    icu::EquivIterator iter(hash, s);
    यावत् (iter.next() != शून्यसूचकः) {
        ++परिणाम;
    }
#यदि_परिभाषितम् UCURR_DEBUG_EQUIV
 {
   वर्ण tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   प्रारूप_लिख("CountEquivalent('%s') = %d\n", tmp, परिणाम);
 }
#यदि_अन्तः
    प्रत्यागम परिणाम;
}

स्थिर ध्रुव icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//संहिता

/**
 * Cleanup callback func
 */
स्थिर UBool U_CALLCONV 
isoCodes_cleanup()
{
    यदि (gIsoCodes != शून्यसूचकः) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = शून्यसूचकः;
    }
    gIsoCodesInitOnce.reset();
    प्रत्यागम सत्य;
}

/**
 * Cleanup callback func
 */
स्थिर UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    विलोपय const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = शून्यसूचकः;
    gCurrSymbolsEquivInitOnce.reset();
    प्रत्यागम सत्य;
}

/**
 * Deleter for IsoCodeEntry
 */
स्थिर शून्य U_CALLCONV
delete_Iso_संहिता_प्रविष्टिः(शून्य *obj) {
    Iso_संहिता_प्रविष्टिः* प्रविष्टिः = static_cast<Iso_संहिता_प्रविष्टिः*>(obj);
    uprv_free(प्रविष्टिः);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
स्थिर शून्य U_CALLCONV
deleteUnicode(शून्य *obj) {
    icu::UnicodeString* प्रविष्टिः = static_cast<icu::UnicodeString*>(obj);
    विलोपय प्रविष्टिः;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
स्थिर अन्तःस्थ वर्ण*
my_UChars_गन्तव्यम्_Chars(वर्ण* परिणाम_सम्बन्धि_Len4, ध्रुव char16_t* currency) {
    u_UChars_गन्तव्यम्_Chars(currency, परिणाम_सम्बन्धि_Len4, ISO_CURRENCY_संहिता_दीर्घता);
    परिणाम_सम्बन्धि_Len4[ISO_CURRENCY_संहिता_दीर्घता] = 0;
    प्रत्यागम परिणाम_सम्बन्धि_Len4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
स्थिर ध्रुव int32_t*
find_Meta_दत्त(ध्रुव char16_t* currency, UError_संहिता& ec) {

    यदि (currency == शून्यसूचकः || *currency == 0) {
        यदि (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_तर्कः_ERROR;
        }
        प्रत्यागम चरमः_RESORT_दत्त;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_दत्त = ures_open_प्रत्यक्षम्(U_ICUDATA_CURR, CURRENCY_दत्त, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_कुञ्चिका(currency_दत्त, CURRENCY_META, currency_दत्त, &ec));

    यदि (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        प्रत्यागम चरमः_RESORT_दत्त;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    वर्ण buf[ISO_CURRENCY_संहिता_दीर्घता+1];
    UError_संहिता ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_कुञ्चिका(currencyMeta.getAlias(), my_UChars_गन्तव्यम्_Chars(buf, currency), शून्यसूचकः, &ec2));
      यदि (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_कुञ्चिका(currencyMeta.getAlias(),DEFAULT_META, शून्यसूचकः, &ec));
        यदि (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            प्रत्यागम चरमः_RESORT_दत्त;
        }
    }

    int32_t len;
    ध्रुव int32_t *दत्त = ures_get_Int_सदिशम्(rb.getAlias(), &len, &ec);
    यदि (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        यदि (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        प्रत्यागम चरमः_RESORT_दत्त;
    }

    प्रत्यागम दत्त;
}

// -------------------------------------

स्थिर CharString
idForLocale(ध्रुव वर्ण* locale, UError_संहिता* ec)
{
    प्रत्यागम ulocimp_get_Region_For_Supplemental_दत्त(locale, असत्य, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV currency_cleanup();
U_CDECL_अन्तः

#यदि !UCONFIG_NO_SERVICE
संरचना CReg;

स्थिर UMutex g_CReg_तालकम्;
स्थिर CReg* g_CReg_शीर्ष = शून्यसूचकः;

संरचना CReg : सार्वजनिक icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_संहिता_दीर्घता+1];
    वर्ण  id[ULOC_FULLNAME_क्षमता];

    CReg(ध्रुव char16_t* _iso, ध्रुव वर्ण* _id)
        : next(शून्यसूचकः)
    {
        uprv_strncpy(id, _id, परिमाण(id)-1);
        id[परिमाण(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_संहिता_दीर्घता);
        iso[ISO_CURRENCY_संहिता_दीर्घता] = 0;
    }

    स्थिर UCurr_Registry_कुञ्चिका reg(ध्रुव char16_t* _iso, ध्रुव वर्ण* _id, UError_संहिता* स्थितिः)
    {
        यदि (स्थितिः && U_SUCCESS(*स्थितिः) && _iso && _id) {
            CReg* n = नूतन CReg(_iso, _id);
            यदि (n) {
                umtx_तालकम्(&g_CReg_तालकम्);
                यदि (!g_CReg_शीर्ष) {
                    /* register for the first time */
                    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_शीर्ष;
                g_CReg_शीर्ष = n;
                umtx_unlock(&g_CReg_तालकम्);
                प्रत्यागम n;
            }
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        }
        प्रत्यागम शून्यसूचकः;
    }

    स्थिर UBool unreg(UCurr_Registry_कुञ्चिका कुञ्चिका) {
        UBool प्राप्तम् = असत्य;
        umtx_तालकम्(&g_CReg_तालकम्);

        CReg** p = &g_CReg_शीर्ष;
        यावत् (*p) {
            यदि (*p == कुञ्चिका) {
                *p = ((CReg*)कुञ्चिका)->next;
                विलोपय (CReg*)कुञ्चिका;
                प्राप्तम् = सत्य;
                भङ्ग;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_तालकम्);
        प्रत्यागम प्राप्तम्;
    }

    स्थिर ध्रुव char16_t* get(ध्रुव वर्ण* id) {
        ध्रुव char16_t* परिणाम = शून्यसूचकः;
        umtx_तालकम्(&g_CReg_तालकम्);
        CReg* p = g_CReg_शीर्ष;

        /* register cleanup of the mutex */
        ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        यावत् (p) {
            यदि (uprv_strcmp(id, p->id) == 0) {
                परिणाम = p->iso;
                भङ्ग;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_तालकम्);
        प्रत्यागम परिणाम;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    स्थिर शून्य cleanup() {
        यावत् (g_CReg_शीर्ष) {
            CReg* n = g_CReg_शीर्ष;
            g_CReg_शीर्ष = g_CReg_शीर्ष->next;
            विलोपय n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_कुञ्चिका U_EXPORT2
ucurr_पञ्जिकाकोशः(ध्रुव char16_t* iso_संहिता, ध्रुव वर्ण* locale, UError_संहिता *स्थितिः)
{
    यदि (स्थितिः && U_SUCCESS(*स्थितिः)) {
        CharString id = idForLocale(locale, स्थितिः);
        प्रत्यागम CReg::reg(iso_संहिता, id.दत्त(), स्थितिः);
    }
    प्रत्यागम शून्यसूचकः;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_कुञ्चिका कुञ्चिका, UError_संहिता* स्थितिः)
{
    यदि (स्थितिः && U_SUCCESS(*स्थितिः)) {
        प्रत्यागम CReg::unreg(कुञ्चिका);
    }
    प्रत्यागम असत्य;
}
#यदि_अन्तः /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
स्थिर UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV currency_cleanup() {
#यदि !UCONFIG_NO_SERVICE
    CReg::cleanup();
#यदि_अन्तः
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    प्रत्यागम सत्य;
}
U_CDECL_अन्तः

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ध्रुव वर्ण* locale,
                char16_t* buff,
                int32_t buff_क्षमता,
                UError_संहिता* ec) {
    यदि (U_FAILURE(*ec)) { प्रत्यागम 0; }
    यदि (buff_क्षमता < 0 || (buff == शून्यसूचकः && buff_क्षमता > 0)) {
        *ec = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    UError_संहिता local_स्थितिः = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_मानम्(locale, "currency", local_स्थितिः);
    int32_t resLen = currency.length();

    यदि (U_SUCCESS(local_स्थितिः) && resLen == 3 && uprv_isInvariantString(currency.दत्त(), resLen)) {
        यदि (resLen < buff_क्षमता) {
            T_CString_गन्तव्यम्_ऊर्ध्वस्थम्_Case(currency.दत्त());
            u_chars_गन्तव्यम्_UChars(currency.दत्त(), buff, resLen);
        }
        प्रत्यागम u_terminateUChars(buff, buff_क्षमता, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    यदि (U_FAILURE(*ec)) {
        प्रत्यागम 0;
    }

#यदि !UCONFIG_NO_SERVICE
    ध्रुव char16_t* परिणाम = CReg::get(id.दत्त());
    यदि (परिणाम) {
        यदि(buff_क्षमता > u_strlen(परिणाम)) {
            u_strcpy(buff, परिणाम);
        }
        resLen = u_strlen(परिणाम);
        प्रत्यागम u_terminateUChars(buff, buff_क्षमता, resLen, ec);
    }
#यदि_अन्तः
    // Remove variants, which is only needed for registration.
    वर्ण *idDelim = uprv_strchr(id.दत्त(), VAR_DELIM);
    यदि (idDelim) {
        id.truncate(idDelim - id.दत्त());
    }

    ध्रुव char16_t* s = शून्यसूचकः;  // Currency code from data file.
    यदि (id.is_रिक्तानि()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_स्थितिः = U_MISSING_RESOURCE_ERROR;
    } अन्यथा {
        // Look up the CurrencyMap element in the root bundle.
        local_स्थितिः = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_प्रत्यक्षम्(U_ICUDATA_CURR, CURRENCY_दत्त, &local_स्थितिः);
        UResourceBundle *cm = ures_get_By_कुञ्चिका(rb, CURRENCY_MAP, rb, &local_स्थितिः);
        LocalUResourceBundlePointer countryArray(ures_get_By_कुञ्चिका(rb, id.दत्त(), cm, &local_स्थितिः));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        यदि (U_SUCCESS(local_स्थितिः)) {
            int32_t array_दीर्घता = ures_get_आकार(countryArray.getAlias());
            प्रति (int32_t i = 0; i < array_दीर्घता; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_get_By_सूचकांक(countryArray.getAlias(), i, शून्यसूचकः, &local_स्थितिः));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_संहिता tender_स्थितिः = local_स्थितिः;
                ध्रुव char16_t *tender =
                    ures_get_String_By_कुञ्चिका(currencyReq.getAlias(), "tender", शून्यसूचकः, &tender_स्थितिः);
                तार्किक isTender = U_FAILURE(tender_स्थितिः) || u_strcmp(tender, u"false") != 0;
                यदि (!isTender && s != शून्यसूचकः) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    अनुवर्त;
                }
                // Fetch the currency code.
                s = ures_get_String_By_कुञ्चिका(currencyReq.getAlias(), "id", &resLen, &local_स्थितिः);
                यदि (isTender) {
                    भङ्ग;
                }
            }
            यदि (U_SUCCESS(local_स्थितिः) && s == शून्यसूचकः) {
                local_स्थितिः = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    यदि ((U_FAILURE(local_स्थितिः)) && strchr(id.दत्त(), '_') != शून्यसूचकः) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        प्रत्यागम ucurr_forLocale(parent.दत्त(), buff, buff_क्षमता, ec);
    }
    यदि (*ec == U_ZERO_ERROR || local_स्थितिः != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_स्थितिः;
    }
    यदि (U_SUCCESS(*ec)) {
        यदि(buff_क्षमता > resLen) {
            u_strcpy(buff, s);
        }
    }
    प्रत्यागम u_terminateUChars(buff, buff_क्षमता, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
स्थिर UBool fallback(CharString& loc) {
    यदि (loc.is_रिक्तानि()) {
        प्रत्यागम असत्य;
    }
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    यदि (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", स्थितिः);
    } अन्यथा {
        loc = ulocimp_getParent(loc.दत्त(), स्थितिः);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    प्रत्यागम सत्य;
}


U_CAPI ध्रुव char16_t* U_EXPORT2
ucurr_getName(ध्रुव char16_t* currency U_LIFETIME_BOUND,
              ध्रुव वर्ण* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UError_संहिता* ec) {

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

    यदि (U_FAILURE(*ec)) {
        प्रत्यागम शून्यसूचकः;
    }

    int32_t choice = (int32_t) nameStyle;
    यदि (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
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
    UError_संहिता ec2 = U_ZERO_ERROR;

    यदि (locale == शून्यसूचकः) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    यदि (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    वर्ण buf[ISO_CURRENCY_संहिता_दीर्घता+1];
    my_UChars_गन्तव्यम्_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_गन्तव्यम्_ऊर्ध्वस्थम्_Case(buf);
    
    ध्रुव char16_t* s = शून्यसूचकः;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.दत्त(), &ec2));

    यदि (nameStyle == UCURR_NARROW_चिह्नम्_NAME || nameStyle == UCURR_FORMAL_चिह्नम्_NAME || nameStyle == UCURR_VARIANT_चिह्नम्_NAME) {
        CharString कुञ्चिका;
        विकल्प (nameStyle) {
        पक्ष UCURR_NARROW_चिह्नम्_NAME:
            कुञ्चिका.append(CURRENCIES_NARROW, ec2);
            भङ्ग;
        पक्ष UCURR_FORMAL_चिह्नम्_NAME:
            कुञ्चिका.append(CURRENCIES_FORMAL, ec2);
            भङ्ग;
        पक्ष UCURR_VARIANT_चिह्नम्_NAME:
            कुञ्चिका.append(CURRENCIES_VARIANT, ec2);
            भङ्ग;
        स्वभाव:
            *ec = U_UNSUPPORTED_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        कुञ्चिका.append("/", ec2);
        कुञ्चिका.append(buf, ec2);
        s = ures_get_String_By_कुञ्चिका_With_Fallback(rb.getAlias(), कुञ्चिका.दत्त(), len, &ec2);
        यदि (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_चिह्नम्_NAME;
        }
    }
    यदि (s == शून्यसूचकः) {
        ures_get_By_कुञ्चिका(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_कुञ्चिका_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_सूचकांक(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    यदि (U_SUCCESS(ec2)) {
        यदि (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    यदि (isChoiceFormat != शून्यसूचकः) {
        *isChoiceFormat = असत्य;
    }
    यदि (U_SUCCESS(ec2)) {
        U_ASSERT(s != शून्यसूचकः);
        प्रत्यागम s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    प्रत्यागम currency;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ucurr_getPluralName(ध्रुव char16_t* currency U_LIFETIME_BOUND,
                    ध्रुव वर्ण* locale,
                    UBool* isChoiceFormat,
                    ध्रुव वर्ण* plural_गणनम्,
                    int32_t* len, // fillin
                    UError_संहिता* ec) {
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

    यदि (U_FAILURE(*ec)) {
        प्रत्यागम शून्यसूचकः;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_संहिता ec2 = U_ZERO_ERROR;

    यदि (locale == शून्यसूचकः) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    यदि (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    वर्ण buf[ISO_CURRENCY_संहिता_दीर्घता+1];
    my_UChars_गन्तव्यम्_Chars(buf, currency);

    ध्रुव char16_t* s = शून्यसूचकः;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.दत्त(), &ec2);

    rb = ures_get_By_कुञ्चिका(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_कुञ्चिका_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_कुञ्चिका_With_Fallback(curr.getAlias(), plural_गणनम्, len, &ec2);
    यदि (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_कुञ्चिका_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        यदि (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            प्रत्यागम ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    यदि (U_SUCCESS(ec2)) {
        यदि (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != शून्यसूचकः);
        प्रत्यागम s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    प्रत्यागम currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#परिभाषय NEED_गन्तव्यम्_BE_DELETED 0x1

// TODO: a better way to define this?
#परिभाषय MAX_CURRENCY_NAME_LEN 100

प्रकारनिर्देश संरचना {
    ध्रुव वर्ण* Iso_संहिता;  //कुञ्चिका
    char16_t* currencyName;  //मानम्
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#यदि_अपरिभाषितम् MIN
#परिभाषय MIN(a,b) (((a)<(b)) ? (a) : (b))
#यदि_अन्तः

#यदि_अपरिभाषितम् MAX
#परिभाषय MAX(a,b) (((a)<(b)) ? (b) : (a))
#यदि_अन्तः


// Comparison function used in quick sort.
स्थिर पूर्णाङ्क U_CALLCONV currencyNameComparator(ध्रुव शून्य* a, ध्रुव शून्य* b) {
    ध्रुव CurrencyNameStruct* currName_1 = static_cast<ध्रुव CurrencyNameStruct*>(a);
    ध्रुव CurrencyNameStruct* currName_2 = static_cast<ध्रुव CurrencyNameStruct*>(b);
    प्रति (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        यदि (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            प्रत्यागम -1;
        }
        यदि (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            प्रत्यागम 1;
        }
    }
    यदि (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        प्रत्यागम -1;
    } अन्यथा यदि (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        प्रत्यागम 1;
    }
    प्रत्यागम 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
स्थिर शून्य
get_Currency_Name_गणनम्(ध्रुव वर्ण* loc, int32_t* योग_currency_name_गणनम्, int32_t* योग_currency_चिह्नम्_गणनम्) {
    U_NAMESPACE_USE
    *योग_currency_name_गणनम् = 0;
    *योग_currency_चिह्नम्_गणनम् = 0;
    ध्रुव char16_t* s = शून्यसूचकः;
    CharString locale;
    {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        locale.append(loc, स्थितिः);
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    }
    ध्रुव icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    प्रति (;;) {
        UError_संहिता ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.दत्त(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_कुञ्चिका(rb.getAlias(), CURRENCIES, शून्यसूचकः, &ec2));
        int32_t n = ures_get_आकार(curr.getAlias());
        प्रति (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer नामानि(ures_get_By_सूचकांक(curr.getAlias(), i, शून्यसूचकः, &ec2));
            int32_t len;
            s = ures_get_String_By_सूचकांक(नामानि.getAlias(), UCURR_चिह्नम्_NAME, &len, &ec2);
            ++(*योग_currency_चिह्नम्_गणनम्);  // currency symbol
            यदि (currencySymbolsEquiv != शून्यसूचकः) {
                *योग_currency_चिह्नम्_गणनम् += गणनम्_Equivalent(*currencySymbolsEquiv, UnicodeString(सत्य, s, len));
            }
            ++(*योग_currency_चिह्नम्_गणनम्); // iso code
            ++(*योग_currency_name_गणनम्); // long name
        }

        // currency plurals
        UError_संहिता ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_कुञ्चिका(rb.getAlias(), CURRENCYPLURALS, शून्यसूचकः, &ec3));
        n = ures_get_आकार(curr_p.getAlias());
        प्रति (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer नामानि(ures_get_By_सूचकांक(curr_p.getAlias(), i, शून्यसूचकः, &ec3));
            *योग_currency_name_गणनम् += ures_get_आकार(नामानि.getAlias());
        }

        यदि (!fallback(locale)) {
            भङ्ग;
        }
    }
}

स्थिर char16_t*
गन्तव्यम्_ऊर्ध्वस्थम्_Case(ध्रुव char16_t* source, int32_t len, ध्रुव वर्ण* locale) {
    char16_t* dest = शून्यसूचकः;
    UError_संहिता ec = U_ZERO_ERROR;
    int32_t destLen = u_str_गन्तव्यम्_ऊर्ध्वस्थम्(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(परिमाण(char16_t) * MAX(destLen, len)));
    यदि (dest == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    u_str_गन्तव्यम्_ऊर्ध्वस्थम्(dest, destLen, source, len, locale, &ec);
    यदि (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    प्रत्यागम dest;
}


स्थिर शून्य delete_Currency_नामानि(CurrencyNameStruct* currency_नामानि, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
स्थिर शून्य
collect_Currency_नामानि(ध्रुव वर्ण* locale, 
                     CurrencyNameStruct** currency_नामानि, 
                     int32_t* योग_currency_name_गणनम्, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* योग_currency_चिह्नम्_गणनम्, 
                     UError_संहिता& ec) {
    यदि (U_FAILURE(ec)) {
        *currency_नामानि = *currencySymbols = शून्यसूचकः;
        *योग_currency_name_गणनम् = *योग_currency_चिह्नम्_गणनम् = 0;
        प्रत्यागम;
    }
    U_NAMESPACE_USE
    ध्रुव icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_संहिता ec2 = U_ZERO_ERROR;

    यदि (locale == शून्यसूचकः) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    यदि (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_तर्कः_ERROR;
        *currency_नामानि = *currencySymbols = शून्यसूचकः;
        *योग_currency_name_गणनम् = *योग_currency_चिह्नम्_गणनम् = 0;
        प्रत्यागम;
    }

    // Get maximum currency name count first.
    get_Currency_Name_गणनम्(loc.दत्त(), योग_currency_name_गणनम्, योग_currency_चिह्नम्_गणनम्);

    *currency_नामानि = static_cast<CurrencyNameStruct*>(
        uprv_malloc(परिमाण(CurrencyNameStruct) * (*योग_currency_name_गणनम्)));
    यदि(*currency_नामानि == शून्यसूचकः) {
        *currencySymbols = शून्यसूचकः;
        *योग_currency_name_गणनम् = *योग_currency_चिह्नम्_गणनम् = 0;
        ec = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(परिमाण(CurrencyNameStruct) * (*योग_currency_चिह्नम्_गणनम्)));

    यदि(*currencySymbols == शून्यसूचकः) {
        uprv_free(*currency_नामानि);
        *currency_नामानि = शून्यसूचकः;
        *योग_currency_name_गणनम् = *योग_currency_चिह्नम्_गणनम् = 0;
        ec = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    ध्रुव char16_t* s = शून्यसूचकः;  // currency name
    वर्ण* iso = शून्यसूचकः;  // currency ISO code

    *योग_currency_name_गणनम् = 0;
    *योग_currency_चिह्नम्_गणनम् = 0;

    UError_संहिता ec3 = U_ZERO_ERROR;
    UError_संहिता ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, शून्यसूचकः, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, शून्यसूचकः, &ec4));
    प्रति (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.दत्त(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_कुञ्चिका(rb.getAlias(), CURRENCIES, शून्यसूचकः, &ec2));
        int32_t n = ures_get_आकार(curr.getAlias());
        प्रति (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer नामानि(ures_get_By_सूचकांक(curr.getAlias(), i, शून्यसूचकः, &ec2));
            int32_t len;
            s = ures_get_String_By_सूचकांक(नामानि.getAlias(), UCURR_चिह्नम्_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<वर्ण*>(ures_get_कुञ्चिका(नामानि.getAlias()));
            यदि (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != शून्यसूचकः) {
                अनुवर्त;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*योग_currency_चिह्नम्_गणनम्)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            यदि (currencySymbolsEquiv != शून्यसूचकः) {
                UnicodeString str(सत्य, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ध्रुव UnicodeString *चिह्नम्;
                यावत् ((चिह्नम् = iter.next()) != शून्यसूचकः) {
                    (*currencySymbols)[(*योग_currency_चिह्नम्_गणनम्)++]
                        = {iso, const_cast<char16_t*>(चिह्नम्->get_बफर()), चिह्नम्->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_सूचकांक(नामानि.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* ऊर्ध्वस्थम्_Name = गन्तव्यम्_ऊर्ध्वस्थम्_Case(s, len, locale);
            यदि (ऊर्ध्वस्थम्_Name == शून्यसूचकः) {
                ec = U_स्मृतिः_आवण्टनम्_ERROR;
                गच्छ error;
            }
            (*currency_नामानि)[(*योग_currency_name_गणनम्)++] = {iso, ऊर्ध्वस्थम्_Name, len, NEED_गन्तव्यम्_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_संहिता = static_cast<char16_t*>(uprv_malloc(परिमाण(char16_t) * 3));
            यदि (iso_संहिता == शून्यसूचकः) {
                ec = U_स्मृतिः_आवण्टनम्_ERROR;
                गच्छ error;
            }
            // Must convert iso[] into Unicode
            u_chars_गन्तव्यम्_UChars(iso, iso_संहिता, 3);
            (*currencySymbols)[(*योग_currency_चिह्नम्_गणनम्)++] = {iso, iso_संहिता, 3, NEED_गन्तव्यम्_BE_DELETED};
        }

        // currency plurals
        UError_संहिता ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_कुञ्चिका(rb.getAlias(), CURRENCYPLURALS, शून्यसूचकः, &ec5));
        n = ures_get_आकार(curr_p.getAlias());
        प्रति (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer नामानि(ures_get_By_सूचकांक(curr_p.getAlias(), i, शून्यसूचकः, &ec5));
            iso = const_cast<वर्ण*>(ures_get_कुञ्चिका(नामानि.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            यदि (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != शून्यसूचकः) {
                अनुवर्त;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_आकार(नामानि.getAlias());
            int32_t len;
            प्रति (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_सूचकांक(नामानि.getAlias(), j, &len, &ec5);
                char16_t* ऊर्ध्वस्थम्_Name = गन्तव्यम्_ऊर्ध्वस्थम्_Case(s, len, locale);
                यदि (ऊर्ध्वस्थम्_Name == शून्यसूचकः) {
                    ec = U_स्मृतिः_आवण्टनम्_ERROR;
                    गच्छ error;
                }
                (*currency_नामानि)[(*योग_currency_name_गणनम्)++] = {iso, ऊर्ध्वस्थम्_Name, len, NEED_गन्तव्यम्_BE_DELETED};
            }
        }

        यदि (!fallback(loc)) {
            भङ्ग;
        }
    }

    // quick sort the struct
    शीघ्रं_क्रमय(*currency_नामानि, *योग_currency_name_गणनम्, 
          परिमाण(CurrencyNameStruct), currencyNameComparator);
    शीघ्रं_क्रमय(*currencySymbols, *योग_currency_चिह्नम्_गणनम्, 
          परिमाण(CurrencyNameStruct), currencyNameComparator);

#यदि_परिभाषितम् UCURR_DEBUG
    प्रारूप_लिख("currency name count: %d\n", *योग_currency_name_गणनम्);
    प्रति (int32_t सूचकांक = 0; सूचकांक < *योग_currency_name_गणनम्; ++सूचकांक) {
        प्रारूप_लिख("index: %d\n", सूचकांक);
        प्रारूप_लिख("iso: %s\n", (*currency_नामानि)[सूचकांक].Iso_संहिता);
        वर्ण curNameBuf[1024];
        स्मृतिं_पूरय(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_नामानि)[सूचकांक].currencyName, (*currency_नामानि)[सूचकांक].currencyNameLen);
        प्रारूप_लिख("currencyName: %s\n", curNameBuf);
        प्रारूप_लिख("len: %d\n", (*currency_नामानि)[सूचकांक].currencyNameLen);
    }
    प्रारूप_लिख("currency symbol count: %d\n", *योग_currency_चिह्नम्_गणनम्);
    प्रति (int32_t सूचकांक = 0; सूचकांक < *योग_currency_चिह्नम्_गणनम्; ++सूचकांक) {
        प्रारूप_लिख("index: %d\n", सूचकांक);
        प्रारूप_लिख("iso: %s\n", (*currencySymbols)[सूचकांक].Iso_संहिता);
        वर्ण curNameBuf[1024];
        स्मृतिं_पूरय(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[सूचकांक].currencyName, (*currencySymbols)[सूचकांक].currencyNameLen);
        प्रारूप_लिख("currencySymbol: %s\n", curNameBuf);
        प्रारूप_लिख("len: %d\n", (*currencySymbols)[सूचकांक].currencyNameLen);
    }
#यदि_अन्तः
    // fail on hashtable errors
    यदि (U_FAILURE(ec3)) {
      ec = ec3;
    } अन्यथा यदि (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    यदि (U_FAILURE(ec)) {
        delete_Currency_नामानि(*currency_नामानि, *योग_currency_name_गणनम्);
        delete_Currency_नामानि(*currencySymbols, *योग_currency_चिह्नम्_गणनम्);
        *currency_नामानि = *currencySymbols = शून्यसूचकः;
        *योग_currency_name_गणनम् = *योग_currency_चिह्नम्_गणनम् = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
स्थिर int32_t
द्विमान_अन्वेषणम्(ध्रुव CurrencyNameStruct* currency_नामानि, 
             int32_t सूचकांक_अन्तर्_Currency_नामानि,
             ध्रुव char16_t कुञ्चिका,
             int32_t* begin, int32_t* अन्तः) {
#यदि_परिभाषितम् UCURR_DEBUG
    प्रारूप_लिख("key = %x\n", कुञ्चिका);
#यदि_अन्तः
   int32_t first = *begin;
   int32_t चरमः = *अन्तः;
   यावत् (first <= चरमः) {
       int32_t mid = (first + चरमः) / 2;  // compute mid point.
       यदि (सूचकांक_अन्तर्_Currency_नामानि >= currency_नामानि[mid].currencyNameLen) {
           first = mid + 1;
       } अन्यथा {
           यदि (कुञ्चिका > currency_नामानि[mid].currencyName[सूचकांक_अन्तर्_Currency_नामानि]) {
               first = mid + 1;
           }
           अन्यथा यदि (कुञ्चिका < currency_नामानि[mid].currencyName[सूचकांक_अन्तर्_Currency_नामानि]) {
               चरमः = mid - 1;
           }
           अन्यथा {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#यदि_परिभाषितम् UCURR_DEBUG
                प्रारूप_लिख("mid = %d\n", mid);
#यदि_अन्तः
                यावत् (L < R) {
                    int32_t M = (L + R) / 2;
#यदि_परिभाषितम् UCURR_DEBUG
                    प्रारूप_लिख("L = %d, R = %d, M = %d\n", L, R, M);
#यदि_अन्तः
                    यदि (सूचकांक_अन्तर्_Currency_नामानि >= currency_नामानि[M].currencyNameLen) {
                        L = M + 1;
                    } अन्यथा {
                        यदि (currency_नामानि[M].currencyName[सूचकांक_अन्तर्_Currency_नामानि] < कुञ्चिका) {
                            L = M + 1;
                        } अन्यथा {
#यदि_परिभाषितम् UCURR_DEBUG
                            U_ASSERT(currency_नामानि[M].currencyName[सूचकांक_अन्तर्_Currency_नामानि] == कुञ्चिका);
#यदि_अन्तः
                            R = M;
                        }
                    }
                }
#यदि_परिभाषितम् UCURR_DEBUG
                U_ASSERT(L == R);
#यदि_अन्तः
                *begin = L;
#यदि_परिभाषितम् UCURR_DEBUG
                प्रारूप_लिख("begin = %d\n", *begin);
                U_ASSERT(currency_नामानि[*begin].currencyName[सूचकांक_अन्तर्_Currency_नामानि] == कुञ्चिका);
#यदि_अन्तः

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *अन्तः;
                यावत् (L < R) {
                    int32_t M = (L + R) / 2;
#यदि_परिभाषितम् UCURR_DEBUG
                    प्रारूप_लिख("L = %d, R = %d, M = %d\n", L, R, M);
#यदि_अन्तः
                    यदि (currency_नामानि[M].currencyNameLen < सूचकांक_अन्तर्_Currency_नामानि) {
                        L = M + 1;
                    } अन्यथा {
                        यदि (currency_नामानि[M].currencyName[सूचकांक_अन्तर्_Currency_नामानि] > कुञ्चिका) {
                            R = M;
                        } अन्यथा {
#यदि_परिभाषितम् UCURR_DEBUG
                            U_ASSERT(currency_नामानि[M].currencyName[सूचकांक_अन्तर्_Currency_नामानि] == कुञ्चिका);
#यदि_अन्तः
                            L = M + 1;
                        }
                    }
                }
#यदि_परिभाषितम् UCURR_DEBUG
                U_ASSERT(L == R);
#यदि_अन्तः
                यदि (currency_नामानि[R].currencyName[सूचकांक_अन्तर्_Currency_नामानि] > कुञ्चिका) {
                    *अन्तः = R - 1;
                } अन्यथा {
                    *अन्तः = R;
                }
#यदि_परिभाषितम् UCURR_DEBUG
                प्रारूप_लिख("end = %d\n", *अन्तः);
#यदि_अन्तः

                // now, found the range. check whether there is exact match
                यदि (currency_नामानि[*begin].currencyNameLen == सूचकांक_अन्तर्_Currency_नामानि + 1) {
                    प्रत्यागम *begin;  // find range and exact match.
                }
                प्रत्यागम -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *अन्तः = -1;
   प्रत्यागम -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
स्थिर शून्य
रैखिकम्_अन्वेषणम्(ध्रुव CurrencyNameStruct* currency_नामानि, 
             int32_t begin, int32_t अन्तः,
             ध्रुव char16_t* text, int32_t textLen,
             int32_t *partial_मेलनम्_Len,
             int32_t *max_मेलनम्_Len, int32_t* max_मेलनम्_सूचकांक) {
    int32_t initial_Partial_मेलनम्_Len = *partial_मेलनम्_Len;
    प्रति (int32_t सूचकांक = begin; सूचकांक <= अन्तः; ++सूचकांक) {
        int32_t len = currency_नामानि[सूचकांक].currencyNameLen;
        यदि (len > *max_मेलनम्_Len && len <= textLen &&
            uprv_memcmp(currency_नामानि[सूचकांक].currencyName, text, len * परिमाण(char16_t)) == 0) {
            *partial_मेलनम्_Len = MAX(*partial_मेलनम्_Len, len);
            *max_मेलनम्_सूचकांक = सूचकांक;
            *max_मेलनम्_Len = len;
#यदि_परिभाषितम् UCURR_DEBUG
            प्रारूप_लिख("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_मेलनम्_सूचकांक, *max_मेलनम्_Len);
#यदि_अन्तः
        } अन्यथा {
            // Check for partial matches.
            प्रति (int32_t i=initial_Partial_मेलनम्_Len; i<MIN(len, textLen); i++) {
                यदि (currency_नामानि[सूचकांक].currencyName[i] != text[i]) {
                    भङ्ग;
                }
                *partial_मेलनम्_Len = MAX(*partial_मेलनम्_Len, i + 1);
            }
        }
    }
}

#परिभाषय रैखिकम्_अन्वेषणम्_देहली 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
स्थिर शून्य
अन्वेषणम्_Currency_Name(ध्रुव CurrencyNameStruct* currency_नामानि, 
                   int32_t योग_currency_गणनम्,
                   ध्रुव char16_t* text, int32_t textLen,
                   int32_t *partial_मेलनम्_Len,
                   int32_t* max_मेलनम्_Len, int32_t* max_मेलनम्_सूचकांक) {
    *max_मेलनम्_सूचकांक = -1;
    *max_मेलनम्_Len = 0;
    int32_t मेलनम्_सूचकांक = -1;
    int32_t द्विमान_अन्वेषणम्_Begin = 0;
    int32_t द्विमान_अन्वेषणम्_अन्तः = योग_currency_गणनम् - 1;
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
    प्रति (int32_t सूचकांक = 0; सूचकांक < textLen; ++सूचकांक) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        मेलनम्_सूचकांक = द्विमान_अन्वेषणम्(currency_नामानि, सूचकांक,
                                  text[सूचकांक],
                                  &द्विमान_अन्वेषणम्_Begin, &द्विमान_अन्वेषणम्_अन्तः);
        यदि (द्विमान_अन्वेषणम्_Begin == -1) { // did not find the range
            भङ्ग;
        }
        *partial_मेलनम्_Len = MAX(*partial_मेलनम्_Len, सूचकांक + 1);
        यदि (मेलनम्_सूचकांक != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_मेलनम्_Len = सूचकांक + 1;
            *max_मेलनम्_सूचकांक = मेलनम्_सूचकांक;
        }
        यदि (द्विमान_अन्वेषणम्_अन्तः - द्विमान_अन्वेषणम्_Begin < रैखिकम्_अन्वेषणम्_देहली) {
            // linear search if within threshold.
            रैखिकम्_अन्वेषणम्(currency_नामानि, द्विमान_अन्वेषणम्_Begin, द्विमान_अन्वेषणम्_अन्तः,
                         text, textLen,
                         partial_मेलनम्_Len,
                         max_मेलनम्_Len, max_मेलनम्_सूचकांक);
            भङ्ग;
        }
    }
}

//========================= currency name cache =====================
प्रकारनिर्देश संरचना {
    वर्ण locale[ULOC_FULLNAME_क्षमता];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_नामानि;  //मानम्
    int32_t योग_Currency_Name_गणनम्;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //मानम्
    int32_t योग_Currency_चिह्नम्_गणनम्;  //गणनम्
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_गणनम्;
} Currency_Name_Cache_प्रविष्टिः;


#परिभाषय CURRENCY_NAME_CACHE_अङ्कः 10

// Reserve 10 cache entries.
स्थिर Currency_Name_Cache_प्रविष्टिः* currCache[CURRENCY_NAME_CACHE_अङ्कः] = {शून्यसूचकः};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
स्थिर int8_t current_Cache_प्रविष्टिः_सूचकांक = 0;

स्थिर UMutex gCurrencyCacheMutex;

// Cache deletion
स्थिर शून्य
delete_Currency_नामानि(CurrencyNameStruct* currency_नामानि, int32_t count) {
    प्रति (int32_t सूचकांक = 0; सूचकांक < count; ++सूचकांक) {
        यदि ( (currency_नामानि[सूचकांक].flag & NEED_गन्तव्यम्_BE_DELETED) ) {
            uprv_free(currency_नामानि[सूचकांक].currencyName);
        }
    }
    uprv_free(currency_नामानि);
}


स्थिर शून्य
delete_Cache_प्रविष्टिः(Currency_Name_Cache_प्रविष्टिः* प्रविष्टिः) {
    delete_Currency_नामानि(प्रविष्टिः->currency_नामानि, प्रविष्टिः->योग_Currency_Name_गणनम्);
    delete_Currency_नामानि(प्रविष्टिः->currencySymbols, प्रविष्टिः->योग_Currency_चिह्नम्_गणनम्);
    uprv_free(प्रविष्टिः);
}


// Cache clean up
स्थिर UBool U_CALLCONV
currency_cache_cleanup() {
    प्रति (int32_t i = 0; i < CURRENCY_NAME_CACHE_अङ्कः; ++i) {
        यदि (currCache[i]) {
            delete_Cache_प्रविष्टिः(currCache[i]);
            currCache[i] = शून्यसूचकः;
        }
    }
    प्रत्यागम सत्य;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
स्थिर Currency_Name_Cache_प्रविष्टिः*
get_Cache_प्रविष्टिः(ध्रुव वर्ण* locale, UError_संहिता& ec) {

    int32_t योग_currency_name_गणनम् = 0;
    CurrencyNameStruct* currency_नामानि = शून्यसूचकः;
    int32_t योग_currency_चिह्नम्_गणनम् = 0;
    CurrencyNameStruct* currencySymbols = शून्यसूचकः;
    Currency_Name_Cache_प्रविष्टिः* cache_प्रविष्टिः = शून्यसूचकः;

    umtx_तालकम्(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t प्राप्तम् = -1;
    प्रति (int8_t i = 0; i < CURRENCY_NAME_CACHE_अङ्कः; ++i) {
        यदि (currCache[i]!= शून्यसूचकः &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            प्राप्तम् = i;
            भङ्ग;
        }
    }
    यदि (प्राप्तम् != -1) {
        cache_प्रविष्टिः = currCache[प्राप्तम्];
        ++(cache_प्रविष्टिः->ref_गणनम्);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    यदि (प्राप्तम् == -1) {
        collect_Currency_नामानि(locale, &currency_नामानि, &योग_currency_name_गणनम्, &currencySymbols, &योग_currency_चिह्नम्_गणनम्, ec);
        यदि (U_FAILURE(ec)) {
            प्रत्यागम शून्यसूचकः;
        }
        umtx_तालकम्(&gCurrencyCacheMutex);
        // check again.
        प्रति (int8_t i = 0; i < CURRENCY_NAME_CACHE_अङ्कः; ++i) {
            यदि (currCache[i]!= शून्यसूचकः &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                प्राप्तम् = i;
                भङ्ग;
            }
        }
        यदि (प्राप्तम् == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_प्रविष्टिः = currCache[current_Cache_प्रविष्टिः_सूचकांक];
            यदि (cache_प्रविष्टिः) {
                --(cache_प्रविष्टिः->ref_गणनम्);
                // delete if the ref count is zero
                यदि (cache_प्रविष्टिः->ref_गणनम् == 0) {
                    delete_Cache_प्रविष्टिः(cache_प्रविष्टिः);
                }
            }
            cache_प्रविष्टिः = static_cast<Currency_Name_Cache_प्रविष्टिः*>(uprv_malloc(परिमाण(Currency_Name_Cache_प्रविष्टिः)));
            यदि (cache_प्रविष्टिः == शून्यसूचकः) {
                delete_Currency_नामानि(currency_नामानि, योग_currency_name_गणनम्);
                delete_Currency_नामानि(currencySymbols, योग_currency_चिह्नम्_गणनम्);
                ec = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम शून्यसूचकः;
            }
            currCache[current_Cache_प्रविष्टिः_सूचकांक] = cache_प्रविष्टिः;
            uprv_strcpy(cache_प्रविष्टिः->locale, locale);
            cache_प्रविष्टिः->currency_नामानि = currency_नामानि;
            cache_प्रविष्टिः->योग_Currency_Name_गणनम् = योग_currency_name_गणनम्;
            cache_प्रविष्टिः->currencySymbols = currencySymbols;
            cache_प्रविष्टिः->योग_Currency_चिह्नम्_गणनम् = योग_currency_चिह्नम्_गणनम्;
            cache_प्रविष्टिः->ref_गणनम् = 2; // one for cache, one for reference
            current_Cache_प्रविष्टिः_सूचकांक = (current_Cache_प्रविष्टिः_सूचकांक + 1) % CURRENCY_NAME_CACHE_अङ्कः;
            ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } अन्यथा {
            delete_Currency_नामानि(currency_नामानि, योग_currency_name_गणनम्);
            delete_Currency_नामानि(currencySymbols, योग_currency_चिह्नम्_गणनम्);
            cache_प्रविष्टिः = currCache[प्राप्तम्];
            ++(cache_प्रविष्टिः->ref_गणनम्);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    प्रत्यागम cache_प्रविष्टिः;
}

स्थिर शून्य release_Cache_प्रविष्टिः(Currency_Name_Cache_प्रविष्टिः* cache_प्रविष्टिः) {
    umtx_तालकम्(&gCurrencyCacheMutex);
    --(cache_प्रविष्टिः->ref_गणनम्);
    यदि (cache_प्रविष्टिः->ref_गणनम् == 0) {  // remove
        delete_Cache_प्रविष्टिः(cache_प्रविष्टिः);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI शून्य
uprv_parseCurrency(ध्रुव वर्ण* locale,
                   ध्रुव icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_मेलनम्_Len,
                   char16_t* परिणाम,
                   UError_संहिता& ec) {
    U_NAMESPACE_USE
    यदि (U_FAILURE(ec)) {
        प्रत्यागम;
    }
    Currency_Name_Cache_प्रविष्टिः* cache_प्रविष्टिः = get_Cache_प्रविष्टिः(locale, ec);
    यदि (U_FAILURE(ec)) {
        प्रत्यागम;
    }

    int32_t योग_currency_name_गणनम् = cache_प्रविष्टिः->योग_Currency_Name_गणनम्;
    CurrencyNameStruct* currency_नामानि = cache_प्रविष्टिः->currency_नामानि;
    int32_t योग_currency_चिह्नम्_गणनम् = cache_प्रविष्टिः->योग_Currency_चिह्नम्_गणनम्;
    CurrencyNameStruct* currencySymbols = cache_प्रविष्टिः->currencySymbols;

    int32_t start = pos.get_सूचकांक();

    char16_t निवेशः_Text[MAX_CURRENCY_NAME_LEN];
    char16_t ऊर्ध्वस्थम्_Text[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, निवेशः_Text);
    UError_संहिता ec1 = U_ZERO_ERROR;
    textLen = u_str_गन्तव्यम्_ऊर्ध्वस्थम्(ऊर्ध्वस्थम्_Text, MAX_CURRENCY_NAME_LEN, निवेशः_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_मेलनम्_Len = 0;

    int32_t max = 0;
    int32_t मेलनम्_सूचकांक = -1;
    // case in-sensitive comparison against currency names
    अन्वेषणम्_Currency_Name(currency_नामानि, योग_currency_name_गणनम्, 
                       ऊर्ध्वस्थम्_Text, textLen, partial_मेलनम्_Len, &max, &मेलनम्_सूचकांक);

#यदि_परिभाषितम् UCURR_DEBUG
    प्रारूप_लिख("search in names, max = %d, matchIndex = %d\n", max, मेलनम्_सूचकांक);
#यदि_अन्तः

    int32_t max_अन्तर्_चिह्नम् = 0;
    int32_t मेलनम्_सूचकांक_अन्तर्_चिह्नम् = -1;
    यदि (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        अन्वेषणम्_Currency_Name(currencySymbols, योग_currency_चिह्नम्_गणनम्, 
                           निवेशः_Text, textLen,
                           partial_मेलनम्_Len,
                           &max_अन्तर्_चिह्नम्, &मेलनम्_सूचकांक_अन्तर्_चिह्नम्);
    }

#यदि_परिभाषितम् UCURR_DEBUG
    प्रारूप_लिख("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_अन्तर्_चिह्नम्, मेलनम्_सूचकांक_अन्तर्_चिह्नम्);
    यदि(मेलनम्_सूचकांक_अन्तर्_चिह्नम् != -1) {
      प्रारूप_लिख("== ISO=%s\n", currencySymbols[मेलनम्_सूचकांक_अन्तर्_चिह्नम्].Iso_संहिता);
    }
#यदि_अन्तः

    यदि (max >= max_अन्तर्_चिह्नम् && मेलनम्_सूचकांक != -1) {
        u_chars_गन्तव्यम्_UChars(currency_नामानि[मेलनम्_सूचकांक].Iso_संहिता, परिणाम, 4);
        pos.समुच्चयः_सूचकांक(start + max);
    } अन्यथा यदि (max_अन्तर्_चिह्नम् >= max && मेलनम्_सूचकांक_अन्तर्_चिह्नम् != -1) {
        u_chars_गन्तव्यम्_UChars(currencySymbols[मेलनम्_सूचकांक_अन्तर्_चिह्नम्].Iso_संहिता, परिणाम, 4);
        pos.समुच्चयः_सूचकांक(start + max_अन्तर्_चिह्नम्);
    }

    // decrease reference count
    release_Cache_प्रविष्टिः(cache_प्रविष्टिः);
}

शून्य uprv_currencyLeads(ध्रुव वर्ण* locale, icu::Unicode_समुच्चयः& परिणाम, UError_संहिता& ec) {
    U_NAMESPACE_USE
    यदि (U_FAILURE(ec)) {
        प्रत्यागम;
    }
    Currency_Name_Cache_प्रविष्टिः* cache_प्रविष्टिः = get_Cache_प्रविष्टिः(locale, ec);
    यदि (U_FAILURE(ec)) {
        प्रत्यागम;
    }

    प्रति (int32_t i=0; i<cache_प्रविष्टिः->योग_Currency_चिह्नम्_गणनम्; i++) {
        ध्रुव CurrencyNameStruct& info = cache_प्रविष्टिः->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        परिणाम.सङ्कलय(cp);
    }

    प्रति (int32_t i=0; i<cache_प्रविष्टिः->योग_Currency_Name_गणनम्; i++) {
        ध्रुव CurrencyNameStruct& info = cache_प्रविष्टिः->currency_नामानि[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        परिणाम.सङ्कलय(cp);
    }

    // decrease reference count
    release_Cache_प्रविष्टिः(cache_प्रविष्टिः);
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
U_CAPI शून्य
uprv_getStaticCurrencyName(ध्रुव char16_t* iso, ध्रुव वर्ण* loc,
                           icu::UnicodeString& परिणाम, UError_संहिता& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ध्रुव char16_t* currname = ucurr_getName(iso, loc, UCURR_चिह्नम्_NAME,
                                          शून्यसूचकः /* isChoiceFormat */, &len, &ec);
    यदि (U_SUCCESS(ec)) {
        परिणाम.समुच्चयः_गन्तव्यम्(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ध्रुव char16_t* currency, UError_संहिता* ec) {
    प्रत्यागम ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ध्रुव char16_t* currency, ध्रुव UCurrencyUsage usage, UError_संहिता* ec) {
    int32_t fracDigits = 0;
    यदि (U_SUCCESS(*ec)) {
        विकल्प (usage) {
            पक्ष UCURR_USAGE_STANDARD:
                fracDigits = (find_Meta_दत्त(currency, *ec))[0];
                भङ्ग;
            पक्ष UCURR_USAGE_CASH:
                fracDigits = (find_Meta_दत्त(currency, *ec))[2];
                भङ्ग;
            स्वभाव:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    प्रत्यागम fracDigits;
}

U_CAPI द्विगुण U_EXPORT2
ucurr_getRoundingIncrement(ध्रुव char16_t* currency, UError_संहिता* ec) {
    प्रत्यागम ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI द्विगुण U_EXPORT2
ucurr_getRoundingIncrementForUsage(ध्रुव char16_t* currency, ध्रुव UCurrencyUsage usage, UError_संहिता* ec) {
    द्विगुण परिणाम = 0.0;

    ध्रुव int32_t *दत्त = find_Meta_दत्त(currency, *ec);
    यदि (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        विकल्प (usage) {
            पक्ष UCURR_USAGE_STANDARD:
                fracDigits = दत्त[0];
                increment = दत्त[1];
                भङ्ग;
            पक्ष UCURR_USAGE_CASH:
                fracDigits = दत्त[2];
                increment = दत्त[3];
                भङ्ग;
            स्वभाव:
                *ec = U_UNSUPPORTED_ERROR;
                प्रत्यागम परिणाम;
        }

        // If the meta data is invalid, return 0.0
        यदि (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } अन्यथा {
            // A rounding value of 0 or 1 indicates no rounding.
            यदि (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                परिणाम = द्विगुण(increment) / POW10[fracDigits];
            }
        }
    }

    प्रत्यागम परिणाम;
}

U_CDECL_BEGIN

प्रकारनिर्देश संरचना UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
स्थिर ध्रुव संरचना CurrencyList {
    ध्रुव वर्ण *currency;
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
    { शून्यसूचकः, 0 } // Leave here to denote the end of the list.
};

#परिभाषय UCURR_MATCHES_BITMASK(variable, type_गन्तव्यम्_मेलनम्) \
    ((type_गन्तव्यम्_मेलनम्) == UCURR_ALL || ((variable) & (type_गन्तव्यम्_मेलनम्)) == (type_गन्तव्यम्_मेलनम्))

स्थिर int32_t U_CALLCONV
ucurr_गणनम्_Currency_List(UEnumeration *enumerator, UError_संहिता * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    प्रति (int32_t idx = 0; gCurrencyList[idx].currency != शून्यसूचकः; idx++) {
        यदि (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    प्रत्यागम count;
}

स्थिर ध्रुव वर्ण* U_CALLCONV
ucurr_अग्र_Currency_List(UEnumeration *enumerator,
                        int32_t* परिणाम_दीर्घता,
                        UError_संहिता * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    यावत् (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ध्रुव संरचना CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        यदि (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            यदि (परिणाम_दीर्घता) {
                *परिणाम_दीर्घता = 3; /* Currency codes are only 3 chars long */
            }
            प्रत्यागम currItem->currency;
        }
    }
    /* We enumerated too far. */
    यदि (परिणाम_दीर्घता) {
        *परिणाम_दीर्घता = 0;
    }
    प्रत्यागम शून्यसूचकः;
}

स्थिर शून्य U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_संहिता * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

स्थिर शून्य U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

स्थिर शून्य U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_संहिता* स्थितिः){
    UError_संहिता local_स्थितिः = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_प्रत्यक्षम्(U_ICUDATA_CURR, CURRENCY_दत्त, &local_स्थितिः);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_कुञ्चिका(rb, CURRENCY_MAP, rb, &local_स्थितिः));

    यदि (U_SUCCESS(local_स्थितिः)) {
        // process each entry in currency map
        प्रति (int32_t i=0; i<ures_get_आकार(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_get_By_सूचकांक(currencyMapArray.getAlias(), i, शून्यसूचकः, &local_स्थितिः));
            // process each currency
            यदि (U_SUCCESS(local_स्थितिः)) {
                प्रति (int32_t j=0; j<ures_get_आकार(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_get_By_सूचकांक(currencyArray.getAlias(), j, शून्यसूचकः, &local_स्थितिः));
                    Iso_संहिता_प्रविष्टिः *प्रविष्टिः = (Iso_संहिता_प्रविष्टिः*)uprv_malloc(परिमाण(Iso_संहिता_प्रविष्टिः));
                    यदि (प्रविष्टिः == शून्यसूचकः) {
                        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                        प्रत्यागम;
                    }

                    // get the ISO code
                    int32_t iso_दीर्घता = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_कुञ्चिका(currencyRes.getAlias(), "id", शून्यसूचकः, &local_स्थितिः));
                    यदि (idRes.isNull()) {
                        अनुवर्त;
                    }
                    ध्रुव char16_t *iso_संहिता = ures_getString(idRes.getAlias(), &iso_दीर्घता, &local_स्थितिः);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_कुञ्चिका(currencyRes.getAlias(), "from", शून्यसूचकः, &local_स्थितिः));

                    यदि (U_SUCCESS(local_स्थितिः)) {
                        int32_t from_दीर्घता = 0;
                        ध्रुव int32_t *fromArray = ures_get_Int_सदिशम्(fromRes.getAlias(), &from_दीर्घता, &local_स्थितिः);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate गन्तव्यम्_Date = U_DATE_MAX;
                    local_स्थितिः = U_ZERO_ERROR;
                    LocalUResourceBundlePointer गन्तव्यम्_Res(ures_get_By_कुञ्चिका(currencyRes.getAlias(), "to", शून्यसूचकः, &local_स्थितिः));

                    यदि (U_SUCCESS(local_स्थितिः)) {
                        int32_t गन्तव्यम्_दीर्घता = 0;
                        ध्रुव int32_t *गन्तव्यम्_Array = ures_get_Int_सदिशम्(गन्तव्यम्_Res.getAlias(), &गन्तव्यम्_दीर्घता, &local_स्थितिः);
                        int64_t currDate64 = (uint64_t)गन्तव्यम्_Array[0] << 32;
                        currDate64 |= ((int64_t)गन्तव्यम्_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        गन्तव्यम्_Date = (UDate)currDate64;
                    }

                    प्रविष्टिः->iso_संहिता = iso_संहिता;
                    प्रविष्टिः->from = fromDate;
                    प्रविष्टिः->गन्तव्यम् = गन्तव्यम्_Date;

                    local_स्थितिः = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_संहिता, प्रविष्टिः, &local_स्थितिः);
                }
            } अन्यथा {
                *स्थितिः = local_स्थितिः;
            }
        }
    } अन्यथा {
        *स्थितिः = local_स्थितिः;
    }
}

स्थिर ध्रुव UEnumeration gEnumCurrencyList = {
    शून्यसूचकः,
    शून्यसूचकः,
    ucurr_closeCurrencyList,
    ucurr_गणनम्_Currency_List,
    uenum_unextDefault,
    ucurr_अग्र_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_अन्तः


स्थिर शून्य U_CALLCONV initIsoCodes(UError_संहिता &स्थितिः) {
    U_ASSERT(gIsoCodes == शून्यसूचकः);
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, शून्यसूचकः, &स्थितिः));
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    uhash_समुच्चयः_मानम्_Deleter(isoCodes.getAlias(), delete_Iso_संहिता_प्रविष्टिः);

    ucurr_createCurrencyList(isoCodes.getAlias(), &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

स्थिर शून्य populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    प्रति (ध्रुव स्वचालित& प्रविष्टिः : unisets::kCurrencyEntries) {
        UnicodeString exemplar(प्रविष्टिः.exemplar);
        ध्रुव Unicode_समुच्चयः* समुच्चयः = unisets::get(प्रविष्टिः.कुञ्चिका);
        यदि (समुच्चयः == शून्यसूचकः) { प्रत्यागम; }
        Unicode_समुच्चयः_Iterator it(*समुच्चयः);
        यावत् (it.next()) {
            UnicodeString value = it.getString();
            यदि (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                अनुवर्त;
            }
            makeEquivalent(exemplar, value, hash, स्थितिः);
            यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        }
    }
}

स्थिर शून्य U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == शून्यसूचकः);
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *अस्थायी = नूतन icu::Hashtable(स्थितिः);
    यदि (अस्थायी == शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि (U_FAILURE(स्थितिः)) {
        विलोपय अस्थायी;
        प्रत्यागम;
    }
    अस्थायी->समुच्चयः_मानम्_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(अस्थायी, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        विलोपय अस्थायी;
        प्रत्यागम;
    }
    gCurrSymbolsEquiv = अस्थायी;
}

U_CAPI UBool U_EXPORT2
ucurr_is_उपलब्धम्(ध्रुव char16_t* iso_संहिता, UDate from, UDate गन्तव्यम्, UError_संहिता* e_Error_संहिता) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_संहिता);
    यदि (U_FAILURE(*e_Error_संहिता)) {
        प्रत्यागम असत्य;
    }

    Iso_संहिता_प्रविष्टिः* परिणाम = (Iso_संहिता_प्रविष्टिः *) uhash_get(gIsoCodes, iso_संहिता);
    यदि (परिणाम == शून्यसूचकः) {
        प्रत्यागम असत्य;
    } अन्यथा यदि (from > गन्तव्यम्) {
        *e_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    } अन्यथा यदि  ((from > परिणाम->गन्तव्यम्) || (गन्तव्यम् < परिणाम->from)) {
        प्रत्यागम असत्य;
    }
    प्रत्यागम सत्य;
}

स्थिर ध्रुव icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    प्रत्यागम gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_संहिता *p_Error_संहिता) {
    UEnumeration *myEnum = शून्यसूचकः;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(परिमाण(UEnumeration));
    यदि (myEnum == शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, परिमाण(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(परिमाण(UCurrencyContext));
    यदि (myContext == शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        uprv_free(myEnum);
        प्रत्यागम शून्यसूचकः;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    प्रत्यागम myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_गणनम्_Currencies(ध्रुव वर्ण* locale, 
                 UDate date, 
                 UError_संहिता* ec)
{
    int32_t curr_गणनम् = 0;

    यदि (ec != शून्यसूचकः && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_संहिता local_स्थितिः = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        यदि (U_FAILURE(*ec))
        {
            प्रत्यागम 0;
        }

        // Remove variants, which is only needed for registration.
        वर्ण *idDelim = strchr(id.दत्त(), VAR_DELIM);
        यदि (idDelim)
        {
            id.truncate(idDelim - id.दत्त());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_open_प्रत्यक्षम्(U_ICUDATA_CURR, CURRENCY_दत्त, &local_स्थितिः);
        UResourceBundle *cm = ures_get_By_कुञ्चिका(rb, CURRENCY_MAP, rb, &local_स्थितिः);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_कुञ्चिका(rb, id.दत्त(), cm, &local_स्थितिः));

        // process each currency to see which one is valid for the given date
        यदि (U_SUCCESS(local_स्थितिः))
        {
            प्रति (int32_t i=0; i<ures_get_आकार(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_get_By_सूचकांक(countryArray.getAlias(), i, शून्यसूचकः, &local_स्थितिः));

                // get the from date
                int32_t from_दीर्घता = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_कुञ्चिका(currencyRes.getAlias(), "from", शून्यसूचकः, &local_स्थितिः));
                ध्रुव int32_t *fromArray = ures_get_Int_सदिशम्(fromRes.getAlias(), &from_दीर्घता, &local_स्थितिः);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                यदि (ures_get_आकार(currencyRes.getAlias())> 2)
                {
                    int32_t गन्तव्यम्_दीर्घता = 0;
                    LocalUResourceBundlePointer गन्तव्यम्_Res(ures_get_By_कुञ्चिका(currencyRes.getAlias(), "to", शून्यसूचकः, &local_स्थितिः));
                    ध्रुव int32_t *गन्तव्यम्_Array = ures_get_Int_सदिशम्(गन्तव्यम्_Res.getAlias(), &गन्तव्यम्_दीर्घता, &local_स्थितिः);

                    currDate64 = (int64_t)गन्तव्यम्_Array[0] << 32;
                    currDate64 |= ((int64_t)गन्तव्यम्_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate गन्तव्यम्_Date = (UDate)currDate64;

                    यदि ((fromDate <= date) && (date < गन्तव्यम्_Date))
                    {
                        curr_गणनम्++;
                    }
                }
                अन्यथा
                {
                    यदि (fromDate <= date)
                    {
                        curr_गणनम्++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        यदि (*ec == U_ZERO_ERROR || local_स्थितिः != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = local_स्थितिः;
        }

        यदि (U_SUCCESS(*ec))
        {
            // no errors
            प्रत्यागम curr_गणनम्;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    प्रत्यागम 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ध्रुव वर्ण* locale, 
                UDate date, 
                int32_t सूचकांक,
                char16_t* buff,
                int32_t buff_क्षमता, 
                UError_संहिता* ec)
{
    int32_t resLen = 0;
	int32_t curr_सूचकांक = 0;
    ध्रुव char16_t* s = शून्यसूचकः;

    यदि (ec != शून्यसूचकः && U_SUCCESS(*ec))
    {
        // check the arguments passed
        यदि ((buff && buff_क्षमता) || !buff_क्षमता )
        {
            // local variables
            UError_संहिता local_स्थितिः = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            यदि (U_FAILURE(*ec))
            {
                प्रत्यागम 0;
            }

            // Remove variants, which is only needed for registration.
            वर्ण *idDelim = strchr(id.दत्त(), VAR_DELIM);
            यदि (idDelim)
            {
                id.truncate(idDelim - id.दत्त());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_open_प्रत्यक्षम्(U_ICUDATA_CURR, CURRENCY_दत्त, &local_स्थितिः);
            UResourceBundle *cm = ures_get_By_कुञ्चिका(rb, CURRENCY_MAP, rb, &local_स्थितिः);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_कुञ्चिका(rb, id.दत्त(), cm, &local_स्थितिः));

            // process each currency to see which one is valid for the given date
            तार्किक मेलनम्_प्राप्तम् = असत्य;
            यदि (U_SUCCESS(local_स्थितिः))
            {
                यदि ((सूचकांक <= 0) || (सूचकांक> ures_get_आकार(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    प्रत्यागम 0;
                }

                प्रति (int32_t i=0; i<ures_get_आकार(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_get_By_सूचकांक(countryArray.getAlias(), i, शून्यसूचकः, &local_स्थितिः));
                    s = ures_get_String_By_कुञ्चिका(currencyRes.getAlias(), "id", &resLen, &local_स्थितिः);

                    // get the from date
                    int32_t from_दीर्घता = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_कुञ्चिका(currencyRes.getAlias(), "from", शून्यसूचकः, &local_स्थितिः));
                    ध्रुव int32_t *fromArray = ures_get_Int_सदिशम्(fromRes.getAlias(), &from_दीर्घता, &local_स्थितिः);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    यदि (ures_get_आकार(currencyRes.getAlias()) > 2)
                    {
                        int32_t गन्तव्यम्_दीर्घता = 0;
                        LocalUResourceBundlePointer गन्तव्यम्_Res(ures_get_By_कुञ्चिका(currencyRes.getAlias(), "to", शून्यसूचकः, &local_स्थितिः));
                        ध्रुव int32_t *गन्तव्यम्_Array = ures_get_Int_सदिशम्(गन्तव्यम्_Res.getAlias(), &गन्तव्यम्_दीर्घता, &local_स्थितिः);

                        currDate64 = (int64_t)गन्तव्यम्_Array[0] << 32;
                        currDate64 |= ((int64_t)गन्तव्यम्_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate गन्तव्यम्_Date = (UDate)currDate64;

                        यदि ((fromDate <= date) && (date < गन्तव्यम्_Date))
                        {
                            curr_सूचकांक++;
                            यदि (curr_सूचकांक == सूचकांक)
                            {
                                मेलनम्_प्राप्तम् = सत्य;
                            }
                        }
                    }
                    अन्यथा
                    {
                        यदि (fromDate <= date)
                        {
                            curr_सूचकांक++;
                            यदि (curr_सूचकांक == सूचकांक)
                            {
                                मेलनम्_प्राप्तम् = सत्य;
                            }
                        }
                    }
                    // check for loop exit
                    यदि (मेलनम्_प्राप्तम्)
                    {
                        भङ्ग;
                    }

                } // end For loop
            }

            // Check for errors
            यदि (*ec == U_ZERO_ERROR || local_स्थितिः != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = local_स्थितिः;
            }

            यदि (U_SUCCESS(*ec))
            {
                // no errors
                यदि((buff_क्षमता> resLen) && मेलनम्_प्राप्तम्)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                अन्यथा
                {
                    प्रत्यागम 0;
                }
            }

            // return null terminated currency string
            प्रत्यागम u_terminateUChars(buff, buff_क्षमता, resLen, ec);
        }
        अन्यथा
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_तर्कः_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    प्रत्यागम resLen;
}

स्थिर ध्रुव UEnumeration default_Keyword_मानानि = {
    शून्यसूचकः,
    शून्यसूचकः,
    ulist_close_keyword_मानानि_iterator,
    ulist_गणनम्_keyword_मानानि,
    uenum_unextDefault,
    ulist_अग्र_keyword_मानम्, 
    ulist_reset_keyword_मानानि_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_मानानि_For_Locale(ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *locale, UBool commonly_प्रयुक्तम्, UError_संहिता* स्थितिः) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_दत्त(locale, सत्य, *स्थितिः);

    // Read value from supplementalData
    UList *मानानि = ulist_create_रिक्तानि_List(स्थितिः);
    UList *other_मानानि = ulist_create_रिक्तानि_List(स्थितिः);
    UEnumeration *en = (UEnumeration *)uprv_malloc(परिमाण(UEnumeration));
    यदि (U_FAILURE(*स्थितिः) || en == शून्यसूचकः) {
        यदि (en == शून्यसूचकः) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            uprv_free(en);
        }
        ulist_deleteList(मानानि);
        ulist_deleteList(other_मानानि);
        प्रत्यागम शून्यसूचकः;
    }
    स्मृतिं_प्रतिलिख(en, &default_Keyword_मानानि, परिमाण(UEnumeration));
    en->context = मानानि;
    
    UResourceBundle* rb = ures_open_प्रत्यक्षम्(U_ICUDATA_CURR, "supplementalData", स्थितिः);
    LocalUResourceBundlePointer bundle(ures_get_By_कुञ्चिका(rb, "CurrencyMap", rb, स्थितिः));
    राशिः_UResource_Bundle bundlekey, regbndl, curbndl, गन्तव्यम्;
    
    यावत् (U_SUCCESS(*स्थितिः) && ures_has_अग्र(bundle.getAlias())) {
        ures_get_अग्र_Resource(bundle.getAlias(), bundlekey.getAlias(), स्थितिः);
        यदि (U_FAILURE(*स्थितिः)) {
            भङ्ग;
        }
        ध्रुव वर्ण *region = ures_get_कुञ्चिका(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        यदि (!isPrefRegion && commonly_प्रयुक्तम्) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            अनुवर्त;
        }
        ures_get_By_कुञ्चिका(bundle.getAlias(), region, regbndl.getAlias(), स्थितिः);
        यदि (U_FAILURE(*स्थितिः)) {
            भङ्ग;
        }
        यावत् (U_SUCCESS(*स्थितिः) && ures_has_अग्र(regbndl.getAlias())) {
            ures_get_अग्र_Resource(regbndl.getAlias(), curbndl.getAlias(), स्थितिः);
            यदि (ures_getType(curbndl.getAlias()) != URES_सारणी) {
                // Currently, an empty ARRAY is mixed in.
                अनुवर्त;
            }
            वर्ण *curID = (वर्ण *)uprv_malloc(परिमाण(वर्ण) * ULOC_KEYWORDS_क्षमता);
            यदि (curID == शून्यसूचकः) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                भङ्ग;
            }
            int32_t curIDLength = ULOC_KEYWORDS_क्षमता;

#यदि U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_कुञ्चिका(curbndl.getAlias(), "id", curID, &curIDLength, सत्य, स्थितिः);
            /* optimize - use the utf-8 string */
#अन्यथा
            {
                       ध्रुव char16_t* defString = ures_get_String_By_कुञ्चिका(curbndl.getAlias(), "id", &curIDLength, स्थितिः);
                       यदि(U_SUCCESS(*स्थितिः)) {
			   यदि(curIDLength+1 > ULOC_KEYWORDS_क्षमता) {
				*स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
			   } अन्यथा {
                           	u_UChars_गन्तव्यम्_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#यदि_अन्तः	

            यदि (U_FAILURE(*स्थितिः)) {
                भङ्ग;
            }
            UBool has_गन्तव्यम् = असत्य;
            ures_get_By_कुञ्चिका(curbndl.getAlias(), "to", गन्तव्यम्.getAlias(), स्थितिः);
            यदि (U_FAILURE(*स्थितिः)) {
                // Do nothing here...
                *स्थितिः = U_ZERO_ERROR;
            } अन्यथा {
                has_गन्तव्यम् = सत्य;
            }
            यदि (isPrefRegion && !has_गन्तव्यम् && !ulist_अन्तर्भवति_String(मानानि, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_सङ्कलय_Item_अन्तः_List(मानानि, curID, सत्य, स्थितिः);
            } अन्यथा यदि (!ulist_अन्तर्भवति_String(other_मानानि, curID, (int32_t)uprv_strlen(curID)) && !commonly_प्रयुक्तम्) {
                ulist_सङ्कलय_Item_अन्तः_List(other_मानानि, curID, सत्य, स्थितिः);
            } अन्यथा {
                uprv_free(curID);
            }
        }
        
    }
    यदि (U_SUCCESS(*स्थितिः)) {
        यदि (commonly_प्रयुक्तम्) {
            यदि (ulist_get_List_आकार(मानानि) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_मानानि_For_Locale(कुञ्चिका, "und", सत्य, स्थितिः);
            }
        } अन्यथा {
            // Consolidate the list
            वर्ण *value = शून्यसूचकः;
            ulist_resetList(other_मानानि);
            यावत् ((value = (वर्ण *)ulist_get_अग्र(other_मानानि)) != शून्यसूचकः) {
                यदि (!ulist_अन्तर्भवति_String(मानानि, value, (int32_t)uprv_strlen(value))) {
                    वर्ण *tmp_मानम् = (वर्ण *)uprv_malloc(परिमाण(वर्ण) * ULOC_KEYWORDS_क्षमता);
                    यदि (tmp_मानम् == शून्यसूचकः) {
                        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                        भङ्ग;
                    }
                    uprv_memcpy(tmp_मानम्, value, uprv_strlen(value) + 1);
                    ulist_सङ्कलय_Item_अन्तः_List(मानानि, tmp_मानम्, सत्य, स्थितिः);
                    यदि (U_FAILURE(*स्थितिः)) {
                        भङ्ग;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } अन्यथा {
        ulist_deleteList(मानानि);
        uprv_free(en);
        मानानि = शून्यसूचकः;
        en = शून्यसूचकः;
    }
    ulist_deleteList(other_मानानि);
    प्रत्यागम en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_संहिता(ध्रुव char16_t* currency) {
    int32_t संहिता = 0;
    यदि (currency && u_strlen(currency) == ISO_CURRENCY_संहिता_दीर्घता) {
        UError_संहिता स्थितिः = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_प्रत्यक्षम्(शून्यसूचकः, "currencyNumericCodes", &स्थितिः);
        LocalUResourceBundlePointer संहिता_Map(ures_get_By_कुञ्चिका(bundle, "codeMap", bundle, &स्थितिः));
        यदि (U_SUCCESS(स्थितिः)) {
            वर्ण alpha_संहिता[ISO_CURRENCY_संहिता_दीर्घता+1];
            my_UChars_गन्तव्यम्_Chars(alpha_संहिता, currency);
            T_CString_गन्तव्यम्_ऊर्ध्वस्थम्_Case(alpha_संहिता);
            ures_get_By_कुञ्चिका(संहिता_Map.getAlias(), alpha_संहिता, संहिता_Map.getAlias(), &स्थितिः);
            पूर्णाङ्क tmp_संहिता = ures_getInt(संहिता_Map.getAlias(), &स्थितिः);
            यदि (U_SUCCESS(स्थितिः)) {
                संहिता = tmp_संहिता;
            }
        }
    }
    प्रत्यागम संहिता;
}
#यदि_अन्तः /* #if !UCONFIG_NO_FORMATTING */

//eof
