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

#شامل "unicode/ures.h"
#شامل "unicode/ustring.h"
#شامل "unicode/ucnv.h"
#شامل "bytesinkutil.h"
#شامل "charstr.h"
#شامل "uresimp.h"
#شامل "ustr_imp.h"
#شامل "cwchar.h"
#شامل "ucln_cmn.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "uhash.h"
#شامل "unicode/uenum.h"
#شامل "uenumimp.h"
#شامل "ulocimp.h"
#شامل "umutex.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل "uresdata.h"

using فضاینام icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ایستا UHashtable *cache = nullptr;
ایستا icu::UInitOnce gCacheInitOnce {};

ایستا UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ایستا int32_t U_CALLCONV hashEntry(ثابت UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    بازگشت uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ایستا UBool U_CALLCONV compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    بازگشت uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ایستا UBool chopLocale(نویسه *name) {
    نویسه *i = uprv_strrchr(name, '_');

    اگر(i != nullptr) {
        *i = '\0';
        بازگشت درست;
    }

    بازگشت نادرست;
}

ایستا UBool hasVariant(ثابت نویسه* localeID) {
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
    بازگشت sink.عدد_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#شامل "localefallback_data.h"

ایستا ثابت نویسه* performFallbackLookup(ثابت نویسه* کلید,
                                         ثابت نویسه* کلید_Strs,
                                         ثابت نویسه* مقدار_Strs U_LIFETIME_BOUND,
                                         ثابت int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    ثابت int32_t* bottom = lookupTable;
    ثابت int32_t* بالاترین = lookupTable + lookupTableLength;

    وقتی (bottom < بالاترین) {
        // Effectively, divide by 2 and round down to an even index
        ثابت int32_t* میانه = bottom + (((بالاترین - bottom) / 4) * 2);
        ثابت نویسه* entry_کلید = &(کلید_Strs[*میانه]);
        int32_t strcmpResult = uprv_strcmp(کلید, entry_کلید);
        اگر (strcmpResult == 0) {
            بازگشت &(مقدار_Strs[میانه[1]]);
        } وگرنه اگر (strcmpResult < 0) {
            بالاترین = میانه;
        } وگرنه {
            bottom = میانه + 2;
        }
    }
    بازگشت nullptr;
}

ایستا CharString getDefaultScript(ثابت CharString& language, ثابت CharString& region) {
    ثابت نویسه* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    اگر (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        اگر (U_FAILURE(err)) {
            بازگشت result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    اگر (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    اگر (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    بازگشت result;
}

شمارش UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_ریشه,
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
    URES_OPEN_LOCALE_ریشه,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
تعریفنوع شمارش UResOpenType UResOpenType;

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
ایستا منطقی getParentLocaleID(نویسه *name, ثابت نویسه *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    اگر (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        بازگشت chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    اگر (U_FAILURE(err)) {
        // hopefully this never happens...
        بازگشت chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    اگر (openType == URES_OPEN_LOCALE_DEFAULT_ریشه) {
        ثابت نویسه* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        اگر (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            بازگشت درست;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    اگر (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        اگر (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } وگرنه {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } وگرنه اگر (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        اگر (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } وگرنه {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } وگرنه اگر (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        اگر (openType != URES_OPEN_LOCALE_DEFAULT_ریشه || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } وگرنه {
            بازگشت نادرست;
        }
    } وگرنه {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        بازگشت نادرست;
    }
    اگر (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        بازگشت درست;
    } وگرنه {
        بازگشت نادرست;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ایستا UBool mayHaveParent(نویسه *name) {
    بازگشت (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
ایستا تهی entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_شمار_Existing++;
    وقتی(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_شمار_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ایستا UResourceDataEntry *getFallbackData(
        ثابت UResourceBundle *resBundle,
        ثابت نویسه **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    اگر(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        بازگشت nullptr;
    }
    اگر(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_کلید(&(dataEntry->fData), dataEntry->fData.ریشه_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    اگر(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        وقتی(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            اگر(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_کلید(&(dataEntry->fData), dataEntry->fData.ریشه_Res, &indexR, resTag);
            }
        }
    }

    اگر(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        بازگشت nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    اگر(i>1) {
        اگر(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_ریشه_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } وگرنه {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    بازگشت dataEntry;
}

ایستا تهی
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    اگر(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    اگر(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    اگر(entry->fPool != nullptr) {
        --entry->fPool->f_شمار_Existing;
    }
    alias = entry->fAlias;
    اگر(alias != nullptr) {
        وقتی(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_شمار_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
ایستا int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_رقم = 0;
    ثابت UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    اگر (cache == nullptr) {
        بازگشت 0;
    }

    انجامبده {
        deletedMore = نادرست;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        وقتی ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            اگر (resB->f_شمار_Existing == 0) {
                rb_Deleted_رقم++;
                deletedMore = درست;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } وقتی(deletedMore);

    بازگشت rb_Deleted_رقم;
}

#ifdef URES_DEBUG
#شامل <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = نادرست;
  int32_t pos = UHASH_FIRST;
  ثابت UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    اگر (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      بازگشت نادرست;
    }

    وقتی ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=درست;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (تهی*)resB, resB->f_شمار_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (تهی*)resB->fPool,
              (تهی*)resB->fAlias,
              (تهی*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_شمار(cache));
    بازگشت cacheNotEmpty;
}

#endif

ایستا UBool U_CALLCONV ures_cleanup()
{
    اگر (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    بازگشت درست;
}

/** INTERNAL: Initializes the cache for resources */
ایستا تهی U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ایستا تهی initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ایستا تهی setEntryName(UResourceDataEntry *res, ثابت نویسه *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    اگر(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    اگر (len < static_cast<int32_t>(اندازه(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    وگرنه {
        res->fName = static_cast<نویسه*>(uprv_malloc(len + 1));
    }
    اگر(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } وگرنه {
        uprv_strcpy(res->fName, name);
    }
}

ایستا UResourceDataEntry *
getPoolEntry(ثابت نویسه *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ایستا UResourceDataEntry *init_entry(ثابت نویسه *localeID, ثابت نویسه *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    ثابت نویسه *name;
    نویسه aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    /* here we try to deduce the right locale name */
    اگر(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } وگرنه اگر(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_ریشه_Locale_Name;
    } وگرنه { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<نویسه*>(name);
    find.fPath = const_cast<نویسه*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    اگر(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(اندازه(UResourceDataEntry)));
        اگر(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت nullptr;
        }

        uprv_memset(r, 0, اندازه(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        اگر (U_FAILURE(*status)) {
            uprv_free(r);
            بازگشت nullptr;
        }

        اگر(path != nullptr) {
            r->fPath = uprv_strdup(path);
            اگر(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                بازگشت nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        اگر (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            اگر (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                بازگشت nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } وگرنه { /* if we have a regular entry */
            Resource aliasres;
            اگر (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                اگر (U_SUCCESS(*status)) {
                    ثابت int32_t *poolIndexes = r->fPool->fData.p_ریشه + 1;
                    اگر(r->fData.p_ریشه[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_کلیدها = reinterpret_cast<ثابت نویسه*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } وگرنه {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } وگرنه {
                    r->fBogus = *status;
                }
            }
            اگر (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                اگر (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ثابت char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    اگر(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            اگر ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (تهی *)r, r, &cacheStatus);
                اگر (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } وگرنه {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    اگر(r != nullptr) {
        /* return the real bundle */
        وقتی(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_شمار_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        اگر(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    بازگشت r;
}

ایستا UResourceDataEntry *
getPoolEntry(ثابت نویسه *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    اگر( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    بازگشت poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ایستا UResourceDataEntry *
findFirstExisting(ثابت نویسه* path, نویسه* name, ثابت نویسه* defaultLocale, UResOpenType openType,
                  UBool *is_ریشه, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = نادرست;
    *foundParent = درست; /* we're starting with a fresh name */
    نویسه origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    وقتی(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        اگر (U_FAILURE(*status)) {
            بازگشت nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        اگر(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_شمار_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } وگرنه {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_ریشه = static_cast<UBool>(uprv_strcmp(name, k_ریشه_Locale_Name) == 0);

        /*Fallback data stuff*/
        اگر (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } وگرنه {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        اگر (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    بازگشت r;
}

ایستا تهی ures_set_Is_پشته_Object( UResourceBundle* resB, UBool state) {
    اگر(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } وگرنه {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ایستا UBool ures_is_پشته_Object(ثابت UResourceBundle* resB) {
  بازگشت((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?نادرست:درست);
}


U_CFUNC تهی ures_init_پشته_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, اندازه(UResourceBundle));
  ures_set_Is_پشته_Object(resB, درست);
}

U_NAMESPACE_BEGIN

پشته_UResource_Bundle::پشته_UResource_Bundle() {
    ures_init_پشته_Object(&bundle);
}

پشته_UResource_Bundle::~پشته_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

ایستا UBool  // returns U_SUCCESS(*status)
load_Parents_Except_ریشه(UResourceDataEntry *&t1,
                      نویسه name[], int32_t nameCapacity,
                      UBool usingUSRData, نویسه usrDataPath[], UErrorCode *status) {
    اگر (U_FAILURE(*status)) { بازگشت نادرست; }
    UBool checkParent = درست;
    وقتی (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        اگر (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            ثابت char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            اگر(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                اگر (uprv_strcmp(name, k_ریشه_Locale_Name) == 0) {
                    بازگشت درست;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        اگر (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            بازگشت نادرست;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        اگر (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            اگر (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                بازگشت نادرست;
            }
        }

        اگر (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } وگرنه {
            t1->fParent = t2;
            اگر (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_شمار_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    بازگشت درست;
}

ایستا UBool  // returns U_SUCCESS(*status)
درج_ریشه_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    اگر (U_FAILURE(*status)) { بازگشت نادرست; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_ریشه_Locale_Name, t1->fPath, &parentStatus);
    اگر (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        بازگشت نادرست;
    }
    t1->fParent = t2;
    t1 = t2;
    بازگشت درست;
}

ایستا UResourceDataEntry *entryOpen(ثابت نویسه* path, ثابت نویسه* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = نادرست;
    UBool is_ریشه = نادرست;
    UBool hasRealData = نادرست;
    UBool hasChopped = درست;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    نویسه name[ULOC_FULLNAME_CAPACITY];
    نویسه usrDataPath[96];

    initCache(status);

    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    uprv_strncpy(name, localeID, اندازه(name) - 1);
    name[اندازه(name) - 1] = 0;

    اگر ( usingUSRData ) {
        اگر ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } وگرنه {
            uprv_strncpy(usrDataPath, path, اندازه(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[اندازه(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ثابت نویسه *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_ریشه, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    اگر (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        برو finish;
    }

    اگر(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = درست;
        اگر ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            اگر (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                برو finish;
            }
            اگر ( u1 != nullptr ) {
                اگر(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } وگرنه {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_شمار_Existing = 0;
                }
            }
        }
        اگر ((hasChopped || mayHaveParent(name)) && !is_ریشه) {
            اگر (!load_Parents_Except_ریشه(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                برو finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    اگر(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_ریشه && !isDefault && !is_ریشه) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_ریشه, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        اگر (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            برو finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        اگر(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = درست;
            isDefault = درست;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            اگر ((hasChopped || mayHaveParent(name)) && !is_ریشه) {
                اگر (!load_Parents_Except_ریشه(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    برو finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    اگر(r == nullptr) {
        uprv_strcpy(name, k_ریشه_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_ریشه, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        اگر (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            برو finish;
        }
        اگر(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = درست;
        } وگرنه { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            برو finish;
        }
    } وگرنه اگر(!is_ریشه && uprv_strcmp(t1->fName, k_ریشه_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        اگر (!درج_ریشه_Bundle(t1, status)) {
            برو finish;
        }
        اگر(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    وقتی(r != nullptr && !is_ریشه && t1->fParent != nullptr) {
        t1->fParent->f_شمار_Existing++;
        t1 = t1->fParent;
    }

finish:
    اگر(U_SUCCESS(*status)) {
        اگر(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        بازگشت r;
    } وگرنه {
        بازگشت nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ایستا UResourceDataEntry *
entryOpenDirect(ثابت نویسه* path, ثابت نویسه* localeID, UErrorCode* status) {
    initCache(status);
    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    اگر (localeID == nullptr) {
        localeID = uloc_getDefault();
    } وگرنه اگر (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_ریشه_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    اگر(U_SUCCESS(*status)) {
        اگر(r->fBogus != U_ZERO_ERROR) {
            r->f_شمار_Existing--;
            r = nullptr;
        }
    } وگرنه {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    اگر(r != nullptr && uprv_strcmp(localeID, k_ریشه_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        نویسه name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        اگر(!chopLocale(name) || uprv_strcmp(name, k_ریشه_Locale_Name) == 0 ||
                load_Parents_Except_ریشه(t1, name, UPRV_LENGTHOF(name), نادرست, nullptr, status)) {
            اگر(uprv_strcmp(t1->fName, k_ریشه_Locale_Name) != 0 && t1->fParent == nullptr) {
                درج_ریشه_Bundle(t1, status);
            }
        }
        اگر(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    اگر(r != nullptr) {
        // TODO: Does this ever loop?
        وقتی(t1->fParent != nullptr) {
            t1->fParent->f_شمار_Existing++;
            t1 = t1->fParent;
        }
    }
    بازگشت r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ایستا تهی entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    وقتی(resB != nullptr) {
        p = resB->fParent;
        resB->f_شمار_Existing--;

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

ایستا تهی entryClose(UResourceDataEntry *resB) {
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
ایستا تهی ures_appendResPath(UResourceBundle *resB, ثابت نویسه* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    اگر(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    اگر(RES_BUFSIZE <= resB->fResPathLen+1) {
        اگر(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<نویسه*>(uprv_malloc((resB->fResPathLen + 1) * اندازه(نویسه)));
            /* Check that memory was allocated correctly. */
            اگر (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } وگرنه {
            نویسه* temp = static_cast<نویسه*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * اندازه(نویسه)));
            /* Check that memory was reallocated correctly. */
            اگر (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

ایستا تهی ures_freeResPath(UResourceBundle *resB) {
    اگر (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

ایستا تهی
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    اگر(resB != nullptr) {
        اگر(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        اگر(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        اگر(ures_is_پشته_Object(resB) == نادرست && freeBundleObj) {
            uprv_free(resB);
        }
#اگر 0 /*U_DEBUG*/
        وگرنه {
            /* poison the data */
            uprv_memset(resB, -1, اندازه(UResourceBundle));
        }
#endif
    }
}

U_CAPI تهی  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, درست);
}

فضاینام {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ثابت نویسه* کلید,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ثابت نویسه* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(ثابت ResourceData& resData,
                               Resource r,
                               ثابت نویسه* کلید,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               ثابت نویسه* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    اگر (U_FAILURE(*status)) { بازگشت resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ثابت char16_t *alias = res_getAlias(&resData, r, &len);
    اگر(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت resB;
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
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    // We have an alias, now let's cut it up.
    ثابت نویسه *path = nullptr, *locale = nullptr, *کلید_Path = nullptr;
    اگر(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        نویسه *chAliasData = chAlias.data();
        نویسه *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        اگر(sep != nullptr) {
            *sep++ = 0;
        }
        اگر(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            کلید_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } وگرنه {
            اگر(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            اگر (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } وگرنه {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                اگر(sep != nullptr) {
                    *sep++ = 0;
                }
                کلید_Path = sep;
            }
        }
    } وگرنه {
        // No path, start with a locale.
        نویسه *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        اگر(sep != nullptr) {
            *sep++ = 0;
        }
        کلید_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    اگر (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } وگرنه {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        اگر(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            بازگشت resB;
        }
        dataEntry = mainRes->fData;
    }

    ثابت نویسه* temp = nullptr;
    اگر(کلید_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.ریشه_Res;
        اگر(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            اگر (U_FAILURE(*status)) {
                بازگشت nullptr;
            }
            نویسه *a_کلید = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_کلید, &temp);
        }
        اگر(کلید) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(کلید, *status);
            اگر (U_FAILURE(*status)) {
                بازگشت nullptr;
            }
            نویسه *a_کلید = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_کلید, &temp);
        } وگرنه اگر(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            اگر(URES_IS_TABLE(type)) {
                ثابت نویسه *a_کلید;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_کلید);
            } وگرنه { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        اگر(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } وگرنه {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } وگرنه {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(کلید_Path, *status);
        اگر (U_FAILURE(*status)) {
            بازگشت nullptr;
        }
        نویسه *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        برای(;;) {
            r = dataEntry->fData.ریشه_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            وقتی(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                اگر(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    بشکن;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                اگر (U_FAILURE(*status)) {
                    بشکن;
                }
                اگر (temp == nullptr || uprv_strcmp(کلید_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, کلید_Path, static_cast<int32_t>(uprv_strlen(کلید_Path)), status);
                    اگر(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    اگر (U_FAILURE(*status)) {
                        بشکن;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            اگر (U_FAILURE(*status) || r != RES_BOGUS) {
                بشکن;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            اگر (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                بشکن;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, کلید_Path);
        }
    }
    اگر(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    بازگشت resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ثابت نویسه* کلید,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ثابت نویسه* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    اگر(status == nullptr || U_FAILURE(*status)) {
        بازگشت resB;
    }
    اگر (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    اگر(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        اگر(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            بازگشت resB;
        }
        بازگشت getAliasTargetAsResourceBundle(
            dataEntry->fData, r, کلید, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    اگر(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(اندازه(UResourceBundle)));
        اگر (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت nullptr;
        }
        ures_set_Is_پشته_Object(resB, نادرست);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } وگرنه {
        اگر(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        اگر(resB->fVersion != nullptr) {
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
        اگر(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = نادرست;
    resB->f_Is_بالاترین_Level = نادرست;
    resB->fIndex = -1;
    resB->f_کلید = کلید; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    اگر(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    اگر(کلید != nullptr) {
        ures_appendResPath(resB, کلید, static_cast<int32_t>(uprv_strlen(کلید)), status);
        اگر(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } وگرنه اگر(idx >= 0) {
        نویسه buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        اگر(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, اندازه(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_شمار_Array_اقلام(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    بازگشت resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ثابت نویسه* کلید,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ثابت UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    بازگشت init_resb_result(
        dataEntry, r, کلید, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ثابت UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_پشته_Object;
    اگر(U_FAILURE(*status) || r == original) {
        بازگشت r;
    }
    اگر(original != nullptr) {
        اگر(r == nullptr) {
            is_پشته_Object = نادرست;
            r = static_cast<UResourceBundle*>(uprv_malloc(اندازه(UResourceBundle)));
            /* test for nullptr */
            اگر (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                بازگشت nullptr;
            }
        } وگرنه {
            is_پشته_Object = ures_is_پشته_Object(r);
            ures_closeBundle(r, نادرست);
        }
        uprv_memcpy(r, original, اندازه(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        اگر(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_پشته_Object(r, is_پشته_Object);
        اگر(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    بازگشت r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ثابت char16_t* U_EXPORT2 ures_getString(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    ثابت char16_t *s;
    اگر (status==nullptr || U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    اگر (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت s;
}

ایستا ثابت نویسه*
ures_toUTF8String(ثابت char16_t* s16,
                  int32_t length16,
                  نویسه* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر (pLength != nullptr) {
        capacity = *pLength;
    } وگرنه {
        capacity = 0;
    }
    اگر (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر (length16 == 0) {
        /* empty string, return as read-only pointer */
        اگر (pLength != nullptr) {
            *pLength = 0;
        }
        اگر (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            بازگشت dest;
        } وگرنه {
            بازگشت "";
        }
    } وگرنه {
        /* We need to transform the string to the destination buffer. */
        اگر (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            بازگشت u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        اگر (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            اگر (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        بازگشت u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI ثابت نویسه* U_EXPORT2
ures_getUTF8String(ثابت UResourceBundle* resB,
                   نویسه* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getString(resB, &length16, status);
    بازگشت ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI ثابت uint8_t* U_EXPORT2
ures_get_دودویی(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ثابت uint8_t *p;
  اگر (status==nullptr || U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  اگر(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت nullptr;
  }
  p = res_get_دودویی({resB}, &resB->getResData(), resB->fRes, len);
  اگر (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  بازگشت p;
}

U_CAPI ثابت int32_t* U_EXPORT2
ures_getIntVector(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ثابت int32_t *p;
  اگر (status==nullptr || U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  اگر(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  اگر (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  بازگشت p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ثابت UResourceBundle* resB, UErrorCode *status) {
  اگر (status==nullptr || U_FAILURE(*status)) {
    بازگشت 0xffffffff;
  }
  اگر(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0xffffffff;
  }
  اگر(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    بازگشت 0xffffffff;
  }
  بازگشت res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ثابت UResourceBundle* resB, UErrorCode *status) {
  اگر (status==nullptr || U_FAILURE(*status)) {
    بازگشت 0xffffffff;
  }
  اگر(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0xffffffff;
  }
  اگر(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    بازگشت 0xffffffff;
  }
  بازگشت res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ثابت UResourceBundle *resB) {
  اگر(resB == nullptr) {
    بازگشت URES_NONE;
  }
  بازگشت res_getPublicType(resB->fRes);
}

U_CAPI ثابت نویسه* U_EXPORT2
ures_get_کلید(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  اگر(resB == nullptr) {
    بازگشت nullptr;
  }
  بازگشت(resB->f_کلید);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(ثابت UResourceBundle *resB) {
  اگر(resB == nullptr) {
    بازگشت 0;
  }
  
  بازگشت resB->fSize;
}

ایستا ثابت char16_t*
ures_getStringWithAlias(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  اگر(RES_GET_TYPE(r) == URES_ALIAS) {
    ثابت char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    بازگشت result;
  } وگرنه {
    بازگشت res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI تهی U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  اگر(resB == nullptr) {
    بازگشت;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(ثابت UResourceBundle *resB) {
  اگر(resB == nullptr) {
    بازگشت نادرست;
  }
  بازگشت resB->fIndex < resB->fSize-1;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ثابت نویسه** کلید,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  اگر (status==nullptr || U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  اگر(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت nullptr;
  }
  
  اگر(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } وگرنه {
    resB->fIndex++;
    انتخاب(RES_GET_TYPE(resB->fRes)) {
    حالت URES_STRING:
    حالت URES_STRING_V2:
      بازگشت res_getString({resB}, &resB->getResData(), resB->fRes, len);
    حالت URES_TABLE:
    حالت URES_TABLE16:
    حالت URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, کلید);
      اگر(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      بازگشت ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    حالت URES_ARRAY:
    حالت URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      اگر(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      بازگشت ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    حالت URES_ALIAS:
      بازگشت ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    حالت URES_INT:
    حالت URES_دودویی:
    حالت URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    پیشفرض:
      بازگشت nullptr;
    }
  }

  بازگشت nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    ثابت نویسه *کلید = nullptr;
    Resource r = RES_BOGUS;

    اگر (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            بازگشت fillIn;
    }
    اگر(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            بازگشت fillIn;
    }

    اگر(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } وگرنه {
        resB->fIndex++;
        انتخاب(RES_GET_TYPE(resB->fRes)) {
        حالت URES_INT:
        حالت URES_دودویی:
        حالت URES_STRING:
        حالت URES_STRING_V2:
        حالت URES_INT_VECTOR:
            بازگشت ures_copyResb(fillIn, resB, status);
        حالت URES_TABLE:
        حالت URES_TABLE16:
        حالت URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &کلید);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            بازگشت init_resb_result(resB->fData, r, کلید, resB->fIndex, resB, fillIn, status);
        حالت URES_ARRAY:
        حالت URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            بازگشت init_resb_result(resB->fData, r, کلید, resB->fIndex, resB, fillIn, status);
        پیشفرض:
            /*return nullptr;*/
            بازگشت fillIn;
        }
    }
    /*return nullptr;*/
    بازگشت fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(ثابت UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    ثابت نویسه* کلید = nullptr;
    Resource r = RES_BOGUS;

    اگر (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        بازگشت fillIn;
    }
    اگر(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        بازگشت fillIn;
    }

    اگر(indexR >= 0 && resB->fSize > indexR) {
        انتخاب(RES_GET_TYPE(resB->fRes)) {
        حالت URES_INT:
        حالت URES_دودویی:
        حالت URES_STRING:
        حالت URES_STRING_V2:
        حالت URES_INT_VECTOR:
            بازگشت ures_copyResb(fillIn, resB, status);
        حالت URES_TABLE:
        حالت URES_TABLE16:
        حالت URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &کلید);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            بازگشت init_resb_result(resB->fData, r, کلید, indexR, resB, fillIn, status);
        حالت URES_ARRAY:
        حالت URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            بازگشت init_resb_result(resB->fData, r, کلید, indexR, resB, fillIn, status);
        پیشفرض:
            /*return nullptr;*/
            بازگشت fillIn;
        }
    } وگرنه {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    بازگشت fillIn;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_getStringByIndex(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    ثابت نویسه* کلید = nullptr;
    Resource r = RES_BOGUS;

    اگر (status==nullptr || U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر(indexS >= 0 && resB->fSize > indexS) {
        انتخاب(RES_GET_TYPE(resB->fRes)) {
        حالت URES_STRING:
        حالت URES_STRING_V2:
            بازگشت res_getString({resB}, &resB->getResData(), resB->fRes, len);
        حالت URES_TABLE:
        حالت URES_TABLE16:
        حالت URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &کلید);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            بازگشت ures_getStringWithAlias(resB, r, indexS, len, status);
        حالت URES_ARRAY:
        حالت URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            بازگشت ures_getStringWithAlias(resB, r, indexS, len, status);
        حالت URES_ALIAS:
            بازگشت ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        حالت URES_INT:
        حالت URES_دودویی:
        حالت URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            بشکن;
        پیشفرض:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          بشکن;
        }
    } وگرنه {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    بازگشت nullptr;
}

U_CAPI ثابت نویسه* U_EXPORT2
ures_getUTF8StringByIndex(ثابت UResourceBundle* resB,
                          int32_t idx,
                          نویسه* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    بازگشت ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(ثابت نویسه* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  نویسه *packageName = nullptr;
  نویسه *pathToResource = nullptr, *save = nullptr;
  نویسه *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  اگر(status == nullptr || U_FAILURE(*status)) {
    بازگشت result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (نویسه *)uprv_malloc(length*اندازه(نویسه));
  /* test for nullptr */
  اگر(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  اگر(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    اگر(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } وگرنه {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  اگر(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  اگر(U_SUCCESS(*status)) {
    اگر(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } وگرنه {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  بازگشت result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(ثابت UResourceBundle* resB,
                     نویسه* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  ثابت نویسه *کلید;

  اگر(status == nullptr || U_FAILURE(*status)) {
    بازگشت result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  انجامبده {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &کلید); 
    اگر(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, کلید, -1, resB, fillIn, status);
        resB = result;
    } وگرنه {
        *status = U_MISSING_RESOURCE_ERROR;
        بشکن;
    }
  } وقتی(*path); /* there is more stuff in the path */

  بازگشت result;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلید_With_Fallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت نویسه* in_کلید,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle پشته_2;
    ثابت char16_t* retVal = nullptr;
    ures_init_پشته_Object(&پشته_2);
    ures_get_By_کلید_With_Fallback(resB, in_کلید, &پشته_2, status);
    int32_t length;
    retVal = ures_getString(&پشته_2, &length, status);
    ures_close(&پشته_2);
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    اگر (len != nullptr) {
        *len = length;
    }
    بازگشت retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ایستا Resource get_Table_Item_By_کلید_Path(ثابت ResourceData *pResData, Resource table, ثابت نویسه *کلید) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(کلید, errorCode);
  اگر (U_FAILURE(errorCode)) { بازگشت RES_BOGUS; }
  نویسه *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  وقتی (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    نویسه *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    اگر (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } وگرنه {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    ثابت نویسه *pathP = pathPart;
    resource = res_get_Table_Item_By_کلید(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  اگر (*pathPart) {
    بازگشت RES_BOGUS;
  }
  بازگشت resource;
}

ایستا تهی createPath(ثابت نویسه* origResPath,
                       int32_t     origResPathLen,
                       ثابت نویسه* resPath,
                       int32_t     resPathLen,
                       ثابت نویسه* in_کلید,
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
    ثابت نویسه* کلید = in_کلید;
    اگر (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        اگر (U_SUCCESS(*status)) {
            ثابت نویسه* resPathLimit = resPath + resPathLen;
            ثابت نویسه* origResPathLimit = origResPath + origResPathLen;
            ثابت نویسه* resPathPtr = resPath;
            ثابت نویسه* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            وقتی (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                وقتی (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                اگر (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                وقتی (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                اگر (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            وقتی (resPathPtr < resPathLimit && *کلید != '\0') {
                وقتی (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                اگر (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                وقتی (*کلید != '\0' && *کلید != RES_PATH_SEPARATOR) {
                    ++کلید;
                }
                اگر (*کلید == RES_PATH_SEPARATOR) {
                    ++کلید;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(کلید, *status);
    } وگرنه {
        path.append(in_کلید, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلید_With_Fallback(ثابت UResourceBundle* resB,
                          ثابت نویسه* in_کلید,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, ریشه_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    اگر (status==nullptr || U_FAILURE(*status)) {
        بازگشت fillIn;
    }
    اگر(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    اگر(URES_IS_TABLE(type)) {
        ثابت نویسه* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_کلید_Path(&resB->getResData(), resB->fRes, in_کلید);
        ثابت نویسه* کلید = in_کلید;
        منطقی did_ریشه_Once = نادرست;
        اگر(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            نویسه *myPath = nullptr;
            ثابت نویسه* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            وقتی(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_ریشه_Once)) { /* Otherwise, we'll look in parents */
                اگر (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } وگرنه {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_ریشه_Once = درست;
                }
                ریشه_Res = dataEntry->fData.ریشه_Res;

                اگر(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_کلید, path, status);
                    اگر (U_FAILURE(*status)) {
                        ures_close(helper);
                        بازگشت fillIn;
                    }
                    myPath = path.data();
                    کلید = in_کلید;
                    انجامبده {
                        res = res_findResource(&(dataEntry->fData), ریشه_Res, &myPath, &کلید);
                        اگر (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            اگر(helper) {
                              dataEntry = helper->fData;
                              ریشه_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } وگرنه {
                              بشکن;
                            }
                        } وگرنه اگر (res == RES_BOGUS) {
                            بشکن;
                        }
                    } وقتی(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            اگر(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                اگر(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_ریشه_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } وگرنه {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, کلید, -1, resB, fillIn, status);
                اگر (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_کلید, path, status);
                } وگرنه {
                    ثابت نویسه* separator = nullptr;
                    اگر (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    اگر (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_کلید, path, status);
                    } وگرنه {
                        createPath(origResPath, origResPathLen, "", 0, in_کلید, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                اگر(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } وگرنه {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } وگرنه {
            fillIn = init_resb_result(resB->fData, res, کلید, -1, resB, fillIn, status);
        }
    } 
    وگرنه {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    بازگشت fillIn;
}

فضاینام {

تهی get_All_اقلام_With_Fallback(
        ثابت UResourceBundle *bundle, Resource_Data_مقدار &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت; }
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
    sink.put(bundle->f_کلید, value, !hasParent, errorCode);
    اگر (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        پشته_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_بالاترین_Level = درست;
        parentRef.fRes = parentRef.getResData().ریشه_Res;
        parentRef.fSize = res_شمار_Array_اقلام(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        پشته_UResource_Bundle containerBundle;
        ثابت UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        اگر (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } وگرنه {
            rb = ures_get_By_کلید_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        اگر (U_SUCCESS(pathErrorCode)) {
            get_All_اقلام_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

ساختار GetAllChildrenSink : عمومی ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    مجازی ~GetAllChildrenSink() override;
    مجازی تهی put(ثابت نویسه *کلید, Resource_مقدار &value, UBool is_ریشه,
           UErrorCode &errorCode) override {
        ResourceTable اقلام_Table = value.getTable(errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        برای (int32_t i = 0; اقلام_Table.get_کلید_And_مقدار(i, کلید, value); ++i) {
            اگر (value.getType() == URES_ALIAS) {
                Resource_Data_مقدار& rdv = static_cast<Resource_Data_مقدار&>(value);
                پشته_UResource_Bundle پشته_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          پشته_Temp_Bundle.getAlias(), &errorCode);
                اگر (U_SUCCESS(errorCode)) {
                    Resource_Data_مقدار aliased_مقدار;
                    aliased_مقدار.setData(aliasRB->getResData());
                    aliased_مقدار.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_مقدار.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    اگر (aliased_مقدار.getType() != URES_TABLE) {
                        dest.put(کلید, aliased_مقدار, is_ریشه, errorCode);
                    } وگرنه {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_مقدار_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        ثابت نویسه* parent_کلید = کلید; // dest.put() changes the key
                        dest.put(parent_کلید, aliased_مقدار, is_ریشه, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        وقتی (aliased_مقدار_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            نویسه* localPathAsCharPtr = localPath.data();
                            ثابت نویسه* child_کلید;
                            entry = entry->fParent;
                            res = entry->fData.ریشه_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_کلید);
                            اگر (newRes != RES_BOGUS) {
                                aliased_مقدار.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_مقدار.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_مقدار_Type = aliased_مقدار.getType();
                                اگر (aliased_مقدار_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_مقدار& rdv2 = static_cast<Resource_Data_مقدار&>(aliased_مقدار);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             پشته_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_مقدار.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_مقدار.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_مقدار_Type = aliased_مقدار.getType();
                                }
                                اگر (aliased_مقدار_Type == URES_TABLE) {
                                    dest.put(parent_کلید, aliased_مقدار, is_ریشه, errorCode);
                                } وگرنه {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    بازگشت;
                                }
                            }
                        }
                    }
                }
            } وگرنه {
                dest.put(کلید, value, is_ریشه, errorCode);
            }
            اگر (U_FAILURE(errorCode)) { بازگشت; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI تهی U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت نویسه *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_اقلام_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI تهی U_EXPORT2
ures_get_مقدار_With_Fallback(ثابت UResourceBundle *bundle, ثابت نویسه *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_مقدار &value, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    اگر (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    ثابت UResourceBundle *rb;
    اگر (*path == 0) {
        // empty path
        rb = bundle;
    } وگرنه {
        rb = ures_get_By_کلید_With_Fallback(bundle, path, tempFillIn, &errorCode);
        اگر (U_FAILURE(errorCode)) {
            بازگشت;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI تهی U_EXPORT2
ures_get_All_اقلام_With_Fallback(ثابت UResourceBundle *bundle, ثابت نویسه *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    اگر (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    پشته_UResource_Bundle پشته_Bundle;
    ثابت UResourceBundle *rb;
    اگر (*path == 0) {
        // empty path
        rb = bundle;
    } وگرنه {
        rb = ures_get_By_کلید_With_Fallback(bundle, path, پشته_Bundle.getAlias(), &errorCode);
        اگر (U_FAILURE(errorCode)) {
            بازگشت;
        }
    }
    // Get all table items with fallback.
    Resource_Data_مقدار value;
    get_All_اقلام_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلید(ثابت UResourceBundle* resB,
              ثابت نویسه* in_کلید,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ثابت نویسه *کلید = in_کلید;

    اگر (status==nullptr || U_FAILURE(*status)) {
        بازگشت fillIn;
    }
    اگر(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    اگر(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_کلید(&resB->getResData(), resB->fRes, &t, &کلید);
        اگر(res == RES_BOGUS) {
            کلید = in_کلید;
            اگر(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &کلید, &res, status);
                اگر(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    بازگشت init_resb_result(dataEntry, res, کلید, -1, resB, fillIn, status);
                } وگرنه {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } وگرنه {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } وگرنه {
            بازگشت init_resb_result(resB->fData, res, کلید, -1, resB, fillIn, status);
        }
    } 
#اگر 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    وگرنه اگر(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == درست) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &کلید, &res, status);
        اگر(U_SUCCESS(*status)) {
            بازگشت init_resb_result(dataEntry, res, کلید, resB, fillIn, status);
        } وگرنه {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    وگرنه {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت fillIn;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلید(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                    ثابت نویسه* in_کلید,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ثابت نویسه* کلید = in_کلید;

    اگر (status==nullptr || U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    اگر(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_کلید(&resB->getResData(), resB->fRes, &t, &کلید);

        اگر(res == RES_BOGUS) {
            کلید = in_کلید;
            اگر(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &کلید, &res, status);
                اگر(U_SUCCESS(*status)) {
                    انتخاب (RES_GET_TYPE(res)) {
                    حالت URES_STRING:
                    حالت URES_STRING_V2:
                        بازگشت res_getString({resB, کلید}, &dataEntry->fData, res, len);
                    حالت URES_ALIAS:
                      {
                        ثابت char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_کلید(resB, in_کلید, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        بازگشت result;
                      }
                    پیشفرض:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } وگرنه {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } وگرنه {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } وگرنه {
            انتخاب (RES_GET_TYPE(res)) {
            حالت URES_STRING:
            حالت URES_STRING_V2:
                بازگشت res_getString({resB, کلید}, &resB->getResData(), res, len);
            حالت URES_ALIAS:
              {
                ثابت char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_کلید(resB, in_کلید, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                بازگشت result;
              }
            پیشفرض:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#اگر 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    وگرنه اگر(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == درست) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &کلید, &res, status);
        اگر(U_SUCCESS(*status)) {
            // TODO: Tracing
            بازگشت res_getString(rd, res, len);
        } وگرنه {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    وگرنه {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت nullptr;
}

U_CAPI ثابت نویسه* U_EXPORT2
ures_get_UTF8_String_By_کلید(ثابت UResourceBundle* resB,
                        ثابت نویسه* کلید,
                        نویسه* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_کلید(resB, کلید, &length16, status);
    بازگشت ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ثابت نویسه* U_EXPORT2
ures_getLocaleInternal(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    اگر (status==nullptr || U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    } وگرنه {
      بازگشت resourceBundle->fData->fName;
    }
}

U_CAPI ثابت نویسه* U_EXPORT2
ures_getLocale(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  بازگشت ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI ثابت نویسه* U_EXPORT2
ures_getLocaleByType(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    اگر (status==nullptr || U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    } وگرنه {
        انتخاب(type) {
        حالت ULOC_ACTUAL_LOCALE:
            بازگشت resourceBundle->fData->fName;
        حالت ULOC_VALID_LOCALE:
            بازگشت resourceBundle->fValidLocaleDataEntry->fName;
        حالت ULOC_REQUESTED_LOCALE:
        پیشفرض:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت nullptr;
        }
    }
}

U_CFUNC ثابت نویسه*
ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  اگر(resB == nullptr) {
    بازگشت nullptr;
  }

  بازگشت resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC ثابت نویسه*
ures_getPath(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  اگر(resB == nullptr) {
    بازگشت nullptr;
  }

  بازگشت resB->fData->fPath;
}
#endif

ایستا UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ثابت نویسه* path,
                  ثابت نویسه* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    UResourceDataEntry *entry;
    اگر(openType != URES_OPEN_DIRECT) {
        اگر (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        اگر(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } وگرنه {
        entry = entryOpenDirect(path, localeID, status);
    }
    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        بازگشت nullptr;
    }

    UBool is_پشته_Object;
    اگر(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(اندازه(UResourceBundle)));
        اگر(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت nullptr;
        }
        is_پشته_Object = نادرست;
    } وگرنه {  // fill-in
        is_پشته_Object = ures_is_پشته_Object(r);
        ures_closeBundle(r, نادرست);
    }
    uprv_memset(r, 0, اندازه(UResourceBundle));
    ures_set_Is_پشته_Object(r, is_پشته_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_بالاترین_Level = درست;
    r->fRes = r->getResData().ریشه_Res;
    r->fSize = res_شمار_Array_اقلام(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    بازگشت r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ثابت نویسه* path, ثابت نویسه* localeID, UErrorCode* status) {
    بازگشت ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_ریشه, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت نویسه* path, ثابت نویسه* localeID, UErrorCode* status) {
    بازگشت ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_ریشه, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(ثابت نویسه* path, ثابت نویسه* localeID, UErrorCode* status) {
    بازگشت ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI تهی U_EXPORT2
ures_openFillIn(UResourceBundle *r, ثابت نویسه* path,
                ثابت نویسه* localeID, UErrorCode* status) {
    اگر(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_ریشه, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI تهی U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, ثابت نویسه* path, ثابت نویسه* localeID, UErrorCode* status) {
    اگر(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_شمار_Array_اقلام(ثابت UResourceBundle* resourceBundle,
                  ثابت نویسه* resource_کلید,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_پشته_Object(&resData);
    اگر (status==nullptr || U_FAILURE(*status)) {
        بازگشت 0;
    }
    اگر(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    ures_get_By_کلید(resourceBundle, resource_کلید, &resData, status);
    
    اگر(resData.getResData().data != nullptr) {
        int32_t result = res_شمار_Array_اقلام(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        بازگشت result;
    } وگرنه {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        بازگشت 0;
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
U_CAPI ثابت نویسه* U_EXPORT2
ures_get_Version_عدد_Internal(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    اگر (!resourceBundle) بازگشت nullptr;

    اگر(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ثابت char16_t* minor_version = ures_get_String_By_کلید(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (نویسه *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        اگر (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            بازگشت nullptr;
        }
       
        اگر(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        وگرنه {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    بازگشت resourceBundle->fVersion;
}

U_CAPI ثابت نویسه* U_EXPORT2
ures_get_Version_عدد(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    بازگشت ures_get_Version_عدد_Internal(resourceBundle);
}

U_CAPI تهی U_EXPORT2 ures_getVersion(ثابت UResourceBundle* resB, UVersionInfo versionInfo) {
    اگر (!resB) بازگشت;

    u_versionFromString(versionInfo, ures_get_Version_عدد_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#اگر defined(URES_درخت_DEBUG)
#شامل <stdio.h>
#endif

تعریفنوع ساختار ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

ایستا تهی U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

ایستا int32_t U_CALLCONV
ures_loc_شمار_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    بازگشت ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


ایستا ثابت نویسه * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    ثابت نویسه *result = nullptr;
    int32_t len = 0;
    اگر (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_کلید(k);
        len = (int32_t)uprv_strlen(result);
    }
    اگر (resultLength) {
        *resultLength = len;
    }
    بازگشت result;
}

ایستا تهی U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

ایستا ثابت UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_شمار_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(ثابت نویسه *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(اندازه(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(اندازه(UEnumeration));
    اگر(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        بازگشت nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, اندازه(UEnumeration));

    ures_init_پشته_Object(&myContext->installed);
    ures_init_پشته_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_کلید(idx, INDEX_TAG, &myContext->installed, status);
    اگر(U_SUCCESS(*status)) {
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_کلید(&myContext->installed));
#endif
        en->context = myContext;
    } وگرنه {
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    بازگشت en;
}

ایستا UBool isLocaleInList(UEnumeration *locEnum, ثابت نویسه *loc_To_جستجو, UErrorCode *status) {
    ثابت نویسه *loc;
    وقتی ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        اگر (uprv_strcmp(loc, loc_To_جستجو) == 0) {
            بازگشت درست;
        }
    }
    بازگشت نادرست;
}

ایستا تهی getParentForFunctionalEquivalent(ثابت نویسه*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    اگر (res != nullptr) {
        ures_get_By_کلید(res, "%%Parent", bund1, &subStatus);
        اگر (U_SUCCESS(subStatus)) {
            int32_t length16;
            ثابت char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    اگر (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(نویسه *result, int32_t resultCapacity,
                             ثابت نویسه *path, ثابت نویسه *resName, ثابت نویسه *keyword, ثابت نویسه *locid,
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
    اگر(U_FAILURE(*status)) بازگشت 0;
    CharString kwVal;
    اگر (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_مقدار(locid, keyword, subStatus);
        اگر (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    اگر (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#اگر defined(URES_درخت_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_پشته_Object(&bund1);
    ures_init_پشته_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    اگر(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = درست;
        اگر (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    اگر(U_FAILURE(subStatus)) {
        *status = subStatus;
        بازگشت 0;
    }
    
    انجامبده {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        اگر(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = نادرست;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        اگر(U_FAILURE(subStatus)) {
            *status = subStatus;
        } وگرنه اگر(subStatus == U_ZERO_ERROR) {
            ures_get_By_کلید(res,resName,&bund1, &subStatus);
            اگر(subStatus == U_ZERO_ERROR) {
                ثابت char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#اگر defined(URES_درخت_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                اگر(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#اگر defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    اگر(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#اگر defined(URES_درخت_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        اگر (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        اگر (found != parent) {
            parent.copyFrom(found, subStatus);
        } وگرنه {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } وقتی(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    انجامبده {
        res = ures_open(path, parent.data(), &subStatus);
        اگر((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = نادرست;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        اگر(U_FAILURE(subStatus)) {
            *status = subStatus;
        } وگرنه اگر(subStatus == U_ZERO_ERROR) {
            ures_get_By_کلید(res,resName,&bund1, &subStatus);
#اگر defined(URES_درخت_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            اگر(subStatus == U_ZERO_ERROR) {
                ures_get_By_کلید(&bund1, kwVal.data(), &bund2, &subStatus);
#اگر defined(URES_درخت_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                اگر(subStatus == U_ZERO_ERROR) {
#اگر defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    اگر (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } وگرنه {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        اگر(defLoc.length() > full.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#اگر defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          اگر(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#اگر defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#اگر defined(URES_درخت_DEBUG)
                        وگرنه {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } وگرنه {
#اگر defined(URES_درخت_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = نادرست;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        اگر (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            ثابت نویسه *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            اگر (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                اگر (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = درست;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        اگر (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } وقتی(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    اگر(full.isEmpty() && kwVal != defVal) {
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        انجامبده { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            اگر((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = نادرست;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#اگر defined(URES_درخت_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            اگر(U_FAILURE(subStatus)) {
                *status = subStatus;
            } وگرنه اگر(subStatus == U_ZERO_ERROR) {
                ures_get_By_کلید(res,resName,&bund1, &subStatus);
                اگر(subStatus == U_ZERO_ERROR) {
                    ures_get_By_کلید(&bund1, kwVal.data(), &bund2, &subStatus);
                    اگر(subStatus == U_ZERO_ERROR) {
#اگر defined(URES_درخت_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        اگر (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } وگرنه {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        اگر(defLoc.length() > full.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#اگر defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          اگر(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#اگر defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#اگر defined(URES_درخت_DEBUG)
                        وگرنه {
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
        } وقتی(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    اگر(U_SUCCESS(*status)) {
        اگر(full.isEmpty()) {
#اگر defined(URES_درخت_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } وگرنه اگر(omitDefault) {
#اگر defined(URES_درخت_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          اگر(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            اگر(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#اگر defined(URES_درخت_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        اگر(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } وگرنه اگر(!omitDefault) {
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

    اگر(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        اگر(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        اگر(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } وگرنه {
        length = 0;
        result[0]=0;
    }
    بازگشت u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_مقادیر(ثابت نویسه *path, ثابت نویسه *keyword, UErrorCode *status)
{
#define مقادیر_BUF_SIZE 2048
#define مقادیر_LIST_SIZE 512
    
    نویسه       مقادیر_Buf[مقادیر_BUF_SIZE];
    int32_t    مقادیر_Index = 0;
    ثابت نویسه *مقادیر_List[مقادیر_LIST_SIZE];
    int32_t    مقادیر_شمار = 0;
    
    ثابت نویسه *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_پشته_Object(&item);
    ures_init_پشته_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    اگر(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        بازگشت nullptr;
    }
    
    مقادیر_Buf[0]=0;
    مقادیر_Buf[1]=0;

    وقتی ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#اگر defined(URES_درخت_DEBUG)
        اگر(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_کلید(bund, keyword, &item, &subStatus);
        
        اگر(!bund || U_FAILURE(subStatus)) {
#اگر defined(URES_درخت_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            ادامه;
        }

        وقتی ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            ثابت نویسه *k;
            int32_t i;
            k = ures_get_کلید(subPtr);

#اگر defined(URES_درخت_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            اگر(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                ادامه;
            }
            برای(i=0; i<مقادیر_شمار; i++) {
                اگر(!uprv_strcmp(مقادیر_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    بشکن;
                }
            }
            اگر(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                اگر((مقادیر_شمار >= (مقادیر_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((مقادیر_Index+kLen+1+1) >= مقادیر_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } وگرنه {
                    uprv_strcpy(مقادیر_Buf+مقادیر_Index, k);
                    مقادیر_List[مقادیر_شمار++] = مقادیر_Buf+مقادیر_Index;
                    مقادیر_Index += kLen;
#اگر defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    مقادیر_Buf[مقادیر_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    مقادیر_Buf[مقادیر_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#اگر defined(URES_درخت_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        مقادیر_Index, مقادیر_شمار);
#endif
    بازگشت uloc_openKeywordList(مقادیر_Buf, مقادیر_Index, status);
}
#اگر 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ثابت UResourceBundle* res1, ثابت UResourceBundle* res2){
    اگر(res1==nullptr || res2==nullptr){
        بازگشت res1==res2; /* pointer comparison */
    }
    اگر(res1->f_کلید==nullptr||  res2->f_کلید==nullptr){
        بازگشت (res1->f_کلید==res2->f_کلید);
    }وگرنه{
        اگر(uprv_strcmp(res1->f_کلید, res2->f_کلید)!=0){
            بازگشت نادرست;
        }
    }
    اگر(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        بازگشت نادرست;
    }
    اگر(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        بازگشت (res1->fData->fPath == res2->fData->fPath);
    }وگرنه{
        اگر(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            بازگشت نادرست;
        }
    }
    اگر(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        بازگشت نادرست;
    }
    اگر(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        بازگشت نادرست;
    }
    اگر(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        بازگشت نادرست;
    }
    اگر(res1->fRes != res2->fRes){
        بازگشت نادرست;
    }
    بازگشت درست;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ثابت UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    اگر(U_FAILURE(*status) || res == nullptr){
        بازگشت nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    اگر(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }وگرنه{
        ret = bundle;
    }
    بازگشت ret;
}
U_CAPI ثابت UResourceBundle* U_EXPORT2
ures_getParentBundle(ثابت UResourceBundle* res){
    اگر(res==nullptr){
        بازگشت nullptr;
    }
    بازگشت res->fParentRes;
}
#endif

U_CAPI تهی U_EXPORT2
ures_get_Version_By_کلید(ثابت UResourceBundle* res, ثابت نویسه *کلید, UVersionInfo ver, UErrorCode *status) {
  ثابت char16_t *str;
  int32_t len;
  str = ures_get_String_By_کلید(res, کلید, &len, status);
  اگر(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
