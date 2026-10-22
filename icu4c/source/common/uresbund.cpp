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

#समावेश "unicode/ures.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/ucnv.h"
#समावेश "bytesinkutil.h"
#समावेश "charstr.h"
#समावेश "uresimp.h"
#समावेश "ustr_imp.h"
#समावेश "cwchar.h"
#समावेश "ucln_cmn.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "mutex.h"
#समावेश "uhash.h"
#समावेश "unicode/uenum.h"
#समावेश "uenumimp.h"
#समावेश "ulocimp.h"
#समावेश "umutex.h"
#समावेश "putilimp.h"
#समावेश "uassert.h"
#समावेश "uresdata.h"

using namespace icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
अटल UHashtable *cache = nullptr;
अटल icu::UInitOnce gCacheInitOnce {};

अटल UMutex resbMutex;

/* INTERNAL: hashes an entry  */
अटल int32_t U_CALLCONV hashEntry(स्थिर UHashTok parm) {
    UResource_तथ्य_Entry* b = static_cast<UResource_तथ्य_Entry*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->fPath;
    वापस uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
अटल UBool U_CALLCONV compareEntries(स्थिर UHashTok p1, स्थिर UHashTok p2) {
    UResource_तथ्य_Entry* b1 = static_cast<UResource_तथ्य_Entry*>(p1.pointer);
    UResource_तथ्य_Entry* b2 = static_cast<UResource_तथ्य_Entry*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->fPath;
    path2.pointer = b2->fPath;
    वापस uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
अटल UBool chopLocale(अक्षर *name) {
    अक्षर *i = uprv_strrchr(name, '_');

    अगर(i != nullptr) {
        *i = '\0';
        वापस true;
    }

    वापस false;
}

अटल UBool hasVariant(स्थिर अक्षर* localeID) {
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
    वापस sink.संख्या_Of_Bytes_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#घोषणा INCLUDED_FROM_URESBUND_CPP
#समावेश "localefallback_data.h"

अटल स्थिर अक्षर* performFallbackLookup(स्थिर अक्षर* कुंजी,
                                         स्थिर अक्षर* कुंजी_Strs,
                                         स्थिर अक्षर* मूल्य_Strs U_LIFETIME_BOUND,
                                         स्थिर int32_t* lookupTable,
                                         int32_t lookup_Table_लंबाई) {
    स्थिर int32_t* bottom = lookupTable;
    स्थिर int32_t* शीर्ष_2 = lookupTable + lookup_Table_लंबाई;

    जबतक (bottom < शीर्ष_2) {
        // Effectively, divide by 2 and round down to an even index
        स्थिर int32_t* मध्य = bottom + (((शीर्ष_2 - bottom) / 4) * 2);
        स्थिर अक्षर* entry_कुंजी = &(कुंजी_Strs[*मध्य]);
        int32_t strcmp_परिणाम = uprv_strcmp(कुंजी, entry_कुंजी);
        अगर (strcmp_परिणाम == 0) {
            वापस &(मूल्य_Strs[मध्य[1]]);
        } अन्यथा अगर (strcmp_परिणाम < 0) {
            शीर्ष_2 = मध्य;
        } अन्यथा {
            bottom = मध्य + 2;
        }
    }
    वापस nullptr;
}

अटल CharString getDefaultScript(स्थिर CharString& language, स्थिर CharString& region) {
    स्थिर अक्षर* defaultScript = nullptr;
    UErrorCode err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString परिणाम("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    अगर (!region.isEmpty()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        अगर (U_FAILURE(err)) {
            वापस परिणाम;
        }
        defaultScript = performFallbackLookup(localeID.तथ्य(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    अगर (defaultScript == nullptr) {
        defaultScript = performFallbackLookup(language.तथ्य(), dsLocaleIDChars, scriptCodeChars, defaultScriptTable, UPRV_LENGTHOF(defaultScriptTable));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    अगर (defaultScript != nullptr) {
        परिणाम.सफा();
        परिणाम.append(defaultScript, err);
    }
    वापस परिणाम;
}

क्रमागत UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_मूल,
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
    URES_OPEN_LOCALE_मूल,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_DIRECT
};
प्रकार क्रमागत UResOpenType UResOpenType;

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
अटल bool getParentLocaleID(अक्षर *name, स्थिर अक्षर *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    माप_प्रकार nameLen = uprv_strlen(name);
    अगर (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        वापस chopLocale(name);
    }
    
    UErrorCode err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, nullptr, nullptr, err);

    अगर (U_FAILURE(err)) {
        // hopefully this never happens...
        वापस chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    अगर (openType == URES_OPEN_LOCALE_DEFAULT_मूल) {
        स्थिर अक्षर* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parentLocaleTable, UPRV_LENGTHOF(parentLocaleTable));
        अगर (parentID != nullptr) {
            uprv_strcpy(name, parentID);
            वापस true;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    अगर (!script.isEmpty() && !region.isEmpty()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        अगर (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } अन्यथा {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } अन्यथा अगर (!region.isEmpty()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UErrorCode err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, nullptr, nullptr, nullptr, err);
        अगर (!origNameScript.isEmpty()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } अन्यथा {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } अन्यथा अगर (!script.isEmpty()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        अगर (openType != URES_OPEN_LOCALE_DEFAULT_मूल || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } अन्यथा {
            वापस false;
        }
    } अन्यथा {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        वापस false;
    }
    अगर (U_SUCCESS(err) && !workingLocale.isEmpty()) {
        uprv_strcpy(name, workingLocale.तथ्य());
        वापस true;
    } अन्यथा {
        वापस false;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
अटल UBool mayHaveParent(अक्षर *name) {
    वापस (name[0] != 0 && uprv_strstr("nb nn",name) != nullptr);
}

/**
 *  Internal function
 */
अटल व्योम entryIncrease(UResource_तथ्य_Entry *entry) {
    Mutex lock(&resbMutex);
    entry->f_गिनती_Existing++;
    जबतक(entry->fParent != nullptr) {
      entry = entry->fParent;
      entry->f_गिनती_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
अटल UResource_तथ्य_Entry *get_Fallback_तथ्य(
        स्थिर UResourceBundle *resBundle,
        स्थिर अक्षर **resTag, Resource *res, UErrorCode *status) {
    UResource_तथ्य_Entry *तथ्य_Entry = resBundle->f_तथ्य;
    int32_t सूचकांक_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    अगर(तथ्य_Entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        वापस nullptr;
    }
    अगर(तथ्य_Entry->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_Table_Item_By_कुंजी(&(तथ्य_Entry->f_तथ्य), तथ्य_Entry->f_तथ्य.मूल_Res, &सूचकांक_R, resTag); /* try to get data from there */
        i++;
    }
    अगर(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        जबतक(*res == RES_BOGUS && तथ्य_Entry->fParent != nullptr) {
            तथ्य_Entry = तथ्य_Entry->fParent;
            अगर(तथ्य_Entry->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_Table_Item_By_कुंजी(&(तथ्य_Entry->f_तथ्य), तथ्य_Entry->f_तथ्य.मूल_Res, &सूचकांक_R, resTag);
            }
        }
    }

    अगर(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *status = U_MISSING_RESOURCE_ERROR;
        वापस nullptr;
    }
    // If the resource is found in parents, we need to adjust the error.
    अगर(i>1) {
        अगर(uprv_strcmp(तथ्य_Entry->fName, uloc_getDefault())==0 || uprv_strcmp(तथ्य_Entry->fName, k_मूल_Locale_Name)==0) {
            *status = U_USING_DEFAULT_WARNING;
        } अन्यथा {
            *status = U_USING_FALLBACK_WARNING;
        }
    }
    वापस तथ्य_Entry;
}

अटल व्योम
free_entry(UResource_तथ्य_Entry *entry) {
    UResource_तथ्य_Entry *alias;
    res_unload(&(entry->f_तथ्य));
    अगर(entry->fName != nullptr && entry->fName != entry->f_Name_बफर) {
        uprv_free(entry->fName);
    }
    अगर(entry->fPath != nullptr) {
        uprv_free(entry->fPath);
    }
    अगर(entry->fPool != nullptr) {
        --entry->fPool->f_गिनती_Existing;
    }
    alias = entry->fAlias;
    अगर(alias != nullptr) {
        जबतक(alias->fAlias != nullptr) {
            alias = alias->fAlias;
        }
        --alias->f_गिनती_Existing;
    }
    uprv_free(entry);
}

/* Works just like ucnv_flushCache() */
अटल int32_t ures_flushCache()
{
    UResource_तथ्य_Entry *resB;
    int32_t pos;
    int32_t rb_Deleted_अंक = 0;
    स्थिर UHashElement *e;
    UBool deletedMore;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex lock(&resbMutex);
    अगर (cache == nullptr) {
        वापस 0;
    }

    करो {
        deletedMore = false;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        जबतक ((e = uhash_अग्र_Element(cache, &pos)) != nullptr)
        {
            resB = static_cast<UResource_तथ्य_Entry*>(e->value.pointer);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            अगर (resB->f_गिनती_Existing == 0) {
                rb_Deleted_अंक++;
                deletedMore = true;
                uhash_removeElement(cache, e);
                free_entry(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } जबतक(deletedMore);

    वापस rb_Deleted_अंक;
}

#अगर_घोषित URES_DEBUG
#समावेश <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cacheNotEmpty = false;
  int32_t pos = UHASH_FIRST;
  स्थिर UHashElement *e;
  UResource_तथ्य_Entry *resB;
  
    Mutex lock(&resbMutex);
    अगर (cache == nullptr) {
      ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache is nullptr.\n", __खाता__, __LINE__);
      वापस false;
    }

    जबतक ((e = uhash_अग्र_Element(cache, &pos)) != nullptr) {
      cacheNotEmpty=true;
      resB = (UResource_तथ्य_Entry *) e->value.pointer;
      ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __खाता__, __LINE__,
              (व्योम*)resB, resB->f_गिनती_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->fPath?resB->fPath:"nullptr",
              (व्योम*)resB->fPool,
              (व्योम*)resB->fAlias,
              (व्योम*)resB->fParent);       
    }
    
    ख_लिखो(मानक_त्रुटि,"%s:%d: RB Cache still contains %d items.\n", __खाता__, __LINE__, uhash_गिनती(cache));
    वापस cacheNotEmpty;
}

#पूर्ण_अगर

अटल UBool U_CALLCONV ures_cleanup()
{
    अगर (cache != nullptr) {
        ures_flushCache();
        uhash_close(cache);
        cache = nullptr;
    }
    gCacheInitOnce.reset();
    वापस true;
}

/** INTERNAL: Initializes the cache for resources */
अटल व्योम U_CALLCONV createCache(UErrorCode &status) {
    U_ASSERT(cache == nullptr);
    cache = uhash_open(hashEntry, compareEntries, nullptr, &status);
    ucln_common_registerCleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
अटल व्योम initCache(UErrorCode *status) {
    umtx_initOnce(gCacheInitOnce, &createCache, *status);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

अटल व्योम setEntryName(UResource_तथ्य_Entry *res, स्थिर अक्षर *name, UErrorCode *status) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    अगर(res->fName != nullptr && res->fName != res->f_Name_बफर) {
        uprv_free(res->fName);
    }
    अगर (len < static_cast<int32_t>(माप(res->f_Name_बफर))) {
        res->fName = res->f_Name_बफर;
    }
    अन्यथा {
        res->fName = static_cast<अक्षर*>(uprv_malloc(len + 1));
    }
    अगर(res->fName == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        uprv_strcpy(res->fName, name);
    }
}

अटल UResource_तथ्य_Entry *
getPoolEntry(स्थिर अक्षर *path, UErrorCode *status);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
अटल UResource_तथ्य_Entry *init_entry(स्थिर अक्षर *localeID, स्थिर अक्षर *path, UErrorCode *status) {
    UResource_तथ्य_Entry *r = nullptr;
    UResource_तथ्य_Entry find;
    /*int32_t hashValue;*/
    स्थिर अक्षर *name;
    अक्षर aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }

    /* here we try to deduce the right locale name */
    अगर(localeID == nullptr) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } अन्यथा अगर(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_मूल_Locale_Name;
    } अन्यथा { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<अक्षर*>(name);
    find.fPath = const_cast<अक्षर*>(path);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_तथ्य_Entry*>(uhash_get(cache, &find));
    अगर(r == nullptr) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_तथ्य_Entry*>(uprv_malloc(माप(UResource_तथ्य_Entry)));
        अगर(r == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }

        uprv_memset(r, 0, माप(UResource_तथ्य_Entry));
        /*r->fHashKey = hashValue;*/

        setEntryName(r, name, status);
        अगर (U_FAILURE(*status)) {
            uprv_free(r);
            वापस nullptr;
        }

        अगर(path != nullptr) {
            r->fPath = uprv_strdup(path);
            अगर(r->fPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(r);
                वापस nullptr;
            }
        }

        /* this is the actual loading */
        res_load(&(r->f_तथ्य), r->fPath, r->fName, status);

        अगर (U_FAILURE(*status)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            अगर (*status == U_MEMORY_ALLOCATION_ERROR) {
                uprv_free(r);
                वापस nullptr;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *status = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } अन्यथा { /* if we have a regular entry */
            Resource aliasres;
            अगर (r->f_तथ्य.usesPoolBundle) {
                r->fPool = getPoolEntry(r->fPath, status);
                अगर (U_SUCCESS(*status)) {
                    स्थिर int32_t *poolIndexes = r->fPool->f_तथ्य.p_मूल + 1;
                    अगर(r->f_तथ्य.p_मूल[1 + URES_सूचकांक_POOL_CHECKSUM] == poolIndexes[URES_सूचकांक_POOL_CHECKSUM]) {
                        r->f_तथ्य.pool_Bundle_कुंजियाँ = reinterpret_cast<स्थिर अक्षर*>(poolIndexes + (poolIndexes[URES_सूचकांक_लंबाई] & 0xff));
                        r->f_तथ्य.poolBundleStrings = r->fPool->f_तथ्य.p16BitUnits;
                    } अन्यथा {
                        r->fBogus = *status = U_INVALID_FORMAT_ERROR;
                    }
                } अन्यथा {
                    r->fBogus = *status;
                }
            }
            अगर (U_SUCCESS(*status)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_तथ्य), "%%ALIAS");
                अगर (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    स्थिर char16_t *alias = res_getStringNoTrace(&(r->f_तथ्य), aliasres, &aliasLen);
                    अगर(alias != nullptr && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UCharsToChars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_entry(aliasName, path, status);
                    }
                }
            }
        }

        {
            UResource_तथ्य_Entry *oldR = nullptr;
            अगर ((oldR = static_cast<UResource_तथ्य_Entry*>(uhash_get(cache, r))) == nullptr) { /* if the data is not cached */
                /* just insert it in the cache */
                UErrorCode cacheStatus = U_ZERO_ERROR;
                uhash_put(cache, (व्योम *)r, r, &cacheStatus);
                अगर (U_FAILURE(cacheStatus)) {
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
    अगर(r != nullptr) {
        /* return the real bundle */
        जबतक(r->fAlias != nullptr) {
            r = r->fAlias;
        }
        r->f_गिनती_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        अगर(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*status)) {
             *status = r->fBogus; /* set the returning status */
        }
    }
    वापस r;
}

अटल UResource_तथ्य_Entry *
getPoolEntry(स्थिर अक्षर *path, UErrorCode *status) {
    UResource_तथ्य_Entry *poolBundle = init_entry(kPoolBundleName, path, status);
    अगर( U_SUCCESS(*status) &&
        (poolBundle == nullptr || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_तथ्य.isPoolBundle)
    ) {
        *status = U_INVALID_FORMAT_ERROR;
    }
    वापस poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
अटल UResource_तथ्य_Entry *
findFirstExisting(स्थिर अक्षर* path, अक्षर* name, स्थिर अक्षर* defaultLocale, UResOpenType openType,
                  UBool *is_मूल, UBool *foundParent, UBool *isDefault, UErrorCode* status) {
    UResource_तथ्य_Entry *r = nullptr;
    UBool has_Real_तथ्य = false;
    *foundParent = true; /* we're starting with a fresh name */
    अक्षर origName[ULOC_FULLNAME_CAPACITY];

    uprv_strcpy(origName, name);
    जबतक(*foundParent && !has_Real_तथ्य) {
        r = init_entry(name, path, status);
        /* Null pointer test */
        अगर (U_FAILURE(*status)) {
            वापस nullptr;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_तथ्य = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        अगर(!has_Real_तथ्य) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_गिनती_Existing--;
            /*entryCloseInt(r);*/
            r = nullptr;
            *status = U_USING_FALLBACK_WARNING;
        } अन्यथा {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_मूल = static_cast<UBool>(uprv_strcmp(name, k_मूल_Locale_Name) == 0);

        /*Fallback data stuff*/
        अगर (!has_Real_तथ्य) {
            *foundParent = getParentLocaleID(name, origName, openType);
        } अन्यथा {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *foundParent = chopLocale(name);
        }
        अगर (*foundParent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    वापस r;
}

अटल व्योम ures_set_Is_ढेर_Object( UResourceBundle* resB, UBool state) {
    अगर(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } अन्यथा {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

अटल UBool ures_is_ढेर_Object(स्थिर UResourceBundle* resB) {
  वापस((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?false:true);
}


U_CFUNC व्योम ures_init_ढेर_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, माप(UResourceBundle));
  ures_set_Is_ढेर_Object(resB, true);
}

U_NAMESPACE_BEGIN

ढेर_UResource_Bundle::ढेर_UResource_Bundle() {
    ures_init_ढेर_Object(&bundle);
}

ढेर_UResource_Bundle::~ढेर_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_END

अटल UBool  // returns U_SUCCESS(*status)
load_Parents_Except_मूल(UResource_तथ्य_Entry *&t1,
                      अक्षर name[], int32_t nameCapacity,
                      UBool usingUSRData, अक्षर usr_तथ्य_Path[], UErrorCode *status) {
    अगर (U_FAILURE(*status)) { वापस false; }
    UBool checkParent = true;
    जबतक (checkParent && t1->fParent == nullptr && !t1->f_तथ्य.noFallback &&
            res_getResource(&t1->f_तथ्य,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->f_तथ्य, "%%Parent");
        अगर (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            स्थिर char16_t *parentLocaleName = res_getStringNoTrace(&(t1->f_तथ्य), parentRes, &parentLocaleLen);
            अगर(parentLocaleName != nullptr && 0 < parentLocaleLen && parentLocaleLen < nameCapacity) {
                u_UCharsToChars(parentLocaleName, name, parentLocaleLen + 1);
                अगर (uprv_strcmp(name, k_मूल_Locale_Name) == 0) {
                    वापस true;
                }
            }
        }
        // Insert regular parents.
        UErrorCode parentStatus = U_ZERO_ERROR;
        UResource_तथ्य_Entry *t2 = init_entry(name, t1->fPath, &parentStatus);
        अगर (U_FAILURE(parentStatus)) {
            *status = parentStatus;
            वापस false;
        }
        UResource_तथ्य_Entry *u2 = nullptr;
        UErrorCode usrStatus = U_ZERO_ERROR;
        अगर (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_entry(name, usr_तथ्य_Path, &usrStatus);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            अगर (usrStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = usrStatus;
                वापस false;
            }
        }

        अगर (usingUSRData && U_SUCCESS(usrStatus) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } अन्यथा {
            t1->fParent = t2;
            अगर (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_गिनती_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    वापस true;
}

अटल UBool  // returns U_SUCCESS(*status)
जोड़ो_मूल_Bundle(UResource_तथ्य_Entry *&t1, UErrorCode *status) {
    अगर (U_FAILURE(*status)) { वापस false; }
    UErrorCode parentStatus = U_ZERO_ERROR;
    UResource_तथ्य_Entry *t2 = init_entry(k_मूल_Locale_Name, t1->fPath, &parentStatus);
    अगर (U_FAILURE(parentStatus)) {
        *status = parentStatus;
        वापस false;
    }
    t1->fParent = t2;
    t1 = t2;
    वापस true;
}

अटल UResource_तथ्य_Entry *entryOpen(स्थिर अक्षर* path, स्थिर अक्षर* localeID,
                                     UResOpenType openType, UErrorCode* status) {
    U_ASSERT(openType != URES_OPEN_DIRECT);
    UErrorCode intStatus = U_ZERO_ERROR;
    UResource_तथ्य_Entry *r = nullptr;
    UResource_तथ्य_Entry *t1 = nullptr;
    UBool isDefault = false;
    UBool is_मूल = false;
    UBool has_Real_तथ्य = false;
    UBool hasChopped = true;
    UBool usingUSRData = U_USE_USRDATA && ( path == nullptr || uprv_strncmp(path,U_ICUDATA_NAME,8) == 0);

    अक्षर name[ULOC_FULLNAME_CAPACITY];
    अक्षर usr_तथ्य_Path[96];

    initCache(status);

    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }

    uprv_strncpy(name, localeID, माप(name) - 1);
    name[माप(name) - 1] = 0;

    अगर ( usingUSRData ) {
        अगर ( path == nullptr ) {
            uprv_strcpy(usr_तथ्य_Path, U_USRDATA_NAME);
        } अन्यथा {
            uprv_strncpy(usr_तथ्य_Path, path, माप(usr_तथ्य_Path) - 1);
            usr_तथ्य_Path[0] = 'u';
            usr_तथ्य_Path[1] = 's';
            usr_तथ्य_Path[2] = 'r';
            usr_तथ्य_Path[माप(usr_तथ्य_Path) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    स्थिर अक्षर *defaultLocale = uloc_getDefault();

    Mutex lock(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(path, name, defaultLocale, openType, &is_मूल, &hasChopped, &isDefault, &intStatus);

    // If we failed due to out-of-memory, report the failure and exit early.
    अगर (intStatus == U_MEMORY_ALLOCATION_ERROR) {
        *status = intStatus;
        जाओ finish;
    }

    अगर(r != nullptr) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_तथ्य = true;
        अगर ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UErrorCode usrStatus = U_ZERO_ERROR;
            UResource_तथ्य_Entry *u1 = init_entry(t1->fName, usr_तथ्य_Path, &usrStatus);
            // If we failed due to out-of-memory, report the failure and exit early.
            अगर (intStatus == U_MEMORY_ALLOCATION_ERROR) {
                *status = intStatus;
                जाओ finish;
            }
            अगर ( u1 != nullptr ) {
                अगर(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } अन्यथा {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_गिनती_Existing = 0;
                }
            }
        }
        अगर ((hasChopped || mayHaveParent(name)) && !is_मूल) {
            अगर (!load_Parents_Except_मूल(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_तथ्य_Path, status)) {
                जाओ finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    अगर(r==nullptr && openType == URES_OPEN_LOCALE_DEFAULT_मूल && !isDefault && !is_मूल) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_मूल, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        अगर (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            जाओ finish;
        }
        intStatus = U_USING_DEFAULT_WARNING;
        अगर(r != nullptr) { /* the default locale exists */
            t1 = r;
            has_Real_तथ्य = true;
            isDefault = true;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            अगर ((hasChopped || mayHaveParent(name)) && !is_मूल) {
                अगर (!load_Parents_Except_मूल(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_तथ्य_Path, status)) {
                    जाओ finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    अगर(r == nullptr) {
        uprv_strcpy(name, k_मूल_Locale_Name);
        r = findFirstExisting(path, name, defaultLocale, openType, &is_मूल, &hasChopped, &isDefault, &intStatus);
        // If we failed due to out-of-memory, report the failure and exit early.
        अगर (intStatus == U_MEMORY_ALLOCATION_ERROR) {
            *status = intStatus;
            जाओ finish;
        }
        अगर(r != nullptr) {
            t1 = r;
            intStatus = U_USING_DEFAULT_WARNING;
            has_Real_तथ्य = true;
        } अन्यथा { /* we don't even have the root locale */
            *status = U_MISSING_RESOURCE_ERROR;
            जाओ finish;
        }
    } अन्यथा अगर(!is_मूल && uprv_strcmp(t1->fName, k_मूल_Locale_Name) != 0 &&
            t1->fParent == nullptr && !r->f_तथ्य.noFallback) {
        अगर (!जोड़ो_मूल_Bundle(t1, status)) {
            जाओ finish;
        }
        अगर(!has_Real_तथ्य) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    जबतक(r != nullptr && !is_मूल && t1->fParent != nullptr) {
        t1->fParent->f_गिनती_Existing++;
        t1 = t1->fParent;
    }

finish:
    अगर(U_SUCCESS(*status)) {
        अगर(intStatus != U_ZERO_ERROR) {
            *status = intStatus;  
        }
        वापस r;
    } अन्यथा {
        वापस nullptr;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
अटल UResource_तथ्य_Entry *
entryOpenDirect(स्थिर अक्षर* path, स्थिर अक्षर* localeID, UErrorCode* status) {
    initCache(status);
    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    } अन्यथा अगर (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_मूल_Locale_Name;
    }

    Mutex lock(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_तथ्य_Entry *r = init_entry(localeID, path, status);
    अगर(U_SUCCESS(*status)) {
        अगर(r->fBogus != U_ZERO_ERROR) {
            r->f_गिनती_Existing--;
            r = nullptr;
        }
    } अन्यथा {
        r = nullptr;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_तथ्य_Entry *t1 = r;
    अगर(r != nullptr && uprv_strcmp(localeID, k_मूल_Locale_Name) != 0 &&  // not root
            r->fParent == nullptr && !r->f_तथ्य.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_CAPACITY) {
        अक्षर name[ULOC_FULLNAME_CAPACITY];
        uprv_strcpy(name, localeID);
        अगर(!chopLocale(name) || uprv_strcmp(name, k_मूल_Locale_Name) == 0 ||
                load_Parents_Except_मूल(t1, name, UPRV_LENGTHOF(name), false, nullptr, status)) {
            अगर(uprv_strcmp(t1->fName, k_मूल_Locale_Name) != 0 && t1->fParent == nullptr) {
                जोड़ो_मूल_Bundle(t1, status);
            }
        }
        अगर(U_FAILURE(*status)) {
            r = nullptr;
        }
    }

    अगर(r != nullptr) {
        // TODO: Does this ever loop?
        जबतक(t1->fParent != nullptr) {
            t1->fParent->f_गिनती_Existing++;
            t1 = t1->fParent;
        }
    }
    वापस r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
अटल व्योम entryCloseInt(UResource_तथ्य_Entry *resB) {
    UResource_तथ्य_Entry *p = resB;

    जबतक(resB != nullptr) {
        p = resB->fParent;
        resB->f_गिनती_Existing--;

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

अटल व्योम entryClose(UResource_तथ्य_Entry *resB) {
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
अटल व्योम ures_appendResPath(UResourceBundle *resB, स्थिर अक्षर* toAdd, int32_t lenToAdd, UErrorCode *status) {
    int32_t resPathLenOrig = resB->fResPathLen;
    अगर(resB->fResPath == nullptr) {
        resB->fResPath = resB->fResBuf;
        *(resB->fResPath) = 0;
        resB->fResPathLen = 0;
    } 
    resB->fResPathLen += lenToAdd;
    अगर(RES_BUFSIZE <= resB->fResPathLen+1) {
        अगर(resB->fResPath == resB->fResBuf) {
            resB->fResPath = static_cast<अक्षर*>(uprv_malloc((resB->fResPathLen + 1) * माप(अक्षर)));
            /* Check that memory was allocated correctly. */
            अगर (resB->fResPath == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            uprv_strcpy(resB->fResPath, resB->fResBuf);
        } अन्यथा {
            अक्षर* अस्थायी = static_cast<अक्षर*>(uprv_realloc(resB->fResPath, (resB->fResPathLen + 1) * माप(अक्षर)));
            /* Check that memory was reallocated correctly. */
            अगर (अस्थायी == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            resB->fResPath = अस्थायी;
        }
    }
    uprv_strcpy(resB->fResPath + resPathLenOrig, toAdd);
}

अटल व्योम ures_freeResPath(UResourceBundle *resB) {
    अगर (resB->fResPath && resB->fResPath != resB->fResBuf) {
        uprv_free(resB->fResPath);
    }
    resB->fResPath = nullptr;
    resB->fResPathLen = 0;
}

अटल व्योम
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    अगर(resB != nullptr) {
        अगर(resB->f_तथ्य != nullptr) {
            entryClose(resB->f_तथ्य);
        }
        अगर(resB->fVersion != nullptr) {
            uprv_free(resB->fVersion);
        }
        ures_freeResPath(resB);

        अगर(ures_is_ढेर_Object(resB) == false && freeBundleObj) {
            uprv_free(resB);
        }
#अगर 0 /*U_DEBUG*/
        अन्यथा {
            /* poison the data */
            uprv_memset(resB, -1, माप(UResourceBundle));
        }
#पूर्ण_अगर
    }
}

U_CAPI व्योम  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, true);
}

namespace {

UResourceBundle* init_resb_परिणाम(UResource_तथ्य_Entry* तथ्य_Entry,
                                  Resource r,
                                  स्थिर अक्षर* कुंजी,
                                  int32_t idx,
                                  UResource_तथ्य_Entry* valid_Locale_तथ्य_Entry,
                                  स्थिर अक्षर* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(स्थिर Resource_तथ्य& res_तथ्य,
                               Resource r,
                               स्थिर अक्षर* कुंजी,
                               int32_t idx,
                               UResource_तथ्य_Entry* valid_Locale_तथ्य_Entry,
                               स्थिर अक्षर* containerResPath,
                               int32_t recursionDepth,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    अगर (U_FAILURE(*status)) { वापस resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    स्थिर char16_t *alias = res_getAlias(&res_तथ्य, r, &len);
    अगर(len <= 0) {
        // bad alias
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस resB;
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
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }

    // We have an alias, now let's cut it up.
    स्थिर अक्षर *path = nullptr, *locale = nullptr, *कुंजी_Path = nullptr;
    अगर(chAlias[0] == RES_PATH_SEPARATOR) {
        // There is a path included.
        अक्षर *ch_Alias_तथ्य = chAlias.तथ्य();
        अक्षर *sep = ch_Alias_तथ्य + 1;
        path = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        अगर(sep != nullptr) {
            *sep++ = 0;
        }
        अगर(uprv_strcmp(path, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            कुंजी_Path = sep;
            // Read from the valid locale which we already have.
            path = locale = nullptr;
        } अन्यथा {
            अगर(uprv_strcmp(path, "ICUDATA") == 0) { /* want ICU data */
                path = nullptr;
            }
            अगर (sep == nullptr) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } अन्यथा {
                locale = sep;
                sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
                अगर(sep != nullptr) {
                    *sep++ = 0;
                }
                कुंजी_Path = sep;
            }
        }
    } अन्यथा {
        // No path, start with a locale.
        अक्षर *sep = chAlias.तथ्य();
        locale = sep;
        sep = uprv_strchr(sep, RES_PATH_SEPARATOR);
        अगर(sep != nullptr) {
            *sep++ = 0;
        }
        कुंजी_Path = sep;
        path = valid_Locale_तथ्य_Entry->fPath;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResource_तथ्य_Entry *तथ्य_Entry;
    अगर (locale == nullptr) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        तथ्य_Entry = valid_Locale_तथ्य_Entry;
    } अन्यथा {
        UErrorCode intStatus = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_openDirect(path, locale, &intStatus));
        अगर(U_FAILURE(intStatus)) {
            // We failed to open the resource bundle we're aliasing to.
            *status = intStatus;
            वापस resB;
        }
        तथ्य_Entry = mainRes->f_तथ्य;
    }

    स्थिर अक्षर* अस्थायी = nullptr;
    अगर(कुंजी_Path == nullptr) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = तथ्य_Entry->f_तथ्य.मूल_Res;
        अगर(containerResPath) {
            chAlias.सफा().append(containerResPath, *status);
            अगर (U_FAILURE(*status)) {
                वापस nullptr;
            }
            अक्षर *a_कुंजी = chAlias.तथ्य();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&तथ्य_Entry->f_तथ्य, r, &a_कुंजी, &अस्थायी);
        }
        अगर(कुंजी) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.सफा().append(कुंजी, *status);
            अगर (U_FAILURE(*status)) {
                वापस nullptr;
            }
            अक्षर *a_कुंजी = chAlias.तथ्य();
            r = res_findResource(&तथ्य_Entry->f_तथ्य, r, &a_कुंजी, &अस्थायी);
        } अन्यथा अगर(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            अगर(URES_IS_TABLE(type)) {
                स्थिर अक्षर *a_कुंजी;
                r = res_get_Table_Item_By_सूचकांक(&तथ्य_Entry->f_तथ्य, r, idx, &a_कुंजी);
            } अन्यथा { /* array */
                r = res_getArrayItem(&तथ्य_Entry->f_तथ्य, r, idx);
            }
        }
        अगर(r != RES_BOGUS) {
            resB = init_resb_परिणाम(
                तथ्य_Entry, r, अस्थायी, -1, valid_Locale_तथ्य_Entry, nullptr, recursionDepth+1,
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
        CharString pathBuf(कुंजी_Path, *status);
        अगर (U_FAILURE(*status)) {
            वापस nullptr;
        }
        अक्षर *myPath = pathBuf.तथ्य();
        containerResPath = nullptr;
        // Now we have fallback following here.
        क्रम(;;) {
            r = तथ्य_Entry->f_तथ्य.मूल_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            जबतक(*myPath && U_SUCCESS(*status)) {
                r = res_findResource(&(तथ्य_Entry->f_तथ्य), r, &myPath, &अस्थायी);
                अगर(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    अवरोध;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_परिणाम(
                    तथ्य_Entry, r, अस्थायी, -1,
                    valid_Locale_तथ्य_Entry, containerResPath, recursionDepth+1,
                    resB, status);
                अगर (U_FAILURE(*status)) {
                    अवरोध;
                }
                अगर (अस्थायी == nullptr || uprv_strcmp(कुंजी_Path, अस्थायी) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_freeResPath(resB);
                    ures_appendResPath(resB, कुंजी_Path, static_cast<int32_t>(uprv_strlen(कुंजी_Path)), status);
                    अगर(resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
                        ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
                    }
                    अगर (U_FAILURE(*status)) {
                        अवरोध;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                तथ्य_Entry = resB->f_तथ्य;
                containerResPath = resB->fResPath;
            }
            अगर (U_FAILURE(*status) || r != RES_BOGUS) {
                अवरोध;
            }
            // Fall back to the parent bundle, if there is one.
            तथ्य_Entry = तथ्य_Entry->fParent;
            अगर (तथ्य_Entry == nullptr) {
                *status = U_MISSING_RESOURCE_ERROR;
                अवरोध;
            }
            // Copy the same keyPath again.
            myPath = pathBuf.तथ्य();
            uprv_strcpy(myPath, कुंजी_Path);
        }
    }
    अगर(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    वापस resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_परिणाम(UResource_तथ्य_Entry* तथ्य_Entry,
                                  Resource r,
                                  स्थिर अक्षर* कुंजी,
                                  int32_t idx,
                                  UResource_तथ्य_Entry* valid_Locale_तथ्य_Entry,
                                  स्थिर अक्षर* containerResPath,
                                  int32_t recursionDepth,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    अगर(status == nullptr || U_FAILURE(*status)) {
        वापस resB;
    }
    अगर (valid_Locale_तथ्य_Entry == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    अगर(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        अगर(recursionDepth >= URES_MAX_ALIAS_LEVEL) {
            *status = U_TOO_MANY_ALIASES_ERROR;
            वापस resB;
        }
        वापस getAliasTargetAsResourceBundle(
            तथ्य_Entry->f_तथ्य, r, कुंजी, idx,
            valid_Locale_तथ्य_Entry, containerResPath, recursionDepth, resB, status);
    }
    अगर(resB == nullptr) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(माप(UResourceBundle)));
        अगर (resB == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }
        ures_set_Is_ढेर_Object(resB, false);
        resB->fResPath = nullptr;
        resB->fResPathLen = 0;
    } अन्यथा {
        अगर(resB->f_तथ्य != nullptr) {
            entryClose(resB->f_तथ्य);
        }
        अगर(resB->fVersion != nullptr) {
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
        अगर(containerResPath != resB->fResPath) {
            ures_freeResPath(resB);
        }
    }
    resB->f_तथ्य = तथ्य_Entry;
    entryIncrease(resB->f_तथ्य);
    resB->fHasFallback = false;
    resB->f_Is_शीर्ष_Level = false;
    resB->f_सूचकांक = -1;
    resB->f_कुंजी = कुंजी; 
    resB->f_Valid_Locale_तथ्य_Entry = valid_Locale_तथ्य_Entry;
    अगर(containerResPath != resB->fResPath) {
        ures_appendResPath(
            resB, containerResPath, static_cast<int32_t>(uprv_strlen(containerResPath)), status);
    }
    अगर(कुंजी != nullptr) {
        ures_appendResPath(resB, कुंजी, static_cast<int32_t>(uprv_strlen(कुंजी)), status);
        अगर(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    } अन्यथा अगर(idx >= 0) {
        अक्षर buf[256];
        int32_t len = T_CString_integerToString(buf, idx, 10);
        ures_appendResPath(resB, buf, len, status);
        अगर(resB->fResPathLen > 0 && resB->fResPath[resB->fResPathLen-1] != RES_PATH_SEPARATOR) {
            ures_appendResPath(resB, RES_PATH_SEPARATOR_S, 1, status);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t usedLen = ((resB->fResBuf == resB->fResPath) ? resB->fResPathLen : 0);
        uprv_memset(resB->fResBuf + usedLen, 0, माप(resB->fResBuf) - usedLen);
    }

    resB->fVersion = nullptr;
    resB->fRes = r;
    resB->f_आकार = res_गिनती_Array_वस्तुएँ(&resB->get_Res_तथ्य(), resB->fRes);
    ResourceTracer(resB).trace("get");
    वापस resB;
}

UResourceBundle* init_resb_परिणाम(UResource_तथ्य_Entry* तथ्य_Entry,
                                  Resource r,
                                  स्थिर अक्षर* कुंजी,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  स्थिर UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UErrorCode* status) {
    वापस init_resb_परिणाम(
        तथ्य_Entry, r, कुंजी, idx,
        container->f_Valid_Locale_तथ्य_Entry, container->fResPath, 0, resB, status);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               स्थिर UResourceBundle* original,
                               UErrorCode* status) {
    UBool is_ढेर_Object;
    अगर(U_FAILURE(*status) || r == original) {
        वापस r;
    }
    अगर(original != nullptr) {
        अगर(r == nullptr) {
            is_ढेर_Object = false;
            r = static_cast<UResourceBundle*>(uprv_malloc(माप(UResourceBundle)));
            /* test for nullptr */
            अगर (r == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                वापस nullptr;
            }
        } अन्यथा {
            is_ढेर_Object = ures_is_ढेर_Object(r);
            ures_closeBundle(r, false);
        }
        uprv_memcpy(r, original, माप(UResourceBundle));
        r->fResPath = nullptr;
        r->fResPathLen = 0;
        अगर(original->fResPath) {
            ures_appendResPath(r, original->fResPath, original->fResPathLen, status);
        }
        ures_set_Is_ढेर_Object(r, is_ढेर_Object);
        अगर(r->f_तथ्य != nullptr) {
            entryIncrease(r->f_तथ्य);
        }
    }
    वापस r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI स्थिर char16_t* U_EXPORT2 ures_getString(स्थिर UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UErrorCode* status) {
    स्थिर char16_t *s;
    अगर (status==nullptr || U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    s = res_getString({resB}, &resB->get_Res_तथ्य(), resB->fRes, len);
    अगर (s == nullptr) {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस s;
}

अटल स्थिर अक्षर*
ures_toUTF8String(स्थिर char16_t* s16,
                  int32_t length16,
                  अक्षर* dest U_LIFETIME_BOUND,
                  int32_t* p_लंबाई,
                  UBool forceCopy,
                  UErrorCode* status) {
    int32_t capacity;

    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर (p_लंबाई != nullptr) {
        capacity = *p_लंबाई;
    } अन्यथा {
        capacity = 0;
    }
    अगर (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    अगर (length16 == 0) {
        /* empty string, return as read-only pointer */
        अगर (p_लंबाई != nullptr) {
            *p_लंबाई = 0;
        }
        अगर (forceCopy) {
            u_terminateChars(dest, capacity, 0, status);
            वापस dest;
        } अन्यथा {
            वापस "";
        }
    } अन्यथा {
        /* We need to transform the string to the destination buffer. */
        अगर (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            वापस u_strToUTF8(nullptr, 0, p_लंबाई, s16, length16, status);
        }
        अगर (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            int32_t max_लंबाई_2 = 3 * length16 + 1;
            अगर (capacity > max_लंबाई_2) {
                dest += capacity - max_लंबाई_2;
                capacity = max_लंबाई_2;
            }
        }
        वापस u_strToUTF8(dest, capacity, p_लंबाई, s16, length16, status);
    }
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ures_getUTF8String(स्थिर UResourceBundle* resB,
                   अक्षर* dest U_LIFETIME_BOUND,
                   int32_t* p_लंबाई,
                   UBool forceCopy,
                   UErrorCode* status) {
    int32_t length16;
    स्थिर char16_t *s16 = ures_getString(resB, &length16, status);
    वापस ures_toUTF8String(s16, length16, dest, p_लंबाई, forceCopy, status);
}

U_CAPI स्थिर uint8_t* U_EXPORT2
ures_get_द्विआधारी(स्थिर UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  स्थिर uint8_t *p;
  अगर (status==nullptr || U_FAILURE(*status)) {
    वापस nullptr;
  }
  अगर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस nullptr;
  }
  p = res_get_द्विआधारी({resB}, &resB->get_Res_तथ्य(), resB->fRes, len);
  अगर (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  वापस p;
}

U_CAPI स्थिर int32_t* U_EXPORT2
ures_getIntVector(स्थिर UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UErrorCode* status) {
  स्थिर int32_t *p;
  अगर (status==nullptr || U_FAILURE(*status)) {
    वापस nullptr;
  }
  अगर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस nullptr;
  }
  p = res_getIntVector({resB}, &resB->get_Res_तथ्य(), resB->fRes, len);
  अगर (p == nullptr) {
    *status = U_RESOURCE_TYPE_MISMATCH;
  }
  वापस p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(स्थिर UResourceBundle* resB, UErrorCode *status) {
  अगर (status==nullptr || U_FAILURE(*status)) {
    वापस 0xffffffff;
  }
  अगर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0xffffffff;
  }
  अगर(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    वापस 0xffffffff;
  }
  वापस res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(स्थिर UResourceBundle* resB, UErrorCode *status) {
  अगर (status==nullptr || U_FAILURE(*status)) {
    वापस 0xffffffff;
  }
  अगर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0xffffffff;
  }
  अगर(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *status = U_RESOURCE_TYPE_MISMATCH;
    वापस 0xffffffff;
  }
  वापस res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(स्थिर UResourceBundle *resB) {
  अगर(resB == nullptr) {
    वापस URES_NONE;
  }
  वापस res_getPublicType(resB->fRes);
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ures_get_कुंजी(स्थिर UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  अगर(resB == nullptr) {
    वापस nullptr;
  }
  वापस(resB->f_कुंजी);
}

U_CAPI int32_t U_EXPORT2 ures_get_आकार(स्थिर UResourceBundle *resB) {
  अगर(resB == nullptr) {
    वापस 0;
  }
  
  वापस resB->f_आकार;
}

अटल स्थिर char16_t*
ures_getStringWithAlias(स्थिर UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_सूचकांक,
                        int32_t* len,
                        UErrorCode* status) {
  अगर(RES_GET_TYPE(r) == URES_ALIAS) {
    स्थिर char16_t* परिणाम = nullptr;
    UResourceBundle *अस्थायी_Res = ures_get_By_सूचकांक(resB, s_सूचकांक, nullptr, status);
    परिणाम = ures_getString(अस्थायी_Res, len, status);
    ures_close(अस्थायी_Res);
    वापस परिणाम;
  } अन्यथा {
    वापस res_getString({resB, s_सूचकांक}, &resB->get_Res_तथ्य(), r, len); 
  }
}

U_CAPI व्योम U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  अगर(resB == nullptr) {
    वापस;
  }
  resB->f_सूचकांक = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_अग्र(स्थिर UResourceBundle *resB) {
  अगर(resB == nullptr) {
    वापस false;
  }
  वापस resB->f_सूचकांक < resB->f_आकार-1;
}

U_CAPI स्थिर char16_t* U_EXPORT2
ures_get_अग्र_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   स्थिर अक्षर** कुंजी,
                   UErrorCode* status) {
  Resource r = RES_BOGUS;
  
  अगर (status==nullptr || U_FAILURE(*status)) {
    वापस nullptr;
  }
  अगर(resB == nullptr) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस nullptr;
  }
  
  अगर(resB->f_सूचकांक == resB->f_आकार-1) {
    *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
  } अन्यथा {
    resB->f_सूचकांक++;
    चयन(RES_GET_TYPE(resB->fRes)) {
    हाल URES_STRING:
    हाल URES_STRING_V2:
      वापस res_getString({resB}, &resB->get_Res_तथ्य(), resB->fRes, len);
    हाल URES_TABLE:
    हाल URES_TABLE16:
    हाल URES_TABLE32:
      r = res_get_Table_Item_By_सूचकांक(&resB->get_Res_तथ्य(), resB->fRes, resB->f_सूचकांक, कुंजी);
      अगर(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      वापस ures_getStringWithAlias(resB, r, resB->f_सूचकांक, len, status);
    हाल URES_ARRAY:
    हाल URES_ARRAY16:
      r = res_getArrayItem(&resB->get_Res_तथ्य(), resB->fRes, resB->f_सूचकांक);
      अगर(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      वापस ures_getStringWithAlias(resB, r, resB->f_सूचकांक, len, status);
    हाल URES_ALIAS:
      वापस ures_getStringWithAlias(resB, resB->fRes, resB->f_सूचकांक, len, status);
    हाल URES_INT:
    हाल URES_द्विआधारी:
    हाल URES_INT_VECTOR:
        *status = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    शेष:
      वापस nullptr;
    }
  }

  वापस nullptr;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_अग्र_Resource(UResourceBundle* resB,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status) {
    स्थिर अक्षर *कुंजी = nullptr;
    Resource r = RES_BOGUS;

    अगर (status==nullptr || U_FAILURE(*status)) {
            /*return nullptr;*/
            वापस fillIn;
    }
    अगर(resB == nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            /*return nullptr;*/
            वापस fillIn;
    }

    अगर(resB->f_सूचकांक == resB->f_आकार-1) {
      *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } अन्यथा {
        resB->f_सूचकांक++;
        चयन(RES_GET_TYPE(resB->fRes)) {
        हाल URES_INT:
        हाल URES_द्विआधारी:
        हाल URES_STRING:
        हाल URES_STRING_V2:
        हाल URES_INT_VECTOR:
            वापस ures_copyResb(fillIn, resB, status);
        हाल URES_TABLE:
        हाल URES_TABLE16:
        हाल URES_TABLE32:
            r = res_get_Table_Item_By_सूचकांक(&resB->get_Res_तथ्य(), resB->fRes, resB->f_सूचकांक, &कुंजी);
            अगर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            वापस init_resb_परिणाम(resB->f_तथ्य, r, कुंजी, resB->f_सूचकांक, resB, fillIn, status);
        हाल URES_ARRAY:
        हाल URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_तथ्य(), resB->fRes, resB->f_सूचकांक);
            अगर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            वापस init_resb_परिणाम(resB->f_तथ्य, r, कुंजी, resB->f_सूचकांक, resB, fillIn, status);
        शेष:
            /*return nullptr;*/
            वापस fillIn;
        }
    }
    /*return nullptr;*/
    वापस fillIn;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_सूचकांक(स्थिर UResourceBundle* resB,
                int32_t सूचकांक_R,
                UResourceBundle* fillIn U_LIFETIME_BOUND,
                UErrorCode* status) {
    स्थिर अक्षर* कुंजी = nullptr;
    Resource r = RES_BOGUS;

    अगर (status==nullptr || U_FAILURE(*status)) {
        /*return nullptr;*/
        वापस fillIn;
    }
    अगर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        /*return nullptr;*/
        वापस fillIn;
    }

    अगर(सूचकांक_R >= 0 && resB->f_आकार > सूचकांक_R) {
        चयन(RES_GET_TYPE(resB->fRes)) {
        हाल URES_INT:
        हाल URES_द्विआधारी:
        हाल URES_STRING:
        हाल URES_STRING_V2:
        हाल URES_INT_VECTOR:
            वापस ures_copyResb(fillIn, resB, status);
        हाल URES_TABLE:
        हाल URES_TABLE16:
        हाल URES_TABLE32:
            r = res_get_Table_Item_By_सूचकांक(&resB->get_Res_तथ्य(), resB->fRes, सूचकांक_R, &कुंजी);
            अगर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            वापस init_resb_परिणाम(resB->f_तथ्य, r, कुंजी, सूचकांक_R, resB, fillIn, status);
        हाल URES_ARRAY:
        हाल URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_तथ्य(), resB->fRes, सूचकांक_R);
            अगर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            वापस init_resb_परिणाम(resB->f_तथ्य, r, कुंजी, सूचकांक_R, resB, fillIn, status);
        शेष:
            /*return nullptr;*/
            वापस fillIn;
        }
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    वापस fillIn;
}

U_CAPI स्थिर char16_t* U_EXPORT2
ures_get_String_By_सूचकांक(स्थिर UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t सूचकांक_S,
                      int32_t* len,
                      UErrorCode* status) {
    स्थिर अक्षर* कुंजी = nullptr;
    Resource r = RES_BOGUS;

    अगर (status==nullptr || U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    अगर(सूचकांक_S >= 0 && resB->f_आकार > सूचकांक_S) {
        चयन(RES_GET_TYPE(resB->fRes)) {
        हाल URES_STRING:
        हाल URES_STRING_V2:
            वापस res_getString({resB}, &resB->get_Res_तथ्य(), resB->fRes, len);
        हाल URES_TABLE:
        हाल URES_TABLE16:
        हाल URES_TABLE32:
            r = res_get_Table_Item_By_सूचकांक(&resB->get_Res_तथ्य(), resB->fRes, सूचकांक_S, &कुंजी);
            अगर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            वापस ures_getStringWithAlias(resB, r, सूचकांक_S, len, status);
        हाल URES_ARRAY:
        हाल URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_तथ्य(), resB->fRes, सूचकांक_S);
            अगर(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            वापस ures_getStringWithAlias(resB, r, सूचकांक_S, len, status);
        हाल URES_ALIAS:
            वापस ures_getStringWithAlias(resB, resB->fRes, सूचकांक_S, len, status);
        हाल URES_INT:
        हाल URES_द्विआधारी:
        हाल URES_INT_VECTOR:
            *status = U_RESOURCE_TYPE_MISMATCH;
            अवरोध;
        शेष:
          /* must not occur */
          *status = U_INTERNAL_PROGRAM_ERROR;
          अवरोध;
        }
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
    }
    वापस nullptr;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ures_get_UTF8_String_By_सूचकांक(स्थिर UResourceBundle* resB,
                          int32_t idx,
                          अक्षर* dest U_LIFETIME_BOUND,
                          int32_t* p_लंबाई,
                          UBool forceCopy,
                          UErrorCode* status) {
    int32_t length16;
    स्थिर char16_t *s16 = ures_get_String_By_सूचकांक(resB, idx, &length16, status);
    वापस ures_toUTF8String(s16, length16, dest, p_लंबाई, forceCopy, status);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(स्थिर अक्षर* path, UResourceBundle* fillIn U_LIFETIME_BOUND, UErrorCode* status)
{
  UResourceBundle *first = nullptr; 
  UResourceBundle *परिणाम = fillIn;
  अक्षर *packageName = nullptr;
  अक्षर *pathToResource = nullptr, *save = nullptr;
  अक्षर *locale = nullptr, *localeEnd = nullptr;
  int32_t length;

  अगर(status == nullptr || U_FAILURE(*status)) {
    वापस परिणाम;
  }

  length = (int32_t)(uprv_strlen(path)+1);
  save = pathToResource = (अक्षर *)uprv_malloc(length*माप(अक्षर));
  /* test for nullptr */
  अगर(pathToResource == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस परिणाम;
  }
  uprv_memcpy(pathToResource, path, length);

  locale = pathToResource;
  अगर(*pathToResource == RES_PATH_SEPARATOR) { /* there is a path specification */
    pathToResource++;
    packageName = pathToResource;
    pathToResource = uprv_strchr(pathToResource, RES_PATH_SEPARATOR);
    अगर(pathToResource == nullptr) {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा {
      *pathToResource = 0;
      locale = pathToResource+1;
    }
  }

  localeEnd = uprv_strchr(locale, RES_PATH_SEPARATOR);
  अगर(localeEnd != nullptr) {
    *localeEnd = 0;
  }

  first = ures_open(packageName, locale, status);

  अगर(U_SUCCESS(*status)) {
    अगर(localeEnd) {
      परिणाम = ures_findSubResource(first, localeEnd+1, fillIn, status);
    } अन्यथा {
      परिणाम = ures_copyResb(fillIn, first, status);
    }
    ures_close(first);
  }
  uprv_free(save);
  वापस परिणाम;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(स्थिर UResourceBundle* resB,
                     अक्षर* path,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status)
{
  Resource res = RES_BOGUS;
  UResourceBundle *परिणाम = fillIn;
  स्थिर अक्षर *कुंजी;

  अगर(status == nullptr || U_FAILURE(*status)) {
    वापस परिणाम;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  करो {
    res = res_findResource(&resB->get_Res_तथ्य(), resB->fRes, &path, &कुंजी); 
    अगर(res != RES_BOGUS) {
        परिणाम = init_resb_परिणाम(resB->f_तथ्य, res, कुंजी, -1, resB, fillIn, status);
        resB = परिणाम;
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
        अवरोध;
    }
  } जबतक(*path); /* there is more stuff in the path */

  वापस परिणाम;
}

U_CAPI स्थिर char16_t* U_EXPORT2
ures_get_String_By_कुंजी_With_Fallback(स्थिर UResourceBundle* resB U_LIFETIME_BOUND,
                                स्थिर अक्षर* in_कुंजी,
                                int32_t* len,
                                UErrorCode* status) {
    UResourceBundle ढेर_2;
    स्थिर char16_t* retVal = nullptr;
    ures_init_ढेर_Object(&ढेर_2);
    ures_get_By_कुंजी_With_Fallback(resB, in_कुंजी, &ढेर_2, status);
    int32_t length;
    retVal = ures_getString(&ढेर_2, &length, status);
    ures_close(&ढेर_2);
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर (length == 3 && retVal[0] == EMPTY_SET && retVal[1] == EMPTY_SET && retVal[2] == EMPTY_SET ) {
        retVal = nullptr;
        length = 0;
        *status = U_MISSING_RESOURCE_ERROR;
    }
    अगर (len != nullptr) {
        *len = length;
    }
    वापस retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
अटल Resource get_Table_Item_By_कुंजी_Path(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource table, स्थिर अक्षर *कुंजी) {
  Resource resource = table;  /* The current resource */
  icu::CharString path;
  UErrorCode errorCode = U_ZERO_ERROR;
  path.append(कुंजी, errorCode);
  अगर (U_FAILURE(errorCode)) { वापस RES_BOGUS; }
  अक्षर *pathPart = path.तथ्य();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  जबतक (*pathPart && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    अक्षर *अग्र_Path_Part = uprv_strchr(pathPart, RES_PATH_SEPARATOR);
    अगर (अग्र_Path_Part != nullptr) {
      *अग्र_Path_Part = 0;  /* Terminating null for this part of path. */
      अग्र_Path_Part++;
    } अन्यथा {
      अग्र_Path_Part = uprv_strchr(pathPart, 0);
    }
    int32_t t;
    स्थिर अक्षर *pathP = pathPart;
    resource = res_get_Table_Item_By_कुंजी(p_Res_तथ्य, resource, &t, &pathP);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    pathPart = अग्र_Path_Part; 
  }
  अगर (*pathPart) {
    वापस RES_BOGUS;
  }
  वापस resource;
}

अटल व्योम createPath(स्थिर अक्षर* origResPath,
                       int32_t     origResPathLen,
                       स्थिर अक्षर* resPath,
                       int32_t     resPathLen,
                       स्थिर अक्षर* in_कुंजी,
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
    path.सफा();
    स्थिर अक्षर* कुंजी = in_कुंजी;
    अगर (resPathLen > 0) {
        path.append(resPath, resPathLen, *status);
        अगर (U_SUCCESS(*status)) {
            स्थिर अक्षर* resPathLimit = resPath + resPathLen;
            स्थिर अक्षर* origResPathLimit = origResPath + origResPathLen;
            स्थिर अक्षर* resPathPtr = resPath;
            स्थिर अक्षर* origResPathPtr = origResPath;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            जबतक (origResPathPtr < origResPathLimit && resPathPtr < resPathLimit) {
                जबतक (origResPathPtr < origResPathLimit && *origResPathPtr != RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                अगर (origResPathPtr < origResPathLimit && *origResPathPtr == RES_PATH_SEPARATOR) {
                    ++origResPathPtr;
                }
                जबतक (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                अगर (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            जबतक (resPathPtr < resPathLimit && *कुंजी != '\0') {
                जबतक (resPathPtr < resPathLimit && *resPathPtr != RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                अगर (resPathPtr < resPathLimit && *resPathPtr == RES_PATH_SEPARATOR) {
                    ++resPathPtr;
                }
                जबतक (*कुंजी != '\0' && *कुंजी != RES_PATH_SEPARATOR) {
                    ++कुंजी;
                }
                अगर (*कुंजी == RES_PATH_SEPARATOR) {
                    ++कुंजी;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        path.append(कुंजी, *status);
    } अन्यथा {
        path.append(in_कुंजी, *status);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_कुंजी_With_Fallback(स्थिर UResourceBundle* resB,
                          स्थिर अक्षर* in_कुंजी,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status) {
    Resource res = RES_BOGUS, मूल_Res = RES_BOGUS;
    UResourceBundle *helper = nullptr;

    अगर (status==nullptr || U_FAILURE(*status)) {
        वापस fillIn;
    }
    अगर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    अगर(URES_IS_TABLE(type)) {
        स्थिर अक्षर* origResPath = resB->fResPath;
        int32_t origResPathLen = resB->fResPathLen;
        res = get_Table_Item_By_कुंजी_Path(&resB->get_Res_तथ्य(), resB->fRes, in_कुंजी);
        स्थिर अक्षर* कुंजी = in_कुंजी;
        bool did_मूल_Once = false;
        अगर(res == RES_BOGUS) {
            UResource_तथ्य_Entry *तथ्य_Entry = resB->f_तथ्य;
            CharString path;
            अक्षर *myPath = nullptr;
            स्थिर अक्षर* resPath = resB->fResPath;
            int32_t len = resB->fResPathLen;
            जबतक(res == RES_BOGUS && (तथ्य_Entry->fParent != nullptr || !did_मूल_Once)) { /* Otherwise, we'll look in parents */
                अगर (तथ्य_Entry->fParent != nullptr) {
                    तथ्य_Entry = तथ्य_Entry->fParent;
                } अन्यथा {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_मूल_Once = true;
                }
                मूल_Res = तथ्य_Entry->f_तथ्य.मूल_Res;

                अगर(तथ्य_Entry->fBogus == U_ZERO_ERROR) {
                    createPath(origResPath, origResPathLen, resPath, len, in_कुंजी, path, status);
                    अगर (U_FAILURE(*status)) {
                        ures_close(helper);
                        वापस fillIn;
                    }
                    myPath = path.तथ्य();
                    कुंजी = in_कुंजी;
                    करो {
                        res = res_findResource(&(तथ्य_Entry->f_तथ्य), मूल_Res, &myPath, &कुंजी);
                        अगर (RES_GET_TYPE(res) == URES_ALIAS && *myPath) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_परिणाम(तथ्य_Entry, res, nullptr, -1, resB, helper, status);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            अगर(helper) {
                              तथ्य_Entry = helper->f_तथ्य;
                              मूल_Res = helper->fRes;
                              resPath = helper->fResPath;
                              len = helper->fResPathLen;

                            } अन्यथा {
                              अवरोध;
                            }
                        } अन्यथा अगर (res == RES_BOGUS) {
                            अवरोध;
                        }
                    } जबतक(*myPath); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            अगर(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                अगर(uprv_strcmp(तथ्य_Entry->fName, uloc_getDefault())==0 || uprv_strcmp(तथ्य_Entry->fName, k_मूल_Locale_Name)==0) {
                    *status = U_USING_DEFAULT_WARNING;
                } अन्यथा {
                    *status = U_USING_FALLBACK_WARNING;
                }

                fillIn = init_resb_परिणाम(तथ्य_Entry, res, कुंजी, -1, resB, fillIn, status);
                अगर (resPath != nullptr) {
                    createPath(origResPath, origResPathLen, resPath, len, in_कुंजी, path, status);
                } अन्यथा {
                    स्थिर अक्षर* separator = nullptr;
                    अगर (fillIn->fResPath != nullptr) {
                        separator = uprv_strchr(fillIn->fResPath, RES_PATH_SEPARATOR);
                    }
                    अगर (separator != nullptr && separator[1] != '\0') {
                        createPath(origResPath, origResPathLen, fillIn->fResPath,
                                   static_cast<int32_t>(uprv_strlen(fillIn->fResPath)), in_कुंजी, path, status);
                    } अन्यथा {
                        createPath(origResPath, origResPathLen, "", 0, in_कुंजी, path, status);
                    }
                }
                ures_freeResPath(fillIn);
                ures_appendResPath(fillIn, path.तथ्य(), path.length(), status);
                अगर(fillIn->fResPath[fillIn->fResPathLen-1] != RES_PATH_SEPARATOR) {
                    ures_appendResPath(fillIn, RES_PATH_SEPARATOR_S, 1, status);
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            fillIn = init_resb_परिणाम(resB->f_तथ्य, res, कुंजी, -1, resB, fillIn, status);
        }
    } 
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    वापस fillIn;
}

namespace {

व्योम get_All_वस्तुएँ_With_Fallback(
        स्थिर UResourceBundle *bundle, Resource_तथ्य_मूल्य &value,
        ResourceSink &sink, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस; }
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
    value.set_तथ्य(bundle->get_Res_तथ्य());
    value.set_Valid_Locale_तथ्य_Entry(bundle->f_Valid_Locale_तथ्य_Entry);
    UResource_तथ्य_Entry *parentEntry = bundle->f_तथ्य->fParent;
    UBool hasParent = parentEntry != nullptr && U_SUCCESS(parentEntry->fBogus);
    value.setResource(bundle->fRes, ResourceTracer(bundle));
    sink.दो(bundle->f_कुंजी, value, !hasParent, errorCode);
    अगर (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ढेर_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.f_तथ्य = parentEntry;
        parentRef.f_Valid_Locale_तथ्य_Entry = bundle->f_Valid_Locale_तथ्य_Entry;
        parentRef.fHasFallback = !parentRef.get_Res_तथ्य().noFallback;
        parentRef.f_Is_शीर्ष_Level = true;
        parentRef.fRes = parentRef.get_Res_तथ्य().मूल_Res;
        parentRef.f_आकार = res_गिनती_Array_वस्तुएँ(&parentRef.get_Res_तथ्य(), parentRef.fRes);
        parentRef.f_सूचकांक = -1;
        entryIncrease(parentEntry);

        // Look up the container item in the parent bundle.
        ढेर_UResource_Bundle containerBundle;
        स्थिर UResourceBundle *rb;
        UErrorCode pathErrorCode = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        अगर (bundle->fResPath == nullptr || *bundle->fResPath == 0) {
            rb = parentBundle.getAlias();
        } अन्यथा {
            rb = ures_get_By_कुंजी_With_Fallback(parentBundle.getAlias(), bundle->fResPath,
                                           containerBundle.getAlias(), &pathErrorCode);
        }
        अगर (U_SUCCESS(pathErrorCode)) {
            get_All_वस्तुएँ_With_Fallback(rb, value, sink, errorCode);
        }
    }
}

काष्ठा GetAllChildrenSink : खुला ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    भव ~GetAllChildrenSink() override;
    भव व्योम दो(स्थिर अक्षर *कुंजी, Resource_मूल्य &value, UBool is_मूल,
           UErrorCode &errorCode) override {
        ResourceTable वस्तुएँ_Table = value.getTable(errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        क्रम (int32_t i = 0; वस्तुएँ_Table.get_कुंजी_And_मूल्य(i, कुंजी, value); ++i) {
            अगर (value.getType() == URES_ALIAS) {
                Resource_तथ्य_मूल्य& rdv = static_cast<Resource_तथ्य_मूल्य&>(value);
                ढेर_UResource_Bundle ढेर_अस्थायी_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.get_तथ्य(), rdv.getResource(), nullptr, -1,
                                                                          rdv.get_Valid_Locale_तथ्य_Entry(), nullptr, 0,
                                                                          ढेर_अस्थायी_Bundle.getAlias(), &errorCode);
                अगर (U_SUCCESS(errorCode)) {
                    Resource_तथ्य_मूल्य aliased_मूल्य;
                    aliased_मूल्य.set_तथ्य(aliasRB->get_Res_तथ्य());
                    aliased_मूल्य.set_Valid_Locale_तथ्य_Entry(aliasRB->f_Valid_Locale_तथ्य_Entry);
                    aliased_मूल्य.setResource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    अगर (aliased_मूल्य.getType() != URES_TABLE) {
                        dest.दो(कुंजी, aliased_मूल्य, is_मूल, errorCode);
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
                        स्थिर अक्षर* parent_कुंजी = कुंजी; // dest.put() changes the key
                        dest.दो(parent_कुंजी, aliased_मूल्य, is_मूल, errorCode);
                        UResource_तथ्य_Entry* entry = aliasRB->f_तथ्य;
                        Resource res = aliasRB->fRes;
                        जबतक (aliased_मूल्य_Type == URES_TABLE && entry->fParent != nullptr) {
                            CharString localPath;
                            localPath.copyFrom(tablePath, errorCode);
                            अक्षर* localPathAsCharPtr = localPath.तथ्य();
                            स्थिर अक्षर* child_कुंजी;
                            entry = entry->fParent;
                            res = entry->f_तथ्य.मूल_Res;
                            Resource newRes = res_findResource(&entry->f_तथ्य, res, &localPathAsCharPtr, &child_कुंजी);
                            अगर (newRes != RES_BOGUS) {
                                aliased_मूल्य.set_तथ्य(entry->f_तथ्य);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_मूल्य.setResource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_मूल्य_Type = aliased_मूल्य.getType();
                                अगर (aliased_मूल्य_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_तथ्य_मूल्य& rdv2 = static_cast<Resource_तथ्य_मूल्य&>(aliased_मूल्य);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.get_तथ्य(), rdv2.getResource(), nullptr, -1,
                                                                             rdv2.get_Valid_Locale_तथ्य_Entry(), nullptr, 0,
                                                                             ढेर_अस्थायी_Bundle.getAlias(), &errorCode);
                                    tablePath.सफा();
                                    tablePath.append(aliasRB->fResPath, errorCode);
                                    entry = aliasRB->f_तथ्य;
                                    res = aliasRB->fRes;
                                    aliased_मूल्य.set_तथ्य(entry->f_तथ्य);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_मूल्य.setResource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_मूल्य_Type = aliased_मूल्य.getType();
                                }
                                अगर (aliased_मूल्य_Type == URES_TABLE) {
                                    dest.दो(parent_कुंजी, aliased_मूल्य, is_मूल, errorCode);
                                } अन्यथा {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    errorCode = U_INTERNAL_PROGRAM_ERROR;
                                    वापस;
                                }
                            }
                        }
                    }
                }
            } अन्यथा {
                dest.दो(कुंजी, value, is_मूल, errorCode);
            }
            अगर (U_FAILURE(errorCode)) { वापस; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI व्योम U_EXPORT2
ures_getAllChildrenWithFallback(स्थिर UResourceBundle *bundle, स्थिर अक्षर *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_वस्तुएँ_With_Fallback(bundle, path, allChildrenSink, errorCode);
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
U_CAPI व्योम U_EXPORT2
ures_get_मूल्य_With_Fallback(स्थिर UResourceBundle *bundle, स्थिर अक्षर *path,
                          UResourceBundle *अस्थायी_Fill_In,
                          Resource_तथ्य_मूल्य &value, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस; }
    अगर (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    स्थिर UResourceBundle *rb;
    अगर (*path == 0) {
        // empty path
        rb = bundle;
    } अन्यथा {
        rb = ures_get_By_कुंजी_With_Fallback(bundle, path, अस्थायी_Fill_In, &errorCode);
        अगर (U_FAILURE(errorCode)) {
            वापस;
        }
    }
    value.set_तथ्य(rb->get_Res_तथ्य());
    value.set_Valid_Locale_तथ्य_Entry(rb->f_Valid_Locale_तथ्य_Entry);
    value.setResource(rb->fRes, ResourceTracer(rb));
}

U_CAPI व्योम U_EXPORT2
ures_get_All_वस्तुएँ_With_Fallback(स्थिर UResourceBundle *bundle, स्थिर अक्षर *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस; }
    अगर (path == nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    ढेर_UResource_Bundle ढेर_Bundle;
    स्थिर UResourceBundle *rb;
    अगर (*path == 0) {
        // empty path
        rb = bundle;
    } अन्यथा {
        rb = ures_get_By_कुंजी_With_Fallback(bundle, path, ढेर_Bundle.getAlias(), &errorCode);
        अगर (U_FAILURE(errorCode)) {
            वापस;
        }
    }
    // Get all table items with fallback.
    Resource_तथ्य_मूल्य value;
    get_All_वस्तुएँ_With_Fallback(rb, value, sink, errorCode);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_कुंजी(स्थिर UResourceBundle* resB,
              स्थिर अक्षर* in_कुंजी,
              UResourceBundle* fillIn U_LIFETIME_BOUND,
              UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResource_तथ्य_Entry *तथ्य_Entry = nullptr;
    स्थिर अक्षर *कुंजी = in_कुंजी;

    अगर (status==nullptr || U_FAILURE(*status)) {
        वापस fillIn;
    }
    अगर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस fillIn;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    अगर(URES_IS_TABLE(type)) {
        int32_t t;
        res = res_get_Table_Item_By_कुंजी(&resB->get_Res_तथ्य(), resB->fRes, &t, &कुंजी);
        अगर(res == RES_BOGUS) {
            कुंजी = in_कुंजी;
            अगर(resB->fHasFallback) {
                तथ्य_Entry = get_Fallback_तथ्य(resB, &कुंजी, &res, status);
                अगर(U_SUCCESS(*status)) {
                    /* check if resB->fResPath gives the right name here */
                    वापस init_resb_परिणाम(तथ्य_Entry, res, कुंजी, -1, resB, fillIn, status);
                } अन्यथा {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            वापस init_resb_परिणाम(resB->f_तथ्य, res, कुंजी, -1, resB, fillIn, status);
        }
    } 
#अगर 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    अन्यथा अगर(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == true) {
        /* here should go a first attempt to locate the key using index table */
        तथ्य_Entry = get_Fallback_तथ्य(resB, &कुंजी, &res, status);
        अगर(U_SUCCESS(*status)) {
            वापस init_resb_परिणाम(तथ्य_Entry, res, कुंजी, resB, fillIn, status);
        } अन्यथा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    }
#पूर्ण_अगर    
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस fillIn;
}

U_CAPI स्थिर char16_t* U_EXPORT2
ures_get_String_By_कुंजी(स्थिर UResourceBundle* resB U_LIFETIME_BOUND,
                    स्थिर अक्षर* in_कुंजी,
                    int32_t* len,
                    UErrorCode* status) {
    Resource res = RES_BOGUS;
    UResource_तथ्य_Entry *तथ्य_Entry = nullptr;
    स्थिर अक्षर* कुंजी = in_कुंजी;

    अगर (status==nullptr || U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर(resB == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    अगर(URES_IS_TABLE(type)) {
        int32_t t=0;

        res = res_get_Table_Item_By_कुंजी(&resB->get_Res_तथ्य(), resB->fRes, &t, &कुंजी);

        अगर(res == RES_BOGUS) {
            कुंजी = in_कुंजी;
            अगर(resB->fHasFallback) {
                तथ्य_Entry = get_Fallback_तथ्य(resB, &कुंजी, &res, status);
                अगर(U_SUCCESS(*status)) {
                    चयन (RES_GET_TYPE(res)) {
                    हाल URES_STRING:
                    हाल URES_STRING_V2:
                        वापस res_getString({resB, कुंजी}, &तथ्य_Entry->f_तथ्य, res, len);
                    हाल URES_ALIAS:
                      {
                        स्थिर char16_t* परिणाम = nullptr;
                        UResourceBundle *अस्थायी_Res = ures_get_By_कुंजी(resB, in_कुंजी, nullptr, status);
                        परिणाम = ures_getString(अस्थायी_Res, len, status);
                        ures_close(अस्थायी_Res);
                        वापस परिणाम;
                      }
                    शेष:
                        *status = U_RESOURCE_TYPE_MISMATCH;
                    }
                } अन्यथा {
                    *status = U_MISSING_RESOURCE_ERROR;
                }
            } अन्यथा {
                *status = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            चयन (RES_GET_TYPE(res)) {
            हाल URES_STRING:
            हाल URES_STRING_V2:
                वापस res_getString({resB, कुंजी}, &resB->get_Res_तथ्य(), res, len);
            हाल URES_ALIAS:
              {
                स्थिर char16_t* परिणाम = nullptr;
                UResourceBundle *अस्थायी_Res = ures_get_By_कुंजी(resB, in_कुंजी, nullptr, status);
                परिणाम = ures_getString(अस्थायी_Res, len, status);
                ures_close(अस्थायी_Res);
                वापस परिणाम;
              }
            शेष:
                *status = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#अगर 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    अन्यथा अगर(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == true) {
        /* here should go a first attempt to locate the key using index table */
        तथ्य_Entry = get_Fallback_तथ्य(resB, &कुंजी, &res, status);
        अगर(U_SUCCESS(*status)) {
            // TODO: Tracing
            वापस res_getString(rd, res, len);
        } अन्यथा {
            *status = U_MISSING_RESOURCE_ERROR;
        }
    } 
#पूर्ण_अगर    
    अन्यथा {
        *status = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस nullptr;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ures_get_UTF8_String_By_कुंजी(स्थिर UResourceBundle* resB,
                        स्थिर अक्षर* कुंजी,
                        अक्षर* dest U_LIFETIME_BOUND,
                        int32_t* p_लंबाई,
                        UBool forceCopy,
                        UErrorCode* status) {
    int32_t length16;
    स्थिर char16_t *s16 = ures_get_String_By_कुंजी(resB, कुंजी, &length16, status);
    वापस ures_toUTF8String(s16, length16, dest, p_लंबाई, forceCopy, status);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI स्थिर अक्षर* U_EXPORT2
ures_getLocaleInternal(स्थिर UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
    अगर (status==nullptr || U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    } अन्यथा {
      वापस resourceBundle->f_तथ्य->fName;
    }
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ures_getLocale(स्थिर UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status)
{
  वापस ures_getLocaleInternal(resourceBundle, status);
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ures_getLocaleByType(स्थिर UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_तथ्य_Locale_Type type,
                     UErrorCode* status) {
    अगर (status==nullptr || U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर (!resourceBundle) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    } अन्यथा {
        चयन(type) {
        हाल ULOC_ACTUAL_LOCALE:
            वापस resourceBundle->f_तथ्य->fName;
        हाल ULOC_VALID_LOCALE:
            वापस resourceBundle->f_Valid_Locale_तथ्य_Entry->fName;
        हाल ULOC_REQUESTED_LOCALE:
        शेष:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस nullptr;
        }
    }
}

U_CFUNC स्थिर अक्षर*
ures_getName(स्थिर UResourceBundle* resB U_LIFETIME_BOUND) {
  अगर(resB == nullptr) {
    वापस nullptr;
  }

  वापस resB->f_तथ्य->fName;
}

#अगर_घोषित URES_DEBUG
U_CFUNC स्थिर अक्षर*
ures_getPath(स्थिर UResourceBundle* resB U_LIFETIME_BOUND) {
  अगर(resB == nullptr) {
    वापस nullptr;
  }

  वापस resB->f_तथ्य->fPath;
}
#पूर्ण_अगर

अटल UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  स्थिर अक्षर* path,
                  स्थिर अक्षर* localeID,
                  UResOpenType openType,
                  UErrorCode* status) {
    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }

    UResource_तथ्य_Entry *entry;
    अगर(openType != URES_OPEN_DIRECT) {
        अगर (localeID == nullptr) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_getBaseName(localeID, *status);
        अगर(U_FAILURE(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस nullptr;
        }
        entry = entryOpen(path, canonLocaleID.तथ्य(), openType, status);
    } अन्यथा {
        entry = entryOpenDirect(path, localeID, status);
    }
    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर(entry == nullptr) {
        *status = U_MISSING_RESOURCE_ERROR;
        वापस nullptr;
    }

    UBool is_ढेर_Object;
    अगर(r == nullptr) {
        r = static_cast<UResourceBundle*>(uprv_malloc(माप(UResourceBundle)));
        अगर(r == nullptr) {
            entryClose(entry);
            *status = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }
        is_ढेर_Object = false;
    } अन्यथा {  // fill-in
        is_ढेर_Object = ures_is_ढेर_Object(r);
        ures_closeBundle(r, false);
    }
    uprv_memset(r, 0, माप(UResourceBundle));
    ures_set_Is_ढेर_Object(r, is_ढेर_Object);

    r->f_Valid_Locale_तथ्य_Entry = r->f_तथ्य = entry;
    r->fHasFallback = openType != URES_OPEN_DIRECT && !r->get_Res_तथ्य().noFallback;
    r->f_Is_शीर्ष_Level = true;
    r->fRes = r->get_Res_तथ्य().मूल_Res;
    r->f_आकार = res_गिनती_Array_वस्तुएँ(&r->get_Res_तथ्य(), r->fRes);
    r->f_सूचकांक = -1;

    ResourceTracer(r).traceOpen();

    वापस r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(स्थिर अक्षर* path, स्थिर अक्षर* localeID, UErrorCode* status) {
    वापस ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_DEFAULT_मूल, status);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(स्थिर अक्षर* path, स्थिर अक्षर* localeID, UErrorCode* status) {
    वापस ures_openWithType(nullptr, path, localeID, URES_OPEN_LOCALE_मूल, status);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_openDirect(स्थिर अक्षर* path, स्थिर अक्षर* localeID, UErrorCode* status) {
    वापस ures_openWithType(nullptr, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI व्योम U_EXPORT2
ures_openFillIn(UResourceBundle *r, स्थिर अक्षर* path,
                स्थिर अक्षर* localeID, UErrorCode* status) {
    अगर(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_LOCALE_DEFAULT_मूल, status);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI व्योम U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r, स्थिर अक्षर* path, स्थिर अक्षर* localeID, UErrorCode* status) {
    अगर(U_SUCCESS(*status) && r == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    ures_openWithType(r, path, localeID, URES_OPEN_DIRECT, status);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_गिनती_Array_वस्तुएँ(स्थिर UResourceBundle* resourceBundle,
                  स्थिर अक्षर* resource_कुंजी,
                  UErrorCode* status)
{
    UResourceBundle res_तथ्य;
    ures_init_ढेर_Object(&res_तथ्य);
    अगर (status==nullptr || U_FAILURE(*status)) {
        वापस 0;
    }
    अगर(resourceBundle == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    ures_get_By_कुंजी(resourceBundle, resource_कुंजी, &res_तथ्य, status);
    
    अगर(res_तथ्य.get_Res_तथ्य().तथ्य != nullptr) {
        int32_t परिणाम = res_गिनती_Array_वस्तुएँ(&res_तथ्य.get_Res_तथ्य(), res_तथ्य.fRes);
        ures_close(&res_तथ्य);
        वापस परिणाम;
    } अन्यथा {
        *status = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_तथ्य);
        वापस 0;
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
U_CAPI स्थिर अक्षर* U_EXPORT2
ures_get_Version_संख्या_Internal(स्थिर UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    अगर (!resourceBundle) वापस nullptr;

    अगर(resourceBundle->fVersion == nullptr) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UErrorCode status = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        स्थिर char16_t* minor_version = ures_get_String_By_कुंजी(resourceBundle, kVersionTag, &minor_len, &status);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->fVersion = (अक्षर *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        अगर (((UResourceBundle *)resourceBundle)->fVersion == nullptr) {
            वापस nullptr;
        }
       
        अगर(minor_len > 0) {
            u_UCharsToChars(minor_version, resourceBundle->fVersion , minor_len);
            resourceBundle->fVersion[len] =  '\0';
        }
        अन्यथा {
            uprv_strcpy(resourceBundle->fVersion, kDefaultMinorVersion);
        }
    }

    वापस resourceBundle->fVersion;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ures_get_Version_संख्या(स्थिर UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    वापस ures_get_Version_संख्या_Internal(resourceBundle);
}

U_CAPI व्योम U_EXPORT2 ures_getVersion(स्थिर UResourceBundle* resB, UVersionInfo versionInfo) {
    अगर (!resB) वापस;

    u_versionFromString(versionInfo, ures_get_Version_संख्या_Internal(resB));
}

/** Tree support functions *******************************/
#घोषणा सूचकांक_LOCALE_NAME "res_index"
#घोषणा सूचकांक_TAG         "InstalledLocales"
#घोषणा DEFAULT_TAG       "default"

#अगर defined(URES_वृक्ष_DEBUG)
#समावेश <stdio.h>
#पूर्ण_अगर

प्रकार काष्ठा ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

अटल व्योम U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

अटल int32_t U_CALLCONV
ures_loc_गिनती_Locales(UEnumeration *en, UErrorCode * /*status*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    वापस ures_get_आकार(&ctx->installed);
}

U_CDECL_BEGIN


अटल स्थिर अक्षर * U_CALLCONV
ures_loc_अग्र_Locale(UEnumeration* en,
                    int32_t* परिणाम_लंबाई,
                    UErrorCode* status) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = nullptr;
    स्थिर अक्षर *परिणाम = nullptr;
    int32_t len = 0;
    अगर (ures_has_अग्र(res) && (k = ures_get_अग्र_Resource(res, &ctx->curr, status)) != nullptr) {
        परिणाम = ures_get_कुंजी(k);
        len = (int32_t)uprv_strlen(परिणाम);
    }
    अगर (परिणाम_लंबाई) {
        *परिणाम_लंबाई = len;
    }
    वापस परिणाम;
}

अटल व्योम U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UErrorCode* /*status*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_END

अटल स्थिर UEnumeration gLocalesEnum = {
    nullptr,
        nullptr,
        ures_loc_closeLocales,
        ures_loc_गिनती_Locales,
        uenum_unextDefault,
        ures_loc_अग्र_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_openAvailableLocales(स्थिर अक्षर *path, UErrorCode *status)
{
    UResourceBundle *idx = nullptr;
    UEnumeration *en = nullptr;
    ULocalesContext *myContext = nullptr;

    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(माप(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(माप(UEnumeration));
    अगर(!en || !myContext) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        वापस nullptr;
    }
    uprv_memcpy(en, &gLocalesEnum, माप(UEnumeration));

    ures_init_ढेर_Object(&myContext->installed);
    ures_init_ढेर_Object(&myContext->curr);
    idx = ures_openDirect(path, सूचकांक_LOCALE_NAME, status);
    ures_get_By_कुंजी(idx, सूचकांक_TAG, &myContext->installed, status);
    अगर(U_SUCCESS(*status)) {
#अगर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "Got %s::%s::[%s] : %s\n", 
            path, सूचकांक_LOCALE_NAME, सूचकांक_TAG, ures_get_कुंजी(&myContext->installed));
#पूर्ण_अगर
        en->context = myContext;
    } अन्यथा {
#अगर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s open failed - %s\n", path, u_errorName(*status));
#पूर्ण_अगर
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = nullptr;
    }
    
    ures_close(idx);
    
    वापस en;
}

अटल UBool isLocaleInList(UEnumeration *locEnum, स्थिर अक्षर *loc_To_खोज, UErrorCode *status) {
    स्थिर अक्षर *loc;
    जबतक ((loc = uenum_अग्र(locEnum, nullptr, status)) != nullptr) {
        अगर (uprv_strcmp(loc, loc_To_खोज) == 0) {
            वापस true;
        }
    }
    वापस false;
}

अटल व्योम getParentForFunctionalEquivalent(स्थिर अक्षर*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UErrorCode subStatus = U_ZERO_ERROR;
    parent.सफा();
    अगर (res != nullptr) {
        ures_get_By_कुंजी(res, "%%Parent", bund1, &subStatus);
        अगर (U_SUCCESS(subStatus)) {
            int32_t length16;
            स्थिर char16_t* s16 = ures_getString(bund1, &length16, &subStatus);
            parent.appendInvariantChars(s16, length16, subStatus);
        }
    }
    
    // If none there, use normal truncation parent
    अगर (U_FAILURE(subStatus) || parent.isEmpty()) {
        subStatus = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, subStatus);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(अक्षर *परिणाम, int32_t परिणाम_Capacity,
                             स्थिर अक्षर *path, स्थिर अक्षर *resName, स्थिर अक्षर *keyword, स्थिर अक्षर *locid,
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
    अगर(U_FAILURE(*status)) वापस 0;
    CharString kwVal;
    अगर (keyword != nullptr && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_मूल्य(locid, keyword, subStatus);
        अगर (kwVal == DEFAULT_TAG) {
            kwVal.सफा();
        }
    }
    अगर (locid == nullptr) {
        locid = uloc_getDefault();
    }
    CharString base = ulocimp_getBaseName(locid, subStatus);
#अगर defined(URES_वृक्ष_DEBUG)
    ख_लिखो(मानक_त्रुटि, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.तथ्य(), base.तथ्य(), u_errorName(subStatus));
#पूर्ण_अगर
    ures_init_ढेर_Object(&bund1);
    ures_init_ढेर_Object(&bund2);

    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    अगर(isAvailable) {
        UEnumeration *locEnum = ures_openAvailableLocales(path, &subStatus);
        *isAvailable = true;
        अगर (U_SUCCESS(subStatus)) {
            *isAvailable = isLocaleInList(locEnum, parent.तथ्य(), &subStatus);
        }
        uenum_close(locEnum);
    }

    अगर(U_FAILURE(subStatus)) {
        *status = subStatus;
        वापस 0;
    }
    
    करो {
        subStatus = U_ZERO_ERROR;
        res = ures_open(path, parent.तथ्य(), &subStatus);
        अगर(((subStatus == U_USING_FALLBACK_WARNING) ||
            (subStatus == U_USING_DEFAULT_WARNING)) && isAvailable)
        {
            *isAvailable = false;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#अगर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s;%s -> %s [%s]\n", path?path:"ICUDATA", parent.तथ्य(), u_errorName(subStatus), ures_getLocale(res, &subStatus));
#पूर्ण_अगर
        अगर(U_FAILURE(subStatus)) {
            *status = subStatus;
        } अन्यथा अगर(subStatus == U_ZERO_ERROR) {
            ures_get_By_कुंजी(res,resName,&bund1, &subStatus);
            अगर(subStatus == U_ZERO_ERROR) {
                स्थिर char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#अगर defined(URES_वृक्ष_DEBUG)
                ख_लिखो(मानक_त्रुटि, "%s;%s : loaded default -> %s\n",
                    path?path:"ICUDATA", parent.तथ्य(), u_errorName(subStatus));
#पूर्ण_अगर
                defUstr = ures_get_String_By_कुंजी(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                अगर(U_SUCCESS(subStatus) && defLen) {
                    defVal.सफा().appendInvariantChars(defUstr, defLen, subStatus);
#अगर defined(URES_वृक्ष_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s;%s -> default %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.तथ्य(), keyword, defVal.तथ्य(), u_errorName(subStatus));
#पूर्ण_अगर
                    defLoc.copyFrom(parent, subStatus);
                    अगर(kwVal.isEmpty()) {
                        kwVal.append(defVal, subStatus);
#अगर defined(URES_वृक्ष_DEBUG)
                        ख_लिखो(मानक_त्रुटि, "%s;%s -> kwVal =  %s\n", 
                            path?path:"ICUDATA", parent.तथ्य(), keyword, kwVal.तथ्य());
#पूर्ण_अगर
                    }
                }
            }
        }
        
        subStatus = U_ZERO_ERROR;

        अगर (res != nullptr) {
            found.सफा().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus), subStatus);
        }

        अगर (found != parent) {
            parent.copyFrom(found, subStatus);
        } अन्यथा {
            getParentForFunctionalEquivalent(found.तथ्य(),res,&bund1,parent);
        }
        ures_close(res);
    } जबतक(defVal.isEmpty() && !found.isEmpty() && found != "root" && U_SUCCESS(*status));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(base, subStatus);
    found.copyFrom(base, subStatus);

    करो {
        res = ures_open(path, parent.तथ्य(), &subStatus);
        अगर((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
            *isAvailable = false;
        }
        isAvailable = nullptr; /* only want to set this the first time around */
        
#अगर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "%s;%s -> %s (looking for %s)\n", 
            path?path:"ICUDATA", parent.तथ्य(), u_errorName(subStatus), kwVal.तथ्य());
#पूर्ण_अगर
        अगर(U_FAILURE(subStatus)) {
            *status = subStatus;
        } अन्यथा अगर(subStatus == U_ZERO_ERROR) {
            ures_get_By_कुंजी(res,resName,&bund1, &subStatus);
#अगर defined(URES_वृक्ष_DEBUG)
/**/ ख_लिखो(मानक_त्रुटि,"@%d [%s] %s\n", __LINE__, resName, u_errorName(subStatus));
#पूर्ण_अगर
            अगर(subStatus == U_ZERO_ERROR) {
                ures_get_By_कुंजी(&bund1, kwVal.तथ्य(), &bund2, &subStatus);
#अगर defined(URES_वृक्ष_DEBUG)
/**/ ख_लिखो(मानक_त्रुटि,"@%d [%s] %s\n", __LINE__, kwVal.तथ्य(), u_errorName(subStatus));
#पूर्ण_अगर
                अगर(subStatus == U_ZERO_ERROR) {
#अगर defined(URES_वृक्ष_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s;%s -> full0 %s=%s,  %s\n", 
                        path?path:"ICUDATA", parent.तथ्य(), keyword, kwVal.तथ्य(), u_errorName(subStatus));
#पूर्ण_अगर
                    अगर (parent.isEmpty()) {
                        full.सफा().append("root", subStatus);
                    } अन्यथा {
                        full.copyFrom(parent, subStatus);
                    }
                        /* now, recalculate default kw if need be */
                        अगर(defLoc.length() > full.length()) {
                          स्थिर char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#अगर defined(URES_वृक्ष_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> recalculating Default0\n", 
                                    path?path:"ICUDATA", full.तथ्य());
#पूर्ण_अगर
                          defUstr = ures_get_String_By_कुंजी(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          अगर(U_SUCCESS(subStatus) && defLen) {
                            defVal.सफा().appendInvariantChars(defUstr, defLen, subStatus);
#अगर defined(URES_वृक्ष_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> default0 %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.तथ्य(), keyword, defVal.तथ्य(), u_errorName(subStatus));
#पूर्ण_अगर
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#अगर defined(URES_वृक्ष_DEBUG)
                        अन्यथा {
                          ख_लिखो(मानक_त्रुटि, "No trim0,  %s <= %s\n", defLoc.तथ्य(), full.तथ्य());
                        }
#पूर्ण_अगर
                } अन्यथा {
#अगर defined(URES_वृक्ष_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "err=%s in %s looking for %s\n", 
                        u_errorName(subStatus), parent.तथ्य(), kwVal.तथ्य());
#पूर्ण_अगर
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
        अगर (res != nullptr && uprv_strcmp(resName, "collations") == 0) {
            स्थिर अक्षर *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &subStatus);
            अगर (U_SUCCESS(subStatus) && validLoc != nullptr && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, subStatus);
                CharString parentLang = ulocimp_getLanguage(parent.toStringPiece(), subStatus);
                अगर (U_SUCCESS(subStatus) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    found.सफा().append(validLoc, subStatus);
                    haveFound = true;
                }
            }
            subStatus = U_ZERO_ERROR;
        }
        अगर (!haveFound) {
            found.copyFrom(parent, subStatus);
        }

        getParentForFunctionalEquivalent(found.तथ्य(),res,&bund1,parent);
        ures_close(res);
        subStatus = U_ZERO_ERROR;
    } जबतक(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));

    अगर(full.isEmpty() && kwVal != defVal) {
#अगर defined(URES_वृक्ष_DEBUG)
        ख_लिखो(मानक_त्रुटि, "Failed to locate kw %s - try default %s\n", kwVal.तथ्य(), defVal.तथ्य());
#पूर्ण_अगर
        kwVal.सफा().append(defVal, subStatus);
        parent.copyFrom(base, subStatus);
        found.copyFrom(base, subStatus);

        करो { /* search for 'default' named item */
            res = ures_open(path, parent.तथ्य(), &subStatus);
            अगर((subStatus == U_USING_FALLBACK_WARNING) && isAvailable) {
                *isAvailable = false;
            }
            isAvailable = nullptr; /* only want to set this the first time around */
            
#अगर defined(URES_वृक्ष_DEBUG)
            ख_लिखो(मानक_त्रुटि, "%s;%s -> %s (looking for default %s)\n",
                path?path:"ICUDATA", parent.तथ्य(), u_errorName(subStatus), kwVal.तथ्य());
#पूर्ण_अगर
            अगर(U_FAILURE(subStatus)) {
                *status = subStatus;
            } अन्यथा अगर(subStatus == U_ZERO_ERROR) {
                ures_get_By_कुंजी(res,resName,&bund1, &subStatus);
                अगर(subStatus == U_ZERO_ERROR) {
                    ures_get_By_कुंजी(&bund1, kwVal.तथ्य(), &bund2, &subStatus);
                    अगर(subStatus == U_ZERO_ERROR) {
#अगर defined(URES_वृक्ष_DEBUG)
                        ख_लिखो(मानक_त्रुटि, "%s;%s -> full1 %s=%s,  %s\n", path?path:"ICUDATA",
                            parent.तथ्य(), keyword, kwVal.तथ्य(), u_errorName(subStatus));
#पूर्ण_अगर
                        अगर (parent.isEmpty()) {
                            full.सफा().append("root", subStatus);
                        } अन्यथा {
                            full.copyFrom(parent, subStatus);
                        }
                        
                        /* now, recalculate default kw if need be */
                        अगर(defLoc.length() > full.length()) {
                          स्थिर char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#अगर defined(URES_वृक्ष_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> recalculating Default1\n", 
                                    path?path:"ICUDATA", full.तथ्य());
#पूर्ण_अगर
                          defUstr = ures_get_String_By_कुंजी(&bund1, DEFAULT_TAG, &defLen, &subStatus);
                          अगर(U_SUCCESS(subStatus) && defLen) {
                            defVal.सफा().appendInvariantChars(defUstr, defLen, subStatus);
#अगर defined(URES_वृक्ष_DEBUG)
                            ख_लिखो(मानक_त्रुटि, "%s;%s -> default %s=%s,  %s\n", 
                                    path?path:"ICUDATA", full.तथ्य(), keyword, defVal.तथ्य(), u_errorName(subStatus));
#पूर्ण_अगर
                            defLoc.copyFrom(full, subStatus);
                          }
                        } /* end of recalculate default KW */
#अगर defined(URES_वृक्ष_DEBUG)
                        अन्यथा {
                          ख_लिखो(मानक_त्रुटि, "No trim1,  %s <= %s\n", defLoc.तथ्य(), full.तथ्य());
                        }
#पूर्ण_अगर
                    }
                }
            }
            
            subStatus = U_ZERO_ERROR;
            found.copyFrom(parent, subStatus);
            getParentForFunctionalEquivalent(found.तथ्य(),res,&bund1,parent);
            ures_close(res);
            subStatus = U_ZERO_ERROR;
        } जबतक(full.isEmpty() && !found.isEmpty() && U_SUCCESS(*status));
    }
    
    अगर(U_SUCCESS(*status)) {
        अगर(full.isEmpty()) {
#अगर defined(URES_वृक्ष_DEBUG)
          ख_लिखो(मानक_त्रुटि, "Still could not load keyword %s=%s\n", keyword, kwVal.तथ्य());
#पूर्ण_अगर
          *status = U_MISSING_RESOURCE_ERROR;
        } अन्यथा अगर(omitDefault) {
#अगर defined(URES_वृक्ष_DEBUG)
          ख_लिखो(मानक_त्रुटि,"Trim? full=%s, defLoc=%s, found=%s\n", full.तथ्य(), defLoc.तथ्य(), found.तथ्य());
#पूर्ण_अगर        
          अगर(defLoc.length() <= full.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            अगर(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#अगर defined(URES_वृक्ष_DEBUG)
              ख_लिखो(मानक_त्रुटि, "Removing unneeded var %s=%s\n", keyword, kwVal.तथ्य());
#पूर्ण_अगर
              kwVal.सफा();
            }
          }
        }
        found.copyFrom(full, subStatus);
        अगर(!kwVal.isEmpty()) {
            found
                .append("@", subStatus)
                .append(keyword, subStatus)
                .append("=", subStatus)
                .append(kwVal, subStatus);
        } अन्यथा अगर(!omitDefault) {
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

    अगर(U_SUCCESS(*status)) {
        int32_t copy_लंबाई = uprv_min(length, परिणाम_Capacity);
        अगर(copy_लंबाई>0) {
            found.extract(परिणाम, copy_लंबाई, subStatus);
        }
        अगर(length == 0) {
          *status = U_MISSING_RESOURCE_ERROR; 
        }
    } अन्यथा {
        length = 0;
        परिणाम[0]=0;
    }
    वापस u_terminateChars(परिणाम, परिणाम_Capacity, length, status);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_मूल्यावली(स्थिर अक्षर *path, स्थिर अक्षर *keyword, UErrorCode *status)
{
#घोषणा मूल्यावली_BUF_आकार 2048
#घोषणा मूल्यावली_LIST_आकार 512
    
    अक्षर       मूल्यावली_Buf[मूल्यावली_BUF_आकार];
    int32_t    मूल्यावली_सूचकांक = 0;
    स्थिर अक्षर *मूल्यावली_List[मूल्यावली_LIST_आकार];
    int32_t    मूल्यावली_गिनती = 0;
    
    स्थिर अक्षर *locale;
    int32_t     locLen;
    
    UEnumeration *locs = nullptr;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_ढेर_Object(&item);
    ures_init_ढेर_Object(&subItem);
    locs = ures_openAvailableLocales(path, status);
    
    अगर(U_FAILURE(*status)) {
        ures_close(&item);
        ures_close(&subItem);
        वापस nullptr;
    }
    
    मूल्यावली_Buf[0]=0;
    मूल्यावली_Buf[1]=0;

    जबतक ((locale = uenum_अग्र(locs, &locLen, status)) != nullptr) {
        UResourceBundle   *bund = nullptr;
        UResourceBundle   *subPtr = nullptr;
        UErrorCode subStatus = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(path, locale, &subStatus);
        
#अगर defined(URES_वृक्ष_DEBUG)
        अगर(!bund || U_FAILURE(subStatus)) {
            ख_लिखो(मानक_त्रुटि, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
        }
#पूर्ण_अगर
        
        ures_get_By_कुंजी(bund, keyword, &item, &subStatus);
        
        अगर(!bund || U_FAILURE(subStatus)) {
#अगर defined(URES_वृक्ष_DEBUG)
            ख_लिखो(मानक_त्रुटि, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                path?path:"<ICUDATA>", keyword, locale, u_errorName(subStatus));
#पूर्ण_अगर
            ures_close(bund);
            bund = nullptr;
            जारी;
        }

        जबतक ((subPtr = ures_get_अग्र_Resource(&item, &subItem, &subStatus)) != nullptr
            && U_SUCCESS(subStatus)) {
            स्थिर अक्षर *k;
            int32_t i;
            k = ures_get_कुंजी(subPtr);

#अगर defined(URES_वृक्ष_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#पूर्ण_अगर
            अगर(k == nullptr || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_TAG) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                जारी;
            }
            क्रम(i=0; i<मूल्यावली_गिनती; i++) {
                अगर(!uprv_strcmp(मूल्यावली_List[i],k)) {
                    k = nullptr; /* found duplicate */
                    अवरोध;
                }
            }
            अगर(k != nullptr) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                अगर((मूल्यावली_गिनती >= (मूल्यावली_LIST_आकार-1)) ||       /* no more space in list .. */
                    ((मूल्यावली_सूचकांक+kLen+1+1) >= मूल्यावली_BUF_आकार)) { /* no more space in buffer (string + 2 nulls) */
                    *status = U_ILLEGAL_ARGUMENT_ERROR; /* out of space.. */
                } अन्यथा {
                    uprv_strcpy(मूल्यावली_Buf+मूल्यावली_सूचकांक, k);
                    मूल्यावली_List[मूल्यावली_गिनती++] = मूल्यावली_Buf+मूल्यावली_सूचकांक;
                    मूल्यावली_सूचकांक += kLen;
#अगर defined(URES_वृक्ष_DEBUG)
                    ख_लिखो(मानक_त्रुटि, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        path?path:"<ICUDATA>", keyword, locale, k);
#पूर्ण_अगर
                    मूल्यावली_Buf[मूल्यावली_सूचकांक++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    मूल्यावली_Buf[मूल्यावली_सूचकांक++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#अगर defined(URES_वृक्ष_DEBUG)
    ख_लिखो(मानक_त्रुटि, "%s:  size %d, #%d\n", u_errorName(*status), 
        मूल्यावली_सूचकांक, मूल्यावली_गिनती);
#पूर्ण_अगर
    वापस uloc_openKeywordList(मूल्यावली_Buf, मूल्यावली_सूचकांक, status);
}
#अगर 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(स्थिर UResourceBundle* res1, स्थिर UResourceBundle* res2){
    अगर(res1==nullptr || res2==nullptr){
        वापस res1==res2; /* pointer comparison */
    }
    अगर(res1->f_कुंजी==nullptr||  res2->f_कुंजी==nullptr){
        वापस (res1->f_कुंजी==res2->f_कुंजी);
    }अन्यथा{
        अगर(uprv_strcmp(res1->f_कुंजी, res2->f_कुंजी)!=0){
            वापस false;
        }
    }
    अगर(uprv_strcmp(res1->f_तथ्य->fName, res2->f_तथ्य->fName)!=0){
        वापस false;
    }
    अगर(res1->f_तथ्य->fPath == nullptr||  res2->f_तथ्य->fPath==nullptr){
        वापस (res1->f_तथ्य->fPath == res2->f_तथ्य->fPath);
    }अन्यथा{
        अगर(uprv_strcmp(res1->f_तथ्य->fPath, res2->f_तथ्य->fPath)!=0){
            वापस false;
        }
    }
    अगर(uprv_strcmp(res1->f_तथ्य->fParent->fName, res2->f_तथ्य->fParent->fName)!=0){
        वापस false;
    }
    अगर(uprv_strcmp(res1->f_तथ्य->fParent->fPath, res2->f_तथ्य->fParent->fPath)!=0){
        वापस false;
    }
    अगर(uprv_strncmp(res1->fResPath, res2->fResPath, res1->fResPathLen)!=0){
        वापस false;
    }
    अगर(res1->fRes != res2->fRes){
        वापस false;
    }
    वापस true;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(स्थिर UResourceBundle* res, UErrorCode* status){
    UResourceBundle* bundle = nullptr;
    UResourceBundle* ret = nullptr;
    अगर(U_FAILURE(*status) || res == nullptr){
        वापस nullptr;
    }
    bundle = ures_open(res->f_तथ्य->fPath, res->f_तथ्य->fName, status);
    अगर(res->fResPath!=nullptr){
        ret = ures_findSubResource(bundle, res->fResPath, nullptr, status);
        ures_close(bundle);
    }अन्यथा{
        ret = bundle;
    }
    वापस ret;
}
U_CAPI स्थिर UResourceBundle* U_EXPORT2
ures_getParentBundle(स्थिर UResourceBundle* res){
    अगर(res==nullptr){
        वापस nullptr;
    }
    वापस res->fParentRes;
}
#पूर्ण_अगर

U_CAPI व्योम U_EXPORT2
ures_get_Version_By_कुंजी(स्थिर UResourceBundle* res, स्थिर अक्षर *कुंजी, UVersionInfo ver, UErrorCode *status) {
  स्थिर char16_t *str;
  int32_t len;
  str = ures_get_String_By_कुंजी(res, कुंजी, &len, status);
  अगर(U_SUCCESS(*status)) {
    u_versionFromUString(ver, str);
  } 
}

/* eof */
