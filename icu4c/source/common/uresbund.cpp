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

#ضمن "unicode/ures.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/ucnv.h"
#ضمن "bytesinkutil.h"
#ضمن "charstr.h"
#ضمن "uresimp.h"
#ضمن "ustr_imp.h"
#ضمن "cwchar.h"
#ضمن "ucln_cmn.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "mutex.h"
#ضمن "uhash.h"
#ضمن "unicode/uenum.h"
#ضمن "uenumimp.h"
#ضمن "ulocimp.h"
#ضمن "umutex.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"
#ضمن "uresdata.h"

using نطاق icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ساكن UHashtable *cache = nullptr;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ساكن int32_t U_CALLCONV hashEntry(ثابت UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    أرجع uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ساكن UBool U_CALLCONV compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ساكن UBool chopLocale(حرف *name) {
    حرف *i = uprv_strrchr(name, '_');

    𐡄𐡍(i != nullptr) {
        *i = '\0';
        أرجع 𐡒𐡔𐡈;
    }

    أرجع خطأ;
}

ساكن UBool hasVariant(ثابت حرف* localeID) {
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
    أرجع sink.NumberOfBytesAppended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#ضمن "localefallback_data.h"

ساكن ثابت حرف* performFallbackLookup(ثابت حرف* key,
                                         ثابت حرف* keyStrs,
                                         ثابت حرف* valueStrs U_LIFETIME_BOUND,
                                         ثابت int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    ثابت int32_t* bottom = lookupTable;
    ثابت int32_t* top = lookupTable + lookupTableLength;

    طالما (bottom < top) {
        // Effectively, divide by 2 and round down to an even index
        ثابت int32_t* middle = bottom + (((top - bottom) / 4) * 2);
        ثابت حرف* entryKey = &(keyStrs[*middle]);
        int32_t strcmpResult = uprv_strcmp(key, entryKey);
        𐡄𐡍 (strcmpResult == 0) {
            أرجع &(valueStrs[middle[1]]);
        } وإلا 𐡄𐡍 (strcmpResult < 0) {
            top = middle;
        } وإلا {
            bottom = middle + 2;
        }
    }
    أرجع nullptr;
}

ساكن CharString getDefaultScript(ثابت CharString& language, ثابت CharString& region) {
    ثابت حرف* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    𐡄𐡍 (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        𐡄𐡍 (U_FAILURE(err)) {
            أرجع result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    𐡄𐡍 (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    𐡄𐡍 (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    أرجع result;
}

تعداد UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_ROOT,
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
    URES_OPEN_LOCALE_ROOT,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
تعريفنوع تعداد UResOpenType UResOpenType;

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
ساكن منطقي getParentLocaleID(حرف *name, ثابت حرف *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    𐡄𐡍 (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        أرجع chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    𐡄𐡍 (U_FAILURE(err)) {
        // hopefully this never happens...
        أرجع chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    𐡄𐡍 (openType == URES_OPEN_LOCALE_DEFAULT_ROOT) {
        ثابت حرف* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        𐡄𐡍 (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            أرجع 𐡒𐡔𐡈;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    𐡄𐡍 (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        𐡄𐡍 (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } وإلا 𐡄𐡍 (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        𐡄𐡍 (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } وإلا 𐡄𐡍 (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        𐡄𐡍 (openType != URES_OPEN_LOCALE_DEFAULT_ROOT || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } وإلا {
            أرجع خطأ;
        }
    } وإلا {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        أرجع خطأ;
    }
    𐡄𐡍 (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        أرجع 𐡒𐡔𐡈;
    } وإلا {
        أرجع خطأ;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ساكن UBool mayHaveParent(حرف *name) {
    أرجع (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
ساكن فراغ entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->fCountExisting++;
    طالما(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->fCountExisting++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ساكن UResourceDataEntry *getFallbackData(
        ثابت UResourceBundle *resBundle,
        ثابت حرف **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    𐡄𐡍(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        أرجع nullptr;
    }
    𐡄𐡍(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_getTableItemByKey(&(dataEntry->fData), dataEntry->fData.rootRes, &indexR, resTag); /* try to get data from there */
        i++;
    }
    𐡄𐡍(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        طالما(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            𐡄𐡍(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_getTableItemByKey(&(dataEntry->fData), dataEntry->fData.rootRes, &indexR, resTag);
            }
        }
    }

    𐡄𐡍(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        أرجع nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    𐡄𐡍(i>1) {
        𐡄𐡍(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, kRootLocaleName)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } وإلا {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    أرجع dataEntry;
}

ساكن فراغ
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    𐡄𐡍(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    𐡄𐡍(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    𐡄𐡍(entry->fPool != nullptr) {
        --entry->fPool->fCountExisting;
    }
    alias = entry->fAlias;
    𐡄𐡍(alias != nullptr) {
        طالما(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->fCountExisting;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
ساكن int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rbDeletedNum = 0;
    ثابت UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    𐡄𐡍 (cache == nullptr) {
        أرجع 0;
    }

    𐡏𐡁𐡃 {
        deletedMore = خطأ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        طالما ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            𐡄𐡍 (resB->fCountExisting == 0) {
                rbDeletedNum++;
                deletedMore = 𐡒𐡔𐡈;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } طالما(deletedMore);

    أرجع rbDeletedNum;
}

#ifdef URES_DEBUG
#ضمن <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = خطأ;
  int32_t pos = UHASH_FIRST;
  ثابت UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    𐡄𐡍 (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      أرجع خطأ;
    }

    طالما ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=𐡒𐡔𐡈;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (فراغ*)resB, resB->fCountExisting,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (فراغ*)resB->fPool,
              (فراغ*)resB->fAlias,
              (فراغ*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_count(cache));
    أرجع cacheNotEmpty;
}

#endif

ساكن UBool U_CALLCONV ures_cleanup()
{
    𐡄𐡍 (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    أرجع 𐡒𐡔𐡈;
}

/** INTERNAL: Initializes the cache for resources */
ساكن فراغ U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ساكن فراغ initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ساكن فراغ setEntryName(UResourceDataEntry *res, ثابت حرف *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    𐡄𐡍(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    𐡄𐡍 (len < static_cast<int32_t>(حجم(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    وإلا {
        res->fName = static_cast<حرف*>(uprv_malloc(len + 1));
    }
    𐡄𐡍(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } وإلا {
        uprv_strcpy(res->fName, name);
    }
}

ساكن UResourceDataEntry *
getPoolEntry(ثابت حرف *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ساكن UResourceDataEntry *init_entry(ثابت حرف *localeID, ثابت حرف *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    ثابت حرف *name;
    حرف aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    𐡄𐡍(U_FAILURE(*status)) {
        أرجع nullptr;
    }

    /* here we try to deduce the right locale name */
    𐡄𐡍(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } وإلا 𐡄𐡍(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = kRootLocaleName;
    } وإلا { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<حرف*>(name);
    find.fPath = const_cast<حرف*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    𐡄𐡍(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(حجم(UResourceDataEntry)));
        𐡄𐡍(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            أرجع nullptr;
        }

        uprv_memset(r, 0, حجم(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        𐡄𐡍 (U_FAILURE(*status)) {
            uprv_free(r);
            أرجع nullptr;
        }

        𐡄𐡍(path != nullptr) {
            r->fPath = uprv_strdup(path);
            𐡄𐡍(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                أرجع nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        𐡄𐡍 (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            𐡄𐡍 (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                أرجع nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } وإلا { /* if we have a regular entry */
            Resource aliasres;
            𐡄𐡍 (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                𐡄𐡍 (U_SUCCESS(*status)) {
                    ثابت int32_t *poolIndexes = r->fPool->fData.pRoot + 1;
                    𐡄𐡍(r->fData.pRoot[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.poolBundleKeys = reinterpret_cast<ثابت حرف*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } وإلا {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } وإلا {
                    r->fBogus = *status;
                }
            }
            𐡄𐡍 (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                𐡄𐡍 (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ثابت char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    𐡄𐡍(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            𐡄𐡍 ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (فراغ *)r, r, &cacheStatus);
                𐡄𐡍 (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } وإلا {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    𐡄𐡍(r != nullptr) {
        /* return the real bundle */
        طالما(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->fCountExisting++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        𐡄𐡍(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    أرجع r;
}

ساكن UResourceDataEntry *
getPoolEntry(ثابت حرف *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    𐡄𐡍( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    أرجع poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ساكن UResourceDataEntry *
findFirstExisting(ثابت حرف* path, حرف* name, ثابت حرف* defaultLocale, UResOpenType openType,
                  UBool *isRoot, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = خطأ;
    *foundParent = 𐡒𐡔𐡈; /* we're starting with a fresh name */
    حرف origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    طالما(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        𐡄𐡍 (U_FAILURE(*status)) {
            أرجع nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        𐡄𐡍(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->fCountExisting--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } وإلا {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *isRoot = static_cast<UBool>(uprv_strcmp(name, kRootLocaleName) == 0);

        /*Fallback data stuff*/
        𐡄𐡍 (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } وإلا {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        𐡄𐡍 (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    أرجع r;
}

ساكن فراغ ures_setIsStackObject( UResourceBundle* resB, UBool state) {
    𐡄𐡍(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } وإلا {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ساكن UBool ures_isStackObject(ثابت UResourceBundle* resB) {
  أرجع((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?خطأ:𐡒𐡔𐡈);
}


U_CFUNC فراغ ures_initStackObject(UResourceBundle* resB) {
  uprv_memset(resB, 0, حجم(UResourceBundle));
  ures_setIsStackObject(resB, 𐡒𐡔𐡈);
}

U_NAMESPACE_BEGIN

StackUResourceBundle::StackUResourceBundle() {
    ures_initStackObject(&bundle);
}

StackUResourceBundle::~StackUResourceBundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

ساكن UBool  // returns U_SUCCESS(*status)
loadParentsExceptRoot(UResourceDataEntry *&t1,
                      حرف name[], int32_t nameCapacity,
                      UBool usingUSRData, حرف usrDataPath[], UErrorCode *status) {
    𐡄𐡍 (U_FAILURE(*status)) { أرجع خطأ; }
    UBool checkParent = 𐡒𐡔𐡈;
    طالما (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        𐡄𐡍 (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            ثابت char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            𐡄𐡍(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                𐡄𐡍 (uprv_strcmp(name, kRootLocaleName) == 0) {
                    أرجع 𐡒𐡔𐡈;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        𐡄𐡍 (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            أرجع خطأ;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        𐡄𐡍 (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            𐡄𐡍 (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                أرجع خطأ;
            }
        }

        𐡄𐡍 (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } وإلا {
            t1->fParent = t2;
            𐡄𐡍 (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->fCountExisting = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    أرجع 𐡒𐡔𐡈;
}

ساكن UBool  // returns U_SUCCESS(*status)
insertRootBundle(UResourceDataEntry *&t1, UErrorCode *status) {
    𐡄𐡍 (U_FAILURE(*status)) { أرجع خطأ; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(kRootLocaleName, t1->fPath, &parentStatus);
    𐡄𐡍 (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        أرجع خطأ;
    }
    t1->fParent = t2;
    t1 = t2;
    أرجع 𐡒𐡔𐡈;
}

ساكن UResourceDataEntry *entryOpen(ثابت حرف* path, ثابت حرف* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = خطأ;
    UBool isRoot = خطأ;
    UBool hasRealData = خطأ;
    UBool hasChopped = 𐡒𐡔𐡈;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    حرف name[ULOC_FULLNAME_CAPACITY];
    حرف usrDataPath[96];

    initCache(status);

    𐡄𐡍(U_FAILURE(*status)) {
        أرجع nullptr;
    }

    uprv_strncpy(name, localeID, حجم(name) - 1);
    name[حجم(name) - 1] = 0;

    𐡄𐡍 ( usingUSRData ) {
        𐡄𐡍 ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } وإلا {
            uprv_strncpy(usrDataPath, path, حجم(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[حجم(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ثابت حرف *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &isRoot, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    𐡄𐡍 (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        اذهب finish;
    }

    𐡄𐡍(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = 𐡒𐡔𐡈;
        𐡄𐡍 ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            𐡄𐡍 (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                اذهب finish;
            }
            𐡄𐡍 ( u1 != nullptr ) {
                𐡄𐡍(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } وإلا {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->fCountExisting = 0;
                }
            }
        }
        𐡄𐡍 ((hasChopped || mayHaveParent(name)) && !isRoot) {
            𐡄𐡍 (!loadParentsExceptRoot(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                اذهب finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    𐡄𐡍(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_ROOT && !isDefault && !isRoot) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &isRoot, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        𐡄𐡍 (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            اذهب finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        𐡄𐡍(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = 𐡒𐡔𐡈;
            isDefault = 𐡒𐡔𐡈;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            𐡄𐡍 ((hasChopped || mayHaveParent(name)) && !isRoot) {
                𐡄𐡍 (!loadParentsExceptRoot(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    اذهب finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    𐡄𐡍(r == nullptr) {
        uprv_strcpy(name, kRootLocaleName);
        r = findFirstExisting(path, name, defaultLocale, openType, &isRoot, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        𐡄𐡍 (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            اذهب finish;
        }
        𐡄𐡍(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = 𐡒𐡔𐡈;
        } وإلا { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            اذهب finish;
        }
    } وإلا 𐡄𐡍(!isRoot && uprv_strcmp(t1->fName, kRootLocaleName) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        𐡄𐡍 (!insertRootBundle(t1, status)) {
            اذهب finish;
        }
        𐡄𐡍(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    طالما(r != nullptr && !isRoot && t1->fParent != nullptr) {
        t1->fParent->fCountExisting++;
        t1 = t1->fParent;
    }

finish:
    𐡄𐡍(U_SUCCESS(*status)) {
        𐡄𐡍(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        أرجع r;
    } وإلا {
        أرجع nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ساكن UResourceDataEntry *
entryOpenDirect(ثابت حرف* path, ثابت حرف* localeID, UErrorCode* status) {
    initCache(status);
    𐡄𐡍(U_FAILURE(*status)) {
        أرجع nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    𐡄𐡍 (localeID == nullptr) {
        localeID = uloc_getDefault();
    } وإلا 𐡄𐡍 (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = kRootLocaleName;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    𐡄𐡍(U_SUCCESS(*status)) {
        𐡄𐡍(r->fBogus != U_ZERO_ERROR) {
            r->fCountExisting--;
            r = nullptr;
        }
    } وإلا {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    𐡄𐡍(r != nullptr && uprv_strcmp(localeID, kRootLocaleName) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        حرف name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        𐡄𐡍(!chopLocale(name) || uprv_strcmp(name, kRootLocaleName) == 0 ||
                loadParentsExceptRoot(t1, name, UPRV_LENGTHOF(name), خطأ, nullptr, status)) {
            𐡄𐡍(uprv_strcmp(t1->fName, kRootLocaleName) != 0 && t1->fParent == nullptr) {
                insertRootBundle(t1, status);
            }
        }
        𐡄𐡍(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    𐡄𐡍(r != nullptr) {
        // TODO: Does this ever loop?
        طالما(t1->fParent != nullptr) {
            t1->fParent->fCountExisting++;
            t1 = t1->fParent;
        }
    }
    أرجع r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ساكن فراغ entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    طالما(resB != nullptr) {
        p = resB->fParent;
        resB->fCountExisting--;

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

ساكن فراغ entryClose(UResourceDataEntry *resB) {
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
ساكن فراغ ures_appendResPath(UResourceBundle *resB, ثابت حرف* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    𐡄𐡍(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    𐡄𐡍(RES_BUFSIZE <= resB->fResPathLen+1) {
        𐡄𐡍(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<حرف*>(uprv_malloc((resB->fResPathLen + 1) * حجم(حرف)));
            /* Check that memory was allocated correctly. */
            𐡄𐡍 (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                أرجع;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } وإلا {
            حرف* temp = static_cast<حرف*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * حجم(حرف)));
            /* Check that memory was reallocated correctly. */
            𐡄𐡍 (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                أرجع;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

ساكن فراغ ures_freeResPath(UResourceBundle *resB) {
    𐡄𐡍 (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

ساكن فراغ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    𐡄𐡍(resB != nullptr) {
        𐡄𐡍(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        𐡄𐡍(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        𐡄𐡍(ures_isStackObject(resB) == خطأ && freeBundleObj) {
            uprv_free(resB);
        }
#𐡄𐡍 0 /*U_DEBUG*/
        وإلا {
            /* poison the data */
            uprv_memset(resB, -1, حجم(UResourceBundle));
        }
#endif
    }
}

U_CAPI فراغ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, 𐡒𐡔𐡈);
}

نطاق {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ثابت حرف* key,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ثابت حرف* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(ثابت ResourceData& resData,
                               Resource r,
                               ثابت حرف* key,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               ثابت حرف* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    𐡄𐡍 (U_FAILURE(*status)) { أرجع resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ثابت char16_t *alias = res_getAlias(&resData, r, &len);
    𐡄𐡍(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع resB;
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
    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع nullptr;
    }

    // We have an alias, now let's cut it up.
    ثابت حرف *path = nullptr, *locale = nullptr, *keyPath = nullptr;
    𐡄𐡍(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        حرف *chAliasData = chAlias.data();
        حرف *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        𐡄𐡍(sep != nullptr) {
            *sep++ = 0;
        }
        𐡄𐡍(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            keyPath = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } وإلا {
            𐡄𐡍(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            𐡄𐡍 (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } وإلا {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                𐡄𐡍(sep != nullptr) {
                    *sep++ = 0;
                }
                keyPath = sep;
            }
        }
    } وإلا {
        // No path, start with a locale.
        حرف *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        𐡄𐡍(sep != nullptr) {
            *sep++ = 0;
        }
        keyPath = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    𐡄𐡍 (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } وإلا {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        𐡄𐡍(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            أرجع resB;
        }
        dataEntry = mainRes->fData;
    }

    ثابت حرف* temp = nullptr;
    𐡄𐡍(keyPath == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.rootRes;
        𐡄𐡍(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            𐡄𐡍 (U_FAILURE(*status)) {
                أرجع nullptr;
            }
            حرف *aKey = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &aKey, &temp);
        }
        𐡄𐡍(key) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(key, *status);
            𐡄𐡍 (U_FAILURE(*status)) {
                أرجع nullptr;
            }
            حرف *aKey = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &aKey, &temp);
        } وإلا 𐡄𐡍(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            𐡄𐡍(URES_IS_TABLE(type)) {
                ثابت حرف *aKey;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &aKey);
            } وإلا { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        𐡄𐡍(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } وإلا {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } وإلا {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(keyPath, *status);
        𐡄𐡍 (U_FAILURE(*status)) {
            أرجع nullptr;
        }
        حرف *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        لكل(;;) {
            r = dataEntry->fData.rootRes;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            طالما(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                𐡄𐡍(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    اكسر;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                𐡄𐡍 (U_FAILURE(*status)) {
                    اكسر;
                }
                𐡄𐡍 (temp == nullptr || uprv_strcmp(keyPath, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, keyPath, static_cast<int32_t>(uprv_strlen(keyPath)), status);
                    𐡄𐡍(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    𐡄𐡍 (U_FAILURE(*status)) {
                        اكسر;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            𐡄𐡍 (U_FAILURE(*status) || r != RES_BOGUS) {
                اكسر;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            𐡄𐡍 (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                اكسر;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, keyPath);
        }
    }
    𐡄𐡍(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    أرجع resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ثابت حرف* key,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ثابت حرف* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    𐡄𐡍(status == nullptr || U_FAILURE(*status)) {
        أرجع resB;
    }
    𐡄𐡍 (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    𐡄𐡍(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        𐡄𐡍(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            أرجع resB;
        }
        أرجع getAliasTargetAsResourceBundle(
            dataEntry->fData, r, key, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    𐡄𐡍(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        𐡄𐡍 (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            أرجع nullptr;
        }
        ures_setIsStackObject(resB, خطأ);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } وإلا {
        𐡄𐡍(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        𐡄𐡍(resB->fVersion != nullptr) {
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
        𐡄𐡍(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = خطأ;
    resB->fIsTopLevel = خطأ;
    resB->fIndex = -1;
    resB->fKey = key; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    𐡄𐡍(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    𐡄𐡍(key != nullptr) {
        ures_appendResPath(resB, key, static_cast<int32_t>(uprv_strlen(key)), status);
        𐡄𐡍(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } وإلا 𐡄𐡍(idx >= 0) {
        حرف buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        𐡄𐡍(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, حجم(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_countArrayItems(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    أرجع resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ثابت حرف* key,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ثابت UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    أرجع init_resb_result(
        dataEntry, r, key, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ثابت UResourceBundle* original,
                               UErrorCode* status) {
    UBool isStackObject;
    𐡄𐡍(U_FAILURE(*status) || r == original) {
        أرجع r;
    }
    𐡄𐡍(original != nullptr) {
        𐡄𐡍(r == nullptr) {
            isStackObject = خطأ;
            r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
            /* test for nullptr */
            𐡄𐡍 (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                أرجع nullptr;
            }
        } وإلا {
            isStackObject = ures_isStackObject(r);
            ures_closeBundle(r, خطأ);
        }
        uprv_memcpy(r, original, حجم(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        𐡄𐡍(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_setIsStackObject(r, isStackObject);
        𐡄𐡍(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    أرجع r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ثابت char16_t* U_EXPORT2 ures_getString(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    ثابت char16_t *s;
    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐡄𐡍(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    𐡄𐡍 (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ساكن ثابت حرف*
ures_toUTF8String(ثابت char16_t* s16,
                  int32_t length16,
                  حرف* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐡄𐡍 (pLength != nullptr) {
        capacity = *pLength;
    } وإلا {
        capacity = 0;
    }
    𐡄𐡍 (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    𐡄𐡍 (length16 == 0) {
        /* empty string, return as read-only pointer */
        𐡄𐡍 (pLength != nullptr) {
            *pLength = 0;
        }
        𐡄𐡍 (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            أرجع dest;
        } وإلا {
            أرجع "";
        }
    } وإلا {
        /* We need to transform the string to the destination buffer. */
        𐡄𐡍 (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            أرجع u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        𐡄𐡍 (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            𐡄𐡍 (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        أرجع u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getUTF8String(ثابت UResourceBundle* resB,
                   حرف* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getString(resB, &length16, status);
    أرجع ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI ثابت uint8_t* U_EXPORT2
ures_getBinary(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ثابت uint8_t *p;
  𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
    أرجع nullptr;
  }
  𐡄𐡍(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع nullptr;
  }
  p = res_getBinary({resB}, &resB->getResData(), resB->fRes, len);
  𐡄𐡍 (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

U_CAPI ثابت int32_t* U_EXPORT2
ures_getIntVector(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ثابت int32_t *p;
  𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
    أرجع nullptr;
  }
  𐡄𐡍(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  𐡄𐡍 (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ثابت UResourceBundle* resB, UErrorCode *status) {
  𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
    أرجع 0xffffffff;
  }
  𐡄𐡍(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0xffffffff;
  }
  𐡄𐡍(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ثابت UResourceBundle* resB, UErrorCode *status) {
  𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
    أرجع 0xffffffff;
  }
  𐡄𐡍(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0xffffffff;
  }
  𐡄𐡍(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ثابت UResourceBundle *resB) {
  𐡄𐡍(resB == nullptr) {
    أرجع URES_NONE;
  }
  أرجع res_getPublicType(resB->fRes);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getKey(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  𐡄𐡍(resB == nullptr) {
    أرجع nullptr;
  }
  أرجع(resB->fKey);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(ثابت UResourceBundle *resB) {
  𐡄𐡍(resB == nullptr) {
    أرجع 0;
  }
  
  أرجع resB->fSize;
}

ساكن ثابت char16_t*
ures_getStringWithAlias(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  𐡄𐡍(RES_GET_TYPE(r) == URES_ALIAS) {
    ثابت char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    أرجع result;
  } وإلا {
    أرجع res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI فراغ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  𐡄𐡍(resB == nullptr) {
    أرجع;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(ثابت UResourceBundle *resB) {
  𐡄𐡍(resB == nullptr) {
    أرجع خطأ;
  }
  أرجع resB->fIndex < resB->fSize-1;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ثابت حرف** key,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
    أرجع nullptr;
  }
  𐡄𐡍(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع nullptr;
  }
  
  𐡄𐡍(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } وإلا {
    resB->fIndex++;
    بدل(RES_GET_TYPE(resB->fRes)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
      أرجع res_getString({resB}, &resB->getResData(), resB->fRes, len);
    حالة URES_TABLE:
    حالة URES_TABLE16:
    حالة URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, key);
      𐡄𐡍(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    حالة URES_ARRAY:
    حالة URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      𐡄𐡍(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    حالة URES_ALIAS:
      أرجع ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    حالة URES_INT:
    حالة URES_BINARY:
    حالة URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    افتراضي:
      أرجع nullptr;
    }
  }

  أرجع nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    ثابت حرف *key = nullptr;
    Resource r = RES_BOGUS;

    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            أرجع fillIn;
    }
    𐡄𐡍(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            أرجع fillIn;
    }

    𐡄𐡍(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } وإلا {
        resB->fIndex++;
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_BINARY:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_VECTOR:
            أرجع ures_copyResb(fillIn, resB, status);
        حالة URES_TABLE:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &key);
            𐡄𐡍(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_result(resB->fData, r, key, resB->fIndex, resB, fillIn, status);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            𐡄𐡍(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_result(resB->fData, r, key, resB->fIndex, resB, fillIn, status);
        افتراضي:
            /*return nullptr;*/
            أرجع fillIn;
        }
    }
    /*return nullptr;*/
    أرجع fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(ثابت UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    ثابت حرف* key = nullptr;
    Resource r = RES_BOGUS;

    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        أرجع fillIn;
    }
    𐡄𐡍(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        أرجع fillIn;
    }

    𐡄𐡍(indexR >= 0 && resB->fSize > indexR) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_BINARY:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_VECTOR:
            أرجع ures_copyResb(fillIn, resB, status);
        حالة URES_TABLE:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &key);
            𐡄𐡍(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_result(resB->fData, r, key, indexR, resB, fillIn, status);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            𐡄𐡍(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_result(resB->fData, r, key, indexR, resB, fillIn, status);
        افتراضي:
            /*return nullptr;*/
            أرجع fillIn;
        }
    } وإلا {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    أرجع fillIn;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_getStringByIndex(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    ثابت حرف* key = nullptr;
    Resource r = RES_BOGUS;

    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐡄𐡍(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    𐡄𐡍(indexS >= 0 && resB->fSize > indexS) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_STRING:
        حالة URES_STRING_V2:
            أرجع res_getString({resB}, &resB->getResData(), resB->fRes, len);
        حالة URES_TABLE:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &key);
            𐡄𐡍(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, indexS, len, status);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            𐡄𐡍(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, indexS, len, status);
        حالة URES_ALIAS:
            أرجع ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        حالة URES_INT:
        حالة URES_BINARY:
        حالة URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            اكسر;
        افتراضي:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          اكسر;
        }
    } وإلا {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    أرجع nullptr;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getUTF8StringByIndex(ثابت UResourceBundle* resB,
                          int32_t idx,
                          حرف* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    أرجع ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(ثابت حرف* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  حرف *packageName = nullptr;
  حرف *pathToResource = nullptr, *save = nullptr;
  حرف *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  𐡄𐡍(status == nullptr || U_FAILURE(*status)) {
    أرجع result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (حرف *)uprv_malloc(length*حجم(حرف));
  /* test for nullptr */
  𐡄𐡍(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    أرجع result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  𐡄𐡍(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    𐡄𐡍(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } وإلا {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  𐡄𐡍(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  𐡄𐡍(U_SUCCESS(*status)) {
    𐡄𐡍(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } وإلا {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  أرجع result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(ثابت UResourceBundle* resB,
                     حرف* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  ثابت حرف *key;

  𐡄𐡍(status == nullptr || U_FAILURE(*status)) {
    أرجع result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  𐡏𐡁𐡃 {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &key); 
    𐡄𐡍(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, key, -1, resB, fillIn, status);
        resB = result;
    } وإلا {
        *status = U_MISSING_RESOURCE_ERROR;
        اكسر;
    }
  } طالما(*path); /* there is more stuff in the path */

  أرجع result;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_getStringByKeyWithFallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت حرف* inKey,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle stack;
    ثابت char16_t* retVal = nullptr;
    ures_initStackObject(&stack);
    ures_getByKeyWithFallback(resB, inKey, &stack, status);
    int32_t length;
    retVal = ures_getString(&stack, &length, status);
    ures_close(&stack);
    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐡄𐡍 (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    𐡄𐡍 (len != nullptr) {
        *len = length;
    }
    أرجع retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ساكن Resource getTableItemByKeyPath(ثابت ResourceData *pResData, Resource table, ثابت حرف *key) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(key, errorCode);
  𐡄𐡍 (U_FAILURE(errorCode)) { أرجع RES_BOGUS; }
  حرف *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  طالما (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    حرف *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    𐡄𐡍 (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } وإلا {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    ثابت حرف *pathP = pathPart;
    resource = res_getTableItemByKey(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  𐡄𐡍 (*pathPart) {
    أرجع RES_BOGUS;
  }
  أرجع resource;
}

ساكن فراغ createPath(ثابت حرف* origResPath,
                       int32_t     origResPathLen,
                       ثابت حرف* resPath,
                       int32_t     resPathLen,
                       ثابت حرف* inKey,
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
    ثابت حرف* key = inKey;
    𐡄𐡍 (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        𐡄𐡍 (U_SUCCESS(*status)) {
            ثابت حرف* resPathLimit = resPath + resPathLen;
            ثابت حرف* origResPathLimit = origResPath + origResPathLen;
            ثابت حرف* resPathPtr = resPath;
            ثابت حرف* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            طالما (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                طالما (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                𐡄𐡍 (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                طالما (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                𐡄𐡍 (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            طالما (resPathPtr < resPathLimit && *key != '\0') {
                طالما (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                𐡄𐡍 (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                طالما (*key != '\0' && *key != RES_PATH_SEPARATOR) {
                    ++key;
                }
                𐡄𐡍 (*key == RES_PATH_SEPARATOR) {
                    ++key;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(key, *status);
    } وإلا {
        path.append(inKey, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByKeyWithFallback(ثابت UResourceBundle* resB,
                          ثابت حرف* inKey,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, rootRes = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        أرجع fillIn;
    }
    𐡄𐡍(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    𐡄𐡍(URES_IS_TABLE(type)) {
        ثابت حرف* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = getTableItemByKeyPath(&resB->getResData(), resB->fRes, inKey);
        ثابت حرف* key = inKey;
        منطقي didRootOnce = خطأ;
        𐡄𐡍(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            حرف *myPath = nullptr;
            ثابت حرف* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            طالما(res == RES_BOGUS && (dataEntry->fParent != nullptr || !didRootOnce)) { /* Otherwise, we'll look in parents */
                𐡄𐡍 (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } وإلا {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    didRootOnce = 𐡒𐡔𐡈;
                }
                rootRes = dataEntry->fData.rootRes;

                𐡄𐡍(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, inKey, path, status);
                    𐡄𐡍 (U_FAILURE(*status)) {
                        ures_close(helper);
                        أرجع fillIn;
                    }
                    myPath = path.data();
                    key = inKey;
                    𐡏𐡁𐡃 {
                        res = res_findResource(&(dataEntry->fData), rootRes, &myPath, &key);
                        𐡄𐡍 (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            𐡄𐡍(helper) {
                              dataEntry = helper->fData;
                              rootRes = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } وإلا {
                              اكسر;
                            }
                        } وإلا 𐡄𐡍 (res == RES_BOGUS) {
                            اكسر;
                        }
                    } طالما(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            𐡄𐡍(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                𐡄𐡍(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, kRootLocaleName)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } وإلا {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, key, -1, resB, fillIn, status);
                𐡄𐡍 (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, inKey, path, status);
                } وإلا {
                    ثابت حرف* separator = nullptr;
                    𐡄𐡍 (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    𐡄𐡍 (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), inKey, path, status);
                    } وإلا {
                        createPath(origResPath, origResPathLen, "", 0, inKey, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                𐡄𐡍(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } وإلا {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            fillIn = init_resb_result(resB->fData, res, key, -1, resB, fillIn, status);
        }
    } 
    وإلا {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    أرجع fillIn;
}

نطاق {

فراغ getAllItemsWithFallback(
        ثابت UResourceBundle *bundle, ResourceDataValue &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    𐡄𐡍 (U_FAILURE(errorCode)) { أرجع; }
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
    sink.put(bundle->fKey, value, !hasParent, errorCode);
    𐡄𐡍 (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        StackUResourceBundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.fIsTopLevel = 𐡒𐡔𐡈;
        parentRef.fRes = parentRef.getResData().rootRes;
        parentRef.fSize = res_countArrayItems(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        StackUResourceBundle containerBundle;
        ثابت UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        𐡄𐡍 (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } وإلا {
            rb = ures_getByKeyWithFallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        𐡄𐡍 (U_SUCCESS(pathErrorCode)) {
            getAllItemsWithFallback(rb, value, sink, errorCode);
        }
    }
}

بنية GetAllChildrenSink : عام ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    virtual ~GetAllChildrenSink() override;
    virtual فراغ put(ثابت حرف *key, ResourceValue &value, UBool isRoot,
           UErrorCode &errorCode) override {
        ResourceTable itemsTable = value.getTable(errorCode);
        𐡄𐡍 (U_FAILURE(errorCode)) { أرجع; }
        لكل (int32_t i = 0; itemsTable.getKeyAndValue(i, key, value); ++i) {
            𐡄𐡍 (value.getType() == URES_ALIAS) {
                ResourceDataValue& rdv = static_cast<ResourceDataValue&>(value);
                StackUResourceBundle stackTempBundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          stackTempBundle.getAlias(), &errorCode);
                𐡄𐡍 (U_SUCCESS(errorCode)) {
                    ResourceDataValue aliasedValue;
                    aliasedValue.setData(aliasRB->getResData());
                    aliasedValue.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliasedValue.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    𐡄𐡍 (aliasedValue.getType() != URES_TABLE) {
                        dest.put(key, aliasedValue, isRoot, errorCode);
                    } وإلا {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliasedValueType = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        ثابت حرف* parentKey = key; // dest.put() changes the key
                        dest.put(parentKey, aliasedValue, isRoot, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        طالما (aliasedValueType == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            حرف* localPathAsCharPtr = localPath.data();
                            ثابت حرف* childKey;
                            entry = entry->fParent;
                            res = entry->fData.rootRes;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &childKey);
                            𐡄𐡍 (newRes != RES_BOGUS) {
                                aliasedValue.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliasedValue.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliasedValueType = aliasedValue.getType();
                                𐡄𐡍 (aliasedValueType == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    ResourceDataValue& rdv2 = static_cast<ResourceDataValue&>(aliasedValue);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             stackTempBundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliasedValue.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliasedValue.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliasedValueType = aliasedValue.getType();
                                }
                                𐡄𐡍 (aliasedValueType == URES_TABLE) {
                                    dest.put(parentKey, aliasedValue, isRoot, errorCode);
                                } وإلا {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    أرجع;
                                }
                            }
                        }
                    }
                }
            } وإلا {
                dest.put(key, value, isRoot, errorCode);
            }
            𐡄𐡍 (U_FAILURE(errorCode)) { أرجع; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI فراغ U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_getAllItemsWithFallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI فراغ U_EXPORT2
ures_getValueWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *path,
                          UResourceBundle *tempFillIn,
                          ResourceDataValue &value, UErrorCode &errorCode) {
    𐡄𐡍 (U_FAILURE(errorCode)) { أرجع; }
    𐡄𐡍 (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    ثابت UResourceBundle *rb;
    𐡄𐡍 (*path == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_getByKeyWithFallback(bundle, path, tempFillIn, &errorCode);
        𐡄𐡍 (U_FAILURE(errorCode)) {
            أرجع;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI فراغ U_EXPORT2
ures_getAllItemsWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    𐡄𐡍 (U_FAILURE(errorCode)) { أرجع; }
    𐡄𐡍 (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    StackUResourceBundle stackBundle;
    ثابت UResourceBundle *rb;
    𐡄𐡍 (*path == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_getByKeyWithFallback(bundle, path, stackBundle.getAlias(), &errorCode);
        𐡄𐡍 (U_FAILURE(errorCode)) {
            أرجع;
        }
    }
    // Get all table items with fallback.
    ResourceDataValue value;
    getAllItemsWithFallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByKey(ثابت UResourceBundle* resB,
              ثابت حرف* inKey,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ثابت حرف *key = inKey;

    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        أرجع fillIn;
    }
    𐡄𐡍(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    𐡄𐡍(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_getTableItemByKey(&resB->getResData(), resB->fRes, &t, &key);
        𐡄𐡍(res == RES_BOGUS) {
            key = inKey;
            𐡄𐡍(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &key, &res, status);
                𐡄𐡍(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    أرجع init_resb_result(dataEntry, res, key, -1, resB, fillIn, status);
                } وإلا {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } وإلا {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            أرجع init_resb_result(resB->fData, res, key, -1, resB, fillIn, status);
        }
    } 
#𐡄𐡍 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    وإلا 𐡄𐡍(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == 𐡒𐡔𐡈) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &key, &res, status);
        𐡄𐡍(U_SUCCESS(*status)) {
            أرجع init_resb_result(dataEntry, res, key, resB, fillIn, status);
        } وإلا {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    وإلا {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع fillIn;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_getStringByKey(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                    ثابت حرف* inKey,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ثابت حرف* key = inKey;

    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐡄𐡍(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    𐡄𐡍(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_getTableItemByKey(&resB->getResData(), resB->fRes, &t, &key);

        𐡄𐡍(res == RES_BOGUS) {
            key = inKey;
            𐡄𐡍(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &key, &res, status);
                𐡄𐡍(U_SUCCESS(*status)) {
                    بدل (RES_GET_TYPE(res)) {
                    حالة URES_STRING:
                    حالة URES_STRING_V2:
                        أرجع res_getString({resB, key}, &dataEntry->fData, res, len);
                    حالة URES_ALIAS:
                      {
                        ثابت char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_getByKey(resB, inKey, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        أرجع result;
                      }
                    افتراضي:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } وإلا {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } وإلا {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            بدل (RES_GET_TYPE(res)) {
            حالة URES_STRING:
            حالة URES_STRING_V2:
                أرجع res_getString({resB, key}, &resB->getResData(), res, len);
            حالة URES_ALIAS:
              {
                ثابت char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_getByKey(resB, inKey, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                أرجع result;
              }
            افتراضي:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#𐡄𐡍 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    وإلا 𐡄𐡍(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == 𐡒𐡔𐡈) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &key, &res, status);
        𐡄𐡍(U_SUCCESS(*status)) {
            // TODO: Tracing
            أرجع res_getString(rd, res, len);
        } وإلا {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    وإلا {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع nullptr;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getUTF8StringByKey(ثابت UResourceBundle* resB,
                        ثابت حرف* key,
                        حرف* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getStringByKey(resB, key, &length16, status);
    أرجع ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ثابت حرف* U_EXPORT2
ures_getLocaleInternal(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐡄𐡍 (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    } وإلا {
      أرجع resourceBundle->fData->fName;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocale(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  أرجع ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocaleByType(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐡄𐡍 (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    } وإلا {
        بدل(type) {
        حالة ULOC_ACTUAL_LOCALE:
            أرجع resourceBundle->fData->fName;
        حالة ULOC_VALID_LOCALE:
            أرجع resourceBundle->fValidLocaleDataEntry->fName;
        حالة ULOC_REQUESTED_LOCALE:
        افتراضي:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع nullptr;
        }
    }
}

U_CFUNC ثابت حرف*
ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  𐡄𐡍(resB == nullptr) {
    أرجع nullptr;
  }

  أرجع resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC ثابت حرف*
ures_getPath(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  𐡄𐡍(resB == nullptr) {
    أرجع nullptr;
  }

  أرجع resB->fData->fPath;
}
#endif

ساكن UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ثابت حرف* path,
                  ثابت حرف* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    𐡄𐡍(U_FAILURE(*status)) {
        أرجع nullptr;
    }

    UResourceDataEntry *entry;
    𐡄𐡍(openType != URES_OPEN_DIRECT) {
        𐡄𐡍 (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        𐡄𐡍(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } وإلا {
        entry = entryOpenDirect(path, localeID, status);
    }
    𐡄𐡍(U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐡄𐡍(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        أرجع nullptr;
    }

    UBool isStackObject;
    𐡄𐡍(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        𐡄𐡍(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            أرجع nullptr;
        }
        isStackObject = خطأ;
    } وإلا {  // fill-in
        isStackObject = ures_isStackObject(r);
        ures_closeBundle(r, خطأ);
    }
    uprv_memset(r, 0, حجم(UResourceBundle));
    ures_setIsStackObject(r, isStackObject);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->fIsTopLevel = 𐡒𐡔𐡈;
    r->fRes = r->getResData().rootRes;
    r->fSize = res_countArrayItems(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    أرجع r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ثابت حرف* path, ثابت حرف* localeID, UErrorCode* status) {
    أرجع ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_ROOT, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت حرف* path, ثابت حرف* localeID, UErrorCode* status) {
    أرجع ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_ROOT, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(ثابت حرف* path, ثابت حرف* localeID, UErrorCode* status) {
    أرجع ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_openFillIn(UResourceBundle *r, ثابت حرف* path,
                ثابت حرف* localeID, UErrorCode* status) {
    𐡄𐡍(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_ROOT, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, ثابت حرف* path, ثابت حرف* localeID, UErrorCode* status) {
    𐡄𐡍(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_countArrayItems(ثابت UResourceBundle* resourceBundle,
                  ثابت حرف* resourceKey,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_initStackObject(&resData);
    𐡄𐡍 (status==nullptr || U_FAILURE(*status)) {
        أرجع 0;
    }
    𐡄𐡍(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    ures_getByKey(resourceBundle, resourceKey, &resData, status);
    
    𐡄𐡍(resData.getResData().data != nullptr) {
        int32_t result = res_countArrayItems(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        أرجع result;
    } وإلا {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        أرجع 0;
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
U_CAPI ثابت حرف* U_EXPORT2
ures_getVersionNumberInternal(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    𐡄𐡍 (!resourceBundle) أرجع nullptr;

    𐡄𐡍(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ثابت char16_t* minor_version = ures_getStringByKey(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (حرف *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        𐡄𐡍 (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            أرجع nullptr;
        }
       
        𐡄𐡍(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        وإلا {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    أرجع resourceBundle->fVersion;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getVersionNumber(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    أرجع ures_getVersionNumberInternal(resourceBundle);
}

U_CAPI فراغ U_EXPORT2 ures_getVersion(ثابت UResourceBundle* resB, UVersionInfo versionInfo) {
    𐡄𐡍 (!resB) أرجع;

    u_versionFromString(versionInfo, ures_getVersionNumberInternal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#𐡄𐡍 defined(URES_TREE_DEBUG)
#ضمن <stdio.h>
#endif

تعريفنوع بنية ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

ساكن فراغ U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

ساكن int32_t U_CALLCONV
ures_loc_countLocales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    أرجع ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


ساكن ثابت حرف * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    ثابت حرف *result = nullptr;
    int32_t len = 0;
    𐡄𐡍 (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_getKey(k);
        len = (int32_t)uprv_strlen(result);
    }
    𐡄𐡍 (resultLength) {
        *resultLength = len;
    }
    أرجع result;
}

ساكن فراغ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

ساكن ثابت UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_countLocales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(ثابت حرف *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    𐡄𐡍(U_FAILURE(*status)) {
        أرجع nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(حجم(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    𐡄𐡍(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        أرجع nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, حجم(UEnumeration));

    ures_initStackObject(&myContext->installed);
    ures_initStackObject(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_getByKey(idx, INDEX_TAG, &myContext->installed, status);
    𐡄𐡍(U_SUCCESS(*status)) {
#𐡄𐡍 defined(URES_TREE_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_getKey(&myContext->installed));
#endif
        en->context = myContext;
    } وإلا {
#𐡄𐡍 defined(URES_TREE_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    أرجع en;
}

ساكن UBool isLocaleInList(UEnumeration *locEnum, ثابت حرف *locToSearch, UErrorCode *status) {
    ثابت حرف *loc;
    طالما ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        𐡄𐡍 (uprv_strcmp(loc, locToSearch) == 0) {
            أرجع 𐡒𐡔𐡈;
        }
    }
    أرجع خطأ;
}

ساكن فراغ getParentForFunctionalEquivalent(ثابت حرف*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    𐡄𐡍 (res != nullptr) {
        ures_getByKey(res, "%%Parent", bund1, &subStatus);
        𐡄𐡍 (U_SUCCESS(subStatus)) {
            int32_t length16;
            ثابت char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    𐡄𐡍 (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(حرف *result, int32_t resultCapacity,
                             ثابت حرف *path, ثابت حرف *resName, ثابت حرف *keyword, ثابت حرف *locid,
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
    𐡄𐡍(U_FAILURE(*status)) أرجع 0;
    CharString kwVal;
    𐡄𐡍 (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_getKeywordValue(locid, keyword, subStatus);
        𐡄𐡍 (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    𐡄𐡍 (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#𐡄𐡍 defined(URES_TREE_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_initStackObject(&bund1);
    ures_initStackObject(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    𐡄𐡍(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = 𐡒𐡔𐡈;
        𐡄𐡍 (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    𐡄𐡍(U_FAILURE(subStatus)) {
        *status = subStatus;
        أرجع 0;
    }
    
    𐡏𐡁𐡃 {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        𐡄𐡍(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = خطأ;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#𐡄𐡍 defined(URES_TREE_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        𐡄𐡍(U_FAILURE(subStatus)) {
            *status = subStatus;
        } وإلا 𐡄𐡍(subStatus == U_ZERO_ERROR) {
            ures_getByKey(res,resName,&bund1, &subStatus);
            𐡄𐡍(subStatus == U_ZERO_ERROR) {
                ثابت char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#𐡄𐡍 defined(URES_TREE_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_getStringByKey(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                𐡄𐡍(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#𐡄𐡍 defined(URES_TREE_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    𐡄𐡍(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#𐡄𐡍 defined(URES_TREE_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        𐡄𐡍 (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        𐡄𐡍 (found != parent) {
            parent.copyFrom(found, subStatus);
        } وإلا {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } طالما(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    𐡏𐡁𐡃 {
        res = ures_open(path, parent.data(), &subStatus);
        𐡄𐡍((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = خطأ;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#𐡄𐡍 defined(URES_TREE_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        𐡄𐡍(U_FAILURE(subStatus)) {
            *status = subStatus;
        } وإلا 𐡄𐡍(subStatus == U_ZERO_ERROR) {
            ures_getByKey(res,resName,&bund1, &subStatus);
#𐡄𐡍 defined(URES_TREE_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            𐡄𐡍(subStatus == U_ZERO_ERROR) {
                ures_getByKey(&bund1, kwVal.data(), &bund2, &subStatus);
#𐡄𐡍 defined(URES_TREE_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                𐡄𐡍(subStatus == U_ZERO_ERROR) {
#𐡄𐡍 defined(URES_TREE_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    𐡄𐡍 (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } وإلا {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        𐡄𐡍(defLoc.length() > full.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#𐡄𐡍 defined(URES_TREE_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_getStringByKey(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          𐡄𐡍(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#𐡄𐡍 defined(URES_TREE_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#𐡄𐡍 defined(URES_TREE_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } وإلا {
#𐡄𐡍 defined(URES_TREE_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = خطأ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        𐡄𐡍 (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            ثابت حرف *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            𐡄𐡍 (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                𐡄𐡍 (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = 𐡒𐡔𐡈;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        𐡄𐡍 (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } طالما(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    𐡄𐡍(full.isEmpty() && kwVal != defVal) {
#𐡄𐡍 defined(URES_TREE_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        𐡏𐡁𐡃 { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            𐡄𐡍((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = خطأ;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#𐡄𐡍 defined(URES_TREE_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            𐡄𐡍(U_FAILURE(subStatus)) {
                *status = subStatus;
            } وإلا 𐡄𐡍(subStatus == U_ZERO_ERROR) {
                ures_getByKey(res,resName,&bund1, &subStatus);
                𐡄𐡍(subStatus == U_ZERO_ERROR) {
                    ures_getByKey(&bund1, kwVal.data(), &bund2, &subStatus);
                    𐡄𐡍(subStatus == U_ZERO_ERROR) {
#𐡄𐡍 defined(URES_TREE_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        𐡄𐡍 (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } وإلا {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        𐡄𐡍(defLoc.length() > full.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#𐡄𐡍 defined(URES_TREE_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_getStringByKey(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          𐡄𐡍(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#𐡄𐡍 defined(URES_TREE_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#𐡄𐡍 defined(URES_TREE_DEBUG)
                        وإلا {
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
        } طالما(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    𐡄𐡍(U_SUCCESS(*status)) {
        𐡄𐡍(full.isEmpty()) {
#𐡄𐡍 defined(URES_TREE_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } وإلا 𐡄𐡍(omitDefault) {
#𐡄𐡍 defined(URES_TREE_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          𐡄𐡍(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            𐡄𐡍(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#𐡄𐡍 defined(URES_TREE_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        𐡄𐡍(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } وإلا 𐡄𐡍(!omitDefault) {
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

    𐡄𐡍(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        𐡄𐡍(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        𐡄𐡍(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } وإلا {
        length = 0;
        result[0]=0;
    }
    أرجع u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_getKeywordValues(ثابت حرف *path, ثابت حرف *keyword, UErrorCode *status)
{
#define VALUES_BUF_SIZE 2048
#define VALUES_LIST_SIZE 512
    
    حرف       valuesBuf[VALUES_BUF_SIZE];
    int32_t    valuesIndex = 0;
    ثابت حرف *valuesList[VALUES_LIST_SIZE];
    int32_t    valuesCount = 0;
    
    ثابت حرف *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_initStackObject(&item);
    ures_initStackObject(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    𐡄𐡍(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        أرجع nullptr;
    }
    
    valuesBuf[0]=0;
    valuesBuf[1]=0;

    طالما ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#𐡄𐡍 defined(URES_TREE_DEBUG)
        𐡄𐡍(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_getByKey(bund, keyword, &item, &subStatus);
        
        𐡄𐡍(!bund || U_FAILURE(subStatus)) {
#𐡄𐡍 defined(URES_TREE_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            استمر;
        }

        طالما ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            ثابت حرف *k;
            int32_t i;
            k = ures_getKey(subPtr);

#𐡄𐡍 defined(URES_TREE_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            𐡄𐡍(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                استمر;
            }
            لكل(i=0; i<valuesCount; i++) {
                𐡄𐡍(!uprv_strcmp(valuesList[i],k)) {
                    k = nullptr; /* found duplicate */
                    اكسر;
                }
            }
            𐡄𐡍(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                𐡄𐡍((valuesCount >= (VALUES_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((valuesIndex+kLen+1+1) >= VALUES_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } وإلا {
                    uprv_strcpy(valuesBuf+valuesIndex, k);
                    valuesList[valuesCount++] = valuesBuf+valuesIndex;
                    valuesIndex += kLen;
#𐡄𐡍 defined(URES_TREE_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    valuesBuf[valuesIndex++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    valuesBuf[valuesIndex++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#𐡄𐡍 defined(URES_TREE_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        valuesIndex, valuesCount);
#endif
    أرجع uloc_openKeywordList(valuesBuf, valuesIndex, status);
}
#𐡄𐡍 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ثابت UResourceBundle* res1, ثابت UResourceBundle* res2){
    𐡄𐡍(res1==nullptr || res2==nullptr){
        أرجع res1==res2; /* pointer comparison */
    }
    𐡄𐡍(res1->fKey==nullptr||  res2->fKey==nullptr){
        أرجع (res1->fKey==res2->fKey);
    }وإلا{
        𐡄𐡍(uprv_strcmp(res1->fKey, res2->fKey)!=0){
            أرجع خطأ;
        }
    }
    𐡄𐡍(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        أرجع خطأ;
    }
    𐡄𐡍(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        أرجع (res1->fData->fPath == res2->fData->fPath);
    }وإلا{
        𐡄𐡍(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            أرجع خطأ;
        }
    }
    𐡄𐡍(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        أرجع خطأ;
    }
    𐡄𐡍(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        أرجع خطأ;
    }
    𐡄𐡍(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        أرجع خطأ;
    }
    𐡄𐡍(res1->fRes != res2->fRes){
        أرجع خطأ;
    }
    أرجع 𐡒𐡔𐡈;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ثابت UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    𐡄𐡍(U_FAILURE(*status) || res == nullptr){
        أرجع nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    𐡄𐡍(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }وإلا{
        ret = bundle;
    }
    أرجع ret;
}
U_CAPI ثابت UResourceBundle* U_EXPORT2
ures_getParentBundle(ثابت UResourceBundle* res){
    𐡄𐡍(res==nullptr){
        أرجع nullptr;
    }
    أرجع res->fParentRes;
}
#endif

U_CAPI فراغ U_EXPORT2
ures_getVersionByKey(ثابت UResourceBundle* res, ثابت حرف *key, UVersionInfo ver, UErrorCode *status) {
  ثابت char16_t *str;
  int32_t len;
  str = ures_getStringByKey(res, key, &len, status);
  𐡄𐡍(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
