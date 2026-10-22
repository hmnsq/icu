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

#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "cwchar.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "unicode/uenum.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uresdata.h"

ಬಳಸಿ ನಾಮಾಕಾಶ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ಸ್ಥಿರ UHashtable *cache = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UInitOnce gCacheInitOnce {};

ಸ್ಥಿರ UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ಸ್ಥಿರ int32_t U_CALLCONV hash_ನಮೂದು(ಸ್ಥಿರಾಂಕ UHashTok parm) {
    UResource_ದತ್ತಾಂಶ_ನಮೂದು* b = static_cast<UResource_ದತ್ತಾಂಶ_ನಮೂದು*>(parm.ಸೂಚಕ);
    UHashTok namekey, pathkey;
    namekey.ಸೂಚಕ = b->fName;
    pathkey.ಸೂಚಕ = b->f_ಮಾರ್ಗ;
    ಹಿಂತಿರುಗಿ uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ಸ್ಥಿರ UBool U_CALLCONV compareEntries(ಸ್ಥಿರಾಂಕ UHashTok p1, ಸ್ಥಿರಾಂಕ UHashTok p2) {
    UResource_ದತ್ತಾಂಶ_ನಮೂದು* b1 = static_cast<UResource_ದತ್ತಾಂಶ_ನಮೂದು*>(p1.ಸೂಚಕ);
    UResource_ದತ್ತಾಂಶ_ನಮೂದು* b2 = static_cast<UResource_ದತ್ತಾಂಶ_ನಮೂದು*>(p2.ಸೂಚಕ);
    UHashTok name1, name2, path1, path2;
    name1.ಸೂಚಕ = b1->fName;
    name2.ಸೂಚಕ = b2->fName;
    path1.ಸೂಚಕ = b1->f_ಮಾರ್ಗ;
    path2.ಸೂಚಕ = b2->f_ಮಾರ್ಗ;
    ಹಿಂತಿರುಗಿ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ಸ್ಥಿರ UBool chopLocale(ಅಕ್ಷರ *name) {
    ಅಕ್ಷರ *i = uprv_strrchr(name, '_');

    ಆದರೆ(i != ಶೂನ್ಯಸೂಚಕ) {
        *i = '\0';
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸ್ಥಿರ UBool hasVariant(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID) {
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(ಶೂನ್ಯಸೂಚಕ, 0);
    ulocimp_getSubtags(
            localeID,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            &sink,
            ಶೂನ್ಯಸೂಚಕ,
            err);
    ಹಿಂತಿರುಗಿ sink.ಸಂಖ್ಯೆ_ದ_ಬೈಟುಗಳು_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#ವ್ಯಾಖ್ಯಾನಿಸು INCLUDED_FROM_URESBUND_CPP
#ಅನ್ತರ್ಭಾವ "localefallback_data.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* performFallbackLookup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ_Strs,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೌಲ್ಯ_Strs U_LIFETIME_BOUND,
                                         ಸ್ಥಿರಾಂಕ int32_t* lookup_ಕೋಷ್ಟಕ,
                                         int32_t lookup_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ int32_t* bottom = lookup_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ int32_t* ತುದಿ = lookup_ಕೋಷ್ಟಕ + lookup_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ;

    ಆಗ (bottom < ತುದಿ) {
        // Effectively, divide by 2 and round down to an even index
        ಸ್ಥಿರಾಂಕ int32_t* ಮಧ್ಯ = bottom + (((ತುದಿ - bottom) / 4) * 2);
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ನಮೂದು_ಕೀಲಿ = &(ಕೀಲಿ_Strs[*ಮಧ್ಯ]);
        int32_t strcmp_ಫಲಿತಾಂಶ = uprv_strcmp(ಕೀಲಿ, ನಮೂದು_ಕೀಲಿ);
        ಆದರೆ (strcmp_ಫಲಿತಾಂಶ == 0) {
            ಹಿಂತಿರುಗಿ &(ಮೌಲ್ಯ_Strs[ಮಧ್ಯ[1]]);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp_ಫಲಿತಾಂಶ < 0) {
            ತುದಿ = ಮಧ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            bottom = ಮಧ್ಯ + 2;
        }
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರ CharString getDefaultScript(ಸ್ಥಿರಾಂಕ CharString& language, ಸ್ಥಿರಾಂಕ CharString& region) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* defaultScript = ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString ಫಲಿತಾಂಶ("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    ಆದರೆ (!region.is_ಖಾಲಿಗಳು()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        ಆದರೆ (U_FAILURE(err)) {
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        defaultScript = performFallbackLookup(localeID.ದತ್ತಾಂಶ(), dsLocaleIDChars, script_ಸಂಕೇತ_Chars, default_Script_ಕೋಷ್ಟಕ, UPRV_LENGTHOF(default_Script_ಕೋಷ್ಟಕ));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    ಆದರೆ (defaultScript == ಶೂನ್ಯಸೂಚಕ) {
        defaultScript = performFallbackLookup(language.ದತ್ತಾಂಶ(), dsLocaleIDChars, script_ಸಂಕೇತ_Chars, default_Script_ಕೋಷ್ಟಕ, UPRV_LENGTHOF(default_Script_ಕೋಷ್ಟಕ));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    ಆದರೆ (defaultScript != ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ.clear();
        ಫಲಿತಾಂಶ.append(defaultScript, err);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಎಣಿಕೆ UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_ಬೇರು,
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
    URES_OPEN_LOCALE_ಬೇರು,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_ನೇರ
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UResOpenType UResOpenType;

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
ಸ್ಥಿರ ತಾರ್ಕಿಕ get_ಪೋಷಕ_Locale_ID(ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    ಆದರೆ (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        ಹಿಂತಿರುಗಿ chopLocale(name);
    }
    
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, err);

    ಆದರೆ (U_FAILURE(err)) {
        // hopefully this never happens...
        ಹಿಂತಿರುಗಿ chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    ಆದರೆ (openType == URES_OPEN_LOCALE_DEFAULT_ಬೇರು) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಪೋಷಕ_ID = performFallbackLookup(name, ಪೋಷಕ_Locale_Chars, ಪೋಷಕ_Locale_Chars, ಪೋಷಕ_Locale_ಕೋಷ್ಟಕ, UPRV_LENGTHOF(ಪೋಷಕ_Locale_ಕೋಷ್ಟಕ));
        ಆದರೆ (ಪೋಷಕ_ID != ಶೂನ್ಯಸೂಚಕ) {
            uprv_strcpy(name, ಪೋಷಕ_ID);
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    ಆದರೆ (!script.is_ಖಾಲಿಗಳು() && !region.is_ಖಾಲಿಗಳು()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        ಆದರೆ (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } ಇಲ್ಲದಿದ್ದರೆ {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!region.is_ಖಾಲಿಗಳು()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_ಸಂಕೇತ err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, err);
        ಆದರೆ (!origNameScript.is_ಖಾಲಿಗಳು()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } ಇಲ್ಲದಿದ್ದರೆ {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!script.is_ಖಾಲಿಗಳು()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        ಆದರೆ (openType != URES_OPEN_LOCALE_DEFAULT_ಬೇರು || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (U_SUCCESS(err) && !workingLocale.is_ಖಾಲಿಗಳು()) {
        uprv_strcpy(name, workingLocale.ದತ್ತಾಂಶ());
        ಹಿಂತಿರುಗಿ ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ಸ್ಥಿರ UBool may_Have_ಪೋಷಕ(ಅಕ್ಷರ *name) {
    ಹಿಂತಿರುಗಿ (name[0] != 0 && uprv_strstr("nb nn",name) != ಶೂನ್ಯಸೂಚಕ);
}

/**
 *  Internal function
 */
ಸ್ಥಿರ ಶೂನ್ಯ ನಮೂದು_Increase(UResource_ದತ್ತಾಂಶ_ನಮೂದು *ನಮೂದು) {
    Mutex ಬೀಗ(&resbMutex);
    ನಮೂದು->f_ಎಣಿಕೆ_Existing++;
    ಆಗ(ನಮೂದು->f_ಪೋಷಕ != ಶೂನ್ಯಸೂಚಕ) {
      ನಮೂದು = ನಮೂದು->f_ಪೋಷಕ;
      ನಮೂದು->f_ಎಣಿಕೆ_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ಸ್ಥಿರ UResource_ದತ್ತಾಂಶ_ನಮೂದು *get_Fallback_ದತ್ತಾಂಶ(
        ಸ್ಥಿರಾಂಕ UResourceBundle *resBundle,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **res_ಟ್ಯಾಗ್, Resource *res, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *ದತ್ತಾಂಶ_ನಮೂದು = resBundle->f_ದತ್ತಾಂಶ;
    int32_t ಸೂಚ್ಯಂಕ_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    ಆದರೆ(ದತ್ತಾಂಶ_ನಮೂದು == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(ದತ್ತಾಂಶ_ನಮೂದು->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(&(ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ), ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ.ಬೇರು_Res, &ಸೂಚ್ಯಂಕ_R, res_ಟ್ಯಾಗ್); /* try to get data from there */
        i++;
    }
    ಆದರೆ(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        ಆಗ(*res == RES_BOGUS && ದತ್ತಾಂಶ_ನಮೂದು->f_ಪೋಷಕ != ಶೂನ್ಯಸೂಚಕ) {
            ದತ್ತಾಂಶ_ನಮೂದು = ದತ್ತಾಂಶ_ನಮೂದು->f_ಪೋಷಕ;
            ಆದರೆ(ದತ್ತಾಂಶ_ನಮೂದು->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(&(ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ), ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ.ಬೇರು_Res, &ಸೂಚ್ಯಂಕ_R, res_ಟ್ಯಾಗ್);
            }
        }
    }

    ಆದರೆ(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    // If the resource is found in parents, we need to adjust the error.
    ಆದರೆ(i>1) {
        ಆದರೆ(uprv_strcmp(ದತ್ತಾಂಶ_ನಮೂದು->fName, uloc_getDefault())==0 || uprv_strcmp(ದತ್ತಾಂಶ_ನಮೂದು->fName, k_ಬೇರು_Locale_Name)==0) {
            *ಸ್ಥಿತಿ_2 = U_USING_DEFAULT_WARNING;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *ಸ್ಥಿತಿ_2 = U_USING_FALLBACK_WARNING;
        }
    }
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_ನಮೂದು;
}

ಸ್ಥಿರ ಶೂನ್ಯ
free_ನಮೂದು(UResource_ದತ್ತಾಂಶ_ನಮೂದು *ನಮೂದು) {
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *alias;
    res_unload(&(ನಮೂದು->f_ದತ್ತಾಂಶ));
    ಆದರೆ(ನಮೂದು->fName != ಶೂನ್ಯಸೂಚಕ && ನಮೂದು->fName != ನಮೂದು->f_Name_ಬಫರ್) {
        uprv_free(ನಮೂದು->fName);
    }
    ಆದರೆ(ನಮೂದು->f_ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(ನಮೂದು->f_ಮಾರ್ಗ);
    }
    ಆದರೆ(ನಮೂದು->fPool != ಶೂನ್ಯಸೂಚಕ) {
        --ನಮೂದು->fPool->f_ಎಣಿಕೆ_Existing;
    }
    alias = ನಮೂದು->fAlias;
    ಆದರೆ(alias != ಶೂನ್ಯಸೂಚಕ) {
        ಆಗ(alias->fAlias != ಶೂನ್ಯಸೂಚಕ) {
            alias = alias->fAlias;
        }
        --alias->f_ಎಣಿಕೆ_Existing;
    }
    uprv_free(ನಮೂದು);
}

/* Works just like ucnv_flushCache() */
ಸ್ಥಿರ int32_t ures_flushCache()
{
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *resB;
    int32_t pos;
    int32_t rb_Deleted_ಅಂಕೆ = 0;
    ಸ್ಥಿರಾಂಕ UHashElement *e;
    UBool deleted_ಇನ್ನಷ್ಟು;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex ಬೀಗ(&resbMutex);
    ಆದರೆ (cache == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಮಾಡು {
        deleted_ಇನ್ನಷ್ಟು = ಸುಳ್ಳು;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_ಮೊದಲ;
        ಆಗ ((e = uhash_ಮುಂದಿನ_Element(cache, &pos)) != ಶೂನ್ಯಸೂಚಕ)
        {
            resB = static_cast<UResource_ದತ್ತಾಂಶ_ನಮೂದು*>(e->value.ಸೂಚಕ);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            ಆದರೆ (resB->f_ಎಣಿಕೆ_Existing == 0) {
                rb_Deleted_ಅಂಕೆ++;
                deleted_ಇನ್ನಷ್ಟು = ನಿಜ;
                uhash_removeElement(cache, e);
                free_ನಮೂದು(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } ಆಗ(deleted_ಇನ್ನಷ್ಟು);

    ಹಿಂತಿರುಗಿ rb_Deleted_ಅಂಕೆ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ URES_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_ಖಾಲಿಗಳು = ಸುಳ್ಳು;
  int32_t pos = UHASH_ಮೊದಲ;
  ಸ್ಥಿರಾಂಕ UHashElement *e;
  UResource_ದತ್ತಾಂಶ_ನಮೂದು *resB;
  
    Mutex ಬೀಗ(&resbMutex);
    ಆದರೆ (cache == ಶೂನ್ಯಸೂಚಕ) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆಗ ((e = uhash_ಮುಂದಿನ_Element(cache, &pos)) != ಶೂನ್ಯಸೂಚಕ) {
      cache_Not_ಖಾಲಿಗಳು=ನಿಜ;
      resB = (UResource_ದತ್ತಾಂಶ_ನಮೂದು *) e->value.ಸೂಚಕ;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (ಶೂನ್ಯ*)resB, resB->f_ಎಣಿಕೆ_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_ಮಾರ್ಗ?resB->f_ಮಾರ್ಗ:"nullptr",
              (ಶೂನ್ಯ*)resB->fPool,
              (ಶೂನ್ಯ*)resB->fAlias,
              (ಶೂನ್ಯ*)resB->f_ಪೋಷಕ);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_ಎಣಿಕೆ(cache));
    ಹಿಂತಿರುಗಿ cache_Not_ಖಾಲಿಗಳು;
}

#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ UBool U_CALLCONV ures_cleanup()
{
    ಆದರೆ (cache != ಶೂನ್ಯಸೂಚಕ) {
        ures_flushCache();
        uhash_close(cache);
        cache = ಶೂನ್ಯಸೂಚಕ;
    }
    gCacheInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/** INTERNAL: Initializes the cache for resources */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV createCache(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(cache == ಶೂನ್ಯಸೂಚಕ);
    cache = uhash_open(hash_ನಮೂದು, compareEntries, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ಸ್ಥಿರ ಶೂನ್ಯ initCache(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    umtx_initOnce(gCacheInitOnce, &createCache, *ಸ್ಥಿತಿ_2);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_ನಮೂದು_Name(UResource_ದತ್ತಾಂಶ_ನಮೂದು *res, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    ಆದರೆ(res->fName != ಶೂನ್ಯಸೂಚಕ && res->fName != res->f_Name_ಬಫರ್) {
        uprv_free(res->fName);
    }
    ಆದರೆ (len < static_cast<int32_t>(ಗಾತ್ರ(res->f_Name_ಬಫರ್))) {
        res->fName = res->f_Name_ಬಫರ್;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        res->fName = static_cast<ಅಕ್ಷರ*>(uprv_malloc(len + 1));
    }
    ಆದರೆ(res->fName == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        uprv_strcpy(res->fName, name);
    }
}

ಸ್ಥಿರ UResource_ದತ್ತಾಂಶ_ನಮೂದು *
get_Pool_ನಮೂದು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ಸ್ಥಿರ UResource_ದತ್ತಾಂಶ_ನಮೂದು *init_ನಮೂದು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *localeID, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *r = ಶೂನ್ಯಸೂಚಕ;
    UResource_ದತ್ತಾಂಶ_ನಮೂದು find;
    /*int32_t hashValue;*/
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
    ಅಕ್ಷರ aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* here we try to deduce the right locale name */
    ಆದರೆ(localeID == ಶೂನ್ಯಸೂಚಕ) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_ಬೇರು_Locale_Name;
    } ಇಲ್ಲದಿದ್ದರೆ { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<ಅಕ್ಷರ*>(name);
    find.f_ಮಾರ್ಗ = const_cast<ಅಕ್ಷರ*>(ಮಾರ್ಗ);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_ದತ್ತಾಂಶ_ನಮೂದು*>(uhash_get(cache, &find));
    ಆದರೆ(r == ಶೂನ್ಯಸೂಚಕ) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_ದತ್ತಾಂಶ_ನಮೂದು*>(uprv_malloc(ಗಾತ್ರ(UResource_ದತ್ತಾಂಶ_ನಮೂದು)));
        ಆದರೆ(r == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        uprv_memset(r, 0, ಗಾತ್ರ(UResource_ದತ್ತಾಂಶ_ನಮೂದು));
        /*r->fHashKey = hashValue;*/

        ಗಣ_ನಮೂದು_Name(r, name, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            uprv_free(r);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        ಆದರೆ(ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ) {
            r->f_ಮಾರ್ಗ = uprv_strdup(ಮಾರ್ಗ);
            ಆದರೆ(r->f_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                uprv_free(r);
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        }

        /* this is the actual loading */
        res_ತುಂಬು(&(r->f_ದತ್ತಾಂಶ), r->f_ಮಾರ್ಗ, r->fName, ಸ್ಥಿತಿ_2);

        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            ಆದರೆ (*ಸ್ಥಿತಿ_2 == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
                uprv_free(r);
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *ಸ್ಥಿತಿ_2 = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } ಇಲ್ಲದಿದ್ದರೆ { /* if we have a regular entry */
            Resource aliasres;
            ಆದರೆ (r->f_ದತ್ತಾಂಶ.ಬಳಸುತ್ತದೆ_Pool_Bundle) {
                r->fPool = get_Pool_ನಮೂದು(r->f_ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
                    ಸ್ಥಿರಾಂಕ int32_t *poolIndexes = r->fPool->f_ದತ್ತಾಂಶ.p_ಬೇರು + 1;
                    ಆದರೆ(r->f_ದತ್ತಾಂಶ.p_ಬೇರು[1 + URES_ಸೂಚ್ಯಂಕ_POOL_CHECKSUM] == poolIndexes[URES_ಸೂಚ್ಯಂಕ_POOL_CHECKSUM]) {
                        r->f_ದತ್ತಾಂಶ.pool_Bundle_ಕೀಲಿಗಳು = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(poolIndexes + (poolIndexes[URES_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ] & 0xff));
                        r->f_ದತ್ತಾಂಶ.pool_Bundle_ಅಕ್ಷರಸರಣಿಗಳು = r->fPool->f_ದತ್ತಾಂಶ.p16_ಬಿಟ್_Units;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        r->fBogus = *ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    r->fBogus = *ಸ್ಥಿತಿ_2;
                }
            }
            ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_ದತ್ತಾಂಶ), "%%ALIAS");
                ಆದರೆ (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ಸ್ಥಿರಾಂಕ char16_t *alias = res_getStringNoTrace(&(r->f_ದತ್ತಾಂಶ), aliasres, &aliasLen);
                    ಆದರೆ(alias != ಶೂನ್ಯಸೂಚಕ && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_ಗಮ್ಯ_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_ನಮೂದು(aliasName, ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
                    }
                }
            }
        }

        {
            UResource_ದತ್ತಾಂಶ_ನಮೂದು *oldR = ಶೂನ್ಯಸೂಚಕ;
            ಆದರೆ ((oldR = static_cast<UResource_ದತ್ತಾಂಶ_ನಮೂದು*>(uhash_get(cache, r))) == ಶೂನ್ಯಸೂಚಕ) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_ಸಂಕೇತ cache_ಸ್ಥಿತಿ = U_ZERO_ERROR;
                uhash_put(cache, (ಶೂನ್ಯ *)r, r, &cache_ಸ್ಥಿತಿ);
                ಆದರೆ (U_FAILURE(cache_ಸ್ಥಿತಿ)) {
                    *ಸ್ಥಿತಿ_2 = cache_ಸ್ಥಿತಿ;
                    free_ನಮೂದು(r);
                    r = ಶೂನ್ಯಸೂಚಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_ನಮೂದು(r);
                r = oldR;
            }
        }

    }
    ಆದರೆ(r != ಶೂನ್ಯಸೂಚಕ) {
        /* return the real bundle */
        ಆಗ(r->fAlias != ಶೂನ್ಯಸೂಚಕ) {
            r = r->fAlias;
        }
        r->f_ಎಣಿಕೆ_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        ಆದರೆ(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
             *ಸ್ಥಿತಿ_2 = r->fBogus; /* set the returning status */
        }
    }
    ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ UResource_ದತ್ತಾಂಶ_ನಮೂದು *
get_Pool_ನಮೂದು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *poolBundle = init_ನಮೂದು(kPoolBundleName, ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
    ಆದರೆ( U_SUCCESS(*ಸ್ಥಿತಿ_2) &&
        (poolBundle == ಶೂನ್ಯಸೂಚಕ || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_ದತ್ತಾಂಶ.isPoolBundle)
    ) {
        *ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
    }
    ಹಿಂತಿರುಗಿ poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ಸ್ಥಿರ UResource_ದತ್ತಾಂಶ_ನಮೂದು *
ಕಂಡುಹಿಡಿ_ಮೊದಲ_Existing(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಅಕ್ಷರ* name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* defaultLocale, UResOpenType openType,
                  UBool *is_ಬೇರು, UBool *ಸಿಕ್ಕಿತು_ಪೋಷಕ, UBool *isDefault, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *r = ಶೂನ್ಯಸೂಚಕ;
    UBool has_Real_ದತ್ತಾಂಶ = ಸುಳ್ಳು;
    *ಸಿಕ್ಕಿತು_ಪೋಷಕ = ನಿಜ; /* we're starting with a fresh name */
    ಅಕ್ಷರ origName[ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ];

    uprv_strcpy(origName, name);
    ಆಗ(*ಸಿಕ್ಕಿತು_ಪೋಷಕ && !has_Real_ದತ್ತಾಂಶ) {
        r = init_ನಮೂದು(name, ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
        /* Null pointer test */
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_ದತ್ತಾಂಶ = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        ಆದರೆ(!has_Real_ದತ್ತಾಂಶ) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_ಎಣಿಕೆ_Existing--;
            /*entryCloseInt(r);*/
            r = ಶೂನ್ಯಸೂಚಕ;
            *ಸ್ಥಿತಿ_2 = U_USING_FALLBACK_WARNING;
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_ಬೇರು = static_cast<UBool>(uprv_strcmp(name, k_ಬೇರು_Locale_Name) == 0);

        /*Fallback data stuff*/
        ಆದರೆ (!has_Real_ದತ್ತಾಂಶ) {
            *ಸಿಕ್ಕಿತು_ಪೋಷಕ = get_ಪೋಷಕ_Locale_ID(name, origName, openType);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *ಸಿಕ್ಕಿತು_ಪೋಷಕ = chopLocale(name);
        }
        ಆದರೆ (*ಸಿಕ್ಕಿತು_ಪೋಷಕ && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಶೂನ್ಯ ures_ಗಣ_Is_ರಾಶಿ_Object( UResourceBundle* resB, UBool state) {
    ಆದರೆ(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ಸ್ಥಿರ UBool ures_is_ರಾಶಿ_Object(ಸ್ಥಿರಾಂಕ UResourceBundle* resB) {
  ಹಿಂತಿರುಗಿ((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?ಸುಳ್ಳು:ನಿಜ);
}


U_CFUNC ಶೂನ್ಯ ures_init_ರಾಶಿ_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, ಗಾತ್ರ(UResourceBundle));
  ures_ಗಣ_Is_ರಾಶಿ_Object(resB, ನಿಜ);
}

U_NAMESPACE_BEGIN

ರಾಶಿ_UResource_Bundle::ರಾಶಿ_UResource_Bundle() {
    ures_init_ರಾಶಿ_Object(&bundle);
}

ರಾಶಿ_UResource_Bundle::~ರಾಶಿ_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_ಕೊನೆ

ಸ್ಥಿರ UBool  // returns U_SUCCESS(*status)
ತುಂಬು_Parents_Except_ಬೇರು(UResource_ದತ್ತಾಂಶ_ನಮೂದು *&t1,
                      ಅಕ್ಷರ name[], int32_t name_ಸಾಮರ್ಥ್ಯ,
                      UBool usingUSRData, ಅಕ್ಷರ usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ[], UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    UBool check_ಪೋಷಕ = ನಿಜ;
    ಆಗ (check_ಪೋಷಕ && t1->f_ಪೋಷಕ == ಶೂನ್ಯಸೂಚಕ && !t1->f_ದತ್ತಾಂಶ.noFallback &&
            res_getResource(&t1->f_ದತ್ತಾಂಶ,"%%ParentIsRoot") == RES_BOGUS) {
        Resource ಪೋಷಕ_Res = res_getResource(&t1->f_ದತ್ತಾಂಶ, "%%Parent");
        ಆದರೆ (ಪೋಷಕ_Res != RES_BOGUS) {  // An explicit parent was found.
            int32_t ಪೋಷಕ_Locale_Len = 0;
            // No tracing: called during initial data loading
            ಸ್ಥಿರಾಂಕ char16_t *ಪೋಷಕ_Locale_Name = res_getStringNoTrace(&(t1->f_ದತ್ತಾಂಶ), ಪೋಷಕ_Res, &ಪೋಷಕ_Locale_Len);
            ಆದರೆ(ಪೋಷಕ_Locale_Name != ಶೂನ್ಯಸೂಚಕ && 0 < ಪೋಷಕ_Locale_Len && ಪೋಷಕ_Locale_Len < name_ಸಾಮರ್ಥ್ಯ) {
                u_UChars_ಗಮ್ಯ_Chars(ಪೋಷಕ_Locale_Name, name, ಪೋಷಕ_Locale_Len + 1);
                ಆದರೆ (uprv_strcmp(name, k_ಬೇರು_Locale_Name) == 0) {
                    ಹಿಂತಿರುಗಿ ನಿಜ;
                }
            }
        }
        // Insert regular parents.
        UError_ಸಂಕೇತ ಪೋಷಕ_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        UResource_ದತ್ತಾಂಶ_ನಮೂದು *t2 = init_ನಮೂದು(name, t1->f_ಮಾರ್ಗ, &ಪೋಷಕ_ಸ್ಥಿತಿ);
        ಆದರೆ (U_FAILURE(ಪೋಷಕ_ಸ್ಥಿತಿ)) {
            *ಸ್ಥಿತಿ_2 = ಪೋಷಕ_ಸ್ಥಿತಿ;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        UResource_ದತ್ತಾಂಶ_ನಮೂದು *u2 = ಶೂನ್ಯಸೂಚಕ;
        UError_ಸಂಕೇತ usr_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        ಆದರೆ (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_ನಮೂದು(name, usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ, &usr_ಸ್ಥಿತಿ);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            ಆದರೆ (usr_ಸ್ಥಿತಿ == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
                *ಸ್ಥಿತಿ_2 = usr_ಸ್ಥಿತಿ;
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }

        ಆದರೆ (usingUSRData && U_SUCCESS(usr_ಸ್ಥಿತಿ) && u2->fBogus == U_ZERO_ERROR) {
            t1->f_ಪೋಷಕ = u2;
            u2->f_ಪೋಷಕ = t2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            t1->f_ಪೋಷಕ = t2;
            ಆದರೆ (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_ಎಣಿಕೆ_Existing = 0;
            }
        }
        t1 = t2;
        check_ಪೋಷಕ = chopLocale(name) || may_Have_ಪೋಷಕ(name);
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ UBool  // returns U_SUCCESS(*status)
ಸೇರಿಸು_ಬೇರು_Bundle(UResource_ದತ್ತಾಂಶ_ನಮೂದು *&t1, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    UError_ಸಂಕೇತ ಪೋಷಕ_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *t2 = init_ನಮೂದು(k_ಬೇರು_Locale_Name, t1->f_ಮಾರ್ಗ, &ಪೋಷಕ_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(ಪೋಷಕ_ಸ್ಥಿತಿ)) {
        *ಸ್ಥಿತಿ_2 = ಪೋಷಕ_ಸ್ಥಿತಿ;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    t1->f_ಪೋಷಕ = t2;
    t1 = t2;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ UResource_ದತ್ತಾಂಶ_ನಮೂದು *ನಮೂದು_Open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                                     UResOpenType openType, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    U_ASSERT(openType != URES_OPEN_ನೇರ);
    UError_ಸಂಕೇತ int_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *r = ಶೂನ್ಯಸೂಚಕ;
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *t1 = ಶೂನ್ಯಸೂಚಕ;
    UBool isDefault = ಸುಳ್ಳು;
    UBool is_ಬೇರು = ಸುಳ್ಳು;
    UBool has_Real_ದತ್ತಾಂಶ = ಸುಳ್ಳು;
    UBool hasChopped = ನಿಜ;
    UBool usingUSRData = U_USE_USRDATA && ( ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ || uprv_strncmp(ಮಾರ್ಗ,U_ICUDATA_NAME,8) == 0);

    ಅಕ್ಷರ name[ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ];
    ಅಕ್ಷರ usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ[96];

    initCache(ಸ್ಥಿತಿ_2);

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    uprv_strncpy(name, localeID, ಗಾತ್ರ(name) - 1);
    name[ಗಾತ್ರ(name) - 1] = 0;

    ಆದರೆ ( usingUSRData ) {
        ಆದರೆ ( ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ ) {
            uprv_strcpy(usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ, U_USRDATA_NAME);
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_strncpy(usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ, ಮಾರ್ಗ, ಗಾತ್ರ(usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ) - 1);
            usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ[0] = 'u';
            usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ[1] = 's';
            usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ[2] = 'r';
            usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ[ಗಾತ್ರ(usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *defaultLocale = uloc_getDefault();

    Mutex ಬೀಗ(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = ಕಂಡುಹಿಡಿ_ಮೊದಲ_Existing(ಮಾರ್ಗ, name, defaultLocale, openType, &is_ಬೇರು, &hasChopped, &isDefault, &int_ಸ್ಥಿತಿ);

    // If we failed due to out-of-memory, report the failure and exit early.
    ಆದರೆ (int_ಸ್ಥಿತಿ == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
        *ಸ್ಥಿತಿ_2 = int_ಸ್ಥಿತಿ;
        ಹೋಗು ಮುಕ್ತಾಯ;
    }

    ಆದರೆ(r != ಶೂನ್ಯಸೂಚಕ) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_ದತ್ತಾಂಶ = ನಿಜ;
        ಆದರೆ ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_ಸಂಕೇತ usr_ಸ್ಥಿತಿ = U_ZERO_ERROR;
            UResource_ದತ್ತಾಂಶ_ನಮೂದು *u1 = init_ನಮೂದು(t1->fName, usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ, &usr_ಸ್ಥಿತಿ);
            // If we failed due to out-of-memory, report the failure and exit early.
            ಆದರೆ (int_ಸ್ಥಿತಿ == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
                *ಸ್ಥಿತಿ_2 = int_ಸ್ಥಿತಿ;
                ಹೋಗು ಮುಕ್ತಾಯ;
            }
            ಆದರೆ ( u1 != ಶೂನ್ಯಸೂಚಕ ) {
                ಆದರೆ(u1->fBogus == U_ZERO_ERROR) {
                    u1->f_ಪೋಷಕ = t1;
                    r = u1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_ಎಣಿಕೆ_Existing = 0;
                }
            }
        }
        ಆದರೆ ((hasChopped || may_Have_ಪೋಷಕ(name)) && !is_ಬೇರು) {
            ಆದರೆ (!ತುಂಬು_Parents_Except_ಬೇರು(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ, ಸ್ಥಿತಿ_2)) {
                ಹೋಗು ಮುಕ್ತಾಯ;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    ಆದರೆ(r==ಶೂನ್ಯಸೂಚಕ && openType == URES_OPEN_LOCALE_DEFAULT_ಬೇರು && !isDefault && !is_ಬೇರು) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = ಕಂಡುಹಿಡಿ_ಮೊದಲ_Existing(ಮಾರ್ಗ, name, defaultLocale, openType, &is_ಬೇರು, &hasChopped, &isDefault, &int_ಸ್ಥಿತಿ);
        // If we failed due to out-of-memory, report the failure and exit early.
        ಆದರೆ (int_ಸ್ಥಿತಿ == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
            *ಸ್ಥಿತಿ_2 = int_ಸ್ಥಿತಿ;
            ಹೋಗು ಮುಕ್ತಾಯ;
        }
        int_ಸ್ಥಿತಿ = U_USING_DEFAULT_WARNING;
        ಆದರೆ(r != ಶೂನ್ಯಸೂಚಕ) { /* the default locale exists */
            t1 = r;
            has_Real_ದತ್ತಾಂಶ = ನಿಜ;
            isDefault = ನಿಜ;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            ಆದರೆ ((hasChopped || may_Have_ಪೋಷಕ(name)) && !is_ಬೇರು) {
                ಆದರೆ (!ತುಂಬು_Parents_Except_ಬೇರು(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_ದತ್ತಾಂಶ_ಮಾರ್ಗ, ಸ್ಥಿತಿ_2)) {
                    ಹೋಗು ಮುಕ್ತಾಯ;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    ಆದರೆ(r == ಶೂನ್ಯಸೂಚಕ) {
        uprv_strcpy(name, k_ಬೇರು_Locale_Name);
        r = ಕಂಡುಹಿಡಿ_ಮೊದಲ_Existing(ಮಾರ್ಗ, name, defaultLocale, openType, &is_ಬೇರು, &hasChopped, &isDefault, &int_ಸ್ಥಿತಿ);
        // If we failed due to out-of-memory, report the failure and exit early.
        ಆದರೆ (int_ಸ್ಥಿತಿ == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
            *ಸ್ಥಿತಿ_2 = int_ಸ್ಥಿತಿ;
            ಹೋಗು ಮುಕ್ತಾಯ;
        }
        ಆದರೆ(r != ಶೂನ್ಯಸೂಚಕ) {
            t1 = r;
            int_ಸ್ಥಿತಿ = U_USING_DEFAULT_WARNING;
            has_Real_ದತ್ತಾಂಶ = ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ { /* we don't even have the root locale */
            *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
            ಹೋಗು ಮುಕ್ತಾಯ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!is_ಬೇರು && uprv_strcmp(t1->fName, k_ಬೇರು_Locale_Name) != 0 &&
            t1->f_ಪೋಷಕ == ಶೂನ್ಯಸೂಚಕ && !r->f_ದತ್ತಾಂಶ.noFallback) {
        ಆದರೆ (!ಸೇರಿಸು_ಬೇರು_Bundle(t1, ಸ್ಥಿತಿ_2)) {
            ಹೋಗು ಮುಕ್ತಾಯ;
        }
        ಆದರೆ(!has_Real_ದತ್ತಾಂಶ) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    ಆಗ(r != ಶೂನ್ಯಸೂಚಕ && !is_ಬೇರು && t1->f_ಪೋಷಕ != ಶೂನ್ಯಸೂಚಕ) {
        t1->f_ಪೋಷಕ->f_ಎಣಿಕೆ_Existing++;
        t1 = t1->f_ಪೋಷಕ;
    }

ಮುಕ್ತಾಯ:
    ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ಆದರೆ(int_ಸ್ಥಿತಿ != U_ZERO_ERROR) {
            *ಸ್ಥಿತಿ_2 = int_ಸ್ಥಿತಿ;  
        }
        ಹಿಂತಿರುಗಿ r;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ಸ್ಥಿರ UResource_ದತ್ತಾಂಶ_ನಮೂದು *
ನಮೂದು_Open_ನೇರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    initCache(ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_ಬೇರು_Locale_Name;
    }

    Mutex ಬೀಗ(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *r = init_ನಮೂದು(localeID, ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ಆದರೆ(r->fBogus != U_ZERO_ERROR) {
            r->f_ಎಣಿಕೆ_Existing--;
            r = ಶೂನ್ಯಸೂಚಕ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        r = ಶೂನ್ಯಸೂಚಕ;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *t1 = r;
    ಆದರೆ(r != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(localeID, k_ಬೇರು_Locale_Name) != 0 &&  // not root
            r->f_ಪೋಷಕ == ಶೂನ್ಯಸೂಚಕ && !r->f_ದತ್ತಾಂಶ.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ) {
        ಅಕ್ಷರ name[ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ];
        uprv_strcpy(name, localeID);
        ಆದರೆ(!chopLocale(name) || uprv_strcmp(name, k_ಬೇರು_Locale_Name) == 0 ||
                ತುಂಬು_Parents_Except_ಬೇರು(t1, name, UPRV_LENGTHOF(name), ಸುಳ್ಳು, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2)) {
            ಆದರೆ(uprv_strcmp(t1->fName, k_ಬೇರು_Locale_Name) != 0 && t1->f_ಪೋಷಕ == ಶೂನ್ಯಸೂಚಕ) {
                ಸೇರಿಸು_ಬೇರು_Bundle(t1, ಸ್ಥಿತಿ_2);
            }
        }
        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            r = ಶೂನ್ಯಸೂಚಕ;
        }
    }

    ಆದರೆ(r != ಶೂನ್ಯಸೂಚಕ) {
        // TODO: Does this ever loop?
        ಆಗ(t1->f_ಪೋಷಕ != ಶೂನ್ಯಸೂಚಕ) {
            t1->f_ಪೋಷಕ->f_ಎಣಿಕೆ_Existing++;
            t1 = t1->f_ಪೋಷಕ;
        }
    }
    ಹಿಂತಿರುಗಿ r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ಸ್ಥಿರ ಶೂನ್ಯ ನಮೂದು_Close_Int(UResource_ದತ್ತಾಂಶ_ನಮೂದು *resB) {
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *p = resB;

    ಆಗ(resB != ಶೂನ್ಯಸೂಚಕ) {
        p = resB->f_ಪೋಷಕ;
        resB->f_ಎಣಿಕೆ_Existing--;

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

ಸ್ಥಿರ ಶೂನ್ಯ ನಮೂದು_Close(UResource_ದತ್ತಾಂಶ_ನಮೂದು *resB) {
  Mutex ಬೀಗ(&resbMutex);
  ನಮೂದು_Close_Int(resB);
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
ಸ್ಥಿರ ಶೂನ್ಯ ures_append_Res_ಮಾರ್ಗ(UResourceBundle *resB, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಗಮ್ಯ_ಕೂಡಿಸು, int32_t len_ಗಮ್ಯ_ಕೂಡಿಸು, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    int32_t res_ಮಾರ್ಗ_Len_Orig = resB->f_Res_ಮಾರ್ಗ_Len;
    ಆದರೆ(resB->f_Res_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
        resB->f_Res_ಮಾರ್ಗ = resB->fResBuf;
        *(resB->f_Res_ಮಾರ್ಗ) = 0;
        resB->f_Res_ಮಾರ್ಗ_Len = 0;
    } 
    resB->f_Res_ಮಾರ್ಗ_Len += len_ಗಮ್ಯ_ಕೂಡಿಸು;
    ಆದರೆ(RES_BUFSIZE <= resB->f_Res_ಮಾರ್ಗ_Len+1) {
        ಆದರೆ(resB->f_Res_ಮಾರ್ಗ == resB->fResBuf) {
            resB->f_Res_ಮಾರ್ಗ = static_cast<ಅಕ್ಷರ*>(uprv_malloc((resB->f_Res_ಮಾರ್ಗ_Len + 1) * ಗಾತ್ರ(ಅಕ್ಷರ)));
            /* Check that memory was allocated correctly. */
            ಆದರೆ (resB->f_Res_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            uprv_strcpy(resB->f_Res_ಮಾರ್ಗ, resB->fResBuf);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಅಕ್ಷರ* ತಾತ್ಕಾಲಿಕ = static_cast<ಅಕ್ಷರ*>(uprv_realloc(resB->f_Res_ಮಾರ್ಗ, (resB->f_Res_ಮಾರ್ಗ_Len + 1) * ಗಾತ್ರ(ಅಕ್ಷರ)));
            /* Check that memory was reallocated correctly. */
            ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಶೂನ್ಯಸೂಚಕ) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            resB->f_Res_ಮಾರ್ಗ = ತಾತ್ಕಾಲಿಕ;
        }
    }
    uprv_strcpy(resB->f_Res_ಮಾರ್ಗ + res_ಮಾರ್ಗ_Len_Orig, ಗಮ್ಯ_ಕೂಡಿಸು);
}

ಸ್ಥಿರ ಶೂನ್ಯ ures_free_Res_ಮಾರ್ಗ(UResourceBundle *resB) {
    ಆದರೆ (resB->f_Res_ಮಾರ್ಗ && resB->f_Res_ಮಾರ್ಗ != resB->fResBuf) {
        uprv_free(resB->f_Res_ಮಾರ್ಗ);
    }
    resB->f_Res_ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
    resB->f_Res_ಮಾರ್ಗ_Len = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    ಆದರೆ(resB != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(resB->f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
            ನಮೂದು_Close(resB->f_ದತ್ತಾಂಶ);
        }
        ಆದರೆ(resB->f_ಆವೃತ್ತಿ != ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(resB->f_ಆವೃತ್ತಿ);
        }
        ures_free_Res_ಮಾರ್ಗ(resB);

        ಆದರೆ(ures_is_ರಾಶಿ_Object(resB) == ಸುಳ್ಳು && freeBundleObj) {
            uprv_free(resB);
        }
#ಆದರೆ 0 /*U_DEBUG*/
        ಇಲ್ಲದಿದ್ದರೆ {
            /* poison the data */
            uprv_memset(resB, -1, ಗಾತ್ರ(UResourceBundle));
        }
#ಆದರೆ_ಅಂತ್ಯ
    }
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, ನಿಜ);
}

ನಾಮಾಕಾಶ {

UResourceBundle* init_resb_ಫಲಿತಾಂಶ(UResource_ದತ್ತಾಂಶ_ನಮೂದು* ದತ್ತಾಂಶ_ನಮೂದು,
                                  Resource r,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                  int32_t idx,
                                  UResource_ದತ್ತಾಂಶ_ನಮೂದು* valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* container_Res_ಮಾರ್ಗ,
                                  int32_t recursion_ಆಳ,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
get_Alias_ಗುರಿ_As_Resource_Bundle(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ& res_ದತ್ತಾಂಶ,
                               Resource r,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                               int32_t idx,
                               UResource_ದತ್ತಾಂಶ_ನಮೂದು* valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* container_Res_ಮಾರ್ಗ,
                               int32_t recursion_ಆಳ,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ಸ್ಥಿರಾಂಕ char16_t *alias = res_getAlias(&res_ದತ್ತಾಂಶ, r, &len);
    ಆದರೆ(len <= 0) {
        // bad alias
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ resB;
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
    chAlias.appendInvariantChars(alias, len, *ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // We have an alias, now let's cut it up.
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ, *locale = ಶೂನ್ಯಸೂಚಕ, *ಕೀಲಿ_ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ(chAlias[0] == RES_ಮಾರ್ಗ_SEPARATOR) {
        // There is a path included.
        ಅಕ್ಷರ *ch_Alias_ದತ್ತಾಂಶ = chAlias.ದತ್ತಾಂಶ();
        ಅಕ್ಷರ *sep = ch_Alias_ದತ್ತಾಂಶ + 1;
        ಮಾರ್ಗ = sep;
        sep = uprv_strchr(sep, RES_ಮಾರ್ಗ_SEPARATOR);
        ಆದರೆ(sep != ಶೂನ್ಯಸೂಚಕ) {
            *sep++ = 0;
        }
        ಆದರೆ(uprv_strcmp(ಮಾರ್ಗ, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            ಕೀಲಿ_ಮಾರ್ಗ = sep;
            // Read from the valid locale which we already have.
            ಮಾರ್ಗ = locale = ಶೂನ್ಯಸೂಚಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(uprv_strcmp(ಮಾರ್ಗ, "ICUDATA") == 0) { /* want ICU data */
                ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
            }
            ಆದರೆ (sep == ಶೂನ್ಯಸೂಚಕ) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } ಇಲ್ಲದಿದ್ದರೆ {
                locale = sep;
                sep = uprv_strchr(sep, RES_ಮಾರ್ಗ_SEPARATOR);
                ಆದರೆ(sep != ಶೂನ್ಯಸೂಚಕ) {
                    *sep++ = 0;
                }
                ಕೀಲಿ_ಮಾರ್ಗ = sep;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // No path, start with a locale.
        ಅಕ್ಷರ *sep = chAlias.ದತ್ತಾಂಶ();
        locale = sep;
        sep = uprv_strchr(sep, RES_ಮಾರ್ಗ_SEPARATOR);
        ಆದರೆ(sep != ಶೂನ್ಯಸೂಚಕ) {
            *sep++ = 0;
        }
        ಕೀಲಿ_ಮಾರ್ಗ = sep;
        ಮಾರ್ಗ = valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು->f_ಮಾರ್ಗ;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    Local_UResource_Bundle_ಸೂಚಕ mainRes;
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *ದತ್ತಾಂಶ_ನಮೂದು;
    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        ದತ್ತಾಂಶ_ನಮೂದು = valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        UError_ಸಂಕೇತ int_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_ನೇರ(ಮಾರ್ಗ, locale, &int_ಸ್ಥಿತಿ));
        ಆದರೆ(U_FAILURE(int_ಸ್ಥಿತಿ)) {
            // We failed to open the resource bundle we're aliasing to.
            *ಸ್ಥಿತಿ_2 = int_ಸ್ಥಿತಿ;
            ಹಿಂತಿರುಗಿ resB;
        }
        ದತ್ತಾಂಶ_ನಮೂದು = mainRes->f_ದತ್ತಾಂಶ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ತಾತ್ಕಾಲಿಕ = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ(ಕೀಲಿ_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ.ಬೇರು_Res;
        ಆದರೆ(container_Res_ಮಾರ್ಗ) {
            chAlias.clear().append(container_Res_ಮಾರ್ಗ, *ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            ಅಕ್ಷರ *a_ಕೀಲಿ = chAlias.ದತ್ತಾಂಶ();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_ಕಂಡುಹಿಡಿ_Resource(&ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ, r, &a_ಕೀಲಿ, &ತಾತ್ಕಾಲಿಕ);
        }
        ಆದರೆ(ಕೀಲಿ) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(ಕೀಲಿ, *ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            ಅಕ್ಷರ *a_ಕೀಲಿ = chAlias.ದತ್ತಾಂಶ();
            r = res_ಕಂಡುಹಿಡಿ_Resource(&ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ, r, &a_ಕೀಲಿ, &ತಾತ್ಕಾಲಿಕ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            ಆದರೆ(URES_IS_ಕೋಷ್ಟಕ(type)) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a_ಕೀಲಿ;
                r = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಸೂಚ್ಯಂಕ(&ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ, r, idx, &a_ಕೀಲಿ);
            } ಇಲ್ಲದಿದ್ದರೆ { /* array */
                r = res_get_Array_ಅಂಶ(&ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ, r, idx);
            }
        }
        ಆದರೆ(r != RES_BOGUS) {
            resB = init_resb_ಫಲಿತಾಂಶ(
                ದತ್ತಾಂಶ_ನಮೂದು, r, ತಾತ್ಕಾಲಿಕ, -1, valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು, ಶೂನ್ಯಸೂಚಕ, recursion_ಆಳ+1,
                resB, ಸ್ಥಿತಿ_2);
        } ಇಲ್ಲದಿದ್ದರೆ {
            *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString ಮಾರ್ಗ_Buf(ಕೀಲಿ_ಮಾರ್ಗ, *ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಅಕ್ಷರ *my_ಮಾರ್ಗ = ಮಾರ್ಗ_Buf.ದತ್ತಾಂಶ();
        container_Res_ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
        // Now we have fallback following here.
        ಗಾಗಿ(;;) {
            r = ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ.ಬೇರು_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            ಆಗ(*my_ಮಾರ್ಗ && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
                r = res_ಕಂಡುಹಿಡಿ_Resource(&(ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ), r, &my_ಮಾರ್ಗ, &ತಾತ್ಕಾಲಿಕ);
                ಆದರೆ(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    ನಿಲ್ಲಿಸು;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_ಫಲಿತಾಂಶ(
                    ದತ್ತಾಂಶ_ನಮೂದು, r, ತಾತ್ಕಾಲಿಕ, -1,
                    valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು, container_Res_ಮಾರ್ಗ, recursion_ಆಳ+1,
                    resB, ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಶೂನ್ಯಸೂಚಕ || uprv_strcmp(ಕೀಲಿ_ಮಾರ್ಗ, ತಾತ್ಕಾಲಿಕ) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_ಮಾರ್ಗ(resB);
                    ures_append_Res_ಮಾರ್ಗ(resB, ಕೀಲಿ_ಮಾರ್ಗ, static_cast<int32_t>(uprv_strlen(ಕೀಲಿ_ಮಾರ್ಗ)), ಸ್ಥಿತಿ_2);
                    ಆದರೆ(resB->f_Res_ಮಾರ್ಗ[resB->f_Res_ಮಾರ್ಗ_Len-1] != RES_ಮಾರ್ಗ_SEPARATOR) {
                        ures_append_Res_ಮಾರ್ಗ(resB, RES_ಮಾರ್ಗ_SEPARATOR_S, 1, ಸ್ಥಿತಿ_2);
                    }
                    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                        ನಿಲ್ಲಿಸು;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                ದತ್ತಾಂಶ_ನಮೂದು = resB->f_ದತ್ತಾಂಶ;
                container_Res_ಮಾರ್ಗ = resB->f_Res_ಮಾರ್ಗ;
            }
            ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2) || r != RES_BOGUS) {
                ನಿಲ್ಲಿಸು;
            }
            // Fall back to the parent bundle, if there is one.
            ದತ್ತಾಂಶ_ನಮೂದು = ದತ್ತಾಂಶ_ನಮೂದು->f_ಪೋಷಕ;
            ಆದರೆ (ದತ್ತಾಂಶ_ನಮೂದು == ಶೂನ್ಯಸೂಚಕ) {
                *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            // Copy the same keyPath again.
            my_ಮಾರ್ಗ = ಮಾರ್ಗ_Buf.ದತ್ತಾಂಶ();
            uprv_strcpy(my_ಮಾರ್ಗ, ಕೀಲಿ_ಮಾರ್ಗ);
        }
    }
    ಆದರೆ(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    ಹಿಂತಿರುಗಿ resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_ಫಲಿತಾಂಶ(UResource_ದತ್ತಾಂಶ_ನಮೂದು* ದತ್ತಾಂಶ_ನಮೂದು,
                                  Resource r,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                  int32_t idx,
                                  UResource_ದತ್ತಾಂಶ_ನಮೂದು* valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* container_Res_ಮಾರ್ಗ,
                                  int32_t recursion_ಆಳ,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ resB;
    }
    ಆದರೆ (valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        ಆದರೆ(recursion_ಆಳ >= URES_MAX_ALIAS_LEVEL) {
            *ಸ್ಥಿತಿ_2 = U_TOO_MANY_ALIASES_ERROR;
            ಹಿಂತಿರುಗಿ resB;
        }
        ಹಿಂತಿರುಗಿ get_Alias_ಗುರಿ_As_Resource_Bundle(
            ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ, r, ಕೀಲಿ, idx,
            valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು, container_Res_ಮಾರ್ಗ, recursion_ಆಳ, resB, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(ಗಾತ್ರ(UResourceBundle)));
        ಆದರೆ (resB == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ures_ಗಣ_Is_ರಾಶಿ_Object(resB, ಸುಳ್ಳು);
        resB->f_Res_ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
        resB->f_Res_ಮಾರ್ಗ_Len = 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(resB->f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
            ನಮೂದು_Close(resB->f_ದತ್ತಾಂಶ);
        }
        ಆದರೆ(resB->f_ಆವೃತ್ತಿ != ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(resB->f_ಆವೃತ್ತಿ);
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
        ಆದರೆ(container_Res_ಮಾರ್ಗ != resB->f_Res_ಮಾರ್ಗ) {
            ures_free_Res_ಮಾರ್ಗ(resB);
        }
    }
    resB->f_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ_ನಮೂದು;
    ನಮೂದು_Increase(resB->f_ದತ್ತಾಂಶ);
    resB->fHasFallback = ಸುಳ್ಳು;
    resB->f_Is_ತುದಿ_Level = ಸುಳ್ಳು;
    resB->f_ಸೂಚ್ಯಂಕ = -1;
    resB->f_ಕೀಲಿ = ಕೀಲಿ; 
    resB->f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು = valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು;
    ಆದರೆ(container_Res_ಮಾರ್ಗ != resB->f_Res_ಮಾರ್ಗ) {
        ures_append_Res_ಮಾರ್ಗ(
            resB, container_Res_ಮಾರ್ಗ, static_cast<int32_t>(uprv_strlen(container_Res_ಮಾರ್ಗ)), ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ(ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ) {
        ures_append_Res_ಮಾರ್ಗ(resB, ಕೀಲಿ, static_cast<int32_t>(uprv_strlen(ಕೀಲಿ)), ಸ್ಥಿತಿ_2);
        ಆದರೆ(resB->f_Res_ಮಾರ್ಗ_Len > 0 && resB->f_Res_ಮಾರ್ಗ[resB->f_Res_ಮಾರ್ಗ_Len-1] != RES_ಮಾರ್ಗ_SEPARATOR) {
            ures_append_Res_ಮಾರ್ಗ(resB, RES_ಮಾರ್ಗ_SEPARATOR_S, 1, ಸ್ಥಿತಿ_2);
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(idx >= 0) {
        ಅಕ್ಷರ buf[256];
        int32_t len = T_CString_integer_ಗಮ್ಯ_String(buf, idx, 10);
        ures_append_Res_ಮಾರ್ಗ(resB, buf, len, ಸ್ಥಿತಿ_2);
        ಆದರೆ(resB->f_Res_ಮಾರ್ಗ_Len > 0 && resB->f_Res_ಮಾರ್ಗ[resB->f_Res_ಮಾರ್ಗ_Len-1] != RES_ಮಾರ್ಗ_SEPARATOR) {
            ures_append_Res_ಮಾರ್ಗ(resB, RES_ಮಾರ್ಗ_SEPARATOR_S, 1, ಸ್ಥಿತಿ_2);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t ಬಳಸಿದ_Len = ((resB->fResBuf == resB->f_Res_ಮಾರ್ಗ) ? resB->f_Res_ಮಾರ್ಗ_Len : 0);
        uprv_memset(resB->fResBuf + ಬಳಸಿದ_Len, 0, ಗಾತ್ರ(resB->fResBuf) - ಬಳಸಿದ_Len);
    }

    resB->f_ಆವೃತ್ತಿ = ಶೂನ್ಯಸೂಚಕ;
    resB->fRes = r;
    resB->f_ಗಾತ್ರ = res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes);
    ResourceTracer(resB).trace("get");
    ಹಿಂತಿರುಗಿ resB;
}

UResourceBundle* init_resb_ಫಲಿತಾಂಶ(UResource_ದತ್ತಾಂಶ_ನಮೂದು* ದತ್ತಾಂಶ_ನಮೂದು,
                                  Resource r,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ಸ್ಥಿರಾಂಕ UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ init_resb_ಫಲಿತಾಂಶ(
        ದತ್ತಾಂಶ_ನಮೂದು, r, ಕೀಲಿ, idx,
        container->f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು, container->f_Res_ಮಾರ್ಗ, 0, resB, ಸ್ಥಿತಿ_2);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ಸ್ಥಿರಾಂಕ UResourceBundle* original,
                               UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    UBool is_ರಾಶಿ_Object;
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2) || r == original) {
        ಹಿಂತಿರುಗಿ r;
    }
    ಆದರೆ(original != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(r == ಶೂನ್ಯಸೂಚಕ) {
            is_ರಾಶಿ_Object = ಸುಳ್ಳು;
            r = static_cast<UResourceBundle*>(uprv_malloc(ಗಾತ್ರ(UResourceBundle)));
            /* test for nullptr */
            ಆದರೆ (r == ಶೂನ್ಯಸೂಚಕ) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            is_ರಾಶಿ_Object = ures_is_ರಾಶಿ_Object(r);
            ures_closeBundle(r, ಸುಳ್ಳು);
        }
        uprv_memcpy(r, original, ಗಾತ್ರ(UResourceBundle));
        r->f_Res_ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
        r->f_Res_ಮಾರ್ಗ_Len = 0;
        ಆದರೆ(original->f_Res_ಮಾರ್ಗ) {
            ures_append_Res_ಮಾರ್ಗ(r, original->f_Res_ಮಾರ್ಗ, original->f_Res_ಮಾರ್ಗ_Len, ಸ್ಥಿತಿ_2);
        }
        ures_ಗಣ_Is_ರಾಶಿ_Object(r, is_ರಾಶಿ_Object);
        ಆದರೆ(r->f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
            ನಮೂದು_Increase(r->f_ದತ್ತಾಂಶ);
        }
    }
    ಹಿಂತಿರುಗಿ r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2 ures_getString(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ char16_t *s;
    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    s = res_getString({resB}, &resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, len);
    ಆದರೆ (s == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ s;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
ures_ಗಮ್ಯ_UTF8_String(ಸ್ಥಿರಾಂಕ char16_t* s16,
                  int32_t length16,
                  ಅಕ್ಷರ* dest U_LIFETIME_BOUND,
                  int32_t* p_ಉದ್ದಳತೆ,
                  UBool forceCopy,
                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    int32_t capacity;

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (p_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
        capacity = *p_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        capacity = 0;
    }
    ಆದರೆ (capacity < 0 || (capacity > 0 && dest == ಶೂನ್ಯಸೂಚಕ)) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (length16 == 0) {
        /* empty string, return as read-only pointer */
        ಆದರೆ (p_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
            *p_ಉದ್ದಳತೆ = 0;
        }
        ಆದರೆ (forceCopy) {
            u_terminateChars(dest, capacity, 0, ಸ್ಥಿತಿ_2);
            ಹಿಂತಿರುಗಿ dest;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ "";
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* We need to transform the string to the destination buffer. */
        ಆದರೆ (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            ಹಿಂತಿರುಗಿ u_str_ಗಮ್ಯ_UTF8(ಶೂನ್ಯಸೂಚಕ, 0, p_ಉದ್ದಳತೆ, s16, length16, ಸ್ಥಿತಿ_2);
        }
        ಆದರೆ (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            int32_t max_ಉದ್ದಳತೆ_2 = 3 * length16 + 1;
            ಆದರೆ (capacity > max_ಉದ್ದಳತೆ_2) {
                dest += capacity - max_ಉದ್ದಳತೆ_2;
                capacity = max_ಉದ್ದಳತೆ_2;
            }
        }
        ಹಿಂತಿರುಗಿ u_str_ಗಮ್ಯ_UTF8(dest, capacity, p_ಉದ್ದಳತೆ, s16, length16, ಸ್ಥಿತಿ_2);
    }
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_getUTF8String(ಸ್ಥಿರಾಂಕ UResourceBundle* resB,
                   ಅಕ್ಷರ* dest U_LIFETIME_BOUND,
                   int32_t* p_ಉದ್ದಳತೆ,
                   UBool forceCopy,
                   UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    int32_t length16;
    ಸ್ಥಿರಾಂಕ char16_t *s16 = ures_getString(resB, &length16, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ ures_ಗಮ್ಯ_UTF8_String(s16, length16, dest, p_ಉದ್ದಳತೆ, forceCopy, ಸ್ಥಿತಿ_2);
}

U_CAPI ಸ್ಥಿರಾಂಕ uint8_t* U_EXPORT2
ures_get_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  ಸ್ಥಿರಾಂಕ uint8_t *p;
  ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  p = res_get_ದ್ವಿಮಾನ({resB}, &resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, len);
  ಆದರೆ (p == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
  }
  ಹಿಂತಿರುಗಿ p;
}

U_CAPI ಸ್ಥಿರಾಂಕ int32_t* U_EXPORT2
ures_get_Int_ಸದಿಶ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  ಸ್ಥಿರಾಂಕ int32_t *p;
  ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  p = res_get_Int_ಸದಿಶ({resB}, &resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, len);
  ಆದರೆ (p == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
  }
  ಹಿಂತಿರುಗಿ p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ಸ್ಥಿರಾಂಕ UResourceBundle* resB, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ 0xffffffff;
  }
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0xffffffff;
  }
  ಆದರೆ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
    ಹಿಂತಿರುಗಿ 0xffffffff;
  }
  ಹಿಂತಿರುಗಿ res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ಸ್ಥಿರಾಂಕ UResourceBundle* resB, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ 0xffffffff;
  }
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0xffffffff;
  }
  ಆದರೆ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
    ಹಿಂತಿರುಗಿ 0xffffffff;
  }
  ಹಿಂತಿರುಗಿ res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ಸ್ಥಿರಾಂಕ UResourceBundle *resB) {
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ URES_NONE;
  }
  ಹಿಂತಿರುಗಿ res_get_ಬಹಿರಂಗ_Type(resB->fRes);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_get_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಹಿಂತಿರುಗಿ(resB->f_ಕೀಲಿ);
}

U_CAPI int32_t U_EXPORT2 ures_get_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ UResourceBundle *resB) {
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ 0;
  }
  
  ಹಿಂತಿರುಗಿ resB->f_ಗಾತ್ರ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t*
ures_getStringWithAlias(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_ಸೂಚ್ಯಂಕ,
                        int32_t* len,
                        UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  ಆದರೆ(RES_GET_TYPE(r) == URES_ALIAS) {
    ಸ್ಥಿರಾಂಕ char16_t* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    UResourceBundle *ತಾತ್ಕಾಲಿಕ_Res = ures_get_By_ಸೂಚ್ಯಂಕ(resB, s_ಸೂಚ್ಯಂಕ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
    ಫಲಿತಾಂಶ = ures_getString(ತಾತ್ಕಾಲಿಕ_Res, len, ಸ್ಥಿತಿ_2);
    ures_close(ತಾತ್ಕಾಲಿಕ_Res);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ res_getString({resB, s_ಸೂಚ್ಯಂಕ}, &resB->get_Res_ದತ್ತಾಂಶ(), r, len); 
  }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ;
  }
  resB->f_ಸೂಚ್ಯಂಕ = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ UResourceBundle *resB) {
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  }
  ಹಿಂತಿರುಗಿ resB->f_ಸೂಚ್ಯಂಕ < resB->f_ಗಾತ್ರ-1;
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
ures_get_ಮುಂದಿನ_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** ಕೀಲಿ,
                   UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  Resource r = RES_BOGUS;
  
  ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  
  ಆದರೆ(resB->f_ಸೂಚ್ಯಂಕ == resB->f_ಗಾತ್ರ-1) {
    *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
  } ಇಲ್ಲದಿದ್ದರೆ {
    resB->f_ಸೂಚ್ಯಂಕ++;
    ಆಯ್ಕೆ(RES_GET_TYPE(resB->fRes)) {
    ಸ್ಥಿತಿ URES_STRING:
    ಸ್ಥಿತಿ URES_STRING_V2:
      ಹಿಂತಿರುಗಿ res_getString({resB}, &resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, len);
    ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ:
    ಸ್ಥಿತಿ URES_TABLE16:
    ಸ್ಥಿತಿ URES_TABLE32:
      r = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಸೂಚ್ಯಂಕ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, resB->f_ಸೂಚ್ಯಂಕ, ಕೀಲಿ);
      ಆದರೆ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ಹಿಂತಿರುಗಿ ures_getStringWithAlias(resB, r, resB->f_ಸೂಚ್ಯಂಕ, len, ಸ್ಥಿತಿ_2);
    ಸ್ಥಿತಿ URES_ARRAY:
    ಸ್ಥಿತಿ URES_ARRAY16:
      r = res_get_Array_ಅಂಶ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, resB->f_ಸೂಚ್ಯಂಕ);
      ಆದರೆ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ಹಿಂತಿರುಗಿ ures_getStringWithAlias(resB, r, resB->f_ಸೂಚ್ಯಂಕ, len, ಸ್ಥಿತಿ_2);
    ಸ್ಥಿತಿ URES_ALIAS:
      ಹಿಂತಿರುಗಿ ures_getStringWithAlias(resB, resB->fRes, resB->f_ಸೂಚ್ಯಂಕ, len, ಸ್ಥಿತಿ_2);
    ಸ್ಥಿತಿ URES_INT:
    ಸ್ಥಿತಿ URES_ದ್ವಿಮಾನ:
    ಸ್ಥಿತಿ URES_INT_ಸದಿಶ:
        *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
  }

  ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_ಮುಂದಿನ_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_ಒಳ U_LIFETIME_BOUND,
                     UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ = ಶೂನ್ಯಸೂಚಕ;
    Resource r = RES_BOGUS;

    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            /*return nullptr;*/
            ಹಿಂತಿರುಗಿ fill_ಒಳ;
    }
    ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            /*return nullptr;*/
            ಹಿಂತಿರುಗಿ fill_ಒಳ;
    }

    ಆದರೆ(resB->f_ಸೂಚ್ಯಂಕ == resB->f_ಗಾತ್ರ-1) {
      *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } ಇಲ್ಲದಿದ್ದರೆ {
        resB->f_ಸೂಚ್ಯಂಕ++;
        ಆಯ್ಕೆ(RES_GET_TYPE(resB->fRes)) {
        ಸ್ಥಿತಿ URES_INT:
        ಸ್ಥಿತಿ URES_ದ್ವಿಮಾನ:
        ಸ್ಥಿತಿ URES_STRING:
        ಸ್ಥಿತಿ URES_STRING_V2:
        ಸ್ಥಿತಿ URES_INT_ಸದಿಶ:
            ಹಿಂತಿರುಗಿ ures_copyResb(fill_ಒಳ, resB, ಸ್ಥಿತಿ_2);
        ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ:
        ಸ್ಥಿತಿ URES_TABLE16:
        ಸ್ಥಿತಿ URES_TABLE32:
            r = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಸೂಚ್ಯಂಕ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, resB->f_ಸೂಚ್ಯಂಕ, &ಕೀಲಿ);
            ಆದರೆ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಹಿಂತಿರುಗಿ init_resb_ಫಲಿತಾಂಶ(resB->f_ದತ್ತಾಂಶ, r, ಕೀಲಿ, resB->f_ಸೂಚ್ಯಂಕ, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
        ಸ್ಥಿತಿ URES_ARRAY:
        ಸ್ಥಿತಿ URES_ARRAY16:
            r = res_get_Array_ಅಂಶ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, resB->f_ಸೂಚ್ಯಂಕ);
            ಆದರೆ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಹಿಂತಿರುಗಿ init_resb_ಫಲಿತಾಂಶ(resB->f_ದತ್ತಾಂಶ, r, ಕೀಲಿ, resB->f_ಸೂಚ್ಯಂಕ, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
        ಪೂರ್ವನಿಯೋಜಿತ:
            /*return nullptr;*/
            ಹಿಂತಿರುಗಿ fill_ಒಳ;
        }
    }
    /*return nullptr;*/
    ಹಿಂತಿರುಗಿ fill_ಒಳ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB,
                int32_t ಸೂಚ್ಯಂಕ_R,
                UResourceBundle* fill_ಒಳ U_LIFETIME_BOUND,
                UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ = ಶೂನ್ಯಸೂಚಕ;
    Resource r = RES_BOGUS;

    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        /*return nullptr;*/
        ಹಿಂತಿರುಗಿ fill_ಒಳ;
    }
    ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        /*return nullptr;*/
        ಹಿಂತಿರುಗಿ fill_ಒಳ;
    }

    ಆದರೆ(ಸೂಚ್ಯಂಕ_R >= 0 && resB->f_ಗಾತ್ರ > ಸೂಚ್ಯಂಕ_R) {
        ಆಯ್ಕೆ(RES_GET_TYPE(resB->fRes)) {
        ಸ್ಥಿತಿ URES_INT:
        ಸ್ಥಿತಿ URES_ದ್ವಿಮಾನ:
        ಸ್ಥಿತಿ URES_STRING:
        ಸ್ಥಿತಿ URES_STRING_V2:
        ಸ್ಥಿತಿ URES_INT_ಸದಿಶ:
            ಹಿಂತಿರುಗಿ ures_copyResb(fill_ಒಳ, resB, ಸ್ಥಿತಿ_2);
        ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ:
        ಸ್ಥಿತಿ URES_TABLE16:
        ಸ್ಥಿತಿ URES_TABLE32:
            r = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಸೂಚ್ಯಂಕ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, ಸೂಚ್ಯಂಕ_R, &ಕೀಲಿ);
            ಆದರೆ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಹಿಂತಿರುಗಿ init_resb_ಫಲಿತಾಂಶ(resB->f_ದತ್ತಾಂಶ, r, ಕೀಲಿ, ಸೂಚ್ಯಂಕ_R, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
        ಸ್ಥಿತಿ URES_ARRAY:
        ಸ್ಥಿತಿ URES_ARRAY16:
            r = res_get_Array_ಅಂಶ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, ಸೂಚ್ಯಂಕ_R);
            ಆದರೆ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಹಿಂತಿರುಗಿ init_resb_ಫಲಿತಾಂಶ(resB->f_ದತ್ತಾಂಶ, r, ಕೀಲಿ, ಸೂಚ್ಯಂಕ_R, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
        ಪೂರ್ವನಿಯೋಜಿತ:
            /*return nullptr;*/
            ಹಿಂತಿರುಗಿ fill_ಒಳ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    ಹಿಂತಿರುಗಿ fill_ಒಳ;
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
ures_get_String_By_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t ಸೂಚ್ಯಂಕ_S,
                      int32_t* len,
                      UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ = ಶೂನ್ಯಸೂಚಕ;
    Resource r = RES_BOGUS;

    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(ಸೂಚ್ಯಂಕ_S >= 0 && resB->f_ಗಾತ್ರ > ಸೂಚ್ಯಂಕ_S) {
        ಆಯ್ಕೆ(RES_GET_TYPE(resB->fRes)) {
        ಸ್ಥಿತಿ URES_STRING:
        ಸ್ಥಿತಿ URES_STRING_V2:
            ಹಿಂತಿರುಗಿ res_getString({resB}, &resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, len);
        ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ:
        ಸ್ಥಿತಿ URES_TABLE16:
        ಸ್ಥಿತಿ URES_TABLE32:
            r = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಸೂಚ್ಯಂಕ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, ಸೂಚ್ಯಂಕ_S, &ಕೀಲಿ);
            ಆದರೆ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಹಿಂತಿರುಗಿ ures_getStringWithAlias(resB, r, ಸೂಚ್ಯಂಕ_S, len, ಸ್ಥಿತಿ_2);
        ಸ್ಥಿತಿ URES_ARRAY:
        ಸ್ಥಿತಿ URES_ARRAY16:
            r = res_get_Array_ಅಂಶ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, ಸೂಚ್ಯಂಕ_S);
            ಆದರೆ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಹಿಂತಿರುಗಿ ures_getStringWithAlias(resB, r, ಸೂಚ್ಯಂಕ_S, len, ಸ್ಥಿತಿ_2);
        ಸ್ಥಿತಿ URES_ALIAS:
            ಹಿಂತಿರುಗಿ ures_getStringWithAlias(resB, resB->fRes, ಸೂಚ್ಯಂಕ_S, len, ಸ್ಥಿತಿ_2);
        ಸ್ಥಿತಿ URES_INT:
        ಸ್ಥಿತಿ URES_ದ್ವಿಮಾನ:
        ಸ್ಥಿತಿ URES_INT_ಸದಿಶ:
            *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
          /* must not occur */
          *ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
          ನಿಲ್ಲಿಸು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_get_UTF8_String_By_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB,
                          int32_t idx,
                          ಅಕ್ಷರ* dest U_LIFETIME_BOUND,
                          int32_t* p_ಉದ್ದಳತೆ,
                          UBool forceCopy,
                          UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    int32_t length16;
    ಸ್ಥಿರಾಂಕ char16_t *s16 = ures_get_String_By_ಸೂಚ್ಯಂಕ(resB, idx, &length16, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ ures_ಗಮ್ಯ_UTF8_String(s16, length16, dest, p_ಉದ್ದಳತೆ, forceCopy, ಸ್ಥಿತಿ_2);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_ಕಂಡುಹಿಡಿ_Resource(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, UResourceBundle* fill_ಒಳ U_LIFETIME_BOUND, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
  UResourceBundle *first = ಶೂನ್ಯಸೂಚಕ; 
  UResourceBundle *ಫಲಿತಾಂಶ = fill_ಒಳ;
  ಅಕ್ಷರ *packageName = ಶೂನ್ಯಸೂಚಕ;
  ಅಕ್ಷರ *ಮಾರ್ಗ_ಗಮ್ಯ_Resource = ಶೂನ್ಯಸೂಚಕ, *save = ಶೂನ್ಯಸೂಚಕ;
  ಅಕ್ಷರ *locale = ಶೂನ್ಯಸೂಚಕ, *locale_ಕೊನೆ = ಶೂನ್ಯಸೂಚಕ;
  int32_t length;

  ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  length = (int32_t)(uprv_strlen(ಮಾರ್ಗ)+1);
  save = ಮಾರ್ಗ_ಗಮ್ಯ_Resource = (ಅಕ್ಷರ *)uprv_malloc(length*ಗಾತ್ರ(ಅಕ್ಷರ));
  /* test for nullptr */
  ಆದರೆ(ಮಾರ್ಗ_ಗಮ್ಯ_Resource == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  uprv_memcpy(ಮಾರ್ಗ_ಗಮ್ಯ_Resource, ಮಾರ್ಗ, length);

  locale = ಮಾರ್ಗ_ಗಮ್ಯ_Resource;
  ಆದರೆ(*ಮಾರ್ಗ_ಗಮ್ಯ_Resource == RES_ಮಾರ್ಗ_SEPARATOR) { /* there is a path specification */
    ಮಾರ್ಗ_ಗಮ್ಯ_Resource++;
    packageName = ಮಾರ್ಗ_ಗಮ್ಯ_Resource;
    ಮಾರ್ಗ_ಗಮ್ಯ_Resource = uprv_strchr(ಮಾರ್ಗ_ಗಮ್ಯ_Resource, RES_ಮಾರ್ಗ_SEPARATOR);
    ಆದರೆ(ಮಾರ್ಗ_ಗಮ್ಯ_Resource == ಶೂನ್ಯಸೂಚಕ) {
      *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
      *ಮಾರ್ಗ_ಗಮ್ಯ_Resource = 0;
      locale = ಮಾರ್ಗ_ಗಮ್ಯ_Resource+1;
    }
  }

  locale_ಕೊನೆ = uprv_strchr(locale, RES_ಮಾರ್ಗ_SEPARATOR);
  ಆದರೆ(locale_ಕೊನೆ != ಶೂನ್ಯಸೂಚಕ) {
    *locale_ಕೊನೆ = 0;
  }

  first = ures_open(packageName, locale, ಸ್ಥಿತಿ_2);

  ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
    ಆದರೆ(locale_ಕೊನೆ) {
      ಫಲಿತಾಂಶ = ures_ಕಂಡುಹಿಡಿ_Sub_Resource(first, locale_ಕೊನೆ+1, fill_ಒಳ, ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = ures_copyResb(fill_ಒಳ, first, ಸ್ಥಿತಿ_2);
    }
    ures_close(first);
  }
  uprv_free(save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_ಕಂಡುಹಿಡಿ_Sub_Resource(ಸ್ಥಿರಾಂಕ UResourceBundle* resB,
                     ಅಕ್ಷರ* ಮಾರ್ಗ,
                     UResourceBundle* fill_ಒಳ U_LIFETIME_BOUND,
                     UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
  Resource res = RES_BOGUS;
  UResourceBundle *ಫಲಿತಾಂಶ = fill_ಒಳ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ;

  ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  ಮಾಡು {
    res = res_ಕಂಡುಹಿಡಿ_Resource(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, &ಮಾರ್ಗ, &ಕೀಲಿ); 
    ಆದರೆ(res != RES_BOGUS) {
        ಫಲಿತಾಂಶ = init_resb_ಫಲಿತಾಂಶ(resB->f_ದತ್ತಾಂಶ, res, ಕೀಲಿ, -1, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
        resB = ಫಲಿತಾಂಶ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        ನಿಲ್ಲಿಸು;
    }
  } ಆಗ(*ಮಾರ್ಗ); /* there is more stuff in the path */

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
ures_get_String_By_ಕೀಲಿ_With_Fallback(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಒಳ_ಕೀಲಿ,
                                int32_t* len,
                                UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    UResourceBundle ರಾಶಿ_5;
    ಸ್ಥಿರಾಂಕ char16_t* retVal = ಶೂನ್ಯಸೂಚಕ;
    ures_init_ರಾಶಿ_Object(&ರಾಶಿ_5);
    ures_get_By_ಕೀಲಿ_With_Fallback(resB, ಒಳ_ಕೀಲಿ, &ರಾಶಿ_5, ಸ್ಥಿತಿ_2);
    int32_t length;
    retVal = ures_getString(&ರಾಶಿ_5, &length, ಸ್ಥಿತಿ_2);
    ures_close(&ರಾಶಿ_5);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (length == 3 && retVal[0] == ಖಾಲಿಗಳು_ಗಣ && retVal[1] == ಖಾಲಿಗಳು_ಗಣ && retVal[2] == ಖಾಲಿಗಳು_ಗಣ ) {
        retVal = ಶೂನ್ಯಸೂಚಕ;
        length = 0;
        *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
    }
    ಆದರೆ (len != ಶೂನ್ಯಸೂಚಕ) {
        *len = length;
    }
    ಹಿಂತಿರುಗಿ retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ಸ್ಥಿರ Resource get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource ಕೋಷ್ಟಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ) {
  Resource resource = ಕೋಷ್ಟಕ;  /* The current resource */
  icu::CharString ಮಾರ್ಗ;
  UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
  ಮಾರ್ಗ.append(ಕೀಲಿ, error_ಸಂಕೇತ);
  ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ RES_BOGUS; }
  ಅಕ್ಷರ *ಮಾರ್ಗ_Part = ಮಾರ್ಗ.ದತ್ತಾಂಶ();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  ಆಗ (*ಮಾರ್ಗ_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    ಅಕ್ಷರ *ಮುಂದಿನ_ಮಾರ್ಗ_Part = uprv_strchr(ಮಾರ್ಗ_Part, RES_ಮಾರ್ಗ_SEPARATOR);
    ಆದರೆ (ಮುಂದಿನ_ಮಾರ್ಗ_Part != ಶೂನ್ಯಸೂಚಕ) {
      *ಮುಂದಿನ_ಮಾರ್ಗ_Part = 0;  /* Terminating null for this part of path. */
      ಮುಂದಿನ_ಮಾರ್ಗ_Part++;
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಮುಂದಿನ_ಮಾರ್ಗ_Part = uprv_strchr(ಮಾರ್ಗ_Part, 0);
    }
    int32_t t;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ_P = ಮಾರ್ಗ_Part;
    resource = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(p_Res_ದತ್ತಾಂಶ, resource, &t, &ಮಾರ್ಗ_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    ಮಾರ್ಗ_Part = ಮುಂದಿನ_ಮಾರ್ಗ_Part; 
  }
  ಆದರೆ (*ಮಾರ್ಗ_Part) {
    ಹಿಂತಿರುಗಿ RES_BOGUS;
  }
  ಹಿಂತಿರುಗಿ resource;
}

ಸ್ಥಿರ ಶೂನ್ಯ create_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* orig_Res_ಮಾರ್ಗ,
                       int32_t     orig_Res_ಮಾರ್ಗ_Len,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* res_ಮಾರ್ಗ,
                       int32_t     res_ಮಾರ್ಗ_Len,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಒಳ_ಕೀಲಿ,
                       CharString& ಮಾರ್ಗ,
                       UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
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
    ಮಾರ್ಗ.clear();
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ = ಒಳ_ಕೀಲಿ;
    ಆದರೆ (res_ಮಾರ್ಗ_Len > 0) {
        ಮಾರ್ಗ.append(res_ಮಾರ್ಗ, res_ಮಾರ್ಗ_Len, *ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* res_ಮಾರ್ಗ_ಎಲ್ಲೆ = res_ಮಾರ್ಗ + res_ಮಾರ್ಗ_Len;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* orig_Res_ಮಾರ್ಗ_ಎಲ್ಲೆ = orig_Res_ಮಾರ್ಗ + orig_Res_ಮಾರ್ಗ_Len;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* res_ಮಾರ್ಗ_Ptr = res_ಮಾರ್ಗ;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* orig_Res_ಮಾರ್ಗ_Ptr = orig_Res_ಮಾರ್ಗ;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            ಆಗ (orig_Res_ಮಾರ್ಗ_Ptr < orig_Res_ಮಾರ್ಗ_ಎಲ್ಲೆ && res_ಮಾರ್ಗ_Ptr < res_ಮಾರ್ಗ_ಎಲ್ಲೆ) {
                ಆಗ (orig_Res_ಮಾರ್ಗ_Ptr < orig_Res_ಮಾರ್ಗ_ಎಲ್ಲೆ && *orig_Res_ಮಾರ್ಗ_Ptr != RES_ಮಾರ್ಗ_SEPARATOR) {
                    ++orig_Res_ಮಾರ್ಗ_Ptr;
                }
                ಆದರೆ (orig_Res_ಮಾರ್ಗ_Ptr < orig_Res_ಮಾರ್ಗ_ಎಲ್ಲೆ && *orig_Res_ಮಾರ್ಗ_Ptr == RES_ಮಾರ್ಗ_SEPARATOR) {
                    ++orig_Res_ಮಾರ್ಗ_Ptr;
                }
                ಆಗ (res_ಮಾರ್ಗ_Ptr < res_ಮಾರ್ಗ_ಎಲ್ಲೆ && *res_ಮಾರ್ಗ_Ptr != RES_ಮಾರ್ಗ_SEPARATOR) {
                    ++res_ಮಾರ್ಗ_Ptr;
                }
                ಆದರೆ (res_ಮಾರ್ಗ_Ptr < res_ಮಾರ್ಗ_ಎಲ್ಲೆ && *res_ಮಾರ್ಗ_Ptr == RES_ಮಾರ್ಗ_SEPARATOR) {
                    ++res_ಮಾರ್ಗ_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            ಆಗ (res_ಮಾರ್ಗ_Ptr < res_ಮಾರ್ಗ_ಎಲ್ಲೆ && *ಕೀಲಿ != '\0') {
                ಆಗ (res_ಮಾರ್ಗ_Ptr < res_ಮಾರ್ಗ_ಎಲ್ಲೆ && *res_ಮಾರ್ಗ_Ptr != RES_ಮಾರ್ಗ_SEPARATOR) {
                    ++res_ಮಾರ್ಗ_Ptr;
                }
                ಆದರೆ (res_ಮಾರ್ಗ_Ptr < res_ಮಾರ್ಗ_ಎಲ್ಲೆ && *res_ಮಾರ್ಗ_Ptr == RES_ಮಾರ್ಗ_SEPARATOR) {
                    ++res_ಮಾರ್ಗ_Ptr;
                }
                ಆಗ (*ಕೀಲಿ != '\0' && *ಕೀಲಿ != RES_ಮಾರ್ಗ_SEPARATOR) {
                    ++ಕೀಲಿ;
                }
                ಆದರೆ (*ಕೀಲಿ == RES_ಮಾರ್ಗ_SEPARATOR) {
                    ++ಕೀಲಿ;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        ಮಾರ್ಗ.append(ಕೀಲಿ, *ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಮಾರ್ಗ.append(ಒಳ_ಕೀಲಿ, *ಸ್ಥಿತಿ_2);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ಕೀಲಿ_With_Fallback(ಸ್ಥಿರಾಂಕ UResourceBundle* resB,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಒಳ_ಕೀಲಿ,
                          UResourceBundle* fill_ಒಳ U_LIFETIME_BOUND,
                          UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    Resource res = RES_BOGUS, ಬೇರು_Res = RES_BOGUS;
    UResourceBundle *helper = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ fill_ಒಳ;
    }
    ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ fill_ಒಳ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ಆದರೆ(URES_IS_ಕೋಷ್ಟಕ(type)) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* orig_Res_ಮಾರ್ಗ = resB->f_Res_ಮಾರ್ಗ;
        int32_t orig_Res_ಮಾರ್ಗ_Len = resB->f_Res_ಮಾರ್ಗ_Len;
        res = get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ_ಮಾರ್ಗ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, ಒಳ_ಕೀಲಿ);
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ = ಒಳ_ಕೀಲಿ;
        ತಾರ್ಕಿಕ did_ಬೇರು_Once = ಸುಳ್ಳು;
        ಆದರೆ(res == RES_BOGUS) {
            UResource_ದತ್ತಾಂಶ_ನಮೂದು *ದತ್ತಾಂಶ_ನಮೂದು = resB->f_ದತ್ತಾಂಶ;
            CharString ಮಾರ್ಗ;
            ಅಕ್ಷರ *my_ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* res_ಮಾರ್ಗ = resB->f_Res_ಮಾರ್ಗ;
            int32_t len = resB->f_Res_ಮಾರ್ಗ_Len;
            ಆಗ(res == RES_BOGUS && (ದತ್ತಾಂಶ_ನಮೂದು->f_ಪೋಷಕ != ಶೂನ್ಯಸೂಚಕ || !did_ಬೇರು_Once)) { /* Otherwise, we'll look in parents */
                ಆದರೆ (ದತ್ತಾಂಶ_ನಮೂದು->f_ಪೋಷಕ != ಶೂನ್ಯಸೂಚಕ) {
                    ದತ್ತಾಂಶ_ನಮೂದು = ದತ್ತಾಂಶ_ನಮೂದು->f_ಪೋಷಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_ಬೇರು_Once = ನಿಜ;
                }
                ಬೇರು_Res = ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ.ಬೇರು_Res;

                ಆದರೆ(ದತ್ತಾಂಶ_ನಮೂದು->fBogus == U_ZERO_ERROR) {
                    create_ಮಾರ್ಗ(orig_Res_ಮಾರ್ಗ, orig_Res_ಮಾರ್ಗ_Len, res_ಮಾರ್ಗ, len, ಒಳ_ಕೀಲಿ, ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
                    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                        ures_close(helper);
                        ಹಿಂತಿರುಗಿ fill_ಒಳ;
                    }
                    my_ಮಾರ್ಗ = ಮಾರ್ಗ.ದತ್ತಾಂಶ();
                    ಕೀಲಿ = ಒಳ_ಕೀಲಿ;
                    ಮಾಡು {
                        res = res_ಕಂಡುಹಿಡಿ_Resource(&(ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ), ಬೇರು_Res, &my_ಮಾರ್ಗ, &ಕೀಲಿ);
                        ಆದರೆ (RES_GET_TYPE(res) == URES_ALIAS && *my_ಮಾರ್ಗ) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ_ನಮೂದು, res, ಶೂನ್ಯಸೂಚಕ, -1, resB, helper, ಸ್ಥಿತಿ_2);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            ಆದರೆ(helper) {
                              ದತ್ತಾಂಶ_ನಮೂದು = helper->f_ದತ್ತಾಂಶ;
                              ಬೇರು_Res = helper->fRes;
                              res_ಮಾರ್ಗ = helper->f_Res_ಮಾರ್ಗ;
                              len = helper->f_Res_ಮಾರ್ಗ_Len;

                            } ಇಲ್ಲದಿದ್ದರೆ {
                              ನಿಲ್ಲಿಸು;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (res == RES_BOGUS) {
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಆಗ(*my_ಮಾರ್ಗ); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            ಆದರೆ(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                ಆದರೆ(uprv_strcmp(ದತ್ತಾಂಶ_ನಮೂದು->fName, uloc_getDefault())==0 || uprv_strcmp(ದತ್ತಾಂಶ_ನಮೂದು->fName, k_ಬೇರು_Locale_Name)==0) {
                    *ಸ್ಥಿತಿ_2 = U_USING_DEFAULT_WARNING;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    *ಸ್ಥಿತಿ_2 = U_USING_FALLBACK_WARNING;
                }

                fill_ಒಳ = init_resb_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ_ನಮೂದು, res, ಕೀಲಿ, -1, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
                ಆದರೆ (res_ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ) {
                    create_ಮಾರ್ಗ(orig_Res_ಮಾರ್ಗ, orig_Res_ಮಾರ್ಗ_Len, res_ಮಾರ್ಗ, len, ಒಳ_ಕೀಲಿ, ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* separator = ಶೂನ್ಯಸೂಚಕ;
                    ಆದರೆ (fill_ಒಳ->f_Res_ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ) {
                        separator = uprv_strchr(fill_ಒಳ->f_Res_ಮಾರ್ಗ, RES_ಮಾರ್ಗ_SEPARATOR);
                    }
                    ಆದರೆ (separator != ಶೂನ್ಯಸೂಚಕ && separator[1] != '\0') {
                        create_ಮಾರ್ಗ(orig_Res_ಮಾರ್ಗ, orig_Res_ಮಾರ್ಗ_Len, fill_ಒಳ->f_Res_ಮಾರ್ಗ,
                                   static_cast<int32_t>(uprv_strlen(fill_ಒಳ->f_Res_ಮಾರ್ಗ)), ಒಳ_ಕೀಲಿ, ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        create_ಮಾರ್ಗ(orig_Res_ಮಾರ್ಗ, orig_Res_ಮಾರ್ಗ_Len, "", 0, ಒಳ_ಕೀಲಿ, ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
                    }
                }
                ures_free_Res_ಮಾರ್ಗ(fill_ಒಳ);
                ures_append_Res_ಮಾರ್ಗ(fill_ಒಳ, ಮಾರ್ಗ.ದತ್ತಾಂಶ(), ಮಾರ್ಗ.length(), ಸ್ಥಿತಿ_2);
                ಆದರೆ(fill_ಒಳ->f_Res_ಮಾರ್ಗ[fill_ಒಳ->f_Res_ಮಾರ್ಗ_Len-1] != RES_ಮಾರ್ಗ_SEPARATOR) {
                    ures_append_Res_ಮಾರ್ಗ(fill_ಒಳ, RES_ಮಾರ್ಗ_SEPARATOR_S, 1, ಸ್ಥಿತಿ_2);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            fill_ಒಳ = init_resb_ಫಲಿತಾಂಶ(resB->f_ದತ್ತಾಂಶ, res, ಕೀಲಿ, -1, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
        }
    } 
    ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    ಹಿಂತಿರುಗಿ fill_ಒಳ;
}

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ get_All_ವಸ್ತುಗಳು_With_Fallback(
        ಸ್ಥಿರಾಂಕ UResourceBundle *bundle, Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ &value,
        ResourceSink &sink, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
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
    value.ಗಣ_ದತ್ತಾಂಶ(bundle->get_Res_ದತ್ತಾಂಶ());
    value.ಗಣ_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು(bundle->f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು);
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *ಪೋಷಕ_ನಮೂದು = bundle->f_ದತ್ತಾಂಶ->f_ಪೋಷಕ;
    UBool has_ಪೋಷಕ = ಪೋಷಕ_ನಮೂದು != ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(ಪೋಷಕ_ನಮೂದು->fBogus);
    value.ಗಣ_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_ಕೀಲಿ, value, !has_ಪೋಷಕ, error_ಸಂಕೇತ);
    ಆದರೆ (has_ಪೋಷಕ) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ರಾಶಿ_UResource_Bundle ಪೋಷಕ_Bundle;
        UResourceBundle &ಪೋಷಕ_Ref = ಪೋಷಕ_Bundle.ref();
        ಪೋಷಕ_Ref.f_ದತ್ತಾಂಶ = ಪೋಷಕ_ನಮೂದು;
        ಪೋಷಕ_Ref.f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು = bundle->f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು;
        ಪೋಷಕ_Ref.fHasFallback = !ಪೋಷಕ_Ref.get_Res_ದತ್ತಾಂಶ().noFallback;
        ಪೋಷಕ_Ref.f_Is_ತುದಿ_Level = ನಿಜ;
        ಪೋಷಕ_Ref.fRes = ಪೋಷಕ_Ref.get_Res_ದತ್ತಾಂಶ().ಬೇರು_Res;
        ಪೋಷಕ_Ref.f_ಗಾತ್ರ = res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(&ಪೋಷಕ_Ref.get_Res_ದತ್ತಾಂಶ(), ಪೋಷಕ_Ref.fRes);
        ಪೋಷಕ_Ref.f_ಸೂಚ್ಯಂಕ = -1;
        ನಮೂದು_Increase(ಪೋಷಕ_ನಮೂದು);

        // Look up the container item in the parent bundle.
        ರಾಶಿ_UResource_Bundle containerBundle;
        ಸ್ಥಿರಾಂಕ UResourceBundle *rb;
        UError_ಸಂಕೇತ ಮಾರ್ಗ_Error_ಸಂಕೇತ = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        ಆದರೆ (bundle->f_Res_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ || *bundle->f_Res_ಮಾರ್ಗ == 0) {
            rb = ಪೋಷಕ_Bundle.getAlias();
        } ಇಲ್ಲದಿದ್ದರೆ {
            rb = ures_get_By_ಕೀಲಿ_With_Fallback(ಪೋಷಕ_Bundle.getAlias(), bundle->f_Res_ಮಾರ್ಗ,
                                           containerBundle.getAlias(), &ಮಾರ್ಗ_Error_ಸಂಕೇತ);
        }
        ಆದರೆ (U_SUCCESS(ಮಾರ್ಗ_Error_ಸಂಕೇತ)) {
            get_All_ವಸ್ತುಗಳು_With_Fallback(rb, value, sink, error_ಸಂಕೇತ);
        }
    }
}

ರಚನೆ GetAllChildrenSink : ಸಾರ್ವಜನಿಕ ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    ವಾಸ್ತವಿಕ ~GetAllChildrenSink() override;
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ put(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value, UBool is_ಬೇರು,
           UError_ಸಂಕೇತ &error_ಸಂಕೇತ) override {
        Resource_ಕೋಷ್ಟಕ ವಸ್ತುಗಳು_ಕೋಷ್ಟಕ = value.get_ಕೋಷ್ಟಕ(error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        ಗಾಗಿ (int32_t i = 0; ವಸ್ತುಗಳು_ಕೋಷ್ಟಕ.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(i, ಕೀಲಿ, value); ++i) {
            ಆದರೆ (value.getType() == URES_ALIAS) {
                Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ& rdv = static_cast<Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ&>(value);
                ರಾಶಿ_UResource_Bundle ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle;
                UResourceBundle* aliasRB = get_Alias_ಗುರಿ_As_Resource_Bundle(rdv.get_ದತ್ತಾಂಶ(), rdv.getResource(), ಶೂನ್ಯಸೂಚಕ, -1,
                                                                          rdv.get_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು(), ಶೂನ್ಯಸೂಚಕ, 0,
                                                                          ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle.getAlias(), &error_ಸಂಕೇತ);
                ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
                    Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ aliased_ಮೌಲ್ಯ;
                    aliased_ಮೌಲ್ಯ.ಗಣ_ದತ್ತಾಂಶ(aliasRB->get_Res_ದತ್ತಾಂಶ());
                    aliased_ಮೌಲ್ಯ.ಗಣ_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು(aliasRB->f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು);
                    aliased_ಮೌಲ್ಯ.ಗಣ_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    ಆದರೆ (aliased_ಮೌಲ್ಯ.getType() != URES_ಕೋಷ್ಟಕ) {
                        dest.put(ಕೀಲಿ, aliased_ಮೌಲ್ಯ, is_ಬೇರು, error_ಸಂಕೇತ);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_ಮೌಲ್ಯ_Type = URES_ಕೋಷ್ಟಕ;
                        CharString ಕೋಷ್ಟಕ_ಮಾರ್ಗ;
                        ಕೋಷ್ಟಕ_ಮಾರ್ಗ.append(aliasRB->f_Res_ಮಾರ್ಗ, error_ಸಂಕೇತ);
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಪೋಷಕ_ಕೀಲಿ = ಕೀಲಿ; // dest.put() changes the key
                        dest.put(ಪೋಷಕ_ಕೀಲಿ, aliased_ಮೌಲ್ಯ, is_ಬೇರು, error_ಸಂಕೇತ);
                        UResource_ದತ್ತಾಂಶ_ನಮೂದು* ನಮೂದು = aliasRB->f_ದತ್ತಾಂಶ;
                        Resource res = aliasRB->fRes;
                        ಆಗ (aliased_ಮೌಲ್ಯ_Type == URES_ಕೋಷ್ಟಕ && ನಮೂದು->f_ಪೋಷಕ != ಶೂನ್ಯಸೂಚಕ) {
                            CharString local_ಮಾರ್ಗ;
                            local_ಮಾರ್ಗ.copyFrom(ಕೋಷ್ಟಕ_ಮಾರ್ಗ, error_ಸಂಕೇತ);
                            ಅಕ್ಷರ* local_ಮಾರ್ಗ_As_Char_Ptr = local_ಮಾರ್ಗ.ದತ್ತಾಂಶ();
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಗು_ಕೀಲಿ;
                            ನಮೂದು = ನಮೂದು->f_ಪೋಷಕ;
                            res = ನಮೂದು->f_ದತ್ತಾಂಶ.ಬೇರು_Res;
                            Resource newRes = res_ಕಂಡುಹಿಡಿ_Resource(&ನಮೂದು->f_ದತ್ತಾಂಶ, res, &local_ಮಾರ್ಗ_As_Char_Ptr, &ಮಗು_ಕೀಲಿ);
                            ಆದರೆ (newRes != RES_BOGUS) {
                                aliased_ಮೌಲ್ಯ.ಗಣ_ದತ್ತಾಂಶ(ನಮೂದು->f_ದತ್ತಾಂಶ);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_ಮೌಲ್ಯ.ಗಣ_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_ಮೌಲ್ಯ_Type = aliased_ಮೌಲ್ಯ.getType();
                                ಆದರೆ (aliased_ಮೌಲ್ಯ_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ& rdv2 = static_cast<Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ&>(aliased_ಮೌಲ್ಯ);
                                    aliasRB = get_Alias_ಗುರಿ_As_Resource_Bundle(rdv2.get_ದತ್ತಾಂಶ(), rdv2.getResource(), ಶೂನ್ಯಸೂಚಕ, -1,
                                                                             rdv2.get_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು(), ಶೂನ್ಯಸೂಚಕ, 0,
                                                                             ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle.getAlias(), &error_ಸಂಕೇತ);
                                    ಕೋಷ್ಟಕ_ಮಾರ್ಗ.clear();
                                    ಕೋಷ್ಟಕ_ಮಾರ್ಗ.append(aliasRB->f_Res_ಮಾರ್ಗ, error_ಸಂಕೇತ);
                                    ನಮೂದು = aliasRB->f_ದತ್ತಾಂಶ;
                                    res = aliasRB->fRes;
                                    aliased_ಮೌಲ್ಯ.ಗಣ_ದತ್ತಾಂಶ(ನಮೂದು->f_ದತ್ತಾಂಶ);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_ಮೌಲ್ಯ.ಗಣ_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_ಮೌಲ್ಯ_Type = aliased_ಮೌಲ್ಯ.getType();
                                }
                                ಆದರೆ (aliased_ಮೌಲ್ಯ_Type == URES_ಕೋಷ್ಟಕ) {
                                    dest.put(ಪೋಷಕ_ಕೀಲಿ, aliased_ಮೌಲ್ಯ, is_ಬೇರು, error_ಸಂಕೇತ);
                                } ಇಲ್ಲದಿದ್ದರೆ {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_ಸಂಕೇತ = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
                                    ಹಿಂತಿರುಗಿ;
                                }
                            }
                        }
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                dest.put(ಕೀಲಿ, value, is_ಬೇರು, error_ಸಂಕೇತ);
            }
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_getAllChildrenWithFallback(ಸ್ಥಿರಾಂಕ UResourceBundle *bundle, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                                icu::ResourceSink &sink, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_ವಸ್ತುಗಳು_With_Fallback(bundle, ಮಾರ್ಗ, allChildrenSink, error_ಸಂಕೇತ);
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
U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_get_ಮೌಲ್ಯ_With_Fallback(ಸ್ಥಿರಾಂಕ UResourceBundle *bundle, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                          UResourceBundle *ತಾತ್ಕಾಲಿಕ_Fill_ಒಳ,
                          Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ &value, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ (ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ UResourceBundle *rb;
    ಆದರೆ (*ಮಾರ್ಗ == 0) {
        // empty path
        rb = bundle;
    } ಇಲ್ಲದಿದ್ದರೆ {
        rb = ures_get_By_ಕೀಲಿ_With_Fallback(bundle, ಮಾರ್ಗ, ತಾತ್ಕಾಲಿಕ_Fill_ಒಳ, &error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
    }
    value.ಗಣ_ದತ್ತಾಂಶ(rb->get_Res_ದತ್ತಾಂಶ());
    value.ಗಣ_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು(rb->f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು);
    value.ಗಣ_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_get_All_ವಸ್ತುಗಳು_With_Fallback(ಸ್ಥಿರಾಂಕ UResourceBundle *bundle, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                             icu::ResourceSink &sink, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ (ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ರಾಶಿ_UResource_Bundle ರಾಶಿ_Bundle;
    ಸ್ಥಿರಾಂಕ UResourceBundle *rb;
    ಆದರೆ (*ಮಾರ್ಗ == 0) {
        // empty path
        rb = bundle;
    } ಇಲ್ಲದಿದ್ದರೆ {
        rb = ures_get_By_ಕೀಲಿ_With_Fallback(bundle, ಮಾರ್ಗ, ರಾಶಿ_Bundle.getAlias(), &error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
    }
    // Get all table items with fallback.
    Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ value;
    get_All_ವಸ್ತುಗಳು_With_Fallback(rb, value, sink, error_ಸಂಕೇತ);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB,
              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಒಳ_ಕೀಲಿ,
              UResourceBundle* fill_ಒಳ U_LIFETIME_BOUND,
              UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    Resource res = RES_BOGUS;
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *ದತ್ತಾಂಶ_ನಮೂದು = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ = ಒಳ_ಕೀಲಿ;

    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ fill_ಒಳ;
    }
    ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ fill_ಒಳ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ಆದರೆ(URES_IS_ಕೋಷ್ಟಕ(type)) {
        int32_t t;
        res = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, &t, &ಕೀಲಿ);
        ಆದರೆ(res == RES_BOGUS) {
            ಕೀಲಿ = ಒಳ_ಕೀಲಿ;
            ಆದರೆ(resB->fHasFallback) {
                ದತ್ತಾಂಶ_ನಮೂದು = get_Fallback_ದತ್ತಾಂಶ(resB, &ಕೀಲಿ, &res, ಸ್ಥಿತಿ_2);
                ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
                    /* check if resB->fResPath gives the right name here */
                    ಹಿಂತಿರುಗಿ init_resb_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ_ನಮೂದು, res, ಕೀಲಿ, -1, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ init_resb_ಫಲಿತಾಂಶ(resB->f_ದತ್ತಾಂಶ, res, ಕೀಲಿ, -1, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
        }
    } 
#ಆದರೆ 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ನಿಜ) {
        /* here should go a first attempt to locate the key using index table */
        ದತ್ತಾಂಶ_ನಮೂದು = get_Fallback_ದತ್ತಾಂಶ(resB, &ಕೀಲಿ, &res, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ init_resb_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ_ನಮೂದು, res, ಕೀಲಿ, resB, fill_ಒಳ, ಸ್ಥಿತಿ_2);
        } ಇಲ್ಲದಿದ್ದರೆ {
            *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        }
    }
#ಆದರೆ_ಅಂತ್ಯ    
    ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ fill_ಒಳ;
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
ures_get_String_By_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಒಳ_ಕೀಲಿ,
                    int32_t* len,
                    UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    Resource res = RES_BOGUS;
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *ದತ್ತಾಂಶ_ನಮೂದು = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ = ಒಳ_ಕೀಲಿ;

    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ಆದರೆ(URES_IS_ಕೋಷ್ಟಕ(type)) {
        int32_t t=0;

        res = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(&resB->get_Res_ದತ್ತಾಂಶ(), resB->fRes, &t, &ಕೀಲಿ);

        ಆದರೆ(res == RES_BOGUS) {
            ಕೀಲಿ = ಒಳ_ಕೀಲಿ;
            ಆದರೆ(resB->fHasFallback) {
                ದತ್ತಾಂಶ_ನಮೂದು = get_Fallback_ದತ್ತಾಂಶ(resB, &ಕೀಲಿ, &res, ಸ್ಥಿತಿ_2);
                ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
                    ಆಯ್ಕೆ (RES_GET_TYPE(res)) {
                    ಸ್ಥಿತಿ URES_STRING:
                    ಸ್ಥಿತಿ URES_STRING_V2:
                        ಹಿಂತಿರುಗಿ res_getString({resB, ಕೀಲಿ}, &ದತ್ತಾಂಶ_ನಮೂದು->f_ದತ್ತಾಂಶ, res, len);
                    ಸ್ಥಿತಿ URES_ALIAS:
                      {
                        ಸ್ಥಿರಾಂಕ char16_t* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
                        UResourceBundle *ತಾತ್ಕಾಲಿಕ_Res = ures_get_By_ಕೀಲಿ(resB, ಒಳ_ಕೀಲಿ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
                        ಫಲಿತಾಂಶ = ures_getString(ತಾತ್ಕಾಲಿಕ_Res, len, ಸ್ಥಿತಿ_2);
                        ures_close(ತಾತ್ಕಾಲಿಕ_Res);
                        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
                      }
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆಯ್ಕೆ (RES_GET_TYPE(res)) {
            ಸ್ಥಿತಿ URES_STRING:
            ಸ್ಥಿತಿ URES_STRING_V2:
                ಹಿಂತಿರುಗಿ res_getString({resB, ಕೀಲಿ}, &resB->get_Res_ದತ್ತಾಂಶ(), res, len);
            ಸ್ಥಿತಿ URES_ALIAS:
              {
                ಸ್ಥಿರಾಂಕ char16_t* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
                UResourceBundle *ತಾತ್ಕಾಲಿಕ_Res = ures_get_By_ಕೀಲಿ(resB, ಒಳ_ಕೀಲಿ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
                ಫಲಿತಾಂಶ = ures_getString(ತಾತ್ಕಾಲಿಕ_Res, len, ಸ್ಥಿತಿ_2);
                ures_close(ತಾತ್ಕಾಲಿಕ_Res);
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
              }
            ಪೂರ್ವನಿಯೋಜಿತ:
                *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#ಆದರೆ 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ನಿಜ) {
        /* here should go a first attempt to locate the key using index table */
        ದತ್ತಾಂಶ_ನಮೂದು = get_Fallback_ದತ್ತಾಂಶ(resB, &ಕೀಲಿ, &res, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
            // TODO: Tracing
            ಹಿಂತಿರುಗಿ res_getString(rd, res, len);
        } ಇಲ್ಲದಿದ್ದರೆ {
            *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        }
    } 
#ಆದರೆ_ಅಂತ್ಯ    
    ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_get_UTF8_String_By_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                        ಅಕ್ಷರ* dest U_LIFETIME_BOUND,
                        int32_t* p_ಉದ್ದಳತೆ,
                        UBool forceCopy,
                        UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    int32_t length16;
    ಸ್ಥಿರಾಂಕ char16_t *s16 = ures_get_String_By_ಕೀಲಿ(resB, ಕೀಲಿ, &length16, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ ures_ಗಮ್ಯ_UTF8_String(s16, length16, dest, p_ಉದ್ದಳತೆ, forceCopy, ಸ್ಥಿತಿ_2);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_get_Locale_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (!resourceBundle) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಹಿಂತಿರುಗಿ resourceBundle->f_ದತ್ತಾಂಶ->fName;
    }
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_getLocale(ಸ್ಥಿರಾಂಕ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
  ಹಿಂತಿರುಗಿ ures_get_Locale_ಆಂತರಿಕ(resourceBundle, ಸ್ಥಿತಿ_2);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_getLocaleByType(ಸ್ಥಿರಾಂಕ UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_ದತ್ತಾಂಶ_Locale_Type type,
                     UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (!resourceBundle) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಯ್ಕೆ(type) {
        ಸ್ಥಿತಿ ULOC_ACTUAL_LOCALE:
            ಹಿಂತಿರುಗಿ resourceBundle->f_ದತ್ತಾಂಶ->fName;
        ಸ್ಥಿತಿ ULOC_VALID_LOCALE:
            ಹಿಂತಿರುಗಿ resourceBundle->f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು->fName;
        ಸ್ಥಿತಿ ULOC_REQUESTED_LOCALE:
        ಪೂರ್ವನಿಯೋಜಿತ:
            *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }
}

U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
ures_getName(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND) {
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  ಹಿಂತಿರುಗಿ resB->f_ದತ್ತಾಂಶ->fName;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ URES_DEBUG
U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
ures_get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB U_LIFETIME_BOUND) {
  ಆದರೆ(resB == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  ಹಿಂತಿರುಗಿ resB->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                  UResOpenType openType,
                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UResource_ದತ್ತಾಂಶ_ನಮೂದು *ನಮೂದು;
    ಆದರೆ(openType != URES_OPEN_ನೇರ) {
        ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_ತಳ_Name(localeID, *ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ನಮೂದು = ನಮೂದು_Open(ಮಾರ್ಗ, canonLocaleID.ದತ್ತಾಂಶ(), openType, ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ನಮೂದು = ನಮೂದು_Open_ನೇರ(ಮಾರ್ಗ, localeID, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(ನಮೂದು == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UBool is_ರಾಶಿ_Object;
    ಆದರೆ(r == ಶೂನ್ಯಸೂಚಕ) {
        r = static_cast<UResourceBundle*>(uprv_malloc(ಗಾತ್ರ(UResourceBundle)));
        ಆದರೆ(r == ಶೂನ್ಯಸೂಚಕ) {
            ನಮೂದು_Close(ನಮೂದು);
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        is_ರಾಶಿ_Object = ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {  // fill-in
        is_ರಾಶಿ_Object = ures_is_ರಾಶಿ_Object(r);
        ures_closeBundle(r, ಸುಳ್ಳು);
    }
    uprv_memset(r, 0, ಗಾತ್ರ(UResourceBundle));
    ures_ಗಣ_Is_ರಾಶಿ_Object(r, is_ರಾಶಿ_Object);

    r->f_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು = r->f_ದತ್ತಾಂಶ = ನಮೂದು;
    r->fHasFallback = openType != URES_OPEN_ನೇರ && !r->get_Res_ದತ್ತಾಂಶ().noFallback;
    r->f_Is_ತುದಿ_Level = ನಿಜ;
    r->fRes = r->get_Res_ದತ್ತಾಂಶ().ಬೇರು_Res;
    r->f_ಗಾತ್ರ = res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(&r->get_Res_ದತ್ತಾಂಶ(), r->fRes);
    r->f_ಸೂಚ್ಯಂಕ = -1;

    ResourceTracer(r).traceOpen();

    ಹಿಂತಿರುಗಿ r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ ures_openWithType(ಶೂನ್ಯಸೂಚಕ, ಮಾರ್ಗ, localeID, URES_OPEN_LOCALE_DEFAULT_ಬೇರು, ಸ್ಥಿತಿ_2);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ ures_openWithType(ಶೂನ್ಯಸೂಚಕ, ಮಾರ್ಗ, localeID, URES_OPEN_LOCALE_ಬೇರು, ಸ್ಥಿತಿ_2);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_ನೇರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ ures_openWithType(ಶೂನ್ಯಸೂಚಕ, ಮಾರ್ಗ, localeID, URES_OPEN_ನೇರ, ಸ್ಥಿತಿ_2);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_open_Fill_ಒಳ(UResourceBundle *r, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2) && r == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ures_openWithType(r, ಮಾರ್ಗ, localeID, URES_OPEN_LOCALE_DEFAULT_ಬೇರು, ಸ್ಥಿತಿ_2);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_open_ನೇರ_Fill_ಒಳ(UResourceBundle *r, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2) && r == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ures_openWithType(r, ಮಾರ್ಗ, localeID, URES_OPEN_ನೇರ, ಸ್ಥಿತಿ_2);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(ಸ್ಥಿರಾಂಕ UResourceBundle* resourceBundle,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* resource_ಕೀಲಿ,
                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    UResourceBundle res_ದತ್ತಾಂಶ;
    ures_init_ರಾಶಿ_Object(&res_ದತ್ತಾಂಶ);
    ಆದರೆ (ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(resourceBundle == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ures_get_By_ಕೀಲಿ(resourceBundle, resource_ಕೀಲಿ, &res_ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
    
    ಆದರೆ(res_ದತ್ತಾಂಶ.get_Res_ದತ್ತಾಂಶ().ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        int32_t ಫಲಿತಾಂಶ = res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(&res_ದತ್ತಾಂಶ.get_Res_ದತ್ತಾಂಶ(), res_ದತ್ತಾಂಶ.fRes);
        ures_close(&res_ದತ್ತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_ದತ್ತಾಂಶ);
        ಹಿಂತಿರುಗಿ 0;
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
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_get_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ಆದರೆ (!resourceBundle) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ(resourceBundle->f_ಆವೃತ್ತಿ == ಶೂನ್ಯಸೂಚಕ) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ಸ್ಥಿರಾಂಕ char16_t* minor_ಆವೃತ್ತಿ = ures_get_String_By_ಕೀಲಿ(resourceBundle, k_ಆವೃತ್ತಿ_ಟ್ಯಾಗ್, &minor_len, &ಸ್ಥಿತಿ_2);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_ಆವೃತ್ತಿ = (ಅಕ್ಷರ *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        ಆದರೆ (((UResourceBundle *)resourceBundle)->f_ಆವೃತ್ತಿ == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
       
        ಆದರೆ(minor_len > 0) {
            u_UChars_ಗಮ್ಯ_Chars(minor_ಆವೃತ್ತಿ, resourceBundle->f_ಆವೃತ್ತಿ , minor_len);
            resourceBundle->f_ಆವೃತ್ತಿ[len] =  '\0';
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            uprv_strcpy(resourceBundle->f_ಆವೃತ್ತಿ, k_Default_Minor_ಆವೃತ್ತಿ);
        }
    }

    ಹಿಂತಿರುಗಿ resourceBundle->f_ಆವೃತ್ತಿ;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ures_get_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ(ಸ್ಥಿರಾಂಕ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ಹಿಂತಿರುಗಿ ures_get_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ_ಆಂತರಿಕ(resourceBundle);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ures_get_ಆವೃತ್ತಿ(ಸ್ಥಿರಾಂಕ UResourceBundle* resB, UVersionInfo ಆವೃತ್ತಿ_Info) {
    ಆದರೆ (!resB) ಹಿಂತಿರುಗಿ;

    u_ಆವೃತ್ತಿ_From_String(ಆವೃತ್ತಿ_Info, ures_get_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ_ಆಂತರಿಕ(resB));
}

/** Tree support functions *******************************/
#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚ್ಯಂಕ_LOCALE_NAME "res_index"
#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚ್ಯಂಕ_ಟ್ಯಾಗ್         "InstalledLocales"
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಟ್ಯಾಗ್       "default"

#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

ಸ್ಥಿರ int32_t U_CALLCONV
ures_loc_ಎಣಿಕೆ_Locales(UEnumeration *en, UError_ಸಂಕೇತ * /*ಸ್ಥಿತಿ*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    ಹಿಂತಿರುಗಿ ures_get_ಗಾತ್ರ(&ctx->installed);
}

U_CDECL_BEGIN


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_CALLCONV
ures_loc_ಮುಂದಿನ_Locale(UEnumeration* en,
                    int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                    UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    int32_t len = 0;
    ಆದರೆ (ures_has_ಮುಂದಿನ(res) && (k = ures_get_ಮುಂದಿನ_Resource(res, &ctx->curr, ಸ್ಥಿತಿ_2)) != ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ = ures_get_ಕೀಲಿ(k);
        len = (int32_t)uprv_strlen(ಫಲಿತಾಂಶ);
    }
    ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = len;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_ಸಂಕೇತ* /*ಸ್ಥಿತಿ*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration gLocalesEnum = {
    ಶೂನ್ಯಸೂಚಕ,
        ಶೂನ್ಯಸೂಚಕ,
        ures_loc_closeLocales,
        ures_loc_ಎಣಿಕೆ_Locales,
        uenum_unextDefault,
        ures_loc_ಮುಂದಿನ_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_ಲಭ್ಯ_Locales(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    UResourceBundle *idx = ಶೂನ್ಯಸೂಚಕ;
    UEnumeration *en = ಶೂನ್ಯಸೂಚಕ;
    ULocalesContext *myContext = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(ಗಾತ್ರ(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(ಗಾತ್ರ(UEnumeration));
    ಆದರೆ(!en || !myContext) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(en, &gLocalesEnum, ಗಾತ್ರ(UEnumeration));

    ures_init_ರಾಶಿ_Object(&myContext->installed);
    ures_init_ರಾಶಿ_Object(&myContext->curr);
    idx = ures_open_ನೇರ(ಮಾರ್ಗ, ಸೂಚ್ಯಂಕ_LOCALE_NAME, ಸ್ಥಿತಿ_2);
    ures_get_By_ಕೀಲಿ(idx, ಸೂಚ್ಯಂಕ_ಟ್ಯಾಗ್, &myContext->installed, ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            ಮಾರ್ಗ, ಸೂಚ್ಯಂಕ_LOCALE_NAME, ಸೂಚ್ಯಂಕ_ಟ್ಯಾಗ್, ures_get_ಕೀಲಿ(&myContext->installed));
#ಆದರೆ_ಅಂತ್ಯ
        en->context = myContext;
    } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", ಮಾರ್ಗ, u_errorName(*ಸ್ಥಿತಿ_2));
#ಆದರೆ_ಅಂತ್ಯ
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = ಶೂನ್ಯಸೂಚಕ;
    }
    
    ures_close(idx);
    
    ಹಿಂತಿರುಗಿ en;
}

ಸ್ಥಿರ UBool is_Locale_ಒಳ_List(UEnumeration *locEnum, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *loc_ಗಮ್ಯ_ಹುಡುಕು, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *loc;
    ಆಗ ((loc = uenum_ಮುಂದಿನ(locEnum, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2)) != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (uprv_strcmp(loc, loc_ಗಮ್ಯ_ಹುಡುಕು) == 0) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸ್ಥಿರ ಶೂನ್ಯ get_ಪೋಷಕ_For_Functional_Equivalent(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      ಪೋಷಕ) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    ಪೋಷಕ.clear();
    ಆದರೆ (res != ಶೂನ್ಯಸೂಚಕ) {
        ures_get_By_ಕೀಲಿ(res, "%%Parent", bund1, &sub_ಸ್ಥಿತಿ);
        ಆದರೆ (U_SUCCESS(sub_ಸ್ಥಿತಿ)) {
            int32_t length16;
            ಸ್ಥಿರಾಂಕ char16_t* s16 = ures_getString(bund1, &length16, &sub_ಸ್ಥಿತಿ);
            ಪೋಷಕ.appendInvariantChars(s16, length16, sub_ಸ್ಥಿತಿ);
        }
    }
    
    // If none there, use normal truncation parent
    ಆದರೆ (U_FAILURE(sub_ಸ್ಥಿತಿ) || ಪೋಷಕ.is_ಖಾಲಿಗಳು()) {
        sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        ಪೋಷಕ = ulocimp_get_ಪೋಷಕ(localeID, sub_ಸ್ಥಿತಿ);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(ಅಕ್ಷರ *ಫಲಿತಾಂಶ, int32_t ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *resName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keyword, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locid,
                             UBool *is_ಲಭ್ಯ, UBool omitDefault, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString ಸಿಕ್ಕಿತು;
    CharString ಪೋಷಕ;
    CharString ತುಂಬಿದ;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    int32_t length = 0;
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    CharString kwVal;
    ಆದರೆ (keyword != ಶೂನ್ಯಸೂಚಕ && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_ಮೌಲ್ಯ(locid, keyword, sub_ಸ್ಥಿತಿ);
        ಆದರೆ (kwVal == DEFAULT_ಟ್ಯಾಗ್) {
            kwVal.clear();
        }
    }
    ಆದರೆ (locid == ಶೂನ್ಯಸೂಚಕ) {
        locid = uloc_getDefault();
    }
    CharString ತಳ = ulocimp_get_ತಳ_Name(locid, sub_ಸ್ಥಿತಿ);
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.ದತ್ತಾಂಶ(), ತಳ.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
    ures_init_ರಾಶಿ_Object(&bund1);
    ures_init_ರಾಶಿ_Object(&bund2);

    ಪೋಷಕ.copyFrom(ತಳ, sub_ಸ್ಥಿತಿ);
    ಸಿಕ್ಕಿತು.copyFrom(ತಳ, sub_ಸ್ಥಿತಿ);

    ಆದರೆ(is_ಲಭ್ಯ) {
        UEnumeration *locEnum = ures_open_ಲಭ್ಯ_Locales(ಮಾರ್ಗ, &sub_ಸ್ಥಿತಿ);
        *is_ಲಭ್ಯ = ನಿಜ;
        ಆದರೆ (U_SUCCESS(sub_ಸ್ಥಿತಿ)) {
            *is_ಲಭ್ಯ = is_Locale_ಒಳ_List(locEnum, ಪೋಷಕ.ದತ್ತಾಂಶ(), &sub_ಸ್ಥಿತಿ);
        }
        uenum_close(locEnum);
    }

    ಆದರೆ(U_FAILURE(sub_ಸ್ಥಿತಿ)) {
        *ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ;
        ಹಿಂತಿರುಗಿ 0;
    }
    
    ಮಾಡು {
        sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        res = ures_open(ಮಾರ್ಗ, ಪೋಷಕ.ದತ್ತಾಂಶ(), &sub_ಸ್ಥಿತಿ);
        ಆದರೆ(((sub_ಸ್ಥಿತಿ == U_USING_FALLBACK_WARNING) ||
            (sub_ಸ್ಥಿತಿ == U_USING_DEFAULT_WARNING)) && is_ಲಭ್ಯ)
        {
            *is_ಲಭ್ಯ = ಸುಳ್ಳು;
        }
        is_ಲಭ್ಯ = ಶೂನ್ಯಸೂಚಕ; /* only want to set this the first time around */
        
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ಪೋಷಕ.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ), ures_getLocale(res, &sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ(U_FAILURE(sub_ಸ್ಥಿತಿ)) {
            *ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sub_ಸ್ಥಿತಿ == U_ZERO_ERROR) {
            ures_get_By_ಕೀಲಿ(res,resName,&bund1, &sub_ಸ್ಥಿತಿ);
            ಆದರೆ(sub_ಸ್ಥಿತಿ == U_ZERO_ERROR) {
                ಸ್ಥಿರಾಂಕ char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ಪೋಷಕ.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
                defUstr = ures_get_String_By_ಕೀಲಿ(&bund1, DEFAULT_ಟ್ಯಾಗ್, &defLen, &sub_ಸ್ಥಿತಿ);
                ಆದರೆ(U_SUCCESS(sub_ಸ್ಥಿತಿ) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_ಸ್ಥಿತಿ);
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ಪೋಷಕ.ದತ್ತಾಂಶ(), keyword, defVal.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
                    defLoc.copyFrom(ಪೋಷಕ, sub_ಸ್ಥಿತಿ);
                    ಆದರೆ(kwVal.is_ಖಾಲಿಗಳು()) {
                        kwVal.append(defVal, sub_ಸ್ಥಿತಿ);
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ಪೋಷಕ.ದತ್ತಾಂಶ(), keyword, kwVal.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
                    }
                }
            }
        }
        
        sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;

        ಆದರೆ (res != ಶೂನ್ಯಸೂಚಕ) {
            ಸಿಕ್ಕಿತು.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_ಸ್ಥಿತಿ), sub_ಸ್ಥಿತಿ);
        }

        ಆದರೆ (ಸಿಕ್ಕಿತು != ಪೋಷಕ) {
            ಪೋಷಕ.copyFrom(ಸಿಕ್ಕಿತು, sub_ಸ್ಥಿತಿ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            get_ಪೋಷಕ_For_Functional_Equivalent(ಸಿಕ್ಕಿತು.ದತ್ತಾಂಶ(),res,&bund1,ಪೋಷಕ);
        }
        ures_close(res);
    } ಆಗ(defVal.is_ಖಾಲಿಗಳು() && !ಸಿಕ್ಕಿತು.is_ಖಾಲಿಗಳು() && ಸಿಕ್ಕಿತು != "root" && U_SUCCESS(*ಸ್ಥಿತಿ_2));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    ಪೋಷಕ.copyFrom(ತಳ, sub_ಸ್ಥಿತಿ);
    ಸಿಕ್ಕಿತು.copyFrom(ತಳ, sub_ಸ್ಥಿತಿ);

    ಮಾಡು {
        res = ures_open(ಮಾರ್ಗ, ಪೋಷಕ.ದತ್ತಾಂಶ(), &sub_ಸ್ಥಿತಿ);
        ಆದರೆ((sub_ಸ್ಥಿತಿ == U_USING_FALLBACK_WARNING) && is_ಲಭ್ಯ) {
            *is_ಲಭ್ಯ = ಸುಳ್ಳು;
        }
        is_ಲಭ್ಯ = ಶೂನ್ಯಸೂಚಕ; /* only want to set this the first time around */
        
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ಪೋಷಕ.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ), kwVal.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ(U_FAILURE(sub_ಸ್ಥಿತಿ)) {
            *ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sub_ಸ್ಥಿತಿ == U_ZERO_ERROR) {
            ures_get_By_ಕೀಲಿ(res,resName,&bund1, &sub_ಸ್ಥಿತಿ);
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
            ಆದರೆ(sub_ಸ್ಥಿತಿ == U_ZERO_ERROR) {
                ures_get_By_ಕೀಲಿ(&bund1, kwVal.ದತ್ತಾಂಶ(), &bund2, &sub_ಸ್ಥಿತಿ);
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
                ಆದರೆ(sub_ಸ್ಥಿತಿ == U_ZERO_ERROR) {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ಪೋಷಕ.ದತ್ತಾಂಶ(), keyword, kwVal.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
                    ಆದರೆ (ಪೋಷಕ.is_ಖಾಲಿಗಳು()) {
                        ತುಂಬಿದ.clear().append("root", sub_ಸ್ಥಿತಿ);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ತುಂಬಿದ.copyFrom(ಪೋಷಕ, sub_ಸ್ಥಿತಿ);
                    }
                        /* now, recalculate default kw if need be */
                        ಆದರೆ(defLoc.length() > ತುಂಬಿದ.length()) {
                          ಸ್ಥಿರಾಂಕ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ತುಂಬಿದ.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
                          defUstr = ures_get_String_By_ಕೀಲಿ(&bund1, DEFAULT_ಟ್ಯಾಗ್, &defLen, &sub_ಸ್ಥಿತಿ);
                          ಆದರೆ(U_SUCCESS(sub_ಸ್ಥಿತಿ) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_ಸ್ಥಿತಿ);
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ತುಂಬಿದ.ದತ್ತಾಂಶ(), keyword, defVal.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
                            defLoc.copyFrom(ತುಂಬಿದ, sub_ಸ್ಥಿತಿ);
                          }
                        } /* end of recalculate default KW */
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                        ಇಲ್ಲದಿದ್ದರೆ {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.ದತ್ತಾಂಶ(), ತುಂಬಿದ.ದತ್ತಾಂಶ());
                        }
#ಆದರೆ_ಅಂತ್ಯ
                } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_ಸ್ಥಿತಿ), ಪೋಷಕ.ದತ್ತಾಂಶ(), kwVal.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
                }
            }
        }
        
        sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        UBool have_ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        ಆದರೆ (res != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(resName, "collations") == 0) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_ಸ್ಥಿತಿ);
            ಆದರೆ (U_SUCCESS(sub_ಸ್ಥಿತಿ) && validLoc != ಶೂನ್ಯಸೂಚಕ && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_ಸ್ಥಿತಿ);
                CharString ಪೋಷಕ_Lang = ulocimp_getLanguage(ಪೋಷಕ.ಗಮ್ಯ_String_Piece(), sub_ಸ್ಥಿತಿ);
                ಆದರೆ (U_SUCCESS(sub_ಸ್ಥಿತಿ) && validLang != ಪೋಷಕ_Lang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    ಸಿಕ್ಕಿತು.clear().append(validLoc, sub_ಸ್ಥಿತಿ);
                    have_ಸಿಕ್ಕಿತು = ನಿಜ;
                }
            }
            sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        }
        ಆದರೆ (!have_ಸಿಕ್ಕಿತು) {
            ಸಿಕ್ಕಿತು.copyFrom(ಪೋಷಕ, sub_ಸ್ಥಿತಿ);
        }

        get_ಪೋಷಕ_For_Functional_Equivalent(ಸಿಕ್ಕಿತು.ದತ್ತಾಂಶ(),res,&bund1,ಪೋಷಕ);
        ures_close(res);
        sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    } ಆಗ(ತುಂಬಿದ.is_ಖಾಲಿಗಳು() && !ಸಿಕ್ಕಿತು.is_ಖಾಲಿಗಳು() && U_SUCCESS(*ಸ್ಥಿತಿ_2));

    ಆದರೆ(ತುಂಬಿದ.is_ಖಾಲಿಗಳು() && kwVal != defVal) {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.ದತ್ತಾಂಶ(), defVal.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
        kwVal.clear().append(defVal, sub_ಸ್ಥಿತಿ);
        ಪೋಷಕ.copyFrom(ತಳ, sub_ಸ್ಥಿತಿ);
        ಸಿಕ್ಕಿತು.copyFrom(ತಳ, sub_ಸ್ಥಿತಿ);

        ಮಾಡು { /* search for 'default' named item */
            res = ures_open(ಮಾರ್ಗ, ಪೋಷಕ.ದತ್ತಾಂಶ(), &sub_ಸ್ಥಿತಿ);
            ಆದರೆ((sub_ಸ್ಥಿತಿ == U_USING_FALLBACK_WARNING) && is_ಲಭ್ಯ) {
                *is_ಲಭ್ಯ = ಸುಳ್ಳು;
            }
            is_ಲಭ್ಯ = ಶೂನ್ಯಸೂಚಕ; /* only want to set this the first time around */
            
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ಪೋಷಕ.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ), kwVal.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
            ಆದರೆ(U_FAILURE(sub_ಸ್ಥಿತಿ)) {
                *ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sub_ಸ್ಥಿತಿ == U_ZERO_ERROR) {
                ures_get_By_ಕೀಲಿ(res,resName,&bund1, &sub_ಸ್ಥಿತಿ);
                ಆದರೆ(sub_ಸ್ಥಿತಿ == U_ZERO_ERROR) {
                    ures_get_By_ಕೀಲಿ(&bund1, kwVal.ದತ್ತಾಂಶ(), &bund2, &sub_ಸ್ಥಿತಿ);
                    ಆದರೆ(sub_ಸ್ಥಿತಿ == U_ZERO_ERROR) {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA",
                            ಪೋಷಕ.ದತ್ತಾಂಶ(), keyword, kwVal.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
                        ಆದರೆ (ಪೋಷಕ.is_ಖಾಲಿಗಳು()) {
                            ತುಂಬಿದ.clear().append("root", sub_ಸ್ಥಿತಿ);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ತುಂಬಿದ.copyFrom(ಪೋಷಕ, sub_ಸ್ಥಿತಿ);
                        }
                        
                        /* now, recalculate default kw if need be */
                        ಆದರೆ(defLoc.length() > ತುಂಬಿದ.length()) {
                          ಸ್ಥಿರಾಂಕ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ತುಂಬಿದ.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
                          defUstr = ures_get_String_By_ಕೀಲಿ(&bund1, DEFAULT_ಟ್ಯಾಗ್, &defLen, &sub_ಸ್ಥಿತಿ);
                          ಆದರೆ(U_SUCCESS(sub_ಸ್ಥಿತಿ) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_ಸ್ಥಿತಿ);
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    ಮಾರ್ಗ?ಮಾರ್ಗ:"ICUDATA", ತುಂಬಿದ.ದತ್ತಾಂಶ(), keyword, defVal.ದತ್ತಾಂಶ(), u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
                            defLoc.copyFrom(ತುಂಬಿದ, sub_ಸ್ಥಿತಿ);
                          }
                        } /* end of recalculate default KW */
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                        ಇಲ್ಲದಿದ್ದರೆ {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.ದತ್ತಾಂಶ(), ತುಂಬಿದ.ದತ್ತಾಂಶ());
                        }
#ಆದರೆ_ಅಂತ್ಯ
                    }
                }
            }
            
            sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
            ಸಿಕ್ಕಿತು.copyFrom(ಪೋಷಕ, sub_ಸ್ಥಿತಿ);
            get_ಪೋಷಕ_For_Functional_Equivalent(ಸಿಕ್ಕಿತು.ದತ್ತಾಂಶ(),res,&bund1,ಪೋಷಕ);
            ures_close(res);
            sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        } ಆಗ(ತುಂಬಿದ.is_ಖಾಲಿಗಳು() && !ಸಿಕ್ಕಿತು.is_ಖಾಲಿಗಳು() && U_SUCCESS(*ಸ್ಥಿತಿ_2));
    }
    
    ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ಆದರೆ(ತುಂಬಿದ.is_ಖಾಲಿಗಳು()) {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
          *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(omitDefault) {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", ತುಂಬಿದ.ದತ್ತಾಂಶ(), defLoc.ದತ್ತಾಂಶ(), ಸಿಕ್ಕಿತು.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ        
          ಆದರೆ(defLoc.length() <= ತುಂಬಿದ.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            ಆದರೆ(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
              kwVal.clear();
            }
          }
        }
        ಸಿಕ್ಕಿತು.copyFrom(ತುಂಬಿದ, sub_ಸ್ಥಿತಿ);
        ಆದರೆ(!kwVal.is_ಖಾಲಿಗಳು()) {
            ಸಿಕ್ಕಿತು
                .append("@", sub_ಸ್ಥಿತಿ)
                .append(keyword, sub_ಸ್ಥಿತಿ)
                .append("=", sub_ಸ್ಥಿತಿ)
                .append(kwVal, sub_ಸ್ಥಿತಿ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!omitDefault) {
            ಸಿಕ್ಕಿತು
                .append("@", sub_ಸ್ಥಿತಿ)
                .append(keyword, sub_ಸ್ಥಿತಿ)
                .append("=", sub_ಸ್ಥಿತಿ)
                .append(defVal, sub_ಸ್ಥಿತಿ);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = ಸಿಕ್ಕಿತು.length();

    ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        int32_t copy_ಉದ್ದಳತೆ = uprv_min(length, ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ);
        ಆದರೆ(copy_ಉದ್ದಳತೆ>0) {
            ಸಿಕ್ಕಿತು.extract(ಫಲಿತಾಂಶ, copy_ಉದ್ದಳತೆ, sub_ಸ್ಥಿತಿ);
        }
        ಆದರೆ(length == 0) {
          *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR; 
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        length = 0;
        ಫಲಿತಾಂಶ[0]=0;
    }
    ಹಿಂತಿರುಗಿ u_terminateChars(ಫಲಿತಾಂಶ, ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ, length, ಸ್ಥಿತಿ_2);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_ಮೌಲ್ಯಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keyword, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
#ವ್ಯಾಖ್ಯಾನಿಸು ಮೌಲ್ಯಗಳು_BUF_ಗಾತ್ರ 2048
#ವ್ಯಾಖ್ಯಾನಿಸು ಮೌಲ್ಯಗಳು_LIST_ಗಾತ್ರ 512
    
    ಅಕ್ಷರ       ಮೌಲ್ಯಗಳು_Buf[ಮೌಲ್ಯಗಳು_BUF_ಗಾತ್ರ];
    int32_t    ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ = 0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯಗಳು_List[ಮೌಲ್ಯಗಳು_LIST_ಗಾತ್ರ];
    int32_t    ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ = 0;
    
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale;
    int32_t     locLen;
    
    UEnumeration *locs = ಶೂನ್ಯಸೂಚಕ;
    
    UResourceBundle    ಅಂಶ;
    UResourceBundle    sub_ಅಂಶ;
    
    ures_init_ರಾಶಿ_Object(&ಅಂಶ);
    ures_init_ರಾಶಿ_Object(&sub_ಅಂಶ);
    locs = ures_open_ಲಭ್ಯ_Locales(ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
    
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ures_close(&ಅಂಶ);
        ures_close(&sub_ಅಂಶ);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    
    ಮೌಲ್ಯಗಳು_Buf[0]=0;
    ಮೌಲ್ಯಗಳು_Buf[1]=0;

    ಆಗ ((locale = uenum_ಮುಂದಿನ(locs, &locLen, ಸ್ಥಿತಿ_2)) != ಶೂನ್ಯಸೂಚಕ) {
        UResourceBundle   *bund = ಶೂನ್ಯಸೂಚಕ;
        UResourceBundle   *subPtr = ಶೂನ್ಯಸೂಚಕ;
        UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(ಮಾರ್ಗ, locale, &sub_ಸ್ಥಿತಿ);
        
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
        ಆದರೆ(!bund || U_FAILURE(sub_ಸ್ಥಿತಿ)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                ಮಾರ್ಗ?ಮಾರ್ಗ:"<ICUDATA>", keyword, locale, u_errorName(sub_ಸ್ಥಿತಿ));
        }
#ಆದರೆ_ಅಂತ್ಯ
        
        ures_get_By_ಕೀಲಿ(bund, keyword, &ಅಂಶ, &sub_ಸ್ಥಿತಿ);
        
        ಆದರೆ(!bund || U_FAILURE(sub_ಸ್ಥಿತಿ)) {
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                ಮಾರ್ಗ?ಮಾರ್ಗ:"<ICUDATA>", keyword, locale, u_errorName(sub_ಸ್ಥಿತಿ));
#ಆದರೆ_ಅಂತ್ಯ
            ures_close(bund);
            bund = ಶೂನ್ಯಸೂಚಕ;
            ಮುಂದುವರಿ;
        }

        ಆಗ ((subPtr = ures_get_ಮುಂದಿನ_Resource(&ಅಂಶ, &sub_ಅಂಶ, &sub_ಸ್ಥಿತಿ)) != ಶೂನ್ಯಸೂಚಕ
            && U_SUCCESS(sub_ಸ್ಥಿತಿ)) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k;
            int32_t i;
            k = ures_get_ಕೀಲಿ(subPtr);

#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#ಆದರೆ_ಅಂತ್ಯ
            ಆದರೆ(k == ಶೂನ್ಯಸೂಚಕ || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_ಟ್ಯಾಗ್) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                ಮುಂದುವರಿ;
            }
            ಗಾಗಿ(i=0; i<ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ; i++) {
                ಆದರೆ(!uprv_strcmp(ಮೌಲ್ಯಗಳು_List[i],k)) {
                    k = ಶೂನ್ಯಸೂಚಕ; /* found duplicate */
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಆದರೆ(k != ಶೂನ್ಯಸೂಚಕ) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                ಆದರೆ((ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ >= (ಮೌಲ್ಯಗಳು_LIST_ಗಾತ್ರ-1)) ||       /* no more space in list .. */
                    ((ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ+kLen+1+1) >= ಮೌಲ್ಯಗಳು_BUF_ಗಾತ್ರ)) { /* no more space in buffer (string + 2 nulls) */
                    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* out of space.. */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    uprv_strcpy(ಮೌಲ್ಯಗಳು_Buf+ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ, k);
                    ಮೌಲ್ಯಗಳು_List[ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ++] = ಮೌಲ್ಯಗಳು_Buf+ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ;
                    ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ += kLen;
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        ಮಾರ್ಗ?ಮಾರ್ಗ:"<ICUDATA>", keyword, locale, k);
#ಆದರೆ_ಅಂತ್ಯ
                    ಮೌಲ್ಯಗಳು_Buf[ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    ಮೌಲ್ಯಗಳು_Buf[ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ++] = 0; /* terminate */
    
    ures_close(&ಅಂಶ);
    ures_close(&sub_ಅಂಶ);
    uenum_close(locs);
#ಆದರೆ defined(URES_ವೃಕ್ಷ_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*ಸ್ಥಿತಿ_2), 
        ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ, ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ uloc_openKeywordList(ಮೌಲ್ಯಗಳು_Buf, ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ, ಸ್ಥಿತಿ_2);
}
#ಆದರೆ 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ಸ್ಥಿರಾಂಕ UResourceBundle* res1, ಸ್ಥಿರಾಂಕ UResourceBundle* res2){
    ಆದರೆ(res1==ಶೂನ್ಯಸೂಚಕ || res2==ಶೂನ್ಯಸೂಚಕ){
        ಹಿಂತಿರುಗಿ res1==res2; /* pointer comparison */
    }
    ಆದರೆ(res1->f_ಕೀಲಿ==ಶೂನ್ಯಸೂಚಕ||  res2->f_ಕೀಲಿ==ಶೂನ್ಯಸೂಚಕ){
        ಹಿಂತಿರುಗಿ (res1->f_ಕೀಲಿ==res2->f_ಕೀಲಿ);
    }ಇಲ್ಲದಿದ್ದರೆ{
        ಆದರೆ(uprv_strcmp(res1->f_ಕೀಲಿ, res2->f_ಕೀಲಿ)!=0){
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಆದರೆ(uprv_strcmp(res1->f_ದತ್ತಾಂಶ->fName, res2->f_ದತ್ತಾಂಶ->fName)!=0){
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(res1->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ||  res2->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ==ಶೂನ್ಯಸೂಚಕ){
        ಹಿಂತಿರುಗಿ (res1->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ == res2->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ);
    }ಇಲ್ಲದಿದ್ದರೆ{
        ಆದರೆ(uprv_strcmp(res1->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ, res2->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ)!=0){
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಆದರೆ(uprv_strcmp(res1->f_ದತ್ತಾಂಶ->f_ಪೋಷಕ->fName, res2->f_ದತ್ತಾಂಶ->f_ಪೋಷಕ->fName)!=0){
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(uprv_strcmp(res1->f_ದತ್ತಾಂಶ->f_ಪೋಷಕ->f_ಮಾರ್ಗ, res2->f_ದತ್ತಾಂಶ->f_ಪೋಷಕ->f_ಮಾರ್ಗ)!=0){
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(uprv_strncmp(res1->f_Res_ಮಾರ್ಗ, res2->f_Res_ಮಾರ್ಗ, res1->f_Res_ಮಾರ್ಗ_Len)!=0){
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(res1->fRes != res2->fRes){
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ಸ್ಥಿರಾಂಕ UResourceBundle* res, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){
    UResourceBundle* bundle = ಶೂನ್ಯಸೂಚಕ;
    UResourceBundle* ret = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2) || res == ಶೂನ್ಯಸೂಚಕ){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    bundle = ures_open(res->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ, res->f_ದತ್ತಾಂಶ->fName, ಸ್ಥಿತಿ_2);
    ಆದರೆ(res->f_Res_ಮಾರ್ಗ!=ಶೂನ್ಯಸೂಚಕ){
        ret = ures_ಕಂಡುಹಿಡಿ_Sub_Resource(bundle, res->f_Res_ಮಾರ್ಗ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
        ures_close(bundle);
    }ಇಲ್ಲದಿದ್ದರೆ{
        ret = bundle;
    }
    ಹಿಂತಿರುಗಿ ret;
}
U_CAPI ಸ್ಥಿರಾಂಕ UResourceBundle* U_EXPORT2
ures_get_ಪೋಷಕ_Bundle(ಸ್ಥಿರಾಂಕ UResourceBundle* res){
    ಆದರೆ(res==ಶೂನ್ಯಸೂಚಕ){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ res->f_ಪೋಷಕ_Res;
}
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_get_ಆವೃತ್ತಿ_By_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UResourceBundle* res, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, UVersionInfo ver, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಸ್ಥಿರಾಂಕ char16_t *str;
  int32_t len;
  str = ures_get_String_By_ಕೀಲಿ(res, ಕೀಲಿ, &len, ಸ್ಥಿತಿ_2);
  ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
    u_ಆವೃತ್ತಿ_From_UString(ver, str);
  } 
}

/* eof */
