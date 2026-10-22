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

#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "ustr_imp.h"
#અન્તર્ભાવ "cwchar.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "unicode/uenum.h"
#અન્તર્ભાવ "uenumimp.h"
#અન્તર્ભાવ "ulocimp.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "uresdata.h"

using નામાકાશ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
સ્થિર UHashtable *cache = nullptr;
સ્થિર icu::UInitOnce gCacheInitOnce {};

સ્થિર UMutex resbMutex;

/* INTERNAL: hashes an entry  */
સ્થિર int32_t U_CALLCONV hashEntry(અચલ UHashTok parm) {
    UResourceDataEntry* b = static_cast<UResourceDataEntry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    પાછા uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
સ્થિર UBool U_CALLCONV compareEntries(અચલ UHashTok p1, અચલ UHashTok p2) {
    UResourceDataEntry* b1 = static_cast<UResourceDataEntry*>(p1.pointer);
    UResourceDataEntry* b2 = static_cast<UResourceDataEntry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    પાછા uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
સ્થિર UBool chopLocale(અક્ષર *name) {
    અક્ષર *i = uprv_strrchr(name, '_');

    જો(i != nullptr) {
        *i = '\0';
        પાછા સાચું;
    }

    પાછા ખોટું;
}

સ્થિર UBool hasVariant(અચલ અક્ષર* localeID) {
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
    પાછા sink.સંખ્યા_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#define INCLUDED_FROM_URESBUND_CPP
#અન્તર્ભાવ "localefallback_data.h"

સ્થિર અચલ અક્ષર* performFallbackLookup(અચલ અક્ષર* ચાવી,
                                         અચલ અક્ષર* ચાવી_Strs,
                                         અચલ અક્ષર* કિંમત_Strs U_LIFETIME_BOUND,
                                         અચલ int32_t* lookupTable,
                                         int32_t lookupTableLength) {
    અચલ int32_t* bottom = lookupTable;
    અચલ int32_t* ટોચ = lookupTable + lookupTableLength;

    જ્યારે (bottom < ટોચ) {
        // Effectively, divide by 2 and round down to an even index
        અચલ int32_t* મધ્ય = bottom + (((ટોચ - bottom) / 4) * 2);
        અચલ અક્ષર* entry_ચાવી = &(ચાવી_Strs[*મધ્ય]);
        int32_t strcmpResult = uprv_strcmp(ચાવી, entry_ચાવી);
        જો (strcmpResult == 0) {
            પાછા &(કિંમત_Strs[મધ્ય[1]]);
        } નહીંતર જો (strcmpResult < 0) {
            ટોચ = મધ્ય;
        } નહીંતર {
            bottom = મધ્ય + 2;
        }
    }
    પાછા nullptr;
}

સ્થિર CharString getDefaultScript(અચલ CharString& language, અચલ CharString& region) {
    અચલ અક્ષર* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString result("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    જો (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        જો (U_FAILURE(err)) {
            પાછા result;
        }
        defaultScript = performFallbackLookup(localeID.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    જો (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.data(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    જો (defaultScript != nullptr) {
        result.clear();
        result.append(defaultScript, err);
    }
    પાછા result;
}

ગણના UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_મૂળ,
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
    URES_OPEN_LOCALE_મૂળ,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
પ્રકારવ્યાખ્યા ગણના UResOpenType UResOpenType;

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
સ્થિર તાર્કિક getParentLocaleID(અક્ષર *name, અચલ અક્ષર *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    જો (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        પાછા chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    જો (U_FAILURE(err)) {
        // hopefully this never happens...
        પાછા chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    જો (openType == URES_OPEN_LOCALE_DEFAULT_મૂળ) {
        અચલ અક્ષર* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        જો (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            પાછા સાચું;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    જો (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        જો (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } નહીંતર {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } નહીંતર જો (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        જો (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } નહીંતર {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } નહીંતર જો (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        જો (openType != URES_OPEN_LOCALE_DEFAULT_મૂળ || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } નહીંતર {
            પાછા ખોટું;
        }
    } નહીંતર {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        પાછા ખોટું;
    }
    જો (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.data());
        પાછા સાચું;
    } નહીંતર {
        પાછા ખોટું;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
સ્થિર UBool mayHaveParent(અક્ષર *name) {
    પાછા (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
સ્થિર શૂન્ય entryIncrease(UResourceDataEntry *entry) {
    Mutex lock(&resbMutex);
    entry->f_ગણતરી_Existing++;
    જ્યારે(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_ગણતરી_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
સ્થિર UResourceDataEntry *getFallbackData(
        અચલ UResourceBundle *resBundle,
        અચલ અક્ષર **resTag, Resource *res, UErrorCode *status) {
    UResourceDataEntry *dataEntry = resBundle->fData;
    int32_t indexR = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    જો(dataEntry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        પાછા nullptr;
    }
    જો(dataEntry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_ચાવી(&(dataEntry->fData), dataEntry->fData.મૂળ_Res, &indexR, resTag); /* try to get data from there */
        i++;
    }
    જો(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        જ્યારે(*res == RES_BOGUS && dataEntry->fParent != nullptr) {
            dataEntry = dataEntry->fParent;
            જો(dataEntry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_ચાવી(&(dataEntry->fData), dataEntry->fData.મૂળ_Res, &indexR, resTag);
            }
        }
    }

    જો(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        પાછા nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    જો(i>1) {
        જો(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_મૂળ_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } નહીંતર {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    પાછા dataEntry;
}

સ્થિર શૂન્ય
free_entry(UResourceDataEntry *entry) {
    UResourceDataEntry *alias;
    res_unload(&(entry->fData));
    જો(entry->fName != nullptr && entry->fName != entry->fNameBuffer) {
        uprv_free(entry->fName);
    }
    જો(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    જો(entry->fPool != nullptr) {
        --entry->fPool->f_ગણતરી_Existing;
    }
    alias = entry->fAlias;
    જો(alias != nullptr) {
        જ્યારે(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_ગણતરી_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
સ્થિર int32_t ures_flushCache()
{
    UResourceDataEntry *resB;
    int32_t pos;
    int32_t rb_Deleted_અંક = 0;
    અચલ UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    જો (cache == nullptr) {
        પાછા 0;
    }

    કરો {
        deletedMore = ખોટું;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        જ્યારે ((e = uhash_nextElement(cache, &pos)) != nullptr)
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

            જો (resB->f_ગણતરી_Existing == 0) {
                rb_Deleted_અંક++;
                deletedMore = સાચું;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } જ્યારે(deletedMore);

    પાછા rb_Deleted_અંક;
}

#ifdef URES_DEBUG
#અન્તર્ભાવ <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = ખોટું;
  int32_t pos = UHASH_FIRST;
  અચલ UHashElement *e;
  UResourceDataEntry *resB;
  
    Mutex lock(&resbMutex);
    જો (cache == nullptr) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      પાછા ખોટું;
    }

    જ્યારે ((e = uhash_nextElement(cache, &pos)) != nullptr) {
      cacheNotEmpty=સાચું;
      resB = (UResourceDataEntry *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (શૂન્ય*)resB, resB->f_ગણતરી_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (શૂન્ય*)resB->fPool,
              (શૂન્ય*)resB->fAlias,
              (શૂન્ય*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_ગણતરી(cache));
    પાછા cacheNotEmpty;
}

#endif

સ્થિર UBool U_CALLCONV ures_cleanup()
{
    જો (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    પાછા સાચું;
}

/** INTERNAL: Initializes the cache for resources */
સ્થિર શૂન્ય U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
સ્થિર શૂન્ય initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

સ્થિર શૂન્ય setEntryName(UResourceDataEntry *res, અચલ અક્ષર *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    જો(res->fName != nullptr && res->fName != res->fNameBuffer) {
        uprv_free(res->fName);
    }
    જો (len < static_cast<int32_t>(કદ(res->fNameBuffer))) {
        res->fName = res->fNameBuffer;
    }
    નહીંતર {
        res->fName = static_cast<અક્ષર*>(uprv_malloc(len + 1));
    }
    જો(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } નહીંતર {
        uprv_strcpy(res->fName, name);
    }
}

સ્થિર UResourceDataEntry *
getPoolEntry(અચલ અક્ષર *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
સ્થિર UResourceDataEntry *init_entry(અચલ અક્ષર *localeID, અચલ અક્ષર *path, UErrorCode *status) {
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry find;
    /*int32_t hashValue;*/
    અચલ અક્ષર *name;
    અક્ષર aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    જો(U_FAILURE(*status)) {
        પાછા nullptr;
    }

    /* here we try to deduce the right locale name */
    જો(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } નહીંતર જો(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_મૂળ_Locale_Name;
    } નહીંતર { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<અક્ષર*>(name);
    find.fPath = const_cast<અક્ષર*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResourceDataEntry*>(uhash_get(cache, &find));
    જો(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResourceDataEntry*>(uprv_malloc(કદ(UResourceDataEntry)));
        જો(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            પાછા nullptr;
        }

        uprv_memset(r, 0, કદ(UResourceDataEntry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        જો (U_FAILURE(*status)) {
            uprv_free(r);
            પાછા nullptr;
        }

        જો(path != nullptr) {
            r->fPath = uprv_strdup(path);
            જો(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                પાછા nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->fData), r->fPath, r->fName, status);

        જો (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            જો (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                પાછા nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } નહીંતર { /* if we have a regular entry */
            Resource aliasres;
            જો (r->fData.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                જો (U_SUCCESS(*status)) {
                    અચલ int32_t *poolIndexes = r->fPool->fData.p_મૂળ + 1;
                    જો(r->fData.p_મૂળ[1 + URES_INDEX_POOL_CHECKSUM] == poolIndexes[URES_INDEX_POOL_CHECKSUM]) {
                        r->fData.pool_Bundle_ચાવીઓ = reinterpret_cast<અચલ અક્ષર*>(poolIndexes + (poolIndexes[URES_INDEX_LENGTH] & 0xff));
                        r->fData.poolBundleStrings = r->fPool->fData.p16BitUnits;
                    } નહીંતર {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } નહીંતર {
                    r->fBogus = *status;
                }
            }
            જો (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->fData), "%%ALIAS");
                જો (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    અચલ char16_t *alias = res_getStringNoTrace(&(r->fData), aliasres, &aliasLen);
                    જો(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResourceDataEntry *oldR = nullptr;
            જો ((oldR = static_cast<UResourceDataEntry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (શૂન્ય *)r, r, &cacheStatus);
                જો (U_FAILURE(cacheStatus)) {
                    *status = cacheStatus;
                    free_entry(r);
                    r = nullptr;
                }
            } નહીંતર {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_entry(r);
                r = oldR;
            }
        }

    }
    જો(r != nullptr) {
        /* return the real bundle */
        જ્યારે(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_ગણતરી_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        જો(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    પાછા r;
}

સ્થિર UResourceDataEntry *
getPoolEntry(અચલ અક્ષર *path, UErrorCode *status) {
    UResourceDataEntry *poolBundle = init_entry(kPoolBundleName, path, status);
    જો( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->fData.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    પાછા poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
સ્થિર UResourceDataEntry *
findFirstExisting(અચલ અક્ષર* path, અક્ષર* name, અચલ અક્ષર* defaultLocale, UResOpenType openType,
                  UBool *is_મૂળ, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResourceDataEntry *r = nullptr;
    UBool hasRealData = ખોટું;
    *foundParent = સાચું; /* we're starting with a fresh name */
    અક્ષર origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    જ્યારે(*foundParent && !hasRealData) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        જો (U_FAILURE(*status)) {
            પાછા nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        hasRealData = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        જો(!hasRealData) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_ગણતરી_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } નહીંતર {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_મૂળ = static_cast<UBool>(uprv_strcmp(name, k_મૂળ_Locale_Name) == 0);

        /*Fallback data stuff*/
        જો (!hasRealData) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } નહીંતર {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        જો (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    પાછા r;
}

સ્થિર શૂન્ય ures_set_Is_ઢગલો_Object( UResourceBundle* resB, UBool state) {
    જો(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } નહીંતર {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

સ્થિર UBool ures_is_ઢગલો_Object(અચલ UResourceBundle* resB) {
  પાછા((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?ખોટું:સાચું);
}


U_CFUNC શૂન્ય ures_init_ઢગલો_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, કદ(UResourceBundle));
  ures_set_Is_ઢગલો_Object(resB, સાચું);
}

U_NAMESPACE_BEGIN

ઢગલો_UResource_Bundle::ઢગલો_UResource_Bundle() {
    ures_init_ઢગલો_Object(&bundle);
}

ઢગલો_UResource_Bundle::~ઢગલો_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

સ્થિર UBool  // returns U_SUCCESS(*status)
load_Parents_Except_મૂળ(UResourceDataEntry *&t1,
                      અક્ષર name[], int32_t nameCapacity,
                      UBool usingUSRData, અક્ષર usrDataPath[], UErrorCode *status) {
    જો (U_FAILURE(*status)) { પાછા ખોટું; }
    UBool checkParent = સાચું;
    જ્યારે (checkParent && t1->fParent == nullptr && !t1->fData.noFallback &&
            res_getResource(&t1->fData,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->fData, "%%Parent");
        જો (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            અચલ char16_t *parentLocaleName = res_getStringNoTrace(&(t1->fData), parentRes, &parentLocaleLen);
            જો(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                જો (uprv_strcmp(name, k_મૂળ_Locale_Name) == 0) {
                    પાછા સાચું;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResourceDataEntry *t2 = init_entry(name, t1->fPath, &parentStatus);
        જો (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            પાછા ખોટું;
        }
        UResourceDataEntry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        જો (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            જો (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                પાછા ખોટું;
            }
        }

        જો (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } નહીંતર {
            t1->fParent = t2;
            જો (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_ગણતરી_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    પાછા સાચું;
}

સ્થિર UBool  // returns U_SUCCESS(*status)
ઉમેરો_મૂળ_Bundle(UResourceDataEntry *&t1, UErrorCode *status) {
    જો (U_FAILURE(*status)) { પાછા ખોટું; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResourceDataEntry *t2 = init_entry(k_મૂળ_Locale_Name, t1->fPath, &parentStatus);
    જો (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        પાછા ખોટું;
    }
    t1->fParent = t2;
    t1 = t2;
    પાછા સાચું;
}

સ્થિર UResourceDataEntry *entryOpen(અચલ અક્ષર* path, અચલ અક્ષર* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResourceDataEntry *r = nullptr;
    UResourceDataEntry *t1 = nullptr;
    UBool isDefault = ખોટું;
    UBool is_મૂળ = ખોટું;
    UBool hasRealData = ખોટું;
    UBool hasChopped = સાચું;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    અક્ષર name[ULOC_FULLNAME_CAPACITY];
    અક્ષર usrDataPath[96];

    initCache(status);

    જો(U_FAILURE(*status)) {
        પાછા nullptr;
    }

    uprv_strncpy(name, localeID, કદ(name) - 1);
    name[કદ(name) - 1] = 0;

    જો ( usingUSRData ) {
        જો ( path == nullptr ) {
            uprv_strcpy(usrDataPath, U_USRDATA_NAME);
        } નહીંતર {
            uprv_strncpy(usrDataPath, path, કદ(usrDataPath) - 1);
            usrDataPath[0] = 'u';
            usrDataPath[1] = 's';
            usrDataPath[2] = 'r';
            usrDataPath[કદ(usrDataPath) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    અચલ અક્ષર *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_મૂળ, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    જો (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        જાઓ finish;
    }

    જો(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        hasRealData = સાચું;
        જો ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResourceDataEntry *u1 = init_entry(t1->fName, usrDataPath, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            જો (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                જાઓ finish;
            }
            જો ( u1 != nullptr ) {
                જો(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } નહીંતર {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_ગણતરી_Existing = 0;
                }
            }
        }
        જો ((hasChopped || mayHaveParent(name)) && !is_મૂળ) {
            જો (!load_Parents_Except_મૂળ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                જાઓ finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    જો(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_મૂળ && !isDefault && !is_મૂળ) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_મૂળ, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        જો (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            જાઓ finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        જો(r != nullptr) { /* the default locale exists */
            t1 = r;
            hasRealData = સાચું;
            isDefault = સાચું;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            જો ((hasChopped || mayHaveParent(name)) && !is_મૂળ) {
                જો (!load_Parents_Except_મૂળ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usrDataPath, status)) {
                    જાઓ finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    જો(r == nullptr) {
        uprv_strcpy(name, k_મૂળ_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_મૂળ, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        જો (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            જાઓ finish;
        }
        જો(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            hasRealData = સાચું;
        } નહીંતર { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            જાઓ finish;
        }
    } નહીંતર જો(!is_મૂળ && uprv_strcmp(t1->fName, k_મૂળ_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->fData.noFallback) {
        જો (!ઉમેરો_મૂળ_Bundle(t1, status)) {
            જાઓ finish;
        }
        જો(!hasRealData) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    જ્યારે(r != nullptr && !is_મૂળ && t1->fParent != nullptr) {
        t1->fParent->f_ગણતરી_Existing++;
        t1 = t1->fParent;
    }

finish:
    જો(U_SUCCESS(*status)) {
        જો(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        પાછા r;
    } નહીંતર {
        પાછા nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
સ્થિર UResourceDataEntry *
entryOpenDirect(અચલ અક્ષર* path, અચલ અક્ષર* localeID, UErrorCode* status) {
    initCache(status);
    જો(U_FAILURE(*status)) {
        પાછા nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    જો (localeID == nullptr) {
        localeID = uloc_getDefault();
    } નહીંતર જો (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_મૂળ_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResourceDataEntry *r = init_entry(localeID, path, status);
    જો(U_SUCCESS(*status)) {
        જો(r->fBogus != U_ZERO_ERROR) {
            r->f_ગણતરી_Existing--;
            r = nullptr;
        }
    } નહીંતર {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResourceDataEntry *t1 = r;
    જો(r != nullptr && uprv_strcmp(localeID, k_મૂળ_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->fData.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        અક્ષર name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        જો(!chopLocale(name) || uprv_strcmp(name, k_મૂળ_Locale_Name) == 0 ||
                load_Parents_Except_મૂળ(t1, name, UPRV_LENGTHOF(name), ખોટું, nullptr, status)) {
            જો(uprv_strcmp(t1->fName, k_મૂળ_Locale_Name) != 0 && t1->fParent == nullptr) {
                ઉમેરો_મૂળ_Bundle(t1, status);
            }
        }
        જો(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    જો(r != nullptr) {
        // TODO: Does this ever loop?
        જ્યારે(t1->fParent != nullptr) {
            t1->fParent->f_ગણતરી_Existing++;
            t1 = t1->fParent;
        }
    }
    પાછા r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
સ્થિર શૂન્ય entryCloseInt(UResourceDataEntry *resB) {
    UResourceDataEntry *p = resB;

    જ્યારે(resB != nullptr) {
        p = resB->fParent;
        resB->f_ગણતરી_Existing--;

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

સ્થિર શૂન્ય entryClose(UResourceDataEntry *resB) {
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
સ્થિર શૂન્ય ures_appendResPath(UResourceBundle *resB, અચલ અક્ષર* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    જો(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    જો(RES_BUFSIZE <= resB->fResPathLen+1) {
        જો(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<અક્ષર*>(uprv_malloc((resB->fResPathLen + 1) * કદ(અક્ષર)));
            /* Check that memory was allocated correctly. */
            જો (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } નહીંતર {
            અક્ષર* temp = static_cast<અક્ષર*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * કદ(અક્ષર)));
            /* Check that memory was reallocated correctly. */
            જો (temp == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
            resB->fResPath = temp;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

સ્થિર શૂન્ય ures_freeResPath(UResourceBundle *resB) {
    જો (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

સ્થિર શૂન્ય
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    જો(resB != nullptr) {
        જો(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        જો(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        જો(ures_is_ઢગલો_Object(resB) == ખોટું && freeBundleObj) {
            uprv_free(resB);
        }
#જો 0 /*U_DEBUG*/
        નહીંતર {
            /* poison the data */
            uprv_memset(resB, -1, કદ(UResourceBundle));
        }
#endif
    }
}

U_CAPI શૂન્ય  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, સાચું);
}

નામાકાશ {

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  અચલ અક્ષર* ચાવી,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  અચલ અક્ષર* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(અચલ ResourceData& resData,
                               Resource r,
                               અચલ અક્ષર* ચાવી,
                               int32_t idx,
                               UResourceDataEntry* validLocaleDataEntry,
                               અચલ અક્ષર* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    જો (U_FAILURE(*status)) { પાછા resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    અચલ char16_t *alias = res_getAlias(&resData, r, &len);
    જો(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા resB;
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
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }

    // We have an alias, now let's cut it up.
    અચલ અક્ષર *path = nullptr, *locale = nullptr, *ચાવી_Path = nullptr;
    જો(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        અક્ષર *chAliasData = chAlias.data();
        અક્ષર *sep = chAliasData + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        જો(sep != nullptr) {
            *sep++ = 0;
        }
        જો(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            ચાવી_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } નહીંતર {
            જો(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            જો (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } નહીંતર {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                જો(sep != nullptr) {
                    *sep++ = 0;
                }
                ચાવી_Path = sep;
            }
        }
    } નહીંતર {
        // No path, start with a locale.
        અક્ષર *sep = chAlias.data();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        જો(sep != nullptr) {
            *sep++ = 0;
        }
        ચાવી_Path = sep;
        path = validLocaleDataEntry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResourceDataEntry *dataEntry;
    જો (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        dataEntry = validLocaleDataEntry;
    } નહીંતર {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        જો(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            પાછા resB;
        }
        dataEntry = mainRes->fData;
    }

    અચલ અક્ષર* temp = nullptr;
    જો(ચાવી_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = dataEntry->fData.મૂળ_Res;
        જો(containerResPath) {
            chAlias.clear().append(containerResPath, *status);
            જો (U_FAILURE(*status)) {
                પાછા nullptr;
            }
            અક્ષર *a_ચાવી = chAlias.data();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&dataEntry->fData, r, &a_ચાવી, &temp);
        }
        જો(ચાવી) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(ચાવી, *status);
            જો (U_FAILURE(*status)) {
                પાછા nullptr;
            }
            અક્ષર *a_ચાવી = chAlias.data();
            r = res_findResource(&dataEntry->fData, r, &a_ચાવી, &temp);
        } નહીંતર જો(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            જો(URES_IS_TABLE(type)) {
                અચલ અક્ષર *a_ચાવી;
                r = res_getTableItemByIndex(&dataEntry->fData, r, idx, &a_ચાવી);
            } નહીંતર { /* array */
                r = res_getArrayItem(&dataEntry->fData, r, idx);
            }
        }
        જો(r != RES_BOGUS) {
            resB = init_resb_result(
                dataEntry, r, temp, -1, validLocaleDataEntry, nullptr, recursionDepth+1,
                resB, status);
        } નહીંતર {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } નહીંતર {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString pathBuf(ચાવી_Path, *status);
        જો (U_FAILURE(*status)) {
            પાછા nullptr;
        }
        અક્ષર *myPath = pathBuf.data();
        containerResPath = nullptr;
        // Now we have fallback following here.
        માટે(;;) {
            r = dataEntry->fData.મૂળ_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            જ્યારે(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(dataEntry->fData), r, &myPath, &temp);
                જો(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    તોડો;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_result(
                    dataEntry, r, temp, -1,
                    validLocaleDataEntry, containerResPath, recursionDepth+1,
                    resB, status);
                જો (U_FAILURE(*status)) {
                    તોડો;
                }
                જો (temp == nullptr || uprv_strcmp(ચાવી_Path, temp) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, ચાવી_Path, static_cast<int32_t>(uprv_strlen(ચાવી_Path)), status);
                    જો(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    જો (U_FAILURE(*status)) {
                        તોડો;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                dataEntry = resB->fData;
                containerResPath = resB->fResPath;
            }
            જો (U_FAILURE(*status) || r != RES_BOGUS) {
                તોડો;
            }
            // Fall back to the parent bundle, if there is one.
            dataEntry = dataEntry->fParent;
            જો (dataEntry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                તોડો;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.data();
            uprv_strcpy(myPath, ચાવી_Path);
        }
    }
    જો(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    પાછા resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  અચલ અક્ષર* ચાવી,
                                  int32_t idx,
                                  UResourceDataEntry* validLocaleDataEntry,
                                  અચલ અક્ષર* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    જો(status == nullptr || U_FAILURE(*status)) {
        પાછા resB;
    }
    જો (validLocaleDataEntry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    જો(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        જો(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            પાછા resB;
        }
        પાછા getAliasTargetAsResourceBundle(
            dataEntry->fData, r, ચાવી, idx,
            validLocaleDataEntry, containerResPath, recursionDepth, resB, status);
    }
    જો(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(કદ(UResourceBundle)));
        જો (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            પાછા nullptr;
        }
        ures_set_Is_ઢગલો_Object(resB, ખોટું);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } નહીંતર {
        જો(resB->fData != nullptr) {
            entryClose(resB->fData);
        }
        જો(resB->fVersion != nullptr) {
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
        જો(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->fData = dataEntry;
    entryIncrease(resB->fData);
    resB->fHasFallback = ખોટું;
    resB->f_Is_ટોચ_Level = ખોટું;
    resB->fIndex = -1;
    resB->f_ચાવી = ચાવી; 
    resB->fValidLocaleDataEntry = validLocaleDataEntry;
    જો(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    જો(ચાવી != nullptr) {
        ures_appendResPath(resB, ચાવી, static_cast<int32_t>(uprv_strlen(ચાવી)), status);
        જો(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } નહીંતર જો(idx >= 0) {
        અક્ષર buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        જો(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, કદ(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->fSize = res_ગણતરી_Array_વસ્તુઓ(&resB->getResData(), resB->fRes);
    ResourceTracer(resB).trace("get");
    પાછા resB;
}

UResourceBundle* init_resb_result(UResourceDataEntry* dataEntry,
                                  Resource r,
                                  અચલ અક્ષર* ચાવી,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  અચલ UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    પાછા init_resb_result(
        dataEntry, r, ચાવી, idx,
        container->fValidLocaleDataEntry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               અચલ UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_ઢગલો_Object;
    જો(U_FAILURE(*status) || r == original) {
        પાછા r;
    }
    જો(original != nullptr) {
        જો(r == nullptr) {
            is_ઢગલો_Object = ખોટું;
            r = static_cast<UResourceBundle*>(uprv_malloc(કદ(UResourceBundle)));
            /* test for nullptr */
            જો (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                પાછા nullptr;
            }
        } નહીંતર {
            is_ઢગલો_Object = ures_is_ઢગલો_Object(r);
            ures_closeBundle(r, ખોટું);
        }
        uprv_memcpy(r, original, કદ(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        જો(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_ઢગલો_Object(r, is_ઢગલો_Object);
        જો(r->fData != nullptr) {
            entryIncrease(r->fData);
        }
    }
    પાછા r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI અચલ char16_t* U_EXPORT2 ures_getString(અચલ UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    અચલ char16_t *s;
    જો (status==nullptr || U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    s = res_getString({resB}, &resB->getResData(), resB->fRes, len);
    જો (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા s;
}

સ્થિર અચલ અક્ષર*
ures_toUTF8String(અચલ char16_t* s16,
                  int32_t length16,
                  અક્ષર* dest U_LIFETIME_BOUND,
                  int32_t* pLength,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો (pLength != nullptr) {
        capacity = *pLength;
    } નહીંતર {
        capacity = 0;
    }
    જો (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    જો (length16 == 0) {
        /* empty string, return as read-only pointer */
        જો (pLength != nullptr) {
            *pLength = 0;
        }
        જો (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            પાછા dest;
        } નહીંતર {
            પાછા "";
        }
    } નહીંતર {
        /* We need to transform the string to the destination buffer. */
        જો (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            પાછા u_strToUTF8(nullptr, 0, pLength, s16, length16, status);
        }
        જો (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            જો (capacity > maxLength) {
                dest += capacity - maxLength;
                capacity = maxLength;
            }
        }
        પાછા u_strToUTF8(dest, capacity, pLength, s16, length16, status);
    }
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ures_getUTF8String(અચલ UResourceBundle* resB,
                   અક્ષર* dest U_LIFETIME_BOUND,
                   int32_t* pLength,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    અચલ char16_t *s16 = ures_getString(resB, &length16, status);
    પાછા ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

U_CAPI અચલ uint8_t* U_EXPORT2
ures_get_દ્વિઅંકી(અચલ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  અચલ uint8_t *p;
  જો (status==nullptr || U_FAILURE(*status)) {
    પાછા nullptr;
  }
  જો(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા nullptr;
  }
  p = res_get_દ્વિઅંકી({resB}, &resB->getResData(), resB->fRes, len);
  જો (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  પાછા p;
}

U_CAPI અચલ int32_t* U_EXPORT2
ures_getIntVector(અચલ UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  અચલ int32_t *p;
  જો (status==nullptr || U_FAILURE(*status)) {
    પાછા nullptr;
  }
  જો(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા nullptr;
  }
  p = res_getIntVector({resB}, &resB->getResData(), resB->fRes, len);
  જો (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  પાછા p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(અચલ UResourceBundle* resB, UErrorCode *status) {
  જો (status==nullptr || U_FAILURE(*status)) {
    પાછા 0xffffffff;
  }
  જો(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0xffffffff;
  }
  જો(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    પાછા 0xffffffff;
  }
  પાછા res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(અચલ UResourceBundle* resB, UErrorCode *status) {
  જો (status==nullptr || U_FAILURE(*status)) {
    પાછા 0xffffffff;
  }
  જો(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0xffffffff;
  }
  જો(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    પાછા 0xffffffff;
  }
  પાછા res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(અચલ UResourceBundle *resB) {
  જો(resB == nullptr) {
    પાછા URES_NONE;
  }
  પાછા res_getPublicType(resB->fRes);
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ures_get_ચાવી(અચલ UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  જો(resB == nullptr) {
    પાછા nullptr;
  }
  પાછા(resB->f_ચાવી);
}

U_CAPI int32_t U_EXPORT2 ures_getSize(અચલ UResourceBundle *resB) {
  જો(resB == nullptr) {
    પાછા 0;
  }
  
  પાછા resB->fSize;
}

સ્થિર અચલ char16_t*
ures_getStringWithAlias(અચલ UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t sIndex,
                        int32_t* len,
                        UErrorCode* status) {
  જો(RES_GET_TYPE(r) == URES_ALIAS) {
    અચલ char16_t* result = nullptr;
    UResourceBundle *tempRes = ures_getByIndex(resB, sIndex, nullptr, status);
    result = ures_getString(tempRes, len, status);
    ures_close(tempRes);
    પાછા result;
  } નહીંતર {
    પાછા res_getString({resB, sIndex}, &resB->getResData(), r, len); 
  }
}

U_CAPI શૂન્ય U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  જો(resB == nullptr) {
    પાછા;
  }
  resB->fIndex = -1;
}

U_CAPI UBool U_EXPORT2 ures_hasNext(અચલ UResourceBundle *resB) {
  જો(resB == nullptr) {
    પાછા ખોટું;
  }
  પાછા resB->fIndex < resB->fSize-1;
}

U_CAPI અચલ char16_t* U_EXPORT2
ures_getNextString(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   અચલ અક્ષર** ચાવી,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  જો (status==nullptr || U_FAILURE(*status)) {
    પાછા nullptr;
  }
  જો(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા nullptr;
  }
  
  જો(resB->fIndex == resB->fSize-1) {
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
  } નહીંતર {
    resB->fIndex++;
    પસંદગી(RES_GET_TYPE(resB->fRes)) {
    સ્થિતિ URES_STRING:
    સ્થિતિ URES_STRING_V2:
      પાછા res_getString({resB}, &resB->getResData(), resB->fRes, len);
    સ્થિતિ URES_TABLE:
    સ્થિતિ URES_TABLE16:
    સ્થિતિ URES_TABLE32:
      r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, ચાવી);
      જો(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      પાછા ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    સ્થિતિ URES_ARRAY:
    સ્થિતિ URES_ARRAY16:
      r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
      જો(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      પાછા ures_getStringWithAlias(resB, r, resB->fIndex, len, status);
    સ્થિતિ URES_ALIAS:
      પાછા ures_getStringWithAlias(resB, resB->fRes, resB->fIndex, len, status);
    સ્થિતિ URES_INT:
    સ્થિતિ URES_દ્વિઅંકી:
    સ્થિતિ URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    મૂળભૂત:
      પાછા nullptr;
    }
  }

  પાછા nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getNextResource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    અચલ અક્ષર *ચાવી = nullptr;
    Resource r = RES_BOGUS;

    જો (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            પાછા fillIn;
    }
    જો(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            પાછા fillIn;
    }

    જો(resB->fIndex == resB->fSize-1) {
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } નહીંતર {
        resB->fIndex++;
        પસંદગી(RES_GET_TYPE(resB->fRes)) {
        સ્થિતિ URES_INT:
        સ્થિતિ URES_દ્વિઅંકી:
        સ્થિતિ URES_STRING:
        સ્થિતિ URES_STRING_V2:
        સ્થિતિ URES_INT_VECTOR:
            પાછા ures_copyResb(fillIn, resB, status);
        સ્થિતિ URES_TABLE:
        સ્થિતિ URES_TABLE16:
        સ્થિતિ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, resB->fIndex, &ચાવી);
            જો(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            પાછા init_resb_result(resB->fData, r, ચાવી, resB->fIndex, resB, fillIn, status);
        સ્થિતિ URES_ARRAY:
        સ્થિતિ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, resB->fIndex);
            જો(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            પાછા init_resb_result(resB->fData, r, ચાવી, resB->fIndex, resB, fillIn, status);
        મૂળભૂત:
            /*return nullptr;*/
            પાછા fillIn;
        }
    }
    /*return nullptr;*/
    પાછા fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_getByIndex(અચલ UResourceBundle* resB,
                int32_t indexR,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    અચલ અક્ષર* ચાવી = nullptr;
    Resource r = RES_BOGUS;

    જો (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        પાછા fillIn;
    }
    જો(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        પાછા fillIn;
    }

    જો(indexR >= 0 && resB->fSize > indexR) {
        પસંદગી(RES_GET_TYPE(resB->fRes)) {
        સ્થિતિ URES_INT:
        સ્થિતિ URES_દ્વિઅંકી:
        સ્થિતિ URES_STRING:
        સ્થિતિ URES_STRING_V2:
        સ્થિતિ URES_INT_VECTOR:
            પાછા ures_copyResb(fillIn, resB, status);
        સ્થિતિ URES_TABLE:
        સ્થિતિ URES_TABLE16:
        સ્થિતિ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexR, &ચાવી);
            જો(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            પાછા init_resb_result(resB->fData, r, ચાવી, indexR, resB, fillIn, status);
        સ્થિતિ URES_ARRAY:
        સ્થિતિ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexR);
            જો(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            પાછા init_resb_result(resB->fData, r, ચાવી, indexR, resB, fillIn, status);
        મૂળભૂત:
            /*return nullptr;*/
            પાછા fillIn;
        }
    } નહીંતર {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    પાછા fillIn;
}

U_CAPI અચલ char16_t* U_EXPORT2
ures_getStringByIndex(અચલ UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t indexS,
                      int32_t* len,
                      UErrorCode* status) {
    અચલ અક્ષર* ચાવી = nullptr;
    Resource r = RES_BOGUS;

    જો (status==nullptr || U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    જો(indexS >= 0 && resB->fSize > indexS) {
        પસંદગી(RES_GET_TYPE(resB->fRes)) {
        સ્થિતિ URES_STRING:
        સ્થિતિ URES_STRING_V2:
            પાછા res_getString({resB}, &resB->getResData(), resB->fRes, len);
        સ્થિતિ URES_TABLE:
        સ્થિતિ URES_TABLE16:
        સ્થિતિ URES_TABLE32:
            r = res_getTableItemByIndex(&resB->getResData(), resB->fRes, indexS, &ચાવી);
            જો(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            પાછા ures_getStringWithAlias(resB, r, indexS, len, status);
        સ્થિતિ URES_ARRAY:
        સ્થિતિ URES_ARRAY16:
            r = res_getArrayItem(&resB->getResData(), resB->fRes, indexS);
            જો(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            પાછા ures_getStringWithAlias(resB, r, indexS, len, status);
        સ્થિતિ URES_ALIAS:
            પાછા ures_getStringWithAlias(resB, resB->fRes, indexS, len, status);
        સ્થિતિ URES_INT:
        સ્થિતિ URES_દ્વિઅંકી:
        સ્થિતિ URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            તોડો;
        મૂળભૂત:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          તોડો;
        }
    } નહીંતર {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    પાછા nullptr;
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ures_getUTF8StringByIndex(અચલ UResourceBundle* resB,
                          int32_t idx,
                          અક્ષર* dest U_LIFETIME_BOUND,
                          int32_t* pLength,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    અચલ char16_t *s16 = ures_getStringByIndex(resB, idx, &length16, status);
    પાછા ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(અચલ અક્ષર* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *result = fillIn;
  અક્ષર *packageName = nullptr;
  અક્ષર *pathToResource = nullptr, *save = nullptr;
  અક્ષર *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  જો(status == nullptr || U_FAILURE(*status)) {
    પાછા result;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (અક્ષર *)uprv_malloc(length*કદ(અક્ષર));
  /* test for nullptr */
  જો(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા result;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  જો(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    જો(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } નહીંતર {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  જો(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  જો(U_SUCCESS(*status)) {
    જો(localeEnd) {
      result = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } નહીંતર {
      result = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  પાછા result;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(અચલ UResourceBundle* resB,
                     અક્ષર* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *result = fillIn;
  અચલ અક્ષર *ચાવી;

  જો(status == nullptr || U_FAILURE(*status)) {
    પાછા result;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  કરો {
    res = res_findResource(&resB->getResData(), resB->fRes, &path, &ચાવી); 
    જો(res != RES_BOGUS) {
        result = init_resb_result(resB->fData, res, ચાવી, -1, resB, fillIn, status);
        resB = result;
    } નહીંતર {
        *status = U_MISSING_RESOURCE_ERROR;
        તોડો;
    }
  } જ્યારે(*path); /* there is more stuff in the path */

  પાછા result;
}

U_CAPI અચલ char16_t* U_EXPORT2
ures_get_String_By_ચાવી_With_Fallback(અચલ UResourceBundle* resB U_LIFETIME_BOUND,
                                અચલ અક્ષર* in_ચાવી,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle ઢગલો_2;
    અચલ char16_t* retVal = nullptr;
    ures_init_ઢગલો_Object(&ઢગલો_2);
    ures_get_By_ચાવી_With_Fallback(resB, in_ચાવી, &ઢગલો_2, status);
    int32_t length;
    retVal = ures_getString(&ઢગલો_2, &length, status);
    ures_close(&ઢગલો_2);
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    જો (len != nullptr) {
        *len = length;
    }
    પાછા retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
સ્થિર Resource get_Table_Item_By_ચાવી_Path(અચલ ResourceData *pResData, Resource table, અચલ અક્ષર *ચાવી) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(ચાવી, errorCode);
  જો (U_FAILURE(errorCode)) { પાછા RES_BOGUS; }
  અક્ષર *pathPart = path.data();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  જ્યારે (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    અક્ષર *nextPathPart = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    જો (nextPathPart != nullptr) {
      *nextPathPart = 0;  /* Terminating null for this part of path. */
      nextPathPart++;
    } નહીંતર {
      nextPathPart = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    અચલ અક્ષર *pathP = pathPart;
    resource = res_get_Table_Item_By_ચાવી(pResData, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = nextPathPart; 
  }
  જો (*pathPart) {
    પાછા RES_BOGUS;
  }
  પાછા resource;
}

સ્થિર શૂન્ય createPath(અચલ અક્ષર* origResPath,
                       int32_t     origResPathLen,
                       અચલ અક્ષર* resPath,
                       int32_t     resPathLen,
                       અચલ અક્ષર* in_ચાવી,
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
    અચલ અક્ષર* ચાવી = in_ચાવી;
    જો (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        જો (U_SUCCESS(*status)) {
            અચલ અક્ષર* resPathLimit = resPath + resPathLen;
            અચલ અક્ષર* origResPathLimit = origResPath + origResPathLen;
            અચલ અક્ષર* resPathPtr = resPath;
            અચલ અક્ષર* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            જ્યારે (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                જ્યારે (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                જો (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                જ્યારે (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                જો (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            જ્યારે (resPathPtr < resPathLimit && *ચાવી != '\0') {
                જ્યારે (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                જો (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                જ્યારે (*ચાવી != '\0' && *ચાવી != RES_PATH_SEPARATOR) {
                    ++ચાવી;
                }
                જો (*ચાવી == RES_PATH_SEPARATOR) {
                    ++ચાવી;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(ચાવી, *status);
    } નહીંતર {
        path.append(in_ચાવી, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ચાવી_With_Fallback(અચલ UResourceBundle* resB,
                          અચલ અક્ષર* in_ચાવી,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, મૂળ_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    જો (status==nullptr || U_FAILURE(*status)) {
        પાછા fillIn;
    }
    જો(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    જો(URES_IS_TABLE(type)) {
        અચલ અક્ષર* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_ચાવી_Path(&resB->getResData(), resB->fRes, in_ચાવી);
        અચલ અક્ષર* ચાવી = in_ચાવી;
        તાર્કિક did_મૂળ_Once = ખોટું;
        જો(res == RES_BOGUS) {
            UResourceDataEntry *dataEntry = resB->fData;
            CharString path;
            અક્ષર *myPath = nullptr;
            અચલ અક્ષર* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            જ્યારે(res == RES_BOGUS && (dataEntry->fParent != nullptr || !did_મૂળ_Once)) { /* Otherwise, we'll look in parents */
                જો (dataEntry->fParent != nullptr) {
                    dataEntry = dataEntry->fParent;
                } નહીંતર {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_મૂળ_Once = સાચું;
                }
                મૂળ_Res = dataEntry->fData.મૂળ_Res;

                જો(dataEntry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_ચાવી, path, status);
                    જો (U_FAILURE(*status)) {
                        ures_close(helper);
                        પાછા fillIn;
                    }
                    myPath = path.data();
                    ચાવી = in_ચાવી;
                    કરો {
                        res = res_findResource(&(dataEntry->fData), મૂળ_Res, &myPath, &ચાવી);
                        જો (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_result(dataEntry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            જો(helper) {
                              dataEntry = helper->fData;
                              મૂળ_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } નહીંતર {
                              તોડો;
                            }
                        } નહીંતર જો (res == RES_BOGUS) {
                            તોડો;
                        }
                    } જ્યારે(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            જો(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                જો(uprv_strcmp(dataEntry->fName, uloc_getDefault())==0 || uprv_strcmp(dataEntry->fName, k_મૂળ_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } નહીંતર {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_result(dataEntry, res, ચાવી, -1, resB, fillIn, status);
                જો (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_ચાવી, path, status);
                } નહીંતર {
                    અચલ અક્ષર* separator = nullptr;
                    જો (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    જો (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_ચાવી, path, status);
                    } નહીંતર {
                        createPath(origResPath, origResPathLen, "", 0, in_ચાવી, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.data(), path.length(), status);
                જો(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } નહીંતર {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } નહીંતર {
            fillIn = init_resb_result(resB->fData, res, ચાવી, -1, resB, fillIn, status);
        }
    } 
    નહીંતર {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    પાછા fillIn;
}

નામાકાશ {

શૂન્ય get_All_વસ્તુઓ_With_Fallback(
        અચલ UResourceBundle *bundle, Resource_Data_કિંમત &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા; }
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
    sink.put(bundle->f_ચાવી, value, !hasParent, errorCode);
    જો (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ઢગલો_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.fData = parentEntry;
        parentRef.fValidLocaleDataEntry = bundle->fValidLocaleDataEntry;
        parentRef.fHasFallback = !parentRef.getResData().noFallback;
        parentRef.f_Is_ટોચ_Level = સાચું;
        parentRef.fRes = parentRef.getResData().મૂળ_Res;
        parentRef.fSize = res_ગણતરી_Array_વસ્તુઓ(&parentRef.getResData(), parentRef.fRes);
        parentRef.fIndex = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        ઢગલો_UResource_Bundle containerBundle;
        અચલ UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        જો (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } નહીંતર {
            rb = ures_get_By_ચાવી_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        જો (U_SUCCESS(pathErrorCode)) {
            get_All_વસ્તુઓ_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

સંરચના GetAllChildrenSink : જાહેર ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    આભાસી ~GetAllChildrenSink() override;
    આભાસી શૂન્ય put(અચલ અક્ષર *ચાવી, Resource_કિંમત &value, UBool is_મૂળ,
           UErrorCode &errorCode) override {
        ResourceTable વસ્તુઓ_Table = value.getTable(errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
        માટે (int32_t i = 0; વસ્તુઓ_Table.get_ચાવી_And_કિંમત(i, ચાવી, value); ++i) {
            જો (value.getType() == URES_ALIAS) {
                Resource_Data_કિંમત& rdv = static_cast<Resource_Data_કિંમત&>(value);
                ઢગલો_UResource_Bundle ઢગલો_Temp_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.getData(), rdv.getResource(), nullptr, -1,
                                                                          rdv.getValidLocaleDataEntry(), nullptr, 0,
                                                                          ઢગલો_Temp_Bundle.getAlias(), &errorCode);
                જો (U_SUCCESS(errorCode)) {
                    Resource_Data_કિંમત aliased_કિંમત;
                    aliased_કિંમત.setData(aliasRB->getResData());
                    aliased_કિંમત.setValidLocaleDataEntry(aliasRB->fValidLocaleDataEntry);
                    aliased_કિંમત.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    જો (aliased_કિંમત.getType() != URES_TABLE) {
                        dest.put(ચાવી, aliased_કિંમત, is_મૂળ, errorCode);
                    } નહીંતર {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_કિંમત_Type = URES_TABLE;
                        CharString tablePath;
                        tablePath.append(aliasRB->fResPath, errorCode);
                        અચલ અક્ષર* parent_ચાવી = ચાવી; // dest.put() changes the key
                        dest.put(parent_ચાવી, aliased_કિંમત, is_મૂળ, errorCode);
                        UResourceDataEntry* entry = aliasRB->fData;
                        Resource res = aliasRB->fRes;
                        જ્યારે (aliased_કિંમત_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            અક્ષર* localPathAsCharPtr = localPath.data();
                            અચલ અક્ષર* child_ચાવી;
                            entry = entry->fParent;
                            res = entry->fData.મૂળ_Res;
                            Resource newRes = res_findResource(&entry->fData, res, &localPathAsCharPtr, &child_ચાવી);
                            જો (newRes != RES_BOGUS) {
                                aliased_કિંમત.setData(entry->fData);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_કિંમત.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_કિંમત_Type = aliased_કિંમત.getType();
                                જો (aliased_કિંમત_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_Data_કિંમત& rdv2 = static_cast<Resource_Data_કિંમત&>(aliased_કિંમત);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.getData(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.getValidLocaleDataEntry(), nullptr, 0,
                                                                             ઢગલો_Temp_Bundle.getAlias(), &errorCode);
                                    tablePath.clear();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->fData;
                                    res = aliasRB->fRes;
                                    aliased_કિંમત.setData(entry->fData);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_કિંમત.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_કિંમત_Type = aliased_કિંમત.getType();
                                }
                                જો (aliased_કિંમત_Type == URES_TABLE) {
                                    dest.put(parent_ચાવી, aliased_કિંમત, is_મૂળ, errorCode);
                                } નહીંતર {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    પાછા;
                                }
                            }
                        }
                    }
                }
            } નહીંતર {
                dest.put(ચાવી, value, is_મૂળ, errorCode);
            }
            જો (U_FAILURE(errorCode)) { પાછા; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI શૂન્ય U_EXPORT2
ures_getAllChildrenWithFallback(અચલ UResourceBundle *bundle, અચલ અક્ષર *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_વસ્તુઓ_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI શૂન્ય U_EXPORT2
ures_get_કિંમત_With_Fallback(અચલ UResourceBundle *bundle, અચલ અક્ષર *path,
                          UResourceBundle *tempFillIn,
                          Resource_Data_કિંમત &value, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા; }
    જો (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    અચલ UResourceBundle *rb;
    જો (*path == 0) {
        // empty path
        rb = bundle;
    } નહીંતર {
        rb = ures_get_By_ચાવી_With_Fallback(bundle, path, tempFillIn, &errorCode);
        જો (U_FAILURE(errorCode)) {
            પાછા;
        }
    }
    value.setData(rb->getResData());
    value.setValidLocaleDataEntry(rb->fValidLocaleDataEntry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI શૂન્ય U_EXPORT2
ures_get_All_વસ્તુઓ_With_Fallback(અચલ UResourceBundle *bundle, અચલ અક્ષર *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા; }
    જો (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    ઢગલો_UResource_Bundle ઢગલો_Bundle;
    અચલ UResourceBundle *rb;
    જો (*path == 0) {
        // empty path
        rb = bundle;
    } નહીંતર {
        rb = ures_get_By_ચાવી_With_Fallback(bundle, path, ઢગલો_Bundle.getAlias(), &errorCode);
        જો (U_FAILURE(errorCode)) {
            પાછા;
        }
    }
    // Get all table items with fallback.
    Resource_Data_કિંમત value;
    get_All_વસ્તુઓ_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ચાવી(અચલ UResourceBundle* resB,
              અચલ અક્ષર* in_ચાવી,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    અચલ અક્ષર *ચાવી = in_ચાવી;

    જો (status==nullptr || U_FAILURE(*status)) {
        પાછા fillIn;
    }
    જો(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    જો(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_ચાવી(&resB->getResData(), resB->fRes, &t, &ચાવી);
        જો(res == RES_BOGUS) {
            ચાવી = in_ચાવી;
            જો(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &ચાવી, &res, status);
                જો(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    પાછા init_resb_result(dataEntry, res, ચાવી, -1, resB, fillIn, status);
                } નહીંતર {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } નહીંતર {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } નહીંતર {
            પાછા init_resb_result(resB->fData, res, ચાવી, -1, resB, fillIn, status);
        }
    } 
#જો 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    નહીંતર જો(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == સાચું) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &ચાવી, &res, status);
        જો(U_SUCCESS(*status)) {
            પાછા init_resb_result(dataEntry, res, ચાવી, resB, fillIn, status);
        } નહીંતર {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#endif    
    નહીંતર {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા fillIn;
}

U_CAPI અચલ char16_t* U_EXPORT2
ures_get_String_By_ચાવી(અચલ UResourceBundle* resB U_LIFETIME_BOUND,
                    અચલ અક્ષર* in_ચાવી,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResourceDataEntry *dataEntry = nullptr;
    અચલ અક્ષર* ચાવી = in_ચાવી;

    જો (status==nullptr || U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    જો(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_ચાવી(&resB->getResData(), resB->fRes, &t, &ચાવી);

        જો(res == RES_BOGUS) {
            ચાવી = in_ચાવી;
            જો(resB->fHasFallback) {
                dataEntry = getFallbackData(resB, &ચાવી, &res, status);
                જો(U_SUCCESS(*status)) {
                    પસંદગી (RES_GET_TYPE(res)) {
                    સ્થિતિ URES_STRING:
                    સ્થિતિ URES_STRING_V2:
                        પાછા res_getString({resB, ચાવી}, &dataEntry->fData, res, len);
                    સ્થિતિ URES_ALIAS:
                      {
                        અચલ char16_t* result = nullptr;
                        UResourceBundle *tempRes = ures_get_By_ચાવી(resB, in_ચાવી, nullptr, status);
                        result = ures_getString(tempRes, len, status);
                        ures_close(tempRes);
                        પાછા result;
                      }
                    મૂળભૂત:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } નહીંતર {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } નહીંતર {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } નહીંતર {
            પસંદગી (RES_GET_TYPE(res)) {
            સ્થિતિ URES_STRING:
            સ્થિતિ URES_STRING_V2:
                પાછા res_getString({resB, ચાવી}, &resB->getResData(), res, len);
            સ્થિતિ URES_ALIAS:
              {
                અચલ char16_t* result = nullptr;
                UResourceBundle *tempRes = ures_get_By_ચાવી(resB, in_ચાવી, nullptr, status);
                result = ures_getString(tempRes, len, status);
                ures_close(tempRes);
                પાછા result;
              }
            મૂળભૂત:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#જો 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    નહીંતર જો(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == સાચું) {
        /* here should go a first attempt to locate the key using index table */
        dataEntry = getFallbackData(resB, &ચાવી, &res, status);
        જો(U_SUCCESS(*status)) {
            // TODO: Tracing
            પાછા res_getString(rd, res, len);
        } નહીંતર {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#endif    
    નહીંતર {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા nullptr;
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ures_get_UTF8_String_By_ચાવી(અચલ UResourceBundle* resB,
                        અચલ અક્ષર* ચાવી,
                        અક્ષર* dest U_LIFETIME_BOUND,
                        int32_t* pLength,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    અચલ char16_t *s16 = ures_get_String_By_ચાવી(resB, ચાવી, &length16, status);
    પાછા ures_toUTF8String(s16, length16, dest, pLength, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI અચલ અક્ષર* U_EXPORT2
ures_getLocaleInternal(અચલ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    જો (status==nullptr || U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    } નહીંતર {
      પાછા resourceBundle->fData->fName;
    }
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ures_getLocale(અચલ UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  પાછા ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ures_getLocaleByType(અચલ UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status) {
    જો (status==nullptr || U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    } નહીંતર {
        પસંદગી(type) {
        સ્થિતિ ULOC_ACTUAL_LOCALE:
            પાછા resourceBundle->fData->fName;
        સ્થિતિ ULOC_VALID_LOCALE:
            પાછા resourceBundle->fValidLocaleDataEntry->fName;
        સ્થિતિ ULOC_REQUESTED_LOCALE:
        મૂળભૂત:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા nullptr;
        }
    }
}

U_CFUNC અચલ અક્ષર*
ures_getName(અચલ UResourceBundle* resB U_LIFETIME_BOUND) {
  જો(resB == nullptr) {
    પાછા nullptr;
  }

  પાછા resB->fData->fName;
}

#ifdef URES_DEBUG
U_CFUNC અચલ અક્ષર*
ures_getPath(અચલ UResourceBundle* resB U_LIFETIME_BOUND) {
  જો(resB == nullptr) {
    પાછા nullptr;
  }

  પાછા resB->fData->fPath;
}
#endif

સ્થિર UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  અચલ અક્ષર* path,
                  અચલ અક્ષર* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    જો(U_FAILURE(*status)) {
        પાછા nullptr;
    }

    UResourceDataEntry *entry;
    જો(openType != URES_OPEN_DIRECT) {
        જો (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        જો(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા nullptr;
        }
        entry = entryOpen(path, canonLocaleID.data(), openType, status);
    } નહીંતર {
        entry = entryOpenDirect(path, localeID, status);
    }
    જો(U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        પાછા nullptr;
    }

    UBool is_ઢગલો_Object;
    જો(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(કદ(UResourceBundle)));
        જો(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            પાછા nullptr;
        }
        is_ઢગલો_Object = ખોટું;
    } નહીંતર {  // fill-in
        is_ઢગલો_Object = ures_is_ઢગલો_Object(r);
        ures_closeBundle(r, ખોટું);
    }
    uprv_memset(r, 0, કદ(UResourceBundle));
    ures_set_Is_ઢગલો_Object(r, is_ઢગલો_Object);

    r->fValidLocaleDataEntry = r->fData = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->getResData().noFallback;
    r->f_Is_ટોચ_Level = સાચું;
    r->fRes = r->getResData().મૂળ_Res;
    r->fSize = res_ગણતરી_Array_વસ્તુઓ(&r->getResData(), r->fRes);
    r->fIndex = -1;

    ResourceTracer(r).traceOpen();

    પાછા r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(અચલ અક્ષર* path, અચલ અક્ષર* localeID, UErrorCode* status) {
    પાછા ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_મૂળ, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(અચલ અક્ષર* path, અચલ અક્ષર* localeID, UErrorCode* status) {
    પાછા ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_મૂળ, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(અચલ અક્ષર* path, અચલ અક્ષર* localeID, UErrorCode* status) {
    પાછા ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI શૂન્ય U_EXPORT2
ures_openFillIn(UResourceBundle *r, અચલ અક્ષર* path,
                અચલ અક્ષર* localeID, UErrorCode* status) {
    જો(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_મૂળ, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI શૂન્ય U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, અચલ અક્ષર* path, અચલ અક્ષર* localeID, UErrorCode* status) {
    જો(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_ગણતરી_Array_વસ્તુઓ(અચલ UResourceBundle* resourceBundle,
                  અચલ અક્ષર* resource_ચાવી,
                  UErrorCode* status)
{
    UResourceBundle resData;
    ures_init_ઢગલો_Object(&resData);
    જો (status==nullptr || U_FAILURE(*status)) {
        પાછા 0;
    }
    જો(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    ures_get_By_ચાવી(resourceBundle, resource_ચાવી, &resData, status);
    
    જો(resData.getResData().data != nullptr) {
        int32_t result = res_ગણતરી_Array_વસ્તુઓ(&resData.getResData(), resData.fRes);
        ures_close(&resData);
        પાછા result;
    } નહીંતર {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&resData);
        પાછા 0;
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
U_CAPI અચલ અક્ષર* U_EXPORT2
ures_get_Version_સંખ્યા_Internal(અચલ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    જો (!resourceBundle) પાછા nullptr;

    જો(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        અચલ char16_t* minor_version = ures_get_String_By_ચાવી(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (અક્ષર *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        જો (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            પાછા nullptr;
        }
       
        જો(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        નહીંતર {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    પાછા resourceBundle->fVersion;
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ures_get_Version_સંખ્યા(અચલ UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    પાછા ures_get_Version_સંખ્યા_Internal(resourceBundle);
}

U_CAPI શૂન્ય U_EXPORT2 ures_getVersion(અચલ UResourceBundle* resB, UVersionInfo versionInfo) {
    જો (!resB) પાછા;

    u_versionFromString(versionInfo, ures_get_Version_સંખ્યા_Internal(resB));
}

/** Tree support functions *******************************/
#define INDEX_LOCALE_NAME "res_index"
#define INDEX_TAG         "InstalledLocales"
#define DEFAULT_TAG       "default"

#જો defined(URES_વૃક્ષ_DEBUG)
#અન્તર્ભાવ <stdio.h>
#endif

પ્રકારવ્યાખ્યા સંરચના ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

સ્થિર શૂન્ય U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

સ્થિર int32_t U_CALLCONV
ures_loc_ગણતરી_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    પાછા ures_getSize(&ctx->installed);
}

U_CDECL_BEGIN


સ્થિર અચલ અક્ષર * U_CALLCONV
ures_loc_nextLocale(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    અચલ અક્ષર *result = nullptr;
    int32_t len = 0;
    જો (ures_hasNext(res) && (k = ures_getNextResource(res, &ctx->curr, status)) != nullptr) {
        result = ures_get_ચાવી(k);
        len = (int32_t)uprv_strlen(result);
    }
    જો (resultLength) {
        *resultLength = len;
    }
    પાછા result;
}

સ્થિર શૂન્ય U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

સ્થિર અચલ UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_ગણતરી_Locales,
        uenum_unextDefault,
        ures_loc_nextLocale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(અચલ અક્ષર *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    જો(U_FAILURE(*status)) {
        પાછા nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(કદ(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(કદ(UEnumeration));
    જો(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        પાછા nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, કદ(UEnumeration));

    ures_init_ઢગલો_Object(&myContext->installed);
    ures_init_ઢગલો_Object(&myContext->curr);
    idx = ures_openDirect(path, INDEX_LOCALE_NAME, status);
    ures_get_By_ચાવી(idx, INDEX_TAG, &myContext->installed, status);
    જો(U_SUCCESS(*status)) {
#જો defined(URES_વૃક્ષ_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            path, INDEX_LOCALE_NAME, INDEX_TAG, ures_get_ચાવી(&myContext->installed));
#endif
        en->context = myContext;
    } નહીંતર {
#જો defined(URES_વૃક્ષ_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", path, u_errorName(*status));
#endif
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    પાછા en;
}

સ્થિર UBool isLocaleInList(UEnumeration *locEnum, અચલ અક્ષર *loc_To_શોધ, UErrorCode *status) {
    અચલ અક્ષર *loc;
    જ્યારે ((loc = uenum_next(locEnum, nullptr, status)) != nullptr) {
        જો (uprv_strcmp(loc, loc_To_શોધ) == 0) {
            પાછા સાચું;
        }
    }
    પાછા ખોટું;
}

સ્થિર શૂન્ય getParentForFunctionalEquivalent(અચલ અક્ષર*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.clear();
    જો (res != nullptr) {
        ures_get_By_ચાવી(res, "%%Parent", bund1, &subStatus);
        જો (U_SUCCESS(subStatus)) {
            int32_t length16;
            અચલ char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    જો (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(અક્ષર *result, int32_t resultCapacity,
                             અચલ અક્ષર *path, અચલ અક્ષર *resName, અચલ અક્ષર *keyword, અચલ અક્ષર *locid,
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
    જો(U_FAILURE(*status)) પાછા 0;
    CharString kwVal;
    જો (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_કિંમત(locid, keyword, subStatus);
        જો (kwVal == DEFAULT_TAG) {
            kwVal.clear();
        }
    }
    જો (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#જો defined(URES_વૃક્ષ_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.data(), base.data(), u_errorName(subStatus));
#endif
    ures_init_ઢગલો_Object(&bund1);
    ures_init_ઢગલો_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    જો(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = સાચું;
        જો (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.data(), &subStatus);
        }
        uenum_close(locEnum);
    }

    જો(U_FAILURE(subStatus)) {
        *status = subStatus;
        પાછા 0;
    }
    
    કરો {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.data(), &subStatus);
        જો(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = ખોટું;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#જો defined(URES_વૃક્ષ_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.data(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#endif
        જો(U_FAILURE(subStatus)) {
            *status = subStatus;
        } નહીંતર જો(subStatus == U_ZERO_ERROR) {
            ures_get_By_ચાવી(res,resName,&bund1, &subStatus);
            જો(subStatus == U_ZERO_ERROR) {
                અચલ char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#જો defined(URES_વૃક્ષ_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.data(), u_errorName(subStatus));
#endif
                defUstr = ures_get_String_By_ચાવી(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                જો(U_SUCCESS(subStatus) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#જો defined(URES_વૃક્ષ_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                    defLoc.copyFrom(parent, subStatus);
                    જો(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#જો defined(URES_વૃક્ષ_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.data(), keyword, kwVal.data());
#endif
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        જો (res != nullptr) {
            found.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        જો (found != parent) {
            parent.copyFrom(found, subStatus);
        } નહીંતર {
            getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        }
        ures_close(res);
    } જ્યારે(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    કરો {
        res = ures_open(path, parent.data(), &subStatus);
        જો((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = ખોટું;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#જો defined(URES_વૃક્ષ_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
        જો(U_FAILURE(subStatus)) {
            *status = subStatus;
        } નહીંતર જો(subStatus == U_ZERO_ERROR) {
            ures_get_By_ચાવી(res,resName,&bund1, &subStatus);
#જો defined(URES_વૃક્ષ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#endif
            જો(subStatus == U_ZERO_ERROR) {
                ures_get_By_ચાવી(&bund1, kwVal.data(), &bund2, &subStatus);
#જો defined(URES_વૃક્ષ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.data(), u_errorName(subStatus));
#endif
                જો(subStatus == U_ZERO_ERROR) {
#જો defined(URES_વૃક્ષ_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                    જો (parent.isEmpty()) {
                        full.clear().append("root", subStatus);
                    } નહીંતર {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        જો(defLoc.length() > full.length()) {
                          અચલ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#જો defined(URES_વૃક્ષ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_ચાવી(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          જો(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#જો defined(URES_વૃક્ષ_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#જો defined(URES_વૃક્ષ_DEBUG)
                        નહીંતર {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.data(), full.data());
                        }
#endif
                } નહીંતર {
#જો defined(URES_વૃક્ષ_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.data(), kwVal.data());
#endif
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;
        UBool haveFound = ખોટું;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        જો (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            અચલ અક્ષર *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            જો (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                જો (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.clear().append(validLoc, subStatus);
                    haveFound = સાચું;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        જો (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.data(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } જ્યારે(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    જો(full.isEmpty() && kwVal != defVal) {
#જો defined(URES_વૃક્ષ_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.data(), defVal.data());
#endif
        kwVal.clear().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        કરો { /* search for 'default' named item */
            res = ures_open(path, parent.data(), &subStatus);
            જો((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = ખોટું;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#જો defined(URES_વૃક્ષ_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.data(), u_errorName(subStatus), kwVal.data());
#endif
            જો(U_FAILURE(subStatus)) {
                *status = subStatus;
            } નહીંતર જો(subStatus == U_ZERO_ERROR) {
                ures_get_By_ચાવી(res,resName,&bund1, &subStatus);
                જો(subStatus == U_ZERO_ERROR) {
                    ures_get_By_ચાવી(&bund1, kwVal.data(), &bund2, &subStatus);
                    જો(subStatus == U_ZERO_ERROR) {
#જો defined(URES_વૃક્ષ_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.data(), keyword, kwVal.data(), u_errorName(subStatus));
#endif
                        જો (parent.isEmpty()) {
                            full.clear().append("root", subStatus);
                        } નહીંતર {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        જો(defLoc.length() > full.length()) {
                          અચલ char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#જો defined(URES_વૃક્ષ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.data());
#endif
                          defUstr = ures_get_String_By_ચાવી(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          જો(U_SUCCESS(subStatus) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, subStatus);
#જો defined(URES_વૃક્ષ_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.data(), keyword, defVal.data(), u_errorName(subStatus));
#endif
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#જો defined(URES_વૃક્ષ_DEBUG)
                        નહીંતર {
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
        } જ્યારે(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    જો(U_SUCCESS(*status)) {
        જો(full.isEmpty()) {
#જો defined(URES_વૃક્ષ_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.data());
#endif
          *status = U_MISSING_RESOURCE_ERROR;
        } નહીંતર જો(omitDefault) {
#જો defined(URES_વૃક્ષ_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", full.data(), defLoc.data(), found.data());
#endif        
          જો(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            જો(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#જો defined(URES_વૃક્ષ_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.data());
#endif
              kwVal.clear();
            }
          }
        }
        found.copyFrom(full, subStatus);
        જો(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } નહીંતર જો(!omitDefault) {
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

    જો(U_SUCCESS(*status)) {
        int32_t copyLength = uprv_min(length, resultCapacity);
        જો(copyLength>0) {
            found.extract(result, copyLength, subStatus);
        }
        જો(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } નહીંતર {
        length = 0;
        result[0]=0;
    }
    પાછા u_terminateChars(result, resultCapacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_કિંમતો(અચલ અક્ષર *path, અચલ અક્ષર *keyword, UErrorCode *status)
{
#define કિંમતો_BUF_SIZE 2048
#define કિંમતો_LIST_SIZE 512
    
    અક્ષર       કિંમતો_Buf[કિંમતો_BUF_SIZE];
    int32_t    કિંમતો_Index = 0;
    અચલ અક્ષર *કિંમતો_List[કિંમતો_LIST_SIZE];
    int32_t    કિંમતો_ગણતરી = 0;
    
    અચલ અક્ષર *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_ઢગલો_Object(&item);
    ures_init_ઢગલો_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    જો(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        પાછા nullptr;
    }
    
    કિંમતો_Buf[0]=0;
    કિંમતો_Buf[1]=0;

    જ્યારે ((locale = uenum_next(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#જો defined(URES_વૃક્ષ_DEBUG)
        જો(!bund || U_FAILURE(subStatus)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#endif
        
        ures_get_By_ચાવી(bund, keyword, &item, &subStatus);
        
        જો(!bund || U_FAILURE(subStatus)) {
#જો defined(URES_વૃક્ષ_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#endif
            ures_close(bund);
            bund = nullptr;
            ચાલુ;
        }

        જ્યારે ((subPtr = ures_getNextResource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            અચલ અક્ષર *k;
            int32_t i;
            k = ures_get_ચાવી(subPtr);

#જો defined(URES_વૃક્ષ_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#endif
            જો(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                ચાલુ;
            }
            માટે(i=0; i<કિંમતો_ગણતરી; i++) {
                જો(!uprv_strcmp(કિંમતો_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    તોડો;
                }
            }
            જો(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                જો((કિંમતો_ગણતરી >= (કિંમતો_LIST_SIZE-1)) ||       /* no more space in list .. */
                    ((કિંમતો_Index+kLen+1+1) >= કિંમતો_BUF_SIZE)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } નહીંતર {
                    uprv_strcpy(કિંમતો_Buf+કિંમતો_Index, k);
                    કિંમતો_List[કિંમતો_ગણતરી++] = કિંમતો_Buf+કિંમતો_Index;
                    કિંમતો_Index += kLen;
#જો defined(URES_વૃક્ષ_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#endif
                    કિંમતો_Buf[કિંમતો_Index++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    કિંમતો_Buf[કિંમતો_Index++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#જો defined(URES_વૃક્ષ_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*status), 
        કિંમતો_Index, કિંમતો_ગણતરી);
#endif
    પાછા uloc_openKeywordList(કિંમતો_Buf, કિંમતો_Index, status);
}
#જો 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(અચલ UResourceBundle* res1, અચલ UResourceBundle* res2){
    જો(res1==nullptr || res2==nullptr){
        પાછા res1==res2; /* pointer comparison */
    }
    જો(res1->f_ચાવી==nullptr||  res2->f_ચાવી==nullptr){
        પાછા (res1->f_ચાવી==res2->f_ચાવી);
    }નહીંતર{
        જો(uprv_strcmp(res1->f_ચાવી, res2->f_ચાવી)!=0){
            પાછા ખોટું;
        }
    }
    જો(uprv_strcmp(res1->fData->fName, res2->fData->fName)!=0){
        પાછા ખોટું;
    }
    જો(res1->fData->fPath == nullptr||  res2->fData->fPath==nullptr){
        પાછા (res1->fData->fPath == res2->fData->fPath);
    }નહીંતર{
        જો(uprv_strcmp(res1->fData->fPath, res2->fData->fPath)!=0){
            પાછા ખોટું;
        }
    }
    જો(uprv_strcmp(res1->fData->fParent->fName, res2->fData->fParent->fName)!=0){
        પાછા ખોટું;
    }
    જો(uprv_strcmp(res1->fData->fParent->fPath, res2->fData->fParent->fPath)!=0){
        પાછા ખોટું;
    }
    જો(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        પાછા ખોટું;
    }
    જો(res1->fRes != res2->fRes){
        પાછા ખોટું;
    }
    પાછા સાચું;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(અચલ UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    જો(U_FAILURE(*status) || res == nullptr){
        પાછા nullptr;
    }
    bundle = ures_open(res->fData->fPath, res->fData->fName, status);
    જો(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }નહીંતર{
        ret = bundle;
    }
    પાછા ret;
}
U_CAPI અચલ UResourceBundle* U_EXPORT2
ures_getParentBundle(અચલ UResourceBundle* res){
    જો(res==nullptr){
        પાછા nullptr;
    }
    પાછા res->fParentRes;
}
#endif

U_CAPI શૂન્ય U_EXPORT2
ures_get_Version_By_ચાવી(અચલ UResourceBundle* res, અચલ અક્ષર *ચાવી, UVersionInfo ver, UErrorCode *status) {
  અચલ char16_t *str;
  int32_t len;
  str = ures_get_String_By_ચાવી(res, ચાવી, &len, status);
  જો(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
