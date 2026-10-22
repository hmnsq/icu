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

#include "unicode/ures.h"
#include "unicode/ustring.h"
#include "unicode/ucnv.h"
#include "bytesinkutil.h"
#include "charstr.h"
#include "uresimp.h"
#include "ustr_imp.h"
#include "cwchar.h"
#include "ucln_cmn.h"
#include "cmemory.h"
#include "cstring.h"
#include "mutex.h"
#include "uhash.h"
#include "unicode/uenum.h"
#include "uenumimp.h"
#include "ulocimp.h"
#include "umutex.h"
#include "putilimp.h"
#include "uassert.h"
#include "uresdata.h"

using namespace icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
static UHashtable *cache = nullptr;
static icu::UInitOnce gCacheInitOnce {};

static UMutex resbMutex;

/* INTERNAL: hashes an entry  */
static int32_t U_CALLCONV hashEntry(const UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    ᤀᤡᤴ uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
static UBool U_CALLCONV compareEntries(const UHashTok p1, const UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    ᤀᤡᤴ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
static UBool chopLocale(var_na *name) {
    var_na *i = uprv_strrchr(name, '_');

    ᤀᤡ᤺(i != nullptr) {
        *i = '\0';
        ᤀᤡᤴ true;
    }

    ᤀᤡᤴ false;
}

static UBool hasVariant(const var_na* localeID) {
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
    ᤀᤡᤴ sink.NumberOfBytesAppended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#include "localefallback_data.h"

static const var_na* performFallbackLookup(const var_na* key,
                                         const var_na* keyStrs,
                                         const var_na* valueStrs U_LIFETIME_BOUND,
                                         const int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    const int32_t* bottom = lookupTable;
    const int32_t* top = lookupTable + lookupTableLength;

    ᤀᤠ᤺ (bottom < top) {
        // Effectively, divide by 2 and round down to an even index
        const int32_t* middle = bottom + (((top - bottom) / 4) * 2);
        const var_na* entryKey = &(keyStrs[*middle]);
        int32_t strcmpResult = uprv_strcmp(key, entryKey);
        ᤀᤡ᤺ (strcmpResult == 0) {
            ᤀᤡᤴ &(valueStrs[middle[1]]);
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (strcmpResult < 0) {
            top = middle;
        } ᤀᤡ᤺ᤆᤡ {
            bottom = middle + 2;
        }
    }
    ᤀᤡᤴ nullptr;
}

static CharString getDefaultScript(const CharString& language, const CharString& region) {
    const var_na* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    ᤀᤡ᤺ (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        ᤀᤡ᤺ (U_FAILURE(err)) {
            ᤀᤡᤴ result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    ᤀᤡ᤺ (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    ᤀᤡ᤺ (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    ᤀᤡᤴ result;
}

ga_nanaa UResOpenType {
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
typedef ga_nanaa UResOpenType UResOpenType;

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
static bool getParentLocaleID(var_na *name, const var_na *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    ᤀᤡ᤺ (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        ᤀᤡᤴ chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    ᤀᤡ᤺ (U_FAILURE(err)) {
        // hopefully this never happens...
        ᤀᤡᤴ chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    ᤀᤡ᤺ (openType == URES_OPEN_LOCALE_DEFAULT_ROOT) {
        const var_na* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        ᤀᤡ᤺ (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            ᤀᤡᤴ true;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    ᤀᤡ᤺ (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        ᤀᤡ᤺ (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } ᤀᤡ᤺ᤆᤡ {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        ᤀᤡ᤺ (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } ᤀᤡ᤺ᤆᤡ {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        ᤀᤡ᤺ (openType != URES_OPEN_LOCALE_DEFAULT_ROOT || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } ᤀᤡ᤺ᤆᤡ {
            ᤀᤡᤴ false;
        }
    } ᤀᤡ᤺ᤆᤡ {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        ᤀᤡᤴ false;
    }
    ᤀᤡ᤺ (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        ᤀᤡᤴ true;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ false;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
static UBool mayHaveParent(var_na *name) {
    ᤀᤡᤴ (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
static void entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->fCountExisting++;
    ᤀᤠ᤺(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->fCountExisting++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
static UResourceDataEntry *getFallbackData(
        const UResourceBundle *resBundle,
        const var_na **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    ᤀᤡ᤺(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_getTableItemByKey(&(dataEntry->fData), dataEntry->fData.rootRes, &indexR, resTag); /* try to get data from there */
        i++;
    }
    ᤀᤡ᤺(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        ᤀᤠ᤺(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            ᤀᤡ᤺(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_getTableItemByKey(&(dataEntry->fData), dataEntry->fData.rootRes, &indexR, resTag);
            }
        }
    }

    ᤀᤡ᤺(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        ᤀᤡᤴ nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    ᤀᤡ᤺(i>1) {
        ᤀᤡ᤺(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, kRootLocaleName)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } ᤀᤡ᤺ᤆᤡ {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    ᤀᤡᤴ dataEntry;
}

static void
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    ᤀᤡ᤺(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    ᤀᤡ᤺(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    ᤀᤡ᤺(entry->fPool != nullptr) {
        --entry->fPool->fCountExisting;
    }
    alias = entry->fAlias;
    ᤀᤡ᤺(alias != nullptr) {
        ᤀᤠ᤺(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->fCountExisting;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
static int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rbDeletedNum = 0;
    const UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    ᤀᤡ᤺ (cache == nullptr) {
        ᤀᤡᤴ 0;
    }

    do {
        deletedMore = false;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        ᤀᤠ᤺ ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            ᤀᤡ᤺ (resB->fCountExisting == 0) {
                rbDeletedNum++;
                deletedMore = true;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } ᤀᤠ᤺(deletedMore);

    ᤀᤡᤴ rbDeletedNum;
}

#ifdef URES_DEBUG
#include <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = false;
  int32_t pos = UHASH_FIRST;
  const UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    ᤀᤡ᤺ (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      ᤀᤡᤴ false;
    }

    ᤀᤠ᤺ ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=true;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (void*)resB, resB->fCountExisting,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (void*)resB->fPool,
              (void*)resB->fAlias,
              (void*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_count(cache));
    ᤀᤡᤴ cacheNotEmpty;
}

#endif

static UBool U_CALLCONV ures_cleanup()
{
    ᤀᤡ᤺ (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    ᤀᤡᤴ true;
}

/** INTERNAL: Initializes the cache for resources */
static void U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
static void initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

static void setEntryName(UResourceDataEntry *res, const var_na *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    ᤀᤡ᤺(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    ᤀᤡ᤺ (len < static_cast<int32_t>(parimaa_na(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    ᤀᤡ᤺ᤆᤡ {
        res->fName = static_cast<var_na*>(uprv_malloc(len + 1));
    }
    ᤀᤡ᤺(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ᤀᤡ᤺ᤆᤡ {
        uprv_strcpy(res->fName, name);
    }
}

static UResourceDataEntry *
getPoolEntry(const var_na *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
static UResourceDataEntry *init_entry(const var_na *localeID, const var_na *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    const var_na *name;
    var_na aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    ᤀᤡ᤺(U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }

    /* here we try to deduce the right locale name */
    ᤀᤡ᤺(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = kRootLocaleName;
    } ᤀᤡ᤺ᤆᤡ { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<var_na*>(name);
    find.fPath = const_cast<var_na*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    ᤀᤡ᤺(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(parimaa_na(UResourceDataEntry)));
        ᤀᤡ᤺(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ᤀᤡᤴ nullptr;
        }

        uprv_memset(r, 0, parimaa_na(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        ᤀᤡ᤺ (U_FAILURE(*status)) {
            uprv_free(r);
            ᤀᤡᤴ nullptr;
        }

        ᤀᤡ᤺(path != nullptr) {
            r->fPath = uprv_strdup(path);
            ᤀᤡ᤺(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                ᤀᤡᤴ nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        ᤀᤡ᤺ (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            ᤀᤡ᤺ (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                ᤀᤡᤴ nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } ᤀᤡ᤺ᤆᤡ { /* if we have a regular entry */
            Resource aliasres;
            ᤀᤡ᤺ (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                ᤀᤡ᤺ (U_SUCCESS(*status)) {
                    const int32_t *poolIndexes = r->fPool->fData.pRoot + 1;
                    ᤀᤡ᤺(r->fData.pRoot[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.poolBundleKeys = reinterpret_cast<const var_na*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } ᤀᤡ᤺ᤆᤡ {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } ᤀᤡ᤺ᤆᤡ {
                    r->fBogus = *status;
                }
            }
            ᤀᤡ᤺ (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                ᤀᤡ᤺ (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    const char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    ᤀᤡ᤺(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            ᤀᤡ᤺ ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (void *)r, r, &cacheStatus);
                ᤀᤡ᤺ (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } ᤀᤡ᤺ᤆᤡ {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    ᤀᤡ᤺(r != nullptr) {
        /* return the real bundle */
        ᤀᤠ᤺(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->fCountExisting++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        ᤀᤡ᤺(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    ᤀᤡᤴ r;
}

static UResourceDataEntry *
getPoolEntry(const var_na *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    ᤀᤡ᤺( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    ᤀᤡᤴ poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
static UResourceDataEntry *
findFirstExisting(const var_na* path, var_na* name, const var_na* defaultLocale, UResOpenType openType,
                  UBool *isRoot, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = false;
    *foundParent = true; /* we're starting with a fresh name */
    var_na origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    ᤀᤠ᤺(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        ᤀᤡ᤺ (U_FAILURE(*status)) {
            ᤀᤡᤴ nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        ᤀᤡ᤺(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->fCountExisting--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } ᤀᤡ᤺ᤆᤡ {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *isRoot = static_cast<UBool>(uprv_strcmp(name, kRootLocaleName) == 0);

        /*Fallback data stuff*/
        ᤀᤡ᤺ (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } ᤀᤡ᤺ᤆᤡ {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        ᤀᤡ᤺ (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    ᤀᤡᤴ r;
}

static void ures_setIsStackObject( UResourceBundle* resB, UBool state) {
    ᤀᤡ᤺(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } ᤀᤡ᤺ᤆᤡ {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

static UBool ures_isStackObject(const UResourceBundle* resB) {
  ᤀᤡᤴ((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?false:true);
}


U_CFUNC void ures_initStackObject(UResourceBundle* resB) {
  uprv_memset(resB, 0, parimaa_na(UResourceBundle));
  ures_setIsStackObject(resB, true);
}

U_NAMESPACE_BEGIN

StackUResourceBundle::StackUResourceBundle() {
    ures_initStackObject(&bundle);
}

StackUResourceBundle::~StackUResourceBundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

static UBool  // returns U_SUCCESS(*status)
loadParentsExceptRoot(UResourceDataEntry *&t1,
                      var_na name[], int32_t nameCapacity,
                      UBool usingUSRData, var_na usrDataPath[], UErrorCode *status) {
    ᤀᤡ᤺ (U_FAILURE(*status)) { ᤀᤡᤴ false; }
    UBool checkParent = true;
    ᤀᤠ᤺ (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        ᤀᤡ᤺ (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            const char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            ᤀᤡ᤺(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                ᤀᤡ᤺ (uprv_strcmp(name, kRootLocaleName) == 0) {
                    ᤀᤡᤴ true;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        ᤀᤡ᤺ (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            ᤀᤡᤴ false;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        ᤀᤡ᤺ (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            ᤀᤡ᤺ (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                ᤀᤡᤴ false;
            }
        }

        ᤀᤡ᤺ (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } ᤀᤡ᤺ᤆᤡ {
            t1->fParent = t2;
            ᤀᤡ᤺ (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->fCountExisting = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    ᤀᤡᤴ true;
}

static UBool  // returns U_SUCCESS(*status)
insertRootBundle(UResourceDataEntry *&t1, UErrorCode *status) {
    ᤀᤡ᤺ (U_FAILURE(*status)) { ᤀᤡᤴ false; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(kRootLocaleName, t1->fPath, &parentStatus);
    ᤀᤡ᤺ (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        ᤀᤡᤴ false;
    }
    t1->fParent = t2;
    t1 = t2;
    ᤀᤡᤴ true;
}

static UResourceDataEntry *entryOpen(const var_na* path, const var_na* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = false;
    UBool isRoot = false;
    UBool hasRealData = false;
    UBool hasChopped = true;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    var_na name[ULOC_FULLNAME_CAPACITY];
    var_na usrDataPath[96];

    initCache(status);

    ᤀᤡ᤺(U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }

    uprv_strncpy(name, localeID, parimaa_na(name) - 1);
    name[parimaa_na(name) - 1] = 0;

    ᤀᤡ᤺ ( usingUSRData ) {
        ᤀᤡ᤺ ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } ᤀᤡ᤺ᤆᤡ {
            uprv_strncpy(usrDataPath, path, parimaa_na(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[parimaa_na(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    const var_na *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &isRoot, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    ᤀᤡ᤺ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        goto finish;
    }

    ᤀᤡ᤺(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = true;
        ᤀᤡ᤺ ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            ᤀᤡ᤺ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                goto finish;
            }
            ᤀᤡ᤺ ( u1 != nullptr ) {
                ᤀᤡ᤺(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } ᤀᤡ᤺ᤆᤡ {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->fCountExisting = 0;
                }
            }
        }
        ᤀᤡ᤺ ((hasChopped || mayHaveParent(name)) && !isRoot) {
            ᤀᤡ᤺ (!loadParentsExceptRoot(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                goto finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    ᤀᤡ᤺(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_ROOT && !isDefault && !isRoot) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &isRoot, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        ᤀᤡ᤺ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            goto finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        ᤀᤡ᤺(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = true;
            isDefault = true;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            ᤀᤡ᤺ ((hasChopped || mayHaveParent(name)) && !isRoot) {
                ᤀᤡ᤺ (!loadParentsExceptRoot(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    goto finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    ᤀᤡ᤺(r == nullptr) {
        uprv_strcpy(name, kRootLocaleName);
        r = findFirstExisting(path, name, defaultLocale, openType, &isRoot, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        ᤀᤡ᤺ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            goto finish;
        }
        ᤀᤡ᤺(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = true;
        } ᤀᤡ᤺ᤆᤡ { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            goto finish;
        }
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!isRoot && uprv_strcmp(t1->fName, kRootLocaleName) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        ᤀᤡ᤺ (!insertRootBundle(t1, status)) {
            goto finish;
        }
        ᤀᤡ᤺(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    ᤀᤠ᤺(r != nullptr && !isRoot && t1->fParent != nullptr) {
        t1->fParent->fCountExisting++;
        t1 = t1->fParent;
    }

finish:
    ᤀᤡ᤺(U_SUCCESS(*status)) {
        ᤀᤡ᤺(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        ᤀᤡᤴ r;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
static UResourceDataEntry *
entryOpenDirect(const var_na* path, const var_na* localeID, UErrorCode* status) {
    initCache(status);
    ᤀᤡ᤺(U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    ᤀᤡ᤺ (localeID == nullptr) {
        localeID = uloc_getDefault();
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = kRootLocaleName;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    ᤀᤡ᤺(U_SUCCESS(*status)) {
        ᤀᤡ᤺(r->fBogus != U_ZERO_ERROR) {
            r->fCountExisting--;
            r = nullptr;
        }
    } ᤀᤡ᤺ᤆᤡ {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    ᤀᤡ᤺(r != nullptr && uprv_strcmp(localeID, kRootLocaleName) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        var_na name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        ᤀᤡ᤺(!chopLocale(name) || uprv_strcmp(name, kRootLocaleName) == 0 ||
                loadParentsExceptRoot(t1, name, UPRV_LENGTHOF(name), false, nullptr, status)) {
            ᤀᤡ᤺(uprv_strcmp(t1->fName, kRootLocaleName) != 0 && t1->fParent == nullptr) {
                insertRootBundle(t1, status);
            }
        }
        ᤀᤡ᤺(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    ᤀᤡ᤺(r != nullptr) {
        // TODO: Does this ever loop?
        ᤀᤠ᤺(t1->fParent != nullptr) {
            t1->fParent->fCountExisting++;
            t1 = t1->fParent;
        }
    }
    ᤀᤡᤴ r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
static void entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    ᤀᤠ᤺(resB != nullptr) {
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

static void entryClose(UResourceDataEntry *resB) {
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
static void ures_appendResPath(UResourceBundle *resB, const var_na* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    ᤀᤡ᤺(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    ᤀᤡ᤺(RES_BUFSIZE <= resB->fResPathLen+1) {
        ᤀᤡ᤺(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<var_na*>(uprv_malloc((resB->fResPathLen + 1) * parimaa_na(var_na)));
            /* Check that memory was allocated correctly. */
            ᤀᤡ᤺ (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ᤀᤡᤴ;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } ᤀᤡ᤺ᤆᤡ {
            var_na* temp = static_cast<var_na*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * parimaa_na(var_na)));
            /* Check that memory was reallocated correctly. */
            ᤀᤡ᤺ (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ᤀᤡᤴ;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

static void ures_freeResPath(UResourceBundle *resB) {
    ᤀᤡ᤺ (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

static void
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    ᤀᤡ᤺(resB != nullptr) {
        ᤀᤡ᤺(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        ᤀᤡ᤺(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        ᤀᤡ᤺(ures_isStackObject(resB) == false && freeBundleObj) {
            uprv_free(resB);
        }
#ᤀᤡ᤺ 0 /*U_DEBUG*/
        ᤀᤡ᤺ᤆᤡ {
            /* poison the data */
            uprv_memset(resB, -1, parimaa_na(UResourceBundle));
        }
#endif
    }
}

U_CAPI void  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, true);
}

namespace {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  const var_na* key,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  const var_na* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(const ResourceData& resData,
                               Resource r,
                               const var_na* key,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               const var_na* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ᤀᤡ᤺ (U_FAILURE(*status)) { ᤀᤡᤴ resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    const char16_t *alias = res_getAlias(&resData, r, &len);
    ᤀᤡ᤺(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ resB;
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
    ᤀᤡ᤺ (U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }

    // We have an alias, now let's cut it up.
    const var_na *path = nullptr, *locale = nullptr, *keyPath = nullptr;
    ᤀᤡ᤺(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        var_na *chAliasData = chAlias.data();
        var_na *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        ᤀᤡ᤺(sep != nullptr) {
            *sep++ = 0;
        }
        ᤀᤡ᤺(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            keyPath = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } ᤀᤡ᤺ᤆᤡ {
            ᤀᤡ᤺(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            ᤀᤡ᤺ (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } ᤀᤡ᤺ᤆᤡ {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                ᤀᤡ᤺(sep != nullptr) {
                    *sep++ = 0;
                }
                keyPath = sep;
            }
        }
    } ᤀᤡ᤺ᤆᤡ {
        // No path, start with a locale.
        var_na *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        ᤀᤡ᤺(sep != nullptr) {
            *sep++ = 0;
        }
        keyPath = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    ᤀᤡ᤺ (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } ᤀᤡ᤺ᤆᤡ {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        ᤀᤡ᤺(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            ᤀᤡᤴ resB;
        }
        dataEntry = mainRes->fData;
    }

    const var_na* temp = nullptr;
    ᤀᤡ᤺(keyPath == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.rootRes;
        ᤀᤡ᤺(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            ᤀᤡ᤺ (U_FAILURE(*status)) {
                ᤀᤡᤴ nullptr;
            }
            var_na *aKey = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &aKey, &temp);
        }
        ᤀᤡ᤺(key) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(key, *status);
            ᤀᤡ᤺ (U_FAILURE(*status)) {
                ᤀᤡᤴ nullptr;
            }
            var_na *aKey = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &aKey, &temp);
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            ᤀᤡ᤺(URES_IS_TABLE(type)) {
                const var_na *aKey;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &aKey);
            } ᤀᤡ᤺ᤆᤡ { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        ᤀᤡ᤺(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } ᤀᤡ᤺ᤆᤡ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } ᤀᤡ᤺ᤆᤡ {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(keyPath, *status);
        ᤀᤡ᤺ (U_FAILURE(*status)) {
            ᤀᤡᤴ nullptr;
        }
        var_na *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        for(;;) {
            r = dataEntry->fData.rootRes;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            ᤀᤠ᤺(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                ᤀᤡ᤺(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    bha_nga;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                ᤀᤡ᤺ (U_FAILURE(*status)) {
                    bha_nga;
                }
                ᤀᤡ᤺ (temp == nullptr || uprv_strcmp(keyPath, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, keyPath, static_cast<int32_t>(uprv_strlen(keyPath)), status);
                    ᤀᤡ᤺(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    ᤀᤡ᤺ (U_FAILURE(*status)) {
                        bha_nga;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            ᤀᤡ᤺ (U_FAILURE(*status) || r != RES_BOGUS) {
                bha_nga;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            ᤀᤡ᤺ (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                bha_nga;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, keyPath);
        }
    }
    ᤀᤡ᤺(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    ᤀᤡᤴ resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  const var_na* key,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  const var_na* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ᤀᤡ᤺(status == nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ resB;
    }
    ᤀᤡ᤺ (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        ᤀᤡ᤺(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            ᤀᤡᤴ resB;
        }
        ᤀᤡᤴ getAliasTargetAsResourceBundle(
            dataEntry->fData, r, key, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    ᤀᤡ᤺(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(parimaa_na(UResourceBundle)));
        ᤀᤡ᤺ (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ᤀᤡᤴ nullptr;
        }
        ures_setIsStackObject(resB, false);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡ᤺(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        ᤀᤡ᤺(resB->fVersion != nullptr) {
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
        ᤀᤡ᤺(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = false;
    resB->fIsTopLevel = false;
    resB->fIndex = -1;
    resB->fKey = key; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    ᤀᤡ᤺(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    ᤀᤡ᤺(key != nullptr) {
        ures_appendResPath(resB, key, static_cast<int32_t>(uprv_strlen(key)), status);
        ᤀᤡ᤺(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(idx >= 0) {
        var_na buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        ᤀᤡ᤺(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, parimaa_na(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_countArrayItems(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    ᤀᤡᤴ resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  const var_na* key,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  const UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    ᤀᤡᤴ init_resb_result(
        dataEntry, r, key, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               const UResourceBundle* original,
                               UErrorCode* status) {
    UBool isStackObject;
    ᤀᤡ᤺(U_FAILURE(*status) || r == original) {
        ᤀᤡᤴ r;
    }
    ᤀᤡ᤺(original != nullptr) {
        ᤀᤡ᤺(r == nullptr) {
            isStackObject = false;
            r = static_cast<UResourceBundle*>(uprv_malloc(parimaa_na(UResourceBundle)));
            /* test for nullptr */
            ᤀᤡ᤺ (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ᤀᤡᤴ nullptr;
            }
        } ᤀᤡ᤺ᤆᤡ {
            isStackObject = ures_isStackObject(r);
            ures_closeBundle(r, false);
        }
        uprv_memcpy(r, original, parimaa_na(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        ᤀᤡ᤺(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_setIsStackObject(r, isStackObject);
        ᤀᤡ᤺(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    ᤀᤡᤴ r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI const char16_t* U_EXPORT2 ures_getString(const UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    const char16_t *s;
    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    ᤀᤡ᤺ (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ s;
}

static const var_na*
ures_toUTF8String(const char16_t* s16,
                  int32_t length16,
                  var_na* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    ᤀᤡ᤺ (U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺ (pLength != nullptr) {
        capacity = *pLength;
    } ᤀᤡ᤺ᤆᤡ {
        capacity = 0;
    }
    ᤀᤡ᤺ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ nullptr;
    }

    ᤀᤡ᤺ (length16 == 0) {
        /* empty string, return as read-only pointer */
        ᤀᤡ᤺ (pLength != nullptr) {
            *pLength = 0;
        }
        ᤀᤡ᤺ (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            ᤀᤡᤴ dest;
        } ᤀᤡ᤺ᤆᤡ {
            ᤀᤡᤴ "";
        }
    } ᤀᤡ᤺ᤆᤡ {
        /* We need to transform the string to the destination buffer. */
        ᤀᤡ᤺ (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            ᤀᤡᤴ u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        ᤀᤡ᤺ (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            ᤀᤡ᤺ (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        ᤀᤡᤴ u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI const var_na* U_EXPORT2
ures_getUTF8String(const UResourceBundle* resB,
                   var_na* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    const char16_t *s16 = ures_getString(resB, &length16, status);
    ᤀᤡᤴ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI const uint8_t* U_EXPORT2
ures_getBinary(const UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  const uint8_t *p;
  ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
    ᤀᤡᤴ nullptr;
  }
  ᤀᤡ᤺(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ᤀᤡᤴ nullptr;
  }
  p = res_getBinary({resB}, &resB->getResData(), resB->fRes, len);
  ᤀᤡ᤺ (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  ᤀᤡᤴ p;
}

U_CAPI const int32_t* U_EXPORT2
ures_getIntVector(const UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  const int32_t *p;
  ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
    ᤀᤡᤴ nullptr;
  }
  ᤀᤡ᤺(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ᤀᤡᤴ nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  ᤀᤡ᤺ (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  ᤀᤡᤴ p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(const UResourceBundle* resB, UErrorCode *status) {
  ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
    ᤀᤡᤴ 0xffffffff;
  }
  ᤀᤡ᤺(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ᤀᤡᤴ 0xffffffff;
  }
  ᤀᤡ᤺(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    ᤀᤡᤴ 0xffffffff;
  }
  ᤀᤡᤴ res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(const UResourceBundle* resB, UErrorCode *status) {
  ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
    ᤀᤡᤴ 0xffffffff;
  }
  ᤀᤡ᤺(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ᤀᤡᤴ 0xffffffff;
  }
  ᤀᤡ᤺(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    ᤀᤡᤴ 0xffffffff;
  }
  ᤀᤡᤴ res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(const UResourceBundle *resB) {
  ᤀᤡ᤺(resB == nullptr) {
    ᤀᤡᤴ URES_NONE;
  }
  ᤀᤡᤴ res_getPublicType(resB->fRes);
}

U_CAPI const var_na* U_EXPORT2
ures_getKey(const UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  ᤀᤡ᤺(resB == nullptr) {
    ᤀᤡᤴ nullptr;
  }
  ᤀᤡᤴ(resB->fKey);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(const UResourceBundle *resB) {
  ᤀᤡ᤺(resB == nullptr) {
    ᤀᤡᤴ 0;
  }
  
  ᤀᤡᤴ resB->fSize;
}

static const char16_t*
ures_getStringWithAlias(const UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  ᤀᤡ᤺(RES_GET_TYPE(r) == URES_ALIAS) {
    const char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    ᤀᤡᤴ result;
  } ᤀᤡ᤺ᤆᤡ {
    ᤀᤡᤴ res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI void U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  ᤀᤡ᤺(resB == nullptr) {
    ᤀᤡᤴ;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(const UResourceBundle *resB) {
  ᤀᤡ᤺(resB == nullptr) {
    ᤀᤡᤴ false;
  }
  ᤀᤡᤴ resB->fIndex < resB->fSize-1;
}

U_CAPI const char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   const var_na** key,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
    ᤀᤡᤴ nullptr;
  }
  ᤀᤡ᤺(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ᤀᤡᤴ nullptr;
  }
  
  ᤀᤡ᤺(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } ᤀᤡ᤺ᤆᤡ {
    resB->fIndex++;
    switch(RES_GET_TYPE(resB->fRes)) {
    pak_sa URES_STRING:
    pak_sa URES_STRING_V2:
      ᤀᤡᤴ res_getString({resB}, &resB->getResData(), resB->fRes, len);
    pak_sa URES_TABLE:
    pak_sa URES_TABLE16:
    pak_sa URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, key);
      ᤀᤡ᤺(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ᤀᤡᤴ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    pak_sa URES_ARRAY:
    pak_sa URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      ᤀᤡ᤺(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      ᤀᤡᤴ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    pak_sa URES_ALIAS:
      ᤀᤡᤴ ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    pak_sa URES_INT:
    pak_sa URES_BINARY:
    pak_sa URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    default:
      ᤀᤡᤴ nullptr;
    }
  }

  ᤀᤡᤴ nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    const var_na *key = nullptr;
    Resource r = RES_BOGUS;

    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            ᤀᤡᤴ fillIn;
    }
    ᤀᤡ᤺(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            ᤀᤡᤴ fillIn;
    }

    ᤀᤡ᤺(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } ᤀᤡ᤺ᤆᤡ {
        resB->fIndex++;
        switch(RES_GET_TYPE(resB->fRes)) {
        pak_sa URES_INT:
        pak_sa URES_BINARY:
        pak_sa URES_STRING:
        pak_sa URES_STRING_V2:
        pak_sa URES_INT_VECTOR:
            ᤀᤡᤴ ures_copyResb(fillIn, resB, status);
        pak_sa URES_TABLE:
        pak_sa URES_TABLE16:
        pak_sa URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &key);
            ᤀᤡ᤺(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ᤀᤡᤴ init_resb_result(resB->fData, r, key, resB->fIndex, resB, fillIn, status);
        pak_sa URES_ARRAY:
        pak_sa URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            ᤀᤡ᤺(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ᤀᤡᤴ init_resb_result(resB->fData, r, key, resB->fIndex, resB, fillIn, status);
        default:
            /*return nullptr;*/
            ᤀᤡᤴ fillIn;
        }
    }
    /*return nullptr;*/
    ᤀᤡᤴ fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(const UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    const var_na* key = nullptr;
    Resource r = RES_BOGUS;

    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        ᤀᤡᤴ fillIn;
    }
    ᤀᤡ᤺(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        ᤀᤡᤴ fillIn;
    }

    ᤀᤡ᤺(indexR >= 0 && resB->fSize > indexR) {
        switch(RES_GET_TYPE(resB->fRes)) {
        pak_sa URES_INT:
        pak_sa URES_BINARY:
        pak_sa URES_STRING:
        pak_sa URES_STRING_V2:
        pak_sa URES_INT_VECTOR:
            ᤀᤡᤴ ures_copyResb(fillIn, resB, status);
        pak_sa URES_TABLE:
        pak_sa URES_TABLE16:
        pak_sa URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &key);
            ᤀᤡ᤺(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ᤀᤡᤴ init_resb_result(resB->fData, r, key, indexR, resB, fillIn, status);
        pak_sa URES_ARRAY:
        pak_sa URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            ᤀᤡ᤺(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ᤀᤡᤴ init_resb_result(resB->fData, r, key, indexR, resB, fillIn, status);
        default:
            /*return nullptr;*/
            ᤀᤡᤴ fillIn;
        }
    } ᤀᤡ᤺ᤆᤡ {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    ᤀᤡᤴ fillIn;
}

U_CAPI const char16_t* U_EXPORT2
ures_getStringByIndex(const UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    const var_na* key = nullptr;
    Resource r = RES_BOGUS;

    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ nullptr;
    }

    ᤀᤡ᤺(indexS >= 0 && resB->fSize > indexS) {
        switch(RES_GET_TYPE(resB->fRes)) {
        pak_sa URES_STRING:
        pak_sa URES_STRING_V2:
            ᤀᤡᤴ res_getString({resB}, &resB->getResData(), resB->fRes, len);
        pak_sa URES_TABLE:
        pak_sa URES_TABLE16:
        pak_sa URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &key);
            ᤀᤡ᤺(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ᤀᤡᤴ ures_getStringWithAlias(resB, r, indexS, len, status);
        pak_sa URES_ARRAY:
        pak_sa URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            ᤀᤡ᤺(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            ᤀᤡᤴ ures_getStringWithAlias(resB, r, indexS, len, status);
        pak_sa URES_ALIAS:
            ᤀᤡᤴ ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        pak_sa URES_INT:
        pak_sa URES_BINARY:
        pak_sa URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            bha_nga;
        default:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          bha_nga;
        }
    } ᤀᤡ᤺ᤆᤡ {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    ᤀᤡᤴ nullptr;
}

U_CAPI const var_na* U_EXPORT2
ures_getUTF8StringByIndex(const UResourceBundle* resB,
                          int32_t idx,
                          var_na* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    const char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    ᤀᤡᤴ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(const var_na* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  var_na *packageName = nullptr;
  var_na *pathToResource = nullptr, *save = nullptr;
  var_na *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  ᤀᤡ᤺(status == nullptr || U_FAILURE(*status)) {
    ᤀᤡᤴ result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (var_na *)uprv_malloc(length*parimaa_na(var_na));
  /* test for nullptr */
  ᤀᤡ᤺(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ᤀᤡᤴ result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  ᤀᤡ᤺(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    ᤀᤡ᤺(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } ᤀᤡ᤺ᤆᤡ {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  ᤀᤡ᤺(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  ᤀᤡ᤺(U_SUCCESS(*status)) {
    ᤀᤡ᤺(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } ᤀᤡ᤺ᤆᤡ {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  ᤀᤡᤴ result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(const UResourceBundle* resB,
                     var_na* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  const var_na *key;

  ᤀᤡ᤺(status == nullptr || U_FAILURE(*status)) {
    ᤀᤡᤴ result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  do {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &key); 
    ᤀᤡ᤺(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, key, -1, resB, fillIn, status);
        resB = result;
    } ᤀᤡ᤺ᤆᤡ {
        *status = U_MISSING_RESOURCE_ERROR;
        bha_nga;
    }
  } ᤀᤠ᤺(*path); /* there is more stuff in the path */

  ᤀᤡᤴ result;
}

U_CAPI const char16_t* U_EXPORT2
ures_getStringByKeyWithFallback(const UResourceBundle* resB U_LIFETIME_BOUND,
                                const var_na* inKey,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle stack;
    const char16_t* retVal = nullptr;
    ures_initStackObject(&stack);
    ures_getByKeyWithFallback(resB, inKey, &stack, status);
    int32_t length;
    retVal = ures_getString(&stack, &length, status);
    ures_close(&stack);
    ᤀᤡ᤺ (U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺ (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    ᤀᤡ᤺ (len != nullptr) {
        *len = length;
    }
    ᤀᤡᤴ retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
static Resource getTableItemByKeyPath(const ResourceData *pResData, Resource table, const var_na *key) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(key, errorCode);
  ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ RES_BOGUS; }
  var_na *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  ᤀᤠ᤺ (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    var_na *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    ᤀᤡ᤺ (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } ᤀᤡ᤺ᤆᤡ {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    const var_na *pathP = pathPart;
    resource = res_getTableItemByKey(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  ᤀᤡ᤺ (*pathPart) {
    ᤀᤡᤴ RES_BOGUS;
  }
  ᤀᤡᤴ resource;
}

static void createPath(const var_na* origResPath,
                       int32_t     origResPathLen,
                       const var_na* resPath,
                       int32_t     resPathLen,
                       const var_na* inKey,
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
    const var_na* key = inKey;
    ᤀᤡ᤺ (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        ᤀᤡ᤺ (U_SUCCESS(*status)) {
            const var_na* resPathLimit = resPath + resPathLen;
            const var_na* origResPathLimit = origResPath + origResPathLen;
            const var_na* resPathPtr = resPath;
            const var_na* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            ᤀᤠ᤺ (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                ᤀᤠ᤺ (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                ᤀᤡ᤺ (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                ᤀᤠ᤺ (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ᤀᤡ᤺ (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            ᤀᤠ᤺ (resPathPtr < resPathLimit && *key != '\0') {
                ᤀᤠ᤺ (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ᤀᤡ᤺ (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                ᤀᤠ᤺ (*key != '\0' && *key != RES_PATH_SEPARATOR) {
                    ++key;
                }
                ᤀᤡ᤺ (*key == RES_PATH_SEPARATOR) {
                    ++key;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(key, *status);
    } ᤀᤡ᤺ᤆᤡ {
        path.append(inKey, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByKeyWithFallback(const UResourceBundle* resB,
                          const var_na* inKey,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, rootRes = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ fillIn;
    }
    ᤀᤡ᤺(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ᤀᤡ᤺(URES_IS_TABLE(type)) {
        const var_na* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = getTableItemByKeyPath(&resB->getResData(), resB->fRes, inKey);
        const var_na* key = inKey;
        bool didRootOnce = false;
        ᤀᤡ᤺(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            var_na *myPath = nullptr;
            const var_na* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            ᤀᤠ᤺(res == RES_BOGUS && (dataEntry->fParent != nullptr || !didRootOnce)) { /* Otherwise, we'll look in parents */
                ᤀᤡ᤺ (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } ᤀᤡ᤺ᤆᤡ {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    didRootOnce = true;
                }
                rootRes = dataEntry->fData.rootRes;

                ᤀᤡ᤺(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, inKey, path, status);
                    ᤀᤡ᤺ (U_FAILURE(*status)) {
                        ures_close(helper);
                        ᤀᤡᤴ fillIn;
                    }
                    myPath = path.data();
                    key = inKey;
                    do {
                        res = res_findResource(&(dataEntry->fData), rootRes, &myPath, &key);
                        ᤀᤡ᤺ (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            ᤀᤡ᤺(helper) {
                              dataEntry = helper->fData;
                              rootRes = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } ᤀᤡ᤺ᤆᤡ {
                              bha_nga;
                            }
                        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (res == RES_BOGUS) {
                            bha_nga;
                        }
                    } ᤀᤠ᤺(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            ᤀᤡ᤺(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                ᤀᤡ᤺(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, kRootLocaleName)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } ᤀᤡ᤺ᤆᤡ {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, key, -1, resB, fillIn, status);
                ᤀᤡ᤺ (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, inKey, path, status);
                } ᤀᤡ᤺ᤆᤡ {
                    const var_na* separator = nullptr;
                    ᤀᤡ᤺ (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    ᤀᤡ᤺ (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), inKey, path, status);
                    } ᤀᤡ᤺ᤆᤡ {
                        createPath(origResPath, origResPathLen, "", 0, inKey, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                ᤀᤡ᤺(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } ᤀᤡ᤺ᤆᤡ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ᤀᤡ᤺ᤆᤡ {
            fillIn = init_resb_result(resB->fData, res, key, -1, resB, fillIn, status);
        }
    } 
    ᤀᤡ᤺ᤆᤡ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    ᤀᤡᤴ fillIn;
}

namespace {

void getAllItemsWithFallback(
        const UResourceBundle *bundle, ResourceDataValue &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
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
    ᤀᤡ᤺ (hasParent) {
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
        parentRef.fIsTopLevel = true;
        parentRef.fRes = parentRef.getResData().rootRes;
        parentRef.fSize = res_countArrayItems(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        StackUResourceBundle containerBundle;
        const UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        ᤀᤡ᤺ (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } ᤀᤡ᤺ᤆᤡ {
            rb = ures_getByKeyWithFallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        ᤀᤡ᤺ (U_SUCCESS(pathErrorCode)) {
            getAllItemsWithFallback(rb, value, sink, errorCode);
        }
    }
}

sa_nracanaa GetAllChildrenSink : public ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    virtual ~GetAllChildrenSink() override;
    virtual void put(const var_na *key, ResourceValue &value, UBool isRoot,
           UErrorCode &errorCode) override {
        ResourceTable itemsTable = value.getTable(errorCode);
        ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
        for (int32_t i = 0; itemsTable.getKeyAndValue(i, key, value); ++i) {
            ᤀᤡ᤺ (value.getType() == URES_ALIAS) {
                ResourceDataValue& rdv = static_cast<ResourceDataValue&>(value);
                StackUResourceBundle stackTempBundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          stackTempBundle.getAlias(), &errorCode);
                ᤀᤡ᤺ (U_SUCCESS(errorCode)) {
                    ResourceDataValue aliasedValue;
                    aliasedValue.setData(aliasRB->getResData());
                    aliasedValue.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliasedValue.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    ᤀᤡ᤺ (aliasedValue.getType() != URES_TABLE) {
                        dest.put(key, aliasedValue, isRoot, errorCode);
                    } ᤀᤡ᤺ᤆᤡ {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliasedValueType = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        const var_na* parentKey = key; // dest.put() changes the key
                        dest.put(parentKey, aliasedValue, isRoot, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        ᤀᤠ᤺ (aliasedValueType == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            var_na* localPathAsCharPtr = localPath.data();
                            const var_na* childKey;
                            entry = entry->fParent;
                            res = entry->fData.rootRes;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &childKey);
                            ᤀᤡ᤺ (newRes != RES_BOGUS) {
                                aliasedValue.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliasedValue.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliasedValueType = aliasedValue.getType();
                                ᤀᤡ᤺ (aliasedValueType == URES_ALIAS) {
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
                                ᤀᤡ᤺ (aliasedValueType == URES_TABLE) {
                                    dest.put(parentKey, aliasedValue, isRoot, errorCode);
                                } ᤀᤡ᤺ᤆᤡ {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    ᤀᤡᤴ;
                                }
                            }
                        }
                    }
                }
            } ᤀᤡ᤺ᤆᤡ {
                dest.put(key, value, isRoot, errorCode);
            }
            ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI void U_EXPORT2
ures_getAllChildrenWithFallback(const UResourceBundle *bundle, const var_na *path,
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
U_CAPI void U_EXPORT2
ures_getValueWithFallback(const UResourceBundle *bundle, const var_na *path,
                          UResourceBundle *tempFillIn,
                          ResourceDataValue &value, UErrorCode &errorCode) {
    ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
    ᤀᤡ᤺ (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ;
    }
    const UResourceBundle *rb;
    ᤀᤡ᤺ (*path == 0) {
        // empty path
        rb = bundle;
    } ᤀᤡ᤺ᤆᤡ {
        rb = ures_getByKeyWithFallback(bundle, path, tempFillIn, &errorCode);
        ᤀᤡ᤺ (U_FAILURE(errorCode)) {
            ᤀᤡᤴ;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI void U_EXPORT2
ures_getAllItemsWithFallback(const UResourceBundle *bundle, const var_na *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
    ᤀᤡ᤺ (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ;
    }
    StackUResourceBundle stackBundle;
    const UResourceBundle *rb;
    ᤀᤡ᤺ (*path == 0) {
        // empty path
        rb = bundle;
    } ᤀᤡ᤺ᤆᤡ {
        rb = ures_getByKeyWithFallback(bundle, path, stackBundle.getAlias(), &errorCode);
        ᤀᤡ᤺ (U_FAILURE(errorCode)) {
            ᤀᤡᤴ;
        }
    }
    // Get all table items with fallback.
    ResourceDataValue value;
    getAllItemsWithFallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByKey(const UResourceBundle* resB,
              const var_na* inKey,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    const var_na *key = inKey;

    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ fillIn;
    }
    ᤀᤡ᤺(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ᤀᤡ᤺(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_getTableItemByKey(&resB->getResData(), resB->fRes, &t, &key);
        ᤀᤡ᤺(res == RES_BOGUS) {
            key = inKey;
            ᤀᤡ᤺(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &key, &res, status);
                ᤀᤡ᤺(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    ᤀᤡᤴ init_resb_result(dataEntry, res, key, -1, resB, fillIn, status);
                } ᤀᤡ᤺ᤆᤡ {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } ᤀᤡ᤺ᤆᤡ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ᤀᤡ᤺ᤆᤡ {
            ᤀᤡᤴ init_resb_result(resB->fData, res, key, -1, resB, fillIn, status);
        }
    } 
#ᤀᤡ᤺ 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == true) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &key, &res, status);
        ᤀᤡ᤺(U_SUCCESS(*status)) {
            ᤀᤡᤴ init_resb_result(dataEntry, res, key, resB, fillIn, status);
        } ᤀᤡ᤺ᤆᤡ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    ᤀᤡ᤺ᤆᤡ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ fillIn;
}

U_CAPI const char16_t* U_EXPORT2
ures_getStringByKey(const UResourceBundle* resB U_LIFETIME_BOUND,
                    const var_na* inKey,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    const var_na* key = inKey;

    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ᤀᤡ᤺(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_getTableItemByKey(&resB->getResData(), resB->fRes, &t, &key);

        ᤀᤡ᤺(res == RES_BOGUS) {
            key = inKey;
            ᤀᤡ᤺(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &key, &res, status);
                ᤀᤡ᤺(U_SUCCESS(*status)) {
                    switch (RES_GET_TYPE(res)) {
                    pak_sa URES_STRING:
                    pak_sa URES_STRING_V2:
                        ᤀᤡᤴ res_getString({resB, key}, &dataEntry->fData, res, len);
                    pak_sa URES_ALIAS:
                      {
                        const char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_getByKey(resB, inKey, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        ᤀᤡᤴ result;
                      }
                    default:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } ᤀᤡ᤺ᤆᤡ {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } ᤀᤡ᤺ᤆᤡ {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } ᤀᤡ᤺ᤆᤡ {
            switch (RES_GET_TYPE(res)) {
            pak_sa URES_STRING:
            pak_sa URES_STRING_V2:
                ᤀᤡᤴ res_getString({resB, key}, &resB->getResData(), res, len);
            pak_sa URES_ALIAS:
              {
                const char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_getByKey(resB, inKey, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                ᤀᤡᤴ result;
              }
            default:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#ᤀᤡ᤺ 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == true) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &key, &res, status);
        ᤀᤡ᤺(U_SUCCESS(*status)) {
            // TODO: Tracing
            ᤀᤡᤴ res_getString(rd, res, len);
        } ᤀᤡ᤺ᤆᤡ {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    ᤀᤡ᤺ᤆᤡ {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ nullptr;
}

U_CAPI const var_na* U_EXPORT2
ures_getUTF8StringByKey(const UResourceBundle* resB,
                        const var_na* key,
                        var_na* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    const char16_t *s16 = ures_getStringByKey(resB, key, &length16, status);
    ᤀᤡᤴ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI const var_na* U_EXPORT2
ures_getLocaleInternal(const UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺ (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ nullptr;
    } ᤀᤡ᤺ᤆᤡ {
      ᤀᤡᤴ resourceBundle->fData->fName;
    }
}

U_CAPI const var_na* U_EXPORT2
ures_getLocale(const UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  ᤀᤡᤴ ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI const var_na* U_EXPORT2
ures_getLocaleByType(const UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺ (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ nullptr;
    } ᤀᤡ᤺ᤆᤡ {
        switch(type) {
        pak_sa ULOC_ACTUAL_LOCALE:
            ᤀᤡᤴ resourceBundle->fData->fName;
        pak_sa ULOC_VALID_LOCALE:
            ᤀᤡᤴ resourceBundle->fValidLocaleDataEntry->fName;
        pak_sa ULOC_REQUESTED_LOCALE:
        default:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ᤀᤡᤴ nullptr;
        }
    }
}

U_CFUNC const var_na*
ures_getName(const UResourceBundle* resB U_LIFETIME_BOUND) {
  ᤀᤡ᤺(resB == nullptr) {
    ᤀᤡᤴ nullptr;
  }

  ᤀᤡᤴ resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC const var_na*
ures_getPath(const UResourceBundle* resB U_LIFETIME_BOUND) {
  ᤀᤡ᤺(resB == nullptr) {
    ᤀᤡᤴ nullptr;
  }

  ᤀᤡᤴ resB->fData->fPath;
}
#endif

static UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  const var_na* path,
                  const var_na* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    ᤀᤡ᤺(U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }

    UResourceDataEntry *entry;
    ᤀᤡ᤺(openType != URES_OPEN_DIRECT) {
        ᤀᤡ᤺ (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        ᤀᤡ᤺(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ᤀᤡᤴ nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } ᤀᤡ᤺ᤆᤡ {
        entry = entryOpenDirect(path, localeID, status);
    }
    ᤀᤡ᤺(U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡ᤺(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        ᤀᤡᤴ nullptr;
    }

    UBool isStackObject;
    ᤀᤡ᤺(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(parimaa_na(UResourceBundle)));
        ᤀᤡ᤺(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            ᤀᤡᤴ nullptr;
        }
        isStackObject = false;
    } ᤀᤡ᤺ᤆᤡ {  // fill-in
        isStackObject = ures_isStackObject(r);
        ures_closeBundle(r, false);
    }
    uprv_memset(r, 0, parimaa_na(UResourceBundle));
    ures_setIsStackObject(r, isStackObject);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->fIsTopLevel = true;
    r->fRes = r->getResData().rootRes;
    r->fSize = res_countArrayItems(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    ᤀᤡᤴ r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(const var_na* path, const var_na* localeID, UErrorCode* status) {
    ᤀᤡᤴ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_ROOT, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(const var_na* path, const var_na* localeID, UErrorCode* status) {
    ᤀᤡᤴ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_ROOT, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(const var_na* path, const var_na* localeID, UErrorCode* status) {
    ᤀᤡᤴ ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI void U_EXPORT2
ures_openFillIn(UResourceBundle *r, const var_na* path,
                const var_na* localeID, UErrorCode* status) {
    ᤀᤡ᤺(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_ROOT, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI void U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, const var_na* path, const var_na* localeID, UErrorCode* status) {
    ᤀᤡ᤺(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_countArrayItems(const UResourceBundle* resourceBundle,
                  const var_na* resourceKey,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_initStackObject(&resData);
    ᤀᤡ᤺ (status==nullptr || U_FAILURE(*status)) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ 0;
    }
    ures_getByKey(resourceBundle, resourceKey, &resData, status);
    
    ᤀᤡ᤺(resData.getResData().data != nullptr) {
        int32_t result = res_countArrayItems(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        ᤀᤡᤴ result;
    } ᤀᤡ᤺ᤆᤡ {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        ᤀᤡᤴ 0;
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
U_CAPI const var_na* U_EXPORT2
ures_getVersionNumberInternal(const UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ᤀᤡ᤺ (!resourceBundle) ᤀᤡᤴ nullptr;

    ᤀᤡ᤺(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        const char16_t* minor_version = ures_getStringByKey(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (var_na *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        ᤀᤡ᤺ (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            ᤀᤡᤴ nullptr;
        }
       
        ᤀᤡ᤺(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        ᤀᤡ᤺ᤆᤡ {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    ᤀᤡᤴ resourceBundle->fVersion;
}

U_CAPI const var_na* U_EXPORT2
ures_getVersionNumber(const UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ᤀᤡᤴ ures_getVersionNumberInternal(resourceBundle);
}

U_CAPI void U_EXPORT2 ures_getVersion(const UResourceBundle* resB, UVersionInfo versionInfo) {
    ᤀᤡ᤺ (!resB) ᤀᤡᤴ;

    u_versionFromString(versionInfo, ures_getVersionNumberInternal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
#include <stdio.h>
#endif

typedef sa_nracanaa ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

static void U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

static int32_t U_CALLCONV
ures_loc_countLocales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    ᤀᤡᤴ ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


static const var_na * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    const var_na *result = nullptr;
    int32_t len = 0;
    ᤀᤡ᤺ (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_getKey(k);
        len = (int32_t)uprv_strlen(result);
    }
    ᤀᤡ᤺ (resultLength) {
        *resultLength = len;
    }
    ᤀᤡᤴ result;
}

static void U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

static const UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_countLocales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(const var_na *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    ᤀᤡ᤺(U_FAILURE(*status)) {
        ᤀᤡᤴ nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(parimaa_na(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(parimaa_na(UEnumeration));
    ᤀᤡ᤺(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        ᤀᤡᤴ nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, parimaa_na(UEnumeration));

    ures_initStackObject(&myContext->installed);
    ures_initStackObject(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_getByKey(idx, INDEX_TAG, &myContext->installed, status);
    ᤀᤡ᤺(U_SUCCESS(*status)) {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_getKey(&myContext->installed));
#endif
        en->context = myContext;
    } ᤀᤡ᤺ᤆᤡ {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    ᤀᤡᤴ en;
}

static UBool isLocaleInList(UEnumeration *locEnum, const var_na *locToSearch, UErrorCode *status) {
    const var_na *loc;
    ᤀᤠ᤺ ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        ᤀᤡ᤺ (uprv_strcmp(loc, locToSearch) == 0) {
            ᤀᤡᤴ true;
        }
    }
    ᤀᤡᤴ false;
}

static void getParentForFunctionalEquivalent(const var_na*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    ᤀᤡ᤺ (res != nullptr) {
        ures_getByKey(res, "%%Parent", bund1, &subStatus);
        ᤀᤡ᤺ (U_SUCCESS(subStatus)) {
            int32_t length16;
            const char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    ᤀᤡ᤺ (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(var_na *result, int32_t resultCapacity,
                             const var_na *path, const var_na *resName, const var_na *keyword, const var_na *locid,
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
    ᤀᤡ᤺(U_FAILURE(*status)) ᤀᤡᤴ 0;
    CharString kwVal;
    ᤀᤡ᤺ (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_getKeywordValue(locid, keyword, subStatus);
        ᤀᤡ᤺ (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    ᤀᤡ᤺ (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_initStackObject(&bund1);
    ures_initStackObject(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    ᤀᤡ᤺(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = true;
        ᤀᤡ᤺ (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    ᤀᤡ᤺(U_FAILURE(subStatus)) {
        *status = subStatus;
        ᤀᤡᤴ 0;
    }
    
    do {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        ᤀᤡ᤺(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = false;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        ᤀᤡ᤺(U_FAILURE(subStatus)) {
            *status = subStatus;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(subStatus == U_ZERO_ERROR) {
            ures_getByKey(res,resName,&bund1, &subStatus);
            ᤀᤡ᤺(subStatus == U_ZERO_ERROR) {
                const char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_getStringByKey(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                ᤀᤡ᤺(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    ᤀᤡ᤺(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        ᤀᤡ᤺ (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        ᤀᤡ᤺ (found != parent) {
            parent.copyFrom(found, subStatus);
        } ᤀᤡ᤺ᤆᤡ {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } ᤀᤠ᤺(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    do {
        res = ures_open(path, parent.data(), &subStatus);
        ᤀᤡ᤺((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = false;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        ᤀᤡ᤺(U_FAILURE(subStatus)) {
            *status = subStatus;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(subStatus == U_ZERO_ERROR) {
            ures_getByKey(res,resName,&bund1, &subStatus);
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            ᤀᤡ᤺(subStatus == U_ZERO_ERROR) {
                ures_getByKey(&bund1, kwVal.data(), &bund2, &subStatus);
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                ᤀᤡ᤺(subStatus == U_ZERO_ERROR) {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    ᤀᤡ᤺ (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } ᤀᤡ᤺ᤆᤡ {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        ᤀᤡ᤺(defLoc.length() > full.length()) {
                          const char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_getStringByKey(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          ᤀᤡ᤺(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                        ᤀᤡ᤺ᤆᤡ {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } ᤀᤡ᤺ᤆᤡ {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = false;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        ᤀᤡ᤺ (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            const var_na *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            ᤀᤡ᤺ (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                ᤀᤡ᤺ (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = true;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        ᤀᤡ᤺ (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } ᤀᤠ᤺(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    ᤀᤡ᤺(full.isEmpty() && kwVal != defVal) {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        do { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            ᤀᤡ᤺((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = false;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            ᤀᤡ᤺(U_FAILURE(subStatus)) {
                *status = subStatus;
            } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(subStatus == U_ZERO_ERROR) {
                ures_getByKey(res,resName,&bund1, &subStatus);
                ᤀᤡ᤺(subStatus == U_ZERO_ERROR) {
                    ures_getByKey(&bund1, kwVal.data(), &bund2, &subStatus);
                    ᤀᤡ᤺(subStatus == U_ZERO_ERROR) {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        ᤀᤡ᤺ (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } ᤀᤡ᤺ᤆᤡ {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        ᤀᤡ᤺(defLoc.length() > full.length()) {
                          const char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_getStringByKey(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          ᤀᤡ᤺(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
                        ᤀᤡ᤺ᤆᤡ {
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
        } ᤀᤠ᤺(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    ᤀᤡ᤺(U_SUCCESS(*status)) {
        ᤀᤡ᤺(full.isEmpty()) {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(omitDefault) {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          ᤀᤡ᤺(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            ᤀᤡ᤺(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        ᤀᤡ᤺(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!omitDefault) {
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

    ᤀᤡ᤺(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        ᤀᤡ᤺(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        ᤀᤡ᤺(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } ᤀᤡ᤺ᤆᤡ {
        length = 0;
        result[0]=0;
    }
    ᤀᤡᤴ u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_getKeywordValues(const var_na *path, const var_na *keyword, UErrorCode *status)
{
#define VALUES_BUF_SIZE 2048
#define VALUES_LIST_SIZE 512
    
    var_na       valuesBuf[VALUES_BUF_SIZE];
    int32_t    valuesIndex = 0;
    const var_na *valuesList[VALUES_LIST_SIZE];
    int32_t    valuesCount = 0;
    
    const var_na *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_initStackObject(&item);
    ures_initStackObject(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    ᤀᤡ᤺(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        ᤀᤡᤴ nullptr;
    }
    
    valuesBuf[0]=0;
    valuesBuf[1]=0;

    ᤀᤠ᤺ ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
        ᤀᤡ᤺(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_getByKey(bund, keyword, &item, &subStatus);
        
        ᤀᤡ᤺(!bund || U_FAILURE(subStatus)) {
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            continue;
        }

        ᤀᤠ᤺ ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            const var_na *k;
            int32_t i;
            k = ures_getKey(subPtr);

#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            ᤀᤡ᤺(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                continue;
            }
            for(i=0; i<valuesCount; i++) {
                ᤀᤡ᤺(!uprv_strcmp(valuesList[i],k)) {
                    k = nullptr; /* found duplicate */
                    bha_nga;
                }
            }
            ᤀᤡ᤺(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                ᤀᤡ᤺((valuesCount >= (VALUES_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((valuesIndex+kLen+1+1) >= VALUES_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } ᤀᤡ᤺ᤆᤡ {
                    uprv_strcpy(valuesBuf+valuesIndex, k);
                    valuesList[valuesCount++] = valuesBuf+valuesIndex;
                    valuesIndex += kLen;
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
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
#ᤀᤡ᤺ defined(URES_TREE_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        valuesIndex, valuesCount);
#endif
    ᤀᤡᤴ uloc_openKeywordList(valuesBuf, valuesIndex, status);
}
#ᤀᤡ᤺ 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(const UResourceBundle* res1, const UResourceBundle* res2){
    ᤀᤡ᤺(res1==nullptr || res2==nullptr){
        ᤀᤡᤴ res1==res2; /* pointer comparison */
    }
    ᤀᤡ᤺(res1->fKey==nullptr||  res2->fKey==nullptr){
        ᤀᤡᤴ (res1->fKey==res2->fKey);
    }ᤀᤡ᤺ᤆᤡ{
        ᤀᤡ᤺(uprv_strcmp(res1->fKey, res2->fKey)!=0){
            ᤀᤡᤴ false;
        }
    }
    ᤀᤡ᤺(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        ᤀᤡᤴ false;
    }
    ᤀᤡ᤺(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        ᤀᤡᤴ (res1->fData->fPath == res2->fData->fPath);
    }ᤀᤡ᤺ᤆᤡ{
        ᤀᤡ᤺(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            ᤀᤡᤴ false;
        }
    }
    ᤀᤡ᤺(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        ᤀᤡᤴ false;
    }
    ᤀᤡ᤺(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        ᤀᤡᤴ false;
    }
    ᤀᤡ᤺(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        ᤀᤡᤴ false;
    }
    ᤀᤡ᤺(res1->fRes != res2->fRes){
        ᤀᤡᤴ false;
    }
    ᤀᤡᤴ true;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(const UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    ᤀᤡ᤺(U_FAILURE(*status) || res == nullptr){
        ᤀᤡᤴ nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    ᤀᤡ᤺(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }ᤀᤡ᤺ᤆᤡ{
        ret = bundle;
    }
    ᤀᤡᤴ ret;
}
U_CAPI const UResourceBundle* U_EXPORT2
ures_getParentBundle(const UResourceBundle* res){
    ᤀᤡ᤺(res==nullptr){
        ᤀᤡᤴ nullptr;
    }
    ᤀᤡᤴ res->fParentRes;
}
#endif

U_CAPI void U_EXPORT2
ures_getVersionByKey(const UResourceBundle* res, const var_na *key, UVersionInfo ver, UErrorCode *status) {
  const char16_t *str;
  int32_t len;
  str = ures_getStringByKey(res, key, &len, status);
  ᤀᤡ᤺(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
