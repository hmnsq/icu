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

#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"
#ଅନ୍ତର୍ଭାଵ "cwchar.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uenum.h"
#ଅନ୍ତର୍ଭାଵ "uenumimp.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "uresdata.h"

using ନାମାକାଶ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ସ୍ଥିର UHashtable *cache = nullptr;
ସ୍ଥିର icu::UInitOnce gCacheInitOnce {};

ସ୍ଥିର UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ସ୍ଥିର int32_t U_CALLCONV hashEntry(ଅଚଳ UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    ଫେରନ୍ତୁ uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ସ୍ଥିର UBool U_CALLCONV compareEntries(ଅଚଳ UHashTok p1, ଅଚଳ UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    ଫେରନ୍ତୁ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ସ୍ଥିର UBool chopLocale(ଅକ୍ଷର *name) {
    ଅକ୍ଷର *i = uprv_strrchr(name, '_');

    ଯଦି(i != nullptr) {
        *i = '\0';
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ସ୍ଥିର UBool hasVariant(ଅଚଳ ଅକ୍ଷର* localeID) {
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
    ଫେରନ୍ତୁ sink.ସଂଖ୍ୟା_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#ଅନ୍ତର୍ଭାଵ "localefallback_data.h"

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* performFallbackLookup(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                         ଅଚଳ ଅକ୍ଷର* ଚାବି_Strs,
                                         ଅଚଳ ଅକ୍ଷର* ମୂଲ୍ୟ_Strs U_LIFETIME_BOUND,
                                         ଅଚଳ int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    ଅଚଳ int32_t* bottom = lookupTable;
    ଅଚଳ int32_t* ଶୀର୍ଷ = lookupTable + lookupTableLength;

    ଯେତେବେଳେ (bottom < ଶୀର୍ଷ) {
        // Effectively, divide by 2 and round down to an even index
        ଅଚଳ int32_t* ମଧ୍ୟ = bottom + (((ଶୀର୍ଷ - bottom) / 4) * 2);
        ଅଚଳ ଅକ୍ଷର* entry_ଚାବି = &(ଚାବି_Strs[*ମଧ୍ୟ]);
        int32_t strcmpResult = uprv_strcmp(ଚାବି, entry_ଚାବି);
        ଯଦି (strcmpResult == 0) {
            ଫେରନ୍ତୁ &(ମୂଲ୍ୟ_Strs[ମଧ୍ୟ[1]]);
        } ଅନ୍ୟଥା ଯଦି (strcmpResult < 0) {
            ଶୀର୍ଷ = ମଧ୍ୟ;
        } ଅନ୍ୟଥା {
            bottom = ମଧ୍ୟ + 2;
        }
    }
    ଫେରନ୍ତୁ nullptr;
}

ସ୍ଥିର CharString getDefaultScript(ଅଚଳ CharString& language, ଅଚଳ CharString& region) {
    ଅଚଳ ଅକ୍ଷର* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    ଯଦି (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        ଯଦି (U_FAILURE(err)) {
            ଫେରନ୍ତୁ result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    ଯଦି (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    ଯଦି (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    ଫେରନ୍ତୁ result;
}

ଗଣନା UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_ମୂଳ,
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
    URES_OPEN_LOCALE_ମୂଳ,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UResOpenType UResOpenType;

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
ସ୍ଥିର ତାର୍କିକ getParentLocaleID(ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    ଯଦି (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        ଫେରନ୍ତୁ chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    ଯଦି (U_FAILURE(err)) {
        // hopefully this never happens...
        ଫେରନ୍ତୁ chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    ଯଦି (openType == URES_OPEN_LOCALE_DEFAULT_ମୂଳ) {
        ଅଚଳ ଅକ୍ଷର* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        ଯଦି (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    ଯଦି (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        ଯଦି (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } ଅନ୍ୟଥା {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } ଅନ୍ୟଥା ଯଦି (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        ଯଦି (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } ଅନ୍ୟଥା {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } ଅନ୍ୟଥା ଯଦି (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        ଯଦି (openType != URES_OPEN_LOCALE_DEFAULT_ମୂଳ || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    } ଅନ୍ୟଥା {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        ଫେରନ୍ତୁ ସତ୍ୟ;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ସ୍ଥିର UBool mayHaveParent(ଅକ୍ଷର *name) {
    ଫେରନ୍ତୁ (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
ସ୍ଥିର ଶୂନ୍ୟ entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_ଗଣନା_Existing++;
    ଯେତେବେଳେ(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_ଗଣନା_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ସ୍ଥିର UResourceDataEntry *getFallbackData(
        ଅଚଳ UResourceBundle *resBundle,
        ଅଚଳ ଅକ୍ଷର **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    ଯଦି(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_ଚାବି(&(dataEntry->fData), dataEntry->fData.ମୂଳ_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    ଯଦି(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        ଯେତେବେଳେ(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            ଯଦି(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_ଚାବି(&(dataEntry->fData), dataEntry->fData.ମୂଳ_Res, &indexR, resTag);
            }
        }
    }

    ଯଦି(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    ଯଦି(i>1) {
        ଯଦି(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_ମୂଳ_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } ଅନ୍ୟଥା {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    ଫେରନ୍ତୁ dataEntry;
}

ସ୍ଥିର ଶୂନ୍ୟ
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    ଯଦି(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    ଯଦି(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    ଯଦି(entry->fPool != nullptr) {
        --entry->fPool->f_ଗଣନା_Existing;
    }
    alias = entry->fAlias;
    ଯଦି(alias != nullptr) {
        ଯେତେବେଳେ(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_ଗଣନା_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
ସ୍ଥିର int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_ଅଙ୍କ = 0;
    ଅଚଳ UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    ଯଦି (cache == nullptr) {
        ଫେରନ୍ତୁ 0;
    }

    କର {
        deletedMore = ମିଥ୍ୟା;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        ଯେତେବେଳେ ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            ଯଦି (resB->f_ଗଣନା_Existing == 0) {
                rb_Deleted_ଅଙ୍କ++;
                deletedMore = ସତ୍ୟ;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } ଯେତେବେଳେ(deletedMore);

    ଫେରନ୍ତୁ rb_Deleted_ଅଙ୍କ;
}

#ifdef URES_DEBUG
#ଅନ୍ତର୍ଭାଵ <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = ମିଥ୍ୟା;
  int32_t pos = UHASH_FIRST;
  ଅଚଳ UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    ଯଦି (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    ଯେତେବେଳେ ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=ସତ୍ୟ;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (ଶୂନ୍ୟ*)resB, resB->f_ଗଣନା_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (ଶୂନ୍ୟ*)resB->fPool,
              (ଶୂନ୍ୟ*)resB->fAlias,
              (ଶୂନ୍ୟ*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_ଗଣନା(cache));
    ଫେରନ୍ତୁ cacheNotEmpty;
}

#endif

ସ୍ଥିର UBool U_CALLCONV ures_cleanup()
{
    ଯଦି (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/** INTERNAL: Initializes the cache for resources */
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ସ୍ଥିର ଶୂନ୍ୟ initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ସ୍ଥିର ଶୂନ୍ୟ setEntryName(UResourceDataEntry *res, ଅଚଳ ଅକ୍ଷର *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    ଯଦି(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    ଯଦି (len < static_cast<int32_t>(ଆକାର(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    ଅନ୍ୟଥା {
        res->fName = static_cast<ଅକ୍ଷର*>(uprv_malloc(len + 1));
    }
    ଯଦି(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ଅନ୍ୟଥା {
        uprv_strcpy(res->fName, name);
    }
}

ସ୍ଥିର UResourceDataEntry *
getPoolEntry(ଅଚଳ ଅକ୍ଷର *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ସ୍ଥିର UResourceDataEntry *init_entry(ଅଚଳ ଅକ୍ଷର *localeID, ଅଚଳ ଅକ୍ଷର *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    ଅଚଳ ଅକ୍ଷର *name;
    ଅକ୍ଷର aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    ଯଦି(U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    /* here we try to deduce the right locale name */
    ଯଦି(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } ଅନ୍ୟଥା ଯଦି(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_ମୂଳ_Locale_Name;
    } ଅନ୍ୟଥା { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<ଅକ୍ଷର*>(name);
    find.fPath = const_cast<ଅକ୍ଷର*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    ଯଦି(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(ଆକାର(UResourceDataEntry)));
        ଯଦି(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }

        uprv_memset(r, 0, ଆକାର(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        ଯଦି (U_FAILURE(*status)) {
            uprv_free(r);
            ଫେରନ୍ତୁ nullptr;
        }

        ଯଦି(path != nullptr) {
            r->fPath = uprv_strdup(path);
            ଯଦି(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                ଫେରନ୍ତୁ nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        ଯଦି (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            ଯଦି (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                ଫେରନ୍ତୁ nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } ଅନ୍ୟଥା { /* if we have a regular entry */
            Resource aliasres;
            ଯଦି (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                ଯଦି (U_SUCCESS(*status)) {
                    ଅଚଳ int32_t *poolIndexes = r->fPool->fData.p_ମୂଳ + 1;
                    ଯଦି(r->fData.p_ମୂଳ[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_ଚାବିଗୁଡ଼ିକ = reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } ଅନ୍ୟଥା {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } ଅନ୍ୟଥା {
                    r->fBogus = *status;
                }
            }
            ଯଦି (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                ଯଦି (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ଅଚଳ char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    ଯଦି(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            ଯଦି ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (ଶୂନ୍ୟ *)r, r, &cacheStatus);
                ଯଦି (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } ଅନ୍ୟଥା {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    ଯଦି(r != nullptr) {
        /* return the real bundle */
        ଯେତେବେଳେ(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_ଗଣନା_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        ଯଦି(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    ଫେରନ୍ତୁ r;
}

ସ୍ଥିର UResourceDataEntry *
getPoolEntry(ଅଚଳ ଅକ୍ଷର *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    ଯଦି( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    ଫେରନ୍ତୁ poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ସ୍ଥିର UResourceDataEntry *
findFirstExisting(ଅଚଳ ଅକ୍ଷର* path, ଅକ୍ଷର* name, ଅଚଳ ଅକ୍ଷର* defaultLocale, UResOpenType openType,
                  UBool *is_ମୂଳ, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = ମିଥ୍ୟା;
    *foundParent = ସତ୍ୟ; /* we're starting with a fresh name */
    ଅକ୍ଷର origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    ଯେତେବେଳେ(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        ଯଦି (U_FAILURE(*status)) {
            ଫେରନ୍ତୁ nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        ଯଦି(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_ଗଣନା_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } ଅନ୍ୟଥା {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_ମୂଳ = static_cast<UBool>(uprv_strcmp(name, k_ମୂଳ_Locale_Name) == 0);

        /*Fallback data stuff*/
        ଯଦି (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } ଅନ୍ୟଥା {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        ଯଦି (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ଶୂନ୍ୟ ures_set_Is_ଗଦା_Object( UResourceBundle* resB, UBool state) {
    ଯଦି(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } ଅନ୍ୟଥା {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ସ୍ଥିର UBool ures_is_ଗଦା_Object(ଅଚଳ UResourceBundle* resB) {
  ଫେରନ୍ତୁ((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?ମିଥ୍ୟା:ସତ୍ୟ);
}


U_CFUNC ଶୂନ୍ୟ ures_init_ଗଦା_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, ଆକାର(UResourceBundle));
  ures_set_Is_ଗଦା_Object(resB, ସତ୍ୟ);
}

U_NAMESPACE_BEGIN

ଗଦା_UResource_Bundle::ଗଦା_UResource_Bundle() {
    ures_init_ଗଦା_Object(&bundle);
}

ଗଦା_UResource_Bundle::~ଗଦା_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

ସ୍ଥିର UBool  // returns U_SUCCESS(*status)
load_Parents_Except_ମୂଳ(UResourceDataEntry *&t1,
                      ଅକ୍ଷର name[], int32_t nameCapacity,
                      UBool usingUSRData, ଅକ୍ଷର usrDataPath[], UErrorCode *status) {
    ଯଦି (U_FAILURE(*status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    UBool checkParent = ସତ୍ୟ;
    ଯେତେବେଳେ (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        ଯଦି (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            ଅଚଳ char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            ଯଦି(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                ଯଦି (uprv_strcmp(name, k_ମୂଳ_Locale_Name) == 0) {
                    ଫେରନ୍ତୁ ସତ୍ୟ;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        ଯଦି (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        ଯଦି (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            ଯଦି (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        }

        ଯଦି (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } ଅନ୍ୟଥା {
            t1->fParent = t2;
            ଯଦି (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_ଗଣନା_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର UBool  // returns U_SUCCESS(*status)
ଭର୍ତ୍ତି_ମୂଳ_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    ଯଦି (U_FAILURE(*status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_ମୂଳ_Locale_Name, t1->fPath, &parentStatus);
    ଯଦି (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    t1->fParent = t2;
    t1 = t2;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର UResourceDataEntry *entryOpen(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = ମିଥ୍ୟା;
    UBool is_ମୂଳ = ମିଥ୍ୟା;
    UBool hasRealData = ମିଥ୍ୟା;
    UBool hasChopped = ସତ୍ୟ;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    ଅକ୍ଷର name[ULOC_FULLNAME_CAPACITY];
    ଅକ୍ଷର usrDataPath[96];

    initCache(status);

    ଯଦି(U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    uprv_strncpy(name, localeID, ଆକାର(name) - 1);
    name[ଆକାର(name) - 1] = 0;

    ଯଦି ( usingUSRData ) {
        ଯଦି ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } ଅନ୍ୟଥା {
            uprv_strncpy(usrDataPath, path, ଆକାର(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[ଆକାର(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ଅଚଳ ଅକ୍ଷର *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_ମୂଳ, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    ଯଦି (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        ଯାଅ finish;
    }

    ଯଦି(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = ସତ୍ୟ;
        ଯଦି ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            ଯଦି (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                ଯାଅ finish;
            }
            ଯଦି ( u1 != nullptr ) {
                ଯଦି(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } ଅନ୍ୟଥା {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_ଗଣନା_Existing = 0;
                }
            }
        }
        ଯଦି ((hasChopped || mayHaveParent(name)) && !is_ମୂଳ) {
            ଯଦି (!load_Parents_Except_ମୂଳ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                ଯାଅ finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    ଯଦି(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_ମୂଳ && !isDefault && !is_ମୂଳ) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_ମୂଳ, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        ଯଦି (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            ଯାଅ finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        ଯଦି(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = ସତ୍ୟ;
            isDefault = ସତ୍ୟ;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            ଯଦି ((hasChopped || mayHaveParent(name)) && !is_ମୂଳ) {
                ଯଦି (!load_Parents_Except_ମୂଳ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    ଯାଅ finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    ଯଦି(r == nullptr) {
        uprv_strcpy(name, k_ମୂଳ_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_ମୂଳ, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        ଯଦି (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            ଯାଅ finish;
        }
        ଯଦି(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = ସତ୍ୟ;
        } ଅନ୍ୟଥା { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            ଯାଅ finish;
        }
    } ଅନ୍ୟଥା ଯଦି(!is_ମୂଳ && uprv_strcmp(t1->fName, k_ମୂଳ_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        ଯଦି (!ଭର୍ତ୍ତି_ମୂଳ_Bundle(t1, status)) {
            ଯାଅ finish;
        }
        ଯଦି(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    ଯେତେବେଳେ(r != nullptr && !is_ମୂଳ && t1->fParent != nullptr) {
        t1->fParent->f_ଗଣନା_Existing++;
        t1 = t1->fParent;
    }

finish:
    ଯଦି(U_SUCCESS(*status)) {
        ଯଦି(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        ଫେରନ୍ତୁ r;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ସ୍ଥିର UResourceDataEntry *
entryOpenDirect(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* localeID, UErrorCode* status) {
    initCache(status);
    ଯଦି(U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    } ଅନ୍ୟଥା ଯଦି (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_ମୂଳ_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    ଯଦି(U_SUCCESS(*status)) {
        ଯଦି(r->fBogus != U_ZERO_ERROR) {
            r->f_ଗଣନା_Existing--;
            r = nullptr;
        }
    } ଅନ୍ୟଥା {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    ଯଦି(r != nullptr && uprv_strcmp(localeID, k_ମୂଳ_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        ଅକ୍ଷର name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        ଯଦି(!chopLocale(name) || uprv_strcmp(name, k_ମୂଳ_Locale_Name) == 0 ||
                load_Parents_Except_ମୂଳ(t1, name, UPRV_LENGTHOF(name), ମିଥ୍ୟା, nullptr, status)) {
            ଯଦି(uprv_strcmp(t1->fName, k_ମୂଳ_Locale_Name) != 0 && t1->fParent == nullptr) {
                ଭର୍ତ୍ତି_ମୂଳ_Bundle(t1, status);
            }
        }
        ଯଦି(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    ଯଦି(r != nullptr) {
        // TODO: Does this ever loop?
        ଯେତେବେଳେ(t1->fParent != nullptr) {
            t1->fParent->f_ଗଣନା_Existing++;
            t1 = t1->fParent;
        }
    }
    ଫେରନ୍ତୁ r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ସ୍ଥିର ଶୂନ୍ୟ entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    ଯେତେବେଳେ(resB != nullptr) {
        p = resB->fParent;
        resB->f_ଗଣନା_Existing--;

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

ସ୍ଥିର ଶୂନ୍ୟ entryClose(UResourceDataEntry *resB) {
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
ସ୍ଥିର ଶୂନ୍ୟ ures_appendResPath(UResourceBundle *resB, ଅଚଳ ଅକ୍ଷର* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    ଯଦି(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    ଯଦି(RES_BUFSIZE <= resB->fResPathLen+1) {
        ଯଦି(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<ଅକ୍ଷର*>(uprv_malloc((resB->fResPathLen + 1) * ଆକାର(ଅକ୍ଷର)));
            /* Check that memory was allocated correctly. */
            ଯଦି (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } ଅନ୍ୟଥା {
            ଅକ୍ଷର* temp = static_cast<ଅକ୍ଷର*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * ଆକାର(ଅକ୍ଷର)));
            /* Check that memory was reallocated correctly. */
            ଯଦି (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

ସ୍ଥିର ଶୂନ୍ୟ ures_freeResPath(UResourceBundle *resB) {
    ଯଦି (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    ଯଦି(resB != nullptr) {
        ଯଦି(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        ଯଦି(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        ଯଦି(ures_is_ଗଦା_Object(resB) == ମିଥ୍ୟା && freeBundleObj) {
            uprv_free(resB);
        }
#ଯଦି 0 /*U_DEBUG*/
        ଅନ୍ୟଥା {
            /* poison the data */
            uprv_memset(resB, -1, ଆକାର(UResourceBundle));
        }
#endif
    }
}

U_CAPI ଶୂନ୍ୟ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, ସତ୍ୟ);
}

ନାମାକାଶ {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ଅଚଳ ଅକ୍ଷର* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(ଅଚଳ ResourceData& resData,
                               Resource r,
                               ଅଚଳ ଅକ୍ଷର* ଚାବି,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               ଅଚଳ ଅକ୍ଷର* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ଯଦି (U_FAILURE(*status)) { ଫେରନ୍ତୁ resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ଅଚଳ char16_t *alias = res_getAlias(&resData, r, &len);
    ଯଦି(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ resB;
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
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    // We have an alias, now let's cut it up.
    ଅଚଳ ଅକ୍ଷର *path = nullptr, *locale = nullptr, *ଚାବି_Path = nullptr;
    ଯଦି(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        ଅକ୍ଷର *chAliasData = chAlias.data();
        ଅକ୍ଷର *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        ଯଦି(sep != nullptr) {
            *sep++ = 0;
        }
        ଯଦି(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            ଚାବି_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } ଅନ୍ୟଥା {
            ଯଦି(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            ଯଦି (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } ଅନ୍ୟଥା {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                ଯଦି(sep != nullptr) {
                    *sep++ = 0;
                }
                ଚାବି_Path = sep;
            }
        }
    } ଅନ୍ୟଥା {
        // No path, start with a locale.
        ଅକ୍ଷର *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        ଯଦି(sep != nullptr) {
            *sep++ = 0;
        }
        ଚାବି_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    ଯଦି (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } ଅନ୍ୟଥା {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        ଯଦି(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            ଫେରନ୍ତୁ resB;
        }
        dataEntry = mainRes->fData;
    }

    ଅଚଳ ଅକ୍ଷର* temp = nullptr;
    ଯଦି(ଚାବି_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.ମୂଳ_Res;
        ଯଦି(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            ଯଦି (U_FAILURE(*status)) {
                ଫେରନ୍ତୁ nullptr;
            }
            ଅକ୍ଷର *a_ଚାବି = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_ଚାବି, &temp);
        }
        ଯଦି(ଚାବି) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(ଚାବି, *status);
            ଯଦି (U_FAILURE(*status)) {
                ଫେରନ୍ତୁ nullptr;
            }
            ଅକ୍ଷର *a_ଚାବି = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_ଚାବି, &temp);
        } ଅନ୍ୟଥା ଯଦି(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            ଯଦି(URES_IS_TABLE(type)) {
                ଅଚଳ ଅକ୍ଷର *a_ଚାବି;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_ଚାବି);
            } ଅନ୍ୟଥା { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        ଯଦି(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } ଅନ୍ୟଥା {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } ଅନ୍ୟଥା {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(ଚାବି_Path, *status);
        ଯଦି (U_FAILURE(*status)) {
            ଫେରନ୍ତୁ nullptr;
        }
        ଅକ୍ଷର *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        ପାଇଁ(;;) {
            r = dataEntry->fData.ମୂଳ_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            ଯେତେବେଳେ(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                ଯଦି(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    ଭାଙ୍ଗ;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                ଯଦି (U_FAILURE(*status)) {
                    ଭାଙ୍ଗ;
                }
                ଯଦି (temp == nullptr || uprv_strcmp(ଚାବି_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, ଚାବି_Path, static_cast<int32_t>(uprv_strlen(ଚାବି_Path)), status);
                    ଯଦି(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    ଯଦି (U_FAILURE(*status)) {
                        ଭାଙ୍ଗ;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            ଯଦି (U_FAILURE(*status) || r != RES_BOGUS) {
                ଭାଙ୍ଗ;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            ଯଦି (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                ଭାଙ୍ଗ;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, ଚାବି_Path);
        }
    }
    ଯଦି(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    ଫେରନ୍ତୁ resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ଅଚଳ ଅକ୍ଷର* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ଯଦି(status == nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ resB;
    }
    ଯଦି (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        ଯଦି(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            ଫେରନ୍ତୁ resB;
        }
        ଫେରନ୍ତୁ getAliasTargetAsResourceBundle(
            dataEntry->fData, r, ଚାବି, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    ଯଦି(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(ଆକାର(UResourceBundle)));
        ଯଦି (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        ures_set_Is_ଗଦା_Object(resB, ମିଥ୍ୟା);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } ଅନ୍ୟଥା {
        ଯଦି(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        ଯଦି(resB->fVersion != nullptr) {
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
        ଯଦି(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = ମିଥ୍ୟା;
    resB->f_Is_ଶୀର୍ଷ_Level = ମିଥ୍ୟା;
    resB->fIndex = -1;
    resB->f_ଚାବି = ଚାବି; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    ଯଦି(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    ଯଦି(ଚାବି != nullptr) {
        ures_appendResPath(resB, ଚାବି, static_cast<int32_t>(uprv_strlen(ଚାବି)), status);
        ଯଦି(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } ଅନ୍ୟଥା ଯଦି(idx >= 0) {
        ଅକ୍ଷର buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        ଯଦି(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, ଆକାର(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_ଗଣନା_Array_ବସ୍ତୁଗୁଡ଼ିକ(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    ଫେରନ୍ତୁ resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ଅଚଳ UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    ଫେରନ୍ତୁ init_resb_result(
        dataEntry, r, ଚାବି, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ଅଚଳ UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_ଗଦା_Object;
    ଯଦି(U_FAILURE(*status) || r == original) {
        ଫେରନ୍ତୁ r;
    }
    ଯଦି(original != nullptr) {
        ଯଦି(r == nullptr) {
            is_ଗଦା_Object = ମିଥ୍ୟା;
            r = static_cast<UResourceBundle*>(uprv_malloc(ଆକାର(UResourceBundle)));
            /* test for nullptr */
            ଯଦି (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ nullptr;
            }
        } ଅନ୍ୟଥା {
            is_ଗଦା_Object = ures_is_ଗଦା_Object(r);
            ures_closeBundle(r, ମିଥ୍ୟା);
        }
        uprv_memcpy(r, original, ଆକାର(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        ଯଦି(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_ଗଦା_Object(r, is_ଗଦା_Object);
        ଯଦି(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    ଫେରନ୍ତୁ r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ଅଚଳ char16_t* U_EXPORT2 ures_getString(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    ଅଚଳ char16_t *s;
    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    ଯଦି (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ s;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର*
ures_toUTF8String(ଅଚଳ char16_t* s16,
                  int32_t length16,
                  ଅକ୍ଷର* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (pLength != nullptr) {
        capacity = *pLength;
    } ଅନ୍ୟଥା {
        capacity = 0;
    }
    ଯଦି (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି (length16 == 0) {
        /* empty string, return as read-only pointer */
        ଯଦି (pLength != nullptr) {
            *pLength = 0;
        }
        ଯଦି (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            ଫେରନ୍ତୁ dest;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ "";
        }
    } ଅନ୍ୟଥା {
        /* We need to transform the string to the destination buffer. */
        ଯଦି (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            ଫେରନ୍ତୁ u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        ଯଦି (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            ଯଦି (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        ଫେରନ୍ତୁ u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_getUTF8String(ଅଚଳ UResourceBundle* resB,
                   ଅକ୍ଷର* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    ଅଚଳ char16_t *s16 = ures_getString(resB, &length16, status);
    ଫେରନ୍ତୁ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI ଅଚଳ uint8_t* U_EXPORT2
ures_get_ଦ୍ୱିମିକ(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ଅଚଳ uint8_t *p;
  ଯଦି (status==nullptr || U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  ଯଦି(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  p = res_get_ଦ୍ୱିମିକ({resB}, &resB->getResData(), resB->fRes, len);
  ଯଦି (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  ଫେରନ୍ତୁ p;
}

U_CAPI ଅଚଳ int32_t* U_EXPORT2
ures_getIntVector(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ଅଚଳ int32_t *p;
  ଯଦି (status==nullptr || U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  ଯଦି(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  ଯଦି (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  ଫେରନ୍ତୁ p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ଅଚଳ UResourceBundle* resB, UErrorCode *status) {
  ଯଦି (status==nullptr || U_FAILURE(*status)) {
    ଫେରନ୍ତୁ 0xffffffff;
  }
  ଯଦି(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0xffffffff;
  }
  ଯଦି(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    ଫେରନ୍ତୁ 0xffffffff;
  }
  ଫେରନ୍ତୁ res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ଅଚଳ UResourceBundle* resB, UErrorCode *status) {
  ଯଦି (status==nullptr || U_FAILURE(*status)) {
    ଫେରନ୍ତୁ 0xffffffff;
  }
  ଯଦି(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0xffffffff;
  }
  ଯଦି(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    ଫେରନ୍ତୁ 0xffffffff;
  }
  ଫେରନ୍ତୁ res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ଅଚଳ UResourceBundle *resB) {
  ଯଦି(resB == nullptr) {
    ଫେରନ୍ତୁ URES_NONE;
  }
  ଫେରନ୍ତୁ res_getPublicType(resB->fRes);
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_get_ଚାବି(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  ଯଦି(resB == nullptr) {
    ଫେରନ୍ତୁ nullptr;
  }
  ଫେରନ୍ତୁ(resB->f_ଚାବି);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(ଅଚଳ UResourceBundle *resB) {
  ଯଦି(resB == nullptr) {
    ଫେରନ୍ତୁ 0;
  }
  
  ଫେରନ୍ତୁ resB->fSize;
}

ସ୍ଥିର ଅଚଳ char16_t*
ures_getStringWithAlias(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  ଯଦି(RES_GET_TYPE(r) == URES_ALIAS) {
    ଅଚଳ char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    ଫେରନ୍ତୁ result;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  ଯଦି(resB == nullptr) {
    ଫେରନ୍ତୁ;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(ଅଚଳ UResourceBundle *resB) {
  ଯଦି(resB == nullptr) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
  }
  ଫେରନ୍ତୁ resB->fIndex < resB->fSize-1;
}

U_CAPI ଅଚଳ char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ଅଚଳ ଅକ୍ଷର** ଚାବି,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  ଯଦି (status==nullptr || U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  ଯଦି(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  
  ଯଦି(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } ଅନ୍ୟଥା {
    resB->fIndex++;
    ବାଛ(RES_GET_TYPE(resB->fRes)) {
    ସ୍ଥିତି URES_STRING:
    ସ୍ଥିତି URES_STRING_V2:
      ଫେରନ୍ତୁ res_getString({resB}, &resB->getResData(), resB->fRes, len);
    ସ୍ଥିତି URES_TABLE:
    ସ୍ଥିତି URES_TABLE16:
    ସ୍ଥିତି URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, ଚାବି);
      ଯଦି(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ଫେରନ୍ତୁ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    ସ୍ଥିତି URES_ARRAY:
    ସ୍ଥିତି URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      ଯଦି(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ଫେରନ୍ତୁ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    ସ୍ଥିତି URES_ALIAS:
      ଫେରନ୍ତୁ ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    ସ୍ଥିତି URES_INT:
    ସ୍ଥିତି URES_ଦ୍ୱିମିକ:
    ସ୍ଥିତି URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ nullptr;
    }
  }

  ଫେରନ୍ତୁ nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    ଅଚଳ ଅକ୍ଷର *ଚାବି = nullptr;
    Resource r = RES_BOGUS;

    ଯଦି (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            ଫେରନ୍ତୁ fillIn;
    }
    ଯଦି(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            ଫେରନ୍ତୁ fillIn;
    }

    ଯଦି(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } ଅନ୍ୟଥା {
        resB->fIndex++;
        ବାଛ(RES_GET_TYPE(resB->fRes)) {
        ସ୍ଥିତି URES_INT:
        ସ୍ଥିତି URES_ଦ୍ୱିମିକ:
        ସ୍ଥିତି URES_STRING:
        ସ୍ଥିତି URES_STRING_V2:
        ସ୍ଥିତି URES_INT_VECTOR:
            ଫେରନ୍ତୁ ures_copyResb(fillIn, resB, status);
        ସ୍ଥିତି URES_TABLE:
        ସ୍ଥିତି URES_TABLE16:
        ସ୍ଥିତି URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &ଚାବି);
            ଯଦି(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ଫେରନ୍ତୁ init_resb_result(resB->fData, r, ଚାବି, resB->fIndex, resB, fillIn, status);
        ସ୍ଥିତି URES_ARRAY:
        ସ୍ଥିତି URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            ଯଦି(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ଫେରନ୍ତୁ init_resb_result(resB->fData, r, ଚାବି, resB->fIndex, resB, fillIn, status);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            /*return nullptr;*/
            ଫେରନ୍ତୁ fillIn;
        }
    }
    /*return nullptr;*/
    ଫେରନ୍ତୁ fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(ଅଚଳ UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    ଅଚଳ ଅକ୍ଷର* ଚାବି = nullptr;
    Resource r = RES_BOGUS;

    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        ଫେରନ୍ତୁ fillIn;
    }
    ଯଦି(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        ଫେରନ୍ତୁ fillIn;
    }

    ଯଦି(indexR >= 0 && resB->fSize > indexR) {
        ବାଛ(RES_GET_TYPE(resB->fRes)) {
        ସ୍ଥିତି URES_INT:
        ସ୍ଥିତି URES_ଦ୍ୱିମିକ:
        ସ୍ଥିତି URES_STRING:
        ସ୍ଥିତି URES_STRING_V2:
        ସ୍ଥିତି URES_INT_VECTOR:
            ଫେରନ୍ତୁ ures_copyResb(fillIn, resB, status);
        ସ୍ଥିତି URES_TABLE:
        ସ୍ଥିତି URES_TABLE16:
        ସ୍ଥିତି URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &ଚାବି);
            ଯଦି(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ଫେରନ୍ତୁ init_resb_result(resB->fData, r, ଚାବି, indexR, resB, fillIn, status);
        ସ୍ଥିତି URES_ARRAY:
        ସ୍ଥିତି URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            ଯଦି(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ଫେରନ୍ତୁ init_resb_result(resB->fData, r, ଚାବି, indexR, resB, fillIn, status);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            /*return nullptr;*/
            ଫେରନ୍ତୁ fillIn;
        }
    } ଅନ୍ୟଥା {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    ଫେରନ୍ତୁ fillIn;
}

U_CAPI ଅଚଳ char16_t* U_EXPORT2
ures_getStringByIndex(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    ଅଚଳ ଅକ୍ଷର* ଚାବି = nullptr;
    Resource r = RES_BOGUS;

    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି(indexS >= 0 && resB->fSize > indexS) {
        ବାଛ(RES_GET_TYPE(resB->fRes)) {
        ସ୍ଥିତି URES_STRING:
        ସ୍ଥିତି URES_STRING_V2:
            ଫେରନ୍ତୁ res_getString({resB}, &resB->getResData(), resB->fRes, len);
        ସ୍ଥିତି URES_TABLE:
        ସ୍ଥିତି URES_TABLE16:
        ସ୍ଥିତି URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &ଚାବି);
            ଯଦି(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ଫେରନ୍ତୁ ures_getStringWithAlias(resB, r, indexS, len, status);
        ସ୍ଥିତି URES_ARRAY:
        ସ୍ଥିତି URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            ଯଦି(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ଫେରନ୍ତୁ ures_getStringWithAlias(resB, r, indexS, len, status);
        ସ୍ଥିତି URES_ALIAS:
            ଫେରନ୍ତୁ ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        ସ୍ଥିତି URES_INT:
        ସ୍ଥିତି URES_ଦ୍ୱିମିକ:
        ସ୍ଥିତି URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          ଭାଙ୍ଗ;
        }
    } ଅନ୍ୟଥା {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    ଫେରନ୍ତୁ nullptr;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_getUTF8StringByIndex(ଅଚଳ UResourceBundle* resB,
                          int32_t idx,
                          ଅକ୍ଷର* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    ଅଚଳ char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    ଫେରନ୍ତୁ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(ଅଚଳ ଅକ୍ଷର* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  ଅକ୍ଷର *packageName = nullptr;
  ଅକ୍ଷର *pathToResource = nullptr, *save = nullptr;
  ଅକ୍ଷର *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  ଯଦି(status == nullptr || U_FAILURE(*status)) {
    ଫେରନ୍ତୁ result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (ଅକ୍ଷର *)uprv_malloc(length*ଆକାର(ଅକ୍ଷର));
  /* test for nullptr */
  ଯଦି(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  ଯଦି(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    ଯଦି(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } ଅନ୍ୟଥା {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  ଯଦି(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  ଯଦି(U_SUCCESS(*status)) {
    ଯଦି(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } ଅନ୍ୟଥା {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  ଫେରନ୍ତୁ result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(ଅଚଳ UResourceBundle* resB,
                     ଅକ୍ଷର* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  ଅଚଳ ଅକ୍ଷର *ଚାବି;

  ଯଦି(status == nullptr || U_FAILURE(*status)) {
    ଫେରନ୍ତୁ result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  କର {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &ଚାବି); 
    ଯଦି(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, ଚାବି, -1, resB, fillIn, status);
        resB = result;
    } ଅନ୍ୟଥା {
        *status = U_MISSING_RESOURCE_ERROR;
        ଭାଙ୍ଗ;
    }
  } ଯେତେବେଳେ(*path); /* there is more stuff in the path */

  ଫେରନ୍ତୁ result;
}

U_CAPI ଅଚଳ char16_t* U_EXPORT2
ures_get_String_By_ଚାବି_With_Fallback(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND,
                                ଅଚଳ ଅକ୍ଷର* in_ଚାବି,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle ଗଦା_2;
    ଅଚଳ char16_t* retVal = nullptr;
    ures_init_ଗଦା_Object(&ଗଦା_2);
    ures_get_By_ଚାବି_With_Fallback(resB, in_ଚାବି, &ଗଦା_2, status);
    int32_t length;
    retVal = ures_getString(&ଗଦା_2, &length, status);
    ures_close(&ଗଦା_2);
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    ଯଦି (len != nullptr) {
        *len = length;
    }
    ଫେରନ୍ତୁ retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ସ୍ଥିର Resource get_Table_Item_By_ଚାବି_Path(ଅଚଳ ResourceData *pResData, Resource table, ଅଚଳ ଅକ୍ଷର *ଚାବି) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(ଚାବି, errorCode);
  ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ RES_BOGUS; }
  ଅକ୍ଷର *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  ଯେତେବେଳେ (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    ଅକ୍ଷର *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    ଯଦି (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } ଅନ୍ୟଥା {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    ଅଚଳ ଅକ୍ଷର *pathP = pathPart;
    resource = res_get_Table_Item_By_ଚାବି(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  ଯଦି (*pathPart) {
    ଫେରନ୍ତୁ RES_BOGUS;
  }
  ଫେରନ୍ତୁ resource;
}

ସ୍ଥିର ଶୂନ୍ୟ createPath(ଅଚଳ ଅକ୍ଷର* origResPath,
                       int32_t     origResPathLen,
                       ଅଚଳ ଅକ୍ଷର* resPath,
                       int32_t     resPathLen,
                       ଅଚଳ ଅକ୍ଷର* in_ଚାବି,
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
    ଅଚଳ ଅକ୍ଷର* ଚାବି = in_ଚାବି;
    ଯଦି (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        ଯଦି (U_SUCCESS(*status)) {
            ଅଚଳ ଅକ୍ଷର* resPathLimit = resPath + resPathLen;
            ଅଚଳ ଅକ୍ଷର* origResPathLimit = origResPath + origResPathLen;
            ଅଚଳ ଅକ୍ଷର* resPathPtr = resPath;
            ଅଚଳ ଅକ୍ଷର* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            ଯେତେବେଳେ (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                ଯେତେବେଳେ (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                ଯଦି (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                ଯେତେବେଳେ (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ଯଦି (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            ଯେତେବେଳେ (resPathPtr < resPathLimit && *ଚାବି != '\0') {
                ଯେତେବେଳେ (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ଯଦି (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ଯେତେବେଳେ (*ଚାବି != '\0' && *ଚାବି != RES_PATH_SEPARATOR) {
                    ++ଚାବି;
                }
                ଯଦି (*ଚାବି == RES_PATH_SEPARATOR) {
                    ++ଚାବି;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(ଚାବି, *status);
    } ଅନ୍ୟଥା {
        path.append(in_ଚାବି, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ଚାବି_With_Fallback(ଅଚଳ UResourceBundle* resB,
                          ଅଚଳ ଅକ୍ଷର* in_ଚାବି,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, ମୂଳ_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ fillIn;
    }
    ଯଦି(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ଯଦି(URES_IS_TABLE(type)) {
        ଅଚଳ ଅକ୍ଷର* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_ଚାବି_Path(&resB->getResData(), resB->fRes, in_ଚାବି);
        ଅଚଳ ଅକ୍ଷର* ଚାବି = in_ଚାବି;
        ତାର୍କିକ did_ମୂଳ_Once = ମିଥ୍ୟା;
        ଯଦି(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            ଅକ୍ଷର *myPath = nullptr;
            ଅଚଳ ଅକ୍ଷର* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            ଯେତେବେଳେ(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_ମୂଳ_Once)) { /* Otherwise, we'll look in parents */
                ଯଦି (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } ଅନ୍ୟଥା {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_ମୂଳ_Once = ସତ୍ୟ;
                }
                ମୂଳ_Res = dataEntry->fData.ମୂଳ_Res;

                ଯଦି(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_ଚାବି, path, status);
                    ଯଦି (U_FAILURE(*status)) {
                        ures_close(helper);
                        ଫେରନ୍ତୁ fillIn;
                    }
                    myPath = path.data();
                    ଚାବି = in_ଚାବି;
                    କର {
                        res = res_findResource(&(dataEntry->fData), ମୂଳ_Res, &myPath, &ଚାବି);
                        ଯଦି (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            ଯଦି(helper) {
                              dataEntry = helper->fData;
                              ମୂଳ_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } ଅନ୍ୟଥା {
                              ଭାଙ୍ଗ;
                            }
                        } ଅନ୍ୟଥା ଯଦି (res == RES_BOGUS) {
                            ଭାଙ୍ଗ;
                        }
                    } ଯେତେବେଳେ(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            ଯଦି(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                ଯଦି(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_ମୂଳ_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } ଅନ୍ୟଥା {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, ଚାବି, -1, resB, fillIn, status);
                ଯଦି (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_ଚାବି, path, status);
                } ଅନ୍ୟଥା {
                    ଅଚଳ ଅକ୍ଷର* separator = nullptr;
                    ଯଦି (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    ଯଦି (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_ଚାବି, path, status);
                    } ଅନ୍ୟଥା {
                        createPath(origResPath, origResPathLen, "", 0, in_ଚାବି, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                ଯଦି(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } ଅନ୍ୟଥା {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ଅନ୍ୟଥା {
            fillIn = init_resb_result(resB->fData, res, ଚାବି, -1, resB, fillIn, status);
        }
    } 
    ଅନ୍ୟଥା {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    ଫେରନ୍ତୁ fillIn;
}

ନାମାକାଶ {

ଶୂନ୍ୟ get_All_ବସ୍ତୁଗୁଡ଼ିକ_With_Fallback(
        ଅଚଳ UResourceBundle *bundle, Resource_Data_ମୂଲ୍ୟ &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
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
    sink.put(bundle->f_ଚାବି, value, !hasParent, errorCode);
    ଯଦି (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ଗଦା_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_ଶୀର୍ଷ_Level = ସତ୍ୟ;
        parentRef.fRes = parentRef.getResData().ମୂଳ_Res;
        parentRef.fSize = res_ଗଣନା_Array_ବସ୍ତୁଗୁଡ଼ିକ(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        ଗଦା_UResource_Bundle containerBundle;
        ଅଚଳ UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        ଯଦି (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } ଅନ୍ୟଥା {
            rb = ures_get_By_ଚାବି_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        ଯଦି (U_SUCCESS(pathErrorCode)) {
            get_All_ବସ୍ତୁଗୁଡ଼ିକ_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

ସଂରଚନା GetAllChildrenSink : ସର୍ବସାଧାରଣ ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    ଆଭାସୀ ~GetAllChildrenSink() override;
    ଆଭାସୀ ଶୂନ୍ୟ put(ଅଚଳ ଅକ୍ଷର *ଚାବି, Resource_ମୂଲ୍ୟ &value, UBool is_ମୂଳ,
           UErrorCode &errorCode) override {
        ResourceTable ବସ୍ତୁଗୁଡ଼ିକ_Table = value.getTable(errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        ପାଇଁ (int32_t i = 0; ବସ୍ତୁଗୁଡ଼ିକ_Table.get_ଚାବି_And_ମୂଲ୍ୟ(i, ଚାବି, value); ++i) {
            ଯଦି (value.getType() == URES_ALIAS) {
                Resource_Data_ମୂଲ୍ୟ& rdv = static_cast<Resource_Data_ମୂଲ୍ୟ&>(value);
                ଗଦା_UResource_Bundle ଗଦା_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          ଗଦା_Temp_Bundle.getAlias(), &errorCode);
                ଯଦି (U_SUCCESS(errorCode)) {
                    Resource_Data_ମୂଲ୍ୟ aliased_ମୂଲ୍ୟ;
                    aliased_ମୂଲ୍ୟ.setData(aliasRB->getResData());
                    aliased_ମୂଲ୍ୟ.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_ମୂଲ୍ୟ.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    ଯଦି (aliased_ମୂଲ୍ୟ.getType() != URES_TABLE) {
                        dest.put(ଚାବି, aliased_ମୂଲ୍ୟ, is_ମୂଳ, errorCode);
                    } ଅନ୍ୟଥା {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_ମୂଲ୍ୟ_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        ଅଚଳ ଅକ୍ଷର* parent_ଚାବି = ଚାବି; // dest.put() changes the key
                        dest.put(parent_ଚାବି, aliased_ମୂଲ୍ୟ, is_ମୂଳ, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        ଯେତେବେଳେ (aliased_ମୂଲ୍ୟ_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            ଅକ୍ଷର* localPathAsCharPtr = localPath.data();
                            ଅଚଳ ଅକ୍ଷର* child_ଚାବି;
                            entry = entry->fParent;
                            res = entry->fData.ମୂଳ_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_ଚାବି);
                            ଯଦି (newRes != RES_BOGUS) {
                                aliased_ମୂଲ୍ୟ.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_ମୂଲ୍ୟ.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_ମୂଲ୍ୟ_Type = aliased_ମୂଲ୍ୟ.getType();
                                ଯଦି (aliased_ମୂଲ୍ୟ_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_ମୂଲ୍ୟ& rdv2 = static_cast<Resource_Data_ମୂଲ୍ୟ&>(aliased_ମୂଲ୍ୟ);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             ଗଦା_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_ମୂଲ୍ୟ.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_ମୂଲ୍ୟ.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_ମୂଲ୍ୟ_Type = aliased_ମୂଲ୍ୟ.getType();
                                }
                                ଯଦି (aliased_ମୂଲ୍ୟ_Type == URES_TABLE) {
                                    dest.put(parent_ଚାବି, aliased_ମୂଲ୍ୟ, is_ମୂଳ, errorCode);
                                } ଅନ୍ୟଥା {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    ଫେରନ୍ତୁ;
                                }
                            }
                        }
                    }
                }
            } ଅନ୍ୟଥା {
                dest.put(ଚାବି, value, is_ମୂଳ, errorCode);
            }
            ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ures_getAllChildrenWithFallback(ଅଚଳ UResourceBundle *bundle, ଅଚଳ ଅକ୍ଷର *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_ବସ୍ତୁଗୁଡ଼ିକ_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI ଶୂନ୍ୟ U_EXPORT2
ures_get_ମୂଲ୍ୟ_With_Fallback(ଅଚଳ UResourceBundle *bundle, ଅଚଳ ଅକ୍ଷର *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_ମୂଲ୍ୟ &value, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    ଯଦି (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ UResourceBundle *rb;
    ଯଦି (*path == 0) {
        // empty path
        rb = bundle;
    } ଅନ୍ୟଥା {
        rb = ures_get_By_ଚାବି_With_Fallback(bundle, path, tempFillIn, &errorCode);
        ଯଦି (U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ures_get_All_ବସ୍ତୁଗୁଡ଼ିକ_With_Fallback(ଅଚଳ UResourceBundle *bundle, ଅଚଳ ଅକ୍ଷର *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    ଯଦି (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଗଦା_UResource_Bundle ଗଦା_Bundle;
    ଅଚଳ UResourceBundle *rb;
    ଯଦି (*path == 0) {
        // empty path
        rb = bundle;
    } ଅନ୍ୟଥା {
        rb = ures_get_By_ଚାବି_With_Fallback(bundle, path, ଗଦା_Bundle.getAlias(), &errorCode);
        ଯଦି (U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ;
        }
    }
    // Get all table items with fallback.
    Resource_Data_ମୂଲ୍ୟ value;
    get_All_ବସ୍ତୁଗୁଡ଼ିକ_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ଚାବି(ଅଚଳ UResourceBundle* resB,
              ଅଚଳ ଅକ୍ଷର* in_ଚାବି,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ଅଚଳ ଅକ୍ଷର *ଚାବି = in_ଚାବି;

    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ fillIn;
    }
    ଯଦି(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ଯଦି(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_ଚାବି(&resB->getResData(), resB->fRes, &t, &ଚାବି);
        ଯଦି(res == RES_BOGUS) {
            ଚାବି = in_ଚାବି;
            ଯଦି(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &ଚାବି, &res, status);
                ଯଦି(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    ଫେରନ୍ତୁ init_resb_result(dataEntry, res, ଚାବି, -1, resB, fillIn, status);
                } ଅନ୍ୟଥା {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } ଅନ୍ୟଥା {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ init_resb_result(resB->fData, res, ଚାବି, -1, resB, fillIn, status);
        }
    } 
#ଯଦି 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    ଅନ୍ୟଥା ଯଦି(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ସତ୍ୟ) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &ଚାବି, &res, status);
        ଯଦି(U_SUCCESS(*status)) {
            ଫେରନ୍ତୁ init_resb_result(dataEntry, res, ଚାବି, resB, fillIn, status);
        } ଅନ୍ୟଥା {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    ଅନ୍ୟଥା {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ fillIn;
}

U_CAPI ଅଚଳ char16_t* U_EXPORT2
ures_get_String_By_ଚାବି(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND,
                    ଅଚଳ ଅକ୍ଷର* in_ଚାବି,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ଅଚଳ ଅକ୍ଷର* ଚାବି = in_ଚାବି;

    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ଯଦି(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_ଚାବି(&resB->getResData(), resB->fRes, &t, &ଚାବି);

        ଯଦି(res == RES_BOGUS) {
            ଚାବି = in_ଚାବି;
            ଯଦି(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &ଚାବି, &res, status);
                ଯଦି(U_SUCCESS(*status)) {
                    ବାଛ (RES_GET_TYPE(res)) {
                    ସ୍ଥିତି URES_STRING:
                    ସ୍ଥିତି URES_STRING_V2:
                        ଫେରନ୍ତୁ res_getString({resB, ଚାବି}, &dataEntry->fData, res, len);
                    ସ୍ଥିତି URES_ALIAS:
                      {
                        ଅଚଳ char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_ଚାବି(resB, in_ଚାବି, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        ଫେରନ୍ତୁ result;
                      }
                    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } ଅନ୍ୟଥା {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } ଅନ୍ୟଥା {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ଅନ୍ୟଥା {
            ବାଛ (RES_GET_TYPE(res)) {
            ସ୍ଥିତି URES_STRING:
            ସ୍ଥିତି URES_STRING_V2:
                ଫେରନ୍ତୁ res_getString({resB, ଚାବି}, &resB->getResData(), res, len);
            ସ୍ଥିତି URES_ALIAS:
              {
                ଅଚଳ char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_ଚାବି(resB, in_ଚାବି, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                ଫେରନ୍ତୁ result;
              }
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#ଯଦି 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    ଅନ୍ୟଥା ଯଦି(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ସତ୍ୟ) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &ଚାବି, &res, status);
        ଯଦି(U_SUCCESS(*status)) {
            // TODO: Tracing
            ଫେରନ୍ତୁ res_getString(rd, res, len);
        } ଅନ୍ୟଥା {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    ଅନ୍ୟଥା {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ nullptr;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_get_UTF8_String_By_ଚାବି(ଅଚଳ UResourceBundle* resB,
                        ଅଚଳ ଅକ୍ଷର* ଚାବି,
                        ଅକ୍ଷର* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    ଅଚଳ char16_t *s16 = ures_get_String_By_ଚାବି(resB, ଚାବି, &length16, status);
    ଫେରନ୍ତୁ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_getLocaleInternal(ଅଚଳ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
      ଫେରନ୍ତୁ resourceBundle->fData->fName;
    }
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_getLocale(ଅଚଳ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  ଫେରନ୍ତୁ ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_getLocaleByType(ଅଚଳ UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
        ବାଛ(type) {
        ସ୍ଥିତି ULOC_ACTUAL_LOCALE:
            ଫେରନ୍ତୁ resourceBundle->fData->fName;
        ସ୍ଥିତି ULOC_VALID_LOCALE:
            ଫେରନ୍ତୁ resourceBundle->fValidLocaleDataEntry->fName;
        ସ୍ଥିତି ULOC_REQUESTED_LOCALE:
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
    }
}

U_CFUNC ଅଚଳ ଅକ୍ଷର*
ures_getName(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND) {
  ଯଦି(resB == nullptr) {
    ଫେରନ୍ତୁ nullptr;
  }

  ଫେରନ୍ତୁ resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC ଅଚଳ ଅକ୍ଷର*
ures_getPath(ଅଚଳ UResourceBundle* resB U_LIFETIME_BOUND) {
  ଯଦି(resB == nullptr) {
    ଫେରନ୍ତୁ nullptr;
  }

  ଫେରନ୍ତୁ resB->fData->fPath;
}
#endif

ସ୍ଥିର UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ଅଚଳ ଅକ୍ଷର* path,
                  ଅଚଳ ଅକ୍ଷର* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    ଯଦି(U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    UResourceDataEntry *entry;
    ଯଦି(openType != URES_OPEN_DIRECT) {
        ଯଦି (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        ଯଦି(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } ଅନ୍ୟଥା {
        entry = entryOpenDirect(path, localeID, status);
    }
    ଯଦି(U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    UBool is_ଗଦା_Object;
    ଯଦି(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(ଆକାର(UResourceBundle)));
        ଯଦି(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        is_ଗଦା_Object = ମିଥ୍ୟା;
    } ଅନ୍ୟଥା {  // fill-in
        is_ଗଦା_Object = ures_is_ଗଦା_Object(r);
        ures_closeBundle(r, ମିଥ୍ୟା);
    }
    uprv_memset(r, 0, ଆକାର(UResourceBundle));
    ures_set_Is_ଗଦା_Object(r, is_ଗଦା_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_ଶୀର୍ଷ_Level = ସତ୍ୟ;
    r->fRes = r->getResData().ମୂଳ_Res;
    r->fSize = res_ଗଣନା_Array_ବସ୍ତୁଗୁଡ଼ିକ(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    ଫେରନ୍ତୁ r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* localeID, UErrorCode* status) {
    ଫେରନ୍ତୁ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_ମୂଳ, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* localeID, UErrorCode* status) {
    ଫେରନ୍ତୁ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_ମୂଳ, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* localeID, UErrorCode* status) {
    ଫେରନ୍ତୁ ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ଶୂନ୍ୟ U_EXPORT2
ures_openFillIn(UResourceBundle *r, ଅଚଳ ଅକ୍ଷର* path,
                ଅଚଳ ଅକ୍ଷର* localeID, UErrorCode* status) {
    ଯଦି(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_ମୂଳ, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ଶୂନ୍ୟ U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* localeID, UErrorCode* status) {
    ଯଦି(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_ଗଣନା_Array_ବସ୍ତୁଗୁଡ଼ିକ(ଅଚଳ UResourceBundle* resourceBundle,
                  ଅଚଳ ଅକ୍ଷର* resource_ଚାବି,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_ଗଦା_Object(&resData);
    ଯଦି (status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ures_get_By_ଚାବି(resourceBundle, resource_ଚାବି, &resData, status);
    
    ଯଦି(resData.getResData().data != nullptr) {
        int32_t result = res_ଗଣନା_Array_ବସ୍ତୁଗୁଡ଼ିକ(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        ଫେରନ୍ତୁ result;
    } ଅନ୍ୟଥା {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        ଫେରନ୍ତୁ 0;
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
U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_get_Version_ସଂଖ୍ୟା_Internal(ଅଚଳ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ଯଦି (!resourceBundle) ଫେରନ୍ତୁ nullptr;

    ଯଦି(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ଅଚଳ char16_t* minor_version = ures_get_String_By_ଚାବି(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (ଅକ୍ଷର *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        ଯଦି (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            ଫେରନ୍ତୁ nullptr;
        }
       
        ଯଦି(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        ଅନ୍ୟଥା {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    ଫେରନ୍ତୁ resourceBundle->fVersion;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ures_get_Version_ସଂଖ୍ୟା(ଅଚଳ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ଫେରନ୍ତୁ ures_get_Version_ସଂଖ୍ୟା_Internal(resourceBundle);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ures_getVersion(ଅଚଳ UResourceBundle* resB, UVersionInfo versionInfo) {
    ଯଦି (!resB) ଫେରନ୍ତୁ;

    u_versionFromString(versionInfo, ures_get_Version_ସଂଖ୍ୟା_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

ସ୍ଥିର int32_t U_CALLCONV
ures_loc_ଗଣନା_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    ଫେରନ୍ତୁ ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    ଅଚଳ ଅକ୍ଷର *result = nullptr;
    int32_t len = 0;
    ଯଦି (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_ଚାବି(k);
        len = (int32_t)uprv_strlen(result);
    }
    ଯଦି (resultLength) {
        *resultLength = len;
    }
    ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

ସ୍ଥିର ଅଚଳ UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_ଗଣନା_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(ଅଚଳ ଅକ୍ଷର *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    ଯଦି(U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(ଆକାର(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(ଆକାର(UEnumeration));
    ଯଦି(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, ଆକାର(UEnumeration));

    ures_init_ଗଦା_Object(&myContext->installed);
    ures_init_ଗଦା_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_ଚାବି(idx, INDEX_TAG, &myContext->installed, status);
    ଯଦି(U_SUCCESS(*status)) {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_ଚାବି(&myContext->installed));
#endif
        en->context = myContext;
    } ଅନ୍ୟଥା {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    ଫେରନ୍ତୁ en;
}

ସ୍ଥିର UBool isLocaleInList(UEnumeration *locEnum, ଅଚଳ ଅକ୍ଷର *loc_To_ସନ୍ଧାନ, UErrorCode *status) {
    ଅଚଳ ଅକ୍ଷର *loc;
    ଯେତେବେଳେ ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        ଯଦି (uprv_strcmp(loc, loc_To_ସନ୍ଧାନ) == 0) {
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ସ୍ଥିର ଶୂନ୍ୟ getParentForFunctionalEquivalent(ଅଚଳ ଅକ୍ଷର*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    ଯଦି (res != nullptr) {
        ures_get_By_ଚାବି(res, "%%Parent", bund1, &subStatus);
        ଯଦି (U_SUCCESS(subStatus)) {
            int32_t length16;
            ଅଚଳ char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    ଯଦି (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(ଅକ୍ଷର *result, int32_t resultCapacity,
                             ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *resName, ଅଚଳ ଅକ୍ଷର *keyword, ଅଚଳ ଅକ୍ଷର *locid,
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
    ଯଦି(U_FAILURE(*status)) ଫେରନ୍ତୁ 0;
    CharString kwVal;
    ଯଦି (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_ମୂଲ୍ୟ(locid, keyword, subStatus);
        ଯଦି (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    ଯଦି (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_ଗଦା_Object(&bund1);
    ures_init_ଗଦା_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    ଯଦି(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = ସତ୍ୟ;
        ଯଦି (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    ଯଦି(U_FAILURE(subStatus)) {
        *status = subStatus;
        ଫେରନ୍ତୁ 0;
    }
    
    କର {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        ଯଦି(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = ମିଥ୍ୟା;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        ଯଦି(U_FAILURE(subStatus)) {
            *status = subStatus;
        } ଅନ୍ୟଥା ଯଦି(subStatus == U_ZERO_ERROR) {
            ures_get_By_ଚାବି(res,resName,&bund1, &subStatus);
            ଯଦି(subStatus == U_ZERO_ERROR) {
                ଅଚଳ char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_ଚାବି(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                ଯଦି(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    ଯଦି(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        ଯଦି (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        ଯଦି (found != parent) {
            parent.copyFrom(found, subStatus);
        } ଅନ୍ୟଥା {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } ଯେତେବେଳେ(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    କର {
        res = ures_open(path, parent.data(), &subStatus);
        ଯଦି((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = ମିଥ୍ୟା;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        ଯଦି(U_FAILURE(subStatus)) {
            *status = subStatus;
        } ଅନ୍ୟଥା ଯଦି(subStatus == U_ZERO_ERROR) {
            ures_get_By_ଚାବି(res,resName,&bund1, &subStatus);
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            ଯଦି(subStatus == U_ZERO_ERROR) {
                ures_get_By_ଚାବି(&bund1, kwVal.data(), &bund2, &subStatus);
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                ଯଦି(subStatus == U_ZERO_ERROR) {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    ଯଦି (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } ଅନ୍ୟଥା {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        ଯଦି(defLoc.length() > full.length()) {
                          ଅଚଳ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_ଚାବି(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          ଯଦି(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                        ଅନ୍ୟଥା {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } ଅନ୍ୟଥା {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = ମିଥ୍ୟା;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        ଯଦି (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            ଅଚଳ ଅକ୍ଷର *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            ଯଦି (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                ଯଦି (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = ସତ୍ୟ;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        ଯଦି (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } ଯେତେବେଳେ(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    ଯଦି(full.isEmpty() && kwVal != defVal) {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        କର { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            ଯଦି((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = ମିଥ୍ୟା;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            ଯଦି(U_FAILURE(subStatus)) {
                *status = subStatus;
            } ଅନ୍ୟଥା ଯଦି(subStatus == U_ZERO_ERROR) {
                ures_get_By_ଚାବି(res,resName,&bund1, &subStatus);
                ଯଦି(subStatus == U_ZERO_ERROR) {
                    ures_get_By_ଚାବି(&bund1, kwVal.data(), &bund2, &subStatus);
                    ଯଦି(subStatus == U_ZERO_ERROR) {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        ଯଦି (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } ଅନ୍ୟଥା {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        ଯଦି(defLoc.length() > full.length()) {
                          ଅଚଳ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_ଚାବି(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          ଯଦି(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                        ଅନ୍ୟଥା {
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
        } ଯେତେବେଳେ(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    ଯଦି(U_SUCCESS(*status)) {
        ଯଦି(full.isEmpty()) {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } ଅନ୍ୟଥା ଯଦି(omitDefault) {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          ଯଦି(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            ଯଦି(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        ଯଦି(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } ଅନ୍ୟଥା ଯଦି(!omitDefault) {
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

    ଯଦି(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        ଯଦି(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        ଯଦି(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } ଅନ୍ୟଥା {
        length = 0;
        result[0]=0;
    }
    ଫେରନ୍ତୁ u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_ମୂଲ୍ୟଗୁଡ଼ିକ(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *keyword, UErrorCode *status)
{
#define ମୂଲ୍ୟଗୁଡ଼ିକ_BUF_SIZE 2048
#define ମୂଲ୍ୟଗୁଡ଼ିକ_LIST_SIZE 512
    
    ଅକ୍ଷର       ମୂଲ୍ୟଗୁଡ଼ିକ_Buf[ମୂଲ୍ୟଗୁଡ଼ିକ_BUF_SIZE];
    int32_t    ମୂଲ୍ୟଗୁଡ଼ିକ_Index = 0;
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟଗୁଡ଼ିକ_List[ମୂଲ୍ୟଗୁଡ଼ିକ_LIST_SIZE];
    int32_t    ମୂଲ୍ୟଗୁଡ଼ିକ_ଗଣନା = 0;
    
    ଅଚଳ ଅକ୍ଷର *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_ଗଦା_Object(&item);
    ures_init_ଗଦା_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    ଯଦି(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        ଫେରନ୍ତୁ nullptr;
    }
    
    ମୂଲ୍ୟଗୁଡ଼ିକ_Buf[0]=0;
    ମୂଲ୍ୟଗୁଡ଼ିକ_Buf[1]=0;

    ଯେତେବେଳେ ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
        ଯଦି(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_ଚାବି(bund, keyword, &item, &subStatus);
        
        ଯଦି(!bund || U_FAILURE(subStatus)) {
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            ଜାରି;
        }

        ଯେତେବେଳେ ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            ଅଚଳ ଅକ୍ଷର *k;
            int32_t i;
            k = ures_get_ଚାବି(subPtr);

#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            ଯଦି(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                ଜାରି;
            }
            ପାଇଁ(i=0; i<ମୂଲ୍ୟଗୁଡ଼ିକ_ଗଣନା; i++) {
                ଯଦି(!uprv_strcmp(ମୂଲ୍ୟଗୁଡ଼ିକ_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    ଭାଙ୍ଗ;
                }
            }
            ଯଦି(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                ଯଦି((ମୂଲ୍ୟଗୁଡ଼ିକ_ଗଣନା >= (ମୂଲ୍ୟଗୁଡ଼ିକ_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((ମୂଲ୍ୟଗୁଡ଼ିକ_Index+kLen+1+1) >= ମୂଲ୍ୟଗୁଡ଼ିକ_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } ଅନ୍ୟଥା {
                    uprv_strcpy(ମୂଲ୍ୟଗୁଡ଼ିକ_Buf+ମୂଲ୍ୟଗୁଡ଼ିକ_Index, k);
                    ମୂଲ୍ୟଗୁଡ଼ିକ_List[ମୂଲ୍ୟଗୁଡ଼ିକ_ଗଣନା++] = ମୂଲ୍ୟଗୁଡ଼ିକ_Buf+ମୂଲ୍ୟଗୁଡ଼ିକ_Index;
                    ମୂଲ୍ୟଗୁଡ଼ିକ_Index += kLen;
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    ମୂଲ୍ୟଗୁଡ଼ିକ_Buf[ମୂଲ୍ୟଗୁଡ଼ିକ_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    ମୂଲ୍ୟଗୁଡ଼ିକ_Buf[ମୂଲ୍ୟଗୁଡ଼ିକ_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#ଯଦି defined(URES_ବୃକ୍ଷ_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        ମୂଲ୍ୟଗୁଡ଼ିକ_Index, ମୂଲ୍ୟଗୁଡ଼ିକ_ଗଣନା);
#endif
    ଫେରନ୍ତୁ uloc_openKeywordList(ମୂଲ୍ୟଗୁଡ଼ିକ_Buf, ମୂଲ୍ୟଗୁଡ଼ିକ_Index, status);
}
#ଯଦି 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ଅଚଳ UResourceBundle* res1, ଅଚଳ UResourceBundle* res2){
    ଯଦି(res1==nullptr || res2==nullptr){
        ଫେରନ୍ତୁ res1==res2; /* pointer comparison */
    }
    ଯଦି(res1->f_ଚାବି==nullptr||  res2->f_ଚାବି==nullptr){
        ଫେରନ୍ତୁ (res1->f_ଚାବି==res2->f_ଚାବି);
    }ଅନ୍ୟଥା{
        ଯଦି(uprv_strcmp(res1->f_ଚାବି, res2->f_ଚାବି)!=0){
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଯଦି(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        ଫେରନ୍ତୁ (res1->fData->fPath == res2->fData->fPath);
    }ଅନ୍ୟଥା{
        ଯଦି(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଯଦି(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(res1->fRes != res2->fRes){
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ଅଚଳ UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    ଯଦି(U_FAILURE(*status) || res == nullptr){
        ଫେରନ୍ତୁ nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    ଯଦି(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }ଅନ୍ୟଥା{
        ret = bundle;
    }
    ଫେରନ୍ତୁ ret;
}
U_CAPI ଅଚଳ UResourceBundle* U_EXPORT2
ures_getParentBundle(ଅଚଳ UResourceBundle* res){
    ଯଦି(res==nullptr){
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ res->fParentRes;
}
#endif

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ures_get_Version_By_ଚାବି(ଅଚଳ UResourceBundle* res, ଅଚଳ ଅକ୍ଷର *ଚାବି, UVersionInfo ver, UErrorCode *status) {
  ଅଚଳ char16_t *str;
  int32_t len;
  str = ures_get_String_By_ଚାବି(res, ଚାବି, &len, status);
  ଯଦି(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
