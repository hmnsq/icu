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
स्थिर int32_t U_CALLCONV hashEntry(अचर UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    फर्काऊ uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
स्थिर UBool U_CALLCONV compareEntries(अचर UHashTok p1, अचर UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    फर्काऊ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
स्थिर UBool chopLocale(वर्ण *name) {
    वर्ण *i = uprv_strrchr(name, '_');

    यदि(i != nullptr) {
        *i = '\0';
        फर्काऊ सत्य;
    }

    फर्काऊ असत्य;
}

स्थिर UBool hasVariant(अचर वर्ण* localeID) {
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
    फर्काऊ sink.सङ्ख्या_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#अन्तर्भाव "localefallback_data.h"

स्थिर अचर वर्ण* performFallbackLookup(अचर वर्ण* साँचो,
                                         अचर वर्ण* साँचो_Strs,
                                         अचर वर्ण* मान_Strs U_LIFETIME_BOUND,
                                         अचर int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    अचर int32_t* bottom = lookupTable;
    अचर int32_t* शीर्ष = lookupTable + lookupTableLength;

    जबसम्म (bottom < शीर्ष) {
        // Effectively, divide by 2 and round down to an even index
        अचर int32_t* बीच = bottom + (((शीर्ष - bottom) / 4) * 2);
        अचर वर्ण* entry_साँचो = &(साँचो_Strs[*बीच]);
        int32_t strcmpResult = uprv_strcmp(साँचो, entry_साँचो);
        यदि (strcmpResult == 0) {
            फर्काऊ &(मान_Strs[बीच[1]]);
        } अन्यथा यदि (strcmpResult < 0) {
            शीर्ष = बीच;
        } अन्यथा {
            bottom = बीच + 2;
        }
    }
    फर्काऊ nullptr;
}

स्थिर CharString getDefaultScript(अचर CharString& language, अचर CharString& region) {
    अचर वर्ण* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    यदि (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        यदि (U_FAILURE(err)) {
            फर्काऊ result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    यदि (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    यदि (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    फर्काऊ result;
}

गणना UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_जरा,
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
    URES_OPEN_LOCALE_जरा,
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
स्थिर तार्किक getParentLocaleID(वर्ण *name, अचर वर्ण *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    माप_प्रकार nameLen = uprv_strlen(name);
    यदि (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        फर्काऊ chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    यदि (U_FAILURE(err)) {
        // hopefully this never happens...
        फर्काऊ chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    यदि (openType == URES_OPEN_LOCALE_DEFAULT_जरा) {
        अचर वर्ण* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        यदि (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            फर्काऊ सत्य;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    यदि (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        यदि (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } अन्यथा {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } अन्यथा यदि (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        यदि (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } अन्यथा {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } अन्यथा यदि (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        यदि (openType != URES_OPEN_LOCALE_DEFAULT_जरा || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } अन्यथा {
            फर्काऊ असत्य;
        }
    } अन्यथा {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        फर्काऊ असत्य;
    }
    यदि (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        फर्काऊ सत्य;
    } अन्यथा {
        फर्काऊ असत्य;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
स्थिर UBool mayHaveParent(वर्ण *name) {
    फर्काऊ (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
स्थिर शून्य entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_गन्ती_Existing++;
    जबसम्म(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_गन्ती_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
स्थिर UResourceDataEntry *getFallbackData(
        अचर UResourceBundle *resBundle,
        अचर वर्ण **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    यदि(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        फर्काऊ nullptr;
    }
    यदि(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_साँचो(&(dataEntry->fData), dataEntry->fData.जरा_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    यदि(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        जबसम्म(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            यदि(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_साँचो(&(dataEntry->fData), dataEntry->fData.जरा_Res, &indexR, resTag);
            }
        }
    }

    यदि(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        फर्काऊ nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    यदि(i>1) {
        यदि(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_जरा_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } अन्यथा {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    फर्काऊ dataEntry;
}

स्थिर शून्य
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    यदि(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    यदि(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    यदि(entry->fPool != nullptr) {
        --entry->fPool->f_गन्ती_Existing;
    }
    alias = entry->fAlias;
    यदि(alias != nullptr) {
        जबसम्म(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_गन्ती_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
स्थिर int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_अङ्क = 0;
    अचर UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    यदि (cache == nullptr) {
        फर्काऊ 0;
    }

    गर {
        deletedMore = असत्य;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        जबसम्म ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            यदि (resB->f_गन्ती_Existing == 0) {
                rb_Deleted_अङ्क++;
                deletedMore = सत्य;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } जबसम्म(deletedMore);

    फर्काऊ rb_Deleted_अङ्क;
}

#ifdef URES_DEBUG
#अन्तर्भाव <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = असत्य;
  int32_t pos = UHASH_FIRST;
  अचर UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    यदि (cache == nullptr) {
      ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      फर्काऊ असत्य;
    }

    जबसम्म ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=सत्य;
      resB = (UResourceDataEntry *) e->value.pointer;
      ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (शून्य*)resB, resB->f_गन्ती_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (शून्य*)resB->fPool,
              (शून्य*)resB->fAlias,
              (शून्य*)resB->fParent);       
    }
    
    ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_गन्ती(cache));
    फर्काऊ cacheNotEmpty;
}

#endif

स्थिर UBool U_CALLCONV ures_cleanup()
{
    यदि (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    फर्काऊ सत्य;
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

स्थिर शून्य setEntryName(UResourceDataEntry *res, अचर वर्ण *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    यदि(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    यदि (len < static_cast<int32_t>(परिमाण(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    अन्यथा {
        res->fName = static_cast<वर्ण*>(uprv_malloc(len + 1));
    }
    यदि(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        uprv_strcpy(res->fName, name);
    }
}

स्थिर UResourceDataEntry *
getPoolEntry(अचर वर्ण *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
स्थिर UResourceDataEntry *init_entry(अचर वर्ण *localeID, अचर वर्ण *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    अचर वर्ण *name;
    वर्ण aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    यदि(U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }

    /* here we try to deduce the right locale name */
    यदि(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } अन्यथा यदि(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_जरा_Locale_Name;
    } अन्यथा { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<वर्ण*>(name);
    find.fPath = const_cast<वर्ण*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    यदि(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(परिमाण(UResourceDataEntry)));
        यदि(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ nullptr;
        }

        uprv_memset(r, 0, परिमाण(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        यदि (U_FAILURE(*status)) {
            uprv_free(r);
            फर्काऊ nullptr;
        }

        यदि(path != nullptr) {
            r->fPath = uprv_strdup(path);
            यदि(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                फर्काऊ nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        यदि (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            यदि (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                फर्काऊ nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } अन्यथा { /* if we have a regular entry */
            Resource aliasres;
            यदि (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                यदि (U_SUCCESS(*status)) {
                    अचर int32_t *poolIndexes = r->fPool->fData.p_जरा + 1;
                    यदि(r->fData.p_जरा[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_साँचोहरू = reinterpret_cast<अचर वर्ण*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } अन्यथा {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } अन्यथा {
                    r->fBogus = *status;
                }
            }
            यदि (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                यदि (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    अचर char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    यदि(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            यदि ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (शून्य *)r, r, &cacheStatus);
                यदि (U_FAILURE(cacheStatus)) {
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
    यदि(r != nullptr) {
        /* return the real bundle */
        जबसम्म(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_गन्ती_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        यदि(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    फर्काऊ r;
}

स्थिर UResourceDataEntry *
getPoolEntry(अचर वर्ण *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    यदि( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    फर्काऊ poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
स्थिर UResourceDataEntry *
findFirstExisting(अचर वर्ण* path, वर्ण* name, अचर वर्ण* defaultLocale, UResOpenType openType,
                  UBool *is_जरा, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = असत्य;
    *foundParent = सत्य; /* we're starting with a fresh name */
    वर्ण origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    जबसम्म(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        यदि (U_FAILURE(*status)) {
            फर्काऊ nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        यदि(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_गन्ती_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } अन्यथा {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_जरा = static_cast<UBool>(uprv_strcmp(name, k_जरा_Locale_Name) == 0);

        /*Fallback data stuff*/
        यदि (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } अन्यथा {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        यदि (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    फर्काऊ r;
}

स्थिर शून्य ures_set_Is_थाक_Object( UResourceBundle* resB, UBool state) {
    यदि(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } अन्यथा {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

स्थिर UBool ures_is_थाक_Object(अचर UResourceBundle* resB) {
  फर्काऊ((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?असत्य:सत्य);
}


U_CFUNC शून्य ures_init_थाक_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, परिमाण(UResourceBundle));
  ures_set_Is_थाक_Object(resB, सत्य);
}

U_NAMESPACE_BEGIN

थाक_UResource_Bundle::थाक_UResource_Bundle() {
    ures_init_थाक_Object(&bundle);
}

थाक_UResource_Bundle::~थाक_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

स्थिर UBool  // returns U_SUCCESS(*status)
load_Parents_Except_जरा(UResourceDataEntry *&t1,
                      वर्ण name[], int32_t nameCapacity,
                      UBool usingUSRData, वर्ण usrDataPath[], UErrorCode *status) {
    यदि (U_FAILURE(*status)) { फर्काऊ असत्य; }
    UBool checkParent = सत्य;
    जबसम्म (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        यदि (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            अचर char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            यदि(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                यदि (uprv_strcmp(name, k_जरा_Locale_Name) == 0) {
                    फर्काऊ सत्य;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        यदि (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            फर्काऊ असत्य;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        यदि (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            यदि (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                फर्काऊ असत्य;
            }
        }

        यदि (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } अन्यथा {
            t1->fParent = t2;
            यदि (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_गन्ती_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    फर्काऊ सत्य;
}

स्थिर UBool  // returns U_SUCCESS(*status)
घुसाउ_जरा_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    यदि (U_FAILURE(*status)) { फर्काऊ असत्य; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_जरा_Locale_Name, t1->fPath, &parentStatus);
    यदि (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        फर्काऊ असत्य;
    }
    t1->fParent = t2;
    t1 = t2;
    फर्काऊ सत्य;
}

स्थिर UResourceDataEntry *entryOpen(अचर वर्ण* path, अचर वर्ण* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = असत्य;
    UBool is_जरा = असत्य;
    UBool hasRealData = असत्य;
    UBool hasChopped = सत्य;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    वर्ण name[ULOC_FULLNAME_CAPACITY];
    वर्ण usrDataPath[96];

    initCache(status);

    यदि(U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }

    uprv_strncpy(name, localeID, परिमाण(name) - 1);
    name[परिमाण(name) - 1] = 0;

    यदि ( usingUSRData ) {
        यदि ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } अन्यथा {
            uprv_strncpy(usrDataPath, path, परिमाण(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[परिमाण(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    अचर वर्ण *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_जरा, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    यदि (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        गच्छ finish;
    }

    यदि(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = सत्य;
        यदि ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            यदि (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                गच्छ finish;
            }
            यदि ( u1 != nullptr ) {
                यदि(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } अन्यथा {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_गन्ती_Existing = 0;
                }
            }
        }
        यदि ((hasChopped || mayHaveParent(name)) && !is_जरा) {
            यदि (!load_Parents_Except_जरा(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                गच्छ finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    यदि(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_जरा && !isDefault && !is_जरा) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_जरा, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        यदि (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            गच्छ finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        यदि(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = सत्य;
            isDefault = सत्य;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            यदि ((hasChopped || mayHaveParent(name)) && !is_जरा) {
                यदि (!load_Parents_Except_जरा(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    गच्छ finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    यदि(r == nullptr) {
        uprv_strcpy(name, k_जरा_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_जरा, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        यदि (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            गच्छ finish;
        }
        यदि(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = सत्य;
        } अन्यथा { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            गच्छ finish;
        }
    } अन्यथा यदि(!is_जरा && uprv_strcmp(t1->fName, k_जरा_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        यदि (!घुसाउ_जरा_Bundle(t1, status)) {
            गच्छ finish;
        }
        यदि(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    जबसम्म(r != nullptr && !is_जरा && t1->fParent != nullptr) {
        t1->fParent->f_गन्ती_Existing++;
        t1 = t1->fParent;
    }

finish:
    यदि(U_SUCCESS(*status)) {
        यदि(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        फर्काऊ r;
    } अन्यथा {
        फर्काऊ nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
स्थिर UResourceDataEntry *
entryOpenDirect(अचर वर्ण* path, अचर वर्ण* localeID, UErrorCode* status) {
    initCache(status);
    यदि(U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    यदि (localeID == nullptr) {
        localeID = uloc_getDefault();
    } अन्यथा यदि (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_जरा_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    यदि(U_SUCCESS(*status)) {
        यदि(r->fBogus != U_ZERO_ERROR) {
            r->f_गन्ती_Existing--;
            r = nullptr;
        }
    } अन्यथा {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    यदि(r != nullptr && uprv_strcmp(localeID, k_जरा_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        वर्ण name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        यदि(!chopLocale(name) || uprv_strcmp(name, k_जरा_Locale_Name) == 0 ||
                load_Parents_Except_जरा(t1, name, UPRV_LENGTHOF(name), असत्य, nullptr, status)) {
            यदि(uprv_strcmp(t1->fName, k_जरा_Locale_Name) != 0 && t1->fParent == nullptr) {
                घुसाउ_जरा_Bundle(t1, status);
            }
        }
        यदि(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    यदि(r != nullptr) {
        // TODO: Does this ever loop?
        जबसम्म(t1->fParent != nullptr) {
            t1->fParent->f_गन्ती_Existing++;
            t1 = t1->fParent;
        }
    }
    फर्काऊ r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
स्थिर शून्य entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    जबसम्म(resB != nullptr) {
        p = resB->fParent;
        resB->f_गन्ती_Existing--;

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
स्थिर शून्य ures_appendResPath(UResourceBundle *resB, अचर वर्ण* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    यदि(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    यदि(RES_BUFSIZE <= resB->fResPathLen+1) {
        यदि(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<वर्ण*>(uprv_malloc((resB->fResPathLen + 1) * परिमाण(वर्ण)));
            /* Check that memory was allocated correctly. */
            यदि (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } अन्यथा {
            वर्ण* temp = static_cast<वर्ण*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * परिमाण(वर्ण)));
            /* Check that memory was reallocated correctly. */
            यदि (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

स्थिर शून्य ures_freeResPath(UResourceBundle *resB) {
    यदि (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

स्थिर शून्य
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    यदि(resB != nullptr) {
        यदि(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        यदि(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        यदि(ures_is_थाक_Object(resB) == असत्य && freeBundleObj) {
            uprv_free(resB);
        }
#यदि 0 /*U_DEBUG*/
        अन्यथा {
            /* poison the data */
            uprv_memset(resB, -1, परिमाण(UResourceBundle));
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
                                  अचर वर्ण* साँचो,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  अचर वर्ण* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(अचर ResourceData& resData,
                               Resource r,
                               अचर वर्ण* साँचो,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               अचर वर्ण* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    यदि (U_FAILURE(*status)) { फर्काऊ resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    अचर char16_t *alias = res_getAlias(&resData, r, &len);
    यदि(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ resB;
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
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }

    // We have an alias, now let's cut it up.
    अचर वर्ण *path = nullptr, *locale = nullptr, *साँचो_Path = nullptr;
    यदि(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        वर्ण *chAliasData = chAlias.data();
        वर्ण *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        यदि(sep != nullptr) {
            *sep++ = 0;
        }
        यदि(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            साँचो_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } अन्यथा {
            यदि(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            यदि (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } अन्यथा {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                यदि(sep != nullptr) {
                    *sep++ = 0;
                }
                साँचो_Path = sep;
            }
        }
    } अन्यथा {
        // No path, start with a locale.
        वर्ण *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        यदि(sep != nullptr) {
            *sep++ = 0;
        }
        साँचो_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    यदि (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } अन्यथा {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        यदि(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            फर्काऊ resB;
        }
        dataEntry = mainRes->fData;
    }

    अचर वर्ण* temp = nullptr;
    यदि(साँचो_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.जरा_Res;
        यदि(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            यदि (U_FAILURE(*status)) {
                फर्काऊ nullptr;
            }
            वर्ण *a_साँचो = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_साँचो, &temp);
        }
        यदि(साँचो) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(साँचो, *status);
            यदि (U_FAILURE(*status)) {
                फर्काऊ nullptr;
            }
            वर्ण *a_साँचो = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_साँचो, &temp);
        } अन्यथा यदि(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            यदि(URES_IS_TABLE(type)) {
                अचर वर्ण *a_साँचो;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_साँचो);
            } अन्यथा { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        यदि(r != RES_BOGUS) {
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
        CharString pathBuf(साँचो_Path, *status);
        यदि (U_FAILURE(*status)) {
            फर्काऊ nullptr;
        }
        वर्ण *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        लागि(;;) {
            r = dataEntry->fData.जरा_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            जबसम्म(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                यदि(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    रोक;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                यदि (U_FAILURE(*status)) {
                    रोक;
                }
                यदि (temp == nullptr || uprv_strcmp(साँचो_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, साँचो_Path, static_cast<int32_t>(uprv_strlen(साँचो_Path)), status);
                    यदि(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    यदि (U_FAILURE(*status)) {
                        रोक;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            यदि (U_FAILURE(*status) || r != RES_BOGUS) {
                रोक;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            यदि (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                रोक;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, साँचो_Path);
        }
    }
    यदि(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    फर्काऊ resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  अचर वर्ण* साँचो,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  अचर वर्ण* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    यदि(status == nullptr || U_FAILURE(*status)) {
        फर्काऊ resB;
    }
    यदि (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    यदि(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        यदि(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            फर्काऊ resB;
        }
        फर्काऊ getAliasTargetAsResourceBundle(
            dataEntry->fData, r, साँचो, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    यदि(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
        यदि (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ nullptr;
        }
        ures_set_Is_थाक_Object(resB, असत्य);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } अन्यथा {
        यदि(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        यदि(resB->fVersion != nullptr) {
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
        यदि(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = असत्य;
    resB->f_Is_शीर्ष_Level = असत्य;
    resB->fIndex = -1;
    resB->f_साँचो = साँचो; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    यदि(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    यदि(साँचो != nullptr) {
        ures_appendResPath(resB, साँचो, static_cast<int32_t>(uprv_strlen(साँचो)), status);
        यदि(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } अन्यथा यदि(idx >= 0) {
        वर्ण buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        यदि(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
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
    resB->fSize = res_गन्ती_Array_वस्तुहरू(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    फर्काऊ resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  अचर वर्ण* साँचो,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  अचर UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    फर्काऊ init_resb_result(
        dataEntry, r, साँचो, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               अचर UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_थाक_Object;
    यदि(U_FAILURE(*status) || r == original) {
        फर्काऊ r;
    }
    यदि(original != nullptr) {
        यदि(r == nullptr) {
            is_थाक_Object = असत्य;
            r = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
            /* test for nullptr */
            यदि (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ nullptr;
            }
        } अन्यथा {
            is_थाक_Object = ures_is_थाक_Object(r);
            ures_closeBundle(r, असत्य);
        }
        uprv_memcpy(r, original, परिमाण(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        यदि(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_थाक_Object(r, is_थाक_Object);
        यदि(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    फर्काऊ r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI अचर char16_t* U_EXPORT2 ures_getString(अचर UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    अचर char16_t *s;
    यदि (status==nullptr || U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    यदि (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    फर्काऊ s;
}

स्थिर अचर वर्ण*
ures_toUTF8String(अचर char16_t* s16,
                  int32_t length16,
                  वर्ण* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि (pLength != nullptr) {
        capacity = *pLength;
    } अन्यथा {
        capacity = 0;
    }
    यदि (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }

    यदि (length16 == 0) {
        /* empty string, return as read-only pointer */
        यदि (pLength != nullptr) {
            *pLength = 0;
        }
        यदि (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            फर्काऊ dest;
        } अन्यथा {
            फर्काऊ "";
        }
    } अन्यथा {
        /* We need to transform the string to the destination buffer. */
        यदि (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            फर्काऊ u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        यदि (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            यदि (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        फर्काऊ u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI अचर वर्ण* U_EXPORT2
ures_getUTF8String(अचर UResourceBundle* resB,
                   वर्ण* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    अचर char16_t *s16 = ures_getString(resB, &length16, status);
    फर्काऊ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI अचर uint8_t* U_EXPORT2
ures_get_द्विआधारी(अचर UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  अचर uint8_t *p;
  यदि (status==nullptr || U_FAILURE(*status)) {
    फर्काऊ nullptr;
  }
  यदि(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ nullptr;
  }
  p = res_get_द्विआधारी({resB}, &resB->getResData(), resB->fRes, len);
  यदि (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  फर्काऊ p;
}

U_CAPI अचर int32_t* U_EXPORT2
ures_getIntVector(अचर UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  अचर int32_t *p;
  यदि (status==nullptr || U_FAILURE(*status)) {
    फर्काऊ nullptr;
  }
  यदि(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  यदि (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  फर्काऊ p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(अचर UResourceBundle* resB, UErrorCode *status) {
  यदि (status==nullptr || U_FAILURE(*status)) {
    फर्काऊ 0xffffffff;
  }
  यदि(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0xffffffff;
  }
  यदि(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    फर्काऊ 0xffffffff;
  }
  फर्काऊ res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(अचर UResourceBundle* resB, UErrorCode *status) {
  यदि (status==nullptr || U_FAILURE(*status)) {
    फर्काऊ 0xffffffff;
  }
  यदि(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0xffffffff;
  }
  यदि(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    फर्काऊ 0xffffffff;
  }
  फर्काऊ res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(अचर UResourceBundle *resB) {
  यदि(resB == nullptr) {
    फर्काऊ URES_NONE;
  }
  फर्काऊ res_getPublicType(resB->fRes);
}

U_CAPI अचर वर्ण* U_EXPORT2
ures_get_साँचो(अचर UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  यदि(resB == nullptr) {
    फर्काऊ nullptr;
  }
  फर्काऊ(resB->f_साँचो);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(अचर UResourceBundle *resB) {
  यदि(resB == nullptr) {
    फर्काऊ 0;
  }
  
  फर्काऊ resB->fSize;
}

स्थिर अचर char16_t*
ures_getStringWithAlias(अचर UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  यदि(RES_GET_TYPE(r) == URES_ALIAS) {
    अचर char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    फर्काऊ result;
  } अन्यथा {
    फर्काऊ res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI शून्य U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  यदि(resB == nullptr) {
    फर्काऊ;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(अचर UResourceBundle *resB) {
  यदि(resB == nullptr) {
    फर्काऊ असत्य;
  }
  फर्काऊ resB->fIndex < resB->fSize-1;
}

U_CAPI अचर char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   अचर वर्ण** साँचो,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  यदि (status==nullptr || U_FAILURE(*status)) {
    फर्काऊ nullptr;
  }
  यदि(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ nullptr;
  }
  
  यदि(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } अन्यथा {
    resB->fIndex++;
    विकल्प(RES_GET_TYPE(resB->fRes)) {
    पक्ष URES_STRING:
    पक्ष URES_STRING_V2:
      फर्काऊ res_getString({resB}, &resB->getResData(), resB->fRes, len);
    पक्ष URES_TABLE:
    पक्ष URES_TABLE16:
    पक्ष URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, साँचो);
      यदि(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      फर्काऊ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    पक्ष URES_ARRAY:
    पक्ष URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      यदि(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      फर्काऊ ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    पक्ष URES_ALIAS:
      फर्काऊ ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    पक्ष URES_INT:
    पक्ष URES_द्विआधारी:
    पक्ष URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    स्वभाव:
      फर्काऊ nullptr;
    }
  }

  फर्काऊ nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    अचर वर्ण *साँचो = nullptr;
    Resource r = RES_BOGUS;

    यदि (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            फर्काऊ fillIn;
    }
    यदि(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            फर्काऊ fillIn;
    }

    यदि(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } अन्यथा {
        resB->fIndex++;
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_INT:
        पक्ष URES_द्विआधारी:
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
        पक्ष URES_INT_VECTOR:
            फर्काऊ ures_copyResb(fillIn, resB, status);
        पक्ष URES_TABLE:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &साँचो);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            फर्काऊ init_resb_result(resB->fData, r, साँचो, resB->fIndex, resB, fillIn, status);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            फर्काऊ init_resb_result(resB->fData, r, साँचो, resB->fIndex, resB, fillIn, status);
        स्वभाव:
            /*return nullptr;*/
            फर्काऊ fillIn;
        }
    }
    /*return nullptr;*/
    फर्काऊ fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(अचर UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    अचर वर्ण* साँचो = nullptr;
    Resource r = RES_BOGUS;

    यदि (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        फर्काऊ fillIn;
    }
    यदि(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        फर्काऊ fillIn;
    }

    यदि(indexR >= 0 && resB->fSize > indexR) {
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_INT:
        पक्ष URES_द्विआधारी:
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
        पक्ष URES_INT_VECTOR:
            फर्काऊ ures_copyResb(fillIn, resB, status);
        पक्ष URES_TABLE:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &साँचो);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            फर्काऊ init_resb_result(resB->fData, r, साँचो, indexR, resB, fillIn, status);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            फर्काऊ init_resb_result(resB->fData, r, साँचो, indexR, resB, fillIn, status);
        स्वभाव:
            /*return nullptr;*/
            फर्काऊ fillIn;
        }
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    फर्काऊ fillIn;
}

U_CAPI अचर char16_t* U_EXPORT2
ures_getStringByIndex(अचर UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    अचर वर्ण* साँचो = nullptr;
    Resource r = RES_BOGUS;

    यदि (status==nullptr || U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }

    यदि(indexS >= 0 && resB->fSize > indexS) {
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
            फर्काऊ res_getString({resB}, &resB->getResData(), resB->fRes, len);
        पक्ष URES_TABLE:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &साँचो);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            फर्काऊ ures_getStringWithAlias(resB, r, indexS, len, status);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            फर्काऊ ures_getStringWithAlias(resB, r, indexS, len, status);
        पक्ष URES_ALIAS:
            फर्काऊ ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        पक्ष URES_INT:
        पक्ष URES_द्विआधारी:
        पक्ष URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            रोक;
        स्वभाव:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          रोक;
        }
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    फर्काऊ nullptr;
}

U_CAPI अचर वर्ण* U_EXPORT2
ures_getUTF8StringByIndex(अचर UResourceBundle* resB,
                          int32_t idx,
                          वर्ण* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    अचर char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    फर्काऊ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(अचर वर्ण* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  वर्ण *packageName = nullptr;
  वर्ण *pathToResource = nullptr, *save = nullptr;
  वर्ण *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  यदि(status == nullptr || U_FAILURE(*status)) {
    फर्काऊ result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (वर्ण *)uprv_malloc(length*परिमाण(वर्ण));
  /* test for nullptr */
  यदि(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    फर्काऊ result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  यदि(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    यदि(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  यदि(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  यदि(U_SUCCESS(*status)) {
    यदि(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } अन्यथा {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  फर्काऊ result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(अचर UResourceBundle* resB,
                     वर्ण* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  अचर वर्ण *साँचो;

  यदि(status == nullptr || U_FAILURE(*status)) {
    फर्काऊ result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  गर {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &साँचो); 
    यदि(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, साँचो, -1, resB, fillIn, status);
        resB = result;
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
        रोक;
    }
  } जबसम्म(*path); /* there is more stuff in the path */

  फर्काऊ result;
}

U_CAPI अचर char16_t* U_EXPORT2
ures_get_String_By_साँचो_With_Fallback(अचर UResourceBundle* resB U_LIFETIME_BOUND,
                                अचर वर्ण* in_साँचो,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle थाक_2;
    अचर char16_t* retVal = nullptr;
    ures_init_थाक_Object(&थाक_2);
    ures_get_By_साँचो_With_Fallback(resB, in_साँचो, &थाक_2, status);
    int32_t length;
    retVal = ures_getString(&थाक_2, &length, status);
    ures_close(&थाक_2);
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    यदि (len != nullptr) {
        *len = length;
    }
    फर्काऊ retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
स्थिर Resource get_Table_Item_By_साँचो_Path(अचर ResourceData *pResData, Resource table, अचर वर्ण *साँचो) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(साँचो, errorCode);
  यदि (U_FAILURE(errorCode)) { फर्काऊ RES_BOGUS; }
  वर्ण *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  जबसम्म (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    वर्ण *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    यदि (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } अन्यथा {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    अचर वर्ण *pathP = pathPart;
    resource = res_get_Table_Item_By_साँचो(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  यदि (*pathPart) {
    फर्काऊ RES_BOGUS;
  }
  फर्काऊ resource;
}

स्थिर शून्य createPath(अचर वर्ण* origResPath,
                       int32_t     origResPathLen,
                       अचर वर्ण* resPath,
                       int32_t     resPathLen,
                       अचर वर्ण* in_साँचो,
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
    अचर वर्ण* साँचो = in_साँचो;
    यदि (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        यदि (U_SUCCESS(*status)) {
            अचर वर्ण* resPathLimit = resPath + resPathLen;
            अचर वर्ण* origResPathLimit = origResPath + origResPathLen;
            अचर वर्ण* resPathPtr = resPath;
            अचर वर्ण* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            जबसम्म (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                जबसम्म (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                यदि (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                जबसम्म (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                यदि (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            जबसम्म (resPathPtr < resPathLimit && *साँचो != '\0') {
                जबसम्म (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                यदि (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                जबसम्म (*साँचो != '\0' && *साँचो != RES_PATH_SEPARATOR) {
                    ++साँचो;
                }
                यदि (*साँचो == RES_PATH_SEPARATOR) {
                    ++साँचो;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(साँचो, *status);
    } अन्यथा {
        path.append(in_साँचो, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_साँचो_With_Fallback(अचर UResourceBundle* resB,
                          अचर वर्ण* in_साँचो,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, जरा_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    यदि (status==nullptr || U_FAILURE(*status)) {
        फर्काऊ fillIn;
    }
    यदि(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    यदि(URES_IS_TABLE(type)) {
        अचर वर्ण* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_साँचो_Path(&resB->getResData(), resB->fRes, in_साँचो);
        अचर वर्ण* साँचो = in_साँचो;
        तार्किक did_जरा_Once = असत्य;
        यदि(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            वर्ण *myPath = nullptr;
            अचर वर्ण* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            जबसम्म(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_जरा_Once)) { /* Otherwise, we'll look in parents */
                यदि (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } अन्यथा {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_जरा_Once = सत्य;
                }
                जरा_Res = dataEntry->fData.जरा_Res;

                यदि(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_साँचो, path, status);
                    यदि (U_FAILURE(*status)) {
                        ures_close(helper);
                        फर्काऊ fillIn;
                    }
                    myPath = path.data();
                    साँचो = in_साँचो;
                    गर {
                        res = res_findResource(&(dataEntry->fData), जरा_Res, &myPath, &साँचो);
                        यदि (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            यदि(helper) {
                              dataEntry = helper->fData;
                              जरा_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } अन्यथा {
                              रोक;
                            }
                        } अन्यथा यदि (res == RES_BOGUS) {
                            रोक;
                        }
                    } जबसम्म(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            यदि(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                यदि(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_जरा_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } अन्यथा {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, साँचो, -1, resB, fillIn, status);
                यदि (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_साँचो, path, status);
                } अन्यथा {
                    अचर वर्ण* separator = nullptr;
                    यदि (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    यदि (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_साँचो, path, status);
                    } अन्यथा {
                        createPath(origResPath, origResPathLen, "", 0, in_साँचो, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                यदि(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            fillIn = init_resb_result(resB->fData, res, साँचो, -1, resB, fillIn, status);
        }
    } 
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    फर्काऊ fillIn;
}

नामाकाश {

शून्य get_All_वस्तुहरू_With_Fallback(
        अचर UResourceBundle *bundle, Resource_Data_मान &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
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
    sink.put(bundle->f_साँचो, value, !hasParent, errorCode);
    यदि (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        थाक_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_शीर्ष_Level = सत्य;
        parentRef.fRes = parentRef.getResData().जरा_Res;
        parentRef.fSize = res_गन्ती_Array_वस्तुहरू(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        थाक_UResource_Bundle containerBundle;
        अचर UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        यदि (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } अन्यथा {
            rb = ures_get_By_साँचो_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        यदि (U_SUCCESS(pathErrorCode)) {
            get_All_वस्तुहरू_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

संरचना GetAllChildrenSink : सार्वजनिक ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    आभास ~GetAllChildrenSink() override;
    आभास शून्य put(अचर वर्ण *साँचो, Resource_मान &value, UBool is_जरा,
           UErrorCode &errorCode) override {
        ResourceTable वस्तुहरू_Table = value.getTable(errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        लागि (int32_t i = 0; वस्तुहरू_Table.get_साँचो_And_मान(i, साँचो, value); ++i) {
            यदि (value.getType() == URES_ALIAS) {
                Resource_Data_मान& rdv = static_cast<Resource_Data_मान&>(value);
                थाक_UResource_Bundle थाक_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          थाक_Temp_Bundle.getAlias(), &errorCode);
                यदि (U_SUCCESS(errorCode)) {
                    Resource_Data_मान aliased_मान;
                    aliased_मान.setData(aliasRB->getResData());
                    aliased_मान.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_मान.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    यदि (aliased_मान.getType() != URES_TABLE) {
                        dest.put(साँचो, aliased_मान, is_जरा, errorCode);
                    } अन्यथा {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_मान_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        अचर वर्ण* parent_साँचो = साँचो; // dest.put() changes the key
                        dest.put(parent_साँचो, aliased_मान, is_जरा, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        जबसम्म (aliased_मान_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            वर्ण* localPathAsCharPtr = localPath.data();
                            अचर वर्ण* child_साँचो;
                            entry = entry->fParent;
                            res = entry->fData.जरा_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_साँचो);
                            यदि (newRes != RES_BOGUS) {
                                aliased_मान.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_मान.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_मान_Type = aliased_मान.getType();
                                यदि (aliased_मान_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_मान& rdv2 = static_cast<Resource_Data_मान&>(aliased_मान);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             थाक_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_मान.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_मान.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_मान_Type = aliased_मान.getType();
                                }
                                यदि (aliased_मान_Type == URES_TABLE) {
                                    dest.put(parent_साँचो, aliased_मान, is_जरा, errorCode);
                                } अन्यथा {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    फर्काऊ;
                                }
                            }
                        }
                    }
                }
            } अन्यथा {
                dest.put(साँचो, value, is_जरा, errorCode);
            }
            यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI शून्य U_EXPORT2
ures_getAllChildrenWithFallback(अचर UResourceBundle *bundle, अचर वर्ण *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_वस्तुहरू_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
ures_get_मान_With_Fallback(अचर UResourceBundle *bundle, अचर वर्ण *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_मान &value, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    यदि (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }
    अचर UResourceBundle *rb;
    यदि (*path == 0) {
        // empty path
        rb = bundle;
    } अन्यथा {
        rb = ures_get_By_साँचो_With_Fallback(bundle, path, tempFillIn, &errorCode);
        यदि (U_FAILURE(errorCode)) {
            फर्काऊ;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI शून्य U_EXPORT2
ures_get_All_वस्तुहरू_With_Fallback(अचर UResourceBundle *bundle, अचर वर्ण *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    यदि (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }
    थाक_UResource_Bundle थाक_Bundle;
    अचर UResourceBundle *rb;
    यदि (*path == 0) {
        // empty path
        rb = bundle;
    } अन्यथा {
        rb = ures_get_By_साँचो_With_Fallback(bundle, path, थाक_Bundle.getAlias(), &errorCode);
        यदि (U_FAILURE(errorCode)) {
            फर्काऊ;
        }
    }
    // Get all table items with fallback.
    Resource_Data_मान value;
    get_All_वस्तुहरू_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_साँचो(अचर UResourceBundle* resB,
              अचर वर्ण* in_साँचो,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    अचर वर्ण *साँचो = in_साँचो;

    यदि (status==nullptr || U_FAILURE(*status)) {
        फर्काऊ fillIn;
    }
    यदि(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    यदि(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_साँचो(&resB->getResData(), resB->fRes, &t, &साँचो);
        यदि(res == RES_BOGUS) {
            साँचो = in_साँचो;
            यदि(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &साँचो, &res, status);
                यदि(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    फर्काऊ init_resb_result(dataEntry, res, साँचो, -1, resB, fillIn, status);
                } अन्यथा {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            फर्काऊ init_resb_result(resB->fData, res, साँचो, -1, resB, fillIn, status);
        }
    } 
#यदि 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    अन्यथा यदि(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == सत्य) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &साँचो, &res, status);
        यदि(U_SUCCESS(*status)) {
            फर्काऊ init_resb_result(dataEntry, res, साँचो, resB, fillIn, status);
        } अन्यथा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    फर्काऊ fillIn;
}

U_CAPI अचर char16_t* U_EXPORT2
ures_get_String_By_साँचो(अचर UResourceBundle* resB U_LIFETIME_BOUND,
                    अचर वर्ण* in_साँचो,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    अचर वर्ण* साँचो = in_साँचो;

    यदि (status==nullptr || U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    यदि(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_साँचो(&resB->getResData(), resB->fRes, &t, &साँचो);

        यदि(res == RES_BOGUS) {
            साँचो = in_साँचो;
            यदि(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &साँचो, &res, status);
                यदि(U_SUCCESS(*status)) {
                    विकल्प (RES_GET_TYPE(res)) {
                    पक्ष URES_STRING:
                    पक्ष URES_STRING_V2:
                        फर्काऊ res_getString({resB, साँचो}, &dataEntry->fData, res, len);
                    पक्ष URES_ALIAS:
                      {
                        अचर char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_साँचो(resB, in_साँचो, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        फर्काऊ result;
                      }
                    स्वभाव:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } अन्यथा {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            विकल्प (RES_GET_TYPE(res)) {
            पक्ष URES_STRING:
            पक्ष URES_STRING_V2:
                फर्काऊ res_getString({resB, साँचो}, &resB->getResData(), res, len);
            पक्ष URES_ALIAS:
              {
                अचर char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_साँचो(resB, in_साँचो, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                फर्काऊ result;
              }
            स्वभाव:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#यदि 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    अन्यथा यदि(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == सत्य) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &साँचो, &res, status);
        यदि(U_SUCCESS(*status)) {
            // TODO: Tracing
            फर्काऊ res_getString(rd, res, len);
        } अन्यथा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    फर्काऊ nullptr;
}

U_CAPI अचर वर्ण* U_EXPORT2
ures_get_UTF8_String_By_साँचो(अचर UResourceBundle* resB,
                        अचर वर्ण* साँचो,
                        वर्ण* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    अचर char16_t *s16 = ures_get_String_By_साँचो(resB, साँचो, &length16, status);
    फर्काऊ ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI अचर वर्ण* U_EXPORT2
ures_getLocaleInternal(अचर UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    यदि (status==nullptr || U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    } अन्यथा {
      फर्काऊ resourceBundle->fData->fName;
    }
}

U_CAPI अचर वर्ण* U_EXPORT2
ures_getLocale(अचर UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  फर्काऊ ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI अचर वर्ण* U_EXPORT2
ures_getLocaleByType(अचर UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    यदि (status==nullptr || U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    } अन्यथा {
        विकल्प(type) {
        पक्ष ULOC_ACTUAL_LOCALE:
            फर्काऊ resourceBundle->fData->fName;
        पक्ष ULOC_VALID_LOCALE:
            फर्काऊ resourceBundle->fValidLocaleDataEntry->fName;
        पक्ष ULOC_REQUESTED_LOCALE:
        स्वभाव:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ nullptr;
        }
    }
}

U_CFUNC अचर वर्ण*
ures_getName(अचर UResourceBundle* resB U_LIFETIME_BOUND) {
  यदि(resB == nullptr) {
    फर्काऊ nullptr;
  }

  फर्काऊ resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC अचर वर्ण*
ures_getPath(अचर UResourceBundle* resB U_LIFETIME_BOUND) {
  यदि(resB == nullptr) {
    फर्काऊ nullptr;
  }

  फर्काऊ resB->fData->fPath;
}
#endif

स्थिर UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  अचर वर्ण* path,
                  अचर वर्ण* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    यदि(U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }

    UResourceDataEntry *entry;
    यदि(openType != URES_OPEN_DIRECT) {
        यदि (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        यदि(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } अन्यथा {
        entry = entryOpenDirect(path, localeID, status);
    }
    यदि(U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        फर्काऊ nullptr;
    }

    UBool is_थाक_Object;
    यदि(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
        यदि(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ nullptr;
        }
        is_थाक_Object = असत्य;
    } अन्यथा {  // fill-in
        is_थाक_Object = ures_is_थाक_Object(r);
        ures_closeBundle(r, असत्य);
    }
    uprv_memset(r, 0, परिमाण(UResourceBundle));
    ures_set_Is_थाक_Object(r, is_थाक_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_शीर्ष_Level = सत्य;
    r->fRes = r->getResData().जरा_Res;
    r->fSize = res_गन्ती_Array_वस्तुहरू(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    फर्काऊ r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(अचर वर्ण* path, अचर वर्ण* localeID, UErrorCode* status) {
    फर्काऊ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_जरा, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(अचर वर्ण* path, अचर वर्ण* localeID, UErrorCode* status) {
    फर्काऊ ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_जरा, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(अचर वर्ण* path, अचर वर्ण* localeID, UErrorCode* status) {
    फर्काऊ ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI शून्य U_EXPORT2
ures_openFillIn(UResourceBundle *r, अचर वर्ण* path,
                अचर वर्ण* localeID, UErrorCode* status) {
    यदि(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_जरा, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI शून्य U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, अचर वर्ण* path, अचर वर्ण* localeID, UErrorCode* status) {
    यदि(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_गन्ती_Array_वस्तुहरू(अचर UResourceBundle* resourceBundle,
                  अचर वर्ण* resource_साँचो,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_थाक_Object(&resData);
    यदि (status==nullptr || U_FAILURE(*status)) {
        फर्काऊ 0;
    }
    यदि(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    ures_get_By_साँचो(resourceBundle, resource_साँचो, &resData, status);
    
    यदि(resData.getResData().data != nullptr) {
        int32_t result = res_गन्ती_Array_वस्तुहरू(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        फर्काऊ result;
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        फर्काऊ 0;
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
U_CAPI अचर वर्ण* U_EXPORT2
ures_get_Version_सङ्ख्या_Internal(अचर UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    यदि (!resourceBundle) फर्काऊ nullptr;

    यदि(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        अचर char16_t* minor_version = ures_get_String_By_साँचो(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (वर्ण *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        यदि (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            फर्काऊ nullptr;
        }
       
        यदि(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        अन्यथा {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    फर्काऊ resourceBundle->fVersion;
}

U_CAPI अचर वर्ण* U_EXPORT2
ures_get_Version_सङ्ख्या(अचर UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    फर्काऊ ures_get_Version_सङ्ख्या_Internal(resourceBundle);
}

U_CAPI शून्य U_EXPORT2 ures_getVersion(अचर UResourceBundle* resB, UVersionInfo versionInfo) {
    यदि (!resB) फर्काऊ;

    u_versionFromString(versionInfo, ures_get_Version_सङ्ख्या_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#यदि defined(URES_रूख_DEBUG)
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
ures_loc_गन्ती_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    फर्काऊ ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


स्थिर अचर वर्ण * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    अचर वर्ण *result = nullptr;
    int32_t len = 0;
    यदि (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_साँचो(k);
        len = (int32_t)uprv_strlen(result);
    }
    यदि (resultLength) {
        *resultLength = len;
    }
    फर्काऊ result;
}

स्थिर शून्य U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

स्थिर अचर UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_गन्ती_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(अचर वर्ण *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    यदि(U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(परिमाण(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(परिमाण(UEnumeration));
    यदि(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        फर्काऊ nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, परिमाण(UEnumeration));

    ures_init_थाक_Object(&myContext->installed);
    ures_init_थाक_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_साँचो(idx, INDEX_TAG, &myContext->installed, status);
    यदि(U_SUCCESS(*status)) {
#यदि defined(URES_रूख_DEBUG)
        ख_लिखो(मानक_त्रुटि, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_साँचो(&myContext->installed));
#endif
        en->context = myContext;
    } अन्यथा {
#यदि defined(URES_रूख_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    फर्काऊ en;
}

स्थिर UBool isLocaleInList(UEnumeration *locEnum, अचर वर्ण *loc_To_खोज, UErrorCode *status) {
    अचर वर्ण *loc;
    जबसम्म ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        यदि (uprv_strcmp(loc, loc_To_खोज) == 0) {
            फर्काऊ सत्य;
        }
    }
    फर्काऊ असत्य;
}

स्थिर शून्य getParentForFunctionalEquivalent(अचर वर्ण*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    यदि (res != nullptr) {
        ures_get_By_साँचो(res, "%%Parent", bund1, &subStatus);
        यदि (U_SUCCESS(subStatus)) {
            int32_t length16;
            अचर char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    यदि (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(वर्ण *result, int32_t resultCapacity,
                             अचर वर्ण *path, अचर वर्ण *resName, अचर वर्ण *keyword, अचर वर्ण *locid,
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
    यदि(U_FAILURE(*status)) फर्काऊ 0;
    CharString kwVal;
    यदि (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_मान(locid, keyword, subStatus);
        यदि (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    यदि (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#यदि defined(URES_रूख_DEBUG)
    ख_लिखो(मानक_त्रुटि, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_थाक_Object(&bund1);
    ures_init_थाक_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    यदि(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = सत्य;
        यदि (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    यदि(U_FAILURE(subStatus)) {
        *status = subStatus;
        फर्काऊ 0;
    }
    
    गर {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        यदि(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = असत्य;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#यदि defined(URES_रूख_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        यदि(U_FAILURE(subStatus)) {
            *status = subStatus;
        } अन्यथा यदि(subStatus == U_ZERO_ERROR) {
            ures_get_By_साँचो(res,resName,&bund1, &subStatus);
            यदि(subStatus == U_ZERO_ERROR) {
                अचर char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#यदि defined(URES_रूख_DEBUG)
                ख_लिखो(मानक_त्रुटि, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_साँचो(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                यदि(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#यदि defined(URES_रूख_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    यदि(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#यदि defined(URES_रूख_DEBUG)
                        ख_लिखो(मानक_त्रुटि, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        यदि (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        यदि (found != parent) {
            parent.copyFrom(found, subStatus);
        } अन्यथा {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } जबसम्म(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    गर {
        res = ures_open(path, parent.data(), &subStatus);
        यदि((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = असत्य;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#यदि defined(URES_रूख_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        यदि(U_FAILURE(subStatus)) {
            *status = subStatus;
        } अन्यथा यदि(subStatus == U_ZERO_ERROR) {
            ures_get_By_साँचो(res,resName,&bund1, &subStatus);
#यदि defined(URES_रूख_DEBUG)
/**/ ख_लिखो(मानक_त्रुटि,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            यदि(subStatus == U_ZERO_ERROR) {
                ures_get_By_साँचो(&bund1, kwVal.data(), &bund2, &subStatus);
#यदि defined(URES_रूख_DEBUG)
/**/ ख_लिखो(मानक_त्रुटि,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                यदि(subStatus == U_ZERO_ERROR) {
#यदि defined(URES_रूख_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    यदि (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } अन्यथा {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        यदि(defLoc.length() > full.length()) {
                          अचर char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#यदि defined(URES_रूख_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_साँचो(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          यदि(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#यदि defined(URES_रूख_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#यदि defined(URES_रूख_DEBUG)
                        अन्यथा {
                          ख_लिखो(मानक_त्रुटि, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } अन्यथा {
#यदि defined(URES_रूख_DEBUG)
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
        यदि (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            अचर वर्ण *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            यदि (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                यदि (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = सत्य;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        यदि (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } जबसम्म(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    यदि(full.isEmpty() && kwVal != defVal) {
#यदि defined(URES_रूख_DEBUG)
        ख_लिखो(मानक_त्रुटि, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        गर { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            यदि((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = असत्य;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#यदि defined(URES_रूख_DEBUG)
            ख_लिखो(मानक_त्रुटि, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            यदि(U_FAILURE(subStatus)) {
                *status = subStatus;
            } अन्यथा यदि(subStatus == U_ZERO_ERROR) {
                ures_get_By_साँचो(res,resName,&bund1, &subStatus);
                यदि(subStatus == U_ZERO_ERROR) {
                    ures_get_By_साँचो(&bund1, kwVal.data(), &bund2, &subStatus);
                    यदि(subStatus == U_ZERO_ERROR) {
#यदि defined(URES_रूख_DEBUG)
                        ख_लिखो(मानक_त्रुटि, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        यदि (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } अन्यथा {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        यदि(defLoc.length() > full.length()) {
                          अचर char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#यदि defined(URES_रूख_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_साँचो(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          यदि(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#यदि defined(URES_रूख_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#यदि defined(URES_रूख_DEBUG)
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
        } जबसम्म(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    यदि(U_SUCCESS(*status)) {
        यदि(full.isEmpty()) {
#यदि defined(URES_रूख_DEBUG)
          ख_लिखो(मानक_त्रुटि, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } अन्यथा यदि(omitDefault) {
#यदि defined(URES_रूख_DEBUG)
          ख_लिखो(मानक_त्रुटि,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          यदि(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            यदि(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#यदि defined(URES_रूख_DEBUG)
              ख_लिखो(मानक_त्रुटि, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        यदि(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } अन्यथा यदि(!omitDefault) {
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

    यदि(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        यदि(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        यदि(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } अन्यथा {
        length = 0;
        result[0]=0;
    }
    फर्काऊ u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_मानहरू(अचर वर्ण *path, अचर वर्ण *keyword, UErrorCode *status)
{
#define मानहरू_BUF_SIZE 2048
#define मानहरू_LIST_SIZE 512
    
    वर्ण       मानहरू_Buf[मानहरू_BUF_SIZE];
    int32_t    मानहरू_Index = 0;
    अचर वर्ण *मानहरू_List[मानहरू_LIST_SIZE];
    int32_t    मानहरू_गन्ती = 0;
    
    अचर वर्ण *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_थाक_Object(&item);
    ures_init_थाक_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    यदि(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        फर्काऊ nullptr;
    }
    
    मानहरू_Buf[0]=0;
    मानहरू_Buf[1]=0;

    जबसम्म ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#यदि defined(URES_रूख_DEBUG)
        यदि(!bund || U_FAILURE(subStatus)) {
            ख_लिखो(मानक_त्रुटि, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_साँचो(bund, keyword, &item, &subStatus);
        
        यदि(!bund || U_FAILURE(subStatus)) {
#यदि defined(URES_रूख_DEBUG)
            ख_लिखो(मानक_त्रुटि, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            जारी;
        }

        जबसम्म ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            अचर वर्ण *k;
            int32_t i;
            k = ures_get_साँचो(subPtr);

#यदि defined(URES_रूख_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            यदि(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                जारी;
            }
            लागि(i=0; i<मानहरू_गन्ती; i++) {
                यदि(!uprv_strcmp(मानहरू_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    रोक;
                }
            }
            यदि(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                यदि((मानहरू_गन्ती >= (मानहरू_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((मानहरू_Index+kLen+1+1) >= मानहरू_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } अन्यथा {
                    uprv_strcpy(मानहरू_Buf+मानहरू_Index, k);
                    मानहरू_List[मानहरू_गन्ती++] = मानहरू_Buf+मानहरू_Index;
                    मानहरू_Index += kLen;
#यदि defined(URES_रूख_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    मानहरू_Buf[मानहरू_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    मानहरू_Buf[मानहरू_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#यदि defined(URES_रूख_DEBUG)
    ख_लिखो(मानक_त्रुटि, "%s:  size %d, #%d\n", u_errorName(*status), 
        मानहरू_Index, मानहरू_गन्ती);
#endif
    फर्काऊ uloc_openKeywordList(मानहरू_Buf, मानहरू_Index, status);
}
#यदि 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(अचर UResourceBundle* res1, अचर UResourceBundle* res2){
    यदि(res1==nullptr || res2==nullptr){
        फर्काऊ res1==res2; /* pointer comparison */
    }
    यदि(res1->f_साँचो==nullptr||  res2->f_साँचो==nullptr){
        फर्काऊ (res1->f_साँचो==res2->f_साँचो);
    }अन्यथा{
        यदि(uprv_strcmp(res1->f_साँचो, res2->f_साँचो)!=0){
            फर्काऊ असत्य;
        }
    }
    यदि(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        फर्काऊ असत्य;
    }
    यदि(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        फर्काऊ (res1->fData->fPath == res2->fData->fPath);
    }अन्यथा{
        यदि(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            फर्काऊ असत्य;
        }
    }
    यदि(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        फर्काऊ असत्य;
    }
    यदि(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        फर्काऊ असत्य;
    }
    यदि(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        फर्काऊ असत्य;
    }
    यदि(res1->fRes != res2->fRes){
        फर्काऊ असत्य;
    }
    फर्काऊ सत्य;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(अचर UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    यदि(U_FAILURE(*status) || res == nullptr){
        फर्काऊ nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    यदि(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }अन्यथा{
        ret = bundle;
    }
    फर्काऊ ret;
}
U_CAPI अचर UResourceBundle* U_EXPORT2
ures_getParentBundle(अचर UResourceBundle* res){
    यदि(res==nullptr){
        फर्काऊ nullptr;
    }
    फर्काऊ res->fParentRes;
}
#endif

U_CAPI शून्य U_EXPORT2
ures_get_Version_By_साँचो(अचर UResourceBundle* res, अचर वर्ण *साँचो, UVersionInfo ver, UErrorCode *status) {
  अचर char16_t *str;
  int32_t len;
  str = ures_get_String_By_साँचो(res, साँचो, &len, status);
  यदि(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
