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

using ನಾಮಾಕಾಶ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ಸ್ಥಿರ UHashtable *cache = nullptr;
ಸ್ಥಿರ icu::UInitOnce gCacheInitOnce {};

ಸ್ಥಿರ UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ಸ್ಥಿರ int32_t U_CALLCONV hashEntry(ಧ್ರುವ UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    ಪ್ರತ್ಯಾಗಮ uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ಸ್ಥಿರ UBool U_CALLCONV compareEntries(ಧ್ರುವ UHashTok p1, ಧ್ರುವ UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    ಪ್ರತ್ಯಾಗಮ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ಸ್ಥಿರ UBool chopLocale(ವರ್ಣ *name) {
    ವರ್ಣ *i = uprv_strrchr(name, '_');

    ಯದಿ(i != nullptr) {
        *i = '\0';
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಸ್ಥಿರ UBool hasVariant(ಧ್ರುವ ವರ್ಣ* localeID) {
    UErrorCode err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(nullptr, 0);
    ulocimp_getSubtags(
            localeID,
            nullptr,
            nullptr,
            nullptr,
            &sink,
            nullptr,
            err);
    ಪ್ರತ್ಯಾಗಮ sink.ಸಂಖ್ಯೆ_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#ಅನ್ತರ್ಭಾವ "localefallback_data.h"

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* performFallbackLookup(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                         ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ_Strs,
                                         ಧ್ರುವ ವರ್ಣ* ಮೌಲ್ಯ_Strs U_LIFETIME_BOUND,
                                         ಧ್ರುವ int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    ಧ್ರುವ int32_t* bottom = lookupTable;
    ಧ್ರುವ int32_t* ತುದಿ = lookupTable + lookupTableLength;

    ಯಾವತ್ (bottom < ತುದಿ) {
        // Effectively, divide by 2 and round down to an even index
        ಧ್ರುವ int32_t* ಮಧ್ಯ = bottom + (((ತುದಿ - bottom) / 4) * 2);
        ಧ್ರುವ ವರ್ಣ* entry_ಕೀಲಿ = &(ಕೀಲಿ_Strs[*ಮಧ್ಯ]);
        int32_t strcmpResult = uprv_strcmp(ಕೀಲಿ, entry_ಕೀಲಿ);
        ಯದಿ (strcmpResult == 0) {
            ಪ್ರತ್ಯಾಗಮ &(ಮೌಲ್ಯ_Strs[ಮಧ್ಯ[1]]);
        } ಅನ್ಯಥಾ ಯದಿ (strcmpResult < 0) {
            ತುದಿ = ಮಧ್ಯ;
        } ಅನ್ಯಥಾ {
            bottom = ಮಧ್ಯ + 2;
        }
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಸ್ಥಿರ CharString getDefaultScript(ಧ್ರುವ CharString& language, ಧ್ರುವ CharString& region) {
    ಧ್ರುವ ವರ್ಣ* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    ಯದಿ (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        ಯದಿ (U_FAILURE(err)) {
            ಪ್ರತ್ಯಾಗಮ result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    ಯದಿ (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    ಯದಿ (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಗಣನಾ UResOpenType {
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
    URES_OPEN_DIRECT
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UResOpenType UResOpenType;

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
ಸ್ಥಿರ ತಾರ್ಕಿಕ getParentLocaleID(ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    ಯದಿ (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        ಪ್ರತ್ಯಾಗಮ chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    ಯದಿ (U_FAILURE(err)) {
        // hopefully this never happens...
        ಪ್ರತ್ಯಾಗಮ chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    ಯದಿ (openType == URES_OPEN_LOCALE_DEFAULT_ಬೇರು) {
        ಧ್ರುವ ವರ್ಣ* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        ಯದಿ (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    ಯದಿ (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        ಯದಿ (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } ಅನ್ಯಥಾ {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } ಅನ್ಯಥಾ ಯದಿ (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        ಯದಿ (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } ಅನ್ಯಥಾ {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } ಅನ್ಯಥಾ ಯದಿ (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        ಯದಿ (openType != URES_OPEN_LOCALE_DEFAULT_ಬೇರು || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    } ಅನ್ಯಥಾ {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ಸ್ಥಿರ UBool mayHaveParent(ವರ್ಣ *name) {
    ಪ್ರತ್ಯಾಗಮ (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
ಸ್ಥಿರ ಶೂನ್ಯ entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_ಎಣಿಕೆ_Existing++;
    ಯಾವತ್(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_ಎಣಿಕೆ_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ಸ್ಥಿರ UResourceDataEntry *getFallbackData(
        ಧ್ರುವ UResourceBundle *resBundle,
        ಧ್ರುವ ವರ್ಣ **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    ಯದಿ(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_ಕೀಲಿ(&(dataEntry->fData), dataEntry->fData.ಬೇರು_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    ಯದಿ(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        ಯಾವತ್(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            ಯದಿ(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_ಕೀಲಿ(&(dataEntry->fData), dataEntry->fData.ಬೇರು_Res, &indexR, resTag);
            }
        }
    }

    ಯದಿ(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    ಯದಿ(i>1) {
        ಯದಿ(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_ಬೇರು_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } ಅನ್ಯಥಾ {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    ಪ್ರತ್ಯಾಗಮ dataEntry;
}

ಸ್ಥಿರ ಶೂನ್ಯ
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    ಯದಿ(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    ಯದಿ(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    ಯದಿ(entry->fPool != nullptr) {
        --entry->fPool->f_ಎಣಿಕೆ_Existing;
    }
    alias = entry->fAlias;
    ಯದಿ(alias != nullptr) {
        ಯಾವತ್(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_ಎಣಿಕೆ_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
ಸ್ಥಿರ int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_ಅಂಕೆ = 0;
    ಧ್ರುವ UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    ಯದಿ (cache == nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಕುರು {
        deletedMore = ಅಸತ್ಯ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        ಯಾವತ್ ((e = uhash_nextElement(cache, &pos)) != nullptr)
        {
            resB = static_cast<UResourceDataEntry*>(e->value.pointer);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            ಯದಿ (resB->f_ಎಣಿಕೆ_Existing == 0) {
                rb_Deleted_ಅಂಕೆ++;
                deletedMore = ಸತ್ಯ;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } ಯಾವತ್(deletedMore);

    ಪ್ರತ್ಯಾಗಮ rb_Deleted_ಅಂಕೆ;
}

#ifdef URES_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = ಅಸತ್ಯ;
  int32_t pos = UHASH_FIRST;
  ಧ್ರುವ UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    ಯದಿ (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    ಯಾವತ್ ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=ಸತ್ಯ;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (ಶೂನ್ಯ*)resB, resB->f_ಎಣಿಕೆ_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (ಶೂನ್ಯ*)resB->fPool,
              (ಶೂನ್ಯ*)resB->fAlias,
              (ಶೂನ್ಯ*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_ಎಣಿಕೆ(cache));
    ಪ್ರತ್ಯಾಗಮ cacheNotEmpty;
}

#endif

ಸ್ಥಿರ UBool U_CALLCONV ures_cleanup()
{
    ಯದಿ (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/** INTERNAL: Initializes the cache for resources */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ಸ್ಥಿರ ಶೂನ್ಯ initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ಸ್ಥಿರ ಶೂನ್ಯ setEntryName(UResourceDataEntry *res, ಧ್ರುವ ವರ್ಣ *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    ಯದಿ(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    ಯದಿ (len < static_cast<int32_t>(ಪರಿಮಾಣ(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    ಅನ್ಯಥಾ {
        res->fName = static_cast<ವರ್ಣ*>(uprv_malloc(len + 1));
    }
    ಯದಿ(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ಅನ್ಯಥಾ {
        uprv_strcpy(res->fName, name);
    }
}

ಸ್ಥಿರ UResourceDataEntry *
getPoolEntry(ಧ್ರುವ ವರ್ಣ *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ಸ್ಥಿರ UResourceDataEntry *init_entry(ಧ್ರುವ ವರ್ಣ *localeID, ಧ್ರುವ ವರ್ಣ *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    ಧ್ರುವ ವರ್ಣ *name;
    ವರ್ಣ aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    ಯದಿ(U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* here we try to deduce the right locale name */
    ಯದಿ(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } ಅನ್ಯಥಾ ಯದಿ(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_ಬೇರು_Locale_Name;
    } ಅನ್ಯಥಾ { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<ವರ್ಣ*>(name);
    find.fPath = const_cast<ವರ್ಣ*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    ಯದಿ(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(ಪರಿಮಾಣ(UResourceDataEntry)));
        ಯದಿ(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }

        uprv_memset(r, 0, ಪರಿಮಾಣ(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        ಯದಿ (U_FAILURE(*status)) {
            uprv_free(r);
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }

        ಯದಿ(path != nullptr) {
            r->fPath = uprv_strdup(path);
            ಯದಿ(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        ಯದಿ (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            ಯದಿ (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } ಅನ್ಯಥಾ { /* if we have a regular entry */
            Resource aliasres;
            ಯದಿ (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                ಯದಿ (U_SUCCESS(*status)) {
                    ಧ್ರುವ int32_t *poolIndexes = r->fPool->fData.p_ಬೇರು + 1;
                    ಯದಿ(r->fData.p_ಬೇರು[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_ಕೀಲಿಗಳು = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } ಅನ್ಯಥಾ {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } ಅನ್ಯಥಾ {
                    r->fBogus = *status;
                }
            }
            ಯದಿ (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                ಯದಿ (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ಧ್ರುವ char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    ಯದಿ(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            ಯದಿ ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (ಶೂನ್ಯ *)r, r, &cacheStatus);
                ಯದಿ (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } ಅನ್ಯಥಾ {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    ಯದಿ(r != nullptr) {
        /* return the real bundle */
        ಯಾವತ್(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_ಎಣಿಕೆ_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        ಯದಿ(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ UResourceDataEntry *
getPoolEntry(ಧ್ರುವ ವರ್ಣ *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    ಯದಿ( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ಸ್ಥಿರ UResourceDataEntry *
findFirstExisting(ಧ್ರುವ ವರ್ಣ* path, ವರ್ಣ* name, ಧ್ರುವ ವರ್ಣ* defaultLocale, UResOpenType openType,
                  UBool *is_ಬೇರು, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = ಅಸತ್ಯ;
    *foundParent = ಸತ್ಯ; /* we're starting with a fresh name */
    ವರ್ಣ origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    ಯಾವತ್(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        ಯದಿ (U_FAILURE(*status)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        ಯದಿ(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_ಎಣಿಕೆ_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } ಅನ್ಯಥಾ {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_ಬೇರು = static_cast<UBool>(uprv_strcmp(name, k_ಬೇರು_Locale_Name) == 0);

        /*Fallback data stuff*/
        ಯದಿ (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } ಅನ್ಯಥಾ {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        ಯದಿ (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಶೂನ್ಯ ures_set_Is_ರಾಶಿ_Object( UResourceBundle* resB, UBool state) {
    ಯದಿ(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } ಅನ್ಯಥಾ {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ಸ್ಥಿರ UBool ures_is_ರಾಶಿ_Object(ಧ್ರುವ UResourceBundle* resB) {
  ಪ್ರತ್ಯಾಗಮ((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?ಅಸತ್ಯ:ಸತ್ಯ);
}


U_CFUNC ಶೂನ್ಯ ures_init_ರಾಶಿ_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, ಪರಿಮಾಣ(UResourceBundle));
  ures_set_Is_ರಾಶಿ_Object(resB, ಸತ್ಯ);
}

U_NAMESPACE_BEGIN

ರಾಶಿ_UResource_Bundle::ರಾಶಿ_UResource_Bundle() {
    ures_init_ರಾಶಿ_Object(&bundle);
}

ರಾಶಿ_UResource_Bundle::~ರಾಶಿ_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

ಸ್ಥಿರ UBool  // returns U_SUCCESS(*status)
load_Parents_Except_ಬೇರು(UResourceDataEntry *&t1,
                      ವರ್ಣ name[], int32_t nameCapacity,
                      UBool usingUSRData, ವರ್ಣ usrDataPath[], UErrorCode *status) {
    ಯದಿ (U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    UBool checkParent = ಸತ್ಯ;
    ಯಾವತ್ (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        ಯದಿ (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            ಧ್ರುವ char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            ಯದಿ(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                ಯದಿ (uprv_strcmp(name, k_ಬೇರು_Locale_Name) == 0) {
                    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        ಯದಿ (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        ಯದಿ (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            ಯದಿ (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }

        ಯದಿ (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } ಅನ್ಯಥಾ {
            t1->fParent = t2;
            ಯದಿ (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_ಎಣಿಕೆ_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ UBool  // returns U_SUCCESS(*status)
ಸೇರಿಸು_ಬೇರು_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    ಯದಿ (U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_ಬೇರು_Locale_Name, t1->fPath, &parentStatus);
    ಯದಿ (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    t1->fParent = t2;
    t1 = t2;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ UResourceDataEntry *entryOpen(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = ಅಸತ್ಯ;
    UBool is_ಬೇರು = ಅಸತ್ಯ;
    UBool hasRealData = ಅಸತ್ಯ;
    UBool hasChopped = ಸತ್ಯ;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    ವರ್ಣ name[ULOC_FULLNAME_CAPACITY];
    ವರ್ಣ usrDataPath[96];

    initCache(status);

    ಯದಿ(U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    uprv_strncpy(name, localeID, ಪರಿಮಾಣ(name) - 1);
    name[ಪರಿಮಾಣ(name) - 1] = 0;

    ಯದಿ ( usingUSRData ) {
        ಯದಿ ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } ಅನ್ಯಥಾ {
            uprv_strncpy(usrDataPath, path, ಪರಿಮಾಣ(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[ಪರಿಮಾಣ(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ಧ್ರುವ ವರ್ಣ *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_ಬೇರು, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    ಯದಿ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        ಗಚ್ಛ finish;
    }

    ಯದಿ(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = ಸತ್ಯ;
        ಯದಿ ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            ಯದಿ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                ಗಚ್ಛ finish;
            }
            ಯದಿ ( u1 != nullptr ) {
                ಯದಿ(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } ಅನ್ಯಥಾ {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_ಎಣಿಕೆ_Existing = 0;
                }
            }
        }
        ಯದಿ ((hasChopped || mayHaveParent(name)) && !is_ಬೇರು) {
            ಯದಿ (!load_Parents_Except_ಬೇರು(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                ಗಚ್ಛ finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    ಯದಿ(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_ಬೇರು && !isDefault && !is_ಬೇರು) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_ಬೇರು, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        ಯದಿ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            ಗಚ್ಛ finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        ಯದಿ(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = ಸತ್ಯ;
            isDefault = ಸತ್ಯ;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            ಯದಿ ((hasChopped || mayHaveParent(name)) && !is_ಬೇರು) {
                ಯದಿ (!load_Parents_Except_ಬೇರು(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    ಗಚ್ಛ finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    ಯದಿ(r == nullptr) {
        uprv_strcpy(name, k_ಬೇರು_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_ಬೇರು, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        ಯದಿ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            ಗಚ್ಛ finish;
        }
        ಯದಿ(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = ಸತ್ಯ;
        } ಅನ್ಯಥಾ { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            ಗಚ್ಛ finish;
        }
    } ಅನ್ಯಥಾ ಯದಿ(!is_ಬೇರು && uprv_strcmp(t1->fName, k_ಬೇರು_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        ಯದಿ (!ಸೇರಿಸು_ಬೇರು_Bundle(t1, status)) {
            ಗಚ್ಛ finish;
        }
        ಯದಿ(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    ಯಾವತ್(r != nullptr && !is_ಬೇರು && t1->fParent != nullptr) {
        t1->fParent->f_ಎಣಿಕೆ_Existing++;
        t1 = t1->fParent;
    }

finish:
    ಯದಿ(U_SUCCESS(*status)) {
        ಯದಿ(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        ಪ್ರತ್ಯಾಗಮ r;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ಸ್ಥಿರ UResourceDataEntry *
entryOpenDirect(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* localeID, UErrorCode* status) {
    initCache(status);
    ಯದಿ(U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    } ಅನ್ಯಥಾ ಯದಿ (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_ಬೇರು_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    ಯದಿ(U_SUCCESS(*status)) {
        ಯದಿ(r->fBogus != U_ZERO_ERROR) {
            r->f_ಎಣಿಕೆ_Existing--;
            r = nullptr;
        }
    } ಅನ್ಯಥಾ {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    ಯದಿ(r != nullptr && uprv_strcmp(localeID, k_ಬೇರು_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        ವರ್ಣ name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        ಯದಿ(!chopLocale(name) || uprv_strcmp(name, k_ಬೇರು_Locale_Name) == 0 ||
                load_Parents_Except_ಬೇರು(t1, name, UPRV_LENGTHOF(name), ಅಸತ್ಯ, nullptr, status)) {
            ಯದಿ(uprv_strcmp(t1->fName, k_ಬೇರು_Locale_Name) != 0 && t1->fParent == nullptr) {
                ಸೇರಿಸು_ಬೇರು_Bundle(t1, status);
            }
        }
        ಯದಿ(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    ಯದಿ(r != nullptr) {
        // TODO: Does this ever loop?
        ಯಾವತ್(t1->fParent != nullptr) {
            t1->fParent->f_ಎಣಿಕೆ_Existing++;
            t1 = t1->fParent;
        }
    }
    ಪ್ರತ್ಯಾಗಮ r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ಸ್ಥಿರ ಶೂನ್ಯ entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    ಯಾವತ್(resB != nullptr) {
        p = resB->fParent;
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

ಸ್ಥಿರ ಶೂನ್ಯ entryClose(UResourceDataEntry *resB) {
  Mutex lock(&resbMutex);
  entryCloseInt(resB);
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
ಸ್ಥಿರ ಶೂನ್ಯ ures_appendResPath(UResourceBundle *resB, ಧ್ರುವ ವರ್ಣ* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    ಯದಿ(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    ಯದಿ(RES_BUFSIZE <= resB->fResPathLen+1) {
        ಯದಿ(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<ವರ್ಣ*>(uprv_malloc((resB->fResPathLen + 1) * ಪರಿಮಾಣ(ವರ್ಣ)));
            /* Check that memory was allocated correctly. */
            ಯದಿ (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } ಅನ್ಯಥಾ {
            ವರ್ಣ* temp = static_cast<ವರ್ಣ*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * ಪರಿಮಾಣ(ವರ್ಣ)));
            /* Check that memory was reallocated correctly. */
            ಯದಿ (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

ಸ್ಥಿರ ಶೂನ್ಯ ures_freeResPath(UResourceBundle *resB) {
    ಯದಿ (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    ಯದಿ(resB != nullptr) {
        ಯದಿ(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        ಯದಿ(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        ಯದಿ(ures_is_ರಾಶಿ_Object(resB) == ಅಸತ್ಯ && freeBundleObj) {
            uprv_free(resB);
        }
#ಯದಿ 0 /*U_DEBUG*/
        ಅನ್ಯಥಾ {
            /* poison the data */
            uprv_memset(resB, -1, ಪರಿಮಾಣ(UResourceBundle));
        }
#endif
    }
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, ಸತ್ಯ);
}

ನಾಮಾಕಾಶ {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ಧ್ರುವ ವರ್ಣ* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(ಧ್ರುವ ResourceData& resData,
                               Resource r,
                               ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               ಧ್ರುವ ವರ್ಣ* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ಯದಿ (U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ಧ್ರುವ char16_t *alias = res_getAlias(&resData, r, &len);
    ಯದಿ(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ resB;
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
    chAlias.appendInvariantChars(alias, len, *status);
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // We have an alias, now let's cut it up.
    ಧ್ರುವ ವರ್ಣ *path = nullptr, *locale = nullptr, *ಕೀಲಿ_Path = nullptr;
    ಯದಿ(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        ವರ್ಣ *chAliasData = chAlias.data();
        ವರ್ಣ *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        ಯದಿ(sep != nullptr) {
            *sep++ = 0;
        }
        ಯದಿ(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            ಕೀಲಿ_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } ಅನ್ಯಥಾ {
            ಯದಿ(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            ಯದಿ (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } ಅನ್ಯಥಾ {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                ಯದಿ(sep != nullptr) {
                    *sep++ = 0;
                }
                ಕೀಲಿ_Path = sep;
            }
        }
    } ಅನ್ಯಥಾ {
        // No path, start with a locale.
        ವರ್ಣ *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        ಯದಿ(sep != nullptr) {
            *sep++ = 0;
        }
        ಕೀಲಿ_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    ಯದಿ (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } ಅನ್ಯಥಾ {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        ಯದಿ(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            ಪ್ರತ್ಯಾಗಮ resB;
        }
        dataEntry = mainRes->fData;
    }

    ಧ್ರುವ ವರ್ಣ* temp = nullptr;
    ಯದಿ(ಕೀಲಿ_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.ಬೇರು_Res;
        ಯದಿ(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            ಯದಿ (U_FAILURE(*status)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            ವರ್ಣ *a_ಕೀಲಿ = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_ಕೀಲಿ, &temp);
        }
        ಯದಿ(ಕೀಲಿ) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(ಕೀಲಿ, *status);
            ಯದಿ (U_FAILURE(*status)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            ವರ್ಣ *a_ಕೀಲಿ = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_ಕೀಲಿ, &temp);
        } ಅನ್ಯಥಾ ಯದಿ(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            ಯದಿ(URES_IS_TABLE(type)) {
                ಧ್ರುವ ವರ್ಣ *a_ಕೀಲಿ;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_ಕೀಲಿ);
            } ಅನ್ಯಥಾ { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        ಯದಿ(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } ಅನ್ಯಥಾ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } ಅನ್ಯಥಾ {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(ಕೀಲಿ_Path, *status);
        ಯದಿ (U_FAILURE(*status)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ವರ್ಣ *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        ಪ್ರತಿ(;;) {
            r = dataEntry->fData.ಬೇರು_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            ಯಾವತ್(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                ಯದಿ(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    ಭಙ್ಗ;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                ಯದಿ (U_FAILURE(*status)) {
                    ಭಙ್ಗ;
                }
                ಯದಿ (temp == nullptr || uprv_strcmp(ಕೀಲಿ_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, ಕೀಲಿ_Path, static_cast<int32_t>(uprv_strlen(ಕೀಲಿ_Path)), status);
                    ಯದಿ(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    ಯದಿ (U_FAILURE(*status)) {
                        ಭಙ್ಗ;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            ಯದಿ (U_FAILURE(*status) || r != RES_BOGUS) {
                ಭಙ್ಗ;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            ಯದಿ (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                ಭಙ್ಗ;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, ಕೀಲಿ_Path);
        }
    }
    ಯದಿ(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    ಪ್ರತ್ಯಾಗಮ resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ಧ್ರುವ ವರ್ಣ* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ಯದಿ(status == nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ resB;
    }
    ಯದಿ (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        ಯದಿ(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            ಪ್ರತ್ಯಾಗಮ resB;
        }
        ಪ್ರತ್ಯಾಗಮ getAliasTargetAsResourceBundle(
            dataEntry->fData, r, ಕೀಲಿ, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    ಯದಿ(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(ಪರಿಮಾಣ(UResourceBundle)));
        ಯದಿ (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ures_set_Is_ರಾಶಿ_Object(resB, ಅಸತ್ಯ);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } ಅನ್ಯಥಾ {
        ಯದಿ(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        ಯದಿ(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
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
        ಯದಿ(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = ಅಸತ್ಯ;
    resB->f_Is_ತುದಿ_Level = ಅಸತ್ಯ;
    resB->fIndex = -1;
    resB->f_ಕೀಲಿ = ಕೀಲಿ; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    ಯದಿ(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    ಯದಿ(ಕೀಲಿ != nullptr) {
        ures_appendResPath(resB, ಕೀಲಿ, static_cast<int32_t>(uprv_strlen(ಕೀಲಿ)), status);
        ಯದಿ(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } ಅನ್ಯಥಾ ಯದಿ(idx >= 0) {
        ವರ್ಣ buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        ಯದಿ(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, ಪರಿಮಾಣ(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    ಪ್ರತ್ಯಾಗಮ resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ಧ್ರುವ UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    ಪ್ರತ್ಯಾಗಮ init_resb_result(
        dataEntry, r, ಕೀಲಿ, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ಧ್ರುವ UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_ರಾಶಿ_Object;
    ಯದಿ(U_FAILURE(*status) || r == original) {
        ಪ್ರತ್ಯಾಗಮ r;
    }
    ಯದಿ(original != nullptr) {
        ಯದಿ(r == nullptr) {
            is_ರಾಶಿ_Object = ಅಸತ್ಯ;
            r = static_cast<UResourceBundle*>(uprv_malloc(ಪರಿಮಾಣ(UResourceBundle)));
            /* test for nullptr */
            ಯದಿ (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
        } ಅನ್ಯಥಾ {
            is_ರಾಶಿ_Object = ures_is_ರಾಶಿ_Object(r);
            ures_closeBundle(r, ಅಸತ್ಯ);
        }
        uprv_memcpy(r, original, ಪರಿಮಾಣ(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        ಯದಿ(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_ರಾಶಿ_Object(r, is_ರಾಶಿ_Object);
        ಯದಿ(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    ಪ್ರತ್ಯಾಗಮ r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2 ures_getString(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    ಧ್ರುವ char16_t *s;
    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    ಯದಿ (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ*
ures_toUTF8String(ಧ್ರುವ char16_t* s16,
                  int32_t length16,
                  ವರ್ಣ* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (pLength != nullptr) {
        capacity = *pLength;
    } ಅನ್ಯಥಾ {
        capacity = 0;
    }
    ಯದಿ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ (length16 == 0) {
        /* empty string, return as read-only pointer */
        ಯದಿ (pLength != nullptr) {
            *pLength = 0;
        }
        ಯದಿ (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            ಪ್ರತ್ಯಾಗಮ dest;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ "";
        }
    } ಅನ್ಯಥಾ {
        /* We need to transform the string to the destination buffer. */
        ಯದಿ (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            ಪ್ರತ್ಯಾಗಮ u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        ಯದಿ (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            int32_t maxLength = 3 * length16 + 1;
            ಯದಿ (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        ಪ್ರತ್ಯಾಗಮ u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_getUTF8String(ಧ್ರುವ UResourceBundle* resB,
                   ವರ್ಣ* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    ಧ್ರುವ char16_t *s16 = ures_getString(resB, &length16, status);
    ಪ್ರತ್ಯಾಗಮ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI ಧ್ರುವ uint8_t* U_EXPORT2
ures_get_ದ್ವಿಮಾನ(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ಧ್ರುವ uint8_t *p;
  ಯದಿ (status==nullptr || U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಯದಿ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  p = res_get_ದ್ವಿಮಾನ({resB}, &resB->getResData(), resB->fRes, len);
  ಯದಿ (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  ಪ್ರತ್ಯಾಗಮ p;
}

U_CAPI ಧ್ರುವ int32_t* U_EXPORT2
ures_getIntVector(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ಧ್ರುವ int32_t *p;
  ಯದಿ (status==nullptr || U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಯದಿ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  ಯದಿ (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  ಪ್ರತ್ಯಾಗಮ p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ಧ್ರುವ UResourceBundle* resB, UErrorCode *status) {
  ಯದಿ (status==nullptr || U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ 0xffffffff;
  }
  ಯದಿ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0xffffffff;
  }
  ಯದಿ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    ಪ್ರತ್ಯಾಗಮ 0xffffffff;
  }
  ಪ್ರತ್ಯಾಗಮ res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ಧ್ರುವ UResourceBundle* resB, UErrorCode *status) {
  ಯದಿ (status==nullptr || U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ 0xffffffff;
  }
  ಯದಿ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0xffffffff;
  }
  ಯದಿ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    ಪ್ರತ್ಯಾಗಮ 0xffffffff;
  }
  ಪ್ರತ್ಯಾಗಮ res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ಧ್ರುವ UResourceBundle *resB) {
  ಯದಿ(resB == nullptr) {
    ಪ್ರತ್ಯಾಗಮ URES_NONE;
  }
  ಪ್ರತ್ಯಾಗಮ res_getPublicType(resB->fRes);
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_get_ಕೀಲಿ(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  ಯದಿ(resB == nullptr) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಪ್ರತ್ಯಾಗಮ(resB->f_ಕೀಲಿ);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(ಧ್ರುವ UResourceBundle *resB) {
  ಯದಿ(resB == nullptr) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  
  ಪ್ರತ್ಯಾಗಮ resB->fSize;
}

ಸ್ಥಿರ ಧ್ರುವ char16_t*
ures_getStringWithAlias(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  ಯದಿ(RES_GET_TYPE(r) == URES_ALIAS) {
    ಧ್ರುವ char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    ಪ್ರತ್ಯಾಗಮ result;
  } ಅನ್ಯಥಾ {
    ಪ್ರತ್ಯಾಗಮ res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  ಯದಿ(resB == nullptr) {
    ಪ್ರತ್ಯಾಗಮ;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(ಧ್ರುವ UResourceBundle *resB) {
  ಯದಿ(resB == nullptr) {
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
  }
  ಪ್ರತ್ಯಾಗಮ resB->fIndex < resB->fSize-1;
}

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ಧ್ರುವ ವರ್ಣ** ಕೀಲಿ,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  ಯದಿ (status==nullptr || U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಯದಿ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  
  ಯದಿ(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } ಅನ್ಯಥಾ {
    resB->fIndex++;
    ವಿಕಲ್ಪ(RES_GET_TYPE(resB->fRes)) {
    ಪಕ್ಷ URES_STRING:
    ಪಕ್ಷ URES_STRING_V2:
      ಪ್ರತ್ಯಾಗಮ res_getString({resB}, &resB->getResData(), resB->fRes, len);
    ಪಕ್ಷ URES_TABLE:
    ಪಕ್ಷ URES_TABLE16:
    ಪಕ್ಷ URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, ಕೀಲಿ);
      ಯದಿ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ಪ್ರತ್ಯಾಗಮ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    ಪಕ್ಷ URES_ARRAY:
    ಪಕ್ಷ URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      ಯದಿ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ಪ್ರತ್ಯಾಗಮ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    ಪಕ್ಷ URES_ALIAS:
      ಪ್ರತ್ಯಾಗಮ ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    ಪಕ್ಷ URES_INT:
    ಪಕ್ಷ URES_ದ್ವಿಮಾನ:
    ಪಕ್ಷ URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
  }

  ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ = nullptr;
    Resource r = RES_BOGUS;

    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            ಪ್ರತ್ಯಾಗಮ fillIn;
    }
    ಯದಿ(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            ಪ್ರತ್ಯಾಗಮ fillIn;
    }

    ಯದಿ(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } ಅನ್ಯಥಾ {
        resB->fIndex++;
        ವಿಕಲ್ಪ(RES_GET_TYPE(resB->fRes)) {
        ಪಕ್ಷ URES_INT:
        ಪಕ್ಷ URES_ದ್ವಿಮಾನ:
        ಪಕ್ಷ URES_STRING:
        ಪಕ್ಷ URES_STRING_V2:
        ಪಕ್ಷ URES_INT_VECTOR:
            ಪ್ರತ್ಯಾಗಮ ures_copyResb(fillIn, resB, status);
        ಪಕ್ಷ URES_TABLE:
        ಪಕ್ಷ URES_TABLE16:
        ಪಕ್ಷ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &ಕೀಲಿ);
            ಯದಿ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಪ್ರತ್ಯಾಗಮ init_resb_result(resB->fData, r, ಕೀಲಿ, resB->fIndex, resB, fillIn, status);
        ಪಕ್ಷ URES_ARRAY:
        ಪಕ್ಷ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            ಯದಿ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಪ್ರತ್ಯಾಗಮ init_resb_result(resB->fData, r, ಕೀಲಿ, resB->fIndex, resB, fillIn, status);
        ಸ್ವಭಾವ:
            /*return nullptr;*/
            ಪ್ರತ್ಯಾಗಮ fillIn;
        }
    }
    /*return nullptr;*/
    ಪ್ರತ್ಯಾಗಮ fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(ಧ್ರುವ UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ = nullptr;
    Resource r = RES_BOGUS;

    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        ಪ್ರತ್ಯಾಗಮ fillIn;
    }
    ಯದಿ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        ಪ್ರತ್ಯಾಗಮ fillIn;
    }

    ಯದಿ(indexR >= 0 && resB->fSize > indexR) {
        ವಿಕಲ್ಪ(RES_GET_TYPE(resB->fRes)) {
        ಪಕ್ಷ URES_INT:
        ಪಕ್ಷ URES_ದ್ವಿಮಾನ:
        ಪಕ್ಷ URES_STRING:
        ಪಕ್ಷ URES_STRING_V2:
        ಪಕ್ಷ URES_INT_VECTOR:
            ಪ್ರತ್ಯಾಗಮ ures_copyResb(fillIn, resB, status);
        ಪಕ್ಷ URES_TABLE:
        ಪಕ್ಷ URES_TABLE16:
        ಪಕ್ಷ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &ಕೀಲಿ);
            ಯದಿ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಪ್ರತ್ಯಾಗಮ init_resb_result(resB->fData, r, ಕೀಲಿ, indexR, resB, fillIn, status);
        ಪಕ್ಷ URES_ARRAY:
        ಪಕ್ಷ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            ಯದಿ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಪ್ರತ್ಯಾಗಮ init_resb_result(resB->fData, r, ಕೀಲಿ, indexR, resB, fillIn, status);
        ಸ್ವಭಾವ:
            /*return nullptr;*/
            ಪ್ರತ್ಯಾಗಮ fillIn;
        }
    } ಅನ್ಯಥಾ {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    ಪ್ರತ್ಯಾಗಮ fillIn;
}

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
ures_getStringByIndex(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ = nullptr;
    Resource r = RES_BOGUS;

    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ(indexS >= 0 && resB->fSize > indexS) {
        ವಿಕಲ್ಪ(RES_GET_TYPE(resB->fRes)) {
        ಪಕ್ಷ URES_STRING:
        ಪಕ್ಷ URES_STRING_V2:
            ಪ್ರತ್ಯಾಗಮ res_getString({resB}, &resB->getResData(), resB->fRes, len);
        ಪಕ್ಷ URES_TABLE:
        ಪಕ್ಷ URES_TABLE16:
        ಪಕ್ಷ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &ಕೀಲಿ);
            ಯದಿ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಪ್ರತ್ಯಾಗಮ ures_getStringWithAlias(resB, r, indexS, len, status);
        ಪಕ್ಷ URES_ARRAY:
        ಪಕ್ಷ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            ಯದಿ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ಪ್ರತ್ಯಾಗಮ ures_getStringWithAlias(resB, r, indexS, len, status);
        ಪಕ್ಷ URES_ALIAS:
            ಪ್ರತ್ಯಾಗಮ ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        ಪಕ್ಷ URES_INT:
        ಪಕ್ಷ URES_ದ್ವಿಮಾನ:
        ಪಕ್ಷ URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          ಭಙ್ಗ;
        }
    } ಅನ್ಯಥಾ {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_getUTF8StringByIndex(ಧ್ರುವ UResourceBundle* resB,
                          int32_t idx,
                          ವರ್ಣ* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    ಧ್ರುವ char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    ಪ್ರತ್ಯಾಗಮ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(ಧ್ರುವ ವರ್ಣ* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  ವರ್ಣ *packageName = nullptr;
  ವರ್ಣ *pathToResource = nullptr, *save = nullptr;
  ವರ್ಣ *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  ಯದಿ(status == nullptr || U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (ವರ್ಣ *)uprv_malloc(length*ಪರಿಮಾಣ(ವರ್ಣ));
  /* test for nullptr */
  ಯದಿ(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  ಯದಿ(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    ಯದಿ(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } ಅನ್ಯಥಾ {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  ಯದಿ(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  ಯದಿ(U_SUCCESS(*status)) {
    ಯದಿ(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } ಅನ್ಯಥಾ {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(ಧ್ರುವ UResourceBundle* resB,
                     ವರ್ಣ* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ;

  ಯದಿ(status == nullptr || U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  ಕುರು {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &ಕೀಲಿ); 
    ಯದಿ(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, ಕೀಲಿ, -1, resB, fillIn, status);
        resB = result;
    } ಅನ್ಯಥಾ {
        *status = U_MISSING_RESOURCE_ERROR;
        ಭಙ್ಗ;
    }
  } ಯಾವತ್(*path); /* there is more stuff in the path */

  ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
ures_get_String_By_ಕೀಲಿ_With_Fallback(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND,
                                ಧ್ರುವ ವರ್ಣ* in_ಕೀಲಿ,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle ರಾಶಿ_2;
    ಧ್ರುವ char16_t* retVal = nullptr;
    ures_init_ರಾಶಿ_Object(&ರಾಶಿ_2);
    ures_get_By_ಕೀಲಿ_With_Fallback(resB, in_ಕೀಲಿ, &ರಾಶಿ_2, status);
    int32_t length;
    retVal = ures_getString(&ರಾಶಿ_2, &length, status);
    ures_close(&ರಾಶಿ_2);
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    ಯದಿ (len != nullptr) {
        *len = length;
    }
    ಪ್ರತ್ಯಾಗಮ retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ಸ್ಥಿರ Resource get_Table_Item_By_ಕೀಲಿ_Path(ಧ್ರುವ ResourceData *pResData, Resource table, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(ಕೀಲಿ, errorCode);
  ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ RES_BOGUS; }
  ವರ್ಣ *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  ಯಾವತ್ (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    ವರ್ಣ *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    ಯದಿ (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } ಅನ್ಯಥಾ {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    ಧ್ರುವ ವರ್ಣ *pathP = pathPart;
    resource = res_get_Table_Item_By_ಕೀಲಿ(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  ಯದಿ (*pathPart) {
    ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
  }
  ಪ್ರತ್ಯಾಗಮ resource;
}

ಸ್ಥಿರ ಶೂನ್ಯ createPath(ಧ್ರುವ ವರ್ಣ* origResPath,
                       int32_t     origResPathLen,
                       ಧ್ರುವ ವರ್ಣ* resPath,
                       int32_t     resPathLen,
                       ಧ್ರುವ ವರ್ಣ* in_ಕೀಲಿ,
                       CharString& path,
                       UErrorCode* status) {
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
    path.clear();
    ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ = in_ಕೀಲಿ;
    ಯದಿ (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        ಯದಿ (U_SUCCESS(*status)) {
            ಧ್ರುವ ವರ್ಣ* resPathLimit = resPath + resPathLen;
            ಧ್ರುವ ವರ್ಣ* origResPathLimit = origResPath + origResPathLen;
            ಧ್ರುವ ವರ್ಣ* resPathPtr = resPath;
            ಧ್ರುವ ವರ್ಣ* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            ಯಾವತ್ (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                ಯಾವತ್ (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                ಯದಿ (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                ಯಾವತ್ (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ಯದಿ (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            ಯಾವತ್ (resPathPtr < resPathLimit && *ಕೀಲಿ != '\0') {
                ಯಾವತ್ (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ಯದಿ (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ಯಾವತ್ (*ಕೀಲಿ != '\0' && *ಕೀಲಿ != RES_PATH_SEPARATOR) {
                    ++ಕೀಲಿ;
                }
                ಯದಿ (*ಕೀಲಿ == RES_PATH_SEPARATOR) {
                    ++ಕೀಲಿ;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(ಕೀಲಿ, *status);
    } ಅನ್ಯಥಾ {
        path.append(in_ಕೀಲಿ, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ಕೀಲಿ_With_Fallback(ಧ್ರುವ UResourceBundle* resB,
                          ಧ್ರುವ ವರ್ಣ* in_ಕೀಲಿ,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, ಬೇರು_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ fillIn;
    }
    ಯದಿ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ಯದಿ(URES_IS_TABLE(type)) {
        ಧ್ರುವ ವರ್ಣ* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_ಕೀಲಿ_Path(&resB->getResData(), resB->fRes, in_ಕೀಲಿ);
        ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ = in_ಕೀಲಿ;
        ತಾರ್ಕಿಕ did_ಬೇರು_Once = ಅಸತ್ಯ;
        ಯದಿ(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            ವರ್ಣ *myPath = nullptr;
            ಧ್ರುವ ವರ್ಣ* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            ಯಾವತ್(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_ಬೇರು_Once)) { /* Otherwise, we'll look in parents */
                ಯದಿ (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } ಅನ್ಯಥಾ {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_ಬೇರು_Once = ಸತ್ಯ;
                }
                ಬೇರು_Res = dataEntry->fData.ಬೇರು_Res;

                ಯದಿ(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_ಕೀಲಿ, path, status);
                    ಯದಿ (U_FAILURE(*status)) {
                        ures_close(helper);
                        ಪ್ರತ್ಯಾಗಮ fillIn;
                    }
                    myPath = path.data();
                    ಕೀಲಿ = in_ಕೀಲಿ;
                    ಕುರು {
                        res = res_findResource(&(dataEntry->fData), ಬೇರು_Res, &myPath, &ಕೀಲಿ);
                        ಯದಿ (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            ಯದಿ(helper) {
                              dataEntry = helper->fData;
                              ಬೇರು_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } ಅನ್ಯಥಾ {
                              ಭಙ್ಗ;
                            }
                        } ಅನ್ಯಥಾ ಯದಿ (res == RES_BOGUS) {
                            ಭಙ್ಗ;
                        }
                    } ಯಾವತ್(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            ಯದಿ(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                ಯದಿ(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_ಬೇರು_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } ಅನ್ಯಥಾ {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, ಕೀಲಿ, -1, resB, fillIn, status);
                ಯದಿ (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_ಕೀಲಿ, path, status);
                } ಅನ್ಯಥಾ {
                    ಧ್ರುವ ವರ್ಣ* separator = nullptr;
                    ಯದಿ (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    ಯದಿ (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_ಕೀಲಿ, path, status);
                    } ಅನ್ಯಥಾ {
                        createPath(origResPath, origResPathLen, "", 0, in_ಕೀಲಿ, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                ಯದಿ(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } ಅನ್ಯಥಾ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ಅನ್ಯಥಾ {
            fillIn = init_resb_result(resB->fData, res, ಕೀಲಿ, -1, resB, fillIn, status);
        }
    } 
    ಅನ್ಯಥಾ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    ಪ್ರತ್ಯಾಗಮ fillIn;
}

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ get_All_ವಸ್ತುಗಳು_With_Fallback(
        ಧ್ರುವ UResourceBundle *bundle, Resource_Data_ಮೌಲ್ಯ &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
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
    value.setData(bundle->getResData());
    value.setValidLocaleDataEntry(bundle->fValidLocaleDataEntry);
    UResourceDataEntry *parentEntry = bundle->fData->fParent;
    UBool hasParent = parentEntry != nullptr && U_SUCCESS(parentEntry->fBogus);
    value.setResource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_ಕೀಲಿ, value, !hasParent, errorCode);
    ಯದಿ (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ರಾಶಿ_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_ತುದಿ_Level = ಸತ್ಯ;
        parentRef.fRes = parentRef.getResData().ಬೇರು_Res;
        parentRef.fSize = res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        ರಾಶಿ_UResource_Bundle containerBundle;
        ಧ್ರುವ UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        ಯದಿ (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } ಅನ್ಯಥಾ {
            rb = ures_get_By_ಕೀಲಿ_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        ಯದಿ (U_SUCCESS(pathErrorCode)) {
            get_All_ವಸ್ತುಗಳು_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

ಸಂರಚನಾ GetAllChildrenSink : ಸಾರ್ವಜನಿಕ ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    ಆಭಾಸ ~GetAllChildrenSink() override;
    ಆಭಾಸ ಶೂನ್ಯ put(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value, UBool is_ಬೇರು,
           UErrorCode &errorCode) override {
        ResourceTable ವಸ್ತುಗಳು_Table = value.getTable(errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        ಪ್ರತಿ (int32_t i = 0; ವಸ್ತುಗಳು_Table.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(i, ಕೀಲಿ, value); ++i) {
            ಯದಿ (value.getType() == URES_ALIAS) {
                Resource_Data_ಮೌಲ್ಯ& rdv = static_cast<Resource_Data_ಮೌಲ್ಯ&>(value);
                ರಾಶಿ_UResource_Bundle ರಾಶಿ_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          ರಾಶಿ_Temp_Bundle.getAlias(), &errorCode);
                ಯದಿ (U_SUCCESS(errorCode)) {
                    Resource_Data_ಮೌಲ್ಯ aliased_ಮೌಲ್ಯ;
                    aliased_ಮೌಲ್ಯ.setData(aliasRB->getResData());
                    aliased_ಮೌಲ್ಯ.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_ಮೌಲ್ಯ.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    ಯದಿ (aliased_ಮೌಲ್ಯ.getType() != URES_TABLE) {
                        dest.put(ಕೀಲಿ, aliased_ಮೌಲ್ಯ, is_ಬೇರು, errorCode);
                    } ಅನ್ಯಥಾ {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_ಮೌಲ್ಯ_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        ಧ್ರುವ ವರ್ಣ* parent_ಕೀಲಿ = ಕೀಲಿ; // dest.put() changes the key
                        dest.put(parent_ಕೀಲಿ, aliased_ಮೌಲ್ಯ, is_ಬೇರು, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        ಯಾವತ್ (aliased_ಮೌಲ್ಯ_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            ವರ್ಣ* localPathAsCharPtr = localPath.data();
                            ಧ್ರುವ ವರ್ಣ* child_ಕೀಲಿ;
                            entry = entry->fParent;
                            res = entry->fData.ಬೇರು_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_ಕೀಲಿ);
                            ಯದಿ (newRes != RES_BOGUS) {
                                aliased_ಮೌಲ್ಯ.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_ಮೌಲ್ಯ.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_ಮೌಲ್ಯ_Type = aliased_ಮೌಲ್ಯ.getType();
                                ಯದಿ (aliased_ಮೌಲ್ಯ_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_ಮೌಲ್ಯ& rdv2 = static_cast<Resource_Data_ಮೌಲ್ಯ&>(aliased_ಮೌಲ್ಯ);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             ರಾಶಿ_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_ಮೌಲ್ಯ.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_ಮೌಲ್ಯ.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_ಮೌಲ್ಯ_Type = aliased_ಮೌಲ್ಯ.getType();
                                }
                                ಯದಿ (aliased_ಮೌಲ್ಯ_Type == URES_TABLE) {
                                    dest.put(parent_ಕೀಲಿ, aliased_ಮೌಲ್ಯ, is_ಬೇರು, errorCode);
                                } ಅನ್ಯಥಾ {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    ಪ್ರತ್ಯಾಗಮ;
                                }
                            }
                        }
                    }
                }
            } ಅನ್ಯಥಾ {
                dest.put(ಕೀಲಿ, value, is_ಬೇರು, errorCode);
            }
            ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_getAllChildrenWithFallback(ಧ್ರುವ UResourceBundle *bundle, ಧ್ರುವ ವರ್ಣ *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_ವಸ್ತುಗಳು_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
ures_get_ಮೌಲ್ಯ_With_Fallback(ಧ್ರುವ UResourceBundle *bundle, ಧ್ರುವ ವರ್ಣ *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_ಮೌಲ್ಯ &value, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಧ್ರುವ UResourceBundle *rb;
    ಯದಿ (*path == 0) {
        // empty path
        rb = bundle;
    } ಅನ್ಯಥಾ {
        rb = ures_get_By_ಕೀಲಿ_With_Fallback(bundle, path, tempFillIn, &errorCode);
        ಯದಿ (U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_get_All_ವಸ್ತುಗಳು_With_Fallback(ಧ್ರುವ UResourceBundle *bundle, ಧ್ರುವ ವರ್ಣ *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ರಾಶಿ_UResource_Bundle ರಾಶಿ_Bundle;
    ಧ್ರುವ UResourceBundle *rb;
    ಯದಿ (*path == 0) {
        // empty path
        rb = bundle;
    } ಅನ್ಯಥಾ {
        rb = ures_get_By_ಕೀಲಿ_With_Fallback(bundle, path, ರಾಶಿ_Bundle.getAlias(), &errorCode);
        ಯದಿ (U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    // Get all table items with fallback.
    Resource_Data_ಮೌಲ್ಯ value;
    get_All_ವಸ್ತುಗಳು_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ಕೀಲಿ(ಧ್ರುವ UResourceBundle* resB,
              ಧ್ರುವ ವರ್ಣ* in_ಕೀಲಿ,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ = in_ಕೀಲಿ;

    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ fillIn;
    }
    ಯದಿ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ಯದಿ(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_ಕೀಲಿ(&resB->getResData(), resB->fRes, &t, &ಕೀಲಿ);
        ಯದಿ(res == RES_BOGUS) {
            ಕೀಲಿ = in_ಕೀಲಿ;
            ಯದಿ(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &ಕೀಲಿ, &res, status);
                ಯದಿ(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    ಪ್ರತ್ಯಾಗಮ init_resb_result(dataEntry, res, ಕೀಲಿ, -1, resB, fillIn, status);
                } ಅನ್ಯಥಾ {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } ಅನ್ಯಥಾ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ init_resb_result(resB->fData, res, ಕೀಲಿ, -1, resB, fillIn, status);
        }
    } 
#ಯದಿ 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    ಅನ್ಯಥಾ ಯದಿ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ಸತ್ಯ) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &ಕೀಲಿ, &res, status);
        ಯದಿ(U_SUCCESS(*status)) {
            ಪ್ರತ್ಯಾಗಮ init_resb_result(dataEntry, res, ಕೀಲಿ, resB, fillIn, status);
        } ಅನ್ಯಥಾ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    ಅನ್ಯಥಾ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ fillIn;
}

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
ures_get_String_By_ಕೀಲಿ(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND,
                    ಧ್ರುವ ವರ್ಣ* in_ಕೀಲಿ,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ = in_ಕೀಲಿ;

    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ಯದಿ(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_ಕೀಲಿ(&resB->getResData(), resB->fRes, &t, &ಕೀಲಿ);

        ಯದಿ(res == RES_BOGUS) {
            ಕೀಲಿ = in_ಕೀಲಿ;
            ಯದಿ(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &ಕೀಲಿ, &res, status);
                ಯದಿ(U_SUCCESS(*status)) {
                    ವಿಕಲ್ಪ (RES_GET_TYPE(res)) {
                    ಪಕ್ಷ URES_STRING:
                    ಪಕ್ಷ URES_STRING_V2:
                        ಪ್ರತ್ಯಾಗಮ res_getString({resB, ಕೀಲಿ}, &dataEntry->fData, res, len);
                    ಪಕ್ಷ URES_ALIAS:
                      {
                        ಧ್ರುವ char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_ಕೀಲಿ(resB, in_ಕೀಲಿ, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        ಪ್ರತ್ಯಾಗಮ result;
                      }
                    ಸ್ವಭಾವ:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } ಅನ್ಯಥಾ {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } ಅನ್ಯಥಾ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ಅನ್ಯಥಾ {
            ವಿಕಲ್ಪ (RES_GET_TYPE(res)) {
            ಪಕ್ಷ URES_STRING:
            ಪಕ್ಷ URES_STRING_V2:
                ಪ್ರತ್ಯಾಗಮ res_getString({resB, ಕೀಲಿ}, &resB->getResData(), res, len);
            ಪಕ್ಷ URES_ALIAS:
              {
                ಧ್ರುವ char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_ಕೀಲಿ(resB, in_ಕೀಲಿ, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                ಪ್ರತ್ಯಾಗಮ result;
              }
            ಸ್ವಭಾವ:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#ಯದಿ 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    ಅನ್ಯಥಾ ಯದಿ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ಸತ್ಯ) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &ಕೀಲಿ, &res, status);
        ಯದಿ(U_SUCCESS(*status)) {
            // TODO: Tracing
            ಪ್ರತ್ಯಾಗಮ res_getString(rd, res, len);
        } ಅನ್ಯಥಾ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    ಅನ್ಯಥಾ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_get_UTF8_String_By_ಕೀಲಿ(ಧ್ರುವ UResourceBundle* resB,
                        ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                        ವರ್ಣ* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    ಧ್ರುವ char16_t *s16 = ures_get_String_By_ಕೀಲಿ(resB, ಕೀಲಿ, &length16, status);
    ಪ್ರತ್ಯಾಗಮ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_getLocaleInternal(ಧ್ರುವ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    } ಅನ್ಯಥಾ {
      ಪ್ರತ್ಯಾಗಮ resourceBundle->fData->fName;
    }
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_getLocale(ಧ್ರುವ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  ಪ್ರತ್ಯಾಗಮ ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_getLocaleByType(ಧ್ರುವ UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    } ಅನ್ಯಥಾ {
        ವಿಕಲ್ಪ(type) {
        ಪಕ್ಷ ULOC_ACTUAL_LOCALE:
            ಪ್ರತ್ಯಾಗಮ resourceBundle->fData->fName;
        ಪಕ್ಷ ULOC_VALID_LOCALE:
            ಪ್ರತ್ಯಾಗಮ resourceBundle->fValidLocaleDataEntry->fName;
        ಪಕ್ಷ ULOC_REQUESTED_LOCALE:
        ಸ್ವಭಾವ:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    }
}

U_CFUNC ಧ್ರುವ ವರ್ಣ*
ures_getName(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND) {
  ಯದಿ(resB == nullptr) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  ಪ್ರತ್ಯಾಗಮ resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC ಧ್ರುವ ವರ್ಣ*
ures_getPath(ಧ್ರುವ UResourceBundle* resB U_LIFETIME_BOUND) {
  ಯದಿ(resB == nullptr) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  ಪ್ರತ್ಯಾಗಮ resB->fData->fPath;
}
#endif

ಸ್ಥಿರ UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ಧ್ರುವ ವರ್ಣ* path,
                  ಧ್ರುವ ವರ್ಣ* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    ಯದಿ(U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    UResourceDataEntry *entry;
    ಯದಿ(openType != URES_OPEN_DIRECT) {
        ಯದಿ (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        ಯದಿ(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } ಅನ್ಯಥಾ {
        entry = entryOpenDirect(path, localeID, status);
    }
    ಯದಿ(U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    UBool is_ರಾಶಿ_Object;
    ಯದಿ(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(ಪರಿಮಾಣ(UResourceBundle)));
        ಯದಿ(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        is_ರಾಶಿ_Object = ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ {  // fill-in
        is_ರಾಶಿ_Object = ures_is_ರಾಶಿ_Object(r);
        ures_closeBundle(r, ಅಸತ್ಯ);
    }
    uprv_memset(r, 0, ಪರಿಮಾಣ(UResourceBundle));
    ures_set_Is_ರಾಶಿ_Object(r, is_ರಾಶಿ_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_ತುದಿ_Level = ಸತ್ಯ;
    r->fRes = r->getResData().ಬೇರು_Res;
    r->fSize = res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    ಪ್ರತ್ಯಾಗಮ r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* localeID, UErrorCode* status) {
    ಪ್ರತ್ಯಾಗಮ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_ಬೇರು, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* localeID, UErrorCode* status) {
    ಪ್ರತ್ಯಾಗಮ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_ಬೇರು, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* localeID, UErrorCode* status) {
    ಪ್ರತ್ಯಾಗಮ ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_openFillIn(UResourceBundle *r, ಧ್ರುವ ವರ್ಣ* path,
                ಧ್ರುವ ವರ್ಣ* localeID, UErrorCode* status) {
    ಯದಿ(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_ಬೇರು, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* localeID, UErrorCode* status) {
    ಯದಿ(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(ಧ್ರುವ UResourceBundle* resourceBundle,
                  ಧ್ರುವ ವರ್ಣ* resource_ಕೀಲಿ,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_ರಾಶಿ_Object(&resData);
    ಯದಿ (status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ures_get_By_ಕೀಲಿ(resourceBundle, resource_ಕೀಲಿ, &resData, status);
    
    ಯದಿ(resData.getResData().data != nullptr) {
        int32_t result = res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        ಪ್ರತ್ಯಾಗಮ result;
    } ಅನ್ಯಥಾ {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        ಪ್ರತ್ಯಾಗಮ 0;
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
U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_get_Version_ಸಂಖ್ಯೆ_Internal(ಧ್ರುವ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ಯದಿ (!resourceBundle) ಪ್ರತ್ಯಾಗಮ nullptr;

    ಯದಿ(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ಧ್ರುವ char16_t* minor_version = ures_get_String_By_ಕೀಲಿ(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (ವರ್ಣ *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        ಯದಿ (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
       
        ಯದಿ(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        ಅನ್ಯಥಾ {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    ಪ್ರತ್ಯಾಗಮ resourceBundle->fVersion;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ures_get_Version_ಸಂಖ್ಯೆ(ಧ್ರುವ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ಪ್ರತ್ಯಾಗಮ ures_get_Version_ಸಂಖ್ಯೆ_Internal(resourceBundle);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ures_getVersion(ಧ್ರುವ UResourceBundle* resB, UVersionInfo versionInfo) {
    ಯದಿ (!resB) ಪ್ರತ್ಯಾಗಮ;

    u_versionFromString(versionInfo, ures_get_Version_ಸಂಖ್ಯೆ_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
#ಅನ್ತರ್ಭಾವ <stdio.h>
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ULocalesContext {
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
ures_loc_ಎಣಿಕೆ_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    ಪ್ರತ್ಯಾಗಮ ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    ಧ್ರುವ ವರ್ಣ *result = nullptr;
    int32_t len = 0;
    ಯದಿ (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_ಕೀಲಿ(k);
        len = (int32_t)uprv_strlen(result);
    }
    ಯದಿ (resultLength) {
        *resultLength = len;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

ಸ್ಥಿರ ಧ್ರುವ UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_ಎಣಿಕೆ_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(ಧ್ರುವ ವರ್ಣ *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    ಯದಿ(U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(ಪರಿಮಾಣ(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(ಪರಿಮಾಣ(UEnumeration));
    ಯದಿ(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, ಪರಿಮಾಣ(UEnumeration));

    ures_init_ರಾಶಿ_Object(&myContext->installed);
    ures_init_ರಾಶಿ_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_ಕೀಲಿ(idx, INDEX_TAG, &myContext->installed, status);
    ಯದಿ(U_SUCCESS(*status)) {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_ಕೀಲಿ(&myContext->installed));
#endif
        en->context = myContext;
    } ಅನ್ಯಥಾ {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    ಪ್ರತ್ಯಾಗಮ en;
}

ಸ್ಥಿರ UBool isLocaleInList(UEnumeration *locEnum, ಧ್ರುವ ವರ್ಣ *loc_To_ಹುಡುಕು, UErrorCode *status) {
    ಧ್ರುವ ವರ್ಣ *loc;
    ಯಾವತ್ ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        ಯದಿ (uprv_strcmp(loc, loc_To_ಹುಡುಕು) == 0) {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ getParentForFunctionalEquivalent(ಧ್ರುವ ವರ್ಣ*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    ಯದಿ (res != nullptr) {
        ures_get_By_ಕೀಲಿ(res, "%%Parent", bund1, &subStatus);
        ಯದಿ (U_SUCCESS(subStatus)) {
            int32_t length16;
            ಧ್ರುವ char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    ಯದಿ (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(ವರ್ಣ *result, int32_t resultCapacity,
                             ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *resName, ಧ್ರುವ ವರ್ಣ *keyword, ಧ್ರುವ ವರ್ಣ *locid,
                             UBool *isAvailable, UBool omitDefault, UErrorCode *status)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString found;
    CharString parent;
    CharString full;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = nullptr;
    UErrorCode subStatus = U_ZERO_ERROR;
    int32_t length = 0;
    ಯದಿ(U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ 0;
    CharString kwVal;
    ಯದಿ (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_ಮೌಲ್ಯ(locid, keyword, subStatus);
        ಯದಿ (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    ಯದಿ (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_ರಾಶಿ_Object(&bund1);
    ures_init_ರಾಶಿ_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    ಯದಿ(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = ಸತ್ಯ;
        ಯದಿ (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    ಯದಿ(U_FAILURE(subStatus)) {
        *status = subStatus;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    
    ಕುರು {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        ಯದಿ(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = ಅಸತ್ಯ;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        ಯದಿ(U_FAILURE(subStatus)) {
            *status = subStatus;
        } ಅನ್ಯಥಾ ಯದಿ(subStatus == U_ZERO_ERROR) {
            ures_get_By_ಕೀಲಿ(res,resName,&bund1, &subStatus);
            ಯದಿ(subStatus == U_ZERO_ERROR) {
                ಧ್ರುವ char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_ಕೀಲಿ(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                ಯದಿ(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    ಯದಿ(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        ಯದಿ (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        ಯದಿ (found != parent) {
            parent.copyFrom(found, subStatus);
        } ಅನ್ಯಥಾ {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } ಯಾವತ್(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    ಕುರು {
        res = ures_open(path, parent.data(), &subStatus);
        ಯದಿ((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = ಅಸತ್ಯ;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        ಯದಿ(U_FAILURE(subStatus)) {
            *status = subStatus;
        } ಅನ್ಯಥಾ ಯದಿ(subStatus == U_ZERO_ERROR) {
            ures_get_By_ಕೀಲಿ(res,resName,&bund1, &subStatus);
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            ಯದಿ(subStatus == U_ZERO_ERROR) {
                ures_get_By_ಕೀಲಿ(&bund1, kwVal.data(), &bund2, &subStatus);
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                ಯದಿ(subStatus == U_ZERO_ERROR) {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    ಯದಿ (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } ಅನ್ಯಥಾ {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        ಯದಿ(defLoc.length() > full.length()) {
                          ಧ್ರುವ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_ಕೀಲಿ(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          ಯದಿ(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                        ಅನ್ಯಥಾ {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } ಅನ್ಯಥಾ {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = ಅಸತ್ಯ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        ಯದಿ (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            ಧ್ರುವ ವರ್ಣ *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            ಯದಿ (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                ಯದಿ (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = ಸತ್ಯ;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        ಯದಿ (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } ಯಾವತ್(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    ಯದಿ(full.isEmpty() && kwVal != defVal) {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        ಕುರು { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            ಯದಿ((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = ಅಸತ್ಯ;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            ಯದಿ(U_FAILURE(subStatus)) {
                *status = subStatus;
            } ಅನ್ಯಥಾ ಯದಿ(subStatus == U_ZERO_ERROR) {
                ures_get_By_ಕೀಲಿ(res,resName,&bund1, &subStatus);
                ಯದಿ(subStatus == U_ZERO_ERROR) {
                    ures_get_By_ಕೀಲಿ(&bund1, kwVal.data(), &bund2, &subStatus);
                    ಯದಿ(subStatus == U_ZERO_ERROR) {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        ಯದಿ (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } ಅನ್ಯಥಾ {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        ಯದಿ(defLoc.length() > full.length()) {
                          ಧ್ರುವ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_ಕೀಲಿ(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          ಯದಿ(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                        ಅನ್ಯಥಾ {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                    }
                }
            }
            
            subStatus = U_ZERO_ERROR;
            found.copyFrom(parent, subStatus);
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
            ures_close(res);
            subStatus = U_ZERO_ERROR;
        } ಯಾವತ್(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    ಯದಿ(U_SUCCESS(*status)) {
        ಯದಿ(full.isEmpty()) {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } ಅನ್ಯಥಾ ಯದಿ(omitDefault) {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          ಯದಿ(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            ಯದಿ(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        ಯದಿ(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } ಅನ್ಯಥಾ ಯದಿ(!omitDefault) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(defVal, subStatus);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = found.length();

    ಯದಿ(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        ಯದಿ(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        ಯದಿ(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } ಅನ್ಯಥಾ {
        length = 0;
        result[0]=0;
    }
    ಪ್ರತ್ಯಾಗಮ u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_ಮೌಲ್ಯಗಳು(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *keyword, UErrorCode *status)
{
#define ಮೌಲ್ಯಗಳು_BUF_SIZE 2048
#define ಮೌಲ್ಯಗಳು_LIST_SIZE 512
    
    ವರ್ಣ       ಮೌಲ್ಯಗಳು_Buf[ಮೌಲ್ಯಗಳು_BUF_SIZE];
    int32_t    ಮೌಲ್ಯಗಳು_Index = 0;
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯಗಳು_List[ಮೌಲ್ಯಗಳು_LIST_SIZE];
    int32_t    ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ = 0;
    
    ಧ್ರುವ ವರ್ಣ *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_ರಾಶಿ_Object(&item);
    ures_init_ರಾಶಿ_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    ಯದಿ(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    
    ಮೌಲ್ಯಗಳು_Buf[0]=0;
    ಮೌಲ್ಯಗಳು_Buf[1]=0;

    ಯಾವತ್ ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
        ಯದಿ(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_ಕೀಲಿ(bund, keyword, &item, &subStatus);
        
        ಯದಿ(!bund || U_FAILURE(subStatus)) {
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            ಅನುವರ್ತ;
        }

        ಯಾವತ್ ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            ಧ್ರುವ ವರ್ಣ *k;
            int32_t i;
            k = ures_get_ಕೀಲಿ(subPtr);

#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            ಯದಿ(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                ಅನುವರ್ತ;
            }
            ಪ್ರತಿ(i=0; i<ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ; i++) {
                ಯದಿ(!uprv_strcmp(ಮೌಲ್ಯಗಳು_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    ಭಙ್ಗ;
                }
            }
            ಯದಿ(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                ಯದಿ((ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ >= (ಮೌಲ್ಯಗಳು_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((ಮೌಲ್ಯಗಳು_Index+kLen+1+1) >= ಮೌಲ್ಯಗಳು_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } ಅನ್ಯಥಾ {
                    uprv_strcpy(ಮೌಲ್ಯಗಳು_Buf+ಮೌಲ್ಯಗಳು_Index, k);
                    ಮೌಲ್ಯಗಳು_List[ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ++] = ಮೌಲ್ಯಗಳು_Buf+ಮೌಲ್ಯಗಳು_Index;
                    ಮೌಲ್ಯಗಳು_Index += kLen;
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    ಮೌಲ್ಯಗಳು_Buf[ಮೌಲ್ಯಗಳು_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    ಮೌಲ್ಯಗಳು_Buf[ಮೌಲ್ಯಗಳು_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#ಯದಿ defined(URES_ವೃಕ್ಷ_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        ಮೌಲ್ಯಗಳು_Index, ಮೌಲ್ಯಗಳು_ಎಣಿಕೆ);
#endif
    ಪ್ರತ್ಯಾಗಮ uloc_openKeywordList(ಮೌಲ್ಯಗಳು_Buf, ಮೌಲ್ಯಗಳು_Index, status);
}
#ಯದಿ 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ಧ್ರುವ UResourceBundle* res1, ಧ್ರುವ UResourceBundle* res2){
    ಯದಿ(res1==nullptr || res2==nullptr){
        ಪ್ರತ್ಯಾಗಮ res1==res2; /* pointer comparison */
    }
    ಯದಿ(res1->f_ಕೀಲಿ==nullptr||  res2->f_ಕೀಲಿ==nullptr){
        ಪ್ರತ್ಯಾಗಮ (res1->f_ಕೀಲಿ==res2->f_ಕೀಲಿ);
    }ಅನ್ಯಥಾ{
        ಯದಿ(uprv_strcmp(res1->f_ಕೀಲಿ, res2->f_ಕೀಲಿ)!=0){
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಯದಿ(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        ಪ್ರತ್ಯಾಗಮ (res1->fData->fPath == res2->fData->fPath);
    }ಅನ್ಯಥಾ{
        ಯದಿ(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಯದಿ(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(res1->fRes != res2->fRes){
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ಧ್ರುವ UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    ಯದಿ(U_FAILURE(*status) || res == nullptr){
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    ಯದಿ(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }ಅನ್ಯಥಾ{
        ret = bundle;
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}
U_CAPI ಧ್ರುವ UResourceBundle* U_EXPORT2
ures_getParentBundle(ಧ್ರುವ UResourceBundle* res){
    ಯದಿ(res==nullptr){
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ res->fParentRes;
}
#endif

U_CAPI ಶೂನ್ಯ U_EXPORT2
ures_get_Version_By_ಕೀಲಿ(ಧ್ರುವ UResourceBundle* res, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, UVersionInfo ver, UErrorCode *status) {
  ಧ್ರುವ char16_t *str;
  int32_t len;
  str = ures_get_String_By_ಕೀಲಿ(res, ಕೀಲಿ, &len, status);
  ಯದಿ(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
