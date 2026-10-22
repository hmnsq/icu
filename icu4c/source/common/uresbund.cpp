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
स्थिर int32_t U_CALLCONV hashEntry(ध्रुव UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    प्रत्यागम uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
स्थिर UBool U_CALLCONV compareEntries(ध्रुव UHashTok p1, ध्रुव UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    प्रत्यागम uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
स्थिर UBool chopLocale(वर्ण *name) {
    वर्ण *i = uprv_strrchr(name, '_');

    जइ(i != nullptr) {
        *i = '\0';
        प्रत्यागम सच्च;
    }

    प्रत्यागम मिच्छ;
}

स्थिर UBool hasVariant(ध्रुव वर्ण* localeID) {
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
    प्रत्यागम sink.सङ्ख्या_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#अन्तर्भाव "localefallback_data.h"

स्थिर ध्रुव वर्ण* performFallbackLookup(ध्रुव वर्ण* कुञ्चिका,
                                         ध्रुव वर्ण* कुञ्चिका_Strs,
                                         ध्रुव वर्ण* मानम्_Strs U_LIFETIME_BOUND,
                                         ध्रुव int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    ध्रुव int32_t* bottom = lookupTable;
    ध्रुव int32_t* शिखरम् = lookupTable + lookupTableLength;

    जाव (bottom < शिखरम्) {
        // Effectively, divide by 2 and round down to an even index
        ध्रुव int32_t* मध्यः = bottom + (((शिखरम् - bottom) / 4) * 2);
        ध्रुव वर्ण* entry_कुञ्चिका = &(कुञ्चिका_Strs[*मध्यः]);
        int32_t strcmpResult = uprv_strcmp(कुञ्चिका, entry_कुञ्चिका);
        जइ (strcmpResult == 0) {
            प्रत्यागम &(मानम्_Strs[मध्यः[1]]);
        } अण्णहा जइ (strcmpResult < 0) {
            शिखरम् = मध्यः;
        } अण्णहा {
            bottom = मध्यः + 2;
        }
    }
    प्रत्यागम nullptr;
}

स्थिर CharString getDefaultScript(ध्रुव CharString& language, ध्रुव CharString& region) {
    ध्रुव वर्ण* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    जइ (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        जइ (U_FAILURE(err)) {
            प्रत्यागम result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    जइ (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    जइ (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    प्रत्यागम result;
}

गणना UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_मूलम्,
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
    URES_OPEN_LOCALE_मूलम्,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
प्रकारनिर्देश गणना UResOpenType UResOpenType;

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
स्थिर तार्किक getParentLocaleID(वर्ण *name, ध्रुव वर्ण *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    जइ (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        प्रत्यागम chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    जइ (U_FAILURE(err)) {
        // hopefully this never happens...
        प्रत्यागम chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    जइ (openType == URES_OPEN_LOCALE_DEFAULT_मूलम्) {
        ध्रुव वर्ण* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        जइ (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            प्रत्यागम सच्च;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    जइ (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        जइ (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } अण्णहा {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } अण्णहा जइ (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        जइ (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } अण्णहा {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } अण्णहा जइ (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        जइ (openType != URES_OPEN_LOCALE_DEFAULT_मूलम् || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } अण्णहा {
            प्रत्यागम मिच्छ;
        }
    } अण्णहा {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        प्रत्यागम मिच्छ;
    }
    जइ (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        प्रत्यागम सच्च;
    } अण्णहा {
        प्रत्यागम मिच्छ;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
स्थिर UBool mayHaveParent(वर्ण *name) {
    प्रत्यागम (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
स्थिर शून्य entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_गणनम्_Existing++;
    जाव(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_गणनम्_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
स्थिर UResourceDataEntry *getFallbackData(
        ध्रुव UResourceBundle *resBundle,
        ध्रुव वर्ण **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    जइ(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम nullptr;
    }
    जइ(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_कुञ्चिका(&(dataEntry->fData), dataEntry->fData.मूलम्_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    जइ(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        जाव(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            जइ(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_कुञ्चिका(&(dataEntry->fData), dataEntry->fData.मूलम्_Res, &indexR, resTag);
            }
        }
    }

    जइ(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    जइ(i>1) {
        जइ(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_मूलम्_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } अण्णहा {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    प्रत्यागम dataEntry;
}

स्थिर शून्य
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    जइ(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    जइ(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    जइ(entry->fPool != nullptr) {
        --entry->fPool->f_गणनम्_Existing;
    }
    alias = entry->fAlias;
    जइ(alias != nullptr) {
        जाव(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_गणनम्_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
स्थिर int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_अङ्कः = 0;
    ध्रुव UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    जइ (cache == nullptr) {
        प्रत्यागम 0;
    }

    करेहि {
        deletedMore = मिच्छ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        जाव ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            जइ (resB->f_गणनम्_Existing == 0) {
                rb_Deleted_अङ्कः++;
                deletedMore = सच्च;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } जाव(deletedMore);

    प्रत्यागम rb_Deleted_अङ्कः;
}

#ifdef URES_DEBUG
#अन्तर्भाव <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = मिच्छ;
  int32_t pos = UHASH_FIRST;
  ध्रुव UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    जइ (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      प्रत्यागम मिच्छ;
    }

    जाव ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=सच्च;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (शून्य*)resB, resB->f_गणनम्_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (शून्य*)resB->fPool,
              (शून्य*)resB->fAlias,
              (शून्य*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_गणनम्(cache));
    प्रत्यागम cacheNotEmpty;
}

#endif

स्थिर UBool U_CALLCONV ures_cleanup()
{
    जइ (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    प्रत्यागम सच्च;
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

स्थिर शून्य setEntryName(UResourceDataEntry *res, ध्रुव वर्ण *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    जइ(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    जइ (len < static_cast<int32_t>(परिमाण(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    अण्णहा {
        res->fName = static_cast<वर्ण*>(uprv_malloc(len + 1));
    }
    जइ(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अण्णहा {
        uprv_strcpy(res->fName, name);
    }
}

स्थिर UResourceDataEntry *
getPoolEntry(ध्रुव वर्ण *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
स्थिर UResourceDataEntry *init_entry(ध्रुव वर्ण *localeID, ध्रुव वर्ण *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    ध्रुव वर्ण *name;
    वर्ण aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    जइ(U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }

    /* here we try to deduce the right locale name */
    जइ(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } अण्णहा जइ(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_मूलम्_Locale_Name;
    } अण्णहा { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<वर्ण*>(name);
    find.fPath = const_cast<वर्ण*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    जइ(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(परिमाण(UResourceDataEntry)));
        जइ(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम nullptr;
        }

        uprv_memset(r, 0, परिमाण(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        जइ (U_FAILURE(*status)) {
            uprv_free(r);
            प्रत्यागम nullptr;
        }

        जइ(path != nullptr) {
            r->fPath = uprv_strdup(path);
            जइ(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                प्रत्यागम nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        जइ (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            जइ (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                प्रत्यागम nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } अण्णहा { /* if we have a regular entry */
            Resource aliasres;
            जइ (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                जइ (U_SUCCESS(*status)) {
                    ध्रुव int32_t *poolIndexes = r->fPool->fData.p_मूलम् + 1;
                    जइ(r->fData.p_मूलम्[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_कुञ्चिकाः = reinterpret_cast<ध्रुव वर्ण*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } अण्णहा {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } अण्णहा {
                    r->fBogus = *status;
                }
            }
            जइ (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                जइ (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ध्रुव char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    जइ(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            जइ ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (शून्य *)r, r, &cacheStatus);
                जइ (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } अण्णहा {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    जइ(r != nullptr) {
        /* return the real bundle */
        जाव(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_गणनम्_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        जइ(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    प्रत्यागम r;
}

स्थिर UResourceDataEntry *
getPoolEntry(ध्रुव वर्ण *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    जइ( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    प्रत्यागम poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
स्थिर UResourceDataEntry *
findFirstExisting(ध्रुव वर्ण* path, वर्ण* name, ध्रुव वर्ण* defaultLocale, UResOpenType openType,
                  UBool *is_मूलम्, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = मिच्छ;
    *foundParent = सच्च; /* we're starting with a fresh name */
    वर्ण origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    जाव(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        जइ (U_FAILURE(*status)) {
            प्रत्यागम nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        जइ(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_गणनम्_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } अण्णहा {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_मूलम् = static_cast<UBool>(uprv_strcmp(name, k_मूलम्_Locale_Name) == 0);

        /*Fallback data stuff*/
        जइ (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } अण्णहा {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        जइ (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    प्रत्यागम r;
}

स्थिर शून्य ures_set_Is_राशिः_Object( UResourceBundle* resB, UBool state) {
    जइ(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } अण्णहा {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

स्थिर UBool ures_is_राशिः_Object(ध्रुव UResourceBundle* resB) {
  प्रत्यागम((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?मिच्छ:सच्च);
}


U_CFUNC शून्य ures_init_राशिः_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, परिमाण(UResourceBundle));
  ures_set_Is_राशिः_Object(resB, सच्च);
}

U_NAMESPACE_BEGIN

राशिः_UResource_Bundle::राशिः_UResource_Bundle() {
    ures_init_राशिः_Object(&bundle);
}

राशिः_UResource_Bundle::~राशिः_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

स्थिर UBool  // returns U_SUCCESS(*status)
load_Parents_Except_मूलम्(UResourceDataEntry *&t1,
                      वर्ण name[], int32_t nameCapacity,
                      UBool usingUSRData, वर्ण usrDataPath[], UErrorCode *status) {
    जइ (U_FAILURE(*status)) { प्रत्यागम मिच्छ; }
    UBool checkParent = सच्च;
    जाव (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        जइ (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            ध्रुव char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            जइ(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                जइ (uprv_strcmp(name, k_मूलम्_Locale_Name) == 0) {
                    प्रत्यागम सच्च;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        जइ (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            प्रत्यागम मिच्छ;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        जइ (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            जइ (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                प्रत्यागम मिच्छ;
            }
        }

        जइ (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } अण्णहा {
            t1->fParent = t2;
            जइ (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_गणनम्_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    प्रत्यागम सच्च;
}

स्थिर UBool  // returns U_SUCCESS(*status)
निवेशय_मूलम्_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    जइ (U_FAILURE(*status)) { प्रत्यागम मिच्छ; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_मूलम्_Locale_Name, t1->fPath, &parentStatus);
    जइ (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        प्रत्यागम मिच्छ;
    }
    t1->fParent = t2;
    t1 = t2;
    प्रत्यागम सच्च;
}

स्थिर UResourceDataEntry *entryOpen(ध्रुव वर्ण* path, ध्रुव वर्ण* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = मिच्छ;
    UBool is_मूलम् = मिच्छ;
    UBool hasRealData = मिच्छ;
    UBool hasChopped = सच्च;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    वर्ण name[ULOC_FULLNAME_CAPACITY];
    वर्ण usrDataPath[96];

    initCache(status);

    जइ(U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }

    uprv_strncpy(name, localeID, परिमाण(name) - 1);
    name[परिमाण(name) - 1] = 0;

    जइ ( usingUSRData ) {
        जइ ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } अण्णहा {
            uprv_strncpy(usrDataPath, path, परिमाण(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[परिमाण(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ध्रुव वर्ण *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_मूलम्, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    जइ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        गच्छ finish;
    }

    जइ(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = सच्च;
        जइ ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            जइ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                गच्छ finish;
            }
            जइ ( u1 != nullptr ) {
                जइ(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } अण्णहा {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_गणनम्_Existing = 0;
                }
            }
        }
        जइ ((hasChopped || mayHaveParent(name)) && !is_मूलम्) {
            जइ (!load_Parents_Except_मूलम्(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                गच्छ finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    जइ(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_मूलम् && !isDefault && !is_मूलम्) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_मूलम्, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        जइ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            गच्छ finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        जइ(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = सच्च;
            isDefault = सच्च;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            जइ ((hasChopped || mayHaveParent(name)) && !is_मूलम्) {
                जइ (!load_Parents_Except_मूलम्(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    गच्छ finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    जइ(r == nullptr) {
        uprv_strcpy(name, k_मूलम्_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_मूलम्, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        जइ (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            गच्छ finish;
        }
        जइ(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = सच्च;
        } अण्णहा { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            गच्छ finish;
        }
    } अण्णहा जइ(!is_मूलम् && uprv_strcmp(t1->fName, k_मूलम्_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        जइ (!निवेशय_मूलम्_Bundle(t1, status)) {
            गच्छ finish;
        }
        जइ(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    जाव(r != nullptr && !is_मूलम् && t1->fParent != nullptr) {
        t1->fParent->f_गणनम्_Existing++;
        t1 = t1->fParent;
    }

finish:
    जइ(U_SUCCESS(*status)) {
        जइ(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        प्रत्यागम r;
    } अण्णहा {
        प्रत्यागम nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
स्थिर UResourceDataEntry *
entryOpenDirect(ध्रुव वर्ण* path, ध्रुव वर्ण* localeID, UErrorCode* status) {
    initCache(status);
    जइ(U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    जइ (localeID == nullptr) {
        localeID = uloc_getDefault();
    } अण्णहा जइ (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_मूलम्_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    जइ(U_SUCCESS(*status)) {
        जइ(r->fBogus != U_ZERO_ERROR) {
            r->f_गणनम्_Existing--;
            r = nullptr;
        }
    } अण्णहा {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    जइ(r != nullptr && uprv_strcmp(localeID, k_मूलम्_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        वर्ण name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        जइ(!chopLocale(name) || uprv_strcmp(name, k_मूलम्_Locale_Name) == 0 ||
                load_Parents_Except_मूलम्(t1, name, UPRV_LENGTHOF(name), मिच्छ, nullptr, status)) {
            जइ(uprv_strcmp(t1->fName, k_मूलम्_Locale_Name) != 0 && t1->fParent == nullptr) {
                निवेशय_मूलम्_Bundle(t1, status);
            }
        }
        जइ(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    जइ(r != nullptr) {
        // TODO: Does this ever loop?
        जाव(t1->fParent != nullptr) {
            t1->fParent->f_गणनम्_Existing++;
            t1 = t1->fParent;
        }
    }
    प्रत्यागम r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
स्थिर शून्य entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    जाव(resB != nullptr) {
        p = resB->fParent;
        resB->f_गणनम्_Existing--;

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
स्थिर शून्य ures_appendResPath(UResourceBundle *resB, ध्रुव वर्ण* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    जइ(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    जइ(RES_BUFSIZE <= resB->fResPathLen+1) {
        जइ(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<वर्ण*>(uprv_malloc((resB->fResPathLen + 1) * परिमाण(वर्ण)));
            /* Check that memory was allocated correctly. */
            जइ (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } अण्णहा {
            वर्ण* temp = static_cast<वर्ण*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * परिमाण(वर्ण)));
            /* Check that memory was reallocated correctly. */
            जइ (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

स्थिर शून्य ures_freeResPath(UResourceBundle *resB) {
    जइ (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

स्थिर शून्य
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    जइ(resB != nullptr) {
        जइ(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        जइ(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        जइ(ures_is_राशिः_Object(resB) == मिच्छ && freeBundleObj) {
            uprv_free(resB);
        }
#जइ 0 /*U_DEBUG*/
        अण्णहा {
            /* poison the data */
            uprv_memset(resB, -1, परिमाण(UResourceBundle));
        }
#endif
    }
}

U_CAPI शून्य  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, सच्च);
}

नामाकाश {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ध्रुव वर्ण* कुञ्चिका,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ध्रुव वर्ण* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(ध्रुव ResourceData& resData,
                               Resource r,
                               ध्रुव वर्ण* कुञ्चिका,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               ध्रुव वर्ण* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    जइ (U_FAILURE(*status)) { प्रत्यागम resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ध्रुव char16_t *alias = res_getAlias(&resData, r, &len);
    जइ(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम resB;
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
    जइ (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }

    // We have an alias, now let's cut it up.
    ध्रुव वर्ण *path = nullptr, *locale = nullptr, *कुञ्चिका_Path = nullptr;
    जइ(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        वर्ण *chAliasData = chAlias.data();
        वर्ण *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        जइ(sep != nullptr) {
            *sep++ = 0;
        }
        जइ(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            कुञ्चिका_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } अण्णहा {
            जइ(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            जइ (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } अण्णहा {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                जइ(sep != nullptr) {
                    *sep++ = 0;
                }
                कुञ्चिका_Path = sep;
            }
        }
    } अण्णहा {
        // No path, start with a locale.
        वर्ण *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        जइ(sep != nullptr) {
            *sep++ = 0;
        }
        कुञ्चिका_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    जइ (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } अण्णहा {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        जइ(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            प्रत्यागम resB;
        }
        dataEntry = mainRes->fData;
    }

    ध्रुव वर्ण* temp = nullptr;
    जइ(कुञ्चिका_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.मूलम्_Res;
        जइ(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            जइ (U_FAILURE(*status)) {
                प्रत्यागम nullptr;
            }
            वर्ण *a_कुञ्चिका = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_कुञ्चिका, &temp);
        }
        जइ(कुञ्चिका) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(कुञ्चिका, *status);
            जइ (U_FAILURE(*status)) {
                प्रत्यागम nullptr;
            }
            वर्ण *a_कुञ्चिका = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_कुञ्चिका, &temp);
        } अण्णहा जइ(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            जइ(URES_IS_TABLE(type)) {
                ध्रुव वर्ण *a_कुञ्चिका;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_कुञ्चिका);
            } अण्णहा { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        जइ(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } अण्णहा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } अण्णहा {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(कुञ्चिका_Path, *status);
        जइ (U_FAILURE(*status)) {
            प्रत्यागम nullptr;
        }
        वर्ण *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        प्रति(;;) {
            r = dataEntry->fData.मूलम्_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            जाव(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                जइ(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    भङ्ग;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                जइ (U_FAILURE(*status)) {
                    भङ्ग;
                }
                जइ (temp == nullptr || uprv_strcmp(कुञ्चिका_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, कुञ्चिका_Path, static_cast<int32_t>(uprv_strlen(कुञ्चिका_Path)), status);
                    जइ(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    जइ (U_FAILURE(*status)) {
                        भङ्ग;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            जइ (U_FAILURE(*status) || r != RES_BOGUS) {
                भङ्ग;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            जइ (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                भङ्ग;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, कुञ्चिका_Path);
        }
    }
    जइ(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    प्रत्यागम resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ध्रुव वर्ण* कुञ्चिका,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  ध्रुव वर्ण* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    जइ(status == nullptr || U_FAILURE(*status)) {
        प्रत्यागम resB;
    }
    जइ (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    जइ(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        जइ(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            प्रत्यागम resB;
        }
        प्रत्यागम getAliasTargetAsResourceBundle(
            dataEntry->fData, r, कुञ्चिका, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    जइ(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
        जइ (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम nullptr;
        }
        ures_set_Is_राशिः_Object(resB, मिच्छ);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } अण्णहा {
        जइ(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        जइ(resB->fVersion != nullptr) {
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
        जइ(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = मिच्छ;
    resB->f_Is_शिखरम्_Level = मिच्छ;
    resB->fIndex = -1;
    resB->f_कुञ्चिका = कुञ्चिका; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    जइ(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    जइ(कुञ्चिका != nullptr) {
        ures_appendResPath(resB, कुञ्चिका, static_cast<int32_t>(uprv_strlen(कुञ्चिका)), status);
        जइ(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } अण्णहा जइ(idx >= 0) {
        वर्ण buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        जइ(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, परिमाण(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_गणनम्_Array_वस्तूनि(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    प्रत्यागम resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  ध्रुव वर्ण* कुञ्चिका,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ध्रुव UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    प्रत्यागम init_resb_result(
        dataEntry, r, कुञ्चिका, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ध्रुव UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_राशिः_Object;
    जइ(U_FAILURE(*status) || r == original) {
        प्रत्यागम r;
    }
    जइ(original != nullptr) {
        जइ(r == nullptr) {
            is_राशिः_Object = मिच्छ;
            r = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
            /* test for nullptr */
            जइ (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम nullptr;
            }
        } अण्णहा {
            is_राशिः_Object = ures_is_राशिः_Object(r);
            ures_closeBundle(r, मिच्छ);
        }
        uprv_memcpy(r, original, परिमाण(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        जइ(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_राशिः_Object(r, is_राशिः_Object);
        जइ(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    प्रत्यागम r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ध्रुव char16_t* U_EXPORT2 ures_getString(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    ध्रुव char16_t *s;
    जइ (status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    जइ (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम s;
}

स्थिर ध्रुव वर्ण*
ures_toUTF8String(ध्रुव char16_t* s16,
                  int32_t length16,
                  वर्ण* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    जइ (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ (pLength != nullptr) {
        capacity = *pLength;
    } अण्णहा {
        capacity = 0;
    }
    जइ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ (length16 == 0) {
        /* empty string, return as read-only pointer */
        जइ (pLength != nullptr) {
            *pLength = 0;
        }
        जइ (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            प्रत्यागम dest;
        } अण्णहा {
            प्रत्यागम "";
        }
    } अण्णहा {
        /* We need to transform the string to the destination buffer. */
        जइ (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            प्रत्यागम u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        जइ (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            जइ (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        प्रत्यागम u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getUTF8String(ध्रुव UResourceBundle* resB,
                   वर्ण* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    ध्रुव char16_t *s16 = ures_getString(resB, &length16, status);
    प्रत्यागम ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI ध्रुव uint8_t* U_EXPORT2
ures_get_द्विमान(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ध्रुव uint8_t *p;
  जइ (status==nullptr || U_FAILURE(*status)) {
    प्रत्यागम nullptr;
  }
  जइ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम nullptr;
  }
  p = res_get_द्विमान({resB}, &resB->getResData(), resB->fRes, len);
  जइ (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  प्रत्यागम p;
}

U_CAPI ध्रुव int32_t* U_EXPORT2
ures_getIntVector(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  ध्रुव int32_t *p;
  जइ (status==nullptr || U_FAILURE(*status)) {
    प्रत्यागम nullptr;
  }
  जइ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  जइ (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  प्रत्यागम p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ध्रुव UResourceBundle* resB, UErrorCode *status) {
  जइ (status==nullptr || U_FAILURE(*status)) {
    प्रत्यागम 0xffffffff;
  }
  जइ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0xffffffff;
  }
  जइ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    प्रत्यागम 0xffffffff;
  }
  प्रत्यागम res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ध्रुव UResourceBundle* resB, UErrorCode *status) {
  जइ (status==nullptr || U_FAILURE(*status)) {
    प्रत्यागम 0xffffffff;
  }
  जइ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0xffffffff;
  }
  जइ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    प्रत्यागम 0xffffffff;
  }
  प्रत्यागम res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ध्रुव UResourceBundle *resB) {
  जइ(resB == nullptr) {
    प्रत्यागम URES_NONE;
  }
  प्रत्यागम res_getPublicType(resB->fRes);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_कुञ्चिका(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  जइ(resB == nullptr) {
    प्रत्यागम nullptr;
  }
  प्रत्यागम(resB->f_कुञ्चिका);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(ध्रुव UResourceBundle *resB) {
  जइ(resB == nullptr) {
    प्रत्यागम 0;
  }
  
  प्रत्यागम resB->fSize;
}

स्थिर ध्रुव char16_t*
ures_getStringWithAlias(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  जइ(RES_GET_TYPE(r) == URES_ALIAS) {
    ध्रुव char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    प्रत्यागम result;
  } अण्णहा {
    प्रत्यागम res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI शून्य U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  जइ(resB == nullptr) {
    प्रत्यागम;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(ध्रुव UResourceBundle *resB) {
  जइ(resB == nullptr) {
    प्रत्यागम मिच्छ;
  }
  प्रत्यागम resB->fIndex < resB->fSize-1;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ध्रुव वर्ण** कुञ्चिका,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  जइ (status==nullptr || U_FAILURE(*status)) {
    प्रत्यागम nullptr;
  }
  जइ(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम nullptr;
  }
  
  जइ(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } अण्णहा {
    resB->fIndex++;
    विकल्प(RES_GET_TYPE(resB->fRes)) {
    पक्ष URES_STRING:
    पक्ष URES_STRING_V2:
      प्रत्यागम res_getString({resB}, &resB->getResData(), resB->fRes, len);
    पक्ष URES_TABLE:
    पक्ष URES_TABLE16:
    पक्ष URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, कुञ्चिका);
      जइ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      प्रत्यागम ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    पक्ष URES_ARRAY:
    पक्ष URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      जइ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      प्रत्यागम ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    पक्ष URES_ALIAS:
      प्रत्यागम ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    पक्ष URES_INT:
    पक्ष URES_द्विमान:
    पक्ष URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    स्वभाव:
      प्रत्यागम nullptr;
    }
  }

  प्रत्यागम nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    ध्रुव वर्ण *कुञ्चिका = nullptr;
    Resource r = RES_BOGUS;

    जइ (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            प्रत्यागम fillIn;
    }
    जइ(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            प्रत्यागम fillIn;
    }

    जइ(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } अण्णहा {
        resB->fIndex++;
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_INT:
        पक्ष URES_द्विमान:
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
        पक्ष URES_INT_VECTOR:
            प्रत्यागम ures_copyResb(fillIn, resB, status);
        पक्ष URES_TABLE:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &कुञ्चिका);
            जइ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम init_resb_result(resB->fData, r, कुञ्चिका, resB->fIndex, resB, fillIn, status);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            जइ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम init_resb_result(resB->fData, r, कुञ्चिका, resB->fIndex, resB, fillIn, status);
        स्वभाव:
            /*return nullptr;*/
            प्रत्यागम fillIn;
        }
    }
    /*return nullptr;*/
    प्रत्यागम fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(ध्रुव UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    ध्रुव वर्ण* कुञ्चिका = nullptr;
    Resource r = RES_BOGUS;

    जइ (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        प्रत्यागम fillIn;
    }
    जइ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        प्रत्यागम fillIn;
    }

    जइ(indexR >= 0 && resB->fSize > indexR) {
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_INT:
        पक्ष URES_द्विमान:
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
        पक्ष URES_INT_VECTOR:
            प्रत्यागम ures_copyResb(fillIn, resB, status);
        पक्ष URES_TABLE:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &कुञ्चिका);
            जइ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम init_resb_result(resB->fData, r, कुञ्चिका, indexR, resB, fillIn, status);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            जइ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम init_resb_result(resB->fData, r, कुञ्चिका, indexR, resB, fillIn, status);
        स्वभाव:
            /*return nullptr;*/
            प्रत्यागम fillIn;
        }
    } अण्णहा {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    प्रत्यागम fillIn;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ures_getStringByIndex(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    ध्रुव वर्ण* कुञ्चिका = nullptr;
    Resource r = RES_BOGUS;

    जइ (status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(indexS >= 0 && resB->fSize > indexS) {
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
            प्रत्यागम res_getString({resB}, &resB->getResData(), resB->fRes, len);
        पक्ष URES_TABLE:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &कुञ्चिका);
            जइ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम ures_getStringWithAlias(resB, r, indexS, len, status);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            जइ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम ures_getStringWithAlias(resB, r, indexS, len, status);
        पक्ष URES_ALIAS:
            प्रत्यागम ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        पक्ष URES_INT:
        पक्ष URES_द्विमान:
        पक्ष URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            भङ्ग;
        स्वभाव:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          भङ्ग;
        }
    } अण्णहा {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    प्रत्यागम nullptr;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getUTF8StringByIndex(ध्रुव UResourceBundle* resB,
                          int32_t idx,
                          वर्ण* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    ध्रुव char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    प्रत्यागम ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(ध्रुव वर्ण* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  वर्ण *packageName = nullptr;
  वर्ण *pathToResource = nullptr, *save = nullptr;
  वर्ण *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  जइ(status == nullptr || U_FAILURE(*status)) {
    प्रत्यागम result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (वर्ण *)uprv_malloc(length*परिमाण(वर्ण));
  /* test for nullptr */
  जइ(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    प्रत्यागम result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  जइ(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    जइ(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } अण्णहा {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  जइ(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  जइ(U_SUCCESS(*status)) {
    जइ(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } अण्णहा {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  प्रत्यागम result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(ध्रुव UResourceBundle* resB,
                     वर्ण* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  ध्रुव वर्ण *कुञ्चिका;

  जइ(status == nullptr || U_FAILURE(*status)) {
    प्रत्यागम result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  करेहि {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &कुञ्चिका); 
    जइ(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, कुञ्चिका, -1, resB, fillIn, status);
        resB = result;
    } अण्णहा {
        *status = U_MISSING_RESOURCE_ERROR;
        भङ्ग;
    }
  } जाव(*path); /* there is more stuff in the path */

  प्रत्यागम result;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ures_get_String_By_कुञ्चिका_With_Fallback(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                                ध्रुव वर्ण* in_कुञ्चिका,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle राशिः_2;
    ध्रुव char16_t* retVal = nullptr;
    ures_init_राशिः_Object(&राशिः_2);
    ures_get_By_कुञ्चिका_With_Fallback(resB, in_कुञ्चिका, &राशिः_2, status);
    int32_t length;
    retVal = ures_getString(&राशिः_2, &length, status);
    ures_close(&राशिः_2);
    जइ (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    जइ (len != nullptr) {
        *len = length;
    }
    प्रत्यागम retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
स्थिर Resource get_Table_Item_By_कुञ्चिका_Path(ध्रुव ResourceData *pResData, Resource table, ध्रुव वर्ण *कुञ्चिका) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(कुञ्चिका, errorCode);
  जइ (U_FAILURE(errorCode)) { प्रत्यागम RES_BOGUS; }
  वर्ण *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  जाव (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    वर्ण *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    जइ (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } अण्णहा {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    ध्रुव वर्ण *pathP = pathPart;
    resource = res_get_Table_Item_By_कुञ्चिका(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  जइ (*pathPart) {
    प्रत्यागम RES_BOGUS;
  }
  प्रत्यागम resource;
}

स्थिर शून्य createPath(ध्रुव वर्ण* origResPath,
                       int32_t     origResPathLen,
                       ध्रुव वर्ण* resPath,
                       int32_t     resPathLen,
                       ध्रुव वर्ण* in_कुञ्चिका,
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
    ध्रुव वर्ण* कुञ्चिका = in_कुञ्चिका;
    जइ (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        जइ (U_SUCCESS(*status)) {
            ध्रुव वर्ण* resPathLimit = resPath + resPathLen;
            ध्रुव वर्ण* origResPathLimit = origResPath + origResPathLen;
            ध्रुव वर्ण* resPathPtr = resPath;
            ध्रुव वर्ण* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            जाव (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                जाव (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                जइ (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                जाव (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                जइ (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            जाव (resPathPtr < resPathLimit && *कुञ्चिका != '\0') {
                जाव (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                जइ (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                जाव (*कुञ्चिका != '\0' && *कुञ्चिका != RES_PATH_SEPARATOR) {
                    ++कुञ्चिका;
                }
                जइ (*कुञ्चिका == RES_PATH_SEPARATOR) {
                    ++कुञ्चिका;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(कुञ्चिका, *status);
    } अण्णहा {
        path.append(in_कुञ्चिका, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_कुञ्चिका_With_Fallback(ध्रुव UResourceBundle* resB,
                          ध्रुव वर्ण* in_कुञ्चिका,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, मूलम्_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    जइ (status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम fillIn;
    }
    जइ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    जइ(URES_IS_TABLE(type)) {
        ध्रुव वर्ण* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_कुञ्चिका_Path(&resB->getResData(), resB->fRes, in_कुञ्चिका);
        ध्रुव वर्ण* कुञ्चिका = in_कुञ्चिका;
        तार्किक did_मूलम्_Once = मिच्छ;
        जइ(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            वर्ण *myPath = nullptr;
            ध्रुव वर्ण* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            जाव(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_मूलम्_Once)) { /* Otherwise, we'll look in parents */
                जइ (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } अण्णहा {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_मूलम्_Once = सच्च;
                }
                मूलम्_Res = dataEntry->fData.मूलम्_Res;

                जइ(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_कुञ्चिका, path, status);
                    जइ (U_FAILURE(*status)) {
                        ures_close(helper);
                        प्रत्यागम fillIn;
                    }
                    myPath = path.data();
                    कुञ्चिका = in_कुञ्चिका;
                    करेहि {
                        res = res_findResource(&(dataEntry->fData), मूलम्_Res, &myPath, &कुञ्चिका);
                        जइ (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            जइ(helper) {
                              dataEntry = helper->fData;
                              मूलम्_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } अण्णहा {
                              भङ्ग;
                            }
                        } अण्णहा जइ (res == RES_BOGUS) {
                            भङ्ग;
                        }
                    } जाव(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            जइ(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                जइ(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_मूलम्_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } अण्णहा {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, कुञ्चिका, -1, resB, fillIn, status);
                जइ (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_कुञ्चिका, path, status);
                } अण्णहा {
                    ध्रुव वर्ण* separator = nullptr;
                    जइ (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    जइ (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_कुञ्चिका, path, status);
                    } अण्णहा {
                        createPath(origResPath, origResPathLen, "", 0, in_कुञ्चिका, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                जइ(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } अण्णहा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अण्णहा {
            fillIn = init_resb_result(resB->fData, res, कुञ्चिका, -1, resB, fillIn, status);
        }
    } 
    अण्णहा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    प्रत्यागम fillIn;
}

नामाकाश {

शून्य get_All_वस्तूनि_With_Fallback(
        ध्रुव UResourceBundle *bundle, Resource_Data_मानम् &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
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
    sink.put(bundle->f_कुञ्चिका, value, !hasParent, errorCode);
    जइ (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        राशिः_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_शिखरम्_Level = सच्च;
        parentRef.fRes = parentRef.getResData().मूलम्_Res;
        parentRef.fSize = res_गणनम्_Array_वस्तूनि(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        राशिः_UResource_Bundle containerBundle;
        ध्रुव UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        जइ (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } अण्णहा {
            rb = ures_get_By_कुञ्चिका_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        जइ (U_SUCCESS(pathErrorCode)) {
            get_All_वस्तूनि_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

संरचना GetAllChildrenSink : सार्वजनिक ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    आभास ~GetAllChildrenSink() override;
    आभास शून्य put(ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value, UBool is_मूलम्,
           UErrorCode &errorCode) override {
        ResourceTable वस्तूनि_Table = value.getTable(errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
        प्रति (int32_t i = 0; वस्तूनि_Table.get_कुञ्चिका_And_मानम्(i, कुञ्चिका, value); ++i) {
            जइ (value.getType() == URES_ALIAS) {
                Resource_Data_मानम्& rdv = static_cast<Resource_Data_मानम्&>(value);
                राशिः_UResource_Bundle राशिः_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          राशिः_Temp_Bundle.getAlias(), &errorCode);
                जइ (U_SUCCESS(errorCode)) {
                    Resource_Data_मानम् aliased_मानम्;
                    aliased_मानम्.setData(aliasRB->getResData());
                    aliased_मानम्.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_मानम्.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    जइ (aliased_मानम्.getType() != URES_TABLE) {
                        dest.put(कुञ्चिका, aliased_मानम्, is_मूलम्, errorCode);
                    } अण्णहा {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_मानम्_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        ध्रुव वर्ण* parent_कुञ्चिका = कुञ्चिका; // dest.put() changes the key
                        dest.put(parent_कुञ्चिका, aliased_मानम्, is_मूलम्, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        जाव (aliased_मानम्_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            वर्ण* localPathAsCharPtr = localPath.data();
                            ध्रुव वर्ण* child_कुञ्चिका;
                            entry = entry->fParent;
                            res = entry->fData.मूलम्_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_कुञ्चिका);
                            जइ (newRes != RES_BOGUS) {
                                aliased_मानम्.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_मानम्.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_मानम्_Type = aliased_मानम्.getType();
                                जइ (aliased_मानम्_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_मानम्& rdv2 = static_cast<Resource_Data_मानम्&>(aliased_मानम्);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             राशिः_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_मानम्.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_मानम्.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_मानम्_Type = aliased_मानम्.getType();
                                }
                                जइ (aliased_मानम्_Type == URES_TABLE) {
                                    dest.put(parent_कुञ्चिका, aliased_मानम्, is_मूलम्, errorCode);
                                } अण्णहा {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    प्रत्यागम;
                                }
                            }
                        }
                    }
                }
            } अण्णहा {
                dest.put(कुञ्चिका, value, is_मूलम्, errorCode);
            }
            जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI शून्य U_EXPORT2
ures_getAllChildrenWithFallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_वस्तूनि_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
ures_get_मानम्_With_Fallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_मानम् &value, UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    जइ (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    ध्रुव UResourceBundle *rb;
    जइ (*path == 0) {
        // empty path
        rb = bundle;
    } अण्णहा {
        rb = ures_get_By_कुञ्चिका_With_Fallback(bundle, path, tempFillIn, &errorCode);
        जइ (U_FAILURE(errorCode)) {
            प्रत्यागम;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI शून्य U_EXPORT2
ures_get_All_वस्तूनि_With_Fallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    जइ (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    राशिः_UResource_Bundle राशिः_Bundle;
    ध्रुव UResourceBundle *rb;
    जइ (*path == 0) {
        // empty path
        rb = bundle;
    } अण्णहा {
        rb = ures_get_By_कुञ्चिका_With_Fallback(bundle, path, राशिः_Bundle.getAlias(), &errorCode);
        जइ (U_FAILURE(errorCode)) {
            प्रत्यागम;
        }
    }
    // Get all table items with fallback.
    Resource_Data_मानम् value;
    get_All_वस्तूनि_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_कुञ्चिका(ध्रुव UResourceBundle* resB,
              ध्रुव वर्ण* in_कुञ्चिका,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ध्रुव वर्ण *कुञ्चिका = in_कुञ्चिका;

    जइ (status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम fillIn;
    }
    जइ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    जइ(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_कुञ्चिका(&resB->getResData(), resB->fRes, &t, &कुञ्चिका);
        जइ(res == RES_BOGUS) {
            कुञ्चिका = in_कुञ्चिका;
            जइ(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &कुञ्चिका, &res, status);
                जइ(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    प्रत्यागम init_resb_result(dataEntry, res, कुञ्चिका, -1, resB, fillIn, status);
                } अण्णहा {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } अण्णहा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अण्णहा {
            प्रत्यागम init_resb_result(resB->fData, res, कुञ्चिका, -1, resB, fillIn, status);
        }
    } 
#जइ 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    अण्णहा जइ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == सच्च) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &कुञ्चिका, &res, status);
        जइ(U_SUCCESS(*status)) {
            प्रत्यागम init_resb_result(dataEntry, res, कुञ्चिका, resB, fillIn, status);
        } अण्णहा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    अण्णहा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम fillIn;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ures_get_String_By_कुञ्चिका(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                    ध्रुव वर्ण* in_कुञ्चिका,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    ध्रुव वर्ण* कुञ्चिका = in_कुञ्चिका;

    जइ (status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    जइ(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_कुञ्चिका(&resB->getResData(), resB->fRes, &t, &कुञ्चिका);

        जइ(res == RES_BOGUS) {
            कुञ्चिका = in_कुञ्चिका;
            जइ(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &कुञ्चिका, &res, status);
                जइ(U_SUCCESS(*status)) {
                    विकल्प (RES_GET_TYPE(res)) {
                    पक्ष URES_STRING:
                    पक्ष URES_STRING_V2:
                        प्रत्यागम res_getString({resB, कुञ्चिका}, &dataEntry->fData, res, len);
                    पक्ष URES_ALIAS:
                      {
                        ध्रुव char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_कुञ्चिका(resB, in_कुञ्चिका, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        प्रत्यागम result;
                      }
                    स्वभाव:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } अण्णहा {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } अण्णहा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अण्णहा {
            विकल्प (RES_GET_TYPE(res)) {
            पक्ष URES_STRING:
            पक्ष URES_STRING_V2:
                प्रत्यागम res_getString({resB, कुञ्चिका}, &resB->getResData(), res, len);
            पक्ष URES_ALIAS:
              {
                ध्रुव char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_कुञ्चिका(resB, in_कुञ्चिका, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                प्रत्यागम result;
              }
            स्वभाव:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#जइ 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    अण्णहा जइ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == सच्च) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &कुञ्चिका, &res, status);
        जइ(U_SUCCESS(*status)) {
            // TODO: Tracing
            प्रत्यागम res_getString(rd, res, len);
        } अण्णहा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    अण्णहा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम nullptr;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_UTF8_String_By_कुञ्चिका(ध्रुव UResourceBundle* resB,
                        ध्रुव वर्ण* कुञ्चिका,
                        वर्ण* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    ध्रुव char16_t *s16 = ures_get_String_By_कुञ्चिका(resB, कुञ्चिका, &length16, status);
    प्रत्यागम ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getLocaleInternal(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    जइ (status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    } अण्णहा {
      प्रत्यागम resourceBundle->fData->fName;
    }
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getLocale(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  प्रत्यागम ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getLocaleByType(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    जइ (status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    } अण्णहा {
        विकल्प(type) {
        पक्ष ULOC_ACTUAL_LOCALE:
            प्रत्यागम resourceBundle->fData->fName;
        पक्ष ULOC_VALID_LOCALE:
            प्रत्यागम resourceBundle->fValidLocaleDataEntry->fName;
        पक्ष ULOC_REQUESTED_LOCALE:
        स्वभाव:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम nullptr;
        }
    }
}

U_CFUNC ध्रुव वर्ण*
ures_getName(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND) {
  जइ(resB == nullptr) {
    प्रत्यागम nullptr;
  }

  प्रत्यागम resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC ध्रुव वर्ण*
ures_getPath(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND) {
  जइ(resB == nullptr) {
    प्रत्यागम nullptr;
  }

  प्रत्यागम resB->fData->fPath;
}
#endif

स्थिर UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ध्रुव वर्ण* path,
                  ध्रुव वर्ण* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    जइ(U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }

    UResourceDataEntry *entry;
    जइ(openType != URES_OPEN_DIRECT) {
        जइ (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        जइ(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } अण्णहा {
        entry = entryOpenDirect(path, localeID, status);
    }
    जइ(U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम nullptr;
    }

    UBool is_राशिः_Object;
    जइ(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
        जइ(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम nullptr;
        }
        is_राशिः_Object = मिच्छ;
    } अण्णहा {  // fill-in
        is_राशिः_Object = ures_is_राशिः_Object(r);
        ures_closeBundle(r, मिच्छ);
    }
    uprv_memset(r, 0, परिमाण(UResourceBundle));
    ures_set_Is_राशिः_Object(r, is_राशिः_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_शिखरम्_Level = सच्च;
    r->fRes = r->getResData().मूलम्_Res;
    r->fSize = res_गणनम्_Array_वस्तूनि(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    प्रत्यागम r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ध्रुव वर्ण* path, ध्रुव वर्ण* localeID, UErrorCode* status) {
    प्रत्यागम ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_मूलम्, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ध्रुव वर्ण* path, ध्रुव वर्ण* localeID, UErrorCode* status) {
    प्रत्यागम ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_मूलम्, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(ध्रुव वर्ण* path, ध्रुव वर्ण* localeID, UErrorCode* status) {
    प्रत्यागम ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI शून्य U_EXPORT2
ures_openFillIn(UResourceBundle *r, ध्रुव वर्ण* path,
                ध्रुव वर्ण* localeID, UErrorCode* status) {
    जइ(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_मूलम्, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI शून्य U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, ध्रुव वर्ण* path, ध्रुव वर्ण* localeID, UErrorCode* status) {
    जइ(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_गणनम्_Array_वस्तूनि(ध्रुव UResourceBundle* resourceBundle,
                  ध्रुव वर्ण* resource_कुञ्चिका,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_राशिः_Object(&resData);
    जइ (status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम 0;
    }
    जइ(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    ures_get_By_कुञ्चिका(resourceBundle, resource_कुञ्चिका, &resData, status);
    
    जइ(resData.getResData().data != nullptr) {
        int32_t result = res_गणनम्_Array_वस्तूनि(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        प्रत्यागम result;
    } अण्णहा {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        प्रत्यागम 0;
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
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_Version_सङ्ख्या_Internal(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    जइ (!resourceBundle) प्रत्यागम nullptr;

    जइ(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ध्रुव char16_t* minor_version = ures_get_String_By_कुञ्चिका(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (वर्ण *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        जइ (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            प्रत्यागम nullptr;
        }
       
        जइ(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        अण्णहा {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    प्रत्यागम resourceBundle->fVersion;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_Version_सङ्ख्या(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    प्रत्यागम ures_get_Version_सङ्ख्या_Internal(resourceBundle);
}

U_CAPI शून्य U_EXPORT2 ures_getVersion(ध्रुव UResourceBundle* resB, UVersionInfo versionInfo) {
    जइ (!resB) प्रत्यागम;

    u_versionFromString(versionInfo, ures_get_Version_सङ्ख्या_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#जइ defined(URES_वृक्षः_DEBUG)
#अन्तर्भाव <stdio.h>
#endif

प्रकारनिर्देश संरचना ULocalesContext {
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
ures_loc_गणनम्_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    प्रत्यागम ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


स्थिर ध्रुव वर्ण * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    ध्रुव वर्ण *result = nullptr;
    int32_t len = 0;
    जइ (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_कुञ्चिका(k);
        len = (int32_t)uprv_strlen(result);
    }
    जइ (resultLength) {
        *resultLength = len;
    }
    प्रत्यागम result;
}

स्थिर शून्य U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

स्थिर ध्रुव UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_गणनम्_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(ध्रुव वर्ण *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    जइ(U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(परिमाण(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(परिमाण(UEnumeration));
    जइ(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        प्रत्यागम nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, परिमाण(UEnumeration));

    ures_init_राशिः_Object(&myContext->installed);
    ures_init_राशिः_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_कुञ्चिका(idx, INDEX_TAG, &myContext->installed, status);
    जइ(U_SUCCESS(*status)) {
#जइ defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_कुञ्चिका(&myContext->installed));
#endif
        en->context = myContext;
    } अण्णहा {
#जइ defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    प्रत्यागम en;
}

स्थिर UBool isLocaleInList(UEnumeration *locEnum, ध्रुव वर्ण *loc_To_अन्वेषणम्, UErrorCode *status) {
    ध्रुव वर्ण *loc;
    जाव ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        जइ (uprv_strcmp(loc, loc_To_अन्वेषणम्) == 0) {
            प्रत्यागम सच्च;
        }
    }
    प्रत्यागम मिच्छ;
}

स्थिर शून्य getParentForFunctionalEquivalent(ध्रुव वर्ण*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    जइ (res != nullptr) {
        ures_get_By_कुञ्चिका(res, "%%Parent", bund1, &subStatus);
        जइ (U_SUCCESS(subStatus)) {
            int32_t length16;
            ध्रुव char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    जइ (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(वर्ण *result, int32_t resultCapacity,
                             ध्रुव वर्ण *path, ध्रुव वर्ण *resName, ध्रुव वर्ण *keyword, ध्रुव वर्ण *locid,
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
    जइ(U_FAILURE(*status)) प्रत्यागम 0;
    CharString kwVal;
    जइ (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_मानम्(locid, keyword, subStatus);
        जइ (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    जइ (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#जइ defined(URES_वृक्षः_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_राशिः_Object(&bund1);
    ures_init_राशिः_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    जइ(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = सच्च;
        जइ (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    जइ(U_FAILURE(subStatus)) {
        *status = subStatus;
        प्रत्यागम 0;
    }
    
    करेहि {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        जइ(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = मिच्छ;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#जइ defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        जइ(U_FAILURE(subStatus)) {
            *status = subStatus;
        } अण्णहा जइ(subStatus == U_ZERO_ERROR) {
            ures_get_By_कुञ्चिका(res,resName,&bund1, &subStatus);
            जइ(subStatus == U_ZERO_ERROR) {
                ध्रुव char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#जइ defined(URES_वृक्षः_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_कुञ्चिका(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                जइ(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#जइ defined(URES_वृक्षः_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    जइ(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#जइ defined(URES_वृक्षः_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        जइ (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        जइ (found != parent) {
            parent.copyFrom(found, subStatus);
        } अण्णहा {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } जाव(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    करेहि {
        res = ures_open(path, parent.data(), &subStatus);
        जइ((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = मिच्छ;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#जइ defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        जइ(U_FAILURE(subStatus)) {
            *status = subStatus;
        } अण्णहा जइ(subStatus == U_ZERO_ERROR) {
            ures_get_By_कुञ्चिका(res,resName,&bund1, &subStatus);
#जइ defined(URES_वृक्षः_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            जइ(subStatus == U_ZERO_ERROR) {
                ures_get_By_कुञ्चिका(&bund1, kwVal.data(), &bund2, &subStatus);
#जइ defined(URES_वृक्षः_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                जइ(subStatus == U_ZERO_ERROR) {
#जइ defined(URES_वृक्षः_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    जइ (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } अण्णहा {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        जइ(defLoc.length() > full.length()) {
                          ध्रुव char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#जइ defined(URES_वृक्षः_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_कुञ्चिका(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          जइ(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#जइ defined(URES_वृक्षः_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#जइ defined(URES_वृक्षः_DEBUG)
                        अण्णहा {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } अण्णहा {
#जइ defined(URES_वृक्षः_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = मिच्छ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        जइ (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            ध्रुव वर्ण *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            जइ (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                जइ (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = सच्च;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        जइ (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } जाव(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    जइ(full.isEmpty() && kwVal != defVal) {
#जइ defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        करेहि { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            जइ((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = मिच्छ;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#जइ defined(URES_वृक्षः_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            जइ(U_FAILURE(subStatus)) {
                *status = subStatus;
            } अण्णहा जइ(subStatus == U_ZERO_ERROR) {
                ures_get_By_कुञ्चिका(res,resName,&bund1, &subStatus);
                जइ(subStatus == U_ZERO_ERROR) {
                    ures_get_By_कुञ्चिका(&bund1, kwVal.data(), &bund2, &subStatus);
                    जइ(subStatus == U_ZERO_ERROR) {
#जइ defined(URES_वृक्षः_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        जइ (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } अण्णहा {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        जइ(defLoc.length() > full.length()) {
                          ध्रुव char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#जइ defined(URES_वृक्षः_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_कुञ्चिका(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          जइ(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#जइ defined(URES_वृक्षः_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#जइ defined(URES_वृक्षः_DEBUG)
                        अण्णहा {
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
        } जाव(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    जइ(U_SUCCESS(*status)) {
        जइ(full.isEmpty()) {
#जइ defined(URES_वृक्षः_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } अण्णहा जइ(omitDefault) {
#जइ defined(URES_वृक्षः_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          जइ(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            जइ(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#जइ defined(URES_वृक्षः_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        जइ(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } अण्णहा जइ(!omitDefault) {
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

    जइ(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        जइ(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        जइ(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } अण्णहा {
        length = 0;
        result[0]=0;
    }
    प्रत्यागम u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_मानानि(ध्रुव वर्ण *path, ध्रुव वर्ण *keyword, UErrorCode *status)
{
#define मानानि_BUF_SIZE 2048
#define मानानि_LIST_SIZE 512
    
    वर्ण       मानानि_Buf[मानानि_BUF_SIZE];
    int32_t    मानानि_Index = 0;
    ध्रुव वर्ण *मानानि_List[मानानि_LIST_SIZE];
    int32_t    मानानि_गणनम् = 0;
    
    ध्रुव वर्ण *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_राशिः_Object(&item);
    ures_init_राशिः_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    जइ(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        प्रत्यागम nullptr;
    }
    
    मानानि_Buf[0]=0;
    मानानि_Buf[1]=0;

    जाव ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#जइ defined(URES_वृक्षः_DEBUG)
        जइ(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_कुञ्चिका(bund, keyword, &item, &subStatus);
        
        जइ(!bund || U_FAILURE(subStatus)) {
#जइ defined(URES_वृक्षः_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            अनुवर्त;
        }

        जाव ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            ध्रुव वर्ण *k;
            int32_t i;
            k = ures_get_कुञ्चिका(subPtr);

#जइ defined(URES_वृक्षः_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            जइ(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                अनुवर्त;
            }
            प्रति(i=0; i<मानानि_गणनम्; i++) {
                जइ(!uprv_strcmp(मानानि_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    भङ्ग;
                }
            }
            जइ(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                जइ((मानानि_गणनम् >= (मानानि_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((मानानि_Index+kLen+1+1) >= मानानि_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } अण्णहा {
                    uprv_strcpy(मानानि_Buf+मानानि_Index, k);
                    मानानि_List[मानानि_गणनम्++] = मानानि_Buf+मानानि_Index;
                    मानानि_Index += kLen;
#जइ defined(URES_वृक्षः_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    मानानि_Buf[मानानि_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    मानानि_Buf[मानानि_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#जइ defined(URES_वृक्षः_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        मानानि_Index, मानानि_गणनम्);
#endif
    प्रत्यागम uloc_openKeywordList(मानानि_Buf, मानानि_Index, status);
}
#जइ 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ध्रुव UResourceBundle* res1, ध्रुव UResourceBundle* res2){
    जइ(res1==nullptr || res2==nullptr){
        प्रत्यागम res1==res2; /* pointer comparison */
    }
    जइ(res1->f_कुञ्चिका==nullptr||  res2->f_कुञ्चिका==nullptr){
        प्रत्यागम (res1->f_कुञ्चिका==res2->f_कुञ्चिका);
    }अण्णहा{
        जइ(uprv_strcmp(res1->f_कुञ्चिका, res2->f_कुञ्चिका)!=0){
            प्रत्यागम मिच्छ;
        }
    }
    जइ(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        प्रत्यागम मिच्छ;
    }
    जइ(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        प्रत्यागम (res1->fData->fPath == res2->fData->fPath);
    }अण्णहा{
        जइ(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            प्रत्यागम मिच्छ;
        }
    }
    जइ(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        प्रत्यागम मिच्छ;
    }
    जइ(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        प्रत्यागम मिच्छ;
    }
    जइ(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        प्रत्यागम मिच्छ;
    }
    जइ(res1->fRes != res2->fRes){
        प्रत्यागम मिच्छ;
    }
    प्रत्यागम सच्च;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ध्रुव UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    जइ(U_FAILURE(*status) || res == nullptr){
        प्रत्यागम nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    जइ(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }अण्णहा{
        ret = bundle;
    }
    प्रत्यागम ret;
}
U_CAPI ध्रुव UResourceBundle* U_EXPORT2
ures_getParentBundle(ध्रुव UResourceBundle* res){
    जइ(res==nullptr){
        प्रत्यागम nullptr;
    }
    प्रत्यागम res->fParentRes;
}
#endif

U_CAPI शून्य U_EXPORT2
ures_get_Version_By_कुञ्चिका(ध्रुव UResourceBundle* res, ध्रुव वर्ण *कुञ्चिका, UVersionInfo ver, UErrorCode *status) {
  ध्रुव char16_t *str;
  int32_t len;
  str = ures_get_String_By_कुञ्चिका(res, कुञ्चिका, &len, status);
  जइ(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
