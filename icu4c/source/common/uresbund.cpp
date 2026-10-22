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

#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "cwchar.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "unicode/uenum.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uresdata.h"

using नामाकाश icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
स्थिर UHashtable *cache = nullptr;
स्थिर icu::UInitOnce gCacheInitOnce {};

स्थिर UMutex resbMutex;

/* INTERNAL: hashes an entry  */
स्थिर int32_t U_CALLCONV hashEntry(स्थिरांक UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    परत uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
स्थिर UBool U_CALLCONV compareEntries(स्थिरांक UHashTok p1, स्थिरांक UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    परत uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
स्थिर UBool chopLocale(अक्षर *name) {
    अक्षर *i = uprv_strrchr(name, '_');

    जर(i != nullptr) {
        *i = '\0';
        परत सत्य;
    }

    परत असत्य;
}

स्थिर UBool hasVariant(स्थिरांक अक्षर* localeID) {
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
    परत sink.संख्या_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#अन्तर्भाव "localefallback_data.h"

स्थिर स्थिरांक अक्षर* performFallbackLookup(स्थिरांक अक्षर* किल्ली,
                                         स्थिरांक अक्षर* किल्ली_Strs,
                                         स्थिरांक अक्षर* मूल्य_Strs U_LIFETIME_BOUND,
                                         स्थिरांक int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    स्थिरांक int32_t* bottom = lookupTable;
    स्थिरांक int32_t* शीर्ष = lookupTable + lookupTableLength;

    पर्यंत (bottom < शीर्ष) {
        // Effectively, divide by 2 and round down to an even index
        स्थिरांक int32_t* मधला = bottom + (((शीर्ष - bottom) / 4) * 2);
        स्थिरांक अक्षर* entry_किल्ली = &(किल्ली_Strs[*मधला]);
        int32_t strcmpResult = uprv_strcmp(किल्ली, entry_किल्ली);
        जर (strcmpResult == 0) {
            परत &(मूल्य_Strs[मधला[1]]);
        } अन्यथा जर (strcmpResult < 0) {
            शीर्ष = मधला;
        } अन्यथा {
            bottom = मधला + 2;
        }
    }
    परत nullptr;
}

स्थिर CharString getDefaultScript(स्थिरांक CharString& language, स्थिरांक CharString& region) {
    स्थिरांक अक्षर* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    जर (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        जर (U_FAILURE(err)) {
            परत result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    जर (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    जर (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    परत result;
}

गणना UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_मूळ,
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
    URES_OPEN_LOCALE_मूळ,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
प्रकारव्याख्या गणना UResOpenType UResOpenType;

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
स्थिर तार्किक getParentLocaleID(अक्षर *name, स्थिरांक अक्षर *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    माप_प्रकार nameLen = uprv_strlen(name);
    जर (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        परत chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    जर (U_FAILURE(err)) {
        // hopefully this never happens...
        परत chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    जर (openType == URES_OPEN_LOCALE_DEFAULT_मूळ) {
        स्थिरांक अक्षर* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        जर (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            परत सत्य;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    जर (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        जर (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } अन्यथा {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } अन्यथा जर (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        जर (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } अन्यथा {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } अन्यथा जर (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        जर (openType != URES_OPEN_LOCALE_DEFAULT_मूळ || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } अन्यथा {
            परत असत्य;
        }
    } अन्यथा {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        परत असत्य;
    }
    जर (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        परत सत्य;
    } अन्यथा {
        परत असत्य;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
स्थिर UBool mayHaveParent(अक्षर *name) {
    परत (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
स्थिर शून्य entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_मोजणी_Existing++;
    पर्यंत(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_मोजणी_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
स्थिर UResourceDataEntry *getFallbackData(
        स्थिरांक UResourceBundle *resBundle,
        स्थिरांक अक्षर **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    जर(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        परत nullptr;
    }
    जर(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_किल्ली(&(dataEntry->fData), dataEntry->fData.मूळ_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    जर(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        पर्यंत(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            जर(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_किल्ली(&(dataEntry->fData), dataEntry->fData.मूळ_Res, &indexR, resTag);
            }
        }
    }

    जर(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        परत nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    जर(i>1) {
        जर(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_मूळ_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } अन्यथा {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    परत dataEntry;
}

स्थिर शून्य
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    जर(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    जर(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    जर(entry->fPool != nullptr) {
        --entry->fPool->f_मोजणी_Existing;
    }
    alias = entry->fAlias;
    जर(alias != nullptr) {
        पर्यंत(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_मोजणी_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
स्थिर int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_अंक = 0;
    स्थिरांक UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    जर (cache == nullptr) {
        परत 0;
    }

    करा {
        deletedMore = असत्य;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        पर्यंत ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            जर (resB->f_मोजणी_Existing == 0) {
                rb_Deleted_अंक++;
                deletedMore = सत्य;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } पर्यंत(deletedMore);

    परत rb_Deleted_अंक;
}

#ifdef URES_DEBUG
#अन्तर्भाव <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = असत्य;
  int32_t pos = UHASH_FIRST;
  स्थिरांक UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    जर (cache == nullptr) {
      ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      परत असत्य;
    }

    पर्यंत ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=सत्य;
      resB = (UResourceDataEntry *) e->value.pointer;
      ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (शून्य*)resB, resB->f_मोजणी_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (शून्य*)resB->fPool,
              (शून्य*)resB->fAlias,
              (शून्य*)resB->fParent);       
    }
    
    ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_मोजणी(cache));
    परत cacheNotEmpty;
}

#endif

स्थिर UBool U_CALLCONV ures_cleanup()
{
    जर (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    परत सत्य;
}

/** INTERNAL: Initializes the cache for resources */
स्थिर शून्य U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
स्थिर शून्य initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

स्थिर शून्य setEntryName(UResourceDataEntry *res, स्थिरांक अक्षर *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    जर(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    जर (len < static_cast<int32_t>(आकार(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    अन्यथा {
        res->fName = static_cast<अक्षर*>(uprv_malloc(len + 1));
    }
    जर(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        uprv_strcpy(res->fName, name);
    }
}

स्थिर UResourceDataEntry *
getPoolEntry(स्थिरांक अक्षर *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
स्थिर UResourceDataEntry *init_entry(स्थिरांक अक्षर *localeID, स्थिरांक अक्षर *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    स्थिरांक अक्षर *name;
    अक्षर aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    जर(U_FAILURE(*status)) {
        परत nullptr;
    }

    /* here we try to deduce the right locale name */
    जर(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } अन्यथा जर(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_मूळ_Locale_Name;
    } अन्यथा { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<अक्षर*>(name);
    find.fPath = const_cast<अक्षर*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    जर(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(आकार(UResourceDataEntry)));
        जर(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            परत nullptr;
        }

        uprv_memset(r, 0, आकार(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        जर (U_FAILURE(*status)) {
            uprv_free(r);
            परत nullptr;
        }

        जर(path != nullptr) {
            r->fPath = uprv_strdup(path);
            जर(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                परत nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        जर (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            जर (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                परत nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } अन्यथा { /* if we have a regular entry */
            Resource aliasres;
            जर (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                जर (U_SUCCESS(*status)) {
                    स्थिरांक int32_t *poolIndexes = r->fPool->fData.p_मूळ + 1;
                    जर(r->fData.p_मूळ[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_किल्ल्या = reinterpret_cast<स्थिरांक अक्षर*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } अन्यथा {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } अन्यथा {
                    r->fBogus = *status;
                }
            }
            जर (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                जर (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    स्थिरांक char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    जर(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            जर ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (शून्य *)r, r, &cacheStatus);
                जर (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } अन्यथा {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    जर(r != nullptr) {
        /* return the real bundle */
        पर्यंत(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_मोजणी_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        जर(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    परत r;
}

स्थिर UResourceDataEntry *
getPoolEntry(स्थिरांक अक्षर *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    जर( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    परत poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
स्थिर UResourceDataEntry *
findFirstExisting(स्थिरांक अक्षर* path, अक्षर* name, स्थिरांक अक्षर* defaultLocale, UResOpenType openType,
                  UBool *is_मूळ, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = असत्य;
    *foundParent = सत्य; /* we're starting with a fresh name */
    अक्षर origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    पर्यंत(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        जर (U_FAILURE(*status)) {
            परत nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        जर(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_मोजणी_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } अन्यथा {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_मूळ = static_cast<UBool>(uprv_strcmp(name, k_मूळ_Locale_Name) == 0);

        /*Fallback data stuff*/
        जर (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } अन्यथा {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        जर (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    परत r;
}

स्थिर शून्य ures_set_Is_चळत_Object( UResourceBundle* resB, UBool state) {
    जर(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } अन्यथा {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

स्थिर UBool ures_is_चळत_Object(स्थिरांक UResourceBundle* resB) {
  परत((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?असत्य:सत्य);
}


U_CFUNC शून्य ures_init_चळत_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, आकार(UResourceBundle));
  ures_set_Is_चळत_Object(resB, सत्य);
}

U_NAMESPACE_BEGIN

चळत_UResource_Bundle::चळत_UResource_Bundle() {
    ures_init_चळत_Object(&bundle);
}

चळत_UResource_Bundle::~चळत_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

स्थिर UBool  // returns U_SUCCESS(*status)
load_Parents_Except_मूळ(UResourceDataEntry *&t1,
                      अक्षर name[], int32_t nameCapacity,
                      UBool usingUSRData, अक्षर usrDataPath[], UErrorCode *status) {
    जर (U_FAILURE(*status)) { परत असत्य; }
    UBool checkParent = सत्य;
    पर्यंत (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        जर (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            स्थिरांक char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            जर(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                जर (uprv_strcmp(name, k_मूळ_Locale_Name) == 0) {
                    परत सत्य;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        जर (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            परत असत्य;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        जर (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            जर (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                परत असत्य;
            }
        }

        जर (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } अन्यथा {
            t1->fParent = t2;
            जर (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_मोजणी_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    परत सत्य;
}

स्थिर UBool  // returns U_SUCCESS(*status)
घाला_मूळ_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    जर (U_FAILURE(*status)) { परत असत्य; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_मूळ_Locale_Name, t1->fPath, &parentStatus);
    जर (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        परत असत्य;
    }
    t1->fParent = t2;
    t1 = t2;
    परत सत्य;
}

स्थिर UResourceDataEntry *entryOpen(स्थिरांक अक्षर* path, स्थिरांक अक्षर* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = असत्य;
    UBool is_मूळ = असत्य;
    UBool hasRealData = असत्य;
    UBool hasChopped = सत्य;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    अक्षर name[ULOC_FULLNAME_CAPACITY];
    अक्षर usrDataPath[96];

    initCache(status);

    जर(U_FAILURE(*status)) {
        परत nullptr;
    }

    uprv_strncpy(name, localeID, आकार(name) - 1);
    name[आकार(name) - 1] = 0;

    जर ( usingUSRData ) {
        जर ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } अन्यथा {
            uprv_strncpy(usrDataPath, path, आकार(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[आकार(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    स्थिरांक अक्षर *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_मूळ, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    जर (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        जा finish;
    }

    जर(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = सत्य;
        जर ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            जर (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                जा finish;
            }
            जर ( u1 != nullptr ) {
                जर(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } अन्यथा {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_मोजणी_Existing = 0;
                }
            }
        }
        जर ((hasChopped || mayHaveParent(name)) && !is_मूळ) {
            जर (!load_Parents_Except_मूळ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                जा finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    जर(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_मूळ && !isDefault && !is_मूळ) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_मूळ, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        जर (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            जा finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        जर(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = सत्य;
            isDefault = सत्य;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            जर ((hasChopped || mayHaveParent(name)) && !is_मूळ) {
                जर (!load_Parents_Except_मूळ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    जा finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    जर(r == nullptr) {
        uprv_strcpy(name, k_मूळ_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_मूळ, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        जर (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            जा finish;
        }
        जर(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = सत्य;
        } अन्यथा { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            जा finish;
        }
    } अन्यथा जर(!is_मूळ && uprv_strcmp(t1->fName, k_मूळ_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        जर (!घाला_मूळ_Bundle(t1, status)) {
            जा finish;
        }
        जर(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    पर्यंत(r != nullptr && !is_मूळ && t1->fParent != nullptr) {
        t1->fParent->f_मोजणी_Existing++;
        t1 = t1->fParent;
    }

finish:
    जर(U_SUCCESS(*status)) {
        जर(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        परत r;
    } अन्यथा {
        परत nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
स्थिर UResourceDataEntry *
entryOpenDirect(स्थिरांक अक्षर* path, स्थिरांक अक्षर* localeID, UErrorCode* status) {
    initCache(status);
    जर(U_FAILURE(*status)) {
        परत nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    जर (localeID == nullptr) {
        localeID = uloc_getDefault();
    } अन्यथा जर (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_मूळ_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    जर(U_SUCCESS(*status)) {
        जर(r->fBogus != U_ZERO_ERROR) {
            r->f_मोजणी_Existing--;
            r = nullptr;
        }
    } अन्यथा {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    जर(r != nullptr && uprv_strcmp(localeID, k_मूळ_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        अक्षर name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        जर(!chopLocale(name) || uprv_strcmp(name, k_मूळ_Locale_Name) == 0 ||
                load_Parents_Except_मूळ(t1, name, UPRV_LENGTHOF(name), असत्य, nullptr, status)) {
            जर(uprv_strcmp(t1->fName, k_मूळ_Locale_Name) != 0 && t1->fParent == nullptr) {
                घाला_मूळ_Bundle(t1, status);
            }
        }
        जर(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    जर(r != nullptr) {
        // TODO: Does this ever loop?
        पर्यंत(t1->fParent != nullptr) {
            t1->fParent->f_मोजणी_Existing++;
            t1 = t1->fParent;
        }
    }
    परत r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
स्थिर शून्य entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    पर्यंत(resB != nullptr) {
        p = resB->fParent;
        resB->f_मोजणी_Existing--;

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

स्थिर शून्य entryClose(UResourceDataEntry *resB) {
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
स्थिर शून्य ures_appendResPath(UResourceBundle *resB, स्थिरांक अक्षर* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    जर(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    जर(RES_BUFSIZE <= resB->fResPathLen+1) {
        जर(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<अक्षर*>(uprv_malloc((resB->fResPathLen + 1) * आकार(अक्षर)));
            /* Check that memory was allocated correctly. */
            जर (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                परत;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } अन्यथा {
            अक्षर* temp = static_cast<अक्षर*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * आकार(अक्षर)));
            /* Check that memory was reallocated correctly. */
            जर (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                परत;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

स्थिर शून्य ures_freeResPath(UResourceBundle *resB) {
    जर (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

स्थिर शून्य
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    जर(resB != nullptr) {
        जर(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        जर(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        जर(ures_is_चळत_Object(resB) == असत्य && freeBundleObj) {
            uprv_free(resB);
        }
#जर 0 /*U_DEBUG*/
        अन्यथा {
            /* poison the data */
            uprv_memset(resB, -1, आकार(UResourceBundle));
        }
#endif
    }
}

U_CAPI शून्य  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, सत्य);
}

नामाकाश {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  स्थिरांक अक्षर* किल्ली,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  स्थिरांक अक्षर* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(स्थिरांक ResourceData& resData,
                               Resource r,
                               स्थिरांक अक्षर* किल्ली,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               स्थिरांक अक्षर* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    जर (U_FAILURE(*status)) { परत resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    स्थिरांक char16_t *alias = res_getAlias(&resData, r, &len);
    जर(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत resB;
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
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }

    // We have an alias, now let's cut it up.
    स्थिरांक अक्षर *path = nullptr, *locale = nullptr, *किल्ली_Path = nullptr;
    जर(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        अक्षर *chAliasData = chAlias.data();
        अक्षर *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        जर(sep != nullptr) {
            *sep++ = 0;
        }
        जर(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            किल्ली_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } अन्यथा {
            जर(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            जर (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } अन्यथा {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                जर(sep != nullptr) {
                    *sep++ = 0;
                }
                किल्ली_Path = sep;
            }
        }
    } अन्यथा {
        // No path, start with a locale.
        अक्षर *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        जर(sep != nullptr) {
            *sep++ = 0;
        }
        किल्ली_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    जर (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } अन्यथा {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        जर(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            परत resB;
        }
        dataEntry = mainRes->fData;
    }

    स्थिरांक अक्षर* temp = nullptr;
    जर(किल्ली_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.मूळ_Res;
        जर(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            जर (U_FAILURE(*status)) {
                परत nullptr;
            }
            अक्षर *a_किल्ली = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_किल्ली, &temp);
        }
        जर(किल्ली) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(किल्ली, *status);
            जर (U_FAILURE(*status)) {
                परत nullptr;
            }
            अक्षर *a_किल्ली = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_किल्ली, &temp);
        } अन्यथा जर(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            जर(URES_IS_TABLE(type)) {
                स्थिरांक अक्षर *a_किल्ली;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_किल्ली);
            } अन्यथा { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        जर(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } अन्यथा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } अन्यथा {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(किल्ली_Path, *status);
        जर (U_FAILURE(*status)) {
            परत nullptr;
        }
        अक्षर *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        साठी(;;) {
            r = dataEntry->fData.मूळ_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            पर्यंत(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                जर(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    थांबा;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                जर (U_FAILURE(*status)) {
                    थांबा;
                }
                जर (temp == nullptr || uprv_strcmp(किल्ली_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, किल्ली_Path, static_cast<int32_t>(uprv_strlen(किल्ली_Path)), status);
                    जर(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    जर (U_FAILURE(*status)) {
                        थांबा;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            जर (U_FAILURE(*status) || r != RES_BOGUS) {
                थांबा;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            जर (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                थांबा;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, किल्ली_Path);
        }
    }
    जर(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    परत resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  स्थिरांक अक्षर* किल्ली,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  स्थिरांक अक्षर* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    जर(status == nullptr || U_FAILURE(*status)) {
        परत resB;
    }
    जर (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    जर(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        जर(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            परत resB;
        }
        परत getAliasTargetAsResourceBundle(
            dataEntry->fData, r, किल्ली, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    जर(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(आकार(UResourceBundle)));
        जर (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            परत nullptr;
        }
        ures_set_Is_चळत_Object(resB, असत्य);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } अन्यथा {
        जर(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        जर(resB->fVersion != nullptr) {
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
        जर(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = असत्य;
    resB->f_Is_शीर्ष_Level = असत्य;
    resB->fIndex = -1;
    resB->f_किल्ली = किल्ली; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    जर(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    जर(किल्ली != nullptr) {
        ures_appendResPath(resB, किल्ली, static_cast<int32_t>(uprv_strlen(किल्ली)), status);
        जर(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } अन्यथा जर(idx >= 0) {
        अक्षर buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        जर(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, आकार(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_मोजणी_Array_वस्तुसंच(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    परत resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  स्थिरांक अक्षर* किल्ली,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  स्थिरांक UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    परत init_resb_result(
        dataEntry, r, किल्ली, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               स्थिरांक UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_चळत_Object;
    जर(U_FAILURE(*status) || r == original) {
        परत r;
    }
    जर(original != nullptr) {
        जर(r == nullptr) {
            is_चळत_Object = असत्य;
            r = static_cast<UResourceBundle*>(uprv_malloc(आकार(UResourceBundle)));
            /* test for nullptr */
            जर (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                परत nullptr;
            }
        } अन्यथा {
            is_चळत_Object = ures_is_चळत_Object(r);
            ures_closeBundle(r, असत्य);
        }
        uprv_memcpy(r, original, आकार(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        जर(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_चळत_Object(r, is_चळत_Object);
        जर(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    परत r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI स्थिरांक char16_t* U_EXPORT2 ures_getString(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    स्थिरांक char16_t *s;
    जर (status==nullptr || U_FAILURE(*status)) {
        परत nullptr;
    }
    जर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    जर (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    परत s;
}

स्थिर स्थिरांक अक्षर*
ures_toUTF8String(स्थिरांक char16_t* s16,
                  int32_t length16,
                  अक्षर* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    जर (U_FAILURE(*status)) {
        परत nullptr;
    }
    जर (pLength != nullptr) {
        capacity = *pLength;
    } अन्यथा {
        capacity = 0;
    }
    जर (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    जर (length16 == 0) {
        /* empty string, return as read-only pointer */
        जर (pLength != nullptr) {
            *pLength = 0;
        }
        जर (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            परत dest;
        } अन्यथा {
            परत "";
        }
    } अन्यथा {
        /* We need to transform the string to the destination buffer. */
        जर (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            परत u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        जर (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            जर (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        परत u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_getUTF8String(स्थिरांक UResourceBundle* resB,
                   अक्षर* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    स्थिरांक char16_t *s16 = ures_getString(resB, &length16, status);
    परत ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI स्थिरांक uint8_t* U_EXPORT2
ures_get_द्विमान(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  स्थिरांक uint8_t *p;
  जर (status==nullptr || U_FAILURE(*status)) {
    परत nullptr;
  }
  जर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत nullptr;
  }
  p = res_get_द्विमान({resB}, &resB->getResData(), resB->fRes, len);
  जर (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  परत p;
}

U_CAPI स्थिरांक int32_t* U_EXPORT2
ures_getIntVector(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  स्थिरांक int32_t *p;
  जर (status==nullptr || U_FAILURE(*status)) {
    परत nullptr;
  }
  जर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  जर (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  परत p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(स्थिरांक UResourceBundle* resB, UErrorCode *status) {
  जर (status==nullptr || U_FAILURE(*status)) {
    परत 0xffffffff;
  }
  जर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0xffffffff;
  }
  जर(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    परत 0xffffffff;
  }
  परत res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(स्थिरांक UResourceBundle* resB, UErrorCode *status) {
  जर (status==nullptr || U_FAILURE(*status)) {
    परत 0xffffffff;
  }
  जर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0xffffffff;
  }
  जर(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    परत 0xffffffff;
  }
  परत res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(स्थिरांक UResourceBundle *resB) {
  जर(resB == nullptr) {
    परत URES_NONE;
  }
  परत res_getPublicType(resB->fRes);
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_get_किल्ली(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  जर(resB == nullptr) {
    परत nullptr;
  }
  परत(resB->f_किल्ली);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(स्थिरांक UResourceBundle *resB) {
  जर(resB == nullptr) {
    परत 0;
  }
  
  परत resB->fSize;
}

स्थिर स्थिरांक char16_t*
ures_getStringWithAlias(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  जर(RES_GET_TYPE(r) == URES_ALIAS) {
    स्थिरांक char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    परत result;
  } अन्यथा {
    परत res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI शून्य U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  जर(resB == nullptr) {
    परत;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(स्थिरांक UResourceBundle *resB) {
  जर(resB == nullptr) {
    परत असत्य;
  }
  परत resB->fIndex < resB->fSize-1;
}

U_CAPI स्थिरांक char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   स्थिरांक अक्षर** किल्ली,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  जर (status==nullptr || U_FAILURE(*status)) {
    परत nullptr;
  }
  जर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत nullptr;
  }
  
  जर(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } अन्यथा {
    resB->fIndex++;
    निवड(RES_GET_TYPE(resB->fRes)) {
    स्थिती URES_STRING:
    स्थिती URES_STRING_V2:
      परत res_getString({resB}, &resB->getResData(), resB->fRes, len);
    स्थिती URES_TABLE:
    स्थिती URES_TABLE16:
    स्थिती URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, किल्ली);
      जर(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      परत ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    स्थिती URES_ARRAY:
    स्थिती URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      जर(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      परत ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    स्थिती URES_ALIAS:
      परत ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    स्थिती URES_INT:
    स्थिती URES_द्विमान:
    स्थिती URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    पूर्वनिर्धारित:
      परत nullptr;
    }
  }

  परत nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    स्थिरांक अक्षर *किल्ली = nullptr;
    Resource r = RES_BOGUS;

    जर (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            परत fillIn;
    }
    जर(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            परत fillIn;
    }

    जर(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } अन्यथा {
        resB->fIndex++;
        निवड(RES_GET_TYPE(resB->fRes)) {
        स्थिती URES_INT:
        स्थिती URES_द्विमान:
        स्थिती URES_STRING:
        स्थिती URES_STRING_V2:
        स्थिती URES_INT_VECTOR:
            परत ures_copyResb(fillIn, resB, status);
        स्थिती URES_TABLE:
        स्थिती URES_TABLE16:
        स्थिती URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &किल्ली);
            जर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            परत init_resb_result(resB->fData, r, किल्ली, resB->fIndex, resB, fillIn, status);
        स्थिती URES_ARRAY:
        स्थिती URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            जर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            परत init_resb_result(resB->fData, r, किल्ली, resB->fIndex, resB, fillIn, status);
        पूर्वनिर्धारित:
            /*return nullptr;*/
            परत fillIn;
        }
    }
    /*return nullptr;*/
    परत fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(स्थिरांक UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    स्थिरांक अक्षर* किल्ली = nullptr;
    Resource r = RES_BOGUS;

    जर (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        परत fillIn;
    }
    जर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        परत fillIn;
    }

    जर(indexR >= 0 && resB->fSize > indexR) {
        निवड(RES_GET_TYPE(resB->fRes)) {
        स्थिती URES_INT:
        स्थिती URES_द्विमान:
        स्थिती URES_STRING:
        स्थिती URES_STRING_V2:
        स्थिती URES_INT_VECTOR:
            परत ures_copyResb(fillIn, resB, status);
        स्थिती URES_TABLE:
        स्थिती URES_TABLE16:
        स्थिती URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &किल्ली);
            जर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            परत init_resb_result(resB->fData, r, किल्ली, indexR, resB, fillIn, status);
        स्थिती URES_ARRAY:
        स्थिती URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            जर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            परत init_resb_result(resB->fData, r, किल्ली, indexR, resB, fillIn, status);
        पूर्वनिर्धारित:
            /*return nullptr;*/
            परत fillIn;
        }
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    परत fillIn;
}

U_CAPI स्थिरांक char16_t* U_EXPORT2
ures_getStringByIndex(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    स्थिरांक अक्षर* किल्ली = nullptr;
    Resource r = RES_BOGUS;

    जर (status==nullptr || U_FAILURE(*status)) {
        परत nullptr;
    }
    जर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    जर(indexS >= 0 && resB->fSize > indexS) {
        निवड(RES_GET_TYPE(resB->fRes)) {
        स्थिती URES_STRING:
        स्थिती URES_STRING_V2:
            परत res_getString({resB}, &resB->getResData(), resB->fRes, len);
        स्थिती URES_TABLE:
        स्थिती URES_TABLE16:
        स्थिती URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &किल्ली);
            जर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            परत ures_getStringWithAlias(resB, r, indexS, len, status);
        स्थिती URES_ARRAY:
        स्थिती URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            जर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            परत ures_getStringWithAlias(resB, r, indexS, len, status);
        स्थिती URES_ALIAS:
            परत ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        स्थिती URES_INT:
        स्थिती URES_द्विमान:
        स्थिती URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            थांबा;
        पूर्वनिर्धारित:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          थांबा;
        }
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    परत nullptr;
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_getUTF8StringByIndex(स्थिरांक UResourceBundle* resB,
                          int32_t idx,
                          अक्षर* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    स्थिरांक char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    परत ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(स्थिरांक अक्षर* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  अक्षर *packageName = nullptr;
  अक्षर *pathToResource = nullptr, *save = nullptr;
  अक्षर *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  जर(status == nullptr || U_FAILURE(*status)) {
    परत result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (अक्षर *)uprv_malloc(length*आकार(अक्षर));
  /* test for nullptr */
  जर(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  जर(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    जर(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  जर(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  जर(U_SUCCESS(*status)) {
    जर(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } अन्यथा {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  परत result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(स्थिरांक UResourceBundle* resB,
                     अक्षर* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  स्थिरांक अक्षर *किल्ली;

  जर(status == nullptr || U_FAILURE(*status)) {
    परत result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  करा {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &किल्ली); 
    जर(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, किल्ली, -1, resB, fillIn, status);
        resB = result;
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
        थांबा;
    }
  } पर्यंत(*path); /* there is more stuff in the path */

  परत result;
}

U_CAPI स्थिरांक char16_t* U_EXPORT2
ures_get_String_By_किल्ली_With_Fallback(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND,
                                स्थिरांक अक्षर* in_किल्ली,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle चळत_2;
    स्थिरांक char16_t* retVal = nullptr;
    ures_init_चळत_Object(&चळत_2);
    ures_get_By_किल्ली_With_Fallback(resB, in_किल्ली, &चळत_2, status);
    int32_t length;
    retVal = ures_getString(&चळत_2, &length, status);
    ures_close(&चळत_2);
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }
    जर (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    जर (len != nullptr) {
        *len = length;
    }
    परत retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
स्थिर Resource get_Table_Item_By_किल्ली_Path(स्थिरांक ResourceData *pResData, Resource table, स्थिरांक अक्षर *किल्ली) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(किल्ली, errorCode);
  जर (U_FAILURE(errorCode)) { परत RES_BOGUS; }
  अक्षर *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  पर्यंत (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    अक्षर *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    जर (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } अन्यथा {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    स्थिरांक अक्षर *pathP = pathPart;
    resource = res_get_Table_Item_By_किल्ली(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  जर (*pathPart) {
    परत RES_BOGUS;
  }
  परत resource;
}

स्थिर शून्य createPath(स्थिरांक अक्षर* origResPath,
                       int32_t     origResPathLen,
                       स्थिरांक अक्षर* resPath,
                       int32_t     resPathLen,
                       स्थिरांक अक्षर* in_किल्ली,
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
    स्थिरांक अक्षर* किल्ली = in_किल्ली;
    जर (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        जर (U_SUCCESS(*status)) {
            स्थिरांक अक्षर* resPathLimit = resPath + resPathLen;
            स्थिरांक अक्षर* origResPathLimit = origResPath + origResPathLen;
            स्थिरांक अक्षर* resPathPtr = resPath;
            स्थिरांक अक्षर* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            पर्यंत (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                पर्यंत (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                जर (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                पर्यंत (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                जर (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            पर्यंत (resPathPtr < resPathLimit && *किल्ली != '\0') {
                पर्यंत (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                जर (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                पर्यंत (*किल्ली != '\0' && *किल्ली != RES_PATH_SEPARATOR) {
                    ++किल्ली;
                }
                जर (*किल्ली == RES_PATH_SEPARATOR) {
                    ++किल्ली;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(किल्ली, *status);
    } अन्यथा {
        path.append(in_किल्ली, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_किल्ली_With_Fallback(स्थिरांक UResourceBundle* resB,
                          स्थिरांक अक्षर* in_किल्ली,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, मूळ_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    जर (status==nullptr || U_FAILURE(*status)) {
        परत fillIn;
    }
    जर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    जर(URES_IS_TABLE(type)) {
        स्थिरांक अक्षर* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_किल्ली_Path(&resB->getResData(), resB->fRes, in_किल्ली);
        स्थिरांक अक्षर* किल्ली = in_किल्ली;
        तार्किक did_मूळ_Once = असत्य;
        जर(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            अक्षर *myPath = nullptr;
            स्थिरांक अक्षर* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            पर्यंत(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_मूळ_Once)) { /* Otherwise, we'll look in parents */
                जर (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } अन्यथा {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_मूळ_Once = सत्य;
                }
                मूळ_Res = dataEntry->fData.मूळ_Res;

                जर(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_किल्ली, path, status);
                    जर (U_FAILURE(*status)) {
                        ures_close(helper);
                        परत fillIn;
                    }
                    myPath = path.data();
                    किल्ली = in_किल्ली;
                    करा {
                        res = res_findResource(&(dataEntry->fData), मूळ_Res, &myPath, &किल्ली);
                        जर (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            जर(helper) {
                              dataEntry = helper->fData;
                              मूळ_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } अन्यथा {
                              थांबा;
                            }
                        } अन्यथा जर (res == RES_BOGUS) {
                            थांबा;
                        }
                    } पर्यंत(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            जर(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                जर(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_मूळ_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } अन्यथा {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, किल्ली, -1, resB, fillIn, status);
                जर (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_किल्ली, path, status);
                } अन्यथा {
                    स्थिरांक अक्षर* separator = nullptr;
                    जर (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    जर (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_किल्ली, path, status);
                    } अन्यथा {
                        createPath(origResPath, origResPathLen, "", 0, in_किल्ली, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                जर(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            fillIn = init_resb_result(resB->fData, res, किल्ली, -1, resB, fillIn, status);
        }
    } 
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    परत fillIn;
}

नामाकाश {

शून्य get_All_वस्तुसंच_With_Fallback(
        स्थिरांक UResourceBundle *bundle, Resource_Data_मूल्य &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत; }
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
    sink.put(bundle->f_किल्ली, value, !hasParent, errorCode);
    जर (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        चळत_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_शीर्ष_Level = सत्य;
        parentRef.fRes = parentRef.getResData().मूळ_Res;
        parentRef.fSize = res_मोजणी_Array_वस्तुसंच(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        चळत_UResource_Bundle containerBundle;
        स्थिरांक UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        जर (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } अन्यथा {
            rb = ures_get_By_किल्ली_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        जर (U_SUCCESS(pathErrorCode)) {
            get_All_वस्तुसंच_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

संरचना GetAllChildrenSink : सार्वजनिक ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    आभासी ~GetAllChildrenSink() override;
    आभासी शून्य put(स्थिरांक अक्षर *किल्ली, Resource_मूल्य &value, UBool is_मूळ,
           UErrorCode &errorCode) override {
        ResourceTable वस्तुसंच_Table = value.getTable(errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        साठी (int32_t i = 0; वस्तुसंच_Table.get_किल्ली_And_मूल्य(i, किल्ली, value); ++i) {
            जर (value.getType() == URES_ALIAS) {
                Resource_Data_मूल्य& rdv = static_cast<Resource_Data_मूल्य&>(value);
                चळत_UResource_Bundle चळत_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          चळत_Temp_Bundle.getAlias(), &errorCode);
                जर (U_SUCCESS(errorCode)) {
                    Resource_Data_मूल्य aliased_मूल्य;
                    aliased_मूल्य.setData(aliasRB->getResData());
                    aliased_मूल्य.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_मूल्य.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    जर (aliased_मूल्य.getType() != URES_TABLE) {
                        dest.put(किल्ली, aliased_मूल्य, is_मूळ, errorCode);
                    } अन्यथा {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_मूल्य_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        स्थिरांक अक्षर* parent_किल्ली = किल्ली; // dest.put() changes the key
                        dest.put(parent_किल्ली, aliased_मूल्य, is_मूळ, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        पर्यंत (aliased_मूल्य_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            अक्षर* localPathAsCharPtr = localPath.data();
                            स्थिरांक अक्षर* child_किल्ली;
                            entry = entry->fParent;
                            res = entry->fData.मूळ_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_किल्ली);
                            जर (newRes != RES_BOGUS) {
                                aliased_मूल्य.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_मूल्य.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_मूल्य_Type = aliased_मूल्य.getType();
                                जर (aliased_मूल्य_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_मूल्य& rdv2 = static_cast<Resource_Data_मूल्य&>(aliased_मूल्य);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             चळत_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_मूल्य.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_मूल्य.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_मूल्य_Type = aliased_मूल्य.getType();
                                }
                                जर (aliased_मूल्य_Type == URES_TABLE) {
                                    dest.put(parent_किल्ली, aliased_मूल्य, is_मूळ, errorCode);
                                } अन्यथा {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    परत;
                                }
                            }
                        }
                    }
                }
            } अन्यथा {
                dest.put(किल्ली, value, is_मूळ, errorCode);
            }
            जर (U_FAILURE(errorCode)) { परत; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI शून्य U_EXPORT2
ures_getAllChildrenWithFallback(स्थिरांक UResourceBundle *bundle, स्थिरांक अक्षर *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_वस्तुसंच_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI शून्य U_EXPORT2
ures_get_मूल्य_With_Fallback(स्थिरांक UResourceBundle *bundle, स्थिरांक अक्षर *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_मूल्य &value, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत; }
    जर (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    स्थिरांक UResourceBundle *rb;
    जर (*path == 0) {
        // empty path
        rb = bundle;
    } अन्यथा {
        rb = ures_get_By_किल्ली_With_Fallback(bundle, path, tempFillIn, &errorCode);
        जर (U_FAILURE(errorCode)) {
            परत;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI शून्य U_EXPORT2
ures_get_All_वस्तुसंच_With_Fallback(स्थिरांक UResourceBundle *bundle, स्थिरांक अक्षर *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत; }
    जर (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    चळत_UResource_Bundle चळत_Bundle;
    स्थिरांक UResourceBundle *rb;
    जर (*path == 0) {
        // empty path
        rb = bundle;
    } अन्यथा {
        rb = ures_get_By_किल्ली_With_Fallback(bundle, path, चळत_Bundle.getAlias(), &errorCode);
        जर (U_FAILURE(errorCode)) {
            परत;
        }
    }
    // Get all table items with fallback.
    Resource_Data_मूल्य value;
    get_All_वस्तुसंच_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_किल्ली(स्थिरांक UResourceBundle* resB,
              स्थिरांक अक्षर* in_किल्ली,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    स्थिरांक अक्षर *किल्ली = in_किल्ली;

    जर (status==nullptr || U_FAILURE(*status)) {
        परत fillIn;
    }
    जर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    जर(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_किल्ली(&resB->getResData(), resB->fRes, &t, &किल्ली);
        जर(res == RES_BOGUS) {
            किल्ली = in_किल्ली;
            जर(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &किल्ली, &res, status);
                जर(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    परत init_resb_result(dataEntry, res, किल्ली, -1, resB, fillIn, status);
                } अन्यथा {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            परत init_resb_result(resB->fData, res, किल्ली, -1, resB, fillIn, status);
        }
    } 
#जर 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    अन्यथा जर(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == सत्य) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &किल्ली, &res, status);
        जर(U_SUCCESS(*status)) {
            परत init_resb_result(dataEntry, res, किल्ली, resB, fillIn, status);
        } अन्यथा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    परत fillIn;
}

U_CAPI स्थिरांक char16_t* U_EXPORT2
ures_get_String_By_किल्ली(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND,
                    स्थिरांक अक्षर* in_किल्ली,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    स्थिरांक अक्षर* किल्ली = in_किल्ली;

    जर (status==nullptr || U_FAILURE(*status)) {
        परत nullptr;
    }
    जर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    जर(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_किल्ली(&resB->getResData(), resB->fRes, &t, &किल्ली);

        जर(res == RES_BOGUS) {
            किल्ली = in_किल्ली;
            जर(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &किल्ली, &res, status);
                जर(U_SUCCESS(*status)) {
                    निवड (RES_GET_TYPE(res)) {
                    स्थिती URES_STRING:
                    स्थिती URES_STRING_V2:
                        परत res_getString({resB, किल्ली}, &dataEntry->fData, res, len);
                    स्थिती URES_ALIAS:
                      {
                        स्थिरांक char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_किल्ली(resB, in_किल्ली, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        परत result;
                      }
                    पूर्वनिर्धारित:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } अन्यथा {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            निवड (RES_GET_TYPE(res)) {
            स्थिती URES_STRING:
            स्थिती URES_STRING_V2:
                परत res_getString({resB, किल्ली}, &resB->getResData(), res, len);
            स्थिती URES_ALIAS:
              {
                स्थिरांक char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_किल्ली(resB, in_किल्ली, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                परत result;
              }
            पूर्वनिर्धारित:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#जर 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    अन्यथा जर(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == सत्य) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &किल्ली, &res, status);
        जर(U_SUCCESS(*status)) {
            // TODO: Tracing
            परत res_getString(rd, res, len);
        } अन्यथा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    परत nullptr;
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_get_UTF8_String_By_किल्ली(स्थिरांक UResourceBundle* resB,
                        स्थिरांक अक्षर* किल्ली,
                        अक्षर* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    स्थिरांक char16_t *s16 = ures_get_String_By_किल्ली(resB, किल्ली, &length16, status);
    परत ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_getLocaleInternal(स्थिरांक UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    जर (status==nullptr || U_FAILURE(*status)) {
        परत nullptr;
    }
    जर (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    } अन्यथा {
      परत resourceBundle->fData->fName;
    }
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_getLocale(स्थिरांक UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  परत ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_getLocaleByType(स्थिरांक UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    जर (status==nullptr || U_FAILURE(*status)) {
        परत nullptr;
    }
    जर (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    } अन्यथा {
        निवड(type) {
        स्थिती ULOC_ACTUAL_LOCALE:
            परत resourceBundle->fData->fName;
        स्थिती ULOC_VALID_LOCALE:
            परत resourceBundle->fValidLocaleDataEntry->fName;
        स्थिती ULOC_REQUESTED_LOCALE:
        पूर्वनिर्धारित:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            परत nullptr;
        }
    }
}

U_CFUNC स्थिरांक अक्षर*
ures_getName(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND) {
  जर(resB == nullptr) {
    परत nullptr;
  }

  परत resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC स्थिरांक अक्षर*
ures_getPath(स्थिरांक UResourceBundle* resB U_LIFETIME_BOUND) {
  जर(resB == nullptr) {
    परत nullptr;
  }

  परत resB->fData->fPath;
}
#endif

स्थिर UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  स्थिरांक अक्षर* path,
                  स्थिरांक अक्षर* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    जर(U_FAILURE(*status)) {
        परत nullptr;
    }

    UResourceDataEntry *entry;
    जर(openType != URES_OPEN_DIRECT) {
        जर (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        जर(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            परत nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } अन्यथा {
        entry = entryOpenDirect(path, localeID, status);
    }
    जर(U_FAILURE(*status)) {
        परत nullptr;
    }
    जर(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        परत nullptr;
    }

    UBool is_चळत_Object;
    जर(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(आकार(UResourceBundle)));
        जर(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            परत nullptr;
        }
        is_चळत_Object = असत्य;
    } अन्यथा {  // fill-in
        is_चळत_Object = ures_is_चळत_Object(r);
        ures_closeBundle(r, असत्य);
    }
    uprv_memset(r, 0, आकार(UResourceBundle));
    ures_set_Is_चळत_Object(r, is_चळत_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_शीर्ष_Level = सत्य;
    r->fRes = r->getResData().मूळ_Res;
    r->fSize = res_मोजणी_Array_वस्तुसंच(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    परत r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(स्थिरांक अक्षर* path, स्थिरांक अक्षर* localeID, UErrorCode* status) {
    परत ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_मूळ, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(स्थिरांक अक्षर* path, स्थिरांक अक्षर* localeID, UErrorCode* status) {
    परत ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_मूळ, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(स्थिरांक अक्षर* path, स्थिरांक अक्षर* localeID, UErrorCode* status) {
    परत ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI शून्य U_EXPORT2
ures_openFillIn(UResourceBundle *r, स्थिरांक अक्षर* path,
                स्थिरांक अक्षर* localeID, UErrorCode* status) {
    जर(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_मूळ, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI शून्य U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, स्थिरांक अक्षर* path, स्थिरांक अक्षर* localeID, UErrorCode* status) {
    जर(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_मोजणी_Array_वस्तुसंच(स्थिरांक UResourceBundle* resourceBundle,
                  स्थिरांक अक्षर* resource_किल्ली,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_चळत_Object(&resData);
    जर (status==nullptr || U_FAILURE(*status)) {
        परत 0;
    }
    जर(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }
    ures_get_By_किल्ली(resourceBundle, resource_किल्ली, &resData, status);
    
    जर(resData.getResData().data != nullptr) {
        int32_t result = res_मोजणी_Array_वस्तुसंच(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        परत result;
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        परत 0;
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
U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_get_Version_संख्या_Internal(स्थिरांक UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    जर (!resourceBundle) परत nullptr;

    जर(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        स्थिरांक char16_t* minor_version = ures_get_String_By_किल्ली(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (अक्षर *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        जर (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            परत nullptr;
        }
       
        जर(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        अन्यथा {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    परत resourceBundle->fVersion;
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ures_get_Version_संख्या(स्थिरांक UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    परत ures_get_Version_संख्या_Internal(resourceBundle);
}

U_CAPI शून्य U_EXPORT2 ures_getVersion(स्थिरांक UResourceBundle* resB, UVersionInfo versionInfo) {
    जर (!resB) परत;

    u_versionFromString(versionInfo, ures_get_Version_संख्या_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#जर defined(URES_वृक्ष_DEBUG)
#अन्तर्भाव <stdio.h>
#endif

प्रकारव्याख्या संरचना ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

स्थिर शून्य U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

स्थिर int32_t U_CALLCONV
ures_loc_मोजणी_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    परत ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


स्थिर स्थिरांक अक्षर * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    स्थिरांक अक्षर *result = nullptr;
    int32_t len = 0;
    जर (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_किल्ली(k);
        len = (int32_t)uprv_strlen(result);
    }
    जर (resultLength) {
        *resultLength = len;
    }
    परत result;
}

स्थिर शून्य U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

स्थिर स्थिरांक UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_मोजणी_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(स्थिरांक अक्षर *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    जर(U_FAILURE(*status)) {
        परत nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(आकार(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(आकार(UEnumeration));
    जर(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        परत nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, आकार(UEnumeration));

    ures_init_चळत_Object(&myContext->installed);
    ures_init_चळत_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_किल्ली(idx, INDEX_TAG, &myContext->installed, status);
    जर(U_SUCCESS(*status)) {
#जर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_किल्ली(&myContext->installed));
#endif
        en->context = myContext;
    } अन्यथा {
#जर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    परत en;
}

स्थिर UBool isLocaleInList(UEnumeration *locEnum, स्थिरांक अक्षर *loc_To_शोध, UErrorCode *status) {
    स्थिरांक अक्षर *loc;
    पर्यंत ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        जर (uprv_strcmp(loc, loc_To_शोध) == 0) {
            परत सत्य;
        }
    }
    परत असत्य;
}

स्थिर शून्य getParentForFunctionalEquivalent(स्थिरांक अक्षर*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    जर (res != nullptr) {
        ures_get_By_किल्ली(res, "%%Parent", bund1, &subStatus);
        जर (U_SUCCESS(subStatus)) {
            int32_t length16;
            स्थिरांक char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    जर (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(अक्षर *result, int32_t resultCapacity,
                             स्थिरांक अक्षर *path, स्थिरांक अक्षर *resName, स्थिरांक अक्षर *keyword, स्थिरांक अक्षर *locid,
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
    जर(U_FAILURE(*status)) परत 0;
    CharString kwVal;
    जर (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_मूल्य(locid, keyword, subStatus);
        जर (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    जर (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#जर defined(URES_वृक्ष_DEBUG)
    ख_लिखो(मानक_त्रुटि, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_चळत_Object(&bund1);
    ures_init_चळत_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    जर(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = सत्य;
        जर (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    जर(U_FAILURE(subStatus)) {
        *status = subStatus;
        परत 0;
    }
    
    करा {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        जर(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = असत्य;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#जर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        जर(U_FAILURE(subStatus)) {
            *status = subStatus;
        } अन्यथा जर(subStatus == U_ZERO_ERROR) {
            ures_get_By_किल्ली(res,resName,&bund1, &subStatus);
            जर(subStatus == U_ZERO_ERROR) {
                स्थिरांक char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#जर defined(URES_वृक्ष_DEBUG)
                ख_लिखो(मानक_त्रुटि, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_किल्ली(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                जर(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#जर defined(URES_वृक्ष_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    जर(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#जर defined(URES_वृक्ष_DEBUG)
                        ख_लिखो(मानक_त्रुटि, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        जर (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        जर (found != parent) {
            parent.copyFrom(found, subStatus);
        } अन्यथा {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } पर्यंत(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    करा {
        res = ures_open(path, parent.data(), &subStatus);
        जर((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = असत्य;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#जर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        जर(U_FAILURE(subStatus)) {
            *status = subStatus;
        } अन्यथा जर(subStatus == U_ZERO_ERROR) {
            ures_get_By_किल्ली(res,resName,&bund1, &subStatus);
#जर defined(URES_वृक्ष_DEBUG)
/**/ ख_लिखो(मानक_त्रुटि,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            जर(subStatus == U_ZERO_ERROR) {
                ures_get_By_किल्ली(&bund1, kwVal.data(), &bund2, &subStatus);
#जर defined(URES_वृक्ष_DEBUG)
/**/ ख_लिखो(मानक_त्रुटि,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                जर(subStatus == U_ZERO_ERROR) {
#जर defined(URES_वृक्ष_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    जर (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } अन्यथा {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        जर(defLoc.length() > full.length()) {
                          स्थिरांक char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#जर defined(URES_वृक्ष_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_किल्ली(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          जर(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#जर defined(URES_वृक्ष_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#जर defined(URES_वृक्ष_DEBUG)
                        अन्यथा {
                          ख_लिखो(मानक_त्रुटि, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } अन्यथा {
#जर defined(URES_वृक्ष_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = असत्य;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        जर (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            स्थिरांक अक्षर *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            जर (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                जर (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = सत्य;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        जर (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } पर्यंत(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    जर(full.isEmpty() && kwVal != defVal) {
#जर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        करा { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            जर((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = असत्य;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#जर defined(URES_वृक्ष_DEBUG)
            ख_लिखो(मानक_त्रुटि, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            जर(U_FAILURE(subStatus)) {
                *status = subStatus;
            } अन्यथा जर(subStatus == U_ZERO_ERROR) {
                ures_get_By_किल्ली(res,resName,&bund1, &subStatus);
                जर(subStatus == U_ZERO_ERROR) {
                    ures_get_By_किल्ली(&bund1, kwVal.data(), &bund2, &subStatus);
                    जर(subStatus == U_ZERO_ERROR) {
#जर defined(URES_वृक्ष_DEBUG)
                        ख_लिखो(मानक_त्रुटि, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        जर (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } अन्यथा {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        जर(defLoc.length() > full.length()) {
                          स्थिरांक char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#जर defined(URES_वृक्ष_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_किल्ली(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          जर(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#जर defined(URES_वृक्ष_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#जर defined(URES_वृक्ष_DEBUG)
                        अन्यथा {
                          ख_लिखो(मानक_त्रुटि, "No trim1,  %s <= %s\n", defLoc.data(), full.data());
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
        } पर्यंत(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    जर(U_SUCCESS(*status)) {
        जर(full.isEmpty()) {
#जर defined(URES_वृक्ष_DEBUG)
          ख_लिखो(मानक_त्रुटि, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } अन्यथा जर(omitDefault) {
#जर defined(URES_वृक्ष_DEBUG)
          ख_लिखो(मानक_त्रुटि,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          जर(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            जर(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#जर defined(URES_वृक्ष_DEBUG)
              ख_लिखो(मानक_त्रुटि, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        जर(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } अन्यथा जर(!omitDefault) {
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

    जर(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        जर(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        जर(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } अन्यथा {
        length = 0;
        result[0]=0;
    }
    परत u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_मूल्ये(स्थिरांक अक्षर *path, स्थिरांक अक्षर *keyword, UErrorCode *status)
{
#define मूल्ये_BUF_SIZE 2048
#define मूल्ये_LIST_SIZE 512
    
    अक्षर       मूल्ये_Buf[मूल्ये_BUF_SIZE];
    int32_t    मूल्ये_Index = 0;
    स्थिरांक अक्षर *मूल्ये_List[मूल्ये_LIST_SIZE];
    int32_t    मूल्ये_मोजणी = 0;
    
    स्थिरांक अक्षर *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_चळत_Object(&item);
    ures_init_चळत_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    जर(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        परत nullptr;
    }
    
    मूल्ये_Buf[0]=0;
    मूल्ये_Buf[1]=0;

    पर्यंत ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#जर defined(URES_वृक्ष_DEBUG)
        जर(!bund || U_FAILURE(subStatus)) {
            ख_लिखो(मानक_त्रुटि, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_किल्ली(bund, keyword, &item, &subStatus);
        
        जर(!bund || U_FAILURE(subStatus)) {
#जर defined(URES_वृक्ष_DEBUG)
            ख_लिखो(मानक_त्रुटि, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            सुरूठेवा;
        }

        पर्यंत ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            स्थिरांक अक्षर *k;
            int32_t i;
            k = ures_get_किल्ली(subPtr);

#जर defined(URES_वृक्ष_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            जर(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                सुरूठेवा;
            }
            साठी(i=0; i<मूल्ये_मोजणी; i++) {
                जर(!uprv_strcmp(मूल्ये_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    थांबा;
                }
            }
            जर(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                जर((मूल्ये_मोजणी >= (मूल्ये_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((मूल्ये_Index+kLen+1+1) >= मूल्ये_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } अन्यथा {
                    uprv_strcpy(मूल्ये_Buf+मूल्ये_Index, k);
                    मूल्ये_List[मूल्ये_मोजणी++] = मूल्ये_Buf+मूल्ये_Index;
                    मूल्ये_Index += kLen;
#जर defined(URES_वृक्ष_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    मूल्ये_Buf[मूल्ये_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    मूल्ये_Buf[मूल्ये_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#जर defined(URES_वृक्ष_DEBUG)
    ख_लिखो(मानक_त्रुटि, "%s:  size %d, #%d\n", u_errorName(*status), 
        मूल्ये_Index, मूल्ये_मोजणी);
#endif
    परत uloc_openKeywordList(मूल्ये_Buf, मूल्ये_Index, status);
}
#जर 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(स्थिरांक UResourceBundle* res1, स्थिरांक UResourceBundle* res2){
    जर(res1==nullptr || res2==nullptr){
        परत res1==res2; /* pointer comparison */
    }
    जर(res1->f_किल्ली==nullptr||  res2->f_किल्ली==nullptr){
        परत (res1->f_किल्ली==res2->f_किल्ली);
    }अन्यथा{
        जर(uprv_strcmp(res1->f_किल्ली, res2->f_किल्ली)!=0){
            परत असत्य;
        }
    }
    जर(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        परत असत्य;
    }
    जर(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        परत (res1->fData->fPath == res2->fData->fPath);
    }अन्यथा{
        जर(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            परत असत्य;
        }
    }
    जर(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        परत असत्य;
    }
    जर(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        परत असत्य;
    }
    जर(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        परत असत्य;
    }
    जर(res1->fRes != res2->fRes){
        परत असत्य;
    }
    परत सत्य;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(स्थिरांक UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    जर(U_FAILURE(*status) || res == nullptr){
        परत nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    जर(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }अन्यथा{
        ret = bundle;
    }
    परत ret;
}
U_CAPI स्थिरांक UResourceBundle* U_EXPORT2
ures_getParentBundle(स्थिरांक UResourceBundle* res){
    जर(res==nullptr){
        परत nullptr;
    }
    परत res->fParentRes;
}
#endif

U_CAPI शून्य U_EXPORT2
ures_get_Version_By_किल्ली(स्थिरांक UResourceBundle* res, स्थिरांक अक्षर *किल्ली, UVersionInfo ver, UErrorCode *status) {
  स्थिरांक char16_t *str;
  int32_t len;
  str = ures_get_String_By_किल्ली(res, किल्ली, &len, status);
  जर(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
