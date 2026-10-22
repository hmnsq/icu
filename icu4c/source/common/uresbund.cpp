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

#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "cwchar.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "unicode/uenum.h"
#അന്തര്ഭാവ "uenumimp.h"
#അന്തര്ഭാവ "ulocimp.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "uresdata.h"

using നാമാകാശ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
സ്ഥിതം UHashtable *cache = nullptr;
സ്ഥിതം icu::UInitOnce gCacheInitOnce {};

സ്ഥിതം UMutex resbMutex;

/* INTERNAL: hashes an entry  */
സ്ഥിതം int32_t U_CALLCONV hashEntry(സ്ഥിരം UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    തിരികെ uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
സ്ഥിതം UBool U_CALLCONV compareEntries(സ്ഥിരം UHashTok p1, സ്ഥിരം UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    തിരികെ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
സ്ഥിതം UBool chopLocale(അക്ഷരം *name) {
    അക്ഷരം *i = uprv_strrchr(name, '_');

    എങ്കിൽ(i != nullptr) {
        *i = '\0';
        തിരികെ സത്യം;
    }

    തിരികെ അസത്യം;
}

സ്ഥിതം UBool hasVariant(സ്ഥിരം അക്ഷരം* localeID) {
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
    തിരികെ sink.സംഖ്യ_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#അന്തര്ഭാവ "localefallback_data.h"

സ്ഥിതം സ്ഥിരം അക്ഷരം* performFallbackLookup(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                         സ്ഥിരം അക്ഷരം* താക്കോൽ_Strs,
                                         സ്ഥിരം അക്ഷരം* മൂല്യം_Strs U_LIFETIME_BOUND,
                                         സ്ഥിരം int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    സ്ഥിരം int32_t* bottom = lookupTable;
    സ്ഥിരം int32_t* മുകൾ = lookupTable + lookupTableLength;

    ആകുമ്പോൾ (bottom < മുകൾ) {
        // Effectively, divide by 2 and round down to an even index
        സ്ഥിരം int32_t* നടുവ് = bottom + (((മുകൾ - bottom) / 4) * 2);
        സ്ഥിരം അക്ഷരം* entry_താക്കോൽ = &(താക്കോൽ_Strs[*നടുവ്]);
        int32_t strcmpResult = uprv_strcmp(താക്കോൽ, entry_താക്കോൽ);
        എങ്കിൽ (strcmpResult == 0) {
            തിരികെ &(മൂല്യം_Strs[നടുവ്[1]]);
        } അല്ലെങ്കിൽ എങ്കിൽ (strcmpResult < 0) {
            മുകൾ = നടുവ്;
        } അല്ലെങ്കിൽ {
            bottom = നടുവ് + 2;
        }
    }
    തിരികെ nullptr;
}

സ്ഥിതം CharString getDefaultScript(സ്ഥിരം CharString& language, സ്ഥിരം CharString& region) {
    സ്ഥിരം അക്ഷരം* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    എങ്കിൽ (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        എങ്കിൽ (U_FAILURE(err)) {
            തിരികെ result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    എങ്കിൽ (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    എങ്കിൽ (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    തിരികെ result;
}

എണ്ണൽ UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_വേര്,
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
    URES_OPEN_LOCALE_വേര്,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
തരനിർവചനം എണ്ണൽ UResOpenType UResOpenType;

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
സ്ഥിതം തർക്കം getParentLocaleID(അക്ഷരം *name, സ്ഥിരം അക്ഷരം *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    എങ്കിൽ (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        തിരികെ chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    എങ്കിൽ (U_FAILURE(err)) {
        // hopefully this never happens...
        തിരികെ chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    എങ്കിൽ (openType == URES_OPEN_LOCALE_DEFAULT_വേര്) {
        സ്ഥിരം അക്ഷരം* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        എങ്കിൽ (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            തിരികെ സത്യം;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    എങ്കിൽ (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        എങ്കിൽ (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } അല്ലെങ്കിൽ {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        എങ്കിൽ (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } അല്ലെങ്കിൽ {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        എങ്കിൽ (openType != URES_OPEN_LOCALE_DEFAULT_വേര് || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } അല്ലെങ്കിൽ {
            തിരികെ അസത്യം;
        }
    } അല്ലെങ്കിൽ {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        തിരികെ സത്യം;
    } അല്ലെങ്കിൽ {
        തിരികെ അസത്യം;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
സ്ഥിതം UBool mayHaveParent(അക്ഷരം *name) {
    തിരികെ (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
സ്ഥിതം ശൂന്യം entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_എണ്ണം_Existing++;
    ആകുമ്പോൾ(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_എണ്ണം_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
സ്ഥിതം UResourceDataEntry *getFallbackData(
        സ്ഥിരം UResourceBundle *resBundle,
        സ്ഥിരം അക്ഷരം **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    എങ്കിൽ(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        തിരികെ nullptr;
    }
    എങ്കിൽ(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_താക്കോൽ(&(dataEntry->fData), dataEntry->fData.വേര്_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    എങ്കിൽ(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        ആകുമ്പോൾ(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            എങ്കിൽ(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_താക്കോൽ(&(dataEntry->fData), dataEntry->fData.വേര്_Res, &indexR, resTag);
            }
        }
    }

    എങ്കിൽ(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        തിരികെ nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    എങ്കിൽ(i>1) {
        എങ്കിൽ(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_വേര്_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } അല്ലെങ്കിൽ {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    തിരികെ dataEntry;
}

സ്ഥിതം ശൂന്യം
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    എങ്കിൽ(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    എങ്കിൽ(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    എങ്കിൽ(entry->fPool != nullptr) {
        --entry->fPool->f_എണ്ണം_Existing;
    }
    alias = entry->fAlias;
    എങ്കിൽ(alias != nullptr) {
        ആകുമ്പോൾ(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_എണ്ണം_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
സ്ഥിതം int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_അക്കം = 0;
    സ്ഥിരം UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    എങ്കിൽ (cache == nullptr) {
        തിരികെ 0;
    }

    ചെയ്യുക {
        deletedMore = അസത്യം;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        ആകുമ്പോൾ ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            എങ്കിൽ (resB->f_എണ്ണം_Existing == 0) {
                rb_Deleted_അക്കം++;
                deletedMore = സത്യം;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } ആകുമ്പോൾ(deletedMore);

    തിരികെ rb_Deleted_അക്കം;
}

#ifdef URES_DEBUG
#അന്തര്ഭാവ <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = അസത്യം;
  int32_t pos = UHASH_FIRST;
  സ്ഥിരം UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    എങ്കിൽ (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      തിരികെ അസത്യം;
    }

    ആകുമ്പോൾ ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=സത്യം;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (ശൂന്യം*)resB, resB->f_എണ്ണം_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (ശൂന്യം*)resB->fPool,
              (ശൂന്യം*)resB->fAlias,
              (ശൂന്യം*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_എണ്ണം(cache));
    തിരികെ cacheNotEmpty;
}

#endif

സ്ഥിതം UBool U_CALLCONV ures_cleanup()
{
    എങ്കിൽ (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    തിരികെ സത്യം;
}

/** INTERNAL: Initializes the cache for resources */
സ്ഥിതം ശൂന്യം U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
സ്ഥിതം ശൂന്യം initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

സ്ഥിതം ശൂന്യം setEntryName(UResourceDataEntry *res, സ്ഥിരം അക്ഷരം *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    എങ്കിൽ(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    എങ്കിൽ (len < static_cast<int32_t>(വലുപ്പം(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    അല്ലെങ്കിൽ {
        res->fName = static_cast<അക്ഷരം*>(uprv_malloc(len + 1));
    }
    എങ്കിൽ(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } അല്ലെങ്കിൽ {
        uprv_strcpy(res->fName, name);
    }
}

സ്ഥിതം UResourceDataEntry *
getPoolEntry(സ്ഥിരം അക്ഷരം *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
സ്ഥിതം UResourceDataEntry *init_entry(സ്ഥിരം അക്ഷരം *localeID, സ്ഥിരം അക്ഷരം *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    സ്ഥിരം അക്ഷരം *name;
    അക്ഷരം aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    എങ്കിൽ(U_FAILURE(*status)) {
        തിരികെ nullptr;
    }

    /* here we try to deduce the right locale name */
    എങ്കിൽ(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } അല്ലെങ്കിൽ എങ്കിൽ(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_വേര്_Locale_Name;
    } അല്ലെങ്കിൽ { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<അക്ഷരം*>(name);
    find.fPath = const_cast<അക്ഷരം*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    എങ്കിൽ(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(വലുപ്പം(UResourceDataEntry)));
        എങ്കിൽ(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }

        uprv_memset(r, 0, വലുപ്പം(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        എങ്കിൽ (U_FAILURE(*status)) {
            uprv_free(r);
            തിരികെ nullptr;
        }

        എങ്കിൽ(path != nullptr) {
            r->fPath = uprv_strdup(path);
            എങ്കിൽ(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                തിരികെ nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        എങ്കിൽ (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            എങ്കിൽ (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                തിരികെ nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } അല്ലെങ്കിൽ { /* if we have a regular entry */
            Resource aliasres;
            എങ്കിൽ (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                എങ്കിൽ (U_SUCCESS(*status)) {
                    സ്ഥിരം int32_t *poolIndexes = r->fPool->fData.p_വേര് + 1;
                    എങ്കിൽ(r->fData.p_വേര്[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_താക്കോലുകൾ = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } അല്ലെങ്കിൽ {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } അല്ലെങ്കിൽ {
                    r->fBogus = *status;
                }
            }
            എങ്കിൽ (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                എങ്കിൽ (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    സ്ഥിരം char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    എങ്കിൽ(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            എങ്കിൽ ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (ശൂന്യം *)r, r, &cacheStatus);
                എങ്കിൽ (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } അല്ലെങ്കിൽ {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    എങ്കിൽ(r != nullptr) {
        /* return the real bundle */
        ആകുമ്പോൾ(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_എണ്ണം_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        എങ്കിൽ(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    തിരികെ r;
}

സ്ഥിതം UResourceDataEntry *
getPoolEntry(സ്ഥിരം അക്ഷരം *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    എങ്കിൽ( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    തിരികെ poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
സ്ഥിതം UResourceDataEntry *
findFirstExisting(സ്ഥിരം അക്ഷരം* path, അക്ഷരം* name, സ്ഥിരം അക്ഷരം* defaultLocale, UResOpenType openType,
                  UBool *is_വേര്, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = അസത്യം;
    *foundParent = സത്യം; /* we're starting with a fresh name */
    അക്ഷരം origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    ആകുമ്പോൾ(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        എങ്കിൽ (U_FAILURE(*status)) {
            തിരികെ nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        എങ്കിൽ(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_എണ്ണം_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } അല്ലെങ്കിൽ {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_വേര് = static_cast<UBool>(uprv_strcmp(name, k_വേര്_Locale_Name) == 0);

        /*Fallback data stuff*/
        എങ്കിൽ (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } അല്ലെങ്കിൽ {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        എങ്കിൽ (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    തിരികെ r;
}

സ്ഥിതം ശൂന്യം ures_set_Is_അട്ടി_Object( UResourceBundle* resB, UBool state) {
    എങ്കിൽ(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } അല്ലെങ്കിൽ {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

സ്ഥിതം UBool ures_is_അട്ടി_Object(സ്ഥിരം UResourceBundle* resB) {
  തിരികെ((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?അസത്യം:സത്യം);
}


U_CFUNC ശൂന്യം ures_init_അട്ടി_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, വലുപ്പം(UResourceBundle));
  ures_set_Is_അട്ടി_Object(resB, സത്യം);
}

U_NAMESPACE_BEGIN

അട്ടി_UResource_Bundle::അട്ടി_UResource_Bundle() {
    ures_init_അട്ടി_Object(&bundle);
}

അട്ടി_UResource_Bundle::~അട്ടി_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

സ്ഥിതം UBool  // returns U_SUCCESS(*status)
load_Parents_Except_വേര്(UResourceDataEntry *&t1,
                      അക്ഷരം name[], int32_t nameCapacity,
                      UBool usingUSRData, അക്ഷരം usrDataPath[], UErrorCode *status) {
    എങ്കിൽ (U_FAILURE(*status)) { തിരികെ അസത്യം; }
    UBool checkParent = സത്യം;
    ആകുമ്പോൾ (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        എങ്കിൽ (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            സ്ഥിരം char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            എങ്കിൽ(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                എങ്കിൽ (uprv_strcmp(name, k_വേര്_Locale_Name) == 0) {
                    തിരികെ സത്യം;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        എങ്കിൽ (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            തിരികെ അസത്യം;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        എങ്കിൽ (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            എങ്കിൽ (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                തിരികെ അസത്യം;
            }
        }

        എങ്കിൽ (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } അല്ലെങ്കിൽ {
            t1->fParent = t2;
            എങ്കിൽ (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_എണ്ണം_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    തിരികെ സത്യം;
}

സ്ഥിതം UBool  // returns U_SUCCESS(*status)
തിരുകുക_വേര്_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    എങ്കിൽ (U_FAILURE(*status)) { തിരികെ അസത്യം; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_വേര്_Locale_Name, t1->fPath, &parentStatus);
    എങ്കിൽ (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        തിരികെ അസത്യം;
    }
    t1->fParent = t2;
    t1 = t2;
    തിരികെ സത്യം;
}

സ്ഥിതം UResourceDataEntry *entryOpen(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = അസത്യം;
    UBool is_വേര് = അസത്യം;
    UBool hasRealData = അസത്യം;
    UBool hasChopped = സത്യം;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    അക്ഷരം name[ULOC_FULLNAME_CAPACITY];
    അക്ഷരം usrDataPath[96];

    initCache(status);

    എങ്കിൽ(U_FAILURE(*status)) {
        തിരികെ nullptr;
    }

    uprv_strncpy(name, localeID, വലുപ്പം(name) - 1);
    name[വലുപ്പം(name) - 1] = 0;

    എങ്കിൽ ( usingUSRData ) {
        എങ്കിൽ ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } അല്ലെങ്കിൽ {
            uprv_strncpy(usrDataPath, path, വലുപ്പം(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[വലുപ്പം(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    സ്ഥിരം അക്ഷരം *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_വേര്, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    എങ്കിൽ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        പോകുക finish;
    }

    എങ്കിൽ(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = സത്യം;
        എങ്കിൽ ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            എങ്കിൽ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                പോകുക finish;
            }
            എങ്കിൽ ( u1 != nullptr ) {
                എങ്കിൽ(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } അല്ലെങ്കിൽ {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_എണ്ണം_Existing = 0;
                }
            }
        }
        എങ്കിൽ ((hasChopped || mayHaveParent(name)) && !is_വേര്) {
            എങ്കിൽ (!load_Parents_Except_വേര്(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                പോകുക finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    എങ്കിൽ(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_വേര് && !isDefault && !is_വേര്) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_വേര്, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        എങ്കിൽ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            പോകുക finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        എങ്കിൽ(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = സത്യം;
            isDefault = സത്യം;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            എങ്കിൽ ((hasChopped || mayHaveParent(name)) && !is_വേര്) {
                എങ്കിൽ (!load_Parents_Except_വേര്(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    പോകുക finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    എങ്കിൽ(r == nullptr) {
        uprv_strcpy(name, k_വേര്_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_വേര്, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        എങ്കിൽ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            പോകുക finish;
        }
        എങ്കിൽ(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = സത്യം;
        } അല്ലെങ്കിൽ { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            പോകുക finish;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(!is_വേര് && uprv_strcmp(t1->fName, k_വേര്_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        എങ്കിൽ (!തിരുകുക_വേര്_Bundle(t1, status)) {
            പോകുക finish;
        }
        എങ്കിൽ(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    ആകുമ്പോൾ(r != nullptr && !is_വേര് && t1->fParent != nullptr) {
        t1->fParent->f_എണ്ണം_Existing++;
        t1 = t1->fParent;
    }

finish:
    എങ്കിൽ(U_SUCCESS(*status)) {
        എങ്കിൽ(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        തിരികെ r;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
സ്ഥിതം UResourceDataEntry *
entryOpenDirect(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* localeID, UErrorCode* status) {
    initCache(status);
    എങ്കിൽ(U_FAILURE(*status)) {
        തിരികെ nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    എങ്കിൽ (localeID == nullptr) {
        localeID = uloc_getDefault();
    } അല്ലെങ്കിൽ എങ്കിൽ (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_വേര്_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    എങ്കിൽ(U_SUCCESS(*status)) {
        എങ്കിൽ(r->fBogus != U_ZERO_ERROR) {
            r->f_എണ്ണം_Existing--;
            r = nullptr;
        }
    } അല്ലെങ്കിൽ {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    എങ്കിൽ(r != nullptr && uprv_strcmp(localeID, k_വേര്_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        അക്ഷരം name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        എങ്കിൽ(!chopLocale(name) || uprv_strcmp(name, k_വേര്_Locale_Name) == 0 ||
                load_Parents_Except_വേര്(t1, name, UPRV_LENGTHOF(name), അസത്യം, nullptr, status)) {
            എങ്കിൽ(uprv_strcmp(t1->fName, k_വേര്_Locale_Name) != 0 && t1->fParent == nullptr) {
                തിരുകുക_വേര്_Bundle(t1, status);
            }
        }
        എങ്കിൽ(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    എങ്കിൽ(r != nullptr) {
        // TODO: Does this ever loop?
        ആകുമ്പോൾ(t1->fParent != nullptr) {
            t1->fParent->f_എണ്ണം_Existing++;
            t1 = t1->fParent;
        }
    }
    തിരികെ r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
സ്ഥിതം ശൂന്യം entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    ആകുമ്പോൾ(resB != nullptr) {
        p = resB->fParent;
        resB->f_എണ്ണം_Existing--;

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

സ്ഥിതം ശൂന്യം entryClose(UResourceDataEntry *resB) {
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
സ്ഥിതം ശൂന്യം ures_appendResPath(UResourceBundle *resB, സ്ഥിരം അക്ഷരം* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    എങ്കിൽ(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    എങ്കിൽ(RES_BUFSIZE <= resB->fResPathLen+1) {
        എങ്കിൽ(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<അക്ഷരം*>(uprv_malloc((resB->fResPathLen + 1) * വലുപ്പം(അക്ഷരം)));
            /* Check that memory was allocated correctly. */
            എങ്കിൽ (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } അല്ലെങ്കിൽ {
            അക്ഷരം* temp = static_cast<അക്ഷരം*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * വലുപ്പം(അക്ഷരം)));
            /* Check that memory was reallocated correctly. */
            എങ്കിൽ (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

സ്ഥിതം ശൂന്യം ures_freeResPath(UResourceBundle *resB) {
    എങ്കിൽ (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

സ്ഥിതം ശൂന്യം
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    എങ്കിൽ(resB != nullptr) {
        എങ്കിൽ(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        എങ്കിൽ(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        എങ്കിൽ(ures_is_അട്ടി_Object(resB) == അസത്യം && freeBundleObj) {
            uprv_free(resB);
        }
#എങ്കിൽ 0 /*U_DEBUG*/
        അല്ലെങ്കിൽ {
            /* poison the data */
            uprv_memset(resB, -1, വലുപ്പം(UResourceBundle));
        }
#endif
    }
}

U_CAPI ശൂന്യം  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, സത്യം);
}

നാമാകാശ {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  സ്ഥിരം അക്ഷരം* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(സ്ഥിരം ResourceData& resData,
                               Resource r,
                               സ്ഥിരം അക്ഷരം* താക്കോൽ,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               സ്ഥിരം അക്ഷരം* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    എങ്കിൽ (U_FAILURE(*status)) { തിരികെ resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    സ്ഥിരം char16_t *alias = res_getAlias(&resData, r, &len);
    എങ്കിൽ(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ resB;
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
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }

    // We have an alias, now let's cut it up.
    സ്ഥിരം അക്ഷരം *path = nullptr, *locale = nullptr, *താക്കോൽ_Path = nullptr;
    എങ്കിൽ(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        അക്ഷരം *chAliasData = chAlias.data();
        അക്ഷരം *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        എങ്കിൽ(sep != nullptr) {
            *sep++ = 0;
        }
        എങ്കിൽ(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            താക്കോൽ_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            എങ്കിൽ (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } അല്ലെങ്കിൽ {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                എങ്കിൽ(sep != nullptr) {
                    *sep++ = 0;
                }
                താക്കോൽ_Path = sep;
            }
        }
    } അല്ലെങ്കിൽ {
        // No path, start with a locale.
        അക്ഷരം *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        എങ്കിൽ(sep != nullptr) {
            *sep++ = 0;
        }
        താക്കോൽ_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    എങ്കിൽ (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } അല്ലെങ്കിൽ {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        എങ്കിൽ(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            തിരികെ resB;
        }
        dataEntry = mainRes->fData;
    }

    സ്ഥിരം അക്ഷരം* temp = nullptr;
    എങ്കിൽ(താക്കോൽ_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.വേര്_Res;
        എങ്കിൽ(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            എങ്കിൽ (U_FAILURE(*status)) {
                തിരികെ nullptr;
            }
            അക്ഷരം *a_താക്കോൽ = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_താക്കോൽ, &temp);
        }
        എങ്കിൽ(താക്കോൽ) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(താക്കോൽ, *status);
            എങ്കിൽ (U_FAILURE(*status)) {
                തിരികെ nullptr;
            }
            അക്ഷരം *a_താക്കോൽ = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_താക്കോൽ, &temp);
        } അല്ലെങ്കിൽ എങ്കിൽ(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            എങ്കിൽ(URES_IS_TABLE(type)) {
                സ്ഥിരം അക്ഷരം *a_താക്കോൽ;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_താക്കോൽ);
            } അല്ലെങ്കിൽ { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        എങ്കിൽ(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } അല്ലെങ്കിൽ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } അല്ലെങ്കിൽ {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(താക്കോൽ_Path, *status);
        എങ്കിൽ (U_FAILURE(*status)) {
            തിരികെ nullptr;
        }
        അക്ഷരം *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        വേണ്ടി(;;) {
            r = dataEntry->fData.വേര്_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            ആകുമ്പോൾ(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                എങ്കിൽ(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    നിർത്തുക;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                എങ്കിൽ (U_FAILURE(*status)) {
                    നിർത്തുക;
                }
                എങ്കിൽ (temp == nullptr || uprv_strcmp(താക്കോൽ_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, താക്കോൽ_Path, static_cast<int32_t>(uprv_strlen(താക്കോൽ_Path)), status);
                    എങ്കിൽ(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    എങ്കിൽ (U_FAILURE(*status)) {
                        നിർത്തുക;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            എങ്കിൽ (U_FAILURE(*status) || r != RES_BOGUS) {
                നിർത്തുക;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            എങ്കിൽ (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                നിർത്തുക;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, താക്കോൽ_Path);
        }
    }
    എങ്കിൽ(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    തിരികെ resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  സ്ഥിരം അക്ഷരം* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    എങ്കിൽ(status == nullptr || U_FAILURE(*status)) {
        തിരികെ resB;
    }
    എങ്കിൽ (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    എങ്കിൽ(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        എങ്കിൽ(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            തിരികെ resB;
        }
        തിരികെ getAliasTargetAsResourceBundle(
            dataEntry->fData, r, താക്കോൽ, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    എങ്കിൽ(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(വലുപ്പം(UResourceBundle)));
        എങ്കിൽ (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }
        ures_set_Is_അട്ടി_Object(resB, അസത്യം);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        എങ്കിൽ(resB->fVersion != nullptr) {
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
        എങ്കിൽ(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = അസത്യം;
    resB->f_Is_മുകൾ_Level = അസത്യം;
    resB->fIndex = -1;
    resB->f_താക്കോൽ = താക്കോൽ; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    എങ്കിൽ(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    എങ്കിൽ(താക്കോൽ != nullptr) {
        ures_appendResPath(resB, താക്കോൽ, static_cast<int32_t>(uprv_strlen(താക്കോൽ)), status);
        എങ്കിൽ(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(idx >= 0) {
        അക്ഷരം buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        എങ്കിൽ(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, വലുപ്പം(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_എണ്ണം_Array_ഇനങ്ങൾ(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    തിരികെ resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  സ്ഥിരം UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    തിരികെ init_resb_result(
        dataEntry, r, താക്കോൽ, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               സ്ഥിരം UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_അട്ടി_Object;
    എങ്കിൽ(U_FAILURE(*status) || r == original) {
        തിരികെ r;
    }
    എങ്കിൽ(original != nullptr) {
        എങ്കിൽ(r == nullptr) {
            is_അട്ടി_Object = അസത്യം;
            r = static_cast<UResourceBundle*>(uprv_malloc(വലുപ്പം(UResourceBundle)));
            /* test for nullptr */
            എങ്കിൽ (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ nullptr;
            }
        } അല്ലെങ്കിൽ {
            is_അട്ടി_Object = ures_is_അട്ടി_Object(r);
            ures_closeBundle(r, അസത്യം);
        }
        uprv_memcpy(r, original, വലുപ്പം(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        എങ്കിൽ(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_അട്ടി_Object(r, is_അട്ടി_Object);
        എങ്കിൽ(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    തിരികെ r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI സ്ഥിരം char16_t* U_EXPORT2 ures_getString(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    സ്ഥിരം char16_t *s;
    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    എങ്കിൽ (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ s;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം*
ures_toUTF8String(സ്ഥിരം char16_t* s16,
                  int32_t length16,
                  അക്ഷരം* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (pLength != nullptr) {
        capacity = *pLength;
    } അല്ലെങ്കിൽ {
        capacity = 0;
    }
    എങ്കിൽ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    എങ്കിൽ (length16 == 0) {
        /* empty string, return as read-only pointer */
        എങ്കിൽ (pLength != nullptr) {
            *pLength = 0;
        }
        എങ്കിൽ (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            തിരികെ dest;
        } അല്ലെങ്കിൽ {
            തിരികെ "";
        }
    } അല്ലെങ്കിൽ {
        /* We need to transform the string to the destination buffer. */
        എങ്കിൽ (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            തിരികെ u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        എങ്കിൽ (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            എങ്കിൽ (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        തിരികെ u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_getUTF8String(സ്ഥിരം UResourceBundle* resB,
                   അക്ഷരം* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    സ്ഥിരം char16_t *s16 = ures_getString(resB, &length16, status);
    തിരികെ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI സ്ഥിരം uint8_t* U_EXPORT2
ures_get_ദ്വിമാന(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  സ്ഥിരം uint8_t *p;
  എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  എങ്കിൽ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ nullptr;
  }
  p = res_get_ദ്വിമാന({resB}, &resB->getResData(), resB->fRes, len);
  എങ്കിൽ (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  തിരികെ p;
}

U_CAPI സ്ഥിരം int32_t* U_EXPORT2
ures_getIntVector(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  സ്ഥിരം int32_t *p;
  എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  എങ്കിൽ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  എങ്കിൽ (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  തിരികെ p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(സ്ഥിരം UResourceBundle* resB, UErrorCode *status) {
  എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
    തിരികെ 0xffffffff;
  }
  എങ്കിൽ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0xffffffff;
  }
  എങ്കിൽ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    തിരികെ 0xffffffff;
  }
  തിരികെ res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(സ്ഥിരം UResourceBundle* resB, UErrorCode *status) {
  എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
    തിരികെ 0xffffffff;
  }
  എങ്കിൽ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0xffffffff;
  }
  എങ്കിൽ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    തിരികെ 0xffffffff;
  }
  തിരികെ res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(സ്ഥിരം UResourceBundle *resB) {
  എങ്കിൽ(resB == nullptr) {
    തിരികെ URES_NONE;
  }
  തിരികെ res_getPublicType(resB->fRes);
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_get_താക്കോൽ(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  എങ്കിൽ(resB == nullptr) {
    തിരികെ nullptr;
  }
  തിരികെ(resB->f_താക്കോൽ);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(സ്ഥിരം UResourceBundle *resB) {
  എങ്കിൽ(resB == nullptr) {
    തിരികെ 0;
  }
  
  തിരികെ resB->fSize;
}

സ്ഥിതം സ്ഥിരം char16_t*
ures_getStringWithAlias(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  എങ്കിൽ(RES_GET_TYPE(r) == URES_ALIAS) {
    സ്ഥിരം char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    തിരികെ result;
  } അല്ലെങ്കിൽ {
    തിരികെ res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI ശൂന്യം U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  എങ്കിൽ(resB == nullptr) {
    തിരികെ;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(സ്ഥിരം UResourceBundle *resB) {
  എങ്കിൽ(resB == nullptr) {
    തിരികെ അസത്യം;
  }
  തിരികെ resB->fIndex < resB->fSize-1;
}

U_CAPI സ്ഥിരം char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   സ്ഥിരം അക്ഷരം** താക്കോൽ,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  എങ്കിൽ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ nullptr;
  }
  
  എങ്കിൽ(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } അല്ലെങ്കിൽ {
    resB->fIndex++;
    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(resB->fRes)) {
    അവസ്ഥ URES_STRING:
    അവസ്ഥ URES_STRING_V2:
      തിരികെ res_getString({resB}, &resB->getResData(), resB->fRes, len);
    അവസ്ഥ URES_TABLE:
    അവസ്ഥ URES_TABLE16:
    അവസ്ഥ URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, താക്കോൽ);
      എങ്കിൽ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      തിരികെ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    അവസ്ഥ URES_ARRAY:
    അവസ്ഥ URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      എങ്കിൽ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      തിരികെ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    അവസ്ഥ URES_ALIAS:
      തിരികെ ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    അവസ്ഥ URES_INT:
    അവസ്ഥ URES_ദ്വിമാന:
    അവസ്ഥ URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    സ്ഥിരസ്ഥിതി:
      തിരികെ nullptr;
    }
  }

  തിരികെ nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    സ്ഥിരം അക്ഷരം *താക്കോൽ = nullptr;
    Resource r = RES_BOGUS;

    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            തിരികെ fillIn;
    }
    എങ്കിൽ(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            തിരികെ fillIn;
    }

    എങ്കിൽ(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } അല്ലെങ്കിൽ {
        resB->fIndex++;
        തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(resB->fRes)) {
        അവസ്ഥ URES_INT:
        അവസ്ഥ URES_ദ്വിമാന:
        അവസ്ഥ URES_STRING:
        അവസ്ഥ URES_STRING_V2:
        അവസ്ഥ URES_INT_VECTOR:
            തിരികെ ures_copyResb(fillIn, resB, status);
        അവസ്ഥ URES_TABLE:
        അവസ്ഥ URES_TABLE16:
        അവസ്ഥ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &താക്കോൽ);
            എങ്കിൽ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            തിരികെ init_resb_result(resB->fData, r, താക്കോൽ, resB->fIndex, resB, fillIn, status);
        അവസ്ഥ URES_ARRAY:
        അവസ്ഥ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            എങ്കിൽ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            തിരികെ init_resb_result(resB->fData, r, താക്കോൽ, resB->fIndex, resB, fillIn, status);
        സ്ഥിരസ്ഥിതി:
            /*return nullptr;*/
            തിരികെ fillIn;
        }
    }
    /*return nullptr;*/
    തിരികെ fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(സ്ഥിരം UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    സ്ഥിരം അക്ഷരം* താക്കോൽ = nullptr;
    Resource r = RES_BOGUS;

    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        തിരികെ fillIn;
    }
    എങ്കിൽ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        തിരികെ fillIn;
    }

    എങ്കിൽ(indexR >= 0 && resB->fSize > indexR) {
        തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(resB->fRes)) {
        അവസ്ഥ URES_INT:
        അവസ്ഥ URES_ദ്വിമാന:
        അവസ്ഥ URES_STRING:
        അവസ്ഥ URES_STRING_V2:
        അവസ്ഥ URES_INT_VECTOR:
            തിരികെ ures_copyResb(fillIn, resB, status);
        അവസ്ഥ URES_TABLE:
        അവസ്ഥ URES_TABLE16:
        അവസ്ഥ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &താക്കോൽ);
            എങ്കിൽ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            തിരികെ init_resb_result(resB->fData, r, താക്കോൽ, indexR, resB, fillIn, status);
        അവസ്ഥ URES_ARRAY:
        അവസ്ഥ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            എങ്കിൽ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            തിരികെ init_resb_result(resB->fData, r, താക്കോൽ, indexR, resB, fillIn, status);
        സ്ഥിരസ്ഥിതി:
            /*return nullptr;*/
            തിരികെ fillIn;
        }
    } അല്ലെങ്കിൽ {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    തിരികെ fillIn;
}

U_CAPI സ്ഥിരം char16_t* U_EXPORT2
ures_getStringByIndex(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    സ്ഥിരം അക്ഷരം* താക്കോൽ = nullptr;
    Resource r = RES_BOGUS;

    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    എങ്കിൽ(indexS >= 0 && resB->fSize > indexS) {
        തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(resB->fRes)) {
        അവസ്ഥ URES_STRING:
        അവസ്ഥ URES_STRING_V2:
            തിരികെ res_getString({resB}, &resB->getResData(), resB->fRes, len);
        അവസ്ഥ URES_TABLE:
        അവസ്ഥ URES_TABLE16:
        അവസ്ഥ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &താക്കോൽ);
            എങ്കിൽ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            തിരികെ ures_getStringWithAlias(resB, r, indexS, len, status);
        അവസ്ഥ URES_ARRAY:
        അവസ്ഥ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            എങ്കിൽ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            തിരികെ ures_getStringWithAlias(resB, r, indexS, len, status);
        അവസ്ഥ URES_ALIAS:
            തിരികെ ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        അവസ്ഥ URES_INT:
        അവസ്ഥ URES_ദ്വിമാന:
        അവസ്ഥ URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          നിർത്തുക;
        }
    } അല്ലെങ്കിൽ {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    തിരികെ nullptr;
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_getUTF8StringByIndex(സ്ഥിരം UResourceBundle* resB,
                          int32_t idx,
                          അക്ഷരം* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    സ്ഥിരം char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    തിരികെ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(സ്ഥിരം അക്ഷരം* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  അക്ഷരം *packageName = nullptr;
  അക്ഷരം *pathToResource = nullptr, *save = nullptr;
  അക്ഷരം *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  എങ്കിൽ(status == nullptr || U_FAILURE(*status)) {
    തിരികെ result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (അക്ഷരം *)uprv_malloc(length*വലുപ്പം(അക്ഷരം));
  /* test for nullptr */
  എങ്കിൽ(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  എങ്കിൽ(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    എങ്കിൽ(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } അല്ലെങ്കിൽ {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  എങ്കിൽ(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  എങ്കിൽ(U_SUCCESS(*status)) {
    എങ്കിൽ(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } അല്ലെങ്കിൽ {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  തിരികെ result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(സ്ഥിരം UResourceBundle* resB,
                     അക്ഷരം* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  സ്ഥിരം അക്ഷരം *താക്കോൽ;

  എങ്കിൽ(status == nullptr || U_FAILURE(*status)) {
    തിരികെ result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  ചെയ്യുക {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &താക്കോൽ); 
    എങ്കിൽ(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, താക്കോൽ, -1, resB, fillIn, status);
        resB = result;
    } അല്ലെങ്കിൽ {
        *status = U_MISSING_RESOURCE_ERROR;
        നിർത്തുക;
    }
  } ആകുമ്പോൾ(*path); /* there is more stuff in the path */

  തിരികെ result;
}

U_CAPI സ്ഥിരം char16_t* U_EXPORT2
ures_get_String_By_താക്കോൽ_With_Fallback(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND,
                                സ്ഥിരം അക്ഷരം* in_താക്കോൽ,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle അട്ടി_2;
    സ്ഥിരം char16_t* retVal = nullptr;
    ures_init_അട്ടി_Object(&അട്ടി_2);
    ures_get_By_താക്കോൽ_With_Fallback(resB, in_താക്കോൽ, &അട്ടി_2, status);
    int32_t length;
    retVal = ures_getString(&അട്ടി_2, &length, status);
    ures_close(&അട്ടി_2);
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    എങ്കിൽ (len != nullptr) {
        *len = length;
    }
    തിരികെ retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
സ്ഥിതം Resource get_Table_Item_By_താക്കോൽ_Path(സ്ഥിരം ResourceData *pResData, Resource table, സ്ഥിരം അക്ഷരം *താക്കോൽ) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(താക്കോൽ, errorCode);
  എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ RES_BOGUS; }
  അക്ഷരം *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  ആകുമ്പോൾ (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    അക്ഷരം *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    എങ്കിൽ (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } അല്ലെങ്കിൽ {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    സ്ഥിരം അക്ഷരം *pathP = pathPart;
    resource = res_get_Table_Item_By_താക്കോൽ(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  എങ്കിൽ (*pathPart) {
    തിരികെ RES_BOGUS;
  }
  തിരികെ resource;
}

സ്ഥിതം ശൂന്യം createPath(സ്ഥിരം അക്ഷരം* origResPath,
                       int32_t     origResPathLen,
                       സ്ഥിരം അക്ഷരം* resPath,
                       int32_t     resPathLen,
                       സ്ഥിരം അക്ഷരം* in_താക്കോൽ,
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
    സ്ഥിരം അക്ഷരം* താക്കോൽ = in_താക്കോൽ;
    എങ്കിൽ (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        എങ്കിൽ (U_SUCCESS(*status)) {
            സ്ഥിരം അക്ഷരം* resPathLimit = resPath + resPathLen;
            സ്ഥിരം അക്ഷരം* origResPathLimit = origResPath + origResPathLen;
            സ്ഥിരം അക്ഷരം* resPathPtr = resPath;
            സ്ഥിരം അക്ഷരം* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            ആകുമ്പോൾ (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                ആകുമ്പോൾ (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                എങ്കിൽ (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                ആകുമ്പോൾ (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                എങ്കിൽ (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            ആകുമ്പോൾ (resPathPtr < resPathLimit && *താക്കോൽ != '\0') {
                ആകുമ്പോൾ (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                എങ്കിൽ (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ആകുമ്പോൾ (*താക്കോൽ != '\0' && *താക്കോൽ != RES_PATH_SEPARATOR) {
                    ++താക്കോൽ;
                }
                എങ്കിൽ (*താക്കോൽ == RES_PATH_SEPARATOR) {
                    ++താക്കോൽ;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(താക്കോൽ, *status);
    } അല്ലെങ്കിൽ {
        path.append(in_താക്കോൽ, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_താക്കോൽ_With_Fallback(സ്ഥിരം UResourceBundle* resB,
                          സ്ഥിരം അക്ഷരം* in_താക്കോൽ,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, വേര്_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        തിരികെ fillIn;
    }
    എങ്കിൽ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    എങ്കിൽ(URES_IS_TABLE(type)) {
        സ്ഥിരം അക്ഷരം* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_താക്കോൽ_Path(&resB->getResData(), resB->fRes, in_താക്കോൽ);
        സ്ഥിരം അക്ഷരം* താക്കോൽ = in_താക്കോൽ;
        തർക്കം did_വേര്_Once = അസത്യം;
        എങ്കിൽ(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            അക്ഷരം *myPath = nullptr;
            സ്ഥിരം അക്ഷരം* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            ആകുമ്പോൾ(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_വേര്_Once)) { /* Otherwise, we'll look in parents */
                എങ്കിൽ (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } അല്ലെങ്കിൽ {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_വേര്_Once = സത്യം;
                }
                വേര്_Res = dataEntry->fData.വേര്_Res;

                എങ്കിൽ(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_താക്കോൽ, path, status);
                    എങ്കിൽ (U_FAILURE(*status)) {
                        ures_close(helper);
                        തിരികെ fillIn;
                    }
                    myPath = path.data();
                    താക്കോൽ = in_താക്കോൽ;
                    ചെയ്യുക {
                        res = res_findResource(&(dataEntry->fData), വേര്_Res, &myPath, &താക്കോൽ);
                        എങ്കിൽ (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            എങ്കിൽ(helper) {
                              dataEntry = helper->fData;
                              വേര്_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } അല്ലെങ്കിൽ {
                              നിർത്തുക;
                            }
                        } അല്ലെങ്കിൽ എങ്കിൽ (res == RES_BOGUS) {
                            നിർത്തുക;
                        }
                    } ആകുമ്പോൾ(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            എങ്കിൽ(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                എങ്കിൽ(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_വേര്_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } അല്ലെങ്കിൽ {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, താക്കോൽ, -1, resB, fillIn, status);
                എങ്കിൽ (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_താക്കോൽ, path, status);
                } അല്ലെങ്കിൽ {
                    സ്ഥിരം അക്ഷരം* separator = nullptr;
                    എങ്കിൽ (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    എങ്കിൽ (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_താക്കോൽ, path, status);
                    } അല്ലെങ്കിൽ {
                        createPath(origResPath, origResPathLen, "", 0, in_താക്കോൽ, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                എങ്കിൽ(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } അല്ലെങ്കിൽ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } അല്ലെങ്കിൽ {
            fillIn = init_resb_result(resB->fData, res, താക്കോൽ, -1, resB, fillIn, status);
        }
    } 
    അല്ലെങ്കിൽ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    തിരികെ fillIn;
}

നാമാകാശ {

ശൂന്യം get_All_ഇനങ്ങൾ_With_Fallback(
        സ്ഥിരം UResourceBundle *bundle, Resource_Data_മൂല്യം &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
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
    sink.put(bundle->f_താക്കോൽ, value, !hasParent, errorCode);
    എങ്കിൽ (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        അട്ടി_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_മുകൾ_Level = സത്യം;
        parentRef.fRes = parentRef.getResData().വേര്_Res;
        parentRef.fSize = res_എണ്ണം_Array_ഇനങ്ങൾ(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        അട്ടി_UResource_Bundle containerBundle;
        സ്ഥിരം UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        എങ്കിൽ (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } അല്ലെങ്കിൽ {
            rb = ures_get_By_താക്കോൽ_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        എങ്കിൽ (U_SUCCESS(pathErrorCode)) {
            get_All_ഇനങ്ങൾ_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

ഘടന GetAllChildrenSink : പൊതു ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    മായാ ~GetAllChildrenSink() override;
    മായാ ശൂന്യം put(സ്ഥിരം അക്ഷരം *താക്കോൽ, Resource_മൂല്യം &value, UBool is_വേര്,
           UErrorCode &errorCode) override {
        ResourceTable ഇനങ്ങൾ_Table = value.getTable(errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        വേണ്ടി (int32_t i = 0; ഇനങ്ങൾ_Table.get_താക്കോൽ_And_മൂല്യം(i, താക്കോൽ, value); ++i) {
            എങ്കിൽ (value.getType() == URES_ALIAS) {
                Resource_Data_മൂല്യം& rdv = static_cast<Resource_Data_മൂല്യം&>(value);
                അട്ടി_UResource_Bundle അട്ടി_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          അട്ടി_Temp_Bundle.getAlias(), &errorCode);
                എങ്കിൽ (U_SUCCESS(errorCode)) {
                    Resource_Data_മൂല്യം aliased_മൂല്യം;
                    aliased_മൂല്യം.setData(aliasRB->getResData());
                    aliased_മൂല്യം.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_മൂല്യം.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    എങ്കിൽ (aliased_മൂല്യം.getType() != URES_TABLE) {
                        dest.put(താക്കോൽ, aliased_മൂല്യം, is_വേര്, errorCode);
                    } അല്ലെങ്കിൽ {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_മൂല്യം_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        സ്ഥിരം അക്ഷരം* parent_താക്കോൽ = താക്കോൽ; // dest.put() changes the key
                        dest.put(parent_താക്കോൽ, aliased_മൂല്യം, is_വേര്, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        ആകുമ്പോൾ (aliased_മൂല്യം_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            അക്ഷരം* localPathAsCharPtr = localPath.data();
                            സ്ഥിരം അക്ഷരം* child_താക്കോൽ;
                            entry = entry->fParent;
                            res = entry->fData.വേര്_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_താക്കോൽ);
                            എങ്കിൽ (newRes != RES_BOGUS) {
                                aliased_മൂല്യം.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_മൂല്യം.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_മൂല്യം_Type = aliased_മൂല്യം.getType();
                                എങ്കിൽ (aliased_മൂല്യം_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_മൂല്യം& rdv2 = static_cast<Resource_Data_മൂല്യം&>(aliased_മൂല്യം);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             അട്ടി_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_മൂല്യം.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_മൂല്യം.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_മൂല്യം_Type = aliased_മൂല്യം.getType();
                                }
                                എങ്കിൽ (aliased_മൂല്യം_Type == URES_TABLE) {
                                    dest.put(parent_താക്കോൽ, aliased_മൂല്യം, is_വേര്, errorCode);
                                } അല്ലെങ്കിൽ {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    തിരികെ;
                                }
                            }
                        }
                    }
                }
            } അല്ലെങ്കിൽ {
                dest.put(താക്കോൽ, value, is_വേര്, errorCode);
            }
            എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI ശൂന്യം U_EXPORT2
ures_getAllChildrenWithFallback(സ്ഥിരം UResourceBundle *bundle, സ്ഥിരം അക്ഷരം *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_ഇനങ്ങൾ_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI ശൂന്യം U_EXPORT2
ures_get_മൂല്യം_With_Fallback(സ്ഥിരം UResourceBundle *bundle, സ്ഥിരം അക്ഷരം *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_മൂല്യം &value, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    എങ്കിൽ (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    സ്ഥിരം UResourceBundle *rb;
    എങ്കിൽ (*path == 0) {
        // empty path
        rb = bundle;
    } അല്ലെങ്കിൽ {
        rb = ures_get_By_താക്കോൽ_With_Fallback(bundle, path, tempFillIn, &errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) {
            തിരികെ;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI ശൂന്യം U_EXPORT2
ures_get_All_ഇനങ്ങൾ_With_Fallback(സ്ഥിരം UResourceBundle *bundle, സ്ഥിരം അക്ഷരം *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    എങ്കിൽ (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    അട്ടി_UResource_Bundle അട്ടി_Bundle;
    സ്ഥിരം UResourceBundle *rb;
    എങ്കിൽ (*path == 0) {
        // empty path
        rb = bundle;
    } അല്ലെങ്കിൽ {
        rb = ures_get_By_താക്കോൽ_With_Fallback(bundle, path, അട്ടി_Bundle.getAlias(), &errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) {
            തിരികെ;
        }
    }
    // Get all table items with fallback.
    Resource_Data_മൂല്യം value;
    get_All_ഇനങ്ങൾ_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_താക്കോൽ(സ്ഥിരം UResourceBundle* resB,
              സ്ഥിരം അക്ഷരം* in_താക്കോൽ,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    സ്ഥിരം അക്ഷരം *താക്കോൽ = in_താക്കോൽ;

    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        തിരികെ fillIn;
    }
    എങ്കിൽ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    എങ്കിൽ(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_താക്കോൽ(&resB->getResData(), resB->fRes, &t, &താക്കോൽ);
        എങ്കിൽ(res == RES_BOGUS) {
            താക്കോൽ = in_താക്കോൽ;
            എങ്കിൽ(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &താക്കോൽ, &res, status);
                എങ്കിൽ(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    തിരികെ init_resb_result(dataEntry, res, താക്കോൽ, -1, resB, fillIn, status);
                } അല്ലെങ്കിൽ {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } അല്ലെങ്കിൽ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } അല്ലെങ്കിൽ {
            തിരികെ init_resb_result(resB->fData, res, താക്കോൽ, -1, resB, fillIn, status);
        }
    } 
#എങ്കിൽ 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    അല്ലെങ്കിൽ എങ്കിൽ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == സത്യം) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &താക്കോൽ, &res, status);
        എങ്കിൽ(U_SUCCESS(*status)) {
            തിരികെ init_resb_result(dataEntry, res, താക്കോൽ, resB, fillIn, status);
        } അല്ലെങ്കിൽ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    അല്ലെങ്കിൽ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ fillIn;
}

U_CAPI സ്ഥിരം char16_t* U_EXPORT2
ures_get_String_By_താക്കോൽ(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND,
                    സ്ഥിരം അക്ഷരം* in_താക്കോൽ,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    സ്ഥിരം അക്ഷരം* താക്കോൽ = in_താക്കോൽ;

    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    എങ്കിൽ(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_താക്കോൽ(&resB->getResData(), resB->fRes, &t, &താക്കോൽ);

        എങ്കിൽ(res == RES_BOGUS) {
            താക്കോൽ = in_താക്കോൽ;
            എങ്കിൽ(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &താക്കോൽ, &res, status);
                എങ്കിൽ(U_SUCCESS(*status)) {
                    തിരഞ്ഞെടുപ്പ് (RES_GET_TYPE(res)) {
                    അവസ്ഥ URES_STRING:
                    അവസ്ഥ URES_STRING_V2:
                        തിരികെ res_getString({resB, താക്കോൽ}, &dataEntry->fData, res, len);
                    അവസ്ഥ URES_ALIAS:
                      {
                        സ്ഥിരം char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_താക്കോൽ(resB, in_താക്കോൽ, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        തിരികെ result;
                      }
                    സ്ഥിരസ്ഥിതി:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } അല്ലെങ്കിൽ {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } അല്ലെങ്കിൽ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } അല്ലെങ്കിൽ {
            തിരഞ്ഞെടുപ്പ് (RES_GET_TYPE(res)) {
            അവസ്ഥ URES_STRING:
            അവസ്ഥ URES_STRING_V2:
                തിരികെ res_getString({resB, താക്കോൽ}, &resB->getResData(), res, len);
            അവസ്ഥ URES_ALIAS:
              {
                സ്ഥിരം char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_താക്കോൽ(resB, in_താക്കോൽ, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                തിരികെ result;
              }
            സ്ഥിരസ്ഥിതി:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#എങ്കിൽ 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    അല്ലെങ്കിൽ എങ്കിൽ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == സത്യം) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &താക്കോൽ, &res, status);
        എങ്കിൽ(U_SUCCESS(*status)) {
            // TODO: Tracing
            തിരികെ res_getString(rd, res, len);
        } അല്ലെങ്കിൽ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    അല്ലെങ്കിൽ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ nullptr;
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_get_UTF8_String_By_താക്കോൽ(സ്ഥിരം UResourceBundle* resB,
                        സ്ഥിരം അക്ഷരം* താക്കോൽ,
                        അക്ഷരം* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    സ്ഥിരം char16_t *s16 = ures_get_String_By_താക്കോൽ(resB, താക്കോൽ, &length16, status);
    തിരികെ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_getLocaleInternal(സ്ഥിരം UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
      തിരികെ resourceBundle->fData->fName;
    }
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_getLocale(സ്ഥിരം UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  തിരികെ ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_getLocaleByType(സ്ഥിരം UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        തിരഞ്ഞെടുപ്പ്(type) {
        അവസ്ഥ ULOC_ACTUAL_LOCALE:
            തിരികെ resourceBundle->fData->fName;
        അവസ്ഥ ULOC_VALID_LOCALE:
            തിരികെ resourceBundle->fValidLocaleDataEntry->fName;
        അവസ്ഥ ULOC_REQUESTED_LOCALE:
        സ്ഥിരസ്ഥിതി:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ nullptr;
        }
    }
}

U_CFUNC സ്ഥിരം അക്ഷരം*
ures_getName(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND) {
  എങ്കിൽ(resB == nullptr) {
    തിരികെ nullptr;
  }

  തിരികെ resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC സ്ഥിരം അക്ഷരം*
ures_getPath(സ്ഥിരം UResourceBundle* resB U_LIFETIME_BOUND) {
  എങ്കിൽ(resB == nullptr) {
    തിരികെ nullptr;
  }

  തിരികെ resB->fData->fPath;
}
#endif

സ്ഥിതം UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  സ്ഥിരം അക്ഷരം* path,
                  സ്ഥിരം അക്ഷരം* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    എങ്കിൽ(U_FAILURE(*status)) {
        തിരികെ nullptr;
    }

    UResourceDataEntry *entry;
    എങ്കിൽ(openType != URES_OPEN_DIRECT) {
        എങ്കിൽ (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        എങ്കിൽ(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } അല്ലെങ്കിൽ {
        entry = entryOpenDirect(path, localeID, status);
    }
    എങ്കിൽ(U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        തിരികെ nullptr;
    }

    UBool is_അട്ടി_Object;
    എങ്കിൽ(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(വലുപ്പം(UResourceBundle)));
        എങ്കിൽ(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }
        is_അട്ടി_Object = അസത്യം;
    } അല്ലെങ്കിൽ {  // fill-in
        is_അട്ടി_Object = ures_is_അട്ടി_Object(r);
        ures_closeBundle(r, അസത്യം);
    }
    uprv_memset(r, 0, വലുപ്പം(UResourceBundle));
    ures_set_Is_അട്ടി_Object(r, is_അട്ടി_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_മുകൾ_Level = സത്യം;
    r->fRes = r->getResData().വേര്_Res;
    r->fSize = res_എണ്ണം_Array_ഇനങ്ങൾ(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    തിരികെ r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* localeID, UErrorCode* status) {
    തിരികെ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_വേര്, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* localeID, UErrorCode* status) {
    തിരികെ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_വേര്, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* localeID, UErrorCode* status) {
    തിരികെ ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ശൂന്യം U_EXPORT2
ures_openFillIn(UResourceBundle *r, സ്ഥിരം അക്ഷരം* path,
                സ്ഥിരം അക്ഷരം* localeID, UErrorCode* status) {
    എങ്കിൽ(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_വേര്, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI ശൂന്യം U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* localeID, UErrorCode* status) {
    എങ്കിൽ(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_എണ്ണം_Array_ഇനങ്ങൾ(സ്ഥിരം UResourceBundle* resourceBundle,
                  സ്ഥിരം അക്ഷരം* resource_താക്കോൽ,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_അട്ടി_Object(&resData);
    എങ്കിൽ (status==nullptr || U_FAILURE(*status)) {
        തിരികെ 0;
    }
    എങ്കിൽ(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    ures_get_By_താക്കോൽ(resourceBundle, resource_താക്കോൽ, &resData, status);
    
    എങ്കിൽ(resData.getResData().data != nullptr) {
        int32_t result = res_എണ്ണം_Array_ഇനങ്ങൾ(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        തിരികെ result;
    } അല്ലെങ്കിൽ {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        തിരികെ 0;
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
U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_get_Version_സംഖ്യ_Internal(സ്ഥിരം UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    എങ്കിൽ (!resourceBundle) തിരികെ nullptr;

    എങ്കിൽ(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        സ്ഥിരം char16_t* minor_version = ures_get_String_By_താക്കോൽ(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (അക്ഷരം *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        എങ്കിൽ (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            തിരികെ nullptr;
        }
       
        എങ്കിൽ(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        അല്ലെങ്കിൽ {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    തിരികെ resourceBundle->fVersion;
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ures_get_Version_സംഖ്യ(സ്ഥിരം UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    തിരികെ ures_get_Version_സംഖ്യ_Internal(resourceBundle);
}

U_CAPI ശൂന്യം U_EXPORT2 ures_getVersion(സ്ഥിരം UResourceBundle* resB, UVersionInfo versionInfo) {
    എങ്കിൽ (!resB) തിരികെ;

    u_versionFromString(versionInfo, ures_get_Version_സംഖ്യ_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
#അന്തര്ഭാവ <stdio.h>
#endif

തരനിർവചനം ഘടന ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

സ്ഥിതം ശൂന്യം U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

സ്ഥിതം int32_t U_CALLCONV
ures_loc_എണ്ണം_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    തിരികെ ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


സ്ഥിതം സ്ഥിരം അക്ഷരം * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    സ്ഥിരം അക്ഷരം *result = nullptr;
    int32_t len = 0;
    എങ്കിൽ (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_താക്കോൽ(k);
        len = (int32_t)uprv_strlen(result);
    }
    എങ്കിൽ (resultLength) {
        *resultLength = len;
    }
    തിരികെ result;
}

സ്ഥിതം ശൂന്യം U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

സ്ഥിതം സ്ഥിരം UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_എണ്ണം_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(സ്ഥിരം അക്ഷരം *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    എങ്കിൽ(U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(വലുപ്പം(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(വലുപ്പം(UEnumeration));
    എങ്കിൽ(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        തിരികെ nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, വലുപ്പം(UEnumeration));

    ures_init_അട്ടി_Object(&myContext->installed);
    ures_init_അട്ടി_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_താക്കോൽ(idx, INDEX_TAG, &myContext->installed, status);
    എങ്കിൽ(U_SUCCESS(*status)) {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_താക്കോൽ(&myContext->installed));
#endif
        en->context = myContext;
    } അല്ലെങ്കിൽ {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    തിരികെ en;
}

സ്ഥിതം UBool isLocaleInList(UEnumeration *locEnum, സ്ഥിരം അക്ഷരം *loc_To_തിരയൽ, UErrorCode *status) {
    സ്ഥിരം അക്ഷരം *loc;
    ആകുമ്പോൾ ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        എങ്കിൽ (uprv_strcmp(loc, loc_To_തിരയൽ) == 0) {
            തിരികെ സത്യം;
        }
    }
    തിരികെ അസത്യം;
}

സ്ഥിതം ശൂന്യം getParentForFunctionalEquivalent(സ്ഥിരം അക്ഷരം*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    എങ്കിൽ (res != nullptr) {
        ures_get_By_താക്കോൽ(res, "%%Parent", bund1, &subStatus);
        എങ്കിൽ (U_SUCCESS(subStatus)) {
            int32_t length16;
            സ്ഥിരം char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    എങ്കിൽ (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(അക്ഷരം *result, int32_t resultCapacity,
                             സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *resName, സ്ഥിരം അക്ഷരം *keyword, സ്ഥിരം അക്ഷരം *locid,
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
    എങ്കിൽ(U_FAILURE(*status)) തിരികെ 0;
    CharString kwVal;
    എങ്കിൽ (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_മൂല്യം(locid, keyword, subStatus);
        എങ്കിൽ (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    എങ്കിൽ (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_അട്ടി_Object(&bund1);
    ures_init_അട്ടി_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    എങ്കിൽ(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = സത്യം;
        എങ്കിൽ (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    എങ്കിൽ(U_FAILURE(subStatus)) {
        *status = subStatus;
        തിരികെ 0;
    }
    
    ചെയ്യുക {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        എങ്കിൽ(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = അസത്യം;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        എങ്കിൽ(U_FAILURE(subStatus)) {
            *status = subStatus;
        } അല്ലെങ്കിൽ എങ്കിൽ(subStatus == U_ZERO_ERROR) {
            ures_get_By_താക്കോൽ(res,resName,&bund1, &subStatus);
            എങ്കിൽ(subStatus == U_ZERO_ERROR) {
                സ്ഥിരം char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_താക്കോൽ(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                എങ്കിൽ(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    എങ്കിൽ(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        എങ്കിൽ (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        എങ്കിൽ (found != parent) {
            parent.copyFrom(found, subStatus);
        } അല്ലെങ്കിൽ {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } ആകുമ്പോൾ(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    ചെയ്യുക {
        res = ures_open(path, parent.data(), &subStatus);
        എങ്കിൽ((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = അസത്യം;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        എങ്കിൽ(U_FAILURE(subStatus)) {
            *status = subStatus;
        } അല്ലെങ്കിൽ എങ്കിൽ(subStatus == U_ZERO_ERROR) {
            ures_get_By_താക്കോൽ(res,resName,&bund1, &subStatus);
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            എങ്കിൽ(subStatus == U_ZERO_ERROR) {
                ures_get_By_താക്കോൽ(&bund1, kwVal.data(), &bund2, &subStatus);
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                എങ്കിൽ(subStatus == U_ZERO_ERROR) {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    എങ്കിൽ (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } അല്ലെങ്കിൽ {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        എങ്കിൽ(defLoc.length() > full.length()) {
                          സ്ഥിരം char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_താക്കോൽ(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          എങ്കിൽ(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                        അല്ലെങ്കിൽ {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } അല്ലെങ്കിൽ {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = അസത്യം;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        എങ്കിൽ (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            സ്ഥിരം അക്ഷരം *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            എങ്കിൽ (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                എങ്കിൽ (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = സത്യം;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        എങ്കിൽ (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } ആകുമ്പോൾ(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    എങ്കിൽ(full.isEmpty() && kwVal != defVal) {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        ചെയ്യുക { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            എങ്കിൽ((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = അസത്യം;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            എങ്കിൽ(U_FAILURE(subStatus)) {
                *status = subStatus;
            } അല്ലെങ്കിൽ എങ്കിൽ(subStatus == U_ZERO_ERROR) {
                ures_get_By_താക്കോൽ(res,resName,&bund1, &subStatus);
                എങ്കിൽ(subStatus == U_ZERO_ERROR) {
                    ures_get_By_താക്കോൽ(&bund1, kwVal.data(), &bund2, &subStatus);
                    എങ്കിൽ(subStatus == U_ZERO_ERROR) {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        എങ്കിൽ (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } അല്ലെങ്കിൽ {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        എങ്കിൽ(defLoc.length() > full.length()) {
                          സ്ഥിരം char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_താക്കോൽ(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          എങ്കിൽ(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                        അല്ലെങ്കിൽ {
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
        } ആകുമ്പോൾ(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    എങ്കിൽ(U_SUCCESS(*status)) {
        എങ്കിൽ(full.isEmpty()) {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } അല്ലെങ്കിൽ എങ്കിൽ(omitDefault) {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          എങ്കിൽ(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            എങ്കിൽ(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        എങ്കിൽ(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } അല്ലെങ്കിൽ എങ്കിൽ(!omitDefault) {
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

    എങ്കിൽ(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        എങ്കിൽ(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        എങ്കിൽ(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } അല്ലെങ്കിൽ {
        length = 0;
        result[0]=0;
    }
    തിരികെ u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_മൂല്യങ്ങൾ(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *keyword, UErrorCode *status)
{
#define മൂല്യങ്ങൾ_BUF_SIZE 2048
#define മൂല്യങ്ങൾ_LIST_SIZE 512
    
    അക്ഷരം       മൂല്യങ്ങൾ_Buf[മൂല്യങ്ങൾ_BUF_SIZE];
    int32_t    മൂല്യങ്ങൾ_Index = 0;
    സ്ഥിരം അക്ഷരം *മൂല്യങ്ങൾ_List[മൂല്യങ്ങൾ_LIST_SIZE];
    int32_t    മൂല്യങ്ങൾ_എണ്ണം = 0;
    
    സ്ഥിരം അക്ഷരം *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_അട്ടി_Object(&item);
    ures_init_അട്ടി_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    എങ്കിൽ(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        തിരികെ nullptr;
    }
    
    മൂല്യങ്ങൾ_Buf[0]=0;
    മൂല്യങ്ങൾ_Buf[1]=0;

    ആകുമ്പോൾ ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
        എങ്കിൽ(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_താക്കോൽ(bund, keyword, &item, &subStatus);
        
        എങ്കിൽ(!bund || U_FAILURE(subStatus)) {
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            തുടരുക;
        }

        ആകുമ്പോൾ ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            സ്ഥിരം അക്ഷരം *k;
            int32_t i;
            k = ures_get_താക്കോൽ(subPtr);

#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            എങ്കിൽ(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                തുടരുക;
            }
            വേണ്ടി(i=0; i<മൂല്യങ്ങൾ_എണ്ണം; i++) {
                എങ്കിൽ(!uprv_strcmp(മൂല്യങ്ങൾ_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    നിർത്തുക;
                }
            }
            എങ്കിൽ(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                എങ്കിൽ((മൂല്യങ്ങൾ_എണ്ണം >= (മൂല്യങ്ങൾ_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((മൂല്യങ്ങൾ_Index+kLen+1+1) >= മൂല്യങ്ങൾ_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } അല്ലെങ്കിൽ {
                    uprv_strcpy(മൂല്യങ്ങൾ_Buf+മൂല്യങ്ങൾ_Index, k);
                    മൂല്യങ്ങൾ_List[മൂല്യങ്ങൾ_എണ്ണം++] = മൂല്യങ്ങൾ_Buf+മൂല്യങ്ങൾ_Index;
                    മൂല്യങ്ങൾ_Index += kLen;
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    മൂല്യങ്ങൾ_Buf[മൂല്യങ്ങൾ_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    മൂല്യങ്ങൾ_Buf[മൂല്യങ്ങൾ_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#എങ്കിൽ defined(URES_വൃക്ഷം_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        മൂല്യങ്ങൾ_Index, മൂല്യങ്ങൾ_എണ്ണം);
#endif
    തിരികെ uloc_openKeywordList(മൂല്യങ്ങൾ_Buf, മൂല്യങ്ങൾ_Index, status);
}
#എങ്കിൽ 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(സ്ഥിരം UResourceBundle* res1, സ്ഥിരം UResourceBundle* res2){
    എങ്കിൽ(res1==nullptr || res2==nullptr){
        തിരികെ res1==res2; /* pointer comparison */
    }
    എങ്കിൽ(res1->f_താക്കോൽ==nullptr||  res2->f_താക്കോൽ==nullptr){
        തിരികെ (res1->f_താക്കോൽ==res2->f_താക്കോൽ);
    }അല്ലെങ്കിൽ{
        എങ്കിൽ(uprv_strcmp(res1->f_താക്കോൽ, res2->f_താക്കോൽ)!=0){
            തിരികെ അസത്യം;
        }
    }
    എങ്കിൽ(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        തിരികെ (res1->fData->fPath == res2->fData->fPath);
    }അല്ലെങ്കിൽ{
        എങ്കിൽ(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            തിരികെ അസത്യം;
        }
    }
    എങ്കിൽ(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(res1->fRes != res2->fRes){
        തിരികെ അസത്യം;
    }
    തിരികെ സത്യം;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(സ്ഥിരം UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    എങ്കിൽ(U_FAILURE(*status) || res == nullptr){
        തിരികെ nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    എങ്കിൽ(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }അല്ലെങ്കിൽ{
        ret = bundle;
    }
    തിരികെ ret;
}
U_CAPI സ്ഥിരം UResourceBundle* U_EXPORT2
ures_getParentBundle(സ്ഥിരം UResourceBundle* res){
    എങ്കിൽ(res==nullptr){
        തിരികെ nullptr;
    }
    തിരികെ res->fParentRes;
}
#endif

U_CAPI ശൂന്യം U_EXPORT2
ures_get_Version_By_താക്കോൽ(സ്ഥിരം UResourceBundle* res, സ്ഥിരം അക്ഷരം *താക്കോൽ, UVersionInfo ver, UErrorCode *status) {
  സ്ഥിരം char16_t *str;
  int32_t len;
  str = ures_get_String_By_താക്കോൽ(res, താക്കോൽ, &len, status);
  എങ്കിൽ(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
