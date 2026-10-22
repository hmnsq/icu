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

#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "bytesinkutil.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "cwchar.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "unicode/uenum.h"
#অন্তর্ভাব "uenumimp.h"
#অন্তর্ভাব "ulocimp.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uresdata.h"

using নামাকাশ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
স্থির UHashtable *cache = nullptr;
স্থির icu::UInitOnce gCacheInitOnce {};

স্থির UMutex resbMutex;

/* INTERNAL: hashes an entry  */
স্থির int32_t U_CALLCONV hashEntry(ধ্রুবক UHashTok parm) {
    UResource_তথ্য_Entry* b = static_cast<UResource_তথ্য_Entry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    ফেরত uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
স্থির UBool U_CALLCONV compareEntries(ধ্রুবক UHashTok p1, ধ্রুবক UHashTok p2) {
    UResource_তথ্য_Entry* b1 = static_cast<UResource_তথ্য_Entry*>(p1.pointer);
    UResource_তথ্য_Entry* b2 = static_cast<UResource_তথ্য_Entry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    ফেরত uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
স্থির UBool chopLocale(অক্ষর *name) {
    অক্ষর *i = uprv_strrchr(name, '_');

    যদি(i != nullptr) {
        *i = '\0';
        ফেরত সত্য;
    }

    ফেরত মিথ্যা;
}

স্থির UBool hasVariant(ধ্রুবক অক্ষর* localeID) {
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
    ফেরত sink.সংখ্যা_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#অন্তর্ভাব "localefallback_data.h"

স্থির ধ্রুবক অক্ষর* performFallbackLookup(ধ্রুবক অক্ষর* চাবি,
                                         ধ্রুবক অক্ষর* চাবি_Strs,
                                         ধ্রুবক অক্ষর* মান_Strs U_LIFETIME_BOUND,
                                         ধ্রুবক int32_t* lookupTable,
                                         int32_t lookup_Table_দৈর্ঘ্য) {
    ধ্রুবক int32_t* bottom = lookupTable;
    ধ্রুবক int32_t* শীর্ষ_2 = lookupTable + lookup_Table_দৈর্ঘ্য;

    যখন (bottom < শীর্ষ_2) {
        // Effectively, divide by 2 and round down to an even index
        ধ্রুবক int32_t* মধ্য = bottom + (((শীর্ষ_2 - bottom) / 4) * 2);
        ধ্রুবক অক্ষর* entry_চাবি = &(চাবি_Strs[*মধ্য]);
        int32_t strcmp_ফলাফল = uprv_strcmp(চাবি, entry_চাবি);
        যদি (strcmp_ফলাফল == 0) {
            ফেরত &(মান_Strs[মধ্য[1]]);
        } অন্যথা যদি (strcmp_ফলাফল < 0) {
            শীর্ষ_2 = মধ্য;
        } অন্যথা {
            bottom = মধ্য + 2;
        }
    }
    ফেরত nullptr;
}

স্থির CharString getDefaultScript(ধ্রুবক CharString& language, ধ্রুবক CharString& region) {
    ধ্রুবক অক্ষর* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString ফলাফল("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    যদি (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        যদি (U_FAILURE(err)) {
            ফেরত ফলাফল;
        }
        defaultScript = performFallbackLookup(localeID.তথ্য(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    যদি (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.তথ্য(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    যদি (defaultScript != nullptr) {
        ফলাফল.clear();
        ফলাফল.append(defaultScript, err);
    }
    ফেরত ফলাফল;
}

গণনা UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_মূল,
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
    URES_OPEN_LOCALE_মূল,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
প্রকারলেখ গণনা UResOpenType UResOpenType;

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
স্থির যৌক্তিক getParentLocaleID(অক্ষর *name, ধ্রুবক অক্ষর *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    যদি (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        ফেরত chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    যদি (U_FAILURE(err)) {
        // hopefully this never happens...
        ফেরত chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    যদি (openType == URES_OPEN_LOCALE_DEFAULT_মূল) {
        ধ্রুবক অক্ষর* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        যদি (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            ফেরত সত্য;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    যদি (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        যদি (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } অন্যথা {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } অন্যথা যদি (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        যদি (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } অন্যথা {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } অন্যথা যদি (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        যদি (openType != URES_OPEN_LOCALE_DEFAULT_মূল || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } অন্যথা {
            ফেরত মিথ্যা;
        }
    } অন্যথা {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        ফেরত মিথ্যা;
    }
    যদি (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.তথ্য());
        ফেরত সত্য;
    } অন্যথা {
        ফেরত মিথ্যা;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
স্থির UBool mayHaveParent(অক্ষর *name) {
    ফেরত (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
স্থির শূন্য entryIncrease(UResource_তথ্য_Entry *entry) {
    Mutex lock(&resbMutex);
    entry->f_গুনতি_Existing++;
    যখন(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_গুনতি_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
স্থির UResource_তথ্য_Entry *get_Fallback_তথ্য(
        ধ্রুবক UResourceBundle *resBundle,
        ধ্রুবক অক্ষর **resTag, Resource *res, UErrorCode *status) {
    UResource_তথ্য_Entry *তথ্য_Entry = resBundle->f_তথ্য;
    int32_t সূচক_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    যদি(তথ্য_Entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        ফেরত nullptr;
    }
    যদি(তথ্য_Entry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_চাবি(&(তথ্য_Entry->f_তথ্য), তথ্য_Entry->f_তথ্য.মূল_Res, &সূচক_R, resTag); /* try to get data from there */
        i++;
    }
    যদি(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        যখন(*res == RES_BOGUS && তথ্য_Entry->fParent != nullptr) {
            তথ্য_Entry = তথ্য_Entry->fParent;
            যদি(তথ্য_Entry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_চাবি(&(তথ্য_Entry->f_তথ্য), তথ্য_Entry->f_তথ্য.মূল_Res, &সূচক_R, resTag);
            }
        }
    }

    যদি(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        ফেরত nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    যদি(i>1) {
        যদি(uprv_strcmp(তথ্য_Entry->fName, uloc_getDefault())==0 || uprv_strcmp(তথ্য_Entry->fName, k_মূল_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } অন্যথা {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    ফেরত তথ্য_Entry;
}

স্থির শূন্য
free_entry(UResource_তথ্য_Entry *entry) {
    UResource_তথ্য_Entry *alias;
    res_unload(&(entry->f_তথ্য));
    যদি(entry->fName != nullptr && entry->fName != entry->f_Name_বাফার) {
        uprv_free(entry->fName);
    }
    যদি(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    যদি(entry->fPool != nullptr) {
        --entry->fPool->f_গুনতি_Existing;
    }
    alias = entry->fAlias;
    যদি(alias != nullptr) {
        যখন(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_গুনতি_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
স্থির int32_t ures_flushCache()
{
    UResource_তথ্য_Entry *resB;
    int32_t pos;
    int32_t rb_Deleted_অঙ্ক = 0;
    ধ্রুবক UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    যদি (cache == nullptr) {
        ফেরত 0;
    }

    করো {
        deletedMore = মিথ্যা;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        যখন ((e = uhash_পরবর্তী_Element(cache, &pos)) != nullptr)
        {
            resB = static_cast<UResource_তথ্য_Entry*>(e->value.pointer);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            যদি (resB->f_গুনতি_Existing == 0) {
                rb_Deleted_অঙ্ক++;
                deletedMore = সত্য;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } যখন(deletedMore);

    ফেরত rb_Deleted_অঙ্ক;
}

#ifdef URES_DEBUG
#অন্তর্ভাব <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = মিথ্যা;
  int32_t pos = UHASH_FIRST;
  ধ্রুবক UHashElement *e;
  UResource_তথ্য_Entry *resB;
  
    Mutex lock(&resbMutex);
    যদি (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      ফেরত মিথ্যা;
    }

    যখন ((e = uhash_পরবর্তী_Element(cache, &pos)) != nullptr) {
      cacheNotEmpty=সত্য;
      resB = (UResource_তথ্য_Entry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (শূন্য*)resB, resB->f_গুনতি_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (শূন্য*)resB->fPool,
              (শূন্য*)resB->fAlias,
              (শূন্য*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_গুনতি(cache));
    ফেরত cacheNotEmpty;
}

#endif

স্থির UBool U_CALLCONV ures_cleanup()
{
    যদি (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    ফেরত সত্য;
}

/** INTERNAL: Initializes the cache for resources */
স্থির শূন্য U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
স্থির শূন্য initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

স্থির শূন্য setEntryName(UResource_তথ্য_Entry *res, ধ্রুবক অক্ষর *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    যদি(res->fName != nullptr && res->fName != res->f_Name_বাফার) {
        uprv_free(res->fName);
    }
    যদি (len < static_cast<int32_t>(আকার(res->f_Name_বাফার))) {
        res->fName = res->f_Name_বাফার;
    }
    অন্যথা {
        res->fName = static_cast<অক্ষর*>(uprv_malloc(len + 1));
    }
    যদি(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা {
        uprv_strcpy(res->fName, name);
    }
}

স্থির UResource_তথ্য_Entry *
getPoolEntry(ধ্রুবক অক্ষর *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
স্থির UResource_তথ্য_Entry *init_entry(ধ্রুবক অক্ষর *localeID, ধ্রুবক অক্ষর *path, UErrorCode *status) {
    UResource_তথ্য_Entry *r = nullptr;
    UResource_তথ্য_Entry find;
    /*int32_t hashValue;*/
    ধ্রুবক অক্ষর *name;
    অক্ষর aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    /* here we try to deduce the right locale name */
    যদি(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } অন্যথা যদি(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_মূল_Locale_Name;
    } অন্যথা { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<অক্ষর*>(name);
    find.fPath = const_cast<অক্ষর*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_তথ্য_Entry*>(uhash_get(cache, &find));
    যদি(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_তথ্য_Entry*>(uprv_malloc(আকার(UResource_তথ্য_Entry)));
        যদি(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        }

        uprv_memset(r, 0, আকার(UResource_তথ্য_Entry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        যদি (U_FAILURE(*status)) {
            uprv_free(r);
            ফেরত nullptr;
        }

        যদি(path != nullptr) {
            r->fPath = uprv_strdup(path);
            যদি(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                ফেরত nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->f_তথ্য), r->fPath, r->fName, status);

        যদি (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            যদি (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                ফেরত nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } অন্যথা { /* if we have a regular entry */
            Resource aliasres;
            যদি (r->f_তথ্য.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                যদি (U_SUCCESS(*status)) {
                    ধ্রুবক int32_t *poolIndexes = r->fPool->f_তথ্য.p_মূল + 1;
                    যদি(r->f_তথ্য.p_মূল[1 + URES_সূচক_POOL_CHECKSUM] == poolIndexes[URES_সূচক_POOL_CHECKSUM]) {
                        r->f_তথ্য.pool_Bundle_চাবিগুলি = reinterpret_cast<ধ্রুবক অক্ষর*>(poolIndexes + (poolIndexes[URES_সূচক_দৈর্ঘ্য] & 0xff));
                        r->f_তথ্য.poolBundleStrings = r->fPool->f_তথ্য.p16BitUnits;
                    } অন্যথা {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } অন্যথা {
                    r->fBogus = *status;
                }
            }
            যদি (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_তথ্য), "%%ALIAS");
                যদি (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ধ্রুবক char16_t *alias = res_getStringNoTrace(&(r->f_তথ্য), aliasres, &aliasLen);
                    যদি(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResource_তথ্য_Entry *oldR = nullptr;
            যদি ((oldR = static_cast<UResource_তথ্য_Entry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (শূন্য *)r, r, &cacheStatus);
                যদি (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } অন্যথা {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    যদি(r != nullptr) {
        /* return the real bundle */
        যখন(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_গুনতি_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        যদি(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    ফেরত r;
}

স্থির UResource_তথ্য_Entry *
getPoolEntry(ধ্রুবক অক্ষর *path, UErrorCode *status) {
    UResource_তথ্য_Entry *poolBundle = init_entry(kPoolBundleName, path, status);
    যদি( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_তথ্য.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    ফেরত poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
স্থির UResource_তথ্য_Entry *
findFirstExisting(ধ্রুবক অক্ষর* path, অক্ষর* name, ধ্রুবক অক্ষর* defaultLocale, UResOpenType openType,
                  UBool *is_মূল, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResource_তথ্য_Entry *r = nullptr;
    UBool has_Real_তথ্য = মিথ্যা;
    *foundParent = সত্য; /* we're starting with a fresh name */
    অক্ষর origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    যখন(*foundParent && !has_Real_তথ্য) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        যদি (U_FAILURE(*status)) {
            ফেরত nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_তথ্য = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        যদি(!has_Real_তথ্য) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_গুনতি_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } অন্যথা {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_মূল = static_cast<UBool>(uprv_strcmp(name, k_মূল_Locale_Name) == 0);

        /*Fallback data stuff*/
        যদি (!has_Real_তথ্য) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } অন্যথা {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        যদি (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    ফেরত r;
}

স্থির শূন্য ures_set_Is_স্তূপ_Object( UResourceBundle* resB, UBool state) {
    যদি(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } অন্যথা {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

স্থির UBool ures_is_স্তূপ_Object(ধ্রুবক UResourceBundle* resB) {
  ফেরত((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?মিথ্যা:সত্য);
}


U_CFUNC শূন্য ures_init_স্তূপ_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, আকার(UResourceBundle));
  ures_set_Is_স্তূপ_Object(resB, সত্য);
}

U_NAMESPACE_BEGIN

স্তূপ_UResource_Bundle::স্তূপ_UResource_Bundle() {
    ures_init_স্তূপ_Object(&bundle);
}

স্তূপ_UResource_Bundle::~স্তূপ_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

স্থির UBool  // returns U_SUCCESS(*status)
load_Parents_Except_মূল(UResource_তথ্য_Entry *&t1,
                      অক্ষর name[], int32_t nameCapacity,
                      UBool usingUSRData, অক্ষর usr_তথ্য_Path[], UErrorCode *status) {
    যদি (U_FAILURE(*status)) { ফেরত মিথ্যা; }
    UBool checkParent = সত্য;
    যখন (checkParent && t1->fParent == nullptr && !t1->f_তথ্য.noFallback &&
            res_getResource(&t1->f_তথ্য,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->f_তথ্য, "%%Parent");
        যদি (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            ধ্রুবক char16_t *parentLocaleName = res_getStringNoTrace(&(t1->f_তথ্য), parentRes, &parentLocaleLen);
            যদি(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                যদি (uprv_strcmp(name, k_মূল_Locale_Name) == 0) {
                    ফেরত সত্য;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResource_তথ্য_Entry *t2 = init_entry(name, t1->fPath, &parentStatus);
        যদি (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            ফেরত মিথ্যা;
        }
        UResource_তথ্য_Entry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        যদি (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usr_তথ্য_Path, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            যদি (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                ফেরত মিথ্যা;
            }
        }

        যদি (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } অন্যথা {
            t1->fParent = t2;
            যদি (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_গুনতি_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    ফেরত সত্য;
}

স্থির UBool  // returns U_SUCCESS(*status)
ঢোকাও_মূল_Bundle(UResource_তথ্য_Entry *&t1, UErrorCode *status) {
    যদি (U_FAILURE(*status)) { ফেরত মিথ্যা; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResource_তথ্য_Entry *t2 = init_entry(k_মূল_Locale_Name, t1->fPath, &parentStatus);
    যদি (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        ফেরত মিথ্যা;
    }
    t1->fParent = t2;
    t1 = t2;
    ফেরত সত্য;
}

স্থির UResource_তথ্য_Entry *entryOpen(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResource_তথ্য_Entry *r = nullptr;
    UResource_তথ্য_Entry *t1 = nullptr;
    UBool isDefault = মিথ্যা;
    UBool is_মূল = মিথ্যা;
    UBool has_Real_তথ্য = মিথ্যা;
    UBool hasChopped = সত্য;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    অক্ষর name[ULOC_FULLNAME_CAPACITY];
    অক্ষর usr_তথ্য_Path[96];

    initCache(status);

    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    uprv_strncpy(name, localeID, আকার(name) - 1);
    name[আকার(name) - 1] = 0;

    যদি ( usingUSRData ) {
        যদি ( path == nullptr ) {
            uprv_strcpy(usr_তথ্য_Path, U_USRDATA_NAME);
        } অন্যথা {
            uprv_strncpy(usr_তথ্য_Path, path, আকার(usr_তথ্য_Path) - 1);
            usr_তথ্য_Path[0] = 'u';
            usr_তথ্য_Path[1] = 's';
            usr_তথ্য_Path[2] = 'r';
            usr_তথ্য_Path[আকার(usr_তথ্য_Path) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ধ্রুবক অক্ষর *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_মূল, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    যদি (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        যাও finish;
    }

    যদি(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_তথ্য = সত্য;
        যদি ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResource_তথ্য_Entry *u1 = init_entry(t1->fName, usr_তথ্য_Path, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            যদি (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                যাও finish;
            }
            যদি ( u1 != nullptr ) {
                যদি(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } অন্যথা {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_গুনতি_Existing = 0;
                }
            }
        }
        যদি ((hasChopped || mayHaveParent(name)) && !is_মূল) {
            যদি (!load_Parents_Except_মূল(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_তথ্য_Path, status)) {
                যাও finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    যদি(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_মূল && !isDefault && !is_মূল) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_মূল, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        যদি (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            যাও finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        যদি(r != nullptr) { /* the default locale exists */
            t1 = r;
            has_Real_তথ্য = সত্য;
            isDefault = সত্য;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            যদি ((hasChopped || mayHaveParent(name)) && !is_মূল) {
                যদি (!load_Parents_Except_মূল(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_তথ্য_Path, status)) {
                    যাও finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    যদি(r == nullptr) {
        uprv_strcpy(name, k_মূল_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_মূল, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        যদি (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            যাও finish;
        }
        যদি(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            has_Real_তথ্য = সত্য;
        } অন্যথা { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            যাও finish;
        }
    } অন্যথা যদি(!is_মূল && uprv_strcmp(t1->fName, k_মূল_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->f_তথ্য.noFallback) {
        যদি (!ঢোকাও_মূল_Bundle(t1, status)) {
            যাও finish;
        }
        যদি(!has_Real_তথ্য) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    যখন(r != nullptr && !is_মূল && t1->fParent != nullptr) {
        t1->fParent->f_গুনতি_Existing++;
        t1 = t1->fParent;
    }

finish:
    যদি(U_SUCCESS(*status)) {
        যদি(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        ফেরত r;
    } অন্যথা {
        ফেরত nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
স্থির UResource_তথ্য_Entry *
entryOpenDirect(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* localeID, UErrorCode* status) {
    initCache(status);
    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    যদি (localeID == nullptr) {
        localeID = uloc_getDefault();
    } অন্যথা যদি (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_মূল_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_তথ্য_Entry *r = init_entry(localeID, path, status);
    যদি(U_SUCCESS(*status)) {
        যদি(r->fBogus != U_ZERO_ERROR) {
            r->f_গুনতি_Existing--;
            r = nullptr;
        }
    } অন্যথা {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_তথ্য_Entry *t1 = r;
    যদি(r != nullptr && uprv_strcmp(localeID, k_মূল_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->f_তথ্য.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        অক্ষর name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        যদি(!chopLocale(name) || uprv_strcmp(name, k_মূল_Locale_Name) == 0 ||
                load_Parents_Except_মূল(t1, name, UPRV_LENGTHOF(name), মিথ্যা, nullptr, status)) {
            যদি(uprv_strcmp(t1->fName, k_মূল_Locale_Name) != 0 && t1->fParent == nullptr) {
                ঢোকাও_মূল_Bundle(t1, status);
            }
        }
        যদি(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    যদি(r != nullptr) {
        // TODO: Does this ever loop?
        যখন(t1->fParent != nullptr) {
            t1->fParent->f_গুনতি_Existing++;
            t1 = t1->fParent;
        }
    }
    ফেরত r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
স্থির শূন্য entryCloseInt(UResource_তথ্য_Entry *resB) {
    UResource_তথ্য_Entry *p = resB;

    যখন(resB != nullptr) {
        p = resB->fParent;
        resB->f_গুনতি_Existing--;

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

স্থির শূন্য entryClose(UResource_তথ্য_Entry *resB) {
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
স্থির শূন্য ures_appendResPath(UResourceBundle *resB, ধ্রুবক অক্ষর* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    যদি(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    যদি(RES_BUFSIZE <= resB->fResPathLen+1) {
        যদি(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<অক্ষর*>(uprv_malloc((resB->fResPathLen + 1) * আকার(অক্ষর)));
            /* Check that memory was allocated correctly. */
            যদি (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } অন্যথা {
            অক্ষর* অস্থায়ী = static_cast<অক্ষর*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * আকার(অক্ষর)));
            /* Check that memory was reallocated correctly. */
            যদি (অস্থায়ী == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            resB->fResPath = অস্থায়ী;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

স্থির শূন্য ures_freeResPath(UResourceBundle *resB) {
    যদি (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

স্থির শূন্য
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    যদি(resB != nullptr) {
        যদি(resB->f_তথ্য != nullptr) {
            entryClose(resB->f_তথ্য);
        }
        যদি(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        যদি(ures_is_স্তূপ_Object(resB) == মিথ্যা && freeBundleObj) {
            uprv_free(resB);
        }
#যদি 0 /*U_DEBUG*/
        অন্যথা {
            /* poison the data */
            uprv_memset(resB, -1, আকার(UResourceBundle));
        }
#endif
    }
}

U_CAPI শূন্য  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, সত্য);
}

নামাকাশ {

UResourceBundle* init_resb_ফলাফল(UResource_তথ্য_Entry* তথ্য_Entry,
                                  Resource r,
                                  ধ্রুবক অক্ষর* চাবি,
                                  int32_t idx,
                                  UResource_তথ্য_Entry* valid_Locale_তথ্য_Entry,
                                  ধ্রুবক অক্ষর* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(ধ্রুবক Resource_তথ্য& res_তথ্য,
                               Resource r,
                               ধ্রুবক অক্ষর* চাবি,
                               int32_t idx,
                               UResource_তথ্য_Entry* valid_Locale_তথ্য_Entry,
                               ধ্রুবক অক্ষর* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    যদি (U_FAILURE(*status)) { ফেরত resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ধ্রুবক char16_t *alias = res_getAlias(&res_তথ্য, r, &len);
    যদি(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত resB;
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
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    // We have an alias, now let's cut it up.
    ধ্রুবক অক্ষর *path = nullptr, *locale = nullptr, *চাবি_Path = nullptr;
    যদি(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        অক্ষর *ch_Alias_তথ্য = chAlias.তথ্য();
        অক্ষর *sep = ch_Alias_তথ্য + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        যদি(sep != nullptr) {
            *sep++ = 0;
        }
        যদি(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            চাবি_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } অন্যথা {
            যদি(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            যদি (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } অন্যথা {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                যদি(sep != nullptr) {
                    *sep++ = 0;
                }
                চাবি_Path = sep;
            }
        }
    } অন্যথা {
        // No path, start with a locale.
        অক্ষর *sep = chAlias.তথ্য();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        যদি(sep != nullptr) {
            *sep++ = 0;
        }
        চাবি_Path = sep;
        path = valid_Locale_তথ্য_Entry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResource_তথ্য_Entry *তথ্য_Entry;
    যদি (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        তথ্য_Entry = valid_Locale_তথ্য_Entry;
    } অন্যথা {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        যদি(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            ফেরত resB;
        }
        তথ্য_Entry = mainRes->f_তথ্য;
    }

    ধ্রুবক অক্ষর* অস্থায়ী = nullptr;
    যদি(চাবি_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = তথ্য_Entry->f_তথ্য.মূল_Res;
        যদি(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            যদি (U_FAILURE(*status)) {
                ফেরত nullptr;
            }
            অক্ষর *a_চাবি = chAlias.তথ্য();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&তথ্য_Entry->f_তথ্য, r, &a_চাবি, &অস্থায়ী);
        }
        যদি(চাবি) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(চাবি, *status);
            যদি (U_FAILURE(*status)) {
                ফেরত nullptr;
            }
            অক্ষর *a_চাবি = chAlias.তথ্য();
            r = res_findResource(&তথ্য_Entry->f_তথ্য, r, &a_চাবি, &অস্থায়ী);
        } অন্যথা যদি(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            যদি(URES_IS_TABLE(type)) {
                ধ্রুবক অক্ষর *a_চাবি;
                r = res_get_Table_Item_By_সূচক(&তথ্য_Entry->f_তথ্য, r, idx, &a_চাবি);
            } অন্যথা { /* array */
                r = res_getArrayItem(&তথ্য_Entry->f_তথ্য, r, idx);
            }
        }
        যদি(r != RES_BOGUS) {
            resB = init_resb_ফলাফল(
                তথ্য_Entry, r, অস্থায়ী, -1, valid_Locale_তথ্য_Entry, nullptr, recursionDepth+1,
                resB, status);
        } অন্যথা {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } অন্যথা {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(চাবি_Path, *status);
        যদি (U_FAILURE(*status)) {
            ফেরত nullptr;
        }
        অক্ষর *myPath = pathBuf.তথ্য();
        containerResPath = nullptr;
        // Now we have fallback following here.
        জন্য(;;) {
            r = তথ্য_Entry->f_তথ্য.মূল_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            যখন(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(তথ্য_Entry->f_তথ্য), r, &myPath, &অস্থায়ী);
                যদি(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    বিরতি;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_ফলাফল(
                    তথ্য_Entry, r, অস্থায়ী, -1,
                    valid_Locale_তথ্য_Entry, containerResPath, recursionDepth+1,
                    resB, status);
                যদি (U_FAILURE(*status)) {
                    বিরতি;
                }
                যদি (অস্থায়ী == nullptr || uprv_strcmp(চাবি_Path, অস্থায়ী) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, চাবি_Path, static_cast<int32_t>(uprv_strlen(চাবি_Path)), status);
                    যদি(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    যদি (U_FAILURE(*status)) {
                        বিরতি;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                তথ্য_Entry = resB->f_তথ্য;
                containerResPath = resB->fResPath;
            }
            যদি (U_FAILURE(*status) || r != RES_BOGUS) {
                বিরতি;
            }
            // Fall back to the parent bundle, if there is one.
            তথ্য_Entry = তথ্য_Entry->fParent;
            যদি (তথ্য_Entry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                বিরতি;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.তথ্য();
            uprv_strcpy(myPath, চাবি_Path);
        }
    }
    যদি(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    ফেরত resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_ফলাফল(UResource_তথ্য_Entry* তথ্য_Entry,
                                  Resource r,
                                  ধ্রুবক অক্ষর* চাবি,
                                  int32_t idx,
                                  UResource_তথ্য_Entry* valid_Locale_তথ্য_Entry,
                                  ধ্রুবক অক্ষর* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    যদি(status == nullptr || U_FAILURE(*status)) {
        ফেরত resB;
    }
    যদি (valid_Locale_তথ্য_Entry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    যদি(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        যদি(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            ফেরত resB;
        }
        ফেরত getAliasTargetAsResourceBundle(
            তথ্য_Entry->f_তথ্য, r, চাবি, idx,
            valid_Locale_তথ্য_Entry, containerResPath, recursionDepth, resB, status);
    }
    যদি(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(আকার(UResourceBundle)));
        যদি (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        }
        ures_set_Is_স্তূপ_Object(resB, মিথ্যা);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } অন্যথা {
        যদি(resB->f_তথ্য != nullptr) {
            entryClose(resB->f_তথ্য);
        }
        যদি(resB->fVersion != nullptr) {
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
        যদি(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->f_তথ্য = তথ্য_Entry;
    entryIncrease(resB->f_তথ্য);
    resB->fHasFallback = মিথ্যা;
    resB->f_Is_শীর্ষ_Level = মিথ্যা;
    resB->f_সূচক = -1;
    resB->f_চাবি = চাবি; 
    resB->f_Valid_Locale_তথ্য_Entry = valid_Locale_তথ্য_Entry;
    যদি(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    যদি(চাবি != nullptr) {
        ures_appendResPath(resB, চাবি, static_cast<int32_t>(uprv_strlen(চাবি)), status);
        যদি(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } অন্যথা যদি(idx >= 0) {
        অক্ষর buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        যদি(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, আকার(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->f_আকার = res_গুনতি_Array_বস্তুসমূহ(&resB->get_Res_তথ্য(), resB->fRes);
    ResourceTracer(resB).trace("get");
    ফেরত resB;
}

UResourceBundle* init_resb_ফলাফল(UResource_তথ্য_Entry* তথ্য_Entry,
                                  Resource r,
                                  ধ্রুবক অক্ষর* চাবি,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ধ্রুবক UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    ফেরত init_resb_ফলাফল(
        তথ্য_Entry, r, চাবি, idx,
        container->f_Valid_Locale_তথ্য_Entry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ধ্রুবক UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_স্তূপ_Object;
    যদি(U_FAILURE(*status) || r == original) {
        ফেরত r;
    }
    যদি(original != nullptr) {
        যদি(r == nullptr) {
            is_স্তূপ_Object = মিথ্যা;
            r = static_cast<UResourceBundle*>(uprv_malloc(আকার(UResourceBundle)));
            /* test for nullptr */
            যদি (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত nullptr;
            }
        } অন্যথা {
            is_স্তূপ_Object = ures_is_স্তূপ_Object(r);
            ures_closeBundle(r, মিথ্যা);
        }
        uprv_memcpy(r, original, আকার(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        যদি(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_স্তূপ_Object(r, is_স্তূপ_Object);
        যদি(r->f_তথ্য != nullptr) {
            entryIncrease(r->f_তথ্য);
        }
    }
    ফেরত r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ধ্রুবক char16_t* U_EXPORT2 ures_getString(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    ধ্রুবক char16_t *s;
    যদি (status==nullptr || U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    s = res_getString({resB}, &resB->get_Res_তথ্য(), resB->fRes, len);
    যদি (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত s;
}

স্থির ধ্রুবক অক্ষর*
ures_toUTF8String(ধ্রুবক char16_t* s16,
                  int32_t length16,
                  অক্ষর* dest U_LIFETIME_BOUND,
                  int32_t* p_দৈর্ঘ্য,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি (p_দৈর্ঘ্য != nullptr) {
        capacity = *p_দৈর্ঘ্য;
    } অন্যথা {
        capacity = 0;
    }
    যদি (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি (length16 == 0) {
        /* empty string, return as read-only pointer */
        যদি (p_দৈর্ঘ্য != nullptr) {
            *p_দৈর্ঘ্য = 0;
        }
        যদি (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            ফেরত dest;
        } অন্যথা {
            ফেরত "";
        }
    } অন্যথা {
        /* We need to transform the string to the destination buffer. */
        যদি (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            ফেরত u_strToUTF8(nullptr, 0, p_দৈর্ঘ্য, s16, length16, status);
        }
        যদি (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            int32_t max_দৈর্ঘ্য_2 = 3 * length16 + 1;
            যদি (capacity > max_দৈর্ঘ্য_2) {
                dest += capacity - max_দৈর্ঘ্য_2;
                capacity = max_দৈর্ঘ্য_2;
            }
        }
        ফেরত u_strToUTF8(dest, capacity, p_দৈর্ঘ্য, s16, length16, status);
    }
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_getUTF8String(ধ্রুবক UResourceBundle* resB,
                   অক্ষর* dest U_LIFETIME_BOUND,
                   int32_t* p_দৈর্ঘ্য,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    ধ্রুবক char16_t *s16 = ures_getString(resB, &length16, status);
    ফেরত ures_toUTF8String(s16, length16, dest, p_দৈর্ঘ্য, forceCopy, status);
}

U_CAPI ধ্রুবক uint8_t* U_EXPORT2
ures_get_দ্বিমিক(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ধ্রুবক uint8_t *p;
  যদি (status==nullptr || U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  যদি(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত nullptr;
  }
  p = res_get_দ্বিমিক({resB}, &resB->get_Res_তথ্য(), resB->fRes, len);
  যদি (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  ফেরত p;
}

U_CAPI ধ্রুবক int32_t* U_EXPORT2
ures_getIntVector(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ধ্রুবক int32_t *p;
  যদি (status==nullptr || U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  যদি(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত nullptr;
  }
  p = res_getIntVector({resB}, &resB->get_Res_তথ্য(), resB->fRes, len);
  যদি (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  ফেরত p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ধ্রুবক UResourceBundle* resB, UErrorCode *status) {
  যদি (status==nullptr || U_FAILURE(*status)) {
    ফেরত 0xffffffff;
  }
  যদি(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0xffffffff;
  }
  যদি(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    ফেরত 0xffffffff;
  }
  ফেরত res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ধ্রুবক UResourceBundle* resB, UErrorCode *status) {
  যদি (status==nullptr || U_FAILURE(*status)) {
    ফেরত 0xffffffff;
  }
  যদি(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0xffffffff;
  }
  যদি(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    ফেরত 0xffffffff;
  }
  ফেরত res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ধ্রুবক UResourceBundle *resB) {
  যদি(resB == nullptr) {
    ফেরত URES_NONE;
  }
  ফেরত res_getPublicType(resB->fRes);
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_get_চাবি(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  যদি(resB == nullptr) {
    ফেরত nullptr;
  }
  ফেরত(resB->f_চাবি);
}

U_CAPI int32_t U_EXPORT2 ures_get_আকার(ধ্রুবক UResourceBundle *resB) {
  যদি(resB == nullptr) {
    ফেরত 0;
  }
  
  ফেরত resB->f_আকার;
}

স্থির ধ্রুবক char16_t*
ures_getStringWithAlias(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_সূচক,
                        int32_t* len,
                        UErrorCode* status) {
  যদি(RES_GET_TYPE(r) == URES_ALIAS) {
    ধ্রুবক char16_t* ফলাফল = nullptr;
    UResourceBundle *অস্থায়ী_Res = ures_get_By_সূচক(resB, s_সূচক, nullptr, status);
    ফলাফল = ures_getString(অস্থায়ী_Res, len, status);
    ures_close(অস্থায়ী_Res);
    ফেরত ফলাফল;
  } অন্যথা {
    ফেরত res_getString({resB, s_সূচক}, &resB->get_Res_তথ্য(), r, len); 
  }
}

U_CAPI শূন্য U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  যদি(resB == nullptr) {
    ফেরত;
  }
  resB->f_সূচক = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_পরবর্তী(ধ্রুবক UResourceBundle *resB) {
  যদি(resB == nullptr) {
    ফেরত মিথ্যা;
  }
  ফেরত resB->f_সূচক < resB->f_আকার-1;
}

U_CAPI ধ্রুবক char16_t* U_EXPORT2
ures_get_পরবর্তী_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ধ্রুবক অক্ষর** চাবি,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  যদি (status==nullptr || U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  যদি(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত nullptr;
  }
  
  যদি(resB->f_সূচক == resB->f_আকার-1) {
    *status = U_সূচক_OUTOFBOUNDS_ERROR;
  } অন্যথা {
    resB->f_সূচক++;
    নির্বাচন(RES_GET_TYPE(resB->fRes)) {
    ক্ষেত্র URES_STRING:
    ক্ষেত্র URES_STRING_V2:
      ফেরত res_getString({resB}, &resB->get_Res_তথ্য(), resB->fRes, len);
    ক্ষেত্র URES_TABLE:
    ক্ষেত্র URES_TABLE16:
    ক্ষেত্র URES_TABLE32:
      r = res_get_Table_Item_By_সূচক(&resB->get_Res_তথ্য(), resB->fRes, resB->f_সূচক, চাবি);
      যদি(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ফেরত ures_getStringWithAlias(resB, r, resB->f_সূচক, len, status);
    ক্ষেত্র URES_ARRAY:
    ক্ষেত্র URES_ARRAY16:
      r = res_getArrayItem(&resB->get_Res_তথ্য(), resB->fRes, resB->f_সূচক);
      যদি(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ফেরত ures_getStringWithAlias(resB, r, resB->f_সূচক, len, status);
    ক্ষেত্র URES_ALIAS:
      ফেরত ures_getStringWithAlias(resB, resB->fRes, resB->f_সূচক, len, status);
    ক্ষেত্র URES_INT:
    ক্ষেত্র URES_দ্বিমিক:
    ক্ষেত্র URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    পূর্বনির্ধারিত:
      ফেরত nullptr;
    }
  }

  ফেরত nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_পরবর্তী_Resource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    ধ্রুবক অক্ষর *চাবি = nullptr;
    Resource r = RES_BOGUS;

    যদি (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            ফেরত fillIn;
    }
    যদি(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            ফেরত fillIn;
    }

    যদি(resB->f_সূচক == resB->f_আকার-1) {
      *status = U_সূচক_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } অন্যথা {
        resB->f_সূচক++;
        নির্বাচন(RES_GET_TYPE(resB->fRes)) {
        ক্ষেত্র URES_INT:
        ক্ষেত্র URES_দ্বিমিক:
        ক্ষেত্র URES_STRING:
        ক্ষেত্র URES_STRING_V2:
        ক্ষেত্র URES_INT_VECTOR:
            ফেরত ures_copyResb(fillIn, resB, status);
        ক্ষেত্র URES_TABLE:
        ক্ষেত্র URES_TABLE16:
        ক্ষেত্র URES_TABLE32:
            r = res_get_Table_Item_By_সূচক(&resB->get_Res_তথ্য(), resB->fRes, resB->f_সূচক, &চাবি);
            যদি(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ফেরত init_resb_ফলাফল(resB->f_তথ্য, r, চাবি, resB->f_সূচক, resB, fillIn, status);
        ক্ষেত্র URES_ARRAY:
        ক্ষেত্র URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_তথ্য(), resB->fRes, resB->f_সূচক);
            যদি(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ফেরত init_resb_ফলাফল(resB->f_তথ্য, r, চাবি, resB->f_সূচক, resB, fillIn, status);
        পূর্বনির্ধারিত:
            /*return nullptr;*/
            ফেরত fillIn;
        }
    }
    /*return nullptr;*/
    ফেরত fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_সূচক(ধ্রুবক UResourceBundle* resB,
                int32_t সূচক_R,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    ধ্রুবক অক্ষর* চাবি = nullptr;
    Resource r = RES_BOGUS;

    যদি (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        ফেরত fillIn;
    }
    যদি(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        ফেরত fillIn;
    }

    যদি(সূচক_R >= 0 && resB->f_আকার > সূচক_R) {
        নির্বাচন(RES_GET_TYPE(resB->fRes)) {
        ক্ষেত্র URES_INT:
        ক্ষেত্র URES_দ্বিমিক:
        ক্ষেত্র URES_STRING:
        ক্ষেত্র URES_STRING_V2:
        ক্ষেত্র URES_INT_VECTOR:
            ফেরত ures_copyResb(fillIn, resB, status);
        ক্ষেত্র URES_TABLE:
        ক্ষেত্র URES_TABLE16:
        ক্ষেত্র URES_TABLE32:
            r = res_get_Table_Item_By_সূচক(&resB->get_Res_তথ্য(), resB->fRes, সূচক_R, &চাবি);
            যদি(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ফেরত init_resb_ফলাফল(resB->f_তথ্য, r, চাবি, সূচক_R, resB, fillIn, status);
        ক্ষেত্র URES_ARRAY:
        ক্ষেত্র URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_তথ্য(), resB->fRes, সূচক_R);
            যদি(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ফেরত init_resb_ফলাফল(resB->f_তথ্য, r, চাবি, সূচক_R, resB, fillIn, status);
        পূর্বনির্ধারিত:
            /*return nullptr;*/
            ফেরত fillIn;
        }
    } অন্যথা {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    ফেরত fillIn;
}

U_CAPI ধ্রুবক char16_t* U_EXPORT2
ures_get_String_By_সূচক(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t সূচক_S,
                      int32_t* len,
                      UErrorCode* status) {
    ধ্রুবক অক্ষর* চাবি = nullptr;
    Resource r = RES_BOGUS;

    যদি (status==nullptr || U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি(সূচক_S >= 0 && resB->f_আকার > সূচক_S) {
        নির্বাচন(RES_GET_TYPE(resB->fRes)) {
        ক্ষেত্র URES_STRING:
        ক্ষেত্র URES_STRING_V2:
            ফেরত res_getString({resB}, &resB->get_Res_তথ্য(), resB->fRes, len);
        ক্ষেত্র URES_TABLE:
        ক্ষেত্র URES_TABLE16:
        ক্ষেত্র URES_TABLE32:
            r = res_get_Table_Item_By_সূচক(&resB->get_Res_তথ্য(), resB->fRes, সূচক_S, &চাবি);
            যদি(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ফেরত ures_getStringWithAlias(resB, r, সূচক_S, len, status);
        ক্ষেত্র URES_ARRAY:
        ক্ষেত্র URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_তথ্য(), resB->fRes, সূচক_S);
            যদি(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ফেরত ures_getStringWithAlias(resB, r, সূচক_S, len, status);
        ক্ষেত্র URES_ALIAS:
            ফেরত ures_getStringWithAlias(resB, resB->fRes, সূচক_S, len, status);
        ক্ষেত্র URES_INT:
        ক্ষেত্র URES_দ্বিমিক:
        ক্ষেত্র URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            বিরতি;
        পূর্বনির্ধারিত:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          বিরতি;
        }
    } অন্যথা {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    ফেরত nullptr;
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_get_UTF8_String_By_সূচক(ধ্রুবক UResourceBundle* resB,
                          int32_t idx,
                          অক্ষর* dest U_LIFETIME_BOUND,
                          int32_t* p_দৈর্ঘ্য,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    ধ্রুবক char16_t *s16 = ures_get_String_By_সূচক(resB, idx, &length16, status);
    ফেরত ures_toUTF8String(s16, length16, dest, p_দৈর্ঘ্য, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(ধ্রুবক অক্ষর* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *ফলাফল = fillIn;
  অক্ষর *packageName = nullptr;
  অক্ষর *pathToResource = nullptr, *save = nullptr;
  অক্ষর *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  যদি(status == nullptr || U_FAILURE(*status)) {
    ফেরত ফলাফল;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (অক্ষর *)uprv_malloc(length*আকার(অক্ষর));
  /* test for nullptr */
  যদি(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত ফলাফল;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  যদি(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    যদি(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } অন্যথা {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  যদি(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  যদি(U_SUCCESS(*status)) {
    যদি(localeEnd) {
      ফলাফল = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } অন্যথা {
      ফলাফল = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  ফেরত ফলাফল;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(ধ্রুবক UResourceBundle* resB,
                     অক্ষর* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *ফলাফল = fillIn;
  ধ্রুবক অক্ষর *চাবি;

  যদি(status == nullptr || U_FAILURE(*status)) {
    ফেরত ফলাফল;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  করো {
    res = res_findResource(&resB->get_Res_তথ্য(), resB->fRes, &path, &চাবি); 
    যদি(res != RES_BOGUS) {
        ফলাফল = init_resb_ফলাফল(resB->f_তথ্য, res, চাবি, -1, resB, fillIn, status);
        resB = ফলাফল;
    } অন্যথা {
        *status = U_MISSING_RESOURCE_ERROR;
        বিরতি;
    }
  } যখন(*path); /* there is more stuff in the path */

  ফেরত ফলাফল;
}

U_CAPI ধ্রুবক char16_t* U_EXPORT2
ures_get_String_By_চাবি_With_Fallback(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND,
                                ধ্রুবক অক্ষর* in_চাবি,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle স্তূপ_2;
    ধ্রুবক char16_t* retVal = nullptr;
    ures_init_স্তূপ_Object(&স্তূপ_2);
    ures_get_By_চাবি_With_Fallback(resB, in_চাবি, &স্তূপ_2, status);
    int32_t length;
    retVal = ures_getString(&স্তূপ_2, &length, status);
    ures_close(&স্তূপ_2);
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    যদি (len != nullptr) {
        *len = length;
    }
    ফেরত retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
স্থির Resource get_Table_Item_By_চাবি_Path(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource table, ধ্রুবক অক্ষর *চাবি) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(চাবি, errorCode);
  যদি (U_FAILURE(errorCode)) { ফেরত RES_BOGUS; }
  অক্ষর *pathPart = path.তথ্য();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  যখন (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    অক্ষর *পরবর্তী_Path_Part = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    যদি (পরবর্তী_Path_Part != nullptr) {
      *পরবর্তী_Path_Part = 0;  /* Terminating null for this part of path. */
      পরবর্তী_Path_Part++;
    } অন্যথা {
      পরবর্তী_Path_Part = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    ধ্রুবক অক্ষর *pathP = pathPart;
    resource = res_get_Table_Item_By_চাবি(p_Res_তথ্য, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = পরবর্তী_Path_Part; 
  }
  যদি (*pathPart) {
    ফেরত RES_BOGUS;
  }
  ফেরত resource;
}

স্থির শূন্য createPath(ধ্রুবক অক্ষর* origResPath,
                       int32_t     origResPathLen,
                       ধ্রুবক অক্ষর* resPath,
                       int32_t     resPathLen,
                       ধ্রুবক অক্ষর* in_চাবি,
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
    ধ্রুবক অক্ষর* চাবি = in_চাবি;
    যদি (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        যদি (U_SUCCESS(*status)) {
            ধ্রুবক অক্ষর* resPathLimit = resPath + resPathLen;
            ধ্রুবক অক্ষর* origResPathLimit = origResPath + origResPathLen;
            ধ্রুবক অক্ষর* resPathPtr = resPath;
            ধ্রুবক অক্ষর* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            যখন (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                যখন (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                যদি (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                যখন (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                যদি (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            যখন (resPathPtr < resPathLimit && *চাবি != '\0') {
                যখন (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                যদি (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                যখন (*চাবি != '\0' && *চাবি != RES_PATH_SEPARATOR) {
                    ++চাবি;
                }
                যদি (*চাবি == RES_PATH_SEPARATOR) {
                    ++চাবি;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(চাবি, *status);
    } অন্যথা {
        path.append(in_চাবি, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_চাবি_With_Fallback(ধ্রুবক UResourceBundle* resB,
                          ধ্রুবক অক্ষর* in_চাবি,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, মূল_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    যদি (status==nullptr || U_FAILURE(*status)) {
        ফেরত fillIn;
    }
    যদি(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    যদি(URES_IS_TABLE(type)) {
        ধ্রুবক অক্ষর* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_চাবি_Path(&resB->get_Res_তথ্য(), resB->fRes, in_চাবি);
        ধ্রুবক অক্ষর* চাবি = in_চাবি;
        যৌক্তিক did_মূল_Once = মিথ্যা;
        যদি(res == RES_BOGUS) {
            UResource_তথ্য_Entry *তথ্য_Entry = resB->f_তথ্য;
            CharString path;
            অক্ষর *myPath = nullptr;
            ধ্রুবক অক্ষর* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            যখন(res == RES_BOGUS && (তথ্য_Entry->fParent != nullptr || !did_মূল_Once)) { /* Otherwise, we'll look in parents */
                যদি (তথ্য_Entry->fParent != nullptr) {
                    তথ্য_Entry = তথ্য_Entry->fParent;
                } অন্যথা {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_মূল_Once = সত্য;
                }
                মূল_Res = তথ্য_Entry->f_তথ্য.মূল_Res;

                যদি(তথ্য_Entry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_চাবি, path, status);
                    যদি (U_FAILURE(*status)) {
                        ures_close(helper);
                        ফেরত fillIn;
                    }
                    myPath = path.তথ্য();
                    চাবি = in_চাবি;
                    করো {
                        res = res_findResource(&(তথ্য_Entry->f_তথ্য), মূল_Res, &myPath, &চাবি);
                        যদি (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_ফলাফল(তথ্য_Entry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            যদি(helper) {
                              তথ্য_Entry = helper->f_তথ্য;
                              মূল_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } অন্যথা {
                              বিরতি;
                            }
                        } অন্যথা যদি (res == RES_BOGUS) {
                            বিরতি;
                        }
                    } যখন(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            যদি(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                যদি(uprv_strcmp(তথ্য_Entry->fName, uloc_getDefault())==0 || uprv_strcmp(তথ্য_Entry->fName, k_মূল_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } অন্যথা {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_ফলাফল(তথ্য_Entry, res, চাবি, -1, resB, fillIn, status);
                যদি (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_চাবি, path, status);
                } অন্যথা {
                    ধ্রুবক অক্ষর* separator = nullptr;
                    যদি (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    যদি (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_চাবি, path, status);
                    } অন্যথা {
                        createPath(origResPath, origResPathLen, "", 0, in_চাবি, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.তথ্য(), path.length(), status);
                যদি(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } অন্যথা {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } অন্যথা {
            fillIn = init_resb_ফলাফল(resB->f_তথ্য, res, চাবি, -1, resB, fillIn, status);
        }
    } 
    অন্যথা {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    ফেরত fillIn;
}

নামাকাশ {

শূন্য get_All_বস্তুসমূহ_With_Fallback(
        ধ্রুবক UResourceBundle *bundle, Resource_তথ্য_মান &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
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
    value.set_তথ্য(bundle->get_Res_তথ্য());
    value.set_Valid_Locale_তথ্য_Entry(bundle->f_Valid_Locale_তথ্য_Entry);
    UResource_তথ্য_Entry *parentEntry = bundle->f_তথ্য->fParent;
    UBool hasParent = parentEntry != nullptr && U_SUCCESS(parentEntry->fBogus);
    value.setResource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_চাবি, value, !hasParent, errorCode);
    যদি (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        স্তূপ_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.f_তথ্য = parentEntry;
        parentRef.f_Valid_Locale_তথ্য_Entry = bundle->f_Valid_Locale_তথ্য_Entry;
        parentRef.fHasFallback = !parentRef.get_Res_তথ্য().noFallback;
        parentRef.f_Is_শীর্ষ_Level = সত্য;
        parentRef.fRes = parentRef.get_Res_তথ্য().মূল_Res;
        parentRef.f_আকার = res_গুনতি_Array_বস্তুসমূহ(&parentRef.get_Res_তথ্য(), parentRef.fRes);
        parentRef.f_সূচক = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        স্তূপ_UResource_Bundle containerBundle;
        ধ্রুবক UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        যদি (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } অন্যথা {
            rb = ures_get_By_চাবি_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        যদি (U_SUCCESS(pathErrorCode)) {
            get_All_বস্তুসমূহ_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

কাঠামো GetAllChildrenSink : সর্বজনীন ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    আভাসী ~GetAllChildrenSink() override;
    আভাসী শূন্য put(ধ্রুবক অক্ষর *চাবি, Resource_মান &value, UBool is_মূল,
           UErrorCode &errorCode) override {
        ResourceTable বস্তুসমূহ_Table = value.getTable(errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        জন্য (int32_t i = 0; বস্তুসমূহ_Table.get_চাবি_And_মান(i, চাবি, value); ++i) {
            যদি (value.getType() == URES_ALIAS) {
                Resource_তথ্য_মান& rdv = static_cast<Resource_তথ্য_মান&>(value);
                স্তূপ_UResource_Bundle স্তূপ_অস্থায়ী_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.get_তথ্য(), rdv.getResource(), nullptr, -1,
                                                                          rdv.get_Valid_Locale_তথ্য_Entry(), nullptr, 0,
                                                                          স্তূপ_অস্থায়ী_Bundle.getAlias(), &errorCode);
                যদি (U_SUCCESS(errorCode)) {
                    Resource_তথ্য_মান aliased_মান;
                    aliased_মান.set_তথ্য(aliasRB->get_Res_তথ্য());
                    aliased_মান.set_Valid_Locale_তথ্য_Entry(aliasRB->f_Valid_Locale_তথ্য_Entry);
                    aliased_মান.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    যদি (aliased_মান.getType() != URES_TABLE) {
                        dest.put(চাবি, aliased_মান, is_মূল, errorCode);
                    } অন্যথা {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_মান_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        ধ্রুবক অক্ষর* parent_চাবি = চাবি; // dest.put() changes the key
                        dest.put(parent_চাবি, aliased_মান, is_মূল, errorCode);
                        UResource_তথ্য_Entry* entry = aliasRB->f_তথ্য;
                        Resource res = aliasRB->fRes;
                        যখন (aliased_মান_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            অক্ষর* localPathAsCharPtr = localPath.তথ্য();
                            ধ্রুবক অক্ষর* child_চাবি;
                            entry = entry->fParent;
                            res = entry->f_তথ্য.মূল_Res;
                            Resource newRes = res_findResource(&entry->f_তথ্য, res, &localPathAsCharPtr, &child_চাবি);
                            যদি (newRes != RES_BOGUS) {
                                aliased_মান.set_তথ্য(entry->f_তথ্য);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_মান.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_মান_Type = aliased_মান.getType();
                                যদি (aliased_মান_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_তথ্য_মান& rdv2 = static_cast<Resource_তথ্য_মান&>(aliased_মান);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.get_তথ্য(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.get_Valid_Locale_তথ্য_Entry(), nullptr, 0,
                                                                             স্তূপ_অস্থায়ী_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->f_তথ্য;
                                    res = aliasRB->fRes;
                                    aliased_মান.set_তথ্য(entry->f_তথ্য);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_মান.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_মান_Type = aliased_মান.getType();
                                }
                                যদি (aliased_মান_Type == URES_TABLE) {
                                    dest.put(parent_চাবি, aliased_মান, is_মূল, errorCode);
                                } অন্যথা {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    ফেরত;
                                }
                            }
                        }
                    }
                }
            } অন্যথা {
                dest.put(চাবি, value, is_মূল, errorCode);
            }
            যদি (U_FAILURE(errorCode)) { ফেরত; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI শূন্য U_EXPORT2
ures_getAllChildrenWithFallback(ধ্রুবক UResourceBundle *bundle, ধ্রুবক অক্ষর *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_বস্তুসমূহ_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI শূন্য U_EXPORT2
ures_get_মান_With_Fallback(ধ্রুবক UResourceBundle *bundle, ধ্রুবক অক্ষর *path,
                          UResourceBundle *অস্থায়ী_Fill_In,
                          Resource_তথ্য_মান &value, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    যদি (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    ধ্রুবক UResourceBundle *rb;
    যদি (*path == 0) {
        // empty path
        rb = bundle;
    } অন্যথা {
        rb = ures_get_By_চাবি_With_Fallback(bundle, path, অস্থায়ী_Fill_In, &errorCode);
        যদি (U_FAILURE(errorCode)) {
            ফেরত;
        }
    }
    value.set_তথ্য(rb->get_Res_তথ্য());
    value.set_Valid_Locale_তথ্য_Entry(rb->f_Valid_Locale_তথ্য_Entry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI শূন্য U_EXPORT2
ures_get_All_বস্তুসমূহ_With_Fallback(ধ্রুবক UResourceBundle *bundle, ধ্রুবক অক্ষর *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    যদি (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    স্তূপ_UResource_Bundle স্তূপ_Bundle;
    ধ্রুবক UResourceBundle *rb;
    যদি (*path == 0) {
        // empty path
        rb = bundle;
    } অন্যথা {
        rb = ures_get_By_চাবি_With_Fallback(bundle, path, স্তূপ_Bundle.getAlias(), &errorCode);
        যদি (U_FAILURE(errorCode)) {
            ফেরত;
        }
    }
    // Get all table items with fallback.
    Resource_তথ্য_মান value;
    get_All_বস্তুসমূহ_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_চাবি(ধ্রুবক UResourceBundle* resB,
              ধ্রুবক অক্ষর* in_চাবি,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResource_তথ্য_Entry *তথ্য_Entry = nullptr;
    ধ্রুবক অক্ষর *চাবি = in_চাবি;

    যদি (status==nullptr || U_FAILURE(*status)) {
        ফেরত fillIn;
    }
    যদি(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    যদি(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_চাবি(&resB->get_Res_তথ্য(), resB->fRes, &t, &চাবি);
        যদি(res == RES_BOGUS) {
            চাবি = in_চাবি;
            যদি(resB->fHasFallback) {
                তথ্য_Entry = get_Fallback_তথ্য(resB, &চাবি, &res, status);
                যদি(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    ফেরত init_resb_ফলাফল(তথ্য_Entry, res, চাবি, -1, resB, fillIn, status);
                } অন্যথা {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } অন্যথা {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } অন্যথা {
            ফেরত init_resb_ফলাফল(resB->f_তথ্য, res, চাবি, -1, resB, fillIn, status);
        }
    } 
#যদি 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    অন্যথা যদি(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == সত্য) {
        /* here should go a first attempt to locate the key using index table */
        তথ্য_Entry = get_Fallback_তথ্য(resB, &চাবি, &res, status);
        যদি(U_SUCCESS(*status)) {
            ফেরত init_resb_ফলাফল(তথ্য_Entry, res, চাবি, resB, fillIn, status);
        } অন্যথা {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    অন্যথা {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত fillIn;
}

U_CAPI ধ্রুবক char16_t* U_EXPORT2
ures_get_String_By_চাবি(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND,
                    ধ্রুবক অক্ষর* in_চাবি,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResource_তথ্য_Entry *তথ্য_Entry = nullptr;
    ধ্রুবক অক্ষর* চাবি = in_চাবি;

    যদি (status==nullptr || U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    যদি(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_চাবি(&resB->get_Res_তথ্য(), resB->fRes, &t, &চাবি);

        যদি(res == RES_BOGUS) {
            চাবি = in_চাবি;
            যদি(resB->fHasFallback) {
                তথ্য_Entry = get_Fallback_তথ্য(resB, &চাবি, &res, status);
                যদি(U_SUCCESS(*status)) {
                    নির্বাচন (RES_GET_TYPE(res)) {
                    ক্ষেত্র URES_STRING:
                    ক্ষেত্র URES_STRING_V2:
                        ফেরত res_getString({resB, চাবি}, &তথ্য_Entry->f_তথ্য, res, len);
                    ক্ষেত্র URES_ALIAS:
                      {
                        ধ্রুবক char16_t* ফলাফল = nullptr;
                        UResourceBundle *অস্থায়ী_Res = ures_get_By_চাবি(resB, in_চাবি, nullptr, status);
                        ফলাফল = ures_getString(অস্থায়ী_Res, len, status);
                        ures_close(অস্থায়ী_Res);
                        ফেরত ফলাফল;
                      }
                    পূর্বনির্ধারিত:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } অন্যথা {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } অন্যথা {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } অন্যথা {
            নির্বাচন (RES_GET_TYPE(res)) {
            ক্ষেত্র URES_STRING:
            ক্ষেত্র URES_STRING_V2:
                ফেরত res_getString({resB, চাবি}, &resB->get_Res_তথ্য(), res, len);
            ক্ষেত্র URES_ALIAS:
              {
                ধ্রুবক char16_t* ফলাফল = nullptr;
                UResourceBundle *অস্থায়ী_Res = ures_get_By_চাবি(resB, in_চাবি, nullptr, status);
                ফলাফল = ures_getString(অস্থায়ী_Res, len, status);
                ures_close(অস্থায়ী_Res);
                ফেরত ফলাফল;
              }
            পূর্বনির্ধারিত:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#যদি 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    অন্যথা যদি(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == সত্য) {
        /* here should go a first attempt to locate the key using index table */
        তথ্য_Entry = get_Fallback_তথ্য(resB, &চাবি, &res, status);
        যদি(U_SUCCESS(*status)) {
            // TODO: Tracing
            ফেরত res_getString(rd, res, len);
        } অন্যথা {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    অন্যথা {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত nullptr;
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_get_UTF8_String_By_চাবি(ধ্রুবক UResourceBundle* resB,
                        ধ্রুবক অক্ষর* চাবি,
                        অক্ষর* dest U_LIFETIME_BOUND,
                        int32_t* p_দৈর্ঘ্য,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    ধ্রুবক char16_t *s16 = ures_get_String_By_চাবি(resB, চাবি, &length16, status);
    ফেরত ures_toUTF8String(s16, length16, dest, p_দৈর্ঘ্য, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_getLocaleInternal(ধ্রুবক UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    যদি (status==nullptr || U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    } অন্যথা {
      ফেরত resourceBundle->f_তথ্য->fName;
    }
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_getLocale(ধ্রুবক UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  ফেরত ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_getLocaleByType(ধ্রুবক UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_তথ্য_Locale_Type type,
                     UErrorCode* status) {
    যদি (status==nullptr || U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    } অন্যথা {
        নির্বাচন(type) {
        ক্ষেত্র ULOC_ACTUAL_LOCALE:
            ফেরত resourceBundle->f_তথ্য->fName;
        ক্ষেত্র ULOC_VALID_LOCALE:
            ফেরত resourceBundle->f_Valid_Locale_তথ্য_Entry->fName;
        ক্ষেত্র ULOC_REQUESTED_LOCALE:
        পূর্বনির্ধারিত:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত nullptr;
        }
    }
}

U_CFUNC ধ্রুবক অক্ষর*
ures_getName(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND) {
  যদি(resB == nullptr) {
    ফেরত nullptr;
  }

  ফেরত resB->f_তথ্য->fName;
}

#ifdef URES_DEBUG
U_CFUNC ধ্রুবক অক্ষর*
ures_getPath(ধ্রুবক UResourceBundle* resB U_LIFETIME_BOUND) {
  যদি(resB == nullptr) {
    ফেরত nullptr;
  }

  ফেরত resB->f_তথ্য->fPath;
}
#endif

স্থির UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ধ্রুবক অক্ষর* path,
                  ধ্রুবক অক্ষর* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    UResource_তথ্য_Entry *entry;
    যদি(openType != URES_OPEN_DIRECT) {
        যদি (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        যদি(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত nullptr;
        }
        entry = entryOpen(path, canonLocaleID.তথ্য(), openType, status);
    } অন্যথা {
        entry = entryOpenDirect(path, localeID, status);
    }
    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        ফেরত nullptr;
    }

    UBool is_স্তূপ_Object;
    যদি(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(আকার(UResourceBundle)));
        যদি(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        }
        is_স্তূপ_Object = মিথ্যা;
    } অন্যথা {  // fill-in
        is_স্তূপ_Object = ures_is_স্তূপ_Object(r);
        ures_closeBundle(r, মিথ্যা);
    }
    uprv_memset(r, 0, আকার(UResourceBundle));
    ures_set_Is_স্তূপ_Object(r, is_স্তূপ_Object);

    r->f_Valid_Locale_তথ্য_Entry = r->f_তথ্য = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->get_Res_তথ্য().noFallback;
    r->f_Is_শীর্ষ_Level = সত্য;
    r->fRes = r->get_Res_তথ্য().মূল_Res;
    r->f_আকার = res_গুনতি_Array_বস্তুসমূহ(&r->get_Res_তথ্য(), r->fRes);
    r->f_সূচক = -1;

    ResourceTracer(r).traceOpen();

    ফেরত r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* localeID, UErrorCode* status) {
    ফেরত ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_মূল, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* localeID, UErrorCode* status) {
    ফেরত ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_মূল, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* localeID, UErrorCode* status) {
    ফেরত ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI শূন্য U_EXPORT2
ures_openFillIn(UResourceBundle *r, ধ্রুবক অক্ষর* path,
                ধ্রুবক অক্ষর* localeID, UErrorCode* status) {
    যদি(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_মূল, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI শূন্য U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* localeID, UErrorCode* status) {
    যদি(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_গুনতি_Array_বস্তুসমূহ(ধ্রুবক UResourceBundle* resourceBundle,
                  ধ্রুবক অক্ষর* resource_চাবি,
                  UErrorCode* status)
{
    UResourceBundle res_তথ্য;
    ures_init_স্তূপ_Object(&res_তথ্য);
    যদি (status==nullptr || U_FAILURE(*status)) {
        ফেরত 0;
    }
    যদি(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    ures_get_By_চাবি(resourceBundle, resource_চাবি, &res_তথ্য, status);
    
    যদি(res_তথ্য.get_Res_তথ্য().তথ্য != nullptr) {
        int32_t ফলাফল = res_গুনতি_Array_বস্তুসমূহ(&res_তথ্য.get_Res_তথ্য(), res_তথ্য.fRes);
        ures_close(&res_তথ্য);
        ফেরত ফলাফল;
    } অন্যথা {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_তথ্য);
        ফেরত 0;
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
U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_get_Version_সংখ্যা_Internal(ধ্রুবক UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    যদি (!resourceBundle) ফেরত nullptr;

    যদি(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ধ্রুবক char16_t* minor_version = ures_get_String_By_চাবি(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (অক্ষর *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        যদি (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            ফেরত nullptr;
        }
       
        যদি(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        অন্যথা {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    ফেরত resourceBundle->fVersion;
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ures_get_Version_সংখ্যা(ধ্রুবক UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ফেরত ures_get_Version_সংখ্যা_Internal(resourceBundle);
}

U_CAPI শূন্য U_EXPORT2 ures_getVersion(ধ্রুবক UResourceBundle* resB, UVersionInfo versionInfo) {
    যদি (!resB) ফেরত;

    u_versionFromString(versionInfo, ures_get_Version_সংখ্যা_Internal(resB));
}

/** Tree support functions *******************************/
#define সূচক_LOCALE_NAME "res_index"
#define সূচক_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#যদি defined(URES_বৃক্ষ_DEBUG)
#অন্তর্ভাব <stdio.h>
#endif

প্রকারলেখ কাঠামো ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

স্থির শূন্য U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

স্থির int32_t U_CALLCONV
ures_loc_গুনতি_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    ফেরত ures_get_আকার(&ctx->installed);
}

U_CDECL_BEGIN


স্থির ধ্রুবক অক্ষর * U_CALLCONV
ures_loc_পরবর্তী_Locale(UEnumeration* en,
                    int32_t* ফলাফল_দৈর্ঘ্য,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    ধ্রুবক অক্ষর *ফলাফল = nullptr;
    int32_t len = 0;
    যদি (ures_has_পরবর্তী(res) && (k = ures_get_পরবর্তী_Resource(res, &ctx->curr, status)) != nullptr) {
        ফলাফল = ures_get_চাবি(k);
        len = (int32_t)uprv_strlen(ফলাফল);
    }
    যদি (ফলাফল_দৈর্ঘ্য) {
        *ফলাফল_দৈর্ঘ্য = len;
    }
    ফেরত ফলাফল;
}

স্থির শূন্য U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

স্থির ধ্রুবক UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_গুনতি_Locales,
        uenum_unextDefault,
        ures_loc_পরবর্তী_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(ধ্রুবক অক্ষর *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(আকার(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(আকার(UEnumeration));
    যদি(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        ফেরত nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, আকার(UEnumeration));

    ures_init_স্তূপ_Object(&myContext->installed);
    ures_init_স্তূপ_Object(&myContext->curr);
    idx = ures_openDirect(path, সূচক_LOCALE_NAME, status);
    ures_get_By_চাবি(idx, সূচক_TAG, &myContext->installed, status);
    যদি(U_SUCCESS(*status)) {
#যদি defined(URES_বৃক্ষ_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, সূচক_LOCALE_NAME, সূচক_TAG, ures_get_চাবি(&myContext->installed));
#endif
        en->context = myContext;
    } অন্যথা {
#যদি defined(URES_বৃক্ষ_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    ফেরত en;
}

স্থির UBool isLocaleInList(UEnumeration *locEnum, ধ্রুবক অক্ষর *loc_To_অনুসন্ধান, UErrorCode *status) {
    ধ্রুবক অক্ষর *loc;
    যখন ((loc = uenum_পরবর্তী(locEnum, nullptr, status)) != nullptr) {
        যদি (uprv_strcmp(loc, loc_To_অনুসন্ধান) == 0) {
            ফেরত সত্য;
        }
    }
    ফেরত মিথ্যা;
}

স্থির শূন্য getParentForFunctionalEquivalent(ধ্রুবক অক্ষর*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    যদি (res != nullptr) {
        ures_get_By_চাবি(res, "%%Parent", bund1, &subStatus);
        যদি (U_SUCCESS(subStatus)) {
            int32_t length16;
            ধ্রুবক char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    যদি (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(অক্ষর *ফলাফল, int32_t ফলাফল_Capacity,
                             ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *resName, ধ্রুবক অক্ষর *keyword, ধ্রুবক অক্ষর *locid,
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
    যদি(U_FAILURE(*status)) ফেরত 0;
    CharString kwVal;
    যদি (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_মান(locid, keyword, subStatus);
        যদি (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    যদি (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#যদি defined(URES_বৃক্ষ_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.তথ্য(), base.তথ্য(), u_errorName(subStatus));
#endif
    ures_init_স্তূপ_Object(&bund1);
    ures_init_স্তূপ_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    যদি(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = সত্য;
        যদি (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.তথ্য(), &subStatus);
        }
        uenum_close(locEnum);
    }

    যদি(U_FAILURE(subStatus)) {
        *status = subStatus;
        ফেরত 0;
    }
    
    করো {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.তথ্য(), &subStatus);
        যদি(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = মিথ্যা;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#যদি defined(URES_বৃক্ষ_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.তথ্য(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        যদি(U_FAILURE(subStatus)) {
            *status = subStatus;
        } অন্যথা যদি(subStatus == U_ZERO_ERROR) {
            ures_get_By_চাবি(res,resName,&bund1, &subStatus);
            যদি(subStatus == U_ZERO_ERROR) {
                ধ্রুবক char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#যদি defined(URES_বৃক্ষ_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.তথ্য(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_চাবি(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                যদি(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#যদি defined(URES_বৃক্ষ_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.তথ্য(), keyword, defVal.তথ্য(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    যদি(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#যদি defined(URES_বৃক্ষ_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.তথ্য(), keyword, kwVal.তথ্য());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        যদি (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        যদি (found != parent) {
            parent.copyFrom(found, subStatus);
        } অন্যথা {
            getParentForFunctionalEquivalent(found.তথ্য(),res,&bund1,parent);
        }
        ures_close(res);
    } যখন(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    করো {
        res = ures_open(path, parent.তথ্য(), &subStatus);
        যদি((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = মিথ্যা;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#যদি defined(URES_বৃক্ষ_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.তথ্য(), u_errorName(subStatus), kwVal.তথ্য());
#endif
        যদি(U_FAILURE(subStatus)) {
            *status = subStatus;
        } অন্যথা যদি(subStatus == U_ZERO_ERROR) {
            ures_get_By_চাবি(res,resName,&bund1, &subStatus);
#যদি defined(URES_বৃক্ষ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            যদি(subStatus == U_ZERO_ERROR) {
                ures_get_By_চাবি(&bund1, kwVal.তথ্য(), &bund2, &subStatus);
#যদি defined(URES_বৃক্ষ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.তথ্য(), u_errorName(subStatus));
#endif
                যদি(subStatus == U_ZERO_ERROR) {
#যদি defined(URES_বৃক্ষ_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.তথ্য(), keyword, kwVal.তথ্য(), u_errorName(subStatus));
#endif
                    যদি (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } অন্যথা {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        যদি(defLoc.length() > full.length()) {
                          ধ্রুবক char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#যদি defined(URES_বৃক্ষ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.তথ্য());
#endif
                          defUstr = ures_get_String_By_চাবি(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          যদি(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#যদি defined(URES_বৃক্ষ_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.তথ্য(), keyword, defVal.তথ্য(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#যদি defined(URES_বৃক্ষ_DEBUG)
                        অন্যথা {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.তথ্য(), full.তথ্য());
                        }
#endif
                } অন্যথা {
#যদি defined(URES_বৃক্ষ_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.তথ্য(), kwVal.তথ্য());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = মিথ্যা;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        যদি (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            ধ্রুবক অক্ষর *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            যদি (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                যদি (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = সত্য;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        যদি (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.তথ্য(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } যখন(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    যদি(full.isEmpty() && kwVal != defVal) {
#যদি defined(URES_বৃক্ষ_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.তথ্য(), defVal.তথ্য());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        করো { /* search for 'default' named item */
            res = ures_open(path, parent.তথ্য(), &subStatus);
            যদি((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = মিথ্যা;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#যদি defined(URES_বৃক্ষ_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.তথ্য(), u_errorName(subStatus), kwVal.তথ্য());
#endif
            যদি(U_FAILURE(subStatus)) {
                *status = subStatus;
            } অন্যথা যদি(subStatus == U_ZERO_ERROR) {
                ures_get_By_চাবি(res,resName,&bund1, &subStatus);
                যদি(subStatus == U_ZERO_ERROR) {
                    ures_get_By_চাবি(&bund1, kwVal.তথ্য(), &bund2, &subStatus);
                    যদি(subStatus == U_ZERO_ERROR) {
#যদি defined(URES_বৃক্ষ_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.তথ্য(), keyword, kwVal.তথ্য(), u_errorName(subStatus));
#endif
                        যদি (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } অন্যথা {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        যদি(defLoc.length() > full.length()) {
                          ধ্রুবক char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#যদি defined(URES_বৃক্ষ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.তথ্য());
#endif
                          defUstr = ures_get_String_By_চাবি(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          যদি(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#যদি defined(URES_বৃক্ষ_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.তথ্য(), keyword, defVal.তথ্য(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#যদি defined(URES_বৃক্ষ_DEBUG)
                        অন্যথা {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.তথ্য(), full.তথ্য());
                        }
#endif
                    }
                }
            }
            
            subStatus = U_ZERO_ERROR;
            found.copyFrom(parent, subStatus);
            getParentForFunctionalEquivalent(found.তথ্য(),res,&bund1,parent);
            ures_close(res);
            subStatus = U_ZERO_ERROR;
        } যখন(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    যদি(U_SUCCESS(*status)) {
        যদি(full.isEmpty()) {
#যদি defined(URES_বৃক্ষ_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.তথ্য());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } অন্যথা যদি(omitDefault) {
#যদি defined(URES_বৃক্ষ_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.তথ্য(), defLoc.তথ্য(), found.তথ্য());
#endif        
          যদি(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            যদি(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#যদি defined(URES_বৃক্ষ_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.তথ্য());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        যদি(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } অন্যথা যদি(!omitDefault) {
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

    যদি(U_SUCCESS(*status)) {
        int32_t copy_দৈর্ঘ্য = uprv_min(length, ফলাফল_Capacity);
        যদি(copy_দৈর্ঘ্য>0) {
            found.extract(ফলাফল, copy_দৈর্ঘ্য, subStatus);
        }
        যদি(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } অন্যথা {
        length = 0;
        ফলাফল[0]=0;
    }
    ফেরত u_terminateChars(ফলাফল, ফলাফল_Capacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_মানসমূহ(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *keyword, UErrorCode *status)
{
#define মানসমূহ_BUF_আকার 2048
#define মানসমূহ_LIST_আকার 512
    
    অক্ষর       মানসমূহ_Buf[মানসমূহ_BUF_আকার];
    int32_t    মানসমূহ_সূচক = 0;
    ধ্রুবক অক্ষর *মানসমূহ_List[মানসমূহ_LIST_আকার];
    int32_t    মানসমূহ_গুনতি = 0;
    
    ধ্রুবক অক্ষর *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_স্তূপ_Object(&item);
    ures_init_স্তূপ_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    যদি(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        ফেরত nullptr;
    }
    
    মানসমূহ_Buf[0]=0;
    মানসমূহ_Buf[1]=0;

    যখন ((locale = uenum_পরবর্তী(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#যদি defined(URES_বৃক্ষ_DEBUG)
        যদি(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_চাবি(bund, keyword, &item, &subStatus);
        
        যদি(!bund || U_FAILURE(subStatus)) {
#যদি defined(URES_বৃক্ষ_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            চালু;
        }

        যখন ((subPtr = ures_get_পরবর্তী_Resource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            ধ্রুবক অক্ষর *k;
            int32_t i;
            k = ures_get_চাবি(subPtr);

#যদি defined(URES_বৃক্ষ_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            যদি(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                চালু;
            }
            জন্য(i=0; i<মানসমূহ_গুনতি; i++) {
                যদি(!uprv_strcmp(মানসমূহ_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    বিরতি;
                }
            }
            যদি(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                যদি((মানসমূহ_গুনতি >= (মানসমূহ_LIST_আকার-1)) ||       /* no more space in list .. */
                    ((মানসমূহ_সূচক+kLen+1+1) >= মানসমূহ_BUF_আকার)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } অন্যথা {
                    uprv_strcpy(মানসমূহ_Buf+মানসমূহ_সূচক, k);
                    মানসমূহ_List[মানসমূহ_গুনতি++] = মানসমূহ_Buf+মানসমূহ_সূচক;
                    মানসমূহ_সূচক += kLen;
#যদি defined(URES_বৃক্ষ_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    মানসমূহ_Buf[মানসমূহ_সূচক++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    মানসমূহ_Buf[মানসমূহ_সূচক++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#যদি defined(URES_বৃক্ষ_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        মানসমূহ_সূচক, মানসমূহ_গুনতি);
#endif
    ফেরত uloc_openKeywordList(মানসমূহ_Buf, মানসমূহ_সূচক, status);
}
#যদি 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ধ্রুবক UResourceBundle* res1, ধ্রুবক UResourceBundle* res2){
    যদি(res1==nullptr || res2==nullptr){
        ফেরত res1==res2; /* pointer comparison */
    }
    যদি(res1->f_চাবি==nullptr||  res2->f_চাবি==nullptr){
        ফেরত (res1->f_চাবি==res2->f_চাবি);
    }অন্যথা{
        যদি(uprv_strcmp(res1->f_চাবি, res2->f_চাবি)!=0){
            ফেরত মিথ্যা;
        }
    }
    যদি(uprv_strcmp(res1->f_তথ্য->fName, res2->f_তথ্য->fName)!=0){
        ফেরত মিথ্যা;
    }
    যদি(res1->f_তথ্য->fPath == nullptr||  res2->f_তথ্য->fPath==nullptr){
        ফেরত (res1->f_তথ্য->fPath == res2->f_তথ্য->fPath);
    }অন্যথা{
        যদি(uprv_strcmp(res1->f_তথ্য->fPath, res2->f_তথ্য->fPath)!=0){
            ফেরত মিথ্যা;
        }
    }
    যদি(uprv_strcmp(res1->f_তথ্য->fParent->fName, res2->f_তথ্য->fParent->fName)!=0){
        ফেরত মিথ্যা;
    }
    যদি(uprv_strcmp(res1->f_তথ্য->fParent->fPath, res2->f_তথ্য->fParent->fPath)!=0){
        ফেরত মিথ্যা;
    }
    যদি(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        ফেরত মিথ্যা;
    }
    যদি(res1->fRes != res2->fRes){
        ফেরত মিথ্যা;
    }
    ফেরত সত্য;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ধ্রুবক UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    যদি(U_FAILURE(*status) || res == nullptr){
        ফেরত nullptr;
    }
    bundle = ures_open(res->f_তথ্য->fPath, res->f_তথ্য->fName, status);
    যদি(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }অন্যথা{
        ret = bundle;
    }
    ফেরত ret;
}
U_CAPI ধ্রুবক UResourceBundle* U_EXPORT2
ures_getParentBundle(ধ্রুবক UResourceBundle* res){
    যদি(res==nullptr){
        ফেরত nullptr;
    }
    ফেরত res->fParentRes;
}
#endif

U_CAPI শূন্য U_EXPORT2
ures_get_Version_By_চাবি(ধ্রুবক UResourceBundle* res, ধ্রুবক অক্ষর *চাবি, UVersionInfo ver, UErrorCode *status) {
  ধ্রুবক char16_t *str;
  int32_t len;
  str = ures_get_String_By_চাবি(res, চাবি, &len, status);
  যদি(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
