// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1997-2016, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*
* File uresbund.cpp
*
* Modification History:
*
*   Date        Name        Description
*   04/01/97    aliu        Creation.
*   06/14/99    stephen     Removed functions taking a filename suffix.
*   07/20/99    stephen     Changed for UResourceBundle typedef'd to void*
*   11/09/99    weiv            Added ures_getLocale()
*   March 2000  weiv        Total overhaul - using data in DLLs
*   06/20/2000  helena      OS/400 port changes; mostly typecast.
*   06/24/02    weiv        Added support for resource sharing
******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cwchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uenum.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uenumimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresdata.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ਸਥਿਰ UHashtable *cache = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UInitOnce gCacheInitOnce {};

ਸਥਿਰ UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ਸਥਿਰ int32_t U_CALLCONV hash_ਇੰਦਰਾਜ(ਅਚਲ UHashTok parm) {
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* b = static_cast<UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ*>(parm.ਸੂਚਕ);
    UHashTok namekey, pathkey;
    namekey.ਸੂਚਕ = b->fName;
    pathkey.ਸੂਚਕ = b->f_ਰਸਤਾ;
    ਵਾਪਸ uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ਸਥਿਰ UBool U_CALLCONV compareEntries(ਅਚਲ UHashTok p1, ਅਚਲ UHashTok p2) {
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* b1 = static_cast<UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ*>(p1.ਸੂਚਕ);
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* b2 = static_cast<UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ*>(p2.ਸੂਚਕ);
    UHashTok name1, name2, path1, path2;
    name1.ਸੂਚਕ = b1->fName;
    name2.ਸੂਚਕ = b2->fName;
    path1.ਸੂਚਕ = b1->f_ਰਸਤਾ;
    path2.ਸੂਚਕ = b2->f_ਰਸਤਾ;
    ਵਾਪਸ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ਸਥਿਰ UBool chopLocale(ਅੱਖਰ *name) {
    ਅੱਖਰ *i = uprv_strrchr(name, '_');

    ਜੇ(i != ਖਾਲੀ_ਸੂਚਕ) {
        *i = '\0';
        ਵਾਪਸ ਸੱਚ;
    }

    ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ UBool hasVariant(ਅਚਲ ਅੱਖਰ* localeID) {
    UError_ਕੋਡ err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(ਖਾਲੀ_ਸੂਚਕ, 0);
    ulocimp_getSubtags(
            localeID,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            &sink,
            ਖਾਲੀ_ਸੂਚਕ,
            err);
    ਵਾਪਸ sink.ਸੰਖਿਆ_ਦਾ_ਬਾਈਟਾਂ_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INCLUDED_FROM_URESBUND_CPP
#ਅਨ੍ਤਰ੍ਭਾਵ "localefallback_data.h"

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* performFallbackLookup(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                         ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ_Strs,
                                         ਅਚਲ ਅੱਖਰ* ਮੁੱਲ_Strs U_LIFETIME_BOUND,
                                         ਅਚਲ int32_t* lookup_ਸਾਰਣੀ,
                                         int32_t lookup_ਸਾਰਣੀ_ਲੰਮਾਈ) {
    ਅਚਲ int32_t* bottom = lookup_ਸਾਰਣੀ;
    ਅਚਲ int32_t* ਸਿਖਰ = lookup_ਸਾਰਣੀ + lookup_ਸਾਰਣੀ_ਲੰਮਾਈ;

    ਜਦੋਂ (bottom < ਸਿਖਰ) {
        // Effectively, divide by 2 and round down to an even index
        ਅਚਲ int32_t* ਵਿਚਕਾਰ = bottom + (((ਸਿਖਰ - bottom) / 4) * 2);
        ਅਚਲ ਅੱਖਰ* ਇੰਦਰਾਜ_ਕੁੰਜੀ = &(ਕੁੰਜੀ_Strs[*ਵਿਚਕਾਰ]);
        int32_t strcmp_ਨਤੀਜਾ = uprv_strcmp(ਕੁੰਜੀ, ਇੰਦਰਾਜ_ਕੁੰਜੀ);
        ਜੇ (strcmp_ਨਤੀਜਾ == 0) {
            ਵਾਪਸ &(ਮੁੱਲ_Strs[ਵਿਚਕਾਰ[1]]);
        } ਨਹੀਂਤਾਂ ਜੇ (strcmp_ਨਤੀਜਾ < 0) {
            ਸਿਖਰ = ਵਿਚਕਾਰ;
        } ਨਹੀਂਤਾਂ {
            bottom = ਵਿਚਕਾਰ + 2;
        }
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਸਥਿਰ CharString getDefaultScript(ਅਚਲ CharString& language, ਅਚਲ CharString& region) {
    ਅਚਲ ਅੱਖਰ* defaultScript = ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString ਨਤੀਜਾ("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    ਜੇ (!region.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        ਜੇ (U_FAILURE(err)) {
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        defaultScript = performFallbackLookup(localeID.ਅੰਕੜੇ(), dsLocaleIDChars, script_ਕੋਡ_Chars, default_Script_ਸਾਰਣੀ, UPRV_LENGTHOF(default_Script_ਸਾਰਣੀ));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    ਜੇ (defaultScript == ਖਾਲੀ_ਸੂਚਕ) {
        defaultScript = performFallbackLookup(language.ਅੰਕੜੇ(), dsLocaleIDChars, script_ਕੋਡ_Chars, default_Script_ਸਾਰਣੀ, UPRV_LENGTHOF(default_Script_ਸਾਰਣੀ));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    ਜੇ (defaultScript != ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ.clear();
        ਨਤੀਜਾ.append(defaultScript, err);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਗਿਣਤੀ UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_ਜੜ੍ਹ,
    // TODO: ICU ticket #11271 "consistent default locale across locale trees"
    // Add an option to look at the main locale tree for whether to
    // fall back to root directly (if the locale has main data) or
    // fall back to the default locale first (if the locale does not even have main data).
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then load the root bundle;
     * never fall back to the default locale.
     *
     * This is used for algorithms that have good pan-Unicode default behavior,
     * such as case mappings, collation, and segmentation (BreakIterator).
     */
    URES_OPEN_LOCALE_ਜੜ੍ਹ,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_ਸਿੱਧਾ
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UResOpenType UResOpenType;

/**
 *  Internal function, determines the search path for resource bundle files.
 *  Currently, this function is used only by findFirstExisting() to help search for resource bundle files when a bundle for the specified
 *  locale doesn't exist.  The code that supports inheritance of resources between existing resource bundle files continues to
 *  use chopLocale() below.
 *  @param name In-out parameter: On input, the locale ID to get a parent locale ID for (this is a locale's base name, without keywords); on output, the
 *  requested parent locale ID.
 *  @param origName The original locale ID the caller of findFirstExisting() requested.  This is the same as `name` on the first call to this function,
 *  but as findFirstExisting() ascends the resource bundle's parent tree, this parameter will continue to be the original locale ID requested.
 */
ਸਥਿਰ ਤਾਰਕਿਕ get_ਮਾਪਾ_Locale_ID(ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    ਜੇ (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        ਵਾਪਸ chopLocale(name);
    }
    
    UError_ਕੋਡ err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, err);

    ਜੇ (U_FAILURE(err)) {
        // hopefully this never happens...
        ਵਾਪਸ chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    ਜੇ (openType == URES_OPEN_LOCALE_DEFAULT_ਜੜ੍ਹ) {
        ਅਚਲ ਅੱਖਰ* ਮਾਪਾ_ID = performFallbackLookup(name, ਮਾਪਾ_Locale_Chars, ਮਾਪਾ_Locale_Chars, ਮਾਪਾ_Locale_ਸਾਰਣੀ, UPRV_LENGTHOF(ਮਾਪਾ_Locale_ਸਾਰਣੀ));
        ਜੇ (ਮਾਪਾ_ID != ਖਾਲੀ_ਸੂਚਕ) {
            uprv_strcpy(name, ਮਾਪਾ_ID);
            ਵਾਪਸ ਸੱਚ;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    ਜੇ (!script.is_ਖਾਲੀ_ਥਾਵਾਂ() && !region.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        ਜੇ (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } ਨਹੀਂਤਾਂ {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } ਨਹੀਂਤਾਂ ਜੇ (!region.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_ਕੋਡ err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, err);
        ਜੇ (!origNameScript.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } ਨਹੀਂਤਾਂ {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } ਨਹੀਂਤਾਂ ਜੇ (!script.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        ਜੇ (openType != URES_OPEN_LOCALE_DEFAULT_ਜੜ੍ਹ || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਝੂਠ;
        }
    } ਨਹੀਂਤਾਂ {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (U_SUCCESS(err) && !workingLocale.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        uprv_strcpy(name, workingLocale.ਅੰਕੜੇ());
        ਵਾਪਸ ਸੱਚ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਝੂਠ;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ਸਥਿਰ UBool may_Have_ਮਾਪਾ(ਅੱਖਰ *name) {
    ਵਾਪਸ (name[0] != 0 && uprv_strstr("nb nn",name) != ਖਾਲੀ_ਸੂਚਕ);
}

/**
 *  Internal function
 */
ਸਥਿਰ ਖਾਲੀ ਇੰਦਰਾਜ_Increase(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ) {
    Mutex ਤਾਲਾ(&resbMutex);
    ਇੰਦਰਾਜ->f_ਗਿਣਤੀ_Existing++;
    ਜਦੋਂ(ਇੰਦਰਾਜ->f_ਮਾਪਾ != ਖਾਲੀ_ਸੂਚਕ) {
      ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->f_ਮਾਪਾ;
      ਇੰਦਰਾਜ->f_ਗਿਣਤੀ_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ਸਥਿਰ UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *get_Fallback_ਅੰਕੜੇ(
        ਅਚਲ UResourceBundle *resBundle,
        ਅਚਲ ਅੱਖਰ **res_ਟੈਗ, Resource *res, UError_ਕੋਡ *ਸਥਿਤੀ) {
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਅੰਕੜੇ_ਇੰਦਰਾਜ = resBundle->f_ਅੰਕੜੇ;
    int32_t ਸੂਚਕਾਂਕ_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    ਜੇ(ਅੰਕੜੇ_ਇੰਦਰਾਜ == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(ਅੰਕੜੇ_ਇੰਦਰਾਜ->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ(&(ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ), ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ.ਜੜ੍ਹ_Res, &ਸੂਚਕਾਂਕ_R, res_ਟੈਗ); /* try to get data from there */
        i++;
    }
    ਜੇ(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        ਜਦੋਂ(*res == RES_BOGUS && ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਮਾਪਾ != ਖਾਲੀ_ਸੂਚਕ) {
            ਅੰਕੜੇ_ਇੰਦਰਾਜ = ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਮਾਪਾ;
            ਜੇ(ਅੰਕੜੇ_ਇੰਦਰਾਜ->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ(&(ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ), ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ.ਜੜ੍ਹ_Res, &ਸੂਚਕਾਂਕ_R, res_ਟੈਗ);
            }
        }
    }

    ਜੇ(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    // If the resource is found in parents, we need to adjust the error.
    ਜੇ(i>1) {
        ਜੇ(uprv_strcmp(ਅੰਕੜੇ_ਇੰਦਰਾਜ->fName, uloc_getDefault())==0 || uprv_strcmp(ਅੰਕੜੇ_ਇੰਦਰਾਜ->fName, k_ਜੜ੍ਹ_Locale_Name)==0) {
            *ਸਥਿਤੀ = U_USING_DEFAULT_WARNING;
        } ਨਹੀਂਤਾਂ {
            *ਸਥਿਤੀ = U_USING_FALLBACK_WARNING;
        }
    }
    ਵਾਪਸ ਅੰਕੜੇ_ਇੰਦਰਾਜ;
}

ਸਥਿਰ ਖਾਲੀ
free_ਇੰਦਰਾਜ(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ) {
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *alias;
    res_unload(&(ਇੰਦਰਾਜ->f_ਅੰਕੜੇ));
    ਜੇ(ਇੰਦਰਾਜ->fName != ਖਾਲੀ_ਸੂਚਕ && ਇੰਦਰਾਜ->fName != ਇੰਦਰਾਜ->f_Name_ਬਫਰ) {
        uprv_free(ਇੰਦਰਾਜ->fName);
    }
    ਜੇ(ਇੰਦਰਾਜ->f_ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(ਇੰਦਰਾਜ->f_ਰਸਤਾ);
    }
    ਜੇ(ਇੰਦਰਾਜ->fPool != ਖਾਲੀ_ਸੂਚਕ) {
        --ਇੰਦਰਾਜ->fPool->f_ਗਿਣਤੀ_Existing;
    }
    alias = ਇੰਦਰਾਜ->fAlias;
    ਜੇ(alias != ਖਾਲੀ_ਸੂਚਕ) {
        ਜਦੋਂ(alias->fAlias != ਖਾਲੀ_ਸੂਚਕ) {
            alias = alias->fAlias;
        }
        --alias->f_ਗਿਣਤੀ_Existing;
    }
    uprv_free(ਇੰਦਰਾਜ);
}

/* Works just like ucnv_flushCache() */
ਸਥਿਰ int32_t ures_flushCache()
{
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *resB;
    int32_t pos;
    int32_t rb_Deleted_ਅੰਕ = 0;
    ਅਚਲ UHashElement *e;
    UBool deleted_ਹੋਰ;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex ਤਾਲਾ(&resbMutex);
    ਜੇ (cache == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }

    ਕਰੋ {
        deleted_ਹੋਰ = ਝੂਠ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_ਪਹਿਲਾ;
        ਜਦੋਂ ((e = uhash_ਅਗਲਾ_Element(cache, &pos)) != ਖਾਲੀ_ਸੂਚਕ)
        {
            resB = static_cast<UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ*>(e->value.ਸੂਚਕ);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            ਜੇ (resB->f_ਗਿਣਤੀ_Existing == 0) {
                rb_Deleted_ਅੰਕ++;
                deleted_ਹੋਰ = ਸੱਚ;
                uhash_removeElement(cache, e);
                free_ਇੰਦਰਾਜ(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } ਜਦੋਂ(deleted_ਹੋਰ);

    ਵਾਪਸ rb_Deleted_ਅੰਕ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ URES_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_ਖਾਲੀ_ਥਾਵਾਂ = ਝੂਠ;
  int32_t pos = UHASH_ਪਹਿਲਾ;
  ਅਚਲ UHashElement *e;
  UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *resB;
  
    Mutex ਤਾਲਾ(&resbMutex);
    ਜੇ (cache == ਖਾਲੀ_ਸੂਚਕ) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      ਵਾਪਸ ਝੂਠ;
    }

    ਜਦੋਂ ((e = uhash_ਅਗਲਾ_Element(cache, &pos)) != ਖਾਲੀ_ਸੂਚਕ) {
      cache_Not_ਖਾਲੀ_ਥਾਵਾਂ=ਸੱਚ;
      resB = (UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *) e->value.ਸੂਚਕ;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (ਖਾਲੀ*)resB, resB->f_ਗਿਣਤੀ_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_ਰਸਤਾ?resB->f_ਰਸਤਾ:"nullptr",
              (ਖਾਲੀ*)resB->fPool,
              (ਖਾਲੀ*)resB->fAlias,
              (ਖਾਲੀ*)resB->f_ਮਾਪਾ);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_ਗਿਣਤੀ(cache));
    ਵਾਪਸ cache_Not_ਖਾਲੀ_ਥਾਵਾਂ;
}

#ਜੇ_ਅੰਤ

ਸਥਿਰ UBool U_CALLCONV ures_cleanup()
{
    ਜੇ (cache != ਖਾਲੀ_ਸੂਚਕ) {
        ures_flushCache();
        uhash_close(cache);
        cache = ਖਾਲੀ_ਸੂਚਕ;
    }
    gCacheInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

/** INTERNAL: Initializes the cache for resources */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV createCache(UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(cache == ਖਾਲੀ_ਸੂਚਕ);
    cache = uhash_open(hash_ਇੰਦਰਾਜ, compareEntries, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ਸਥਿਰ ਖਾਲੀ initCache(UError_ਕੋਡ *ਸਥਿਤੀ) {
    umtx_initOnce(gCacheInitOnce, &createCache, *ਸਥਿਤੀ);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_ਇੰਦਰਾਜ_Name(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *res, ਅਚਲ ਅੱਖਰ *name, UError_ਕੋਡ *ਸਥਿਤੀ) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    ਜੇ(res->fName != ਖਾਲੀ_ਸੂਚਕ && res->fName != res->f_Name_ਬਫਰ) {
        uprv_free(res->fName);
    }
    ਜੇ (len < static_cast<int32_t>(ਆਕਾਰ(res->f_Name_ਬਫਰ))) {
        res->fName = res->f_Name_ਬਫਰ;
    }
    ਨਹੀਂਤਾਂ {
        res->fName = static_cast<ਅੱਖਰ*>(uprv_malloc(len + 1));
    }
    ਜੇ(res->fName == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ {
        uprv_strcpy(res->fName, name);
    }
}

ਸਥਿਰ UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *
get_Pool_ਇੰਦਰਾਜ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ਸਥਿਰ UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *init_ਇੰਦਰਾਜ(ਅਚਲ ਅੱਖਰ *localeID, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ) {
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *r = ਖਾਲੀ_ਸੂਚਕ;
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ find;
    /*int32_t hashValue;*/
    ਅਚਲ ਅੱਖਰ *name;
    ਅੱਖਰ aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* here we try to deduce the right locale name */
    ਜੇ(localeID == ਖਾਲੀ_ਸੂਚਕ) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } ਨਹੀਂਤਾਂ ਜੇ(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_ਜੜ੍ਹ_Locale_Name;
    } ਨਹੀਂਤਾਂ { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<ਅੱਖਰ*>(name);
    find.f_ਰਸਤਾ = const_cast<ਅੱਖਰ*>(ਰਸਤਾ);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ*>(uhash_get(cache, &find));
    ਜੇ(r == ਖਾਲੀ_ਸੂਚਕ) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ*>(uprv_malloc(ਆਕਾਰ(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ)));
        ਜੇ(r == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        uprv_memset(r, 0, ਆਕਾਰ(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ));
        /*r->fHashKey = hashValue;*/

        ਸੈੱਟ_ਇੰਦਰਾਜ_Name(r, name, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            uprv_free(r);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        ਜੇ(ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ) {
            r->f_ਰਸਤਾ = uprv_strdup(ਰਸਤਾ);
            ਜੇ(r->f_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                uprv_free(r);
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        }

        /* this is the actual loading */
        res_ਲੱਦੋ(&(r->f_ਅੰਕੜੇ), r->f_ਰਸਤਾ, r->fName, ਸਥਿਤੀ);

        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            ਜੇ (*ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
                uprv_free(r);
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *ਸਥਿਤੀ = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } ਨਹੀਂਤਾਂ { /* if we have a regular entry */
            Resource aliasres;
            ਜੇ (r->f_ਅੰਕੜੇ.ਵਰਤਦਾ_Pool_Bundle) {
                r->fPool = get_Pool_ਇੰਦਰਾਜ(r->f_ਰਸਤਾ, ਸਥਿਤੀ);
                ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
                    ਅਚਲ int32_t *poolIndexes = r->fPool->f_ਅੰਕੜੇ.p_ਜੜ੍ਹ + 1;
                    ਜੇ(r->f_ਅੰਕੜੇ.p_ਜੜ੍ਹ[1 + URES_ਸੂਚਕਾਂਕ_POOL_CHECKSUM] == poolIndexes[URES_ਸੂਚਕਾਂਕ_POOL_CHECKSUM]) {
                        r->f_ਅੰਕੜੇ.pool_Bundle_ਕੁੰਜੀਆਂ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(poolIndexes + (poolIndexes[URES_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ] & 0xff));
                        r->f_ਅੰਕੜੇ.pool_Bundle_ਲੜੀਆਂ = r->fPool->f_ਅੰਕੜੇ.p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units;
                    } ਨਹੀਂਤਾਂ {
                        r->fBogus = *ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
                    }
                } ਨਹੀਂਤਾਂ {
                    r->fBogus = *ਸਥਿਤੀ;
                }
            }
            ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_ਅੰਕੜੇ), "%%ALIAS");
                ਜੇ (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ਅਚਲ char16_t *alias = res_getStringNoTrace(&(r->f_ਅੰਕੜੇ), aliasres, &aliasLen);
                    ਜੇ(alias != ਖਾਲੀ_ਸੂਚਕ && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_ਮੰਜ਼ਿਲ_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_ਇੰਦਰਾਜ(aliasName, ਰਸਤਾ, ਸਥਿਤੀ);
                    }
                }
            }
        }

        {
            UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *oldR = ਖਾਲੀ_ਸੂਚਕ;
            ਜੇ ((oldR = static_cast<UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ*>(uhash_get(cache, r))) == ਖਾਲੀ_ਸੂਚਕ) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_ਕੋਡ cache_ਸਥਿਤੀ = U_ZERO_ERROR;
                uhash_put(cache, (ਖਾਲੀ *)r, r, &cache_ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(cache_ਸਥਿਤੀ)) {
                    *ਸਥਿਤੀ = cache_ਸਥਿਤੀ;
                    free_ਇੰਦਰਾਜ(r);
                    r = ਖਾਲੀ_ਸੂਚਕ;
                }
            } ਨਹੀਂਤਾਂ {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_ਇੰਦਰਾਜ(r);
                r = oldR;
            }
        }

    }
    ਜੇ(r != ਖਾਲੀ_ਸੂਚਕ) {
        /* return the real bundle */
        ਜਦੋਂ(r->fAlias != ਖਾਲੀ_ਸੂਚਕ) {
            r = r->fAlias;
        }
        r->f_ਗਿਣਤੀ_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        ਜੇ(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*ਸਥਿਤੀ)) {
             *ਸਥਿਤੀ = r->fBogus; /* set the returning status */
        }
    }
    ਵਾਪਸ r;
}

ਸਥਿਰ UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *
get_Pool_ਇੰਦਰਾਜ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ) {
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *poolBundle = init_ਇੰਦਰਾਜ(kPoolBundleName, ਰਸਤਾ, ਸਥਿਤੀ);
    ਜੇ( U_SUCCESS(*ਸਥਿਤੀ) &&
        (poolBundle == ਖਾਲੀ_ਸੂਚਕ || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_ਅੰਕੜੇ.isPoolBundle)
    ) {
        *ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
    }
    ਵਾਪਸ poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ਸਥਿਰ UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *
ਲੱਭੋ_ਪਹਿਲਾ_Existing(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅੱਖਰ* name, ਅਚਲ ਅੱਖਰ* defaultLocale, UResOpenType openType,
                  UBool *is_ਜੜ੍ਹ, UBool *ਮਿਲਿਆ_ਮਾਪਾ, UBool *isDefault, UError_ਕੋਡ* ਸਥਿਤੀ) {
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *r = ਖਾਲੀ_ਸੂਚਕ;
    UBool has_Real_ਅੰਕੜੇ = ਝੂਠ;
    *ਮਿਲਿਆ_ਮਾਪਾ = ਸੱਚ; /* we're starting with a fresh name */
    ਅੱਖਰ origName[ULOC_FULLNAME_ਸਮਰੱਥਾ];

    uprv_strcpy(origName, name);
    ਜਦੋਂ(*ਮਿਲਿਆ_ਮਾਪਾ && !has_Real_ਅੰਕੜੇ) {
        r = init_ਇੰਦਰਾਜ(name, ਰਸਤਾ, ਸਥਿਤੀ);
        /* Null pointer test */
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_ਅੰਕੜੇ = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        ਜੇ(!has_Real_ਅੰਕੜੇ) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_ਗਿਣਤੀ_Existing--;
            /*entryCloseInt(r);*/
            r = ਖਾਲੀ_ਸੂਚਕ;
            *ਸਥਿਤੀ = U_USING_FALLBACK_WARNING;
        } ਨਹੀਂਤਾਂ {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_ਜੜ੍ਹ = static_cast<UBool>(uprv_strcmp(name, k_ਜੜ੍ਹ_Locale_Name) == 0);

        /*Fallback data stuff*/
        ਜੇ (!has_Real_ਅੰਕੜੇ) {
            *ਮਿਲਿਆ_ਮਾਪਾ = get_ਮਾਪਾ_Locale_ID(name, origName, openType);
        } ਨਹੀਂਤਾਂ {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *ਮਿਲਿਆ_ਮਾਪਾ = chopLocale(name);
        }
        ਜੇ (*ਮਿਲਿਆ_ਮਾਪਾ && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    ਵਾਪਸ r;
}

ਸਥਿਰ ਖਾਲੀ ures_ਸੈੱਟ_Is_ਢੇਰ_Object( UResourceBundle* resB, UBool state) {
    ਜੇ(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } ਨਹੀਂਤਾਂ {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ਸਥਿਰ UBool ures_is_ਢੇਰ_Object(ਅਚਲ UResourceBundle* resB) {
  ਵਾਪਸ((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?ਝੂਠ:ਸੱਚ);
}


U_CFUNC ਖਾਲੀ ures_init_ਢੇਰ_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, ਆਕਾਰ(UResourceBundle));
  ures_ਸੈੱਟ_Is_ਢੇਰ_Object(resB, ਸੱਚ);
}

U_NAMESPACE_BEGIN

ਢੇਰ_UResource_Bundle::ਢੇਰ_UResource_Bundle() {
    ures_init_ਢੇਰ_Object(&bundle);
}

ਢੇਰ_UResource_Bundle::~ਢੇਰ_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_ਸਿਰਾ

ਸਥਿਰ UBool  // returns U_SUCCESS(*status)
ਲੱਦੋ_Parents_Except_ਜੜ੍ਹ(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *&t1,
                      ਅੱਖਰ name[], int32_t name_ਸਮਰੱਥਾ,
                      UBool usingUSRData, ਅੱਖਰ usr_ਅੰਕੜੇ_ਰਸਤਾ[], UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }
    UBool check_ਮਾਪਾ = ਸੱਚ;
    ਜਦੋਂ (check_ਮਾਪਾ && t1->f_ਮਾਪਾ == ਖਾਲੀ_ਸੂਚਕ && !t1->f_ਅੰਕੜੇ.noFallback &&
            res_getResource(&t1->f_ਅੰਕੜੇ,"%%ParentIsRoot") == RES_BOGUS) {
        Resource ਮਾਪਾ_Res = res_getResource(&t1->f_ਅੰਕੜੇ, "%%Parent");
        ਜੇ (ਮਾਪਾ_Res != RES_BOGUS) {  // An explicit parent was found.
            int32_t ਮਾਪਾ_Locale_Len = 0;
            // No tracing: called during initial data loading
            ਅਚਲ char16_t *ਮਾਪਾ_Locale_Name = res_getStringNoTrace(&(t1->f_ਅੰਕੜੇ), ਮਾਪਾ_Res, &ਮਾਪਾ_Locale_Len);
            ਜੇ(ਮਾਪਾ_Locale_Name != ਖਾਲੀ_ਸੂਚਕ && 0 < ਮਾਪਾ_Locale_Len && ਮਾਪਾ_Locale_Len < name_ਸਮਰੱਥਾ) {
                u_UChars_ਮੰਜ਼ਿਲ_Chars(ਮਾਪਾ_Locale_Name, name, ਮਾਪਾ_Locale_Len + 1);
                ਜੇ (uprv_strcmp(name, k_ਜੜ੍ਹ_Locale_Name) == 0) {
                    ਵਾਪਸ ਸੱਚ;
                }
            }
        }
        // Insert regular parents.
        UError_ਕੋਡ ਮਾਪਾ_ਸਥਿਤੀ = U_ZERO_ERROR;
        UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *t2 = init_ਇੰਦਰਾਜ(name, t1->f_ਰਸਤਾ, &ਮਾਪਾ_ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਮਾਪਾ_ਸਥਿਤੀ)) {
            *ਸਥਿਤੀ = ਮਾਪਾ_ਸਥਿਤੀ;
            ਵਾਪਸ ਝੂਠ;
        }
        UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *u2 = ਖਾਲੀ_ਸੂਚਕ;
        UError_ਕੋਡ usr_ਸਥਿਤੀ = U_ZERO_ERROR;
        ਜੇ (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_ਇੰਦਰਾਜ(name, usr_ਅੰਕੜੇ_ਰਸਤਾ, &usr_ਸਥਿਤੀ);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            ਜੇ (usr_ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
                *ਸਥਿਤੀ = usr_ਸਥਿਤੀ;
                ਵਾਪਸ ਝੂਠ;
            }
        }

        ਜੇ (usingUSRData && U_SUCCESS(usr_ਸਥਿਤੀ) && u2->fBogus == U_ZERO_ERROR) {
            t1->f_ਮਾਪਾ = u2;
            u2->f_ਮਾਪਾ = t2;
        } ਨਹੀਂਤਾਂ {
            t1->f_ਮਾਪਾ = t2;
            ਜੇ (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_ਗਿਣਤੀ_Existing = 0;
            }
        }
        t1 = t2;
        check_ਮਾਪਾ = chopLocale(name) || may_Have_ਮਾਪਾ(name);
    }
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ UBool  // returns U_SUCCESS(*status)
ਪਾਓ_ਜੜ੍ਹ_Bundle(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *&t1, UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }
    UError_ਕੋਡ ਮਾਪਾ_ਸਥਿਤੀ = U_ZERO_ERROR;
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *t2 = init_ਇੰਦਰਾਜ(k_ਜੜ੍ਹ_Locale_Name, t1->f_ਰਸਤਾ, &ਮਾਪਾ_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਮਾਪਾ_ਸਥਿਤੀ)) {
        *ਸਥਿਤੀ = ਮਾਪਾ_ਸਥਿਤੀ;
        ਵਾਪਸ ਝੂਠ;
    }
    t1->f_ਮਾਪਾ = t2;
    t1 = t2;
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ_Open(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* localeID,
                                     UResOpenType openType, UError_ਕੋਡ* ਸਥਿਤੀ) {
    U_ASSERT(openType != URES_OPEN_ਸਿੱਧਾ);
    UError_ਕੋਡ int_ਸਥਿਤੀ = U_ZERO_ERROR;
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *r = ਖਾਲੀ_ਸੂਚਕ;
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *t1 = ਖਾਲੀ_ਸੂਚਕ;
    UBool isDefault = ਝੂਠ;
    UBool is_ਜੜ੍ਹ = ਝੂਠ;
    UBool has_Real_ਅੰਕੜੇ = ਝੂਠ;
    UBool hasChopped = ਸੱਚ;
    UBool usingUSRData = U_USE_USRDATA && ( ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ || uprv_strncmp(ਰਸਤਾ,U_ICUDATA_NAME,8) == 0);

    ਅੱਖਰ name[ULOC_FULLNAME_ਸਮਰੱਥਾ];
    ਅੱਖਰ usr_ਅੰਕੜੇ_ਰਸਤਾ[96];

    initCache(ਸਥਿਤੀ);

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    uprv_strncpy(name, localeID, ਆਕਾਰ(name) - 1);
    name[ਆਕਾਰ(name) - 1] = 0;

    ਜੇ ( usingUSRData ) {
        ਜੇ ( ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ ) {
            uprv_strcpy(usr_ਅੰਕੜੇ_ਰਸਤਾ, U_USRDATA_NAME);
        } ਨਹੀਂਤਾਂ {
            uprv_strncpy(usr_ਅੰਕੜੇ_ਰਸਤਾ, ਰਸਤਾ, ਆਕਾਰ(usr_ਅੰਕੜੇ_ਰਸਤਾ) - 1);
            usr_ਅੰਕੜੇ_ਰਸਤਾ[0] = 'u';
            usr_ਅੰਕੜੇ_ਰਸਤਾ[1] = 's';
            usr_ਅੰਕੜੇ_ਰਸਤਾ[2] = 'r';
            usr_ਅੰਕੜੇ_ਰਸਤਾ[ਆਕਾਰ(usr_ਅੰਕੜੇ_ਰਸਤਾ) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ਅਚਲ ਅੱਖਰ *defaultLocale = uloc_getDefault();

    Mutex ਤਾਲਾ(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = ਲੱਭੋ_ਪਹਿਲਾ_Existing(ਰਸਤਾ, name, defaultLocale, openType, &is_ਜੜ੍ਹ, &hasChopped, &isDefault, &int_ਸਥਿਤੀ);

    // If we failed due to out-of-memory, report the failure and exit early.
    ਜੇ (int_ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
        *ਸਥਿਤੀ = int_ਸਥਿਤੀ;
        ਜਾਓ ਅੰਤ;
    }

    ਜੇ(r != ਖਾਲੀ_ਸੂਚਕ) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_ਅੰਕੜੇ = ਸੱਚ;
        ਜੇ ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_ਕੋਡ usr_ਸਥਿਤੀ = U_ZERO_ERROR;
            UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *u1 = init_ਇੰਦਰਾਜ(t1->fName, usr_ਅੰਕੜੇ_ਰਸਤਾ, &usr_ਸਥਿਤੀ);
            // If we failed due to out-of-memory, report the failure and exit early.
            ਜੇ (int_ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
                *ਸਥਿਤੀ = int_ਸਥਿਤੀ;
                ਜਾਓ ਅੰਤ;
            }
            ਜੇ ( u1 != ਖਾਲੀ_ਸੂਚਕ ) {
                ਜੇ(u1->fBogus == U_ZERO_ERROR) {
                    u1->f_ਮਾਪਾ = t1;
                    r = u1;
                } ਨਹੀਂਤਾਂ {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_ਗਿਣਤੀ_Existing = 0;
                }
            }
        }
        ਜੇ ((hasChopped || may_Have_ਮਾਪਾ(name)) && !is_ਜੜ੍ਹ) {
            ਜੇ (!ਲੱਦੋ_Parents_Except_ਜੜ੍ਹ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_ਅੰਕੜੇ_ਰਸਤਾ, ਸਥਿਤੀ)) {
                ਜਾਓ ਅੰਤ;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    ਜੇ(r==ਖਾਲੀ_ਸੂਚਕ && openType == URES_OPEN_LOCALE_DEFAULT_ਜੜ੍ਹ && !isDefault && !is_ਜੜ੍ਹ) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = ਲੱਭੋ_ਪਹਿਲਾ_Existing(ਰਸਤਾ, name, defaultLocale, openType, &is_ਜੜ੍ਹ, &hasChopped, &isDefault, &int_ਸਥਿਤੀ);
        // If we failed due to out-of-memory, report the failure and exit early.
        ਜੇ (int_ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
            *ਸਥਿਤੀ = int_ਸਥਿਤੀ;
            ਜਾਓ ਅੰਤ;
        }
        int_ਸਥਿਤੀ = U_USING_DEFAULT_WARNING;
        ਜੇ(r != ਖਾਲੀ_ਸੂਚਕ) { /* the default locale exists */
            t1 = r;
            has_Real_ਅੰਕੜੇ = ਸੱਚ;
            isDefault = ਸੱਚ;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            ਜੇ ((hasChopped || may_Have_ਮਾਪਾ(name)) && !is_ਜੜ੍ਹ) {
                ਜੇ (!ਲੱਦੋ_Parents_Except_ਜੜ੍ਹ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_ਅੰਕੜੇ_ਰਸਤਾ, ਸਥਿਤੀ)) {
                    ਜਾਓ ਅੰਤ;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    ਜੇ(r == ਖਾਲੀ_ਸੂਚਕ) {
        uprv_strcpy(name, k_ਜੜ੍ਹ_Locale_Name);
        r = ਲੱਭੋ_ਪਹਿਲਾ_Existing(ਰਸਤਾ, name, defaultLocale, openType, &is_ਜੜ੍ਹ, &hasChopped, &isDefault, &int_ਸਥਿਤੀ);
        // If we failed due to out-of-memory, report the failure and exit early.
        ਜੇ (int_ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
            *ਸਥਿਤੀ = int_ਸਥਿਤੀ;
            ਜਾਓ ਅੰਤ;
        }
        ਜੇ(r != ਖਾਲੀ_ਸੂਚਕ) {
            t1 = r;
            int_ਸਥਿਤੀ = U_USING_DEFAULT_WARNING;
            has_Real_ਅੰਕੜੇ = ਸੱਚ;
        } ਨਹੀਂਤਾਂ { /* we don't even have the root locale */
            *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
            ਜਾਓ ਅੰਤ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(!is_ਜੜ੍ਹ && uprv_strcmp(t1->fName, k_ਜੜ੍ਹ_Locale_Name) != 0 &&
            t1->f_ਮਾਪਾ == ਖਾਲੀ_ਸੂਚਕ && !r->f_ਅੰਕੜੇ.noFallback) {
        ਜੇ (!ਪਾਓ_ਜੜ੍ਹ_Bundle(t1, ਸਥਿਤੀ)) {
            ਜਾਓ ਅੰਤ;
        }
        ਜੇ(!has_Real_ਅੰਕੜੇ) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    ਜਦੋਂ(r != ਖਾਲੀ_ਸੂਚਕ && !is_ਜੜ੍ਹ && t1->f_ਮਾਪਾ != ਖਾਲੀ_ਸੂਚਕ) {
        t1->f_ਮਾਪਾ->f_ਗਿਣਤੀ_Existing++;
        t1 = t1->f_ਮਾਪਾ;
    }

ਅੰਤ:
    ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
        ਜੇ(int_ਸਥਿਤੀ != U_ZERO_ERROR) {
            *ਸਥਿਤੀ = int_ਸਥਿਤੀ;  
        }
        ਵਾਪਸ r;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ਸਥਿਰ UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *
ਇੰਦਰਾਜ_Open_ਸਿੱਧਾ(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* localeID, UError_ਕੋਡ* ਸਥਿਤੀ) {
    initCache(ਸਥਿਤੀ);
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    } ਨਹੀਂਤਾਂ ਜੇ (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_ਜੜ੍ਹ_Locale_Name;
    }

    Mutex ਤਾਲਾ(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *r = init_ਇੰਦਰਾਜ(localeID, ਰਸਤਾ, ਸਥਿਤੀ);
    ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
        ਜੇ(r->fBogus != U_ZERO_ERROR) {
            r->f_ਗਿਣਤੀ_Existing--;
            r = ਖਾਲੀ_ਸੂਚਕ;
        }
    } ਨਹੀਂਤਾਂ {
        r = ਖਾਲੀ_ਸੂਚਕ;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *t1 = r;
    ਜੇ(r != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(localeID, k_ਜੜ੍ਹ_Locale_Name) != 0 &&  // not root
            r->f_ਮਾਪਾ == ਖਾਲੀ_ਸੂਚਕ && !r->f_ਅੰਕੜੇ.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_ਸਮਰੱਥਾ) {
        ਅੱਖਰ name[ULOC_FULLNAME_ਸਮਰੱਥਾ];
        uprv_strcpy(name, localeID);
        ਜੇ(!chopLocale(name) || uprv_strcmp(name, k_ਜੜ੍ਹ_Locale_Name) == 0 ||
                ਲੱਦੋ_Parents_Except_ਜੜ੍ਹ(t1, name, UPRV_LENGTHOF(name), ਝੂਠ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ)) {
            ਜੇ(uprv_strcmp(t1->fName, k_ਜੜ੍ਹ_Locale_Name) != 0 && t1->f_ਮਾਪਾ == ਖਾਲੀ_ਸੂਚਕ) {
                ਪਾਓ_ਜੜ੍ਹ_Bundle(t1, ਸਥਿਤੀ);
            }
        }
        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
            r = ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    ਜੇ(r != ਖਾਲੀ_ਸੂਚਕ) {
        // TODO: Does this ever loop?
        ਜਦੋਂ(t1->f_ਮਾਪਾ != ਖਾਲੀ_ਸੂਚਕ) {
            t1->f_ਮਾਪਾ->f_ਗਿਣਤੀ_Existing++;
            t1 = t1->f_ਮਾਪਾ;
        }
    }
    ਵਾਪਸ r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ਸਥਿਰ ਖਾਲੀ ਇੰਦਰਾਜ_Close_Int(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *resB) {
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *p = resB;

    ਜਦੋਂ(resB != ਖਾਲੀ_ਸੂਚਕ) {
        p = resB->f_ਮਾਪਾ;
        resB->f_ਗਿਣਤੀ_Existing--;

        /* Entries are left in the cache. TODO: add ures_flushCache() to force a flush
         of the cache. */
/*
        if(resB->fCountExisting <= 0) {
            uhash_remove(cache, resB);
            if(resB->fBogus == U_ZERO_ERROR) {
                res_unload(&(resB->fData));
            }
            if(resB->fName != nullptr) {
                uprv_free(resB->fName);
            }
            if(resB->fPath != nullptr) {
                uprv_free(resB->fPath);
            }
            uprv_free(resB);
        }
*/

        resB = p;
    }
}

/** 
 *  API: closes a resource bundle and cleans up.
 */

ਸਥਿਰ ਖਾਲੀ ਇੰਦਰਾਜ_Close(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *resB) {
  Mutex ਤਾਲਾ(&resbMutex);
  ਇੰਦਰਾਜ_Close_Int(resB);
}

/*
U_CFUNC void ures_setResPath(UResourceBundle *resB, const char* toAdd) {
  if(resB->fResPath == nullptr) {
    resB->fResPath = resB->fResBuf;
    *(resB->fResPath) = 0;
  } 
  resB->fResPathLen = uprv_strlen(toAdd);
  if(RES_BUFSIZE <= resB->fResPathLen+1) {
    if(resB->fResPath == resB->fResBuf) {
      resB->fResPath = (char *)uprv_malloc((resB->fResPathLen+1)*sizeof(char));
    } else {
      resB->fResPath = (char *)uprv_realloc(resB->fResPath, (resB->fResPathLen+1)*sizeof(char));
    }
  }
  uprv_strcpy(resB->fResPath, toAdd);
}
*/
ਸਥਿਰ ਖਾਲੀ ures_append_Res_ਰਸਤਾ(UResourceBundle *resB, ਅਚਲ ਅੱਖਰ* ਮੰਜ਼ਿਲ_ਜੋੜੋ, int32_t len_ਮੰਜ਼ਿਲ_ਜੋੜੋ, UError_ਕੋਡ *ਸਥਿਤੀ) {
    int32_t res_ਰਸਤਾ_Len_Orig = resB->f_Res_ਰਸਤਾ_Len;
    ਜੇ(resB->f_Res_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
        resB->f_Res_ਰਸਤਾ = resB->fResBuf;
        *(resB->f_Res_ਰਸਤਾ) = 0;
        resB->f_Res_ਰਸਤਾ_Len = 0;
    } 
    resB->f_Res_ਰਸਤਾ_Len += len_ਮੰਜ਼ਿਲ_ਜੋੜੋ;
    ਜੇ(RES_BUFSIZE <= resB->f_Res_ਰਸਤਾ_Len+1) {
        ਜੇ(resB->f_Res_ਰਸਤਾ == resB->fResBuf) {
            resB->f_Res_ਰਸਤਾ = static_cast<ਅੱਖਰ*>(uprv_malloc((resB->f_Res_ਰਸਤਾ_Len + 1) * ਆਕਾਰ(ਅੱਖਰ)));
            /* Check that memory was allocated correctly. */
            ਜੇ (resB->f_Res_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            uprv_strcpy(resB->f_Res_ਰਸਤਾ, resB->fResBuf);
        } ਨਹੀਂਤਾਂ {
            ਅੱਖਰ* ਆਰਜ਼ੀ = static_cast<ਅੱਖਰ*>(uprv_realloc(resB->f_Res_ਰਸਤਾ, (resB->f_Res_ਰਸਤਾ_Len + 1) * ਆਕਾਰ(ਅੱਖਰ)));
            /* Check that memory was reallocated correctly. */
            ਜੇ (ਆਰਜ਼ੀ == ਖਾਲੀ_ਸੂਚਕ) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            resB->f_Res_ਰਸਤਾ = ਆਰਜ਼ੀ;
        }
    }
    uprv_strcpy(resB->f_Res_ਰਸਤਾ + res_ਰਸਤਾ_Len_Orig, ਮੰਜ਼ਿਲ_ਜੋੜੋ);
}

ਸਥਿਰ ਖਾਲੀ ures_free_Res_ਰਸਤਾ(UResourceBundle *resB) {
    ਜੇ (resB->f_Res_ਰਸਤਾ && resB->f_Res_ਰਸਤਾ != resB->fResBuf) {
        uprv_free(resB->f_Res_ਰਸਤਾ);
    }
    resB->f_Res_ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
    resB->f_Res_ਰਸਤਾ_Len = 0;
}

ਸਥਿਰ ਖਾਲੀ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    ਜੇ(resB != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(resB->f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
            ਇੰਦਰਾਜ_Close(resB->f_ਅੰਕੜੇ);
        }
        ਜੇ(resB->f_ਸੰਸਕਰਣ != ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(resB->f_ਸੰਸਕਰਣ);
        }
        ures_free_Res_ਰਸਤਾ(resB);

        ਜੇ(ures_is_ਢੇਰ_Object(resB) == ਝੂਠ && freeBundleObj) {
            uprv_free(resB);
        }
#ਜੇ 0 /*U_DEBUG*/
        ਨਹੀਂਤਾਂ {
            /* poison the data */
            uprv_memset(resB, -1, ਆਕਾਰ(UResourceBundle));
        }
#ਜੇ_ਅੰਤ
    }
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, ਸੱਚ);
}

ਨਾਮਾਕਾਸ਼ {

UResourceBundle* init_resb_ਨਤੀਜਾ(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* ਅੰਕੜੇ_ਇੰਦਰਾਜ,
                                  Resource r,
                                  ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                  int32_t idx,
                                  UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ,
                                  ਅਚਲ ਅੱਖਰ* container_Res_ਰਸਤਾ,
                                  int32_t recursion_ਡੂੰਘਾਈ,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ਕੋਡ* ਸਥਿਤੀ);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
get_Alias_ਨਿਸ਼ਾਨਾ_As_Resource_Bundle(ਅਚਲ Resource_ਅੰਕੜੇ& res_ਅੰਕੜੇ,
                               Resource r,
                               ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                               int32_t idx,
                               UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ,
                               ਅਚਲ ਅੱਖਰ* container_Res_ਰਸਤਾ,
                               int32_t recursion_ਡੂੰਘਾਈ,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_ਕੋਡ* ਸਥਿਤੀ) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ਅਚਲ char16_t *alias = res_getAlias(&res_ਅੰਕੜੇ, r, &len);
    ਜੇ(len <= 0) {
        // bad alias
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ resB;
    }

    // Copy the UTF-16 alias string into an invariant-character string.
    //
    // We do this so that res_findResource() can modify the path,
    // which allows us to remove redundant _res_findResource() variants
    // in uresdata.c.
    // res_findResource() now NUL-terminates each segment so that table keys
    // can always be compared with strcmp() instead of strncmp().
    // Saves code there and simplifies testing and code coverage.
    //
    // markus 2003oct17
    CharString chAlias;
    chAlias.appendInvariantChars(alias, len, *ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // We have an alias, now let's cut it up.
    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ, *locale = ਖਾਲੀ_ਸੂਚਕ, *ਕੁੰਜੀ_ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ(chAlias[0] == RES_ਰਸਤਾ_SEPARATOR) {
        // There is a path included.
        ਅੱਖਰ *ch_Alias_ਅੰਕੜੇ = chAlias.ਅੰਕੜੇ();
        ਅੱਖਰ *sep = ch_Alias_ਅੰਕੜੇ + 1;
        ਰਸਤਾ = sep;
        sep = uprv_strchr(sep, RES_ਰਸਤਾ_SEPARATOR);
        ਜੇ(sep != ਖਾਲੀ_ਸੂਚਕ) {
            *sep++ = 0;
        }
        ਜੇ(uprv_strcmp(ਰਸਤਾ, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            ਕੁੰਜੀ_ਰਸਤਾ = sep;
            // Read from the valid locale which we already have.
            ਰਸਤਾ = locale = ਖਾਲੀ_ਸੂਚਕ;
        } ਨਹੀਂਤਾਂ {
            ਜੇ(uprv_strcmp(ਰਸਤਾ, "ICUDATA") == 0) { /* want ICU data */
                ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
            }
            ਜੇ (sep == ਖਾਲੀ_ਸੂਚਕ) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } ਨਹੀਂਤਾਂ {
                locale = sep;
                sep = uprv_strchr(sep, RES_ਰਸਤਾ_SEPARATOR);
                ਜੇ(sep != ਖਾਲੀ_ਸੂਚਕ) {
                    *sep++ = 0;
                }
                ਕੁੰਜੀ_ਰਸਤਾ = sep;
            }
        }
    } ਨਹੀਂਤਾਂ {
        // No path, start with a locale.
        ਅੱਖਰ *sep = chAlias.ਅੰਕੜੇ();
        locale = sep;
        sep = uprv_strchr(sep, RES_ਰਸਤਾ_SEPARATOR);
        ਜੇ(sep != ਖਾਲੀ_ਸੂਚਕ) {
            *sep++ = 0;
        }
        ਕੁੰਜੀ_ਰਸਤਾ = sep;
        ਰਸਤਾ = valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਰਸਤਾ;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    Local_UResource_Bundle_ਸੂਚਕ mainRes;
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਅੰਕੜੇ_ਇੰਦਰਾਜ;
    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        ਅੰਕੜੇ_ਇੰਦਰਾਜ = valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ;
    } ਨਹੀਂਤਾਂ {
        UError_ਕੋਡ int_ਸਥਿਤੀ = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_ਸਿੱਧਾ(ਰਸਤਾ, locale, &int_ਸਥਿਤੀ));
        ਜੇ(U_FAILURE(int_ਸਥਿਤੀ)) {
            // We failed to open the resource bundle we're aliasing to.
            *ਸਥਿਤੀ = int_ਸਥਿਤੀ;
            ਵਾਪਸ resB;
        }
        ਅੰਕੜੇ_ਇੰਦਰਾਜ = mainRes->f_ਅੰਕੜੇ;
    }

    ਅਚਲ ਅੱਖਰ* ਆਰਜ਼ੀ = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ(ਕੁੰਜੀ_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ.ਜੜ੍ਹ_Res;
        ਜੇ(container_Res_ਰਸਤਾ) {
            chAlias.clear().append(container_Res_ਰਸਤਾ, *ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            ਅੱਖਰ *a_ਕੁੰਜੀ = chAlias.ਅੰਕੜੇ();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_ਲੱਭੋ_Resource(&ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ, r, &a_ਕੁੰਜੀ, &ਆਰਜ਼ੀ);
        }
        ਜੇ(ਕੁੰਜੀ) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(ਕੁੰਜੀ, *ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            ਅੱਖਰ *a_ਕੁੰਜੀ = chAlias.ਅੰਕੜੇ();
            r = res_ਲੱਭੋ_Resource(&ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ, r, &a_ਕੁੰਜੀ, &ਆਰਜ਼ੀ);
        } ਨਹੀਂਤਾਂ ਜੇ(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            ਜੇ(URES_IS_ਸਾਰਣੀ(type)) {
                ਅਚਲ ਅੱਖਰ *a_ਕੁੰਜੀ;
                r = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਸੂਚਕਾਂਕ(&ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ, r, idx, &a_ਕੁੰਜੀ);
            } ਨਹੀਂਤਾਂ { /* array */
                r = res_get_Array_ਵਸਤੂ(&ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ, r, idx);
            }
        }
        ਜੇ(r != RES_BOGUS) {
            resB = init_resb_ਨਤੀਜਾ(
                ਅੰਕੜੇ_ਇੰਦਰਾਜ, r, ਆਰਜ਼ੀ, -1, valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ, ਖਾਲੀ_ਸੂਚਕ, recursion_ਡੂੰਘਾਈ+1,
                resB, ਸਥਿਤੀ);
        } ਨਹੀਂਤਾਂ {
            *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString ਰਸਤਾ_Buf(ਕੁੰਜੀ_ਰਸਤਾ, *ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਅੱਖਰ *my_ਰਸਤਾ = ਰਸਤਾ_Buf.ਅੰਕੜੇ();
        container_Res_ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
        // Now we have fallback following here.
        ਲਈ(;;) {
            r = ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ.ਜੜ੍ਹ_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            ਜਦੋਂ(*my_ਰਸਤਾ && U_SUCCESS(*ਸਥਿਤੀ)) {
                r = res_ਲੱਭੋ_Resource(&(ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ), r, &my_ਰਸਤਾ, &ਆਰਜ਼ੀ);
                ਜੇ(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    ਰੋਕੋ;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_ਨਤੀਜਾ(
                    ਅੰਕੜੇ_ਇੰਦਰਾਜ, r, ਆਰਜ਼ੀ, -1,
                    valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ, container_Res_ਰਸਤਾ, recursion_ਡੂੰਘਾਈ+1,
                    resB, ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
                    ਰੋਕੋ;
                }
                ਜੇ (ਆਰਜ਼ੀ == ਖਾਲੀ_ਸੂਚਕ || uprv_strcmp(ਕੁੰਜੀ_ਰਸਤਾ, ਆਰਜ਼ੀ) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_ਰਸਤਾ(resB);
                    ures_append_Res_ਰਸਤਾ(resB, ਕੁੰਜੀ_ਰਸਤਾ, static_cast<int32_t>(uprv_strlen(ਕੁੰਜੀ_ਰਸਤਾ)), ਸਥਿਤੀ);
                    ਜੇ(resB->f_Res_ਰਸਤਾ[resB->f_Res_ਰਸਤਾ_Len-1] != RES_ਰਸਤਾ_SEPARATOR) {
                        ures_append_Res_ਰਸਤਾ(resB, RES_ਰਸਤਾ_SEPARATOR_S, 1, ਸਥਿਤੀ);
                    }
                    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
                        ਰੋਕੋ;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                ਅੰਕੜੇ_ਇੰਦਰਾਜ = resB->f_ਅੰਕੜੇ;
                container_Res_ਰਸਤਾ = resB->f_Res_ਰਸਤਾ;
            }
            ਜੇ (U_FAILURE(*ਸਥਿਤੀ) || r != RES_BOGUS) {
                ਰੋਕੋ;
            }
            // Fall back to the parent bundle, if there is one.
            ਅੰਕੜੇ_ਇੰਦਰਾਜ = ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਮਾਪਾ;
            ਜੇ (ਅੰਕੜੇ_ਇੰਦਰਾਜ == ਖਾਲੀ_ਸੂਚਕ) {
                *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
                ਰੋਕੋ;
            }
            // Copy the same keyPath again.
            my_ਰਸਤਾ = ਰਸਤਾ_Buf.ਅੰਕੜੇ();
            uprv_strcpy(my_ਰਸਤਾ, ਕੁੰਜੀ_ਰਸਤਾ);
        }
    }
    ਜੇ(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    ਵਾਪਸ resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_ਨਤੀਜਾ(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* ਅੰਕੜੇ_ਇੰਦਰਾਜ,
                                  Resource r,
                                  ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                  int32_t idx,
                                  UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ,
                                  ਅਚਲ ਅੱਖਰ* container_Res_ਰਸਤਾ,
                                  int32_t recursion_ਡੂੰਘਾਈ,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ਕੋਡ* ਸਥਿਤੀ) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ resB;
    }
    ਜੇ (valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        ਜੇ(recursion_ਡੂੰਘਾਈ >= URES_MAX_ALIAS_LEVEL) {
            *ਸਥਿਤੀ = U_TOO_MANY_ALIASES_ERROR;
            ਵਾਪਸ resB;
        }
        ਵਾਪਸ get_Alias_ਨਿਸ਼ਾਨਾ_As_Resource_Bundle(
            ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ, r, ਕੁੰਜੀ, idx,
            valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ, container_Res_ਰਸਤਾ, recursion_ਡੂੰਘਾਈ, resB, ਸਥਿਤੀ);
    }
    ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(ਆਕਾਰ(UResourceBundle)));
        ਜੇ (resB == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ures_ਸੈੱਟ_Is_ਢੇਰ_Object(resB, ਝੂਠ);
        resB->f_Res_ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
        resB->f_Res_ਰਸਤਾ_Len = 0;
    } ਨਹੀਂਤਾਂ {
        ਜੇ(resB->f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
            ਇੰਦਰਾਜ_Close(resB->f_ਅੰਕੜੇ);
        }
        ਜੇ(resB->f_ਸੰਸਕਰਣ != ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(resB->f_ਸੰਸਕਰਣ);
        }
        /* 
        weiv: if stack object was passed in, it doesn't really need to be reinited,
        since the purpose of initing is to remove stack junk. However, at this point 
        we would not do anything to an allocated object, so stack object should be
        treated the same
        */
        /*
        if(ures_isStackObject(resB) != false) {
        ures_initStackObject(resB);
        }
        */
        ਜੇ(container_Res_ਰਸਤਾ != resB->f_Res_ਰਸਤਾ) {
            ures_free_Res_ਰਸਤਾ(resB);
        }
    }
    resB->f_ਅੰਕੜੇ = ਅੰਕੜੇ_ਇੰਦਰਾਜ;
    ਇੰਦਰਾਜ_Increase(resB->f_ਅੰਕੜੇ);
    resB->fHasFallback = ਝੂਠ;
    resB->f_Is_ਸਿਖਰ_Level = ਝੂਠ;
    resB->f_ਸੂਚਕਾਂਕ = -1;
    resB->f_ਕੁੰਜੀ = ਕੁੰਜੀ; 
    resB->f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ = valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ;
    ਜੇ(container_Res_ਰਸਤਾ != resB->f_Res_ਰਸਤਾ) {
        ures_append_Res_ਰਸਤਾ(
            resB, container_Res_ਰਸਤਾ, static_cast<int32_t>(uprv_strlen(container_Res_ਰਸਤਾ)), ਸਥਿਤੀ);
    }
    ਜੇ(ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ) {
        ures_append_Res_ਰਸਤਾ(resB, ਕੁੰਜੀ, static_cast<int32_t>(uprv_strlen(ਕੁੰਜੀ)), ਸਥਿਤੀ);
        ਜੇ(resB->f_Res_ਰਸਤਾ_Len > 0 && resB->f_Res_ਰਸਤਾ[resB->f_Res_ਰਸਤਾ_Len-1] != RES_ਰਸਤਾ_SEPARATOR) {
            ures_append_Res_ਰਸਤਾ(resB, RES_ਰਸਤਾ_SEPARATOR_S, 1, ਸਥਿਤੀ);
        }
    } ਨਹੀਂਤਾਂ ਜੇ(idx >= 0) {
        ਅੱਖਰ buf[256];
        int32_t len = T_CString_integer_ਮੰਜ਼ਿਲ_String(buf, idx, 10);
        ures_append_Res_ਰਸਤਾ(resB, buf, len, ਸਥਿਤੀ);
        ਜੇ(resB->f_Res_ਰਸਤਾ_Len > 0 && resB->f_Res_ਰਸਤਾ[resB->f_Res_ਰਸਤਾ_Len-1] != RES_ਰਸਤਾ_SEPARATOR) {
            ures_append_Res_ਰਸਤਾ(resB, RES_ਰਸਤਾ_SEPARATOR_S, 1, ਸਥਿਤੀ);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t ਵਰਤਿਆ_Len = ((resB->fResBuf == resB->f_Res_ਰਸਤਾ) ? resB->f_Res_ਰਸਤਾ_Len : 0);
        uprv_memset(resB->fResBuf + ਵਰਤਿਆ_Len, 0, ਆਕਾਰ(resB->fResBuf) - ਵਰਤਿਆ_Len);
    }

    resB->f_ਸੰਸਕਰਣ = ਖਾਲੀ_ਸੂਚਕ;
    resB->fRes = r;
    resB->f_ਮਾਪ = res_ਗਿਣਤੀ_Array_ਚੀਜ਼ਾਂ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes);
    ResourceTracer(resB).trace("get");
    ਵਾਪਸ resB;
}

UResourceBundle* init_resb_ਨਤੀਜਾ(UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* ਅੰਕੜੇ_ਇੰਦਰਾਜ,
                                  Resource r,
                                  ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ਅਚਲ UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਵਾਪਸ init_resb_ਨਤੀਜਾ(
        ਅੰਕੜੇ_ਇੰਦਰਾਜ, r, ਕੁੰਜੀ, idx,
        container->f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ, container->f_Res_ਰਸਤਾ, 0, resB, ਸਥਿਤੀ);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ਅਚਲ UResourceBundle* original,
                               UError_ਕੋਡ* ਸਥਿਤੀ) {
    UBool is_ਢੇਰ_Object;
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ) || r == original) {
        ਵਾਪਸ r;
    }
    ਜੇ(original != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(r == ਖਾਲੀ_ਸੂਚਕ) {
            is_ਢੇਰ_Object = ਝੂਠ;
            r = static_cast<UResourceBundle*>(uprv_malloc(ਆਕਾਰ(UResourceBundle)));
            /* test for nullptr */
            ਜੇ (r == ਖਾਲੀ_ਸੂਚਕ) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        } ਨਹੀਂਤਾਂ {
            is_ਢੇਰ_Object = ures_is_ਢੇਰ_Object(r);
            ures_closeBundle(r, ਝੂਠ);
        }
        uprv_memcpy(r, original, ਆਕਾਰ(UResourceBundle));
        r->f_Res_ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
        r->f_Res_ਰਸਤਾ_Len = 0;
        ਜੇ(original->f_Res_ਰਸਤਾ) {
            ures_append_Res_ਰਸਤਾ(r, original->f_Res_ਰਸਤਾ, original->f_Res_ਰਸਤਾ_Len, ਸਥਿਤੀ);
        }
        ures_ਸੈੱਟ_Is_ਢੇਰ_Object(r, is_ਢੇਰ_Object);
        ਜੇ(r->f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
            ਇੰਦਰਾਜ_Increase(r->f_ਅੰਕੜੇ);
        }
    }
    ਵਾਪਸ r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ਅਚਲ char16_t* U_EXPORT2 ures_getString(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਅਚਲ char16_t *s;
    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    s = res_getString({resB}, &resB->get_Res_ਅੰਕੜੇ(), resB->fRes, len);
    ਜੇ (s == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ s;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ*
ures_ਮੰਜ਼ਿਲ_UTF8_String(ਅਚਲ char16_t* s16,
                  int32_t length16,
                  ਅੱਖਰ* dest U_LIFETIME_BOUND,
                  int32_t* p_ਲੰਮਾਈ,
                  UBool forceCopy,
                  UError_ਕੋਡ* ਸਥਿਤੀ) {
    int32_t capacity;

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (p_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
        capacity = *p_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        capacity = 0;
    }
    ਜੇ (capacity < 0 || (capacity > 0 && dest == ਖਾਲੀ_ਸੂਚਕ)) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (length16 == 0) {
        /* empty string, return as read-only pointer */
        ਜੇ (p_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
            *p_ਲੰਮਾਈ = 0;
        }
        ਜੇ (forceCopy) {
            u_terminateChars(dest, capacity, 0, ਸਥਿਤੀ);
            ਵਾਪਸ dest;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ "";
        }
    } ਨਹੀਂਤਾਂ {
        /* We need to transform the string to the destination buffer. */
        ਜੇ (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            ਵਾਪਸ u_str_ਮੰਜ਼ਿਲ_UTF8(ਖਾਲੀ_ਸੂਚਕ, 0, p_ਲੰਮਾਈ, s16, length16, ਸਥਿਤੀ);
        }
        ਜੇ (!forceCopy && (length16 <= 0x2aaaaaaa)) {
            /*
             * We know the string will fit into dest because each char16_t turns
             * into at most three UTF-8 bytes. Fill the latter part of dest
             * so that callers do not expect to use dest as a string pointer,
             * hopefully leading to more robust code for when resource bundles
             * may store UTF-8 natively.
             * (In which case dest would not be used at all.)
             *
             * We do not do this if forceCopy=true because then the caller
             * expects the string to start exactly at dest.
             *
             * The test above for <= 0x2aaaaaaa prevents overflows.
             * The +1 is for the NUL terminator.
             */
            int32_t max_ਲੰਮਾਈ_2 = 3 * length16 + 1;
            ਜੇ (capacity > max_ਲੰਮਾਈ_2) {
                dest += capacity - max_ਲੰਮਾਈ_2;
                capacity = max_ਲੰਮਾਈ_2;
            }
        }
        ਵਾਪਸ u_str_ਮੰਜ਼ਿਲ_UTF8(dest, capacity, p_ਲੰਮਾਈ, s16, length16, ਸਥਿਤੀ);
    }
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_getUTF8String(ਅਚਲ UResourceBundle* resB,
                   ਅੱਖਰ* dest U_LIFETIME_BOUND,
                   int32_t* p_ਲੰਮਾਈ,
                   UBool forceCopy,
                   UError_ਕੋਡ* ਸਥਿਤੀ) {
    int32_t length16;
    ਅਚਲ char16_t *s16 = ures_getString(resB, &length16, ਸਥਿਤੀ);
    ਵਾਪਸ ures_ਮੰਜ਼ਿਲ_UTF8_String(s16, length16, dest, p_ਲੰਮਾਈ, forceCopy, ਸਥਿਤੀ);
}

U_CAPI ਅਚਲ uint8_t* U_EXPORT2
ures_get_ਦੁਆਧਾਰੀ(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_ਕੋਡ* ਸਥਿਤੀ) {
  ਅਚਲ uint8_t *p;
  ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  p = res_get_ਦੁਆਧਾਰੀ({resB}, &resB->get_Res_ਅੰਕੜੇ(), resB->fRes, len);
  ਜੇ (p == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
  }
  ਵਾਪਸ p;
}

U_CAPI ਅਚਲ int32_t* U_EXPORT2
ures_get_Int_ਸਦਿਸ਼(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_ਕੋਡ* ਸਥਿਤੀ) {
  ਅਚਲ int32_t *p;
  ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  p = res_get_Int_ਸਦਿਸ਼({resB}, &resB->get_Res_ਅੰਕੜੇ(), resB->fRes, len);
  ਜੇ (p == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
  }
  ਵਾਪਸ p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ਅਚਲ UResourceBundle* resB, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ 0xffffffff;
  }
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0xffffffff;
  }
  ਜੇ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
    ਵਾਪਸ 0xffffffff;
  }
  ਵਾਪਸ res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ਅਚਲ UResourceBundle* resB, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ 0xffffffff;
  }
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0xffffffff;
  }
  ਜੇ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
    ਵਾਪਸ 0xffffffff;
  }
  ਵਾਪਸ res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ਅਚਲ UResourceBundle *resB) {
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ URES_NONE;
  }
  ਵਾਪਸ res_get_ਜਨਤਕ_Type(resB->fRes);
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_get_ਕੁੰਜੀ(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਵਾਪਸ(resB->f_ਕੁੰਜੀ);
}

U_CAPI int32_t U_EXPORT2 ures_get_ਮਾਪ(ਅਚਲ UResourceBundle *resB) {
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ 0;
  }
  
  ਵਾਪਸ resB->f_ਮਾਪ;
}

ਸਥਿਰ ਅਚਲ char16_t*
ures_getStringWithAlias(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_ਸੂਚਕਾਂਕ,
                        int32_t* len,
                        UError_ਕੋਡ* ਸਥਿਤੀ) {
  ਜੇ(RES_GET_TYPE(r) == URES_ALIAS) {
    ਅਚਲ char16_t* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    UResourceBundle *ਆਰਜ਼ੀ_Res = ures_get_By_ਸੂਚਕਾਂਕ(resB, s_ਸੂਚਕਾਂਕ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
    ਨਤੀਜਾ = ures_getString(ਆਰਜ਼ੀ_Res, len, ਸਥਿਤੀ);
    ures_close(ਆਰਜ਼ੀ_Res);
    ਵਾਪਸ ਨਤੀਜਾ;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ res_getString({resB, s_ਸੂਚਕਾਂਕ}, &resB->get_Res_ਅੰਕੜੇ(), r, len); 
  }
}

U_CAPI ਖਾਲੀ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ;
  }
  resB->f_ਸੂਚਕਾਂਕ = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_ਅਗਲਾ(ਅਚਲ UResourceBundle *resB) {
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ ਝੂਠ;
  }
  ਵਾਪਸ resB->f_ਸੂਚਕਾਂਕ < resB->f_ਮਾਪ-1;
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2
ures_get_ਅਗਲਾ_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ਅਚਲ ਅੱਖਰ** ਕੁੰਜੀ,
                   UError_ਕੋਡ* ਸਥਿਤੀ) {
  Resource r = RES_BOGUS;
  
  ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  
  ਜੇ(resB->f_ਸੂਚਕਾਂਕ == resB->f_ਮਾਪ-1) {
    *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
  } ਨਹੀਂਤਾਂ {
    resB->f_ਸੂਚਕਾਂਕ++;
    ਚੋਣ(RES_GET_TYPE(resB->fRes)) {
    ਹਾਲਤ URES_STRING:
    ਹਾਲਤ URES_STRING_V2:
      ਵਾਪਸ res_getString({resB}, &resB->get_Res_ਅੰਕੜੇ(), resB->fRes, len);
    ਹਾਲਤ URES_ਸਾਰਣੀ:
    ਹਾਲਤ URES_TABLE16:
    ਹਾਲਤ URES_TABLE32:
      r = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਸੂਚਕਾਂਕ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, resB->f_ਸੂਚਕਾਂਕ, ਕੁੰਜੀ);
      ਜੇ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ਵਾਪਸ ures_getStringWithAlias(resB, r, resB->f_ਸੂਚਕਾਂਕ, len, ਸਥਿਤੀ);
    ਹਾਲਤ URES_ARRAY:
    ਹਾਲਤ URES_ARRAY16:
      r = res_get_Array_ਵਸਤੂ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, resB->f_ਸੂਚਕਾਂਕ);
      ਜੇ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ਵਾਪਸ ures_getStringWithAlias(resB, r, resB->f_ਸੂਚਕਾਂਕ, len, ਸਥਿਤੀ);
    ਹਾਲਤ URES_ALIAS:
      ਵਾਪਸ ures_getStringWithAlias(resB, resB->fRes, resB->f_ਸੂਚਕਾਂਕ, len, ਸਥਿਤੀ);
    ਹਾਲਤ URES_INT:
    ਹਾਲਤ URES_ਦੁਆਧਾਰੀ:
    ਹਾਲਤ URES_INT_ਸਦਿਸ਼:
        *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    ਮੂਲ:
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
  }

  ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_ਅਗਲਾ_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_ਵਿੱਚ U_LIFETIME_BOUND,
                     UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ = ਖਾਲੀ_ਸੂਚਕ;
    Resource r = RES_BOGUS;

    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
            /*return nullptr;*/
            ਵਾਪਸ fill_ਵਿੱਚ;
    }
    ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            /*return nullptr;*/
            ਵਾਪਸ fill_ਵਿੱਚ;
    }

    ਜੇ(resB->f_ਸੂਚਕਾਂਕ == resB->f_ਮਾਪ-1) {
      *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } ਨਹੀਂਤਾਂ {
        resB->f_ਸੂਚਕਾਂਕ++;
        ਚੋਣ(RES_GET_TYPE(resB->fRes)) {
        ਹਾਲਤ URES_INT:
        ਹਾਲਤ URES_ਦੁਆਧਾਰੀ:
        ਹਾਲਤ URES_STRING:
        ਹਾਲਤ URES_STRING_V2:
        ਹਾਲਤ URES_INT_ਸਦਿਸ਼:
            ਵਾਪਸ ures_copyResb(fill_ਵਿੱਚ, resB, ਸਥਿਤੀ);
        ਹਾਲਤ URES_ਸਾਰਣੀ:
        ਹਾਲਤ URES_TABLE16:
        ਹਾਲਤ URES_TABLE32:
            r = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਸੂਚਕਾਂਕ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, resB->f_ਸੂਚਕਾਂਕ, &ਕੁੰਜੀ);
            ਜੇ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ਵਾਪਸ init_resb_ਨਤੀਜਾ(resB->f_ਅੰਕੜੇ, r, ਕੁੰਜੀ, resB->f_ਸੂਚਕਾਂਕ, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
        ਹਾਲਤ URES_ARRAY:
        ਹਾਲਤ URES_ARRAY16:
            r = res_get_Array_ਵਸਤੂ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, resB->f_ਸੂਚਕਾਂਕ);
            ਜੇ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ਵਾਪਸ init_resb_ਨਤੀਜਾ(resB->f_ਅੰਕੜੇ, r, ਕੁੰਜੀ, resB->f_ਸੂਚਕਾਂਕ, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
        ਮੂਲ:
            /*return nullptr;*/
            ਵਾਪਸ fill_ਵਿੱਚ;
        }
    }
    /*return nullptr;*/
    ਵਾਪਸ fill_ਵਿੱਚ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ਸੂਚਕਾਂਕ(ਅਚਲ UResourceBundle* resB,
                int32_t ਸੂਚਕਾਂਕ_R,
                UResourceBundle* fill_ਵਿੱਚ U_LIFETIME_BOUND,
                UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ = ਖਾਲੀ_ਸੂਚਕ;
    Resource r = RES_BOGUS;

    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        /*return nullptr;*/
        ਵਾਪਸ fill_ਵਿੱਚ;
    }
    ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        /*return nullptr;*/
        ਵਾਪਸ fill_ਵਿੱਚ;
    }

    ਜੇ(ਸੂਚਕਾਂਕ_R >= 0 && resB->f_ਮਾਪ > ਸੂਚਕਾਂਕ_R) {
        ਚੋਣ(RES_GET_TYPE(resB->fRes)) {
        ਹਾਲਤ URES_INT:
        ਹਾਲਤ URES_ਦੁਆਧਾਰੀ:
        ਹਾਲਤ URES_STRING:
        ਹਾਲਤ URES_STRING_V2:
        ਹਾਲਤ URES_INT_ਸਦਿਸ਼:
            ਵਾਪਸ ures_copyResb(fill_ਵਿੱਚ, resB, ਸਥਿਤੀ);
        ਹਾਲਤ URES_ਸਾਰਣੀ:
        ਹਾਲਤ URES_TABLE16:
        ਹਾਲਤ URES_TABLE32:
            r = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਸੂਚਕਾਂਕ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, ਸੂਚਕਾਂਕ_R, &ਕੁੰਜੀ);
            ਜੇ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ਵਾਪਸ init_resb_ਨਤੀਜਾ(resB->f_ਅੰਕੜੇ, r, ਕੁੰਜੀ, ਸੂਚਕਾਂਕ_R, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
        ਹਾਲਤ URES_ARRAY:
        ਹਾਲਤ URES_ARRAY16:
            r = res_get_Array_ਵਸਤੂ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, ਸੂਚਕਾਂਕ_R);
            ਜੇ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ਵਾਪਸ init_resb_ਨਤੀਜਾ(resB->f_ਅੰਕੜੇ, r, ਕੁੰਜੀ, ਸੂਚਕਾਂਕ_R, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
        ਮੂਲ:
            /*return nullptr;*/
            ਵਾਪਸ fill_ਵਿੱਚ;
        }
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    ਵਾਪਸ fill_ਵਿੱਚ;
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2
ures_get_String_By_ਸੂਚਕਾਂਕ(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t ਸੂਚਕਾਂਕ_S,
                      int32_t* len,
                      UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ = ਖਾਲੀ_ਸੂਚਕ;
    Resource r = RES_BOGUS;

    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(ਸੂਚਕਾਂਕ_S >= 0 && resB->f_ਮਾਪ > ਸੂਚਕਾਂਕ_S) {
        ਚੋਣ(RES_GET_TYPE(resB->fRes)) {
        ਹਾਲਤ URES_STRING:
        ਹਾਲਤ URES_STRING_V2:
            ਵਾਪਸ res_getString({resB}, &resB->get_Res_ਅੰਕੜੇ(), resB->fRes, len);
        ਹਾਲਤ URES_ਸਾਰਣੀ:
        ਹਾਲਤ URES_TABLE16:
        ਹਾਲਤ URES_TABLE32:
            r = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਸੂਚਕਾਂਕ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, ਸੂਚਕਾਂਕ_S, &ਕੁੰਜੀ);
            ਜੇ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ਵਾਪਸ ures_getStringWithAlias(resB, r, ਸੂਚਕਾਂਕ_S, len, ਸਥਿਤੀ);
        ਹਾਲਤ URES_ARRAY:
        ਹਾਲਤ URES_ARRAY16:
            r = res_get_Array_ਵਸਤੂ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, ਸੂਚਕਾਂਕ_S);
            ਜੇ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ਵਾਪਸ ures_getStringWithAlias(resB, r, ਸੂਚਕਾਂਕ_S, len, ਸਥਿਤੀ);
        ਹਾਲਤ URES_ALIAS:
            ਵਾਪਸ ures_getStringWithAlias(resB, resB->fRes, ਸੂਚਕਾਂਕ_S, len, ਸਥਿਤੀ);
        ਹਾਲਤ URES_INT:
        ਹਾਲਤ URES_ਦੁਆਧਾਰੀ:
        ਹਾਲਤ URES_INT_ਸਦਿਸ਼:
            *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
            ਰੋਕੋ;
        ਮੂਲ:
          /* must not occur */
          *ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
          ਰੋਕੋ;
        }
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_get_UTF8_String_By_ਸੂਚਕਾਂਕ(ਅਚਲ UResourceBundle* resB,
                          int32_t idx,
                          ਅੱਖਰ* dest U_LIFETIME_BOUND,
                          int32_t* p_ਲੰਮਾਈ,
                          UBool forceCopy,
                          UError_ਕੋਡ* ਸਥਿਤੀ) {
    int32_t length16;
    ਅਚਲ char16_t *s16 = ures_get_String_By_ਸੂਚਕਾਂਕ(resB, idx, &length16, ਸਥਿਤੀ);
    ਵਾਪਸ ures_ਮੰਜ਼ਿਲ_UTF8_String(s16, length16, dest, p_ਲੰਮਾਈ, forceCopy, ਸਥਿਤੀ);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_ਲੱਭੋ_Resource(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, UResourceBundle* fill_ਵਿੱਚ U_LIFETIME_BOUND, UError_ਕੋਡ* ਸਥਿਤੀ)
{
  UResourceBundle *first = ਖਾਲੀ_ਸੂਚਕ; 
  UResourceBundle *ਨਤੀਜਾ = fill_ਵਿੱਚ;
  ਅੱਖਰ *packageName = ਖਾਲੀ_ਸੂਚਕ;
  ਅੱਖਰ *ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource = ਖਾਲੀ_ਸੂਚਕ, *save = ਖਾਲੀ_ਸੂਚਕ;
  ਅੱਖਰ *locale = ਖਾਲੀ_ਸੂਚਕ, *locale_ਸਿਰਾ = ਖਾਲੀ_ਸੂਚਕ;
  int32_t length;

  ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  length = (int32_t)(uprv_strlen(ਰਸਤਾ)+1);
  save = ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource = (ਅੱਖਰ *)uprv_malloc(length*ਆਕਾਰ(ਅੱਖਰ));
  /* test for nullptr */
  ਜੇ(ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  uprv_memcpy(ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource, ਰਸਤਾ, length);

  locale = ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource;
  ਜੇ(*ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource == RES_ਰਸਤਾ_SEPARATOR) { /* there is a path specification */
    ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource++;
    packageName = ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource;
    ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource = uprv_strchr(ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource, RES_ਰਸਤਾ_SEPARATOR);
    ਜੇ(ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource == ਖਾਲੀ_ਸੂਚਕ) {
      *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    } ਨਹੀਂਤਾਂ {
      *ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource = 0;
      locale = ਰਸਤਾ_ਮੰਜ਼ਿਲ_Resource+1;
    }
  }

  locale_ਸਿਰਾ = uprv_strchr(locale, RES_ਰਸਤਾ_SEPARATOR);
  ਜੇ(locale_ਸਿਰਾ != ਖਾਲੀ_ਸੂਚਕ) {
    *locale_ਸਿਰਾ = 0;
  }

  first = ures_open(packageName, locale, ਸਥਿਤੀ);

  ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
    ਜੇ(locale_ਸਿਰਾ) {
      ਨਤੀਜਾ = ures_ਲੱਭੋ_Sub_Resource(first, locale_ਸਿਰਾ+1, fill_ਵਿੱਚ, ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = ures_copyResb(fill_ਵਿੱਚ, first, ਸਥਿਤੀ);
    }
    ures_close(first);
  }
  uprv_free(save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_ਲੱਭੋ_Sub_Resource(ਅਚਲ UResourceBundle* resB,
                     ਅੱਖਰ* ਰਸਤਾ,
                     UResourceBundle* fill_ਵਿੱਚ U_LIFETIME_BOUND,
                     UError_ਕੋਡ* ਸਥਿਤੀ)
{
  Resource res = RES_BOGUS;
  UResourceBundle *ਨਤੀਜਾ = fill_ਵਿੱਚ;
  ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ;

  ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  ਕਰੋ {
    res = res_ਲੱਭੋ_Resource(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, &ਰਸਤਾ, &ਕੁੰਜੀ); 
    ਜੇ(res != RES_BOGUS) {
        ਨਤੀਜਾ = init_resb_ਨਤੀਜਾ(resB->f_ਅੰਕੜੇ, res, ਕੁੰਜੀ, -1, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
        resB = ਨਤੀਜਾ;
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        ਰੋਕੋ;
    }
  } ਜਦੋਂ(*ਰਸਤਾ); /* there is more stuff in the path */

  ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2
ures_get_String_By_ਕੁੰਜੀ_With_Fallback(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND,
                                ਅਚਲ ਅੱਖਰ* ਵਿੱਚ_ਕੁੰਜੀ,
                                int32_t* len,
                                UError_ਕੋਡ* ਸਥਿਤੀ) {
    UResourceBundle ਢੇਰ_5;
    ਅਚਲ char16_t* retVal = ਖਾਲੀ_ਸੂਚਕ;
    ures_init_ਢੇਰ_Object(&ਢੇਰ_5);
    ures_get_By_ਕੁੰਜੀ_With_Fallback(resB, ਵਿੱਚ_ਕੁੰਜੀ, &ਢੇਰ_5, ਸਥਿਤੀ);
    int32_t length;
    retVal = ures_getString(&ਢੇਰ_5, &length, ਸਥਿਤੀ);
    ures_close(&ਢੇਰ_5);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (length == 3 && retVal[0] == ਖਾਲੀ_ਥਾਵਾਂ_ਸੈੱਟ && retVal[1] == ਖਾਲੀ_ਥਾਵਾਂ_ਸੈੱਟ && retVal[2] == ਖਾਲੀ_ਥਾਵਾਂ_ਸੈੱਟ ) {
        retVal = ਖਾਲੀ_ਸੂਚਕ;
        length = 0;
        *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
    }
    ਜੇ (len != ਖਾਲੀ_ਸੂਚਕ) {
        *len = length;
    }
    ਵਾਪਸ retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ਸਥਿਰ Resource get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ_ਰਸਤਾ(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource ਸਾਰਣੀ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ) {
  Resource resource = ਸਾਰਣੀ;  /* The current resource */
  icu::CharString ਰਸਤਾ;
  UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
  ਰਸਤਾ.append(ਕੁੰਜੀ, error_ਕੋਡ);
  ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ RES_BOGUS; }
  ਅੱਖਰ *ਰਸਤਾ_Part = ਰਸਤਾ.ਅੰਕੜੇ();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  ਜਦੋਂ (*ਰਸਤਾ_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    ਅੱਖਰ *ਅਗਲਾ_ਰਸਤਾ_Part = uprv_strchr(ਰਸਤਾ_Part, RES_ਰਸਤਾ_SEPARATOR);
    ਜੇ (ਅਗਲਾ_ਰਸਤਾ_Part != ਖਾਲੀ_ਸੂਚਕ) {
      *ਅਗਲਾ_ਰਸਤਾ_Part = 0;  /* Terminating null for this part of path. */
      ਅਗਲਾ_ਰਸਤਾ_Part++;
    } ਨਹੀਂਤਾਂ {
      ਅਗਲਾ_ਰਸਤਾ_Part = uprv_strchr(ਰਸਤਾ_Part, 0);
    }
    int32_t t;
    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ_P = ਰਸਤਾ_Part;
    resource = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ(p_Res_ਅੰਕੜੇ, resource, &t, &ਰਸਤਾ_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    ਰਸਤਾ_Part = ਅਗਲਾ_ਰਸਤਾ_Part; 
  }
  ਜੇ (*ਰਸਤਾ_Part) {
    ਵਾਪਸ RES_BOGUS;
  }
  ਵਾਪਸ resource;
}

ਸਥਿਰ ਖਾਲੀ create_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ* orig_Res_ਰਸਤਾ,
                       int32_t     orig_Res_ਰਸਤਾ_Len,
                       ਅਚਲ ਅੱਖਰ* res_ਰਸਤਾ,
                       int32_t     res_ਰਸਤਾ_Len,
                       ਅਚਲ ਅੱਖਰ* ਵਿੱਚ_ਕੁੰਜੀ,
                       CharString& ਰਸਤਾ,
                       UError_ਕੋਡ* ਸਥਿਤੀ) {
    // This is a utility function used by ures_getByKeyWithFallback() below.  This function builds a path from
    // resPath and inKey, returning the result in `path`.  Originally, this function just cleared `path` and
    // appended resPath and inKey to it, but that caused problems for horizontal inheritance.
    //
    // In normal cases, resPath is the same as origResPath, but if ures_getByKeyWithFallback() has followed an
    // alias, resPath may be different from origResPath.  Not only may the existing path elements be different,
    // but resPath may also have MORE path elements than origResPath did.  If it does, those additional path
    // elements SUPERSEDE the corresponding elements of inKey.  So this code counts the number of elements in
    // resPath and origResPath and, for each path element in resPath that doesn't have a counterpart in origResPath,
    // deletes a path element from the beginning of inKey.  The remainder of inKey is then appended to
    // resPath to form the result.  (We're not using uprv_strchr() here because resPath and origResPath may
    // not be zero-terminated.)
    ਰਸਤਾ.clear();
    ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ = ਵਿੱਚ_ਕੁੰਜੀ;
    ਜੇ (res_ਰਸਤਾ_Len > 0) {
        ਰਸਤਾ.append(res_ਰਸਤਾ, res_ਰਸਤਾ_Len, *ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
            ਅਚਲ ਅੱਖਰ* res_ਰਸਤਾ_ਹੱਦਬੰਦੀ = res_ਰਸਤਾ + res_ਰਸਤਾ_Len;
            ਅਚਲ ਅੱਖਰ* orig_Res_ਰਸਤਾ_ਹੱਦਬੰਦੀ = orig_Res_ਰਸਤਾ + orig_Res_ਰਸਤਾ_Len;
            ਅਚਲ ਅੱਖਰ* res_ਰਸਤਾ_Ptr = res_ਰਸਤਾ;
            ਅਚਲ ਅੱਖਰ* orig_Res_ਰਸਤਾ_Ptr = orig_Res_ਰਸਤਾ;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            ਜਦੋਂ (orig_Res_ਰਸਤਾ_Ptr < orig_Res_ਰਸਤਾ_ਹੱਦਬੰਦੀ && res_ਰਸਤਾ_Ptr < res_ਰਸਤਾ_ਹੱਦਬੰਦੀ) {
                ਜਦੋਂ (orig_Res_ਰਸਤਾ_Ptr < orig_Res_ਰਸਤਾ_ਹੱਦਬੰਦੀ && *orig_Res_ਰਸਤਾ_Ptr != RES_ਰਸਤਾ_SEPARATOR) {
                    ++orig_Res_ਰਸਤਾ_Ptr;
                }
                ਜੇ (orig_Res_ਰਸਤਾ_Ptr < orig_Res_ਰਸਤਾ_ਹੱਦਬੰਦੀ && *orig_Res_ਰਸਤਾ_Ptr == RES_ਰਸਤਾ_SEPARATOR) {
                    ++orig_Res_ਰਸਤਾ_Ptr;
                }
                ਜਦੋਂ (res_ਰਸਤਾ_Ptr < res_ਰਸਤਾ_ਹੱਦਬੰਦੀ && *res_ਰਸਤਾ_Ptr != RES_ਰਸਤਾ_SEPARATOR) {
                    ++res_ਰਸਤਾ_Ptr;
                }
                ਜੇ (res_ਰਸਤਾ_Ptr < res_ਰਸਤਾ_ਹੱਦਬੰਦੀ && *res_ਰਸਤਾ_Ptr == RES_ਰਸਤਾ_SEPARATOR) {
                    ++res_ਰਸਤਾ_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            ਜਦੋਂ (res_ਰਸਤਾ_Ptr < res_ਰਸਤਾ_ਹੱਦਬੰਦੀ && *ਕੁੰਜੀ != '\0') {
                ਜਦੋਂ (res_ਰਸਤਾ_Ptr < res_ਰਸਤਾ_ਹੱਦਬੰਦੀ && *res_ਰਸਤਾ_Ptr != RES_ਰਸਤਾ_SEPARATOR) {
                    ++res_ਰਸਤਾ_Ptr;
                }
                ਜੇ (res_ਰਸਤਾ_Ptr < res_ਰਸਤਾ_ਹੱਦਬੰਦੀ && *res_ਰਸਤਾ_Ptr == RES_ਰਸਤਾ_SEPARATOR) {
                    ++res_ਰਸਤਾ_Ptr;
                }
                ਜਦੋਂ (*ਕੁੰਜੀ != '\0' && *ਕੁੰਜੀ != RES_ਰਸਤਾ_SEPARATOR) {
                    ++ਕੁੰਜੀ;
                }
                ਜੇ (*ਕੁੰਜੀ == RES_ਰਸਤਾ_SEPARATOR) {
                    ++ਕੁੰਜੀ;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        ਰਸਤਾ.append(ਕੁੰਜੀ, *ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        ਰਸਤਾ.append(ਵਿੱਚ_ਕੁੰਜੀ, *ਸਥਿਤੀ);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ਕੁੰਜੀ_With_Fallback(ਅਚਲ UResourceBundle* resB,
                          ਅਚਲ ਅੱਖਰ* ਵਿੱਚ_ਕੁੰਜੀ,
                          UResourceBundle* fill_ਵਿੱਚ U_LIFETIME_BOUND,
                          UError_ਕੋਡ* ਸਥਿਤੀ) {
    Resource res = RES_BOGUS, ਜੜ੍ਹ_Res = RES_BOGUS;
    UResourceBundle *helper = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ fill_ਵਿੱਚ;
    }
    ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ fill_ਵਿੱਚ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ਜੇ(URES_IS_ਸਾਰਣੀ(type)) {
        ਅਚਲ ਅੱਖਰ* orig_Res_ਰਸਤਾ = resB->f_Res_ਰਸਤਾ;
        int32_t orig_Res_ਰਸਤਾ_Len = resB->f_Res_ਰਸਤਾ_Len;
        res = get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ_ਰਸਤਾ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, ਵਿੱਚ_ਕੁੰਜੀ);
        ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ = ਵਿੱਚ_ਕੁੰਜੀ;
        ਤਾਰਕਿਕ did_ਜੜ੍ਹ_Once = ਝੂਠ;
        ਜੇ(res == RES_BOGUS) {
            UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਅੰਕੜੇ_ਇੰਦਰਾਜ = resB->f_ਅੰਕੜੇ;
            CharString ਰਸਤਾ;
            ਅੱਖਰ *my_ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
            ਅਚਲ ਅੱਖਰ* res_ਰਸਤਾ = resB->f_Res_ਰਸਤਾ;
            int32_t len = resB->f_Res_ਰਸਤਾ_Len;
            ਜਦੋਂ(res == RES_BOGUS && (ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਮਾਪਾ != ਖਾਲੀ_ਸੂਚਕ || !did_ਜੜ੍ਹ_Once)) { /* Otherwise, we'll look in parents */
                ਜੇ (ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਮਾਪਾ != ਖਾਲੀ_ਸੂਚਕ) {
                    ਅੰਕੜੇ_ਇੰਦਰਾਜ = ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਮਾਪਾ;
                } ਨਹੀਂਤਾਂ {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_ਜੜ੍ਹ_Once = ਸੱਚ;
                }
                ਜੜ੍ਹ_Res = ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ.ਜੜ੍ਹ_Res;

                ਜੇ(ਅੰਕੜੇ_ਇੰਦਰਾਜ->fBogus == U_ZERO_ERROR) {
                    create_ਰਸਤਾ(orig_Res_ਰਸਤਾ, orig_Res_ਰਸਤਾ_Len, res_ਰਸਤਾ, len, ਵਿੱਚ_ਕੁੰਜੀ, ਰਸਤਾ, ਸਥਿਤੀ);
                    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
                        ures_close(helper);
                        ਵਾਪਸ fill_ਵਿੱਚ;
                    }
                    my_ਰਸਤਾ = ਰਸਤਾ.ਅੰਕੜੇ();
                    ਕੁੰਜੀ = ਵਿੱਚ_ਕੁੰਜੀ;
                    ਕਰੋ {
                        res = res_ਲੱਭੋ_Resource(&(ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ), ਜੜ੍ਹ_Res, &my_ਰਸਤਾ, &ਕੁੰਜੀ);
                        ਜੇ (RES_GET_TYPE(res) == URES_ALIAS && *my_ਰਸਤਾ) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_ਨਤੀਜਾ(ਅੰਕੜੇ_ਇੰਦਰਾਜ, res, ਖਾਲੀ_ਸੂਚਕ, -1, resB, helper, ਸਥਿਤੀ);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            ਜੇ(helper) {
                              ਅੰਕੜੇ_ਇੰਦਰਾਜ = helper->f_ਅੰਕੜੇ;
                              ਜੜ੍ਹ_Res = helper->fRes;
                              res_ਰਸਤਾ = helper->f_Res_ਰਸਤਾ;
                              len = helper->f_Res_ਰਸਤਾ_Len;

                            } ਨਹੀਂਤਾਂ {
                              ਰੋਕੋ;
                            }
                        } ਨਹੀਂਤਾਂ ਜੇ (res == RES_BOGUS) {
                            ਰੋਕੋ;
                        }
                    } ਜਦੋਂ(*my_ਰਸਤਾ); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            ਜੇ(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                ਜੇ(uprv_strcmp(ਅੰਕੜੇ_ਇੰਦਰਾਜ->fName, uloc_getDefault())==0 || uprv_strcmp(ਅੰਕੜੇ_ਇੰਦਰਾਜ->fName, k_ਜੜ੍ਹ_Locale_Name)==0) {
                    *ਸਥਿਤੀ = U_USING_DEFAULT_WARNING;
                } ਨਹੀਂਤਾਂ {
                    *ਸਥਿਤੀ = U_USING_FALLBACK_WARNING;
                }

                fill_ਵਿੱਚ = init_resb_ਨਤੀਜਾ(ਅੰਕੜੇ_ਇੰਦਰਾਜ, res, ਕੁੰਜੀ, -1, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
                ਜੇ (res_ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ) {
                    create_ਰਸਤਾ(orig_Res_ਰਸਤਾ, orig_Res_ਰਸਤਾ_Len, res_ਰਸਤਾ, len, ਵਿੱਚ_ਕੁੰਜੀ, ਰਸਤਾ, ਸਥਿਤੀ);
                } ਨਹੀਂਤਾਂ {
                    ਅਚਲ ਅੱਖਰ* separator = ਖਾਲੀ_ਸੂਚਕ;
                    ਜੇ (fill_ਵਿੱਚ->f_Res_ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ) {
                        separator = uprv_strchr(fill_ਵਿੱਚ->f_Res_ਰਸਤਾ, RES_ਰਸਤਾ_SEPARATOR);
                    }
                    ਜੇ (separator != ਖਾਲੀ_ਸੂਚਕ && separator[1] != '\0') {
                        create_ਰਸਤਾ(orig_Res_ਰਸਤਾ, orig_Res_ਰਸਤਾ_Len, fill_ਵਿੱਚ->f_Res_ਰਸਤਾ,
                                   static_cast<int32_t>(uprv_strlen(fill_ਵਿੱਚ->f_Res_ਰਸਤਾ)), ਵਿੱਚ_ਕੁੰਜੀ, ਰਸਤਾ, ਸਥਿਤੀ);
                    } ਨਹੀਂਤਾਂ {
                        create_ਰਸਤਾ(orig_Res_ਰਸਤਾ, orig_Res_ਰਸਤਾ_Len, "", 0, ਵਿੱਚ_ਕੁੰਜੀ, ਰਸਤਾ, ਸਥਿਤੀ);
                    }
                }
                ures_free_Res_ਰਸਤਾ(fill_ਵਿੱਚ);
                ures_append_Res_ਰਸਤਾ(fill_ਵਿੱਚ, ਰਸਤਾ.ਅੰਕੜੇ(), ਰਸਤਾ.length(), ਸਥਿਤੀ);
                ਜੇ(fill_ਵਿੱਚ->f_Res_ਰਸਤਾ[fill_ਵਿੱਚ->f_Res_ਰਸਤਾ_Len-1] != RES_ਰਸਤਾ_SEPARATOR) {
                    ures_append_Res_ਰਸਤਾ(fill_ਵਿੱਚ, RES_ਰਸਤਾ_SEPARATOR_S, 1, ਸਥਿਤੀ);
                }
            } ਨਹੀਂਤਾਂ {
                *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
            }
        } ਨਹੀਂਤਾਂ {
            fill_ਵਿੱਚ = init_resb_ਨਤੀਜਾ(resB->f_ਅੰਕੜੇ, res, ਕੁੰਜੀ, -1, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
        }
    } 
    ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    ਵਾਪਸ fill_ਵਿੱਚ;
}

ਨਾਮਾਕਾਸ਼ {

ਖਾਲੀ get_All_ਚੀਜ਼ਾਂ_With_Fallback(
        ਅਚਲ UResourceBundle *bundle, Resource_ਅੰਕੜੇ_ਮੁੱਲ &value,
        ResourceSink &sink, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    // We recursively enumerate child-first,
    // only storing parent items in the absence of child items.
    // The sink needs to store a placeholder value for the no-fallback/no-inheritance marker
    // to prevent a parent item from being stored.
    //
    // It would be possible to recursively enumerate parent-first,
    // overriding parent items with child items.
    // When the sink sees the no-fallback/no-inheritance marker,
    // then it would remove the parent's item.
    // We would deserialize parent values even though they are overridden in a child bundle.
    value.ਸੈੱਟ_ਅੰਕੜੇ(bundle->get_Res_ਅੰਕੜੇ());
    value.ਸੈੱਟ_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ(bundle->f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ);
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਮਾਪਾ_ਇੰਦਰਾਜ = bundle->f_ਅੰਕੜੇ->f_ਮਾਪਾ;
    UBool has_ਮਾਪਾ = ਮਾਪਾ_ਇੰਦਰਾਜ != ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(ਮਾਪਾ_ਇੰਦਰਾਜ->fBogus);
    value.ਸੈੱਟ_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_ਕੁੰਜੀ, value, !has_ਮਾਪਾ, error_ਕੋਡ);
    ਜੇ (has_ਮਾਪਾ) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ਢੇਰ_UResource_Bundle ਮਾਪਾ_Bundle;
        UResourceBundle &ਮਾਪਾ_Ref = ਮਾਪਾ_Bundle.ref();
        ਮਾਪਾ_Ref.f_ਅੰਕੜੇ = ਮਾਪਾ_ਇੰਦਰਾਜ;
        ਮਾਪਾ_Ref.f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ = bundle->f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ;
        ਮਾਪਾ_Ref.fHasFallback = !ਮਾਪਾ_Ref.get_Res_ਅੰਕੜੇ().noFallback;
        ਮਾਪਾ_Ref.f_Is_ਸਿਖਰ_Level = ਸੱਚ;
        ਮਾਪਾ_Ref.fRes = ਮਾਪਾ_Ref.get_Res_ਅੰਕੜੇ().ਜੜ੍ਹ_Res;
        ਮਾਪਾ_Ref.f_ਮਾਪ = res_ਗਿਣਤੀ_Array_ਚੀਜ਼ਾਂ(&ਮਾਪਾ_Ref.get_Res_ਅੰਕੜੇ(), ਮਾਪਾ_Ref.fRes);
        ਮਾਪਾ_Ref.f_ਸੂਚਕਾਂਕ = -1;
        ਇੰਦਰਾਜ_Increase(ਮਾਪਾ_ਇੰਦਰਾਜ);

        // Look up the container item in the parent bundle.
        ਢੇਰ_UResource_Bundle containerBundle;
        ਅਚਲ UResourceBundle *rb;
        UError_ਕੋਡ ਰਸਤਾ_Error_ਕੋਡ = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        ਜੇ (bundle->f_Res_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ || *bundle->f_Res_ਰਸਤਾ == 0) {
            rb = ਮਾਪਾ_Bundle.getAlias();
        } ਨਹੀਂਤਾਂ {
            rb = ures_get_By_ਕੁੰਜੀ_With_Fallback(ਮਾਪਾ_Bundle.getAlias(), bundle->f_Res_ਰਸਤਾ,
                                           containerBundle.getAlias(), &ਰਸਤਾ_Error_ਕੋਡ);
        }
        ਜੇ (U_SUCCESS(ਰਸਤਾ_Error_ਕੋਡ)) {
            get_All_ਚੀਜ਼ਾਂ_With_Fallback(rb, value, sink, error_ਕੋਡ);
        }
    }
}

ਬਣਤਰ GetAllChildrenSink : ਜਨਤਕ ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    ਆਭਾਸੀ ~GetAllChildrenSink() override;
    ਆਭਾਸੀ ਖਾਲੀ put(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, Resource_ਮੁੱਲ &value, UBool is_ਜੜ੍ਹ,
           UError_ਕੋਡ &error_ਕੋਡ) override {
        Resource_ਸਾਰਣੀ ਚੀਜ਼ਾਂ_ਸਾਰਣੀ = value.get_ਸਾਰਣੀ(error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        ਲਈ (int32_t i = 0; ਚੀਜ਼ਾਂ_ਸਾਰਣੀ.get_ਕੁੰਜੀ_And_ਮੁੱਲ(i, ਕੁੰਜੀ, value); ++i) {
            ਜੇ (value.getType() == URES_ALIAS) {
                Resource_ਅੰਕੜੇ_ਮੁੱਲ& rdv = static_cast<Resource_ਅੰਕੜੇ_ਮੁੱਲ&>(value);
                ਢੇਰ_UResource_Bundle ਢੇਰ_ਆਰਜ਼ੀ_Bundle;
                UResourceBundle* aliasRB = get_Alias_ਨਿਸ਼ਾਨਾ_As_Resource_Bundle(rdv.get_ਅੰਕੜੇ(), rdv.getResource(), ਖਾਲੀ_ਸੂਚਕ, -1,
                                                                          rdv.get_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ(), ਖਾਲੀ_ਸੂਚਕ, 0,
                                                                          ਢੇਰ_ਆਰਜ਼ੀ_Bundle.getAlias(), &error_ਕੋਡ);
                ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
                    Resource_ਅੰਕੜੇ_ਮੁੱਲ aliased_ਮੁੱਲ;
                    aliased_ਮੁੱਲ.ਸੈੱਟ_ਅੰਕੜੇ(aliasRB->get_Res_ਅੰਕੜੇ());
                    aliased_ਮੁੱਲ.ਸੈੱਟ_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ(aliasRB->f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ);
                    aliased_ਮੁੱਲ.ਸੈੱਟ_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    ਜੇ (aliased_ਮੁੱਲ.getType() != URES_ਸਾਰਣੀ) {
                        dest.put(ਕੁੰਜੀ, aliased_ਮੁੱਲ, is_ਜੜ੍ਹ, error_ਕੋਡ);
                    } ਨਹੀਂਤਾਂ {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_ਮੁੱਲ_Type = URES_ਸਾਰਣੀ;
                        CharString ਸਾਰਣੀ_ਰਸਤਾ;
                        ਸਾਰਣੀ_ਰਸਤਾ.append(aliasRB->f_Res_ਰਸਤਾ, error_ਕੋਡ);
                        ਅਚਲ ਅੱਖਰ* ਮਾਪਾ_ਕੁੰਜੀ = ਕੁੰਜੀ; // dest.put() changes the key
                        dest.put(ਮਾਪਾ_ਕੁੰਜੀ, aliased_ਮੁੱਲ, is_ਜੜ੍ਹ, error_ਕੋਡ);
                        UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ* ਇੰਦਰਾਜ = aliasRB->f_ਅੰਕੜੇ;
                        Resource res = aliasRB->fRes;
                        ਜਦੋਂ (aliased_ਮੁੱਲ_Type == URES_ਸਾਰਣੀ && ਇੰਦਰਾਜ->f_ਮਾਪਾ != ਖਾਲੀ_ਸੂਚਕ) {
                            CharString local_ਰਸਤਾ;
                            local_ਰਸਤਾ.copyFrom(ਸਾਰਣੀ_ਰਸਤਾ, error_ਕੋਡ);
                            ਅੱਖਰ* local_ਰਸਤਾ_As_Char_Ptr = local_ਰਸਤਾ.ਅੰਕੜੇ();
                            ਅਚਲ ਅੱਖਰ* ਬੱਚਾ_ਕੁੰਜੀ;
                            ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->f_ਮਾਪਾ;
                            res = ਇੰਦਰਾਜ->f_ਅੰਕੜੇ.ਜੜ੍ਹ_Res;
                            Resource newRes = res_ਲੱਭੋ_Resource(&ਇੰਦਰਾਜ->f_ਅੰਕੜੇ, res, &local_ਰਸਤਾ_As_Char_Ptr, &ਬੱਚਾ_ਕੁੰਜੀ);
                            ਜੇ (newRes != RES_BOGUS) {
                                aliased_ਮੁੱਲ.ਸੈੱਟ_ਅੰਕੜੇ(ਇੰਦਰਾਜ->f_ਅੰਕੜੇ);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_ਮੁੱਲ.ਸੈੱਟ_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_ਮੁੱਲ_Type = aliased_ਮੁੱਲ.getType();
                                ਜੇ (aliased_ਮੁੱਲ_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_ਅੰਕੜੇ_ਮੁੱਲ& rdv2 = static_cast<Resource_ਅੰਕੜੇ_ਮੁੱਲ&>(aliased_ਮੁੱਲ);
                                    aliasRB = get_Alias_ਨਿਸ਼ਾਨਾ_As_Resource_Bundle(rdv2.get_ਅੰਕੜੇ(), rdv2.getResource(), ਖਾਲੀ_ਸੂਚਕ, -1,
                                                                             rdv2.get_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ(), ਖਾਲੀ_ਸੂਚਕ, 0,
                                                                             ਢੇਰ_ਆਰਜ਼ੀ_Bundle.getAlias(), &error_ਕੋਡ);
                                    ਸਾਰਣੀ_ਰਸਤਾ.clear();
                                    ਸਾਰਣੀ_ਰਸਤਾ.append(aliasRB->f_Res_ਰਸਤਾ, error_ਕੋਡ);
                                    ਇੰਦਰਾਜ = aliasRB->f_ਅੰਕੜੇ;
                                    res = aliasRB->fRes;
                                    aliased_ਮੁੱਲ.ਸੈੱਟ_ਅੰਕੜੇ(ਇੰਦਰਾਜ->f_ਅੰਕੜੇ);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_ਮੁੱਲ.ਸੈੱਟ_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_ਮੁੱਲ_Type = aliased_ਮੁੱਲ.getType();
                                }
                                ਜੇ (aliased_ਮੁੱਲ_Type == URES_ਸਾਰਣੀ) {
                                    dest.put(ਮਾਪਾ_ਕੁੰਜੀ, aliased_ਮੁੱਲ, is_ਜੜ੍ਹ, error_ਕੋਡ);
                                } ਨਹੀਂਤਾਂ {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_ਕੋਡ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
                                    ਵਾਪਸ;
                                }
                            }
                        }
                    }
                }
            } ਨਹੀਂਤਾਂ {
                dest.put(ਕੁੰਜੀ, value, is_ਜੜ੍ਹ, error_ਕੋਡ);
            }
            ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI ਖਾਲੀ U_EXPORT2
ures_getAllChildrenWithFallback(ਅਚਲ UResourceBundle *bundle, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                                icu::ResourceSink &sink, UError_ਕੋਡ &error_ਕੋਡ) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_ਚੀਜ਼ਾਂ_With_Fallback(bundle, ਰਸਤਾ, allChildrenSink, error_ਕੋਡ);
}

}  // namespace

// Requires a ResourceDataValue fill-in, so that we need not cast from a ResourceValue.
// Unfortunately, the caller must know which subclass to make and pass in.
// Alternatively, we could make it as polymorphic as in Java by
// returning a ResourceValue pointer (possibly wrapped into a LocalPointer)
// that the caller then owns.
//
// Also requires a UResourceBundle fill-in, so that the value's ResourceTracer
// can point to a non-local bundle.
// Without tracing, the child bundle could be a function-local object.
U_CAPI ਖਾਲੀ U_EXPORT2
ures_get_ਮੁੱਲ_With_Fallback(ਅਚਲ UResourceBundle *bundle, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                          UResourceBundle *ਆਰਜ਼ੀ_Fill_ਵਿੱਚ,
                          Resource_ਅੰਕੜੇ_ਮੁੱਲ &value, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    ਜੇ (ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਅਚਲ UResourceBundle *rb;
    ਜੇ (*ਰਸਤਾ == 0) {
        // empty path
        rb = bundle;
    } ਨਹੀਂਤਾਂ {
        rb = ures_get_By_ਕੁੰਜੀ_With_Fallback(bundle, ਰਸਤਾ, ਆਰਜ਼ੀ_Fill_ਵਿੱਚ, &error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
    }
    value.ਸੈੱਟ_ਅੰਕੜੇ(rb->get_Res_ਅੰਕੜੇ());
    value.ਸੈੱਟ_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ(rb->f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ);
    value.ਸੈੱਟ_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI ਖਾਲੀ U_EXPORT2
ures_get_All_ਚੀਜ਼ਾਂ_With_Fallback(ਅਚਲ UResourceBundle *bundle, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                             icu::ResourceSink &sink, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    ਜੇ (ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਢੇਰ_UResource_Bundle ਢੇਰ_Bundle;
    ਅਚਲ UResourceBundle *rb;
    ਜੇ (*ਰਸਤਾ == 0) {
        // empty path
        rb = bundle;
    } ਨਹੀਂਤਾਂ {
        rb = ures_get_By_ਕੁੰਜੀ_With_Fallback(bundle, ਰਸਤਾ, ਢੇਰ_Bundle.getAlias(), &error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
    }
    // Get all table items with fallback.
    Resource_ਅੰਕੜੇ_ਮੁੱਲ value;
    get_All_ਚੀਜ਼ਾਂ_With_Fallback(rb, value, sink, error_ਕੋਡ);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ਕੁੰਜੀ(ਅਚਲ UResourceBundle* resB,
              ਅਚਲ ਅੱਖਰ* ਵਿੱਚ_ਕੁੰਜੀ,
              UResourceBundle* fill_ਵਿੱਚ U_LIFETIME_BOUND,
              UError_ਕੋਡ* ਸਥਿਤੀ) {
    Resource res = RES_BOGUS;
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਅੰਕੜੇ_ਇੰਦਰਾਜ = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ = ਵਿੱਚ_ਕੁੰਜੀ;

    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ fill_ਵਿੱਚ;
    }
    ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ fill_ਵਿੱਚ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ਜੇ(URES_IS_ਸਾਰਣੀ(type)) {
        int32_t t;
        res = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, &t, &ਕੁੰਜੀ);
        ਜੇ(res == RES_BOGUS) {
            ਕੁੰਜੀ = ਵਿੱਚ_ਕੁੰਜੀ;
            ਜੇ(resB->fHasFallback) {
                ਅੰਕੜੇ_ਇੰਦਰਾਜ = get_Fallback_ਅੰਕੜੇ(resB, &ਕੁੰਜੀ, &res, ਸਥਿਤੀ);
                ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
                    /* check if resB->fResPath gives the right name here */
                    ਵਾਪਸ init_resb_ਨਤੀਜਾ(ਅੰਕੜੇ_ਇੰਦਰਾਜ, res, ਕੁੰਜੀ, -1, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
                } ਨਹੀਂਤਾਂ {
                    *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
                }
            } ਨਹੀਂਤਾਂ {
                *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
            }
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ init_resb_ਨਤੀਜਾ(resB->f_ਅੰਕੜੇ, res, ਕੁੰਜੀ, -1, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
        }
    } 
#ਜੇ 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    ਨਹੀਂਤਾਂ ਜੇ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ਸੱਚ) {
        /* here should go a first attempt to locate the key using index table */
        ਅੰਕੜੇ_ਇੰਦਰਾਜ = get_Fallback_ਅੰਕੜੇ(resB, &ਕੁੰਜੀ, &res, ਸਥਿਤੀ);
        ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
            ਵਾਪਸ init_resb_ਨਤੀਜਾ(ਅੰਕੜੇ_ਇੰਦਰਾਜ, res, ਕੁੰਜੀ, resB, fill_ਵਿੱਚ, ਸਥਿਤੀ);
        } ਨਹੀਂਤਾਂ {
            *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        }
    }
#ਜੇ_ਅੰਤ    
    ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ fill_ਵਿੱਚ;
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2
ures_get_String_By_ਕੁੰਜੀ(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND,
                    ਅਚਲ ਅੱਖਰ* ਵਿੱਚ_ਕੁੰਜੀ,
                    int32_t* len,
                    UError_ਕੋਡ* ਸਥਿਤੀ) {
    Resource res = RES_BOGUS;
    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਅੰਕੜੇ_ਇੰਦਰਾਜ = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ = ਵਿੱਚ_ਕੁੰਜੀ;

    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ਜੇ(URES_IS_ਸਾਰਣੀ(type)) {
        int32_t t=0;

        res = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ(&resB->get_Res_ਅੰਕੜੇ(), resB->fRes, &t, &ਕੁੰਜੀ);

        ਜੇ(res == RES_BOGUS) {
            ਕੁੰਜੀ = ਵਿੱਚ_ਕੁੰਜੀ;
            ਜੇ(resB->fHasFallback) {
                ਅੰਕੜੇ_ਇੰਦਰਾਜ = get_Fallback_ਅੰਕੜੇ(resB, &ਕੁੰਜੀ, &res, ਸਥਿਤੀ);
                ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
                    ਚੋਣ (RES_GET_TYPE(res)) {
                    ਹਾਲਤ URES_STRING:
                    ਹਾਲਤ URES_STRING_V2:
                        ਵਾਪਸ res_getString({resB, ਕੁੰਜੀ}, &ਅੰਕੜੇ_ਇੰਦਰਾਜ->f_ਅੰਕੜੇ, res, len);
                    ਹਾਲਤ URES_ALIAS:
                      {
                        ਅਚਲ char16_t* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
                        UResourceBundle *ਆਰਜ਼ੀ_Res = ures_get_By_ਕੁੰਜੀ(resB, ਵਿੱਚ_ਕੁੰਜੀ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
                        ਨਤੀਜਾ = ures_getString(ਆਰਜ਼ੀ_Res, len, ਸਥਿਤੀ);
                        ures_close(ਆਰਜ਼ੀ_Res);
                        ਵਾਪਸ ਨਤੀਜਾ;
                      }
                    ਮੂਲ:
                        *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
                    }
                } ਨਹੀਂਤਾਂ {
                    *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
                }
            } ਨਹੀਂਤਾਂ {
                *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
            }
        } ਨਹੀਂਤਾਂ {
            ਚੋਣ (RES_GET_TYPE(res)) {
            ਹਾਲਤ URES_STRING:
            ਹਾਲਤ URES_STRING_V2:
                ਵਾਪਸ res_getString({resB, ਕੁੰਜੀ}, &resB->get_Res_ਅੰਕੜੇ(), res, len);
            ਹਾਲਤ URES_ALIAS:
              {
                ਅਚਲ char16_t* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
                UResourceBundle *ਆਰਜ਼ੀ_Res = ures_get_By_ਕੁੰਜੀ(resB, ਵਿੱਚ_ਕੁੰਜੀ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
                ਨਤੀਜਾ = ures_getString(ਆਰਜ਼ੀ_Res, len, ਸਥਿਤੀ);
                ures_close(ਆਰਜ਼ੀ_Res);
                ਵਾਪਸ ਨਤੀਜਾ;
              }
            ਮੂਲ:
                *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#ਜੇ 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    ਨਹੀਂਤਾਂ ਜੇ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ਸੱਚ) {
        /* here should go a first attempt to locate the key using index table */
        ਅੰਕੜੇ_ਇੰਦਰਾਜ = get_Fallback_ਅੰਕੜੇ(resB, &ਕੁੰਜੀ, &res, ਸਥਿਤੀ);
        ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
            // TODO: Tracing
            ਵਾਪਸ res_getString(rd, res, len);
        } ਨਹੀਂਤਾਂ {
            *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        }
    } 
#ਜੇ_ਅੰਤ    
    ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_get_UTF8_String_By_ਕੁੰਜੀ(ਅਚਲ UResourceBundle* resB,
                        ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                        ਅੱਖਰ* dest U_LIFETIME_BOUND,
                        int32_t* p_ਲੰਮਾਈ,
                        UBool forceCopy,
                        UError_ਕੋਡ* ਸਥਿਤੀ) {
    int32_t length16;
    ਅਚਲ char16_t *s16 = ures_get_String_By_ਕੁੰਜੀ(resB, ਕੁੰਜੀ, &length16, ਸਥਿਤੀ);
    ਵਾਪਸ ures_ਮੰਜ਼ਿਲ_UTF8_String(s16, length16, dest, p_ਲੰਮਾਈ, forceCopy, ਸਥਿਤੀ);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_get_Locale_ਅੰਦਰੂਨੀ(ਅਚਲ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (!resourceBundle) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
      ਵਾਪਸ resourceBundle->f_ਅੰਕੜੇ->fName;
    }
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_getLocale(ਅਚਲ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_ਕੋਡ* ਸਥਿਤੀ)
{
  ਵਾਪਸ ures_get_Locale_ਅੰਦਰੂਨੀ(resourceBundle, ਸਥਿਤੀ);
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_getLocaleByType(ਅਚਲ UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_ਅੰਕੜੇ_Locale_Type type,
                     UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (!resourceBundle) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        ਚੋਣ(type) {
        ਹਾਲਤ ULOC_ACTUAL_LOCALE:
            ਵਾਪਸ resourceBundle->f_ਅੰਕੜੇ->fName;
        ਹਾਲਤ ULOC_VALID_LOCALE:
            ਵਾਪਸ resourceBundle->f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ->fName;
        ਹਾਲਤ ULOC_REQUESTED_LOCALE:
        ਮੂਲ:
            *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }
}

U_CFUNC ਅਚਲ ਅੱਖਰ*
ures_getName(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND) {
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  ਵਾਪਸ resB->f_ਅੰਕੜੇ->fName;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ URES_DEBUG
U_CFUNC ਅਚਲ ਅੱਖਰ*
ures_get_ਰਸਤਾ(ਅਚਲ UResourceBundle* resB U_LIFETIME_BOUND) {
  ਜੇ(resB == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  ਵਾਪਸ resB->f_ਅੰਕੜੇ->f_ਰਸਤਾ;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ਅਚਲ ਅੱਖਰ* ਰਸਤਾ,
                  ਅਚਲ ਅੱਖਰ* localeID,
                  UResOpenType openType,
                  UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UResource_ਅੰਕੜੇ_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ;
    ਜੇ(openType != URES_OPEN_ਸਿੱਧਾ) {
        ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_ਅਧਾਰ_Name(localeID, *ਸਥਿਤੀ);
        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
            *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਇੰਦਰਾਜ = ਇੰਦਰਾਜ_Open(ਰਸਤਾ, canonLocaleID.ਅੰਕੜੇ(), openType, ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        ਇੰਦਰਾਜ = ਇੰਦਰਾਜ_Open_ਸਿੱਧਾ(ਰਸਤਾ, localeID, ਸਥਿਤੀ);
    }
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(ਇੰਦਰਾਜ == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UBool is_ਢੇਰ_Object;
    ਜੇ(r == ਖਾਲੀ_ਸੂਚਕ) {
        r = static_cast<UResourceBundle*>(uprv_malloc(ਆਕਾਰ(UResourceBundle)));
        ਜੇ(r == ਖਾਲੀ_ਸੂਚਕ) {
            ਇੰਦਰਾਜ_Close(ਇੰਦਰਾਜ);
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        is_ਢੇਰ_Object = ਝੂਠ;
    } ਨਹੀਂਤਾਂ {  // fill-in
        is_ਢੇਰ_Object = ures_is_ਢੇਰ_Object(r);
        ures_closeBundle(r, ਝੂਠ);
    }
    uprv_memset(r, 0, ਆਕਾਰ(UResourceBundle));
    ures_ਸੈੱਟ_Is_ਢੇਰ_Object(r, is_ਢੇਰ_Object);

    r->f_Valid_Locale_ਅੰਕੜੇ_ਇੰਦਰਾਜ = r->f_ਅੰਕੜੇ = ਇੰਦਰਾਜ;
    r->fHasFallback = openType != URES_OPEN_ਸਿੱਧਾ && !r->get_Res_ਅੰਕੜੇ().noFallback;
    r->f_Is_ਸਿਖਰ_Level = ਸੱਚ;
    r->fRes = r->get_Res_ਅੰਕੜੇ().ਜੜ੍ਹ_Res;
    r->f_ਮਾਪ = res_ਗਿਣਤੀ_Array_ਚੀਜ਼ਾਂ(&r->get_Res_ਅੰਕੜੇ(), r->fRes);
    r->f_ਸੂਚਕਾਂਕ = -1;

    ResourceTracer(r).traceOpen();

    ਵਾਪਸ r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* localeID, UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਵਾਪਸ ures_openWithType(ਖਾਲੀ_ਸੂਚਕ, ਰਸਤਾ, localeID, URES_OPEN_LOCALE_DEFAULT_ਜੜ੍ਹ, ਸਥਿਤੀ);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* localeID, UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਵਾਪਸ ures_openWithType(ਖਾਲੀ_ਸੂਚਕ, ਰਸਤਾ, localeID, URES_OPEN_LOCALE_ਜੜ੍ਹ, ਸਥਿਤੀ);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_ਸਿੱਧਾ(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* localeID, UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਵਾਪਸ ures_openWithType(ਖਾਲੀ_ਸੂਚਕ, ਰਸਤਾ, localeID, URES_OPEN_ਸਿੱਧਾ, ਸਥਿਤੀ);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
ures_open_Fill_ਵਿੱਚ(UResourceBundle *r, ਅਚਲ ਅੱਖਰ* ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ* localeID, UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ(U_SUCCESS(*ਸਥਿਤੀ) && r == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ures_openWithType(r, ਰਸਤਾ, localeID, URES_OPEN_LOCALE_DEFAULT_ਜੜ੍ਹ, ਸਥਿਤੀ);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
ures_open_ਸਿੱਧਾ_Fill_ਵਿੱਚ(UResourceBundle *r, ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* localeID, UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ(U_SUCCESS(*ਸਥਿਤੀ) && r == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ures_openWithType(r, ਰਸਤਾ, localeID, URES_OPEN_ਸਿੱਧਾ, ਸਥਿਤੀ);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_ਗਿਣਤੀ_Array_ਚੀਜ਼ਾਂ(ਅਚਲ UResourceBundle* resourceBundle,
                  ਅਚਲ ਅੱਖਰ* resource_ਕੁੰਜੀ,
                  UError_ਕੋਡ* ਸਥਿਤੀ)
{
    UResourceBundle res_ਅੰਕੜੇ;
    ures_init_ਢੇਰ_Object(&res_ਅੰਕੜੇ);
    ਜੇ (ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(resourceBundle == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ures_get_By_ਕੁੰਜੀ(resourceBundle, resource_ਕੁੰਜੀ, &res_ਅੰਕੜੇ, ਸਥਿਤੀ);
    
    ਜੇ(res_ਅੰਕੜੇ.get_Res_ਅੰਕੜੇ().ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        int32_t ਨਤੀਜਾ = res_ਗਿਣਤੀ_Array_ਚੀਜ਼ਾਂ(&res_ਅੰਕੜੇ.get_Res_ਅੰਕੜੇ(), res_ਅੰਕੜੇ.fRes);
        ures_close(&res_ਅੰਕੜੇ);
        ਵਾਪਸ ਨਤੀਜਾ;
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_ਅੰਕੜੇ);
        ਵਾਪਸ 0;
    }
}

/**
 * Internal function.
 * Return the version number associated with this ResourceBundle as a string.
 *
 * @param resourceBundle The resource bundle for which the version is checked.
 * @return  A version number string as specified in the resource bundle or its parent.
 *          The caller does not own this string.
 * @see ures_getVersion
 * @internal
 */
U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_get_ਸੰਸਕਰਣ_ਸੰਖਿਆ_ਅੰਦਰੂਨੀ(ਅਚਲ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ਜੇ (!resourceBundle) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ(resourceBundle->f_ਸੰਸਕਰਣ == ਖਾਲੀ_ਸੂਚਕ) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ਅਚਲ char16_t* minor_ਸੰਸਕਰਣ = ures_get_String_By_ਕੁੰਜੀ(resourceBundle, k_ਸੰਸਕਰਣ_ਟੈਗ, &minor_len, &ਸਥਿਤੀ);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_ਸੰਸਕਰਣ = (ਅੱਖਰ *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        ਜੇ (((UResourceBundle *)resourceBundle)->f_ਸੰਸਕਰਣ == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
       
        ਜੇ(minor_len > 0) {
            u_UChars_ਮੰਜ਼ਿਲ_Chars(minor_ਸੰਸਕਰਣ, resourceBundle->f_ਸੰਸਕਰਣ , minor_len);
            resourceBundle->f_ਸੰਸਕਰਣ[len] =  '\0';
        }
        ਨਹੀਂਤਾਂ {
            uprv_strcpy(resourceBundle->f_ਸੰਸਕਰਣ, k_Default_Minor_ਸੰਸਕਰਣ);
        }
    }

    ਵਾਪਸ resourceBundle->f_ਸੰਸਕਰਣ;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ures_get_ਸੰਸਕਰਣ_ਸੰਖਿਆ(ਅਚਲ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ਵਾਪਸ ures_get_ਸੰਸਕਰਣ_ਸੰਖਿਆ_ਅੰਦਰੂਨੀ(resourceBundle);
}

U_CAPI ਖਾਲੀ U_EXPORT2 ures_get_ਸੰਸਕਰਣ(ਅਚਲ UResourceBundle* resB, UVersionInfo ਸੰਸਕਰਣ_Info) {
    ਜੇ (!resB) ਵਾਪਸ;

    u_ਸੰਸਕਰਣ_From_String(ਸੰਸਕਰਣ_Info, ures_get_ਸੰਸਕਰਣ_ਸੰਖਿਆ_ਅੰਦਰੂਨੀ(resB));
}

/** Tree support functions *******************************/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕਾਂਕ_LOCALE_NAME "res_index"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕਾਂਕ_ਟੈਗ         "InstalledLocales"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਟੈਗ       "default"

#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

ਸਥਿਰ int32_t U_CALLCONV
ures_loc_ਗਿਣਤੀ_Locales(UEnumeration *en, UError_ਕੋਡ * /*ਸਥਿਤੀ*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    ਵਾਪਸ ures_get_ਮਾਪ(&ctx->installed);
}

U_CDECL_BEGIN


ਸਥਿਰ ਅਚਲ ਅੱਖਰ * U_CALLCONV
ures_loc_ਅਗਲਾ_Locale(UEnumeration* en,
                    int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                    UError_ਕੋਡ* ਸਥਿਤੀ) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ ਅੱਖਰ *ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t len = 0;
    ਜੇ (ures_has_ਅਗਲਾ(res) && (k = ures_get_ਅਗਲਾ_Resource(res, &ctx->curr, ਸਥਿਤੀ)) != ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ = ures_get_ਕੁੰਜੀ(k);
        len = (int32_t)uprv_strlen(ਨਤੀਜਾ);
    }
    ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = len;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_ਕੋਡ* /*ਸਥਿਤੀ*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UEnumeration gLocalesEnum = {
    ਖਾਲੀ_ਸੂਚਕ,
        ਖਾਲੀ_ਸੂਚਕ,
        ures_loc_closeLocales,
        ures_loc_ਗਿਣਤੀ_Locales,
        uenum_unextDefault,
        ures_loc_ਅਗਲਾ_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_ਉਪਲਬਧ_Locales(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ)
{
    UResourceBundle *idx = ਖਾਲੀ_ਸੂਚਕ;
    UEnumeration *en = ਖਾਲੀ_ਸੂਚਕ;
    ULocalesContext *myContext = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(ਆਕਾਰ(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(ਆਕਾਰ(UEnumeration));
    ਜੇ(!en || !myContext) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(en, &gLocalesEnum, ਆਕਾਰ(UEnumeration));

    ures_init_ਢੇਰ_Object(&myContext->installed);
    ures_init_ਢੇਰ_Object(&myContext->curr);
    idx = ures_open_ਸਿੱਧਾ(ਰਸਤਾ, ਸੂਚਕਾਂਕ_LOCALE_NAME, ਸਥਿਤੀ);
    ures_get_By_ਕੁੰਜੀ(idx, ਸੂਚਕਾਂਕ_ਟੈਗ, &myContext->installed, ਸਥਿਤੀ);
    ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            ਰਸਤਾ, ਸੂਚਕਾਂਕ_LOCALE_NAME, ਸੂਚਕਾਂਕ_ਟੈਗ, ures_get_ਕੁੰਜੀ(&myContext->installed));
#ਜੇ_ਅੰਤ
        en->context = myContext;
    } ਨਹੀਂਤਾਂ {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", ਰਸਤਾ, u_errorName(*ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = ਖਾਲੀ_ਸੂਚਕ;
    }
    
    ures_close(idx);
    
    ਵਾਪਸ en;
}

ਸਥਿਰ UBool is_Locale_ਵਿੱਚ_List(UEnumeration *locEnum, ਅਚਲ ਅੱਖਰ *loc_ਮੰਜ਼ਿਲ_ਖੋਜ, UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਅਚਲ ਅੱਖਰ *loc;
    ਜਦੋਂ ((loc = uenum_ਅਗਲਾ(locEnum, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ)) != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (uprv_strcmp(loc, loc_ਮੰਜ਼ਿਲ_ਖੋਜ) == 0) {
            ਵਾਪਸ ਸੱਚ;
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ ਖਾਲੀ get_ਮਾਪਾ_For_Functional_Equivalent(ਅਚਲ ਅੱਖਰ*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      ਮਾਪਾ) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
    ਮਾਪਾ.clear();
    ਜੇ (res != ਖਾਲੀ_ਸੂਚਕ) {
        ures_get_By_ਕੁੰਜੀ(res, "%%Parent", bund1, &sub_ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(sub_ਸਥਿਤੀ)) {
            int32_t length16;
            ਅਚਲ char16_t* s16 = ures_getString(bund1, &length16, &sub_ਸਥਿਤੀ);
            ਮਾਪਾ.appendInvariantChars(s16, length16, sub_ਸਥਿਤੀ);
        }
    }
    
    // If none there, use normal truncation parent
    ਜੇ (U_FAILURE(sub_ਸਥਿਤੀ) || ਮਾਪਾ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        sub_ਸਥਿਤੀ = U_ZERO_ERROR;
        ਮਾਪਾ = ulocimp_get_ਮਾਪਾ(localeID, sub_ਸਥਿਤੀ);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(ਅੱਖਰ *ਨਤੀਜਾ, int32_t ਨਤੀਜਾ_ਸਮਰੱਥਾ,
                             ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *resName, ਅਚਲ ਅੱਖਰ *keyword, ਅਚਲ ਅੱਖਰ *locid,
                             UBool *is_ਉਪਲਬਧ, UBool omitDefault, UError_ਕੋਡ *ਸਥਿਤੀ)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString ਮਿਲਿਆ;
    CharString ਮਾਪਾ;
    CharString ਭਰਿਆ;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
    int32_t length = 0;
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ 0;
    CharString kwVal;
    ਜੇ (keyword != ਖਾਲੀ_ਸੂਚਕ && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_ਮੁੱਲ(locid, keyword, sub_ਸਥਿਤੀ);
        ਜੇ (kwVal == DEFAULT_ਟੈਗ) {
            kwVal.clear();
        }
    }
    ਜੇ (locid == ਖਾਲੀ_ਸੂਚਕ) {
        locid = uloc_getDefault();
    }
    CharString ਅਧਾਰ = ulocimp_get_ਅਧਾਰ_Name(locid, sub_ਸਥਿਤੀ);
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.ਅੰਕੜੇ(), ਅਧਾਰ.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
    ures_init_ਢੇਰ_Object(&bund1);
    ures_init_ਢੇਰ_Object(&bund2);

    ਮਾਪਾ.copyFrom(ਅਧਾਰ, sub_ਸਥਿਤੀ);
    ਮਿਲਿਆ.copyFrom(ਅਧਾਰ, sub_ਸਥਿਤੀ);

    ਜੇ(is_ਉਪਲਬਧ) {
        UEnumeration *locEnum = ures_open_ਉਪਲਬਧ_Locales(ਰਸਤਾ, &sub_ਸਥਿਤੀ);
        *is_ਉਪਲਬਧ = ਸੱਚ;
        ਜੇ (U_SUCCESS(sub_ਸਥਿਤੀ)) {
            *is_ਉਪਲਬਧ = is_Locale_ਵਿੱਚ_List(locEnum, ਮਾਪਾ.ਅੰਕੜੇ(), &sub_ਸਥਿਤੀ);
        }
        uenum_close(locEnum);
    }

    ਜੇ(U_FAILURE(sub_ਸਥਿਤੀ)) {
        *ਸਥਿਤੀ = sub_ਸਥਿਤੀ;
        ਵਾਪਸ 0;
    }
    
    ਕਰੋ {
        sub_ਸਥਿਤੀ = U_ZERO_ERROR;
        res = ures_open(ਰਸਤਾ, ਮਾਪਾ.ਅੰਕੜੇ(), &sub_ਸਥਿਤੀ);
        ਜੇ(((sub_ਸਥਿਤੀ == U_USING_FALLBACK_WARNING) ||
            (sub_ਸਥਿਤੀ == U_USING_DEFAULT_WARNING)) && is_ਉਪਲਬਧ)
        {
            *is_ਉਪਲਬਧ = ਝੂਠ;
        }
        is_ਉਪਲਬਧ = ਖਾਲੀ_ਸੂਚਕ; /* only want to set this the first time around */
        
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਮਾਪਾ.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ), ures_getLocale(res, &sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
        ਜੇ(U_FAILURE(sub_ਸਥਿਤੀ)) {
            *ਸਥਿਤੀ = sub_ਸਥਿਤੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(sub_ਸਥਿਤੀ == U_ZERO_ERROR) {
            ures_get_By_ਕੁੰਜੀ(res,resName,&bund1, &sub_ਸਥਿਤੀ);
            ਜੇ(sub_ਸਥਿਤੀ == U_ZERO_ERROR) {
                ਅਚਲ char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਮਾਪਾ.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
                defUstr = ures_get_String_By_ਕੁੰਜੀ(&bund1, DEFAULT_ਟੈਗ, &defLen, &sub_ਸਥਿਤੀ);
                ਜੇ(U_SUCCESS(sub_ਸਥਿਤੀ) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_ਸਥਿਤੀ);
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਮਾਪਾ.ਅੰਕੜੇ(), keyword, defVal.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
                    defLoc.copyFrom(ਮਾਪਾ, sub_ਸਥਿਤੀ);
                    ਜੇ(kwVal.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                        kwVal.append(defVal, sub_ਸਥਿਤੀ);
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਮਾਪਾ.ਅੰਕੜੇ(), keyword, kwVal.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
                    }
                }
            }
        }
        
        sub_ਸਥਿਤੀ = U_ZERO_ERROR;

        ਜੇ (res != ਖਾਲੀ_ਸੂਚਕ) {
            ਮਿਲਿਆ.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_ਸਥਿਤੀ), sub_ਸਥਿਤੀ);
        }

        ਜੇ (ਮਿਲਿਆ != ਮਾਪਾ) {
            ਮਾਪਾ.copyFrom(ਮਿਲਿਆ, sub_ਸਥਿਤੀ);
        } ਨਹੀਂਤਾਂ {
            get_ਮਾਪਾ_For_Functional_Equivalent(ਮਿਲਿਆ.ਅੰਕੜੇ(),res,&bund1,ਮਾਪਾ);
        }
        ures_close(res);
    } ਜਦੋਂ(defVal.is_ਖਾਲੀ_ਥਾਵਾਂ() && !ਮਿਲਿਆ.is_ਖਾਲੀ_ਥਾਵਾਂ() && ਮਿਲਿਆ != "root" && U_SUCCESS(*ਸਥਿਤੀ));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    ਮਾਪਾ.copyFrom(ਅਧਾਰ, sub_ਸਥਿਤੀ);
    ਮਿਲਿਆ.copyFrom(ਅਧਾਰ, sub_ਸਥਿਤੀ);

    ਕਰੋ {
        res = ures_open(ਰਸਤਾ, ਮਾਪਾ.ਅੰਕੜੇ(), &sub_ਸਥਿਤੀ);
        ਜੇ((sub_ਸਥਿਤੀ == U_USING_FALLBACK_WARNING) && is_ਉਪਲਬਧ) {
            *is_ਉਪਲਬਧ = ਝੂਠ;
        }
        is_ਉਪਲਬਧ = ਖਾਲੀ_ਸੂਚਕ; /* only want to set this the first time around */
        
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਮਾਪਾ.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ), kwVal.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
        ਜੇ(U_FAILURE(sub_ਸਥਿਤੀ)) {
            *ਸਥਿਤੀ = sub_ਸਥਿਤੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(sub_ਸਥਿਤੀ == U_ZERO_ERROR) {
            ures_get_By_ਕੁੰਜੀ(res,resName,&bund1, &sub_ਸਥਿਤੀ);
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
            ਜੇ(sub_ਸਥਿਤੀ == U_ZERO_ERROR) {
                ures_get_By_ਕੁੰਜੀ(&bund1, kwVal.ਅੰਕੜੇ(), &bund2, &sub_ਸਥਿਤੀ);
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
                ਜੇ(sub_ਸਥਿਤੀ == U_ZERO_ERROR) {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਮਾਪਾ.ਅੰਕੜੇ(), keyword, kwVal.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
                    ਜੇ (ਮਾਪਾ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                        ਭਰਿਆ.clear().append("root", sub_ਸਥਿਤੀ);
                    } ਨਹੀਂਤਾਂ {
                        ਭਰਿਆ.copyFrom(ਮਾਪਾ, sub_ਸਥਿਤੀ);
                    }
                        /* now, recalculate default kw if need be */
                        ਜੇ(defLoc.length() > ਭਰਿਆ.length()) {
                          ਅਚਲ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਭਰਿਆ.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
                          defUstr = ures_get_String_By_ਕੁੰਜੀ(&bund1, DEFAULT_ਟੈਗ, &defLen, &sub_ਸਥਿਤੀ);
                          ਜੇ(U_SUCCESS(sub_ਸਥਿਤੀ) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_ਸਥਿਤੀ);
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਭਰਿਆ.ਅੰਕੜੇ(), keyword, defVal.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
                            defLoc.copyFrom(ਭਰਿਆ, sub_ਸਥਿਤੀ);
                          }
                        } /* end of recalculate default KW */
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                        ਨਹੀਂਤਾਂ {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.ਅੰਕੜੇ(), ਭਰਿਆ.ਅੰਕੜੇ());
                        }
#ਜੇ_ਅੰਤ
                } ਨਹੀਂਤਾਂ {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_ਸਥਿਤੀ), ਮਾਪਾ.ਅੰਕੜੇ(), kwVal.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
                }
            }
        }
        
        sub_ਸਥਿਤੀ = U_ZERO_ERROR;
        UBool have_ਮਿਲਿਆ = ਝੂਠ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        ਜੇ (res != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(resName, "collations") == 0) {
            ਅਚਲ ਅੱਖਰ *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(sub_ਸਥਿਤੀ) && validLoc != ਖਾਲੀ_ਸੂਚਕ && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_ਸਥਿਤੀ);
                CharString ਮਾਪਾ_Lang = ulocimp_getLanguage(ਮਾਪਾ.ਮੰਜ਼ਿਲ_String_Piece(), sub_ਸਥਿਤੀ);
                ਜੇ (U_SUCCESS(sub_ਸਥਿਤੀ) && validLang != ਮਾਪਾ_Lang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    ਮਿਲਿਆ.clear().append(validLoc, sub_ਸਥਿਤੀ);
                    have_ਮਿਲਿਆ = ਸੱਚ;
                }
            }
            sub_ਸਥਿਤੀ = U_ZERO_ERROR;
        }
        ਜੇ (!have_ਮਿਲਿਆ) {
            ਮਿਲਿਆ.copyFrom(ਮਾਪਾ, sub_ਸਥਿਤੀ);
        }

        get_ਮਾਪਾ_For_Functional_Equivalent(ਮਿਲਿਆ.ਅੰਕੜੇ(),res,&bund1,ਮਾਪਾ);
        ures_close(res);
        sub_ਸਥਿਤੀ = U_ZERO_ERROR;
    } ਜਦੋਂ(ਭਰਿਆ.is_ਖਾਲੀ_ਥਾਵਾਂ() && !ਮਿਲਿਆ.is_ਖਾਲੀ_ਥਾਵਾਂ() && U_SUCCESS(*ਸਥਿਤੀ));

    ਜੇ(ਭਰਿਆ.is_ਖਾਲੀ_ਥਾਵਾਂ() && kwVal != defVal) {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.ਅੰਕੜੇ(), defVal.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
        kwVal.clear().append(defVal, sub_ਸਥਿਤੀ);
        ਮਾਪਾ.copyFrom(ਅਧਾਰ, sub_ਸਥਿਤੀ);
        ਮਿਲਿਆ.copyFrom(ਅਧਾਰ, sub_ਸਥਿਤੀ);

        ਕਰੋ { /* search for 'default' named item */
            res = ures_open(ਰਸਤਾ, ਮਾਪਾ.ਅੰਕੜੇ(), &sub_ਸਥਿਤੀ);
            ਜੇ((sub_ਸਥਿਤੀ == U_USING_FALLBACK_WARNING) && is_ਉਪਲਬਧ) {
                *is_ਉਪਲਬਧ = ਝੂਠ;
            }
            is_ਉਪਲਬਧ = ਖਾਲੀ_ਸੂਚਕ; /* only want to set this the first time around */
            
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਮਾਪਾ.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ), kwVal.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
            ਜੇ(U_FAILURE(sub_ਸਥਿਤੀ)) {
                *ਸਥਿਤੀ = sub_ਸਥਿਤੀ;
            } ਨਹੀਂਤਾਂ ਜੇ(sub_ਸਥਿਤੀ == U_ZERO_ERROR) {
                ures_get_By_ਕੁੰਜੀ(res,resName,&bund1, &sub_ਸਥਿਤੀ);
                ਜੇ(sub_ਸਥਿਤੀ == U_ZERO_ERROR) {
                    ures_get_By_ਕੁੰਜੀ(&bund1, kwVal.ਅੰਕੜੇ(), &bund2, &sub_ਸਥਿਤੀ);
                    ਜੇ(sub_ਸਥਿਤੀ == U_ZERO_ERROR) {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", ਰਸਤਾ?ਰਸਤਾ:"ICUDATA",
                            ਮਾਪਾ.ਅੰਕੜੇ(), keyword, kwVal.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
                        ਜੇ (ਮਾਪਾ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                            ਭਰਿਆ.clear().append("root", sub_ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ {
                            ਭਰਿਆ.copyFrom(ਮਾਪਾ, sub_ਸਥਿਤੀ);
                        }
                        
                        /* now, recalculate default kw if need be */
                        ਜੇ(defLoc.length() > ਭਰਿਆ.length()) {
                          ਅਚਲ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਭਰਿਆ.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
                          defUstr = ures_get_String_By_ਕੁੰਜੀ(&bund1, DEFAULT_ਟੈਗ, &defLen, &sub_ਸਥਿਤੀ);
                          ਜੇ(U_SUCCESS(sub_ਸਥਿਤੀ) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_ਸਥਿਤੀ);
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    ਰਸਤਾ?ਰਸਤਾ:"ICUDATA", ਭਰਿਆ.ਅੰਕੜੇ(), keyword, defVal.ਅੰਕੜੇ(), u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
                            defLoc.copyFrom(ਭਰਿਆ, sub_ਸਥਿਤੀ);
                          }
                        } /* end of recalculate default KW */
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                        ਨਹੀਂਤਾਂ {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.ਅੰਕੜੇ(), ਭਰਿਆ.ਅੰਕੜੇ());
                        }
#ਜੇ_ਅੰਤ
                    }
                }
            }
            
            sub_ਸਥਿਤੀ = U_ZERO_ERROR;
            ਮਿਲਿਆ.copyFrom(ਮਾਪਾ, sub_ਸਥਿਤੀ);
            get_ਮਾਪਾ_For_Functional_Equivalent(ਮਿਲਿਆ.ਅੰਕੜੇ(),res,&bund1,ਮਾਪਾ);
            ures_close(res);
            sub_ਸਥਿਤੀ = U_ZERO_ERROR;
        } ਜਦੋਂ(ਭਰਿਆ.is_ਖਾਲੀ_ਥਾਵਾਂ() && !ਮਿਲਿਆ.is_ਖਾਲੀ_ਥਾਵਾਂ() && U_SUCCESS(*ਸਥਿਤੀ));
    }
    
    ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
        ਜੇ(ਭਰਿਆ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
          *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
        } ਨਹੀਂਤਾਂ ਜੇ(omitDefault) {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", ਭਰਿਆ.ਅੰਕੜੇ(), defLoc.ਅੰਕੜੇ(), ਮਿਲਿਆ.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ        
          ਜੇ(defLoc.length() <= ਭਰਿਆ.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            ਜੇ(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
              kwVal.clear();
            }
          }
        }
        ਮਿਲਿਆ.copyFrom(ਭਰਿਆ, sub_ਸਥਿਤੀ);
        ਜੇ(!kwVal.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ਮਿਲਿਆ
                .append("@", sub_ਸਥਿਤੀ)
                .append(keyword, sub_ਸਥਿਤੀ)
                .append("=", sub_ਸਥਿਤੀ)
                .append(kwVal, sub_ਸਥਿਤੀ);
        } ਨਹੀਂਤਾਂ ਜੇ(!omitDefault) {
            ਮਿਲਿਆ
                .append("@", sub_ਸਥਿਤੀ)
                .append(keyword, sub_ਸਥਿਤੀ)
                .append("=", sub_ਸਥਿਤੀ)
                .append(defVal, sub_ਸਥਿਤੀ);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = ਮਿਲਿਆ.length();

    ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
        int32_t copy_ਲੰਮਾਈ = uprv_min(length, ਨਤੀਜਾ_ਸਮਰੱਥਾ);
        ਜੇ(copy_ਲੰਮਾਈ>0) {
            ਮਿਲਿਆ.extract(ਨਤੀਜਾ, copy_ਲੰਮਾਈ, sub_ਸਥਿਤੀ);
        }
        ਜੇ(length == 0) {
          *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR; 
        }
    } ਨਹੀਂਤਾਂ {
        length = 0;
        ਨਤੀਜਾ[0]=0;
    }
    ਵਾਪਸ u_terminateChars(ਨਤੀਜਾ, ਨਤੀਜਾ_ਸਮਰੱਥਾ, length, ਸਥਿਤੀ);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_ਮੁੱਲਾਂ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *keyword, UError_ਕੋਡ *ਸਥਿਤੀ)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੁੱਲਾਂ_BUF_ਮਾਪ 2048
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੁੱਲਾਂ_LIST_ਮਾਪ 512
    
    ਅੱਖਰ       ਮੁੱਲਾਂ_Buf[ਮੁੱਲਾਂ_BUF_ਮਾਪ];
    int32_t    ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ = 0;
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲਾਂ_List[ਮੁੱਲਾਂ_LIST_ਮਾਪ];
    int32_t    ਮੁੱਲਾਂ_ਗਿਣਤੀ = 0;
    
    ਅਚਲ ਅੱਖਰ *locale;
    int32_t     locLen;
    
    UEnumeration *locs = ਖਾਲੀ_ਸੂਚਕ;
    
    UResourceBundle    ਵਸਤੂ;
    UResourceBundle    sub_ਵਸਤੂ;
    
    ures_init_ਢੇਰ_Object(&ਵਸਤੂ);
    ures_init_ਢੇਰ_Object(&sub_ਵਸਤੂ);
    locs = ures_open_ਉਪਲਬਧ_Locales(ਰਸਤਾ, ਸਥਿਤੀ);
    
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ures_close(&ਵਸਤੂ);
        ures_close(&sub_ਵਸਤੂ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    
    ਮੁੱਲਾਂ_Buf[0]=0;
    ਮੁੱਲਾਂ_Buf[1]=0;

    ਜਦੋਂ ((locale = uenum_ਅਗਲਾ(locs, &locLen, ਸਥਿਤੀ)) != ਖਾਲੀ_ਸੂਚਕ) {
        UResourceBundle   *bund = ਖਾਲੀ_ਸੂਚਕ;
        UResourceBundle   *subPtr = ਖਾਲੀ_ਸੂਚਕ;
        UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(ਰਸਤਾ, locale, &sub_ਸਥਿਤੀ);
        
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
        ਜੇ(!bund || U_FAILURE(sub_ਸਥਿਤੀ)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                ਰਸਤਾ?ਰਸਤਾ:"<ICUDATA>", keyword, locale, u_errorName(sub_ਸਥਿਤੀ));
        }
#ਜੇ_ਅੰਤ
        
        ures_get_By_ਕੁੰਜੀ(bund, keyword, &ਵਸਤੂ, &sub_ਸਥਿਤੀ);
        
        ਜੇ(!bund || U_FAILURE(sub_ਸਥਿਤੀ)) {
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                ਰਸਤਾ?ਰਸਤਾ:"<ICUDATA>", keyword, locale, u_errorName(sub_ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
            ures_close(bund);
            bund = ਖਾਲੀ_ਸੂਚਕ;
            ਜਾਰੀ;
        }

        ਜਦੋਂ ((subPtr = ures_get_ਅਗਲਾ_Resource(&ਵਸਤੂ, &sub_ਵਸਤੂ, &sub_ਸਥਿਤੀ)) != ਖਾਲੀ_ਸੂਚਕ
            && U_SUCCESS(sub_ਸਥਿਤੀ)) {
            ਅਚਲ ਅੱਖਰ *k;
            int32_t i;
            k = ures_get_ਕੁੰਜੀ(subPtr);

#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#ਜੇ_ਅੰਤ
            ਜੇ(k == ਖਾਲੀ_ਸੂਚਕ || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_ਟੈਗ) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                ਜਾਰੀ;
            }
            ਲਈ(i=0; i<ਮੁੱਲਾਂ_ਗਿਣਤੀ; i++) {
                ਜੇ(!uprv_strcmp(ਮੁੱਲਾਂ_List[i],k)) {
                    k = ਖਾਲੀ_ਸੂਚਕ; /* found duplicate */
                    ਰੋਕੋ;
                }
            }
            ਜੇ(k != ਖਾਲੀ_ਸੂਚਕ) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                ਜੇ((ਮੁੱਲਾਂ_ਗਿਣਤੀ >= (ਮੁੱਲਾਂ_LIST_ਮਾਪ-1)) ||       /* no more space in list .. */
                    ((ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ+kLen+1+1) >= ਮੁੱਲਾਂ_BUF_ਮਾਪ)) { /* no more space in buffer (string + 2 nulls) */
                    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* out of space.. */
                } ਨਹੀਂਤਾਂ {
                    uprv_strcpy(ਮੁੱਲਾਂ_Buf+ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ, k);
                    ਮੁੱਲਾਂ_List[ਮੁੱਲਾਂ_ਗਿਣਤੀ++] = ਮੁੱਲਾਂ_Buf+ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ;
                    ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ += kLen;
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        ਰਸਤਾ?ਰਸਤਾ:"<ICUDATA>", keyword, locale, k);
#ਜੇ_ਅੰਤ
                    ਮੁੱਲਾਂ_Buf[ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    ਮੁੱਲਾਂ_Buf[ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ++] = 0; /* terminate */
    
    ures_close(&ਵਸਤੂ);
    ures_close(&sub_ਵਸਤੂ);
    uenum_close(locs);
#ਜੇ defined(URES_ਰੁੱਖ_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*ਸਥਿਤੀ), 
        ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ, ਮੁੱਲਾਂ_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ uloc_openKeywordList(ਮੁੱਲਾਂ_Buf, ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ, ਸਥਿਤੀ);
}
#ਜੇ 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ਅਚਲ UResourceBundle* res1, ਅਚਲ UResourceBundle* res2){
    ਜੇ(res1==ਖਾਲੀ_ਸੂਚਕ || res2==ਖਾਲੀ_ਸੂਚਕ){
        ਵਾਪਸ res1==res2; /* pointer comparison */
    }
    ਜੇ(res1->f_ਕੁੰਜੀ==ਖਾਲੀ_ਸੂਚਕ||  res2->f_ਕੁੰਜੀ==ਖਾਲੀ_ਸੂਚਕ){
        ਵਾਪਸ (res1->f_ਕੁੰਜੀ==res2->f_ਕੁੰਜੀ);
    }ਨਹੀਂਤਾਂ{
        ਜੇ(uprv_strcmp(res1->f_ਕੁੰਜੀ, res2->f_ਕੁੰਜੀ)!=0){
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਜੇ(uprv_strcmp(res1->f_ਅੰਕੜੇ->fName, res2->f_ਅੰਕੜੇ->fName)!=0){
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(res1->f_ਅੰਕੜੇ->f_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ||  res2->f_ਅੰਕੜੇ->f_ਰਸਤਾ==ਖਾਲੀ_ਸੂਚਕ){
        ਵਾਪਸ (res1->f_ਅੰਕੜੇ->f_ਰਸਤਾ == res2->f_ਅੰਕੜੇ->f_ਰਸਤਾ);
    }ਨਹੀਂਤਾਂ{
        ਜੇ(uprv_strcmp(res1->f_ਅੰਕੜੇ->f_ਰਸਤਾ, res2->f_ਅੰਕੜੇ->f_ਰਸਤਾ)!=0){
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਜੇ(uprv_strcmp(res1->f_ਅੰਕੜੇ->f_ਮਾਪਾ->fName, res2->f_ਅੰਕੜੇ->f_ਮਾਪਾ->fName)!=0){
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(uprv_strcmp(res1->f_ਅੰਕੜੇ->f_ਮਾਪਾ->f_ਰਸਤਾ, res2->f_ਅੰਕੜੇ->f_ਮਾਪਾ->f_ਰਸਤਾ)!=0){
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(uprv_strncmp(res1->f_Res_ਰਸਤਾ, res2->f_Res_ਰਸਤਾ, res1->f_Res_ਰਸਤਾ_Len)!=0){
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(res1->fRes != res2->fRes){
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ ਸੱਚ;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ਅਚਲ UResourceBundle* res, UError_ਕੋਡ* ਸਥਿਤੀ){
    UResourceBundle* bundle = ਖਾਲੀ_ਸੂਚਕ;
    UResourceBundle* ret = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ) || res == ਖਾਲੀ_ਸੂਚਕ){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    bundle = ures_open(res->f_ਅੰਕੜੇ->f_ਰਸਤਾ, res->f_ਅੰਕੜੇ->fName, ਸਥਿਤੀ);
    ਜੇ(res->f_Res_ਰਸਤਾ!=ਖਾਲੀ_ਸੂਚਕ){
        ret = ures_ਲੱਭੋ_Sub_Resource(bundle, res->f_Res_ਰਸਤਾ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
        ures_close(bundle);
    }ਨਹੀਂਤਾਂ{
        ret = bundle;
    }
    ਵਾਪਸ ret;
}
U_CAPI ਅਚਲ UResourceBundle* U_EXPORT2
ures_get_ਮਾਪਾ_Bundle(ਅਚਲ UResourceBundle* res){
    ਜੇ(res==ਖਾਲੀ_ਸੂਚਕ){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ res->f_ਮਾਪਾ_Res;
}
#ਜੇ_ਅੰਤ

U_CAPI ਖਾਲੀ U_EXPORT2
ures_get_ਸੰਸਕਰਣ_By_ਕੁੰਜੀ(ਅਚਲ UResourceBundle* res, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, UVersionInfo ver, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਅਚਲ char16_t *str;
  int32_t len;
  str = ures_get_String_By_ਕੁੰਜੀ(res, ਕੁੰਜੀ, &len, ਸਥਿਤੀ);
  ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
    u_ਸੰਸਕਰਣ_From_UString(ver, str);
  } 
}

/* eof */
