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

#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "cwchar.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "unicode/uenum.h"
#அந்தர்பாவ "uenumimp.h"
#அந்தர்பாவ "ulocimp.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "uresdata.h"

using நாமாகாஶ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
நிலையான UHashtable *cache = nullptr;
நிலையான icu::UInitOnce gCacheInitOnce {};

நிலையான UMutex resbMutex;

/* INTERNAL: hashes an entry  */
நிலையான int32_t U_CALLCONV hashEntry(மாறிலி UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    திரும்பு uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
நிலையான UBool U_CALLCONV compareEntries(மாறிலி UHashTok p1, மாறிலி UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    திரும்பு uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
நிலையான UBool chopLocale(எழுத்து *name) {
    எழுத்து *i = uprv_strrchr(name, '_');

    எனில்(i != nullptr) {
        *i = '\0';
        திரும்பு உண்மை;
    }

    திரும்பு பொய்;
}

நிலையான UBool hasVariant(மாறிலி எழுத்து* localeID) {
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
    திரும்பு sink.எண்_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#அந்தர்பாவ "localefallback_data.h"

நிலையான மாறிலி எழுத்து* performFallbackLookup(மாறிலி எழுத்து* சாவி,
                                         மாறிலி எழுத்து* சாவி_Strs,
                                         மாறிலி எழுத்து* மதிப்பு_Strs U_LIFETIME_BOUND,
                                         மாறிலி int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    மாறிலி int32_t* bottom = lookupTable;
    மாறிலி int32_t* உச்சி = lookupTable + lookupTableLength;

    போது (bottom < உச்சி) {
        // Effectively, divide by 2 and round down to an even index
        மாறிலி int32_t* நடு = bottom + (((உச்சி - bottom) / 4) * 2);
        மாறிலி எழுத்து* entry_சாவி = &(சாவி_Strs[*நடு]);
        int32_t strcmpResult = uprv_strcmp(சாவி, entry_சாவி);
        எனில் (strcmpResult == 0) {
            திரும்பு &(மதிப்பு_Strs[நடு[1]]);
        } இல்லையெனில் எனில் (strcmpResult < 0) {
            உச்சி = நடு;
        } இல்லையெனில் {
            bottom = நடு + 2;
        }
    }
    திரும்பு nullptr;
}

நிலையான CharString getDefaultScript(மாறிலி CharString& language, மாறிலி CharString& region) {
    மாறிலி எழுத்து* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    எனில் (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        எனில் (U_FAILURE(err)) {
            திரும்பு result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    எனில் (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    எனில் (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    திரும்பு result;
}

எண்ணிக்கை UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_வேர்,
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
    URES_OPEN_LOCALE_வேர்,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
வகைவரை எண்ணிக்கை UResOpenType UResOpenType;

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
நிலையான தருக்க getParentLocaleID(எழுத்து *name, மாறிலி எழுத்து *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    எனில் (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        திரும்பு chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    எனில் (U_FAILURE(err)) {
        // hopefully this never happens...
        திரும்பு chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    எனில் (openType == URES_OPEN_LOCALE_DEFAULT_வேர்) {
        மாறிலி எழுத்து* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        எனில் (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            திரும்பு உண்மை;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    எனில் (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        எனில் (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } இல்லையெனில் {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } இல்லையெனில் எனில் (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        எனில் (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } இல்லையெனில் {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } இல்லையெனில் எனில் (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        எனில் (openType != URES_OPEN_LOCALE_DEFAULT_வேர் || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } இல்லையெனில் {
            திரும்பு பொய்;
        }
    } இல்லையெனில் {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        திரும்பு பொய்;
    }
    எனில் (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        திரும்பு உண்மை;
    } இல்லையெனில் {
        திரும்பு பொய்;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
நிலையான UBool mayHaveParent(எழுத்து *name) {
    திரும்பு (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
நிலையான வெறுமை entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_மொத்தம்_Existing++;
    போது(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_மொத்தம்_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
நிலையான UResourceDataEntry *getFallbackData(
        மாறிலி UResourceBundle *resBundle,
        மாறிலி எழுத்து **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    எனில்(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        திரும்பு nullptr;
    }
    எனில்(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_சாவி(&(dataEntry->fData), dataEntry->fData.வேர்_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    எனில்(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        போது(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            எனில்(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_சாவி(&(dataEntry->fData), dataEntry->fData.வேர்_Res, &indexR, resTag);
            }
        }
    }

    எனில்(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        திரும்பு nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    எனில்(i>1) {
        எனில்(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_வேர்_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } இல்லையெனில் {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    திரும்பு dataEntry;
}

நிலையான வெறுமை
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    எனில்(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    எனில்(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    எனில்(entry->fPool != nullptr) {
        --entry->fPool->f_மொத்தம்_Existing;
    }
    alias = entry->fAlias;
    எனில்(alias != nullptr) {
        போது(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_மொத்தம்_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
நிலையான int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_இலக்கம் = 0;
    மாறிலி UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    எனில் (cache == nullptr) {
        திரும்பு 0;
    }

    செய் {
        deletedMore = பொய்;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        போது ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            எனில் (resB->f_மொத்தம்_Existing == 0) {
                rb_Deleted_இலக்கம்++;
                deletedMore = உண்மை;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } போது(deletedMore);

    திரும்பு rb_Deleted_இலக்கம்;
}

#ifdef URES_DEBUG
#அந்தர்பாவ <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = பொய்;
  int32_t pos = UHASH_FIRST;
  மாறிலி UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    எனில் (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      திரும்பு பொய்;
    }

    போது ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=உண்மை;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (வெறுமை*)resB, resB->f_மொத்தம்_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (வெறுமை*)resB->fPool,
              (வெறுமை*)resB->fAlias,
              (வெறுமை*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_மொத்தம்(cache));
    திரும்பு cacheNotEmpty;
}

#endif

நிலையான UBool U_CALLCONV ures_cleanup()
{
    எனில் (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    திரும்பு உண்மை;
}

/** INTERNAL: Initializes the cache for resources */
நிலையான வெறுமை U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
நிலையான வெறுமை initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

நிலையான வெறுமை setEntryName(UResourceDataEntry *res, மாறிலி எழுத்து *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    எனில்(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    எனில் (len < static_cast<int32_t>(அளவு(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    இல்லையெனில் {
        res->fName = static_cast<எழுத்து*>(uprv_malloc(len + 1));
    }
    எனில்(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } இல்லையெனில் {
        uprv_strcpy(res->fName, name);
    }
}

நிலையான UResourceDataEntry *
getPoolEntry(மாறிலி எழுத்து *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
நிலையான UResourceDataEntry *init_entry(மாறிலி எழுத்து *localeID, மாறிலி எழுத்து *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    மாறிலி எழுத்து *name;
    எழுத்து aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    /* here we try to deduce the right locale name */
    எனில்(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } இல்லையெனில் எனில்(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_வேர்_Locale_Name;
    } இல்லையெனில் { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<எழுத்து*>(name);
    find.fPath = const_cast<எழுத்து*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    எனில்(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(அளவு(UResourceDataEntry)));
        எனில்(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு nullptr;
        }

        uprv_memset(r, 0, அளவு(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        எனில் (U_FAILURE(*status)) {
            uprv_free(r);
            திரும்பு nullptr;
        }

        எனில்(path != nullptr) {
            r->fPath = uprv_strdup(path);
            எனில்(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                திரும்பு nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        எனில் (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            எனில் (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                திரும்பு nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } இல்லையெனில் { /* if we have a regular entry */
            Resource aliasres;
            எனில் (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                எனில் (U_SUCCESS(*status)) {
                    மாறிலி int32_t *poolIndexes = r->fPool->fData.p_வேர் + 1;
                    எனில்(r->fData.p_வேர்[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_சாவிகள் = reinterpret_cast<மாறிலி எழுத்து*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } இல்லையெனில் {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } இல்லையெனில் {
                    r->fBogus = *status;
                }
            }
            எனில் (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                எனில் (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    மாறிலி char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    எனில்(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            எனில் ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (வெறுமை *)r, r, &cacheStatus);
                எனில் (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } இல்லையெனில் {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    எனில்(r != nullptr) {
        /* return the real bundle */
        போது(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_மொத்தம்_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        எனில்(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    திரும்பு r;
}

நிலையான UResourceDataEntry *
getPoolEntry(மாறிலி எழுத்து *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    எனில்( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    திரும்பு poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
நிலையான UResourceDataEntry *
findFirstExisting(மாறிலி எழுத்து* path, எழுத்து* name, மாறிலி எழுத்து* defaultLocale, UResOpenType openType,
                  UBool *is_வேர், UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = பொய்;
    *foundParent = உண்மை; /* we're starting with a fresh name */
    எழுத்து origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    போது(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        எனில் (U_FAILURE(*status)) {
            திரும்பு nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        எனில்(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_மொத்தம்_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } இல்லையெனில் {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_வேர் = static_cast<UBool>(uprv_strcmp(name, k_வேர்_Locale_Name) == 0);

        /*Fallback data stuff*/
        எனில் (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } இல்லையெனில் {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        எனில் (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    திரும்பு r;
}

நிலையான வெறுமை ures_set_Is_அடுக்கு_Object( UResourceBundle* resB, UBool state) {
    எனில்(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } இல்லையெனில் {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

நிலையான UBool ures_is_அடுக்கு_Object(மாறிலி UResourceBundle* resB) {
  திரும்பு((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?பொய்:உண்மை);
}


U_CFUNC வெறுமை ures_init_அடுக்கு_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, அளவு(UResourceBundle));
  ures_set_Is_அடுக்கு_Object(resB, உண்மை);
}

U_NAMESPACE_BEGIN

அடுக்கு_UResource_Bundle::அடுக்கு_UResource_Bundle() {
    ures_init_அடுக்கு_Object(&bundle);
}

அடுக்கு_UResource_Bundle::~அடுக்கு_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

நிலையான UBool  // returns U_SUCCESS(*status)
load_Parents_Except_வேர்(UResourceDataEntry *&t1,
                      எழுத்து name[], int32_t nameCapacity,
                      UBool usingUSRData, எழுத்து usrDataPath[], UErrorCode *status) {
    எனில் (U_FAILURE(*status)) { திரும்பு பொய்; }
    UBool checkParent = உண்மை;
    போது (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        எனில் (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            மாறிலி char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            எனில்(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                எனில் (uprv_strcmp(name, k_வேர்_Locale_Name) == 0) {
                    திரும்பு உண்மை;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        எனில் (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            திரும்பு பொய்;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        எனில் (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            எனில் (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                திரும்பு பொய்;
            }
        }

        எனில் (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } இல்லையெனில் {
            t1->fParent = t2;
            எனில் (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_மொத்தம்_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    திரும்பு உண்மை;
}

நிலையான UBool  // returns U_SUCCESS(*status)
செருகு_வேர்_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    எனில் (U_FAILURE(*status)) { திரும்பு பொய்; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_வேர்_Locale_Name, t1->fPath, &parentStatus);
    எனில் (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        திரும்பு பொய்;
    }
    t1->fParent = t2;
    t1 = t2;
    திரும்பு உண்மை;
}

நிலையான UResourceDataEntry *entryOpen(மாறிலி எழுத்து* path, மாறிலி எழுத்து* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = பொய்;
    UBool is_வேர் = பொய்;
    UBool hasRealData = பொய்;
    UBool hasChopped = உண்மை;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    எழுத்து name[ULOC_FULLNAME_CAPACITY];
    எழுத்து usrDataPath[96];

    initCache(status);

    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    uprv_strncpy(name, localeID, அளவு(name) - 1);
    name[அளவு(name) - 1] = 0;

    எனில் ( usingUSRData ) {
        எனில் ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } இல்லையெனில் {
            uprv_strncpy(usrDataPath, path, அளவு(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[அளவு(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    மாறிலி எழுத்து *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_வேர், &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    எனில் (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        செல் finish;
    }

    எனில்(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = உண்மை;
        எனில் ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            எனில் (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                செல் finish;
            }
            எனில் ( u1 != nullptr ) {
                எனில்(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } இல்லையெனில் {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_மொத்தம்_Existing = 0;
                }
            }
        }
        எனில் ((hasChopped || mayHaveParent(name)) && !is_வேர்) {
            எனில் (!load_Parents_Except_வேர்(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                செல் finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    எனில்(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_வேர் && !isDefault && !is_வேர்) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_வேர், &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        எனில் (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            செல் finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        எனில்(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = உண்மை;
            isDefault = உண்மை;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            எனில் ((hasChopped || mayHaveParent(name)) && !is_வேர்) {
                எனில் (!load_Parents_Except_வேர்(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    செல் finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    எனில்(r == nullptr) {
        uprv_strcpy(name, k_வேர்_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_வேர், &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        எனில் (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            செல் finish;
        }
        எனில்(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = உண்மை;
        } இல்லையெனில் { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            செல் finish;
        }
    } இல்லையெனில் எனில்(!is_வேர் && uprv_strcmp(t1->fName, k_வேர்_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        எனில் (!செருகு_வேர்_Bundle(t1, status)) {
            செல் finish;
        }
        எனில்(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    போது(r != nullptr && !is_வேர் && t1->fParent != nullptr) {
        t1->fParent->f_மொத்தம்_Existing++;
        t1 = t1->fParent;
    }

finish:
    எனில்(U_SUCCESS(*status)) {
        எனில்(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        திரும்பு r;
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
நிலையான UResourceDataEntry *
entryOpenDirect(மாறிலி எழுத்து* path, மாறிலி எழுத்து* localeID, UErrorCode* status) {
    initCache(status);
    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    எனில் (localeID == nullptr) {
        localeID = uloc_getDefault();
    } இல்லையெனில் எனில் (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_வேர்_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    எனில்(U_SUCCESS(*status)) {
        எனில்(r->fBogus != U_ZERO_ERROR) {
            r->f_மொத்தம்_Existing--;
            r = nullptr;
        }
    } இல்லையெனில் {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    எனில்(r != nullptr && uprv_strcmp(localeID, k_வேர்_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        எழுத்து name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        எனில்(!chopLocale(name) || uprv_strcmp(name, k_வேர்_Locale_Name) == 0 ||
                load_Parents_Except_வேர்(t1, name, UPRV_LENGTHOF(name), பொய், nullptr, status)) {
            எனில்(uprv_strcmp(t1->fName, k_வேர்_Locale_Name) != 0 && t1->fParent == nullptr) {
                செருகு_வேர்_Bundle(t1, status);
            }
        }
        எனில்(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    எனில்(r != nullptr) {
        // TODO: Does this ever loop?
        போது(t1->fParent != nullptr) {
            t1->fParent->f_மொத்தம்_Existing++;
            t1 = t1->fParent;
        }
    }
    திரும்பு r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
நிலையான வெறுமை entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    போது(resB != nullptr) {
        p = resB->fParent;
        resB->f_மொத்தம்_Existing--;

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

நிலையான வெறுமை entryClose(UResourceDataEntry *resB) {
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
நிலையான வெறுமை ures_appendResPath(UResourceBundle *resB, மாறிலி எழுத்து* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    எனில்(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    எனில்(RES_BUFSIZE <= resB->fResPathLen+1) {
        எனில்(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<எழுத்து*>(uprv_malloc((resB->fResPathLen + 1) * அளவு(எழுத்து)));
            /* Check that memory was allocated correctly. */
            எனில் (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } இல்லையெனில் {
            எழுத்து* temp = static_cast<எழுத்து*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * அளவு(எழுத்து)));
            /* Check that memory was reallocated correctly. */
            எனில் (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

நிலையான வெறுமை ures_freeResPath(UResourceBundle *resB) {
    எனில் (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

நிலையான வெறுமை
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    எனில்(resB != nullptr) {
        எனில்(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        எனில்(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        எனில்(ures_is_அடுக்கு_Object(resB) == பொய் && freeBundleObj) {
            uprv_free(resB);
        }
#எனில் 0 /*U_DEBUG*/
        இல்லையெனில் {
            /* poison the data */
            uprv_memset(resB, -1, அளவு(UResourceBundle));
        }
#endif
    }
}

U_CAPI வெறுமை  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, உண்மை);
}

நாமாகாஶ {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  மாறிலி எழுத்து* சாவி,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  மாறிலி எழுத்து* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(மாறிலி ResourceData& resData,
                               Resource r,
                               மாறிலி எழுத்து* சாவி,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               மாறிலி எழுத்து* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    எனில் (U_FAILURE(*status)) { திரும்பு resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    மாறிலி char16_t *alias = res_getAlias(&resData, r, &len);
    எனில்(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு resB;
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
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    // We have an alias, now let's cut it up.
    மாறிலி எழுத்து *path = nullptr, *locale = nullptr, *சாவி_Path = nullptr;
    எனில்(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        எழுத்து *chAliasData = chAlias.data();
        எழுத்து *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        எனில்(sep != nullptr) {
            *sep++ = 0;
        }
        எனில்(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            சாவி_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } இல்லையெனில் {
            எனில்(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            எனில் (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } இல்லையெனில் {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                எனில்(sep != nullptr) {
                    *sep++ = 0;
                }
                சாவி_Path = sep;
            }
        }
    } இல்லையெனில் {
        // No path, start with a locale.
        எழுத்து *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        எனில்(sep != nullptr) {
            *sep++ = 0;
        }
        சாவி_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    எனில் (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } இல்லையெனில் {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        எனில்(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            திரும்பு resB;
        }
        dataEntry = mainRes->fData;
    }

    மாறிலி எழுத்து* temp = nullptr;
    எனில்(சாவி_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.வேர்_Res;
        எனில்(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            எனில் (U_FAILURE(*status)) {
                திரும்பு nullptr;
            }
            எழுத்து *a_சாவி = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_சாவி, &temp);
        }
        எனில்(சாவி) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(சாவி, *status);
            எனில் (U_FAILURE(*status)) {
                திரும்பு nullptr;
            }
            எழுத்து *a_சாவி = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_சாவி, &temp);
        } இல்லையெனில் எனில்(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            எனில்(URES_IS_TABLE(type)) {
                மாறிலி எழுத்து *a_சாவி;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_சாவி);
            } இல்லையெனில் { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        எனில்(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } இல்லையெனில் {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } இல்லையெனில் {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(சாவி_Path, *status);
        எனில் (U_FAILURE(*status)) {
            திரும்பு nullptr;
        }
        எழுத்து *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        க்கு(;;) {
            r = dataEntry->fData.வேர்_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            போது(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                எனில்(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    நிறுத்து;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                எனில் (U_FAILURE(*status)) {
                    நிறுத்து;
                }
                எனில் (temp == nullptr || uprv_strcmp(சாவி_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, சாவி_Path, static_cast<int32_t>(uprv_strlen(சாவி_Path)), status);
                    எனில்(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    எனில் (U_FAILURE(*status)) {
                        நிறுத்து;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            எனில் (U_FAILURE(*status) || r != RES_BOGUS) {
                நிறுத்து;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            எனில் (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                நிறுத்து;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, சாவி_Path);
        }
    }
    எனில்(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    திரும்பு resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  மாறிலி எழுத்து* சாவி,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  மாறிலி எழுத்து* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    எனில்(status == nullptr || U_FAILURE(*status)) {
        திரும்பு resB;
    }
    எனில் (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    எனில்(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        எனில்(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            திரும்பு resB;
        }
        திரும்பு getAliasTargetAsResourceBundle(
            dataEntry->fData, r, சாவி, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    எனில்(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(அளவு(UResourceBundle)));
        எனில் (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு nullptr;
        }
        ures_set_Is_அடுக்கு_Object(resB, பொய்);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } இல்லையெனில் {
        எனில்(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        எனில்(resB->fVersion != nullptr) {
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
        எனில்(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = பொய்;
    resB->f_Is_உச்சி_Level = பொய்;
    resB->fIndex = -1;
    resB->f_சாவி = சாவி; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    எனில்(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    எனில்(சாவி != nullptr) {
        ures_appendResPath(resB, சாவி, static_cast<int32_t>(uprv_strlen(சாவி)), status);
        எனில்(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } இல்லையெனில் எனில்(idx >= 0) {
        எழுத்து buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        எனில்(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, அளவு(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_மொத்தம்_Array_பொருட்கள்(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    திரும்பு resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  மாறிலி எழுத்து* சாவி,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  மாறிலி UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    திரும்பு init_resb_result(
        dataEntry, r, சாவி, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               மாறிலி UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_அடுக்கு_Object;
    எனில்(U_FAILURE(*status) || r == original) {
        திரும்பு r;
    }
    எனில்(original != nullptr) {
        எனில்(r == nullptr) {
            is_அடுக்கு_Object = பொய்;
            r = static_cast<UResourceBundle*>(uprv_malloc(அளவு(UResourceBundle)));
            /* test for nullptr */
            எனில் (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு nullptr;
            }
        } இல்லையெனில் {
            is_அடுக்கு_Object = ures_is_அடுக்கு_Object(r);
            ures_closeBundle(r, பொய்);
        }
        uprv_memcpy(r, original, அளவு(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        எனில்(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_அடுக்கு_Object(r, is_அடுக்கு_Object);
        எனில்(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    திரும்பு r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI மாறிலி char16_t* U_EXPORT2 ures_getString(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    மாறிலி char16_t *s;
    எனில் (status==nullptr || U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில்(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    எனில் (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு s;
}

நிலையான மாறிலி எழுத்து*
ures_toUTF8String(மாறிலி char16_t* s16,
                  int32_t length16,
                  எழுத்து* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில் (pLength != nullptr) {
        capacity = *pLength;
    } இல்லையெனில் {
        capacity = 0;
    }
    எனில் (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எனில் (length16 == 0) {
        /* empty string, return as read-only pointer */
        எனில் (pLength != nullptr) {
            *pLength = 0;
        }
        எனில் (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            திரும்பு dest;
        } இல்லையெனில் {
            திரும்பு "";
        }
    } இல்லையெனில் {
        /* We need to transform the string to the destination buffer. */
        எனில் (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            திரும்பு u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        எனில் (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            எனில் (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        திரும்பு u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_getUTF8String(மாறிலி UResourceBundle* resB,
                   எழுத்து* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    மாறிலி char16_t *s16 = ures_getString(resB, &length16, status);
    திரும்பு ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI மாறிலி uint8_t* U_EXPORT2
ures_get_இருமம்(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  மாறிலி uint8_t *p;
  எனில் (status==nullptr || U_FAILURE(*status)) {
    திரும்பு nullptr;
  }
  எனில்(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு nullptr;
  }
  p = res_get_இருமம்({resB}, &resB->getResData(), resB->fRes, len);
  எனில் (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  திரும்பு p;
}

U_CAPI மாறிலி int32_t* U_EXPORT2
ures_getIntVector(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  மாறிலி int32_t *p;
  எனில் (status==nullptr || U_FAILURE(*status)) {
    திரும்பு nullptr;
  }
  எனில்(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  எனில் (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  திரும்பு p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(மாறிலி UResourceBundle* resB, UErrorCode *status) {
  எனில் (status==nullptr || U_FAILURE(*status)) {
    திரும்பு 0xffffffff;
  }
  எனில்(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0xffffffff;
  }
  எனில்(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    திரும்பு 0xffffffff;
  }
  திரும்பு res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(மாறிலி UResourceBundle* resB, UErrorCode *status) {
  எனில் (status==nullptr || U_FAILURE(*status)) {
    திரும்பு 0xffffffff;
  }
  எனில்(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0xffffffff;
  }
  எனில்(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    திரும்பு 0xffffffff;
  }
  திரும்பு res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(மாறிலி UResourceBundle *resB) {
  எனில்(resB == nullptr) {
    திரும்பு URES_NONE;
  }
  திரும்பு res_getPublicType(resB->fRes);
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_get_சாவி(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  எனில்(resB == nullptr) {
    திரும்பு nullptr;
  }
  திரும்பு(resB->f_சாவி);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(மாறிலி UResourceBundle *resB) {
  எனில்(resB == nullptr) {
    திரும்பு 0;
  }
  
  திரும்பு resB->fSize;
}

நிலையான மாறிலி char16_t*
ures_getStringWithAlias(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  எனில்(RES_GET_TYPE(r) == URES_ALIAS) {
    மாறிலி char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    திரும்பு result;
  } இல்லையெனில் {
    திரும்பு res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI வெறுமை U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  எனில்(resB == nullptr) {
    திரும்பு;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(மாறிலி UResourceBundle *resB) {
  எனில்(resB == nullptr) {
    திரும்பு பொய்;
  }
  திரும்பு resB->fIndex < resB->fSize-1;
}

U_CAPI மாறிலி char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   மாறிலி எழுத்து** சாவி,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  எனில் (status==nullptr || U_FAILURE(*status)) {
    திரும்பு nullptr;
  }
  எனில்(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு nullptr;
  }
  
  எனில்(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } இல்லையெனில் {
    resB->fIndex++;
    தேர்வு(RES_GET_TYPE(resB->fRes)) {
    நிலை URES_STRING:
    நிலை URES_STRING_V2:
      திரும்பு res_getString({resB}, &resB->getResData(), resB->fRes, len);
    நிலை URES_TABLE:
    நிலை URES_TABLE16:
    நிலை URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, சாவி);
      எனில்(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      திரும்பு ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    நிலை URES_ARRAY:
    நிலை URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      எனில்(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      திரும்பு ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    நிலை URES_ALIAS:
      திரும்பு ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    நிலை URES_INT:
    நிலை URES_இருமம்:
    நிலை URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    இயல்பு:
      திரும்பு nullptr;
    }
  }

  திரும்பு nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    மாறிலி எழுத்து *சாவி = nullptr;
    Resource r = RES_BOGUS;

    எனில் (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            திரும்பு fillIn;
    }
    எனில்(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            திரும்பு fillIn;
    }

    எனில்(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } இல்லையெனில் {
        resB->fIndex++;
        தேர்வு(RES_GET_TYPE(resB->fRes)) {
        நிலை URES_INT:
        நிலை URES_இருமம்:
        நிலை URES_STRING:
        நிலை URES_STRING_V2:
        நிலை URES_INT_VECTOR:
            திரும்பு ures_copyResb(fillIn, resB, status);
        நிலை URES_TABLE:
        நிலை URES_TABLE16:
        நிலை URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &சாவி);
            எனில்(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            திரும்பு init_resb_result(resB->fData, r, சாவி, resB->fIndex, resB, fillIn, status);
        நிலை URES_ARRAY:
        நிலை URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            எனில்(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            திரும்பு init_resb_result(resB->fData, r, சாவி, resB->fIndex, resB, fillIn, status);
        இயல்பு:
            /*return nullptr;*/
            திரும்பு fillIn;
        }
    }
    /*return nullptr;*/
    திரும்பு fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(மாறிலி UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    மாறிலி எழுத்து* சாவி = nullptr;
    Resource r = RES_BOGUS;

    எனில் (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        திரும்பு fillIn;
    }
    எனில்(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        திரும்பு fillIn;
    }

    எனில்(indexR >= 0 && resB->fSize > indexR) {
        தேர்வு(RES_GET_TYPE(resB->fRes)) {
        நிலை URES_INT:
        நிலை URES_இருமம்:
        நிலை URES_STRING:
        நிலை URES_STRING_V2:
        நிலை URES_INT_VECTOR:
            திரும்பு ures_copyResb(fillIn, resB, status);
        நிலை URES_TABLE:
        நிலை URES_TABLE16:
        நிலை URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &சாவி);
            எனில்(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            திரும்பு init_resb_result(resB->fData, r, சாவி, indexR, resB, fillIn, status);
        நிலை URES_ARRAY:
        நிலை URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            எனில்(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            திரும்பு init_resb_result(resB->fData, r, சாவி, indexR, resB, fillIn, status);
        இயல்பு:
            /*return nullptr;*/
            திரும்பு fillIn;
        }
    } இல்லையெனில் {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    திரும்பு fillIn;
}

U_CAPI மாறிலி char16_t* U_EXPORT2
ures_getStringByIndex(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    மாறிலி எழுத்து* சாவி = nullptr;
    Resource r = RES_BOGUS;

    எனில் (status==nullptr || U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில்(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எனில்(indexS >= 0 && resB->fSize > indexS) {
        தேர்வு(RES_GET_TYPE(resB->fRes)) {
        நிலை URES_STRING:
        நிலை URES_STRING_V2:
            திரும்பு res_getString({resB}, &resB->getResData(), resB->fRes, len);
        நிலை URES_TABLE:
        நிலை URES_TABLE16:
        நிலை URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &சாவி);
            எனில்(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            திரும்பு ures_getStringWithAlias(resB, r, indexS, len, status);
        நிலை URES_ARRAY:
        நிலை URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            எனில்(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            திரும்பு ures_getStringWithAlias(resB, r, indexS, len, status);
        நிலை URES_ALIAS:
            திரும்பு ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        நிலை URES_INT:
        நிலை URES_இருமம்:
        நிலை URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            நிறுத்து;
        இயல்பு:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          நிறுத்து;
        }
    } இல்லையெனில் {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    திரும்பு nullptr;
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_getUTF8StringByIndex(மாறிலி UResourceBundle* resB,
                          int32_t idx,
                          எழுத்து* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    மாறிலி char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    திரும்பு ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(மாறிலி எழுத்து* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  எழுத்து *packageName = nullptr;
  எழுத்து *pathToResource = nullptr, *save = nullptr;
  எழுத்து *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  எனில்(status == nullptr || U_FAILURE(*status)) {
    திரும்பு result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (எழுத்து *)uprv_malloc(length*அளவு(எழுத்து));
  /* test for nullptr */
  எனில்(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    திரும்பு result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  எனில்(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    எனில்(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } இல்லையெனில் {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  எனில்(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  எனில்(U_SUCCESS(*status)) {
    எனில்(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } இல்லையெனில் {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  திரும்பு result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(மாறிலி UResourceBundle* resB,
                     எழுத்து* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  மாறிலி எழுத்து *சாவி;

  எனில்(status == nullptr || U_FAILURE(*status)) {
    திரும்பு result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  செய் {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &சாவி); 
    எனில்(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, சாவி, -1, resB, fillIn, status);
        resB = result;
    } இல்லையெனில் {
        *status = U_MISSING_RESOURCE_ERROR;
        நிறுத்து;
    }
  } போது(*path); /* there is more stuff in the path */

  திரும்பு result;
}

U_CAPI மாறிலி char16_t* U_EXPORT2
ures_get_String_By_சாவி_With_Fallback(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND,
                                மாறிலி எழுத்து* in_சாவி,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle அடுக்கு_2;
    மாறிலி char16_t* retVal = nullptr;
    ures_init_அடுக்கு_Object(&அடுக்கு_2);
    ures_get_By_சாவி_With_Fallback(resB, in_சாவி, &அடுக்கு_2, status);
    int32_t length;
    retVal = ures_getString(&அடுக்கு_2, &length, status);
    ures_close(&அடுக்கு_2);
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில் (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    எனில் (len != nullptr) {
        *len = length;
    }
    திரும்பு retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
நிலையான Resource get_Table_Item_By_சாவி_Path(மாறிலி ResourceData *pResData, Resource table, மாறிலி எழுத்து *சாவி) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(சாவி, errorCode);
  எனில் (U_FAILURE(errorCode)) { திரும்பு RES_BOGUS; }
  எழுத்து *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  போது (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    எழுத்து *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    எனில் (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } இல்லையெனில் {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    மாறிலி எழுத்து *pathP = pathPart;
    resource = res_get_Table_Item_By_சாவி(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  எனில் (*pathPart) {
    திரும்பு RES_BOGUS;
  }
  திரும்பு resource;
}

நிலையான வெறுமை createPath(மாறிலி எழுத்து* origResPath,
                       int32_t     origResPathLen,
                       மாறிலி எழுத்து* resPath,
                       int32_t     resPathLen,
                       மாறிலி எழுத்து* in_சாவி,
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
    மாறிலி எழுத்து* சாவி = in_சாவி;
    எனில் (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        எனில் (U_SUCCESS(*status)) {
            மாறிலி எழுத்து* resPathLimit = resPath + resPathLen;
            மாறிலி எழுத்து* origResPathLimit = origResPath + origResPathLen;
            மாறிலி எழுத்து* resPathPtr = resPath;
            மாறிலி எழுத்து* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            போது (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                போது (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                எனில் (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                போது (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                எனில் (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            போது (resPathPtr < resPathLimit && *சாவி != '\0') {
                போது (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                எனில் (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                போது (*சாவி != '\0' && *சாவி != RES_PATH_SEPARATOR) {
                    ++சாவி;
                }
                எனில் (*சாவி == RES_PATH_SEPARATOR) {
                    ++சாவி;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(சாவி, *status);
    } இல்லையெனில் {
        path.append(in_சாவி, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_சாவி_With_Fallback(மாறிலி UResourceBundle* resB,
                          மாறிலி எழுத்து* in_சாவி,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, வேர்_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    எனில் (status==nullptr || U_FAILURE(*status)) {
        திரும்பு fillIn;
    }
    எனில்(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    எனில்(URES_IS_TABLE(type)) {
        மாறிலி எழுத்து* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_சாவி_Path(&resB->getResData(), resB->fRes, in_சாவி);
        மாறிலி எழுத்து* சாவி = in_சாவி;
        தருக்க did_வேர்_Once = பொய்;
        எனில்(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            எழுத்து *myPath = nullptr;
            மாறிலி எழுத்து* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            போது(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_வேர்_Once)) { /* Otherwise, we'll look in parents */
                எனில் (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } இல்லையெனில் {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_வேர்_Once = உண்மை;
                }
                வேர்_Res = dataEntry->fData.வேர்_Res;

                எனில்(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_சாவி, path, status);
                    எனில் (U_FAILURE(*status)) {
                        ures_close(helper);
                        திரும்பு fillIn;
                    }
                    myPath = path.data();
                    சாவி = in_சாவி;
                    செய் {
                        res = res_findResource(&(dataEntry->fData), வேர்_Res, &myPath, &சாவி);
                        எனில் (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            எனில்(helper) {
                              dataEntry = helper->fData;
                              வேர்_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } இல்லையெனில் {
                              நிறுத்து;
                            }
                        } இல்லையெனில் எனில் (res == RES_BOGUS) {
                            நிறுத்து;
                        }
                    } போது(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            எனில்(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                எனில்(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_வேர்_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } இல்லையெனில் {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, சாவி, -1, resB, fillIn, status);
                எனில் (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_சாவி, path, status);
                } இல்லையெனில் {
                    மாறிலி எழுத்து* separator = nullptr;
                    எனில் (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    எனில் (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_சாவி, path, status);
                    } இல்லையெனில் {
                        createPath(origResPath, origResPathLen, "", 0, in_சாவி, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                எனில்(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } இல்லையெனில் {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } இல்லையெனில் {
            fillIn = init_resb_result(resB->fData, res, சாவி, -1, resB, fillIn, status);
        }
    } 
    இல்லையெனில் {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    திரும்பு fillIn;
}

நாமாகாஶ {

வெறுமை get_All_பொருட்கள்_With_Fallback(
        மாறிலி UResourceBundle *bundle, Resource_Data_மதிப்பு &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
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
    sink.put(bundle->f_சாவி, value, !hasParent, errorCode);
    எனில் (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        அடுக்கு_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_உச்சி_Level = உண்மை;
        parentRef.fRes = parentRef.getResData().வேர்_Res;
        parentRef.fSize = res_மொத்தம்_Array_பொருட்கள்(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        அடுக்கு_UResource_Bundle containerBundle;
        மாறிலி UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        எனில் (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } இல்லையெனில் {
            rb = ures_get_By_சாவி_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        எனில் (U_SUCCESS(pathErrorCode)) {
            get_All_பொருட்கள்_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

கட்டமைப்பு GetAllChildrenSink : பொது ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    மெய்நிகர் ~GetAllChildrenSink() override;
    மெய்நிகர் வெறுமை put(மாறிலி எழுத்து *சாவி, Resource_மதிப்பு &value, UBool is_வேர்,
           UErrorCode &errorCode) override {
        ResourceTable பொருட்கள்_Table = value.getTable(errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        க்கு (int32_t i = 0; பொருட்கள்_Table.get_சாவி_And_மதிப்பு(i, சாவி, value); ++i) {
            எனில் (value.getType() == URES_ALIAS) {
                Resource_Data_மதிப்பு& rdv = static_cast<Resource_Data_மதிப்பு&>(value);
                அடுக்கு_UResource_Bundle அடுக்கு_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          அடுக்கு_Temp_Bundle.getAlias(), &errorCode);
                எனில் (U_SUCCESS(errorCode)) {
                    Resource_Data_மதிப்பு aliased_மதிப்பு;
                    aliased_மதிப்பு.setData(aliasRB->getResData());
                    aliased_மதிப்பு.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_மதிப்பு.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    எனில் (aliased_மதிப்பு.getType() != URES_TABLE) {
                        dest.put(சாவி, aliased_மதிப்பு, is_வேர், errorCode);
                    } இல்லையெனில் {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_மதிப்பு_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        மாறிலி எழுத்து* parent_சாவி = சாவி; // dest.put() changes the key
                        dest.put(parent_சாவி, aliased_மதிப்பு, is_வேர், errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        போது (aliased_மதிப்பு_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            எழுத்து* localPathAsCharPtr = localPath.data();
                            மாறிலி எழுத்து* child_சாவி;
                            entry = entry->fParent;
                            res = entry->fData.வேர்_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_சாவி);
                            எனில் (newRes != RES_BOGUS) {
                                aliased_மதிப்பு.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_மதிப்பு.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_மதிப்பு_Type = aliased_மதிப்பு.getType();
                                எனில் (aliased_மதிப்பு_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_மதிப்பு& rdv2 = static_cast<Resource_Data_மதிப்பு&>(aliased_மதிப்பு);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             அடுக்கு_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_மதிப்பு.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_மதிப்பு.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_மதிப்பு_Type = aliased_மதிப்பு.getType();
                                }
                                எனில் (aliased_மதிப்பு_Type == URES_TABLE) {
                                    dest.put(parent_சாவி, aliased_மதிப்பு, is_வேர், errorCode);
                                } இல்லையெனில் {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    திரும்பு;
                                }
                            }
                        }
                    }
                }
            } இல்லையெனில் {
                dest.put(சாவி, value, is_வேர், errorCode);
            }
            எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI வெறுமை U_EXPORT2
ures_getAllChildrenWithFallback(மாறிலி UResourceBundle *bundle, மாறிலி எழுத்து *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_பொருட்கள்_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI வெறுமை U_EXPORT2
ures_get_மதிப்பு_With_Fallback(மாறிலி UResourceBundle *bundle, மாறிலி எழுத்து *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_மதிப்பு &value, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    எனில் (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    மாறிலி UResourceBundle *rb;
    எனில் (*path == 0) {
        // empty path
        rb = bundle;
    } இல்லையெனில் {
        rb = ures_get_By_சாவி_With_Fallback(bundle, path, tempFillIn, &errorCode);
        எனில் (U_FAILURE(errorCode)) {
            திரும்பு;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI வெறுமை U_EXPORT2
ures_get_All_பொருட்கள்_With_Fallback(மாறிலி UResourceBundle *bundle, மாறிலி எழுத்து *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    எனில் (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    அடுக்கு_UResource_Bundle அடுக்கு_Bundle;
    மாறிலி UResourceBundle *rb;
    எனில் (*path == 0) {
        // empty path
        rb = bundle;
    } இல்லையெனில் {
        rb = ures_get_By_சாவி_With_Fallback(bundle, path, அடுக்கு_Bundle.getAlias(), &errorCode);
        எனில் (U_FAILURE(errorCode)) {
            திரும்பு;
        }
    }
    // Get all table items with fallback.
    Resource_Data_மதிப்பு value;
    get_All_பொருட்கள்_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_சாவி(மாறிலி UResourceBundle* resB,
              மாறிலி எழுத்து* in_சாவி,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    மாறிலி எழுத்து *சாவி = in_சாவி;

    எனில் (status==nullptr || U_FAILURE(*status)) {
        திரும்பு fillIn;
    }
    எனில்(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    எனில்(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_சாவி(&resB->getResData(), resB->fRes, &t, &சாவி);
        எனில்(res == RES_BOGUS) {
            சாவி = in_சாவி;
            எனில்(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &சாவி, &res, status);
                எனில்(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    திரும்பு init_resb_result(dataEntry, res, சாவி, -1, resB, fillIn, status);
                } இல்லையெனில் {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } இல்லையெனில் {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } இல்லையெனில் {
            திரும்பு init_resb_result(resB->fData, res, சாவி, -1, resB, fillIn, status);
        }
    } 
#எனில் 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    இல்லையெனில் எனில்(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == உண்மை) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &சாவி, &res, status);
        எனில்(U_SUCCESS(*status)) {
            திரும்பு init_resb_result(dataEntry, res, சாவி, resB, fillIn, status);
        } இல்லையெனில் {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    இல்லையெனில் {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு fillIn;
}

U_CAPI மாறிலி char16_t* U_EXPORT2
ures_get_String_By_சாவி(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND,
                    மாறிலி எழுத்து* in_சாவி,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    மாறிலி எழுத்து* சாவி = in_சாவி;

    எனில் (status==nullptr || U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில்(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    எனில்(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_சாவி(&resB->getResData(), resB->fRes, &t, &சாவி);

        எனில்(res == RES_BOGUS) {
            சாவி = in_சாவி;
            எனில்(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &சாவி, &res, status);
                எனில்(U_SUCCESS(*status)) {
                    தேர்வு (RES_GET_TYPE(res)) {
                    நிலை URES_STRING:
                    நிலை URES_STRING_V2:
                        திரும்பு res_getString({resB, சாவி}, &dataEntry->fData, res, len);
                    நிலை URES_ALIAS:
                      {
                        மாறிலி char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_சாவி(resB, in_சாவி, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        திரும்பு result;
                      }
                    இயல்பு:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } இல்லையெனில் {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } இல்லையெனில் {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } இல்லையெனில் {
            தேர்வு (RES_GET_TYPE(res)) {
            நிலை URES_STRING:
            நிலை URES_STRING_V2:
                திரும்பு res_getString({resB, சாவி}, &resB->getResData(), res, len);
            நிலை URES_ALIAS:
              {
                மாறிலி char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_சாவி(resB, in_சாவி, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                திரும்பு result;
              }
            இயல்பு:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#எனில் 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    இல்லையெனில் எனில்(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == உண்மை) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &சாவி, &res, status);
        எனில்(U_SUCCESS(*status)) {
            // TODO: Tracing
            திரும்பு res_getString(rd, res, len);
        } இல்லையெனில் {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    இல்லையெனில் {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு nullptr;
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_get_UTF8_String_By_சாவி(மாறிலி UResourceBundle* resB,
                        மாறிலி எழுத்து* சாவி,
                        எழுத்து* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    மாறிலி char16_t *s16 = ures_get_String_By_சாவி(resB, சாவி, &length16, status);
    திரும்பு ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_getLocaleInternal(மாறிலி UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    எனில் (status==nullptr || U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில் (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    } இல்லையெனில் {
      திரும்பு resourceBundle->fData->fName;
    }
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_getLocale(மாறிலி UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  திரும்பு ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_getLocaleByType(மாறிலி UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    எனில் (status==nullptr || U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில் (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    } இல்லையெனில் {
        தேர்வு(type) {
        நிலை ULOC_ACTUAL_LOCALE:
            திரும்பு resourceBundle->fData->fName;
        நிலை ULOC_VALID_LOCALE:
            திரும்பு resourceBundle->fValidLocaleDataEntry->fName;
        நிலை ULOC_REQUESTED_LOCALE:
        இயல்பு:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு nullptr;
        }
    }
}

U_CFUNC மாறிலி எழுத்து*
ures_getName(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND) {
  எனில்(resB == nullptr) {
    திரும்பு nullptr;
  }

  திரும்பு resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC மாறிலி எழுத்து*
ures_getPath(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND) {
  எனில்(resB == nullptr) {
    திரும்பு nullptr;
  }

  திரும்பு resB->fData->fPath;
}
#endif

நிலையான UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  மாறிலி எழுத்து* path,
                  மாறிலி எழுத்து* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    UResourceDataEntry *entry;
    எனில்(openType != URES_OPEN_DIRECT) {
        எனில் (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        எனில்(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } இல்லையெனில் {
        entry = entryOpenDirect(path, localeID, status);
    }
    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில்(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        திரும்பு nullptr;
    }

    UBool is_அடுக்கு_Object;
    எனில்(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(அளவு(UResourceBundle)));
        எனில்(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு nullptr;
        }
        is_அடுக்கு_Object = பொய்;
    } இல்லையெனில் {  // fill-in
        is_அடுக்கு_Object = ures_is_அடுக்கு_Object(r);
        ures_closeBundle(r, பொய்);
    }
    uprv_memset(r, 0, அளவு(UResourceBundle));
    ures_set_Is_அடுக்கு_Object(r, is_அடுக்கு_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_உச்சி_Level = உண்மை;
    r->fRes = r->getResData().வேர்_Res;
    r->fSize = res_மொத்தம்_Array_பொருட்கள்(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    திரும்பு r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(மாறிலி எழுத்து* path, மாறிலி எழுத்து* localeID, UErrorCode* status) {
    திரும்பு ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_வேர், status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(மாறிலி எழுத்து* path, மாறிலி எழுத்து* localeID, UErrorCode* status) {
    திரும்பு ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_வேர், status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(மாறிலி எழுத்து* path, மாறிலி எழுத்து* localeID, UErrorCode* status) {
    திரும்பு ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI வெறுமை U_EXPORT2
ures_openFillIn(UResourceBundle *r, மாறிலி எழுத்து* path,
                மாறிலி எழுத்து* localeID, UErrorCode* status) {
    எனில்(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_வேர், status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI வெறுமை U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, மாறிலி எழுத்து* path, மாறிலி எழுத்து* localeID, UErrorCode* status) {
    எனில்(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_மொத்தம்_Array_பொருட்கள்(மாறிலி UResourceBundle* resourceBundle,
                  மாறிலி எழுத்து* resource_சாவி,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_அடுக்கு_Object(&resData);
    எனில் (status==nullptr || U_FAILURE(*status)) {
        திரும்பு 0;
    }
    எனில்(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    ures_get_By_சாவி(resourceBundle, resource_சாவி, &resData, status);
    
    எனில்(resData.getResData().data != nullptr) {
        int32_t result = res_மொத்தம்_Array_பொருட்கள்(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        திரும்பு result;
    } இல்லையெனில் {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        திரும்பு 0;
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
U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_get_Version_எண்_Internal(மாறிலி UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    எனில் (!resourceBundle) திரும்பு nullptr;

    எனில்(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        மாறிலி char16_t* minor_version = ures_get_String_By_சாவி(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (எழுத்து *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        எனில் (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            திரும்பு nullptr;
        }
       
        எனில்(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        இல்லையெனில் {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    திரும்பு resourceBundle->fVersion;
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_get_Version_எண்(மாறிலி UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    திரும்பு ures_get_Version_எண்_Internal(resourceBundle);
}

U_CAPI வெறுமை U_EXPORT2 ures_getVersion(மாறிலி UResourceBundle* resB, UVersionInfo versionInfo) {
    எனில் (!resB) திரும்பு;

    u_versionFromString(versionInfo, ures_get_Version_எண்_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#எனில் defined(URES_மரம்_DEBUG)
#அந்தர்பாவ <stdio.h>
#endif

வகைவரை கட்டமைப்பு ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

நிலையான வெறுமை U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

நிலையான int32_t U_CALLCONV
ures_loc_மொத்தம்_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    திரும்பு ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


நிலையான மாறிலி எழுத்து * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    மாறிலி எழுத்து *result = nullptr;
    int32_t len = 0;
    எனில் (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_சாவி(k);
        len = (int32_t)uprv_strlen(result);
    }
    எனில் (resultLength) {
        *resultLength = len;
    }
    திரும்பு result;
}

நிலையான வெறுமை U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

நிலையான மாறிலி UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_மொத்தம்_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(மாறிலி எழுத்து *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(அளவு(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(அளவு(UEnumeration));
    எனில்(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        திரும்பு nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, அளவு(UEnumeration));

    ures_init_அடுக்கு_Object(&myContext->installed);
    ures_init_அடுக்கு_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_சாவி(idx, INDEX_TAG, &myContext->installed, status);
    எனில்(U_SUCCESS(*status)) {
#எனில் defined(URES_மரம்_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_சாவி(&myContext->installed));
#endif
        en->context = myContext;
    } இல்லையெனில் {
#எனில் defined(URES_மரம்_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    திரும்பு en;
}

நிலையான UBool isLocaleInList(UEnumeration *locEnum, மாறிலி எழுத்து *loc_To_தேடல், UErrorCode *status) {
    மாறிலி எழுத்து *loc;
    போது ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        எனில் (uprv_strcmp(loc, loc_To_தேடல்) == 0) {
            திரும்பு உண்மை;
        }
    }
    திரும்பு பொய்;
}

நிலையான வெறுமை getParentForFunctionalEquivalent(மாறிலி எழுத்து*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    எனில் (res != nullptr) {
        ures_get_By_சாவி(res, "%%Parent", bund1, &subStatus);
        எனில் (U_SUCCESS(subStatus)) {
            int32_t length16;
            மாறிலி char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    எனில் (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(எழுத்து *result, int32_t resultCapacity,
                             மாறிலி எழுத்து *path, மாறிலி எழுத்து *resName, மாறிலி எழுத்து *keyword, மாறிலி எழுத்து *locid,
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
    எனில்(U_FAILURE(*status)) திரும்பு 0;
    CharString kwVal;
    எனில் (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_மதிப்பு(locid, keyword, subStatus);
        எனில் (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    எனில் (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#எனில் defined(URES_மரம்_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_அடுக்கு_Object(&bund1);
    ures_init_அடுக்கு_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    எனில்(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = உண்மை;
        எனில் (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    எனில்(U_FAILURE(subStatus)) {
        *status = subStatus;
        திரும்பு 0;
    }
    
    செய் {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        எனில்(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = பொய்;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#எனில் defined(URES_மரம்_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        எனில்(U_FAILURE(subStatus)) {
            *status = subStatus;
        } இல்லையெனில் எனில்(subStatus == U_ZERO_ERROR) {
            ures_get_By_சாவி(res,resName,&bund1, &subStatus);
            எனில்(subStatus == U_ZERO_ERROR) {
                மாறிலி char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#எனில் defined(URES_மரம்_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_சாவி(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                எனில்(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#எனில் defined(URES_மரம்_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    எனில்(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#எனில் defined(URES_மரம்_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        எனில் (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        எனில் (found != parent) {
            parent.copyFrom(found, subStatus);
        } இல்லையெனில் {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } போது(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    செய் {
        res = ures_open(path, parent.data(), &subStatus);
        எனில்((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = பொய்;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#எனில் defined(URES_மரம்_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        எனில்(U_FAILURE(subStatus)) {
            *status = subStatus;
        } இல்லையெனில் எனில்(subStatus == U_ZERO_ERROR) {
            ures_get_By_சாவி(res,resName,&bund1, &subStatus);
#எனில் defined(URES_மரம்_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            எனில்(subStatus == U_ZERO_ERROR) {
                ures_get_By_சாவி(&bund1, kwVal.data(), &bund2, &subStatus);
#எனில் defined(URES_மரம்_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                எனில்(subStatus == U_ZERO_ERROR) {
#எனில் defined(URES_மரம்_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    எனில் (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } இல்லையெனில் {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        எனில்(defLoc.length() > full.length()) {
                          மாறிலி char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#எனில் defined(URES_மரம்_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_சாவி(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          எனில்(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#எனில் defined(URES_மரம்_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#எனில் defined(URES_மரம்_DEBUG)
                        இல்லையெனில் {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } இல்லையெனில் {
#எனில் defined(URES_மரம்_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = பொய்;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        எனில் (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            மாறிலி எழுத்து *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            எனில் (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                எனில் (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = உண்மை;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        எனில் (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } போது(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    எனில்(full.isEmpty() && kwVal != defVal) {
#எனில் defined(URES_மரம்_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        செய் { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            எனில்((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = பொய்;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#எனில் defined(URES_மரம்_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            எனில்(U_FAILURE(subStatus)) {
                *status = subStatus;
            } இல்லையெனில் எனில்(subStatus == U_ZERO_ERROR) {
                ures_get_By_சாவி(res,resName,&bund1, &subStatus);
                எனில்(subStatus == U_ZERO_ERROR) {
                    ures_get_By_சாவி(&bund1, kwVal.data(), &bund2, &subStatus);
                    எனில்(subStatus == U_ZERO_ERROR) {
#எனில் defined(URES_மரம்_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        எனில் (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } இல்லையெனில் {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        எனில்(defLoc.length() > full.length()) {
                          மாறிலி char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#எனில் defined(URES_மரம்_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_சாவி(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          எனில்(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#எனில் defined(URES_மரம்_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#எனில் defined(URES_மரம்_DEBUG)
                        இல்லையெனில் {
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
        } போது(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    எனில்(U_SUCCESS(*status)) {
        எனில்(full.isEmpty()) {
#எனில் defined(URES_மரம்_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } இல்லையெனில் எனில்(omitDefault) {
#எனில் defined(URES_மரம்_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          எனில்(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            எனில்(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#எனில் defined(URES_மரம்_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        எனில்(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } இல்லையெனில் எனில்(!omitDefault) {
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

    எனில்(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        எனில்(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        எனில்(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } இல்லையெனில் {
        length = 0;
        result[0]=0;
    }
    திரும்பு u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_மதிப்புகள்(மாறிலி எழுத்து *path, மாறிலி எழுத்து *keyword, UErrorCode *status)
{
#define மதிப்புகள்_BUF_SIZE 2048
#define மதிப்புகள்_LIST_SIZE 512
    
    எழுத்து       மதிப்புகள்_Buf[மதிப்புகள்_BUF_SIZE];
    int32_t    மதிப்புகள்_Index = 0;
    மாறிலி எழுத்து *மதிப்புகள்_List[மதிப்புகள்_LIST_SIZE];
    int32_t    மதிப்புகள்_மொத்தம் = 0;
    
    மாறிலி எழுத்து *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_அடுக்கு_Object(&item);
    ures_init_அடுக்கு_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    எனில்(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        திரும்பு nullptr;
    }
    
    மதிப்புகள்_Buf[0]=0;
    மதிப்புகள்_Buf[1]=0;

    போது ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#எனில் defined(URES_மரம்_DEBUG)
        எனில்(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_சாவி(bund, keyword, &item, &subStatus);
        
        எனில்(!bund || U_FAILURE(subStatus)) {
#எனில் defined(URES_மரம்_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            தொடர்;
        }

        போது ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            மாறிலி எழுத்து *k;
            int32_t i;
            k = ures_get_சாவி(subPtr);

#எனில் defined(URES_மரம்_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            எனில்(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                தொடர்;
            }
            க்கு(i=0; i<மதிப்புகள்_மொத்தம்; i++) {
                எனில்(!uprv_strcmp(மதிப்புகள்_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    நிறுத்து;
                }
            }
            எனில்(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                எனில்((மதிப்புகள்_மொத்தம் >= (மதிப்புகள்_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((மதிப்புகள்_Index+kLen+1+1) >= மதிப்புகள்_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } இல்லையெனில் {
                    uprv_strcpy(மதிப்புகள்_Buf+மதிப்புகள்_Index, k);
                    மதிப்புகள்_List[மதிப்புகள்_மொத்தம்++] = மதிப்புகள்_Buf+மதிப்புகள்_Index;
                    மதிப்புகள்_Index += kLen;
#எனில் defined(URES_மரம்_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    மதிப்புகள்_Buf[மதிப்புகள்_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    மதிப்புகள்_Buf[மதிப்புகள்_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#எனில் defined(URES_மரம்_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        மதிப்புகள்_Index, மதிப்புகள்_மொத்தம்);
#endif
    திரும்பு uloc_openKeywordList(மதிப்புகள்_Buf, மதிப்புகள்_Index, status);
}
#எனில் 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(மாறிலி UResourceBundle* res1, மாறிலி UResourceBundle* res2){
    எனில்(res1==nullptr || res2==nullptr){
        திரும்பு res1==res2; /* pointer comparison */
    }
    எனில்(res1->f_சாவி==nullptr||  res2->f_சாவி==nullptr){
        திரும்பு (res1->f_சாவி==res2->f_சாவி);
    }இல்லையெனில்{
        எனில்(uprv_strcmp(res1->f_சாவி, res2->f_சாவி)!=0){
            திரும்பு பொய்;
        }
    }
    எனில்(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        திரும்பு பொய்;
    }
    எனில்(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        திரும்பு (res1->fData->fPath == res2->fData->fPath);
    }இல்லையெனில்{
        எனில்(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            திரும்பு பொய்;
        }
    }
    எனில்(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        திரும்பு பொய்;
    }
    எனில்(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        திரும்பு பொய்;
    }
    எனில்(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        திரும்பு பொய்;
    }
    எனில்(res1->fRes != res2->fRes){
        திரும்பு பொய்;
    }
    திரும்பு உண்மை;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(மாறிலி UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    எனில்(U_FAILURE(*status) || res == nullptr){
        திரும்பு nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    எனில்(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }இல்லையெனில்{
        ret = bundle;
    }
    திரும்பு ret;
}
U_CAPI மாறிலி UResourceBundle* U_EXPORT2
ures_getParentBundle(மாறிலி UResourceBundle* res){
    எனில்(res==nullptr){
        திரும்பு nullptr;
    }
    திரும்பு res->fParentRes;
}
#endif

U_CAPI வெறுமை U_EXPORT2
ures_get_Version_By_சாவி(மாறிலி UResourceBundle* res, மாறிலி எழுத்து *சாவி, UVersionInfo ver, UErrorCode *status) {
  மாறிலி char16_t *str;
  int32_t len;
  str = ures_get_String_By_சாவி(res, சாவி, &len, status);
  எனில்(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
