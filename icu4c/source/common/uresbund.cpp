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

उपयुज्य नामाकाश icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
स्थिर UHashtable *cache = शून्यसूचकः;
स्थिर icu::UInitOnce gCacheInitOnce {};

स्थिर UMutex resbMutex;

/* INTERNAL: hashes an entry  */
स्थिर int32_t U_CALLCONV hash_प्रविष्टिः(ध्रुव UHashTok parm) {
    UResource_दत्त_प्रविष्टिः* b = static_cast<UResource_दत्त_प्रविष्टिः*>(parm.pointer);
    UHashTok namekey, pathkey;
    namekey.pointer = b->fName;
    pathkey.pointer = b->f_मार्गः;
    प्रत्यागम uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
स्थिर UBool U_CALLCONV compareEntries(ध्रुव UHashTok p1, ध्रुव UHashTok p2) {
    UResource_दत्त_प्रविष्टिः* b1 = static_cast<UResource_दत्त_प्रविष्टिः*>(p1.pointer);
    UResource_दत्त_प्रविष्टिः* b2 = static_cast<UResource_दत्त_प्रविष्टिः*>(p2.pointer);
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->fName;
    name2.pointer = b2->fName;
    path1.pointer = b1->f_मार्गः;
    path2.pointer = b2->f_मार्गः;
    प्रत्यागम uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
स्थिर UBool chopLocale(वर्ण *name) {
    वर्ण *i = uprv_strrchr(name, '_');

    यदि(i != शून्यसूचकः) {
        *i = '\0';
        प्रत्यागम सत्य;
    }

    प्रत्यागम असत्य;
}

स्थिर UBool hasVariant(ध्रुव वर्ण* localeID) {
    UError_संहिता err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(शून्यसूचकः, 0);
    ulocimp_getSubtags(
            localeID,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            &sink,
            शून्यसूचकः,
            err);
    प्रत्यागम sink.सङ्ख्या_सम्बन्धि_अष्टकानि_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#परिभाषय INCLUDED_FROM_URESBUND_CPP
#अन्तर्भाव "localefallback_data.h"

स्थिर ध्रुव वर्ण* performFallbackLookup(ध्रुव वर्ण* कुञ्चिका,
                                         ध्रुव वर्ण* कुञ्चिका_Strs,
                                         ध्रुव वर्ण* मानम्_Strs U_LIFETIME_BOUND,
                                         ध्रुव int32_t* lookup_सारणी,
                                         int32_t lookup_सारणी_दीर्घता) {
    ध्रुव int32_t* bottom = lookup_सारणी;
    ध्रुव int32_t* शिखरम् = lookup_सारणी + lookup_सारणी_दीर्घता;

    यावत् (bottom < शिखरम्) {
        // Effectively, divide by 2 and round down to an even index
        ध्रुव int32_t* मध्यः = bottom + (((शिखरम् - bottom) / 4) * 2);
        ध्रुव वर्ण* प्रविष्टिः_कुञ्चिका = &(कुञ्चिका_Strs[*मध्यः]);
        int32_t strcmp_परिणाम = uprv_strcmp(कुञ्चिका, प्रविष्टिः_कुञ्चिका);
        यदि (strcmp_परिणाम == 0) {
            प्रत्यागम &(मानम्_Strs[मध्यः[1]]);
        } अन्यथा यदि (strcmp_परिणाम < 0) {
            शिखरम् = मध्यः;
        } अन्यथा {
            bottom = मध्यः + 2;
        }
    }
    प्रत्यागम शून्यसूचकः;
}

स्थिर CharString getDefaultScript(ध्रुव CharString& language, ध्रुव CharString& region) {
    ध्रुव वर्ण* defaultScript = शून्यसूचकः;
    UError_संहिता err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString परिणाम("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    यदि (!region.is_रिक्तानि()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        यदि (U_FAILURE(err)) {
            प्रत्यागम परिणाम;
        }
        defaultScript = performFallbackLookup(localeID.दत्त(), dsLocaleIDChars, script_संहिता_Chars, default_Script_सारणी, UPRV_LENGTHOF(default_Script_सारणी));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    यदि (defaultScript == शून्यसूचकः) {
        defaultScript = performFallbackLookup(language.दत्त(), dsLocaleIDChars, script_संहिता_Chars, default_Script_सारणी, UPRV_LENGTHOF(default_Script_सारणी));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    यदि (defaultScript != शून्यसूचकः) {
        परिणाम.clear();
        परिणाम.append(defaultScript, err);
    }
    प्रत्यागम परिणाम;
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
    URES_OPEN_प्रत्यक्षम्
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
    यदि (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        प्रत्यागम chopLocale(name);
    }
    
    UError_संहिता err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, शून्यसूचकः, शून्यसूचकः, err);

    यदि (U_FAILURE(err)) {
        // hopefully this never happens...
        प्रत्यागम chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    यदि (openType == URES_OPEN_LOCALE_DEFAULT_मूलम्) {
        ध्रुव वर्ण* parentID = performFallbackLookup(name, parentLocaleChars, parentLocaleChars, parent_Locale_सारणी, UPRV_LENGTHOF(parent_Locale_सारणी));
        यदि (parentID != शून्यसूचकः) {
            uprv_strcpy(name, parentID);
            प्रत्यागम सत्य;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    यदि (!script.is_रिक्तानि() && !region.is_रिक्तानि()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        यदि (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } अन्यथा {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } अन्यथा यदि (!region.is_रिक्तानि()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_संहिता err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, err);
        यदि (!origNameScript.is_रिक्तानि()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } अन्यथा {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } अन्यथा यदि (!script.is_रिक्तानि()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        यदि (openType != URES_OPEN_LOCALE_DEFAULT_मूलम् || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    } अन्यथा {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        प्रत्यागम असत्य;
    }
    यदि (U_SUCCESS(err) && !workingLocale.is_रिक्तानि()) {
        uprv_strcpy(name, workingLocale.दत्त());
        प्रत्यागम सत्य;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
स्थिर UBool mayHaveParent(वर्ण *name) {
    प्रत्यागम (name[0] != 0 && uprv_strstr("nb nn",name) != शून्यसूचकः);
}

/**
 *  Internal function
 */
स्थिर शून्य प्रविष्टिः_Increase(UResource_दत्त_प्रविष्टिः *प्रविष्टिः) {
    Mutex तालकम्(&resbMutex);
    प्रविष्टिः->f_गणनम्_Existing++;
    यावत्(प्रविष्टिः->fParent != शून्यसूचकः) {
      प्रविष्टिः = प्रविष्टिः->fParent;
      प्रविष्टिः->f_गणनम्_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
स्थिर UResource_दत्त_प्रविष्टिः *get_Fallback_दत्त(
        ध्रुव UResourceBundle *resBundle,
        ध्रुव वर्ण **res_चिह्नकम्, Resource *res, UError_संहिता *स्थितिः) {
    UResource_दत्त_प्रविष्टिः *दत्त_प्रविष्टिः = resBundle->f_दत्त;
    int32_t सूचकांक_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    यदि(दत्त_प्रविष्टिः == शून्यसूचकः) {
        *स्थितिः = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    यदि(दत्त_प्रविष्टिः->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_सारणी_Item_By_कुञ्चिका(&(दत्त_प्रविष्टिः->f_दत्त), दत्त_प्रविष्टिः->f_दत्त.मूलम्_Res, &सूचकांक_R, res_चिह्नकम्); /* try to get data from there */
        i++;
    }
    यदि(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        यावत्(*res == RES_BOGUS && दत्त_प्रविष्टिः->fParent != शून्यसूचकः) {
            दत्त_प्रविष्टिः = दत्त_प्रविष्टिः->fParent;
            यदि(दत्त_प्रविष्टिः->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_सारणी_Item_By_कुञ्चिका(&(दत्त_प्रविष्टिः->f_दत्त), दत्त_प्रविष्टिः->f_दत्त.मूलम्_Res, &सूचकांक_R, res_चिह्नकम्);
            }
        }
    }

    यदि(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *स्थितिः = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    // If the resource is found in parents, we need to adjust the error.
    यदि(i>1) {
        यदि(uprv_strcmp(दत्त_प्रविष्टिः->fName, uloc_getDefault())==0 || uprv_strcmp(दत्त_प्रविष्टिः->fName, k_मूलम्_Locale_Name)==0) {
            *स्थितिः = U_USING_DEFAULT_WARNING;
        } अन्यथा {
            *स्थितिः = U_USING_FALLBACK_WARNING;
        }
    }
    प्रत्यागम दत्त_प्रविष्टिः;
}

स्थिर शून्य
free_प्रविष्टिः(UResource_दत्त_प्रविष्टिः *प्रविष्टिः) {
    UResource_दत्त_प्रविष्टिः *alias;
    res_unload(&(प्रविष्टिः->f_दत्त));
    यदि(प्रविष्टिः->fName != शून्यसूचकः && प्रविष्टिः->fName != प्रविष्टिः->f_Name_बफर) {
        uprv_free(प्रविष्टिः->fName);
    }
    यदि(प्रविष्टिः->f_मार्गः != शून्यसूचकः) {
        uprv_free(प्रविष्टिः->f_मार्गः);
    }
    यदि(प्रविष्टिः->fPool != शून्यसूचकः) {
        --प्रविष्टिः->fPool->f_गणनम्_Existing;
    }
    alias = प्रविष्टिः->fAlias;
    यदि(alias != शून्यसूचकः) {
        यावत्(alias->fAlias != शून्यसूचकः) {
            alias = alias->fAlias;
        }
        --alias->f_गणनम्_Existing;
    }
    uprv_free(प्रविष्टिः);
}

/* Works just like ucnv_flushCache() */
स्थिर int32_t ures_flushCache()
{
    UResource_दत्त_प्रविष्टिः *resB;
    int32_t pos;
    int32_t rb_Deleted_अङ्कः = 0;
    ध्रुव UHashElement *e;
    UBool deleted_अधिकम्;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex तालकम्(&resbMutex);
    यदि (cache == शून्यसूचकः) {
        प्रत्यागम 0;
    }

    कुरु {
        deleted_अधिकम् = असत्य;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_FIRST;
        यावत् ((e = uhash_अग्र_Element(cache, &pos)) != शून्यसूचकः)
        {
            resB = static_cast<UResource_दत्त_प्रविष्टिः*>(e->value.pointer);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            यदि (resB->f_गणनम्_Existing == 0) {
                rb_Deleted_अङ्कः++;
                deleted_अधिकम् = सत्य;
                uhash_removeElement(cache, e);
                free_प्रविष्टिः(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } यावत्(deleted_अधिकम्);

    प्रत्यागम rb_Deleted_अङ्कः;
}

#यदि_परिभाषितम् URES_DEBUG
#अन्तर्भाव <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_रिक्तानि = असत्य;
  int32_t pos = UHASH_FIRST;
  ध्रुव UHashElement *e;
  UResource_दत्त_प्रविष्टिः *resB;
  
    Mutex तालकम्(&resbMutex);
    यदि (cache == शून्यसूचकः) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      प्रत्यागम असत्य;
    }

    यावत् ((e = uhash_अग्र_Element(cache, &pos)) != शून्यसूचकः) {
      cache_Not_रिक्तानि=सत्य;
      resB = (UResource_दत्त_प्रविष्टिः *) e->value.pointer;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (शून्य*)resB, resB->f_गणनम्_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_मार्गः?resB->f_मार्गः:"nullptr",
              (शून्य*)resB->fPool,
              (शून्य*)resB->fAlias,
              (शून्य*)resB->fParent);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_गणनम्(cache));
    प्रत्यागम cache_Not_रिक्तानि;
}

#यदि_अन्तः

स्थिर UBool U_CALLCONV ures_cleanup()
{
    यदि (cache != शून्यसूचकः) {
        ures_flushCache();
        uhash_close(cache);
        cache = शून्यसूचकः;
    }
    gCacheInitOnce.reset();
    प्रत्यागम सत्य;
}

/** INTERNAL: Initializes the cache for resources */
स्थिर शून्य U_CALLCONV createCache(UError_संहिता &स्थितिः) {
    U_ASSERT(cache == शून्यसूचकः);
    cache = uhash_open(hash_प्रविष्टिः, compareEntries, शून्यसूचकः, &स्थितिः);
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
स्थिर शून्य initCache(UError_संहिता *स्थितिः) {
    umtx_initOnce(gCacheInitOnce, &createCache, *स्थितिः);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

स्थिर शून्य समुच्चयः_प्रविष्टिः_Name(UResource_दत्त_प्रविष्टिः *res, ध्रुव वर्ण *name, UError_संहिता *स्थितिः) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    यदि(res->fName != शून्यसूचकः && res->fName != res->f_Name_बफर) {
        uprv_free(res->fName);
    }
    यदि (len < static_cast<int32_t>(परिमाण(res->f_Name_बफर))) {
        res->fName = res->f_Name_बफर;
    }
    अन्यथा {
        res->fName = static_cast<वर्ण*>(uprv_malloc(len + 1));
    }
    यदि(res->fName == शून्यसूचकः) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा {
        uprv_strcpy(res->fName, name);
    }
}

स्थिर UResource_दत्त_प्रविष्टिः *
get_Pool_प्रविष्टिः(ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
स्थिर UResource_दत्त_प्रविष्टिः *init_प्रविष्टिः(ध्रुव वर्ण *localeID, ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः) {
    UResource_दत्त_प्रविष्टिः *r = शून्यसूचकः;
    UResource_दत्त_प्रविष्टिः find;
    /*int32_t hashValue;*/
    ध्रुव वर्ण *name;
    वर्ण aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    /* here we try to deduce the right locale name */
    यदि(localeID == शून्यसूचकः) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } अन्यथा यदि(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_मूलम्_Locale_Name;
    } अन्यथा { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<वर्ण*>(name);
    find.f_मार्गः = const_cast<वर्ण*>(मार्गः);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_दत्त_प्रविष्टिः*>(uhash_get(cache, &find));
    यदि(r == शून्यसूचकः) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_दत्त_प्रविष्टिः*>(uprv_malloc(परिमाण(UResource_दत्त_प्रविष्टिः)));
        यदि(r == शून्यसूचकः) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }

        uprv_memset(r, 0, परिमाण(UResource_दत्त_प्रविष्टिः));
        /*r->fHashKey = hashValue;*/

        समुच्चयः_प्रविष्टिः_Name(r, name, स्थितिः);
        यदि (U_FAILURE(*स्थितिः)) {
            uprv_free(r);
            प्रत्यागम शून्यसूचकः;
        }

        यदि(मार्गः != शून्यसूचकः) {
            r->f_मार्गः = uprv_strdup(मार्गः);
            यदि(r->f_मार्गः == शून्यसूचकः) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                uprv_free(r);
                प्रत्यागम शून्यसूचकः;
            }
        }

        /* this is the actual loading */
        res_आरोपय(&(r->f_दत्त), r->f_मार्गः, r->fName, स्थितिः);

        यदि (U_FAILURE(*स्थितिः)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            यदि (*स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
                uprv_free(r);
                प्रत्यागम शून्यसूचकः;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *स्थितिः = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } अन्यथा { /* if we have a regular entry */
            Resource aliasres;
            यदि (r->f_दत्त.उपयुङ्क्ते_Pool_Bundle) {
                r->fPool = get_Pool_प्रविष्टिः(r->f_मार्गः, स्थितिः);
                यदि (U_SUCCESS(*स्थितिः)) {
                    ध्रुव int32_t *poolIndexes = r->fPool->f_दत्त.p_मूलम् + 1;
                    यदि(r->f_दत्त.p_मूलम्[1 + URES_सूचकांक_POOL_CHECKSUM] == poolIndexes[URES_सूचकांक_POOL_CHECKSUM]) {
                        r->f_दत्त.pool_Bundle_कुञ्चिकाः = reinterpret_cast<ध्रुव वर्ण*>(poolIndexes + (poolIndexes[URES_सूचकांक_दीर्घता] & 0xff));
                        r->f_दत्त.pool_Bundle_वाक्यानि = r->fPool->f_दत्त.p16BitUnits;
                    } अन्यथा {
                        r->fBogus = *स्थितिः = U_INVALID_FORMAT_ERROR;
                    }
                } अन्यथा {
                    r->fBogus = *स्थितिः;
                }
            }
            यदि (U_SUCCESS(*स्थितिः)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_दत्त), "%%ALIAS");
                यदि (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ध्रुव char16_t *alias = res_getStringNoTrace(&(r->f_दत्त), aliasres, &aliasLen);
                    यदि(alias != शून्यसूचकः && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_गन्तव्यम्_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_प्रविष्टिः(aliasName, मार्गः, स्थितिः);
                    }
                }
            }
        }

        {
            UResource_दत्त_प्रविष्टिः *oldR = शून्यसूचकः;
            यदि ((oldR = static_cast<UResource_दत्त_प्रविष्टिः*>(uhash_get(cache, r))) == शून्यसूचकः) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_संहिता cache_स्थितिः = U_ZERO_ERROR;
                uhash_put(cache, (शून्य *)r, r, &cache_स्थितिः);
                यदि (U_FAILURE(cache_स्थितिः)) {
                    *स्थितिः = cache_स्थितिः;
                    free_प्रविष्टिः(r);
                    r = शून्यसूचकः;
                }
            } अन्यथा {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_प्रविष्टिः(r);
                r = oldR;
            }
        }

    }
    यदि(r != शून्यसूचकः) {
        /* return the real bundle */
        यावत्(r->fAlias != शून्यसूचकः) {
            r = r->fAlias;
        }
        r->f_गणनम्_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        यदि(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*स्थितिः)) {
             *स्थितिः = r->fBogus; /* set the returning status */
        }
    }
    प्रत्यागम r;
}

स्थिर UResource_दत्त_प्रविष्टिः *
get_Pool_प्रविष्टिः(ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः) {
    UResource_दत्त_प्रविष्टिः *poolBundle = init_प्रविष्टिः(kPoolBundleName, मार्गः, स्थितिः);
    यदि( U_SUCCESS(*स्थितिः) &&
        (poolBundle == शून्यसूचकः || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_दत्त.isPoolBundle)
    ) {
        *स्थितिः = U_INVALID_FORMAT_ERROR;
    }
    प्रत्यागम poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
स्थिर UResource_दत्त_प्रविष्टिः *
findFirstExisting(ध्रुव वर्ण* मार्गः, वर्ण* name, ध्रुव वर्ण* defaultLocale, UResOpenType openType,
                  UBool *is_मूलम्, UBool *प्राप्तम्_Parent, UBool *isDefault, UError_संहिता* स्थितिः) {
    UResource_दत्त_प्रविष्टिः *r = शून्यसूचकः;
    UBool has_Real_दत्त = असत्य;
    *प्राप्तम्_Parent = सत्य; /* we're starting with a fresh name */
    वर्ण origName[ULOC_FULLNAME_क्षमता];

    uprv_strcpy(origName, name);
    यावत्(*प्राप्तम्_Parent && !has_Real_दत्त) {
        r = init_प्रविष्टिः(name, मार्गः, स्थितिः);
        /* Null pointer test */
        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम शून्यसूचकः;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_दत्त = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        यदि(!has_Real_दत्त) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_गणनम्_Existing--;
            /*entryCloseInt(r);*/
            r = शून्यसूचकः;
            *स्थितिः = U_USING_FALLBACK_WARNING;
        } अन्यथा {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_मूलम् = static_cast<UBool>(uprv_strcmp(name, k_मूलम्_Locale_Name) == 0);

        /*Fallback data stuff*/
        यदि (!has_Real_दत्त) {
            *प्राप्तम्_Parent = getParentLocaleID(name, origName, openType);
        } अन्यथा {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *प्राप्तम्_Parent = chopLocale(name);
        }
        यदि (*प्राप्तम्_Parent && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    प्रत्यागम r;
}

स्थिर शून्य ures_समुच्चयः_Is_राशिः_Object( UResourceBundle* resB, UBool state) {
    यदि(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } अन्यथा {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

स्थिर UBool ures_is_राशिः_Object(ध्रुव UResourceBundle* resB) {
  प्रत्यागम((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?असत्य:सत्य);
}


U_CFUNC शून्य ures_init_राशिः_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, परिमाण(UResourceBundle));
  ures_समुच्चयः_Is_राशिः_Object(resB, सत्य);
}

U_NAMESPACE_BEGIN

राशिः_UResource_Bundle::राशिः_UResource_Bundle() {
    ures_init_राशिः_Object(&bundle);
}

राशिः_UResource_Bundle::~राशिः_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_अन्तः

स्थिर UBool  // returns U_SUCCESS(*status)
आरोपय_Parents_Except_मूलम्(UResource_दत्त_प्रविष्टिः *&t1,
                      वर्ण name[], int32_t name_क्षमता,
                      UBool usingUSRData, वर्ण usr_दत्त_मार्गः[], UError_संहिता *स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) { प्रत्यागम असत्य; }
    UBool checkParent = सत्य;
    यावत् (checkParent && t1->fParent == शून्यसूचकः && !t1->f_दत्त.noFallback &&
            res_getResource(&t1->f_दत्त,"%%ParentIsRoot") == RES_BOGUS) {
        Resource parentRes = res_getResource(&t1->f_दत्त, "%%Parent");
        यदि (parentRes != RES_BOGUS) {  // An explicit parent was found.
            int32_t parentLocaleLen = 0;
            // No tracing: called during initial data loading
            ध्रुव char16_t *parentLocaleName = res_getStringNoTrace(&(t1->f_दत्त), parentRes, &parentLocaleLen);
            यदि(parentLocaleName != शून्यसूचकः && 0 < parentLocaleLen && parentLocaleLen < name_क्षमता) {
                u_UChars_गन्तव्यम्_Chars(parentLocaleName, name, parentLocaleLen + 1);
                यदि (uprv_strcmp(name, k_मूलम्_Locale_Name) == 0) {
                    प्रत्यागम सत्य;
                }
            }
        }
        // Insert regular parents.
        UError_संहिता parent_स्थितिः = U_ZERO_ERROR;
        UResource_दत्त_प्रविष्टिः *t2 = init_प्रविष्टिः(name, t1->f_मार्गः, &parent_स्थितिः);
        यदि (U_FAILURE(parent_स्थितिः)) {
            *स्थितिः = parent_स्थितिः;
            प्रत्यागम असत्य;
        }
        UResource_दत्त_प्रविष्टिः *u2 = शून्यसूचकः;
        UError_संहिता usr_स्थितिः = U_ZERO_ERROR;
        यदि (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_प्रविष्टिः(name, usr_दत्त_मार्गः, &usr_स्थितिः);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            यदि (usr_स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
                *स्थितिः = usr_स्थितिः;
                प्रत्यागम असत्य;
            }
        }

        यदि (usingUSRData && U_SUCCESS(usr_स्थितिः) && u2->fBogus == U_ZERO_ERROR) {
            t1->fParent = u2;
            u2->fParent = t2;
        } अन्यथा {
            t1->fParent = t2;
            यदि (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_गणनम्_Existing = 0;
            }
        }
        t1 = t2;
        checkParent = chopLocale(name) || mayHaveParent(name);
    }
    प्रत्यागम सत्य;
}

स्थिर UBool  // returns U_SUCCESS(*status)
निवेशय_मूलम्_Bundle(UResource_दत्त_प्रविष्टिः *&t1, UError_संहिता *स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) { प्रत्यागम असत्य; }
    UError_संहिता parent_स्थितिः = U_ZERO_ERROR;
    UResource_दत्त_प्रविष्टिः *t2 = init_प्रविष्टिः(k_मूलम्_Locale_Name, t1->f_मार्गः, &parent_स्थितिः);
    यदि (U_FAILURE(parent_स्थितिः)) {
        *स्थितिः = parent_स्थितिः;
        प्रत्यागम असत्य;
    }
    t1->fParent = t2;
    t1 = t2;
    प्रत्यागम सत्य;
}

स्थिर UResource_दत्त_प्रविष्टिः *प्रविष्टिः_Open(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* localeID,
                                     UResOpenType openType, UError_संहिता* स्थितिः) {
    U_ASSERT(openType != URES_OPEN_प्रत्यक्षम्);
    UError_संहिता int_स्थितिः = U_ZERO_ERROR;
    UResource_दत्त_प्रविष्टिः *r = शून्यसूचकः;
    UResource_दत्त_प्रविष्टिः *t1 = शून्यसूचकः;
    UBool isDefault = असत्य;
    UBool is_मूलम् = असत्य;
    UBool has_Real_दत्त = असत्य;
    UBool hasChopped = सत्य;
    UBool usingUSRData = U_USE_USRDATA && ( मार्गः == शून्यसूचकः || uprv_strncmp(मार्गः,U_ICUDATA_NAME,8) == 0);

    वर्ण name[ULOC_FULLNAME_क्षमता];
    वर्ण usr_दत्त_मार्गः[96];

    initCache(स्थितिः);

    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    uprv_strncpy(name, localeID, परिमाण(name) - 1);
    name[परिमाण(name) - 1] = 0;

    यदि ( usingUSRData ) {
        यदि ( मार्गः == शून्यसूचकः ) {
            uprv_strcpy(usr_दत्त_मार्गः, U_USRDATA_NAME);
        } अन्यथा {
            uprv_strncpy(usr_दत्त_मार्गः, मार्गः, परिमाण(usr_दत्त_मार्गः) - 1);
            usr_दत्त_मार्गः[0] = 'u';
            usr_दत्त_मार्गः[1] = 's';
            usr_दत्त_मार्गः[2] = 'r';
            usr_दत्त_मार्गः[परिमाण(usr_दत्त_मार्गः) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ध्रुव वर्ण *defaultLocale = uloc_getDefault();

    Mutex तालकम्(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = findFirstExisting(मार्गः, name, defaultLocale, openType, &is_मूलम्, &hasChopped, &isDefault, &int_स्थितिः);

    // If we failed due to out-of-memory, report the failure and exit early.
    यदि (int_स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
        *स्थितिः = int_स्थितिः;
        गच्छ finish;
    }

    यदि(r != शून्यसूचकः) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_दत्त = सत्य;
        यदि ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_संहिता usr_स्थितिः = U_ZERO_ERROR;
            UResource_दत्त_प्रविष्टिः *u1 = init_प्रविष्टिः(t1->fName, usr_दत्त_मार्गः, &usr_स्थितिः);
            // If we failed due to out-of-memory, report the failure and exit early.
            यदि (int_स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
                *स्थितिः = int_स्थितिः;
                गच्छ finish;
            }
            यदि ( u1 != शून्यसूचकः ) {
                यदि(u1->fBogus == U_ZERO_ERROR) {
                    u1->fParent = t1;
                    r = u1;
                } अन्यथा {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_गणनम्_Existing = 0;
                }
            }
        }
        यदि ((hasChopped || mayHaveParent(name)) && !is_मूलम्) {
            यदि (!आरोपय_Parents_Except_मूलम्(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_दत्त_मार्गः, स्थितिः)) {
                गच्छ finish;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    यदि(r==शून्यसूचकः && openType == URES_OPEN_LOCALE_DEFAULT_मूलम् && !isDefault && !is_मूलम्) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = findFirstExisting(मार्गः, name, defaultLocale, openType, &is_मूलम्, &hasChopped, &isDefault, &int_स्थितिः);
        // If we failed due to out-of-memory, report the failure and exit early.
        यदि (int_स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
            *स्थितिः = int_स्थितिः;
            गच्छ finish;
        }
        int_स्थितिः = U_USING_DEFAULT_WARNING;
        यदि(r != शून्यसूचकः) { /* the default locale exists */
            t1 = r;
            has_Real_दत्त = सत्य;
            isDefault = सत्य;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            यदि ((hasChopped || mayHaveParent(name)) && !is_मूलम्) {
                यदि (!आरोपय_Parents_Except_मूलम्(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_दत्त_मार्गः, स्थितिः)) {
                    गच्छ finish;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    यदि(r == शून्यसूचकः) {
        uprv_strcpy(name, k_मूलम्_Locale_Name);
        r = findFirstExisting(मार्गः, name, defaultLocale, openType, &is_मूलम्, &hasChopped, &isDefault, &int_स्थितिः);
        // If we failed due to out-of-memory, report the failure and exit early.
        यदि (int_स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
            *स्थितिः = int_स्थितिः;
            गच्छ finish;
        }
        यदि(r != शून्यसूचकः) {
            t1 = r;
            int_स्थितिः = U_USING_DEFAULT_WARNING;
            has_Real_दत्त = सत्य;
        } अन्यथा { /* we don't even have the root locale */
            *स्थितिः = U_MISSING_RESOURCE_ERROR;
            गच्छ finish;
        }
    } अन्यथा यदि(!is_मूलम् && uprv_strcmp(t1->fName, k_मूलम्_Locale_Name) != 0 &&
            t1->fParent == शून्यसूचकः && !r->f_दत्त.noFallback) {
        यदि (!निवेशय_मूलम्_Bundle(t1, स्थितिः)) {
            गच्छ finish;
        }
        यदि(!has_Real_दत्त) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    यावत्(r != शून्यसूचकः && !is_मूलम् && t1->fParent != शून्यसूचकः) {
        t1->fParent->f_गणनम्_Existing++;
        t1 = t1->fParent;
    }

finish:
    यदि(U_SUCCESS(*स्थितिः)) {
        यदि(int_स्थितिः != U_ZERO_ERROR) {
            *स्थितिः = int_स्थितिः;  
        }
        प्रत्यागम r;
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
स्थिर UResource_दत्त_प्रविष्टिः *
प्रविष्टिः_Open_प्रत्यक्षम्(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* localeID, UError_संहिता* स्थितिः) {
    initCache(स्थितिः);
    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    } अन्यथा यदि (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_मूलम्_Locale_Name;
    }

    Mutex तालकम्(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_दत्त_प्रविष्टिः *r = init_प्रविष्टिः(localeID, मार्गः, स्थितिः);
    यदि(U_SUCCESS(*स्थितिः)) {
        यदि(r->fBogus != U_ZERO_ERROR) {
            r->f_गणनम्_Existing--;
            r = शून्यसूचकः;
        }
    } अन्यथा {
        r = शून्यसूचकः;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_दत्त_प्रविष्टिः *t1 = r;
    यदि(r != शून्यसूचकः && uprv_strcmp(localeID, k_मूलम्_Locale_Name) != 0 &&  // not root
            r->fParent == शून्यसूचकः && !r->f_दत्त.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_क्षमता) {
        वर्ण name[ULOC_FULLNAME_क्षमता];
        uprv_strcpy(name, localeID);
        यदि(!chopLocale(name) || uprv_strcmp(name, k_मूलम्_Locale_Name) == 0 ||
                आरोपय_Parents_Except_मूलम्(t1, name, UPRV_LENGTHOF(name), असत्य, शून्यसूचकः, स्थितिः)) {
            यदि(uprv_strcmp(t1->fName, k_मूलम्_Locale_Name) != 0 && t1->fParent == शून्यसूचकः) {
                निवेशय_मूलम्_Bundle(t1, स्थितिः);
            }
        }
        यदि(U_FAILURE(*स्थितिः)) {
            r = शून्यसूचकः;
        }
    }

    यदि(r != शून्यसूचकः) {
        // TODO: Does this ever loop?
        यावत्(t1->fParent != शून्यसूचकः) {
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
स्थिर शून्य प्रविष्टिः_Close_Int(UResource_दत्त_प्रविष्टिः *resB) {
    UResource_दत्त_प्रविष्टिः *p = resB;

    यावत्(resB != शून्यसूचकः) {
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

स्थिर शून्य प्रविष्टिः_Close(UResource_दत्त_प्रविष्टिः *resB) {
  Mutex तालकम्(&resbMutex);
  प्रविष्टिः_Close_Int(resB);
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
स्थिर शून्य ures_append_Res_मार्गः(UResourceBundle *resB, ध्रुव वर्ण* गन्तव्यम्_सङ्कलय, int32_t len_गन्तव्यम्_सङ्कलय, UError_संहिता *स्थितिः) {
    int32_t res_मार्गः_Len_Orig = resB->f_Res_मार्गः_Len;
    यदि(resB->f_Res_मार्गः == शून्यसूचकः) {
        resB->f_Res_मार्गः = resB->fResBuf;
        *(resB->f_Res_मार्गः) = 0;
        resB->f_Res_मार्गः_Len = 0;
    } 
    resB->f_Res_मार्गः_Len += len_गन्तव्यम्_सङ्कलय;
    यदि(RES_BUFSIZE <= resB->f_Res_मार्गः_Len+1) {
        यदि(resB->f_Res_मार्गः == resB->fResBuf) {
            resB->f_Res_मार्गः = static_cast<वर्ण*>(uprv_malloc((resB->f_Res_मार्गः_Len + 1) * परिमाण(वर्ण)));
            /* Check that memory was allocated correctly. */
            यदि (resB->f_Res_मार्गः == शून्यसूचकः) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            uprv_strcpy(resB->f_Res_मार्गः, resB->fResBuf);
        } अन्यथा {
            वर्ण* अस्थायी = static_cast<वर्ण*>(uprv_realloc(resB->f_Res_मार्गः, (resB->f_Res_मार्गः_Len + 1) * परिमाण(वर्ण)));
            /* Check that memory was reallocated correctly. */
            यदि (अस्थायी == शून्यसूचकः) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            resB->f_Res_मार्गः = अस्थायी;
        }
    }
    uprv_strcpy(resB->f_Res_मार्गः + res_मार्गः_Len_Orig, गन्तव्यम्_सङ्कलय);
}

स्थिर शून्य ures_free_Res_मार्गः(UResourceBundle *resB) {
    यदि (resB->f_Res_मार्गः && resB->f_Res_मार्गः != resB->fResBuf) {
        uprv_free(resB->f_Res_मार्गः);
    }
    resB->f_Res_मार्गः = शून्यसूचकः;
    resB->f_Res_मार्गः_Len = 0;
}

स्थिर शून्य
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    यदि(resB != शून्यसूचकः) {
        यदि(resB->f_दत्त != शून्यसूचकः) {
            प्रविष्टिः_Close(resB->f_दत्त);
        }
        यदि(resB->f_संस्करणम् != शून्यसूचकः) {
            uprv_free(resB->f_संस्करणम्);
        }
        ures_free_Res_मार्गः(resB);

        यदि(ures_is_राशिः_Object(resB) == असत्य && freeBundleObj) {
            uprv_free(resB);
        }
#यदि 0 /*U_DEBUG*/
        अन्यथा {
            /* poison the data */
            uprv_memset(resB, -1, परिमाण(UResourceBundle));
        }
#यदि_अन्तः
    }
}

U_CAPI शून्य  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, सत्य);
}

नामाकाश {

UResourceBundle* init_resb_परिणाम(UResource_दत्त_प्रविष्टिः* दत्त_प्रविष्टिः,
                                  Resource r,
                                  ध्रुव वर्ण* कुञ्चिका,
                                  int32_t idx,
                                  UResource_दत्त_प्रविष्टिः* valid_Locale_दत्त_प्रविष्टिः,
                                  ध्रुव वर्ण* container_Res_मार्गः,
                                  int32_t recursion_गभीरता,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_संहिता* स्थितिः);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
getAliasTargetAsResourceBundle(ध्रुव Resource_दत्त& res_दत्त,
                               Resource r,
                               ध्रुव वर्ण* कुञ्चिका,
                               int32_t idx,
                               UResource_दत्त_प्रविष्टिः* valid_Locale_दत्त_प्रविष्टिः,
                               ध्रुव वर्ण* container_Res_मार्गः,
                               int32_t recursion_गभीरता,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_संहिता* स्थितिः) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    यदि (U_FAILURE(*स्थितिः)) { प्रत्यागम resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ध्रुव char16_t *alias = res_getAlias(&res_दत्त, r, &len);
    यदि(len <= 0) {
        // bad alias
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
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
    chAlias.appendInvariantChars(alias, len, *स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    // We have an alias, now let's cut it up.
    ध्रुव वर्ण *मार्गः = शून्यसूचकः, *locale = शून्यसूचकः, *कुञ्चिका_मार्गः = शून्यसूचकः;
    यदि(chAlias[0] == RES_मार्गः_SEPARATOR) {
        // There is a path included.
        वर्ण *ch_Alias_दत्त = chAlias.दत्त();
        वर्ण *sep = ch_Alias_दत्त + 1;
        मार्गः = sep;
        sep = uprv_strchr(sep, RES_मार्गः_SEPARATOR);
        यदि(sep != शून्यसूचकः) {
            *sep++ = 0;
        }
        यदि(uprv_strcmp(मार्गः, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            कुञ्चिका_मार्गः = sep;
            // Read from the valid locale which we already have.
            मार्गः = locale = शून्यसूचकः;
        } अन्यथा {
            यदि(uprv_strcmp(मार्गः, "ICUDATA") == 0) { /* want ICU data */
                मार्गः = शून्यसूचकः;
            }
            यदि (sep == शून्यसूचकः) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } अन्यथा {
                locale = sep;
                sep = uprv_strchr(sep, RES_मार्गः_SEPARATOR);
                यदि(sep != शून्यसूचकः) {
                    *sep++ = 0;
                }
                कुञ्चिका_मार्गः = sep;
            }
        }
    } अन्यथा {
        // No path, start with a locale.
        वर्ण *sep = chAlias.दत्त();
        locale = sep;
        sep = uprv_strchr(sep, RES_मार्गः_SEPARATOR);
        यदि(sep != शून्यसूचकः) {
            *sep++ = 0;
        }
        कुञ्चिका_मार्गः = sep;
        मार्गः = valid_Locale_दत्त_प्रविष्टिः->f_मार्गः;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    LocalUResourceBundlePointer mainRes;
    UResource_दत्त_प्रविष्टिः *दत्त_प्रविष्टिः;
    यदि (locale == शून्यसूचकः) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        दत्त_प्रविष्टिः = valid_Locale_दत्त_प्रविष्टिः;
    } अन्यथा {
        UError_संहिता int_स्थितिः = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_प्रत्यक्षम्(मार्गः, locale, &int_स्थितिः));
        यदि(U_FAILURE(int_स्थितिः)) {
            // We failed to open the resource bundle we're aliasing to.
            *स्थितिः = int_स्थितिः;
            प्रत्यागम resB;
        }
        दत्त_प्रविष्टिः = mainRes->f_दत्त;
    }

    ध्रुव वर्ण* अस्थायी = शून्यसूचकः;
    यदि(कुञ्चिका_मार्गः == शून्यसूचकः) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = दत्त_प्रविष्टिः->f_दत्त.मूलम्_Res;
        यदि(container_Res_मार्गः) {
            chAlias.clear().append(container_Res_मार्गः, *स्थितिः);
            यदि (U_FAILURE(*स्थितिः)) {
                प्रत्यागम शून्यसूचकः;
            }
            वर्ण *a_कुञ्चिका = chAlias.दत्त();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_findResource(&दत्त_प्रविष्टिः->f_दत्त, r, &a_कुञ्चिका, &अस्थायी);
        }
        यदि(कुञ्चिका) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(कुञ्चिका, *स्थितिः);
            यदि (U_FAILURE(*स्थितिः)) {
                प्रत्यागम शून्यसूचकः;
            }
            वर्ण *a_कुञ्चिका = chAlias.दत्त();
            r = res_findResource(&दत्त_प्रविष्टिः->f_दत्त, r, &a_कुञ्चिका, &अस्थायी);
        } अन्यथा यदि(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            यदि(URES_IS_सारणी(type)) {
                ध्रुव वर्ण *a_कुञ्चिका;
                r = res_get_सारणी_Item_By_सूचकांक(&दत्त_प्रविष्टिः->f_दत्त, r, idx, &a_कुञ्चिका);
            } अन्यथा { /* array */
                r = res_getArrayItem(&दत्त_प्रविष्टिः->f_दत्त, r, idx);
            }
        }
        यदि(r != RES_BOGUS) {
            resB = init_resb_परिणाम(
                दत्त_प्रविष्टिः, r, अस्थायी, -1, valid_Locale_दत्त_प्रविष्टिः, शून्यसूचकः, recursion_गभीरता+1,
                resB, स्थितिः);
        } अन्यथा {
            *स्थितिः = U_MISSING_RESOURCE_ERROR;
        }
    } अन्यथा {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString मार्गः_Buf(कुञ्चिका_मार्गः, *स्थितिः);
        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम शून्यसूचकः;
        }
        वर्ण *my_मार्गः = मार्गः_Buf.दत्त();
        container_Res_मार्गः = शून्यसूचकः;
        // Now we have fallback following here.
        प्रति(;;) {
            r = दत्त_प्रविष्टिः->f_दत्त.मूलम्_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            यावत्(*my_मार्गः && U_SUCCESS(*स्थितिः)) {
                r = res_findResource(&(दत्त_प्रविष्टिः->f_दत्त), r, &my_मार्गः, &अस्थायी);
                यदि(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    भङ्ग;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_परिणाम(
                    दत्त_प्रविष्टिः, r, अस्थायी, -1,
                    valid_Locale_दत्त_प्रविष्टिः, container_Res_मार्गः, recursion_गभीरता+1,
                    resB, स्थितिः);
                यदि (U_FAILURE(*स्थितिः)) {
                    भङ्ग;
                }
                यदि (अस्थायी == शून्यसूचकः || uprv_strcmp(कुञ्चिका_मार्गः, अस्थायी) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_मार्गः(resB);
                    ures_append_Res_मार्गः(resB, कुञ्चिका_मार्गः, static_cast<int32_t>(uprv_strlen(कुञ्चिका_मार्गः)), स्थितिः);
                    यदि(resB->f_Res_मार्गः[resB->f_Res_मार्गः_Len-1] != RES_मार्गः_SEPARATOR) {
                        ures_append_Res_मार्गः(resB, RES_मार्गः_SEPARATOR_S, 1, स्थितिः);
                    }
                    यदि (U_FAILURE(*स्थितिः)) {
                        भङ्ग;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                दत्त_प्रविष्टिः = resB->f_दत्त;
                container_Res_मार्गः = resB->f_Res_मार्गः;
            }
            यदि (U_FAILURE(*स्थितिः) || r != RES_BOGUS) {
                भङ्ग;
            }
            // Fall back to the parent bundle, if there is one.
            दत्त_प्रविष्टिः = दत्त_प्रविष्टिः->fParent;
            यदि (दत्त_प्रविष्टिः == शून्यसूचकः) {
                *स्थितिः = U_MISSING_RESOURCE_ERROR;
                भङ्ग;
            }
            // Copy the same keyPath again.
            my_मार्गः = मार्गः_Buf.दत्त();
            uprv_strcpy(my_मार्गः, कुञ्चिका_मार्गः);
        }
    }
    यदि(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    प्रत्यागम resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_परिणाम(UResource_दत्त_प्रविष्टिः* दत्त_प्रविष्टिः,
                                  Resource r,
                                  ध्रुव वर्ण* कुञ्चिका,
                                  int32_t idx,
                                  UResource_दत्त_प्रविष्टिः* valid_Locale_दत्त_प्रविष्टिः,
                                  ध्रुव वर्ण* container_Res_मार्गः,
                                  int32_t recursion_गभीरता,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_संहिता* स्थितिः) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम resB;
    }
    यदि (valid_Locale_दत्त_प्रविष्टिः == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    यदि(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        यदि(recursion_गभीरता >= URES_MAX_ALIAS_LEVEL) {
            *स्थितिः = U_TOO_MANY_ALIASES_ERROR;
            प्रत्यागम resB;
        }
        प्रत्यागम getAliasTargetAsResourceBundle(
            दत्त_प्रविष्टिः->f_दत्त, r, कुञ्चिका, idx,
            valid_Locale_दत्त_प्रविष्टिः, container_Res_मार्गः, recursion_गभीरता, resB, स्थितिः);
    }
    यदि(resB == शून्यसूचकः) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
        यदि (resB == शून्यसूचकः) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        ures_समुच्चयः_Is_राशिः_Object(resB, असत्य);
        resB->f_Res_मार्गः = शून्यसूचकः;
        resB->f_Res_मार्गः_Len = 0;
    } अन्यथा {
        यदि(resB->f_दत्त != शून्यसूचकः) {
            प्रविष्टिः_Close(resB->f_दत्त);
        }
        यदि(resB->f_संस्करणम् != शून्यसूचकः) {
            uprv_free(resB->f_संस्करणम्);
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
        यदि(container_Res_मार्गः != resB->f_Res_मार्गः) {
            ures_free_Res_मार्गः(resB);
        }
    }
    resB->f_दत्त = दत्त_प्रविष्टिः;
    प्रविष्टिः_Increase(resB->f_दत्त);
    resB->fHasFallback = असत्य;
    resB->f_Is_शिखरम्_Level = असत्य;
    resB->f_सूचकांक = -1;
    resB->f_कुञ्चिका = कुञ्चिका; 
    resB->f_Valid_Locale_दत्त_प्रविष्टिः = valid_Locale_दत्त_प्रविष्टिः;
    यदि(container_Res_मार्गः != resB->f_Res_मार्गः) {
        ures_append_Res_मार्गः(
            resB, container_Res_मार्गः, static_cast<int32_t>(uprv_strlen(container_Res_मार्गः)), स्थितिः);
    }
    यदि(कुञ्चिका != शून्यसूचकः) {
        ures_append_Res_मार्गः(resB, कुञ्चिका, static_cast<int32_t>(uprv_strlen(कुञ्चिका)), स्थितिः);
        यदि(resB->f_Res_मार्गः_Len > 0 && resB->f_Res_मार्गः[resB->f_Res_मार्गः_Len-1] != RES_मार्गः_SEPARATOR) {
            ures_append_Res_मार्गः(resB, RES_मार्गः_SEPARATOR_S, 1, स्थितिः);
        }
    } अन्यथा यदि(idx >= 0) {
        वर्ण buf[256];
        int32_t len = T_CString_integer_गन्तव्यम्_String(buf, idx, 10);
        ures_append_Res_मार्गः(resB, buf, len, स्थितिः);
        यदि(resB->f_Res_मार्गः_Len > 0 && resB->f_Res_मार्गः[resB->f_Res_मार्गः_Len-1] != RES_मार्गः_SEPARATOR) {
            ures_append_Res_मार्गः(resB, RES_मार्गः_SEPARATOR_S, 1, स्थितिः);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t प्रयुक्तम्_Len = ((resB->fResBuf == resB->f_Res_मार्गः) ? resB->f_Res_मार्गः_Len : 0);
        uprv_memset(resB->fResBuf + प्रयुक्तम्_Len, 0, परिमाण(resB->fResBuf) - प्रयुक्तम्_Len);
    }

    resB->f_संस्करणम् = शून्यसूचकः;
    resB->fRes = r;
    resB->f_आकार = res_गणनम्_Array_वस्तूनि(&resB->get_Res_दत्त(), resB->fRes);
    ResourceTracer(resB).trace("get");
    प्रत्यागम resB;
}

UResourceBundle* init_resb_परिणाम(UResource_दत्त_प्रविष्टिः* दत्त_प्रविष्टिः,
                                  Resource r,
                                  ध्रुव वर्ण* कुञ्चिका,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ध्रुव UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_संहिता* स्थितिः) {
    प्रत्यागम init_resb_परिणाम(
        दत्त_प्रविष्टिः, r, कुञ्चिका, idx,
        container->f_Valid_Locale_दत्त_प्रविष्टिः, container->f_Res_मार्गः, 0, resB, स्थितिः);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ध्रुव UResourceBundle* original,
                               UError_संहिता* स्थितिः) {
    UBool is_राशिः_Object;
    यदि(U_FAILURE(*स्थितिः) || r == original) {
        प्रत्यागम r;
    }
    यदि(original != शून्यसूचकः) {
        यदि(r == शून्यसूचकः) {
            is_राशिः_Object = असत्य;
            r = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
            /* test for nullptr */
            यदि (r == शून्यसूचकः) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम शून्यसूचकः;
            }
        } अन्यथा {
            is_राशिः_Object = ures_is_राशिः_Object(r);
            ures_closeBundle(r, असत्य);
        }
        uprv_memcpy(r, original, परिमाण(UResourceBundle));
        r->f_Res_मार्गः = शून्यसूचकः;
        r->f_Res_मार्गः_Len = 0;
        यदि(original->f_Res_मार्गः) {
            ures_append_Res_मार्गः(r, original->f_Res_मार्गः, original->f_Res_मार्गः_Len, स्थितिः);
        }
        ures_समुच्चयः_Is_राशिः_Object(r, is_राशिः_Object);
        यदि(r->f_दत्त != शून्यसूचकः) {
            प्रविष्टिः_Increase(r->f_दत्त);
        }
    }
    प्रत्यागम r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ध्रुव char16_t* U_EXPORT2 ures_getString(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_संहिता* स्थितिः) {
    ध्रुव char16_t *s;
    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(resB == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    s = res_getString({resB}, &resB->get_Res_दत्त(), resB->fRes, len);
    यदि (s == शून्यसूचकः) {
        *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम s;
}

स्थिर ध्रुव वर्ण*
ures_गन्तव्यम्_UTF8_String(ध्रुव char16_t* s16,
                  int32_t length16,
                  वर्ण* dest U_LIFETIME_BOUND,
                  int32_t* p_दीर्घता,
                  UBool forceCopy,
                  UError_संहिता* स्थितिः) {
    int32_t capacity;

    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (p_दीर्घता != शून्यसूचकः) {
        capacity = *p_दीर्घता;
    } अन्यथा {
        capacity = 0;
    }
    यदि (capacity < 0 || (capacity > 0 && dest == शून्यसूचकः)) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि (length16 == 0) {
        /* empty string, return as read-only pointer */
        यदि (p_दीर्घता != शून्यसूचकः) {
            *p_दीर्घता = 0;
        }
        यदि (forceCopy) {
            u_terminateChars(dest, capacity, 0, स्थितिः);
            प्रत्यागम dest;
        } अन्यथा {
            प्रत्यागम "";
        }
    } अन्यथा {
        /* We need to transform the string to the destination buffer. */
        यदि (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            प्रत्यागम u_str_गन्तव्यम्_UTF8(शून्यसूचकः, 0, p_दीर्घता, s16, length16, स्थितिः);
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
            int32_t max_दीर्घता_2 = 3 * length16 + 1;
            यदि (capacity > max_दीर्घता_2) {
                dest += capacity - max_दीर्घता_2;
                capacity = max_दीर्घता_2;
            }
        }
        प्रत्यागम u_str_गन्तव्यम्_UTF8(dest, capacity, p_दीर्घता, s16, length16, स्थितिः);
    }
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getUTF8String(ध्रुव UResourceBundle* resB,
                   वर्ण* dest U_LIFETIME_BOUND,
                   int32_t* p_दीर्घता,
                   UBool forceCopy,
                   UError_संहिता* स्थितिः) {
    int32_t length16;
    ध्रुव char16_t *s16 = ures_getString(resB, &length16, स्थितिः);
    प्रत्यागम ures_गन्तव्यम्_UTF8_String(s16, length16, dest, p_दीर्घता, forceCopy, स्थितिः);
}

U_CAPI ध्रुव uint8_t* U_EXPORT2
ures_get_द्विमान(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_संहिता* स्थितिः) {
  ध्रुव uint8_t *p;
  यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  यदि(resB == शून्यसूचकः) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  p = res_get_द्विमान({resB}, &resB->get_Res_दत्त(), resB->fRes, len);
  यदि (p == शून्यसूचकः) {
    *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
  }
  प्रत्यागम p;
}

U_CAPI ध्रुव int32_t* U_EXPORT2
ures_get_Int_सदिशम्(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_संहिता* स्थितिः) {
  ध्रुव int32_t *p;
  यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  यदि(resB == शून्यसूचकः) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  p = res_get_Int_सदिशम्({resB}, &resB->get_Res_दत्त(), resB->fRes, len);
  यदि (p == शून्यसूचकः) {
    *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
  }
  प्रत्यागम p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ध्रुव UResourceBundle* resB, UError_संहिता *स्थितिः) {
  यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
    प्रत्यागम 0xffffffff;
  }
  यदि(resB == शून्यसूचकः) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0xffffffff;
  }
  यदि(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
    प्रत्यागम 0xffffffff;
  }
  प्रत्यागम res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ध्रुव UResourceBundle* resB, UError_संहिता *स्थितिः) {
  यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
    प्रत्यागम 0xffffffff;
  }
  यदि(resB == शून्यसूचकः) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0xffffffff;
  }
  यदि(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
    प्रत्यागम 0xffffffff;
  }
  प्रत्यागम res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ध्रुव UResourceBundle *resB) {
  यदि(resB == शून्यसूचकः) {
    प्रत्यागम URES_NONE;
  }
  प्रत्यागम res_get_लौकिकम्_Type(resB->fRes);
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
  यदि(resB == शून्यसूचकः) {
    प्रत्यागम शून्यसूचकः;
  }
  प्रत्यागम(resB->f_कुञ्चिका);
}

U_CAPI int32_t U_EXPORT2 ures_get_आकार(ध्रुव UResourceBundle *resB) {
  यदि(resB == शून्यसूचकः) {
    प्रत्यागम 0;
  }
  
  प्रत्यागम resB->f_आकार;
}

स्थिर ध्रुव char16_t*
ures_getStringWithAlias(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_सूचकांक,
                        int32_t* len,
                        UError_संहिता* स्थितिः) {
  यदि(RES_GET_TYPE(r) == URES_ALIAS) {
    ध्रुव char16_t* परिणाम = शून्यसूचकः;
    UResourceBundle *अस्थायी_Res = ures_get_By_सूचकांक(resB, s_सूचकांक, शून्यसूचकः, स्थितिः);
    परिणाम = ures_getString(अस्थायी_Res, len, स्थितिः);
    ures_close(अस्थायी_Res);
    प्रत्यागम परिणाम;
  } अन्यथा {
    प्रत्यागम res_getString({resB, s_सूचकांक}, &resB->get_Res_दत्त(), r, len); 
  }
}

U_CAPI शून्य U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  यदि(resB == शून्यसूचकः) {
    प्रत्यागम;
  }
  resB->f_सूचकांक = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_अग्र(ध्रुव UResourceBundle *resB) {
  यदि(resB == शून्यसूचकः) {
    प्रत्यागम असत्य;
  }
  प्रत्यागम resB->f_सूचकांक < resB->f_आकार-1;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ures_get_अग्र_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ध्रुव वर्ण** कुञ्चिका,
                   UError_संहिता* स्थितिः) {
  Resource r = RES_BOGUS;
  
  यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  यदि(resB == शून्यसूचकः) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  
  यदि(resB->f_सूचकांक == resB->f_आकार-1) {
    *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
  } अन्यथा {
    resB->f_सूचकांक++;
    विकल्प(RES_GET_TYPE(resB->fRes)) {
    पक्ष URES_STRING:
    पक्ष URES_STRING_V2:
      प्रत्यागम res_getString({resB}, &resB->get_Res_दत्त(), resB->fRes, len);
    पक्ष URES_सारणी:
    पक्ष URES_TABLE16:
    पक्ष URES_TABLE32:
      r = res_get_सारणी_Item_By_सूचकांक(&resB->get_Res_दत्त(), resB->fRes, resB->f_सूचकांक, कुञ्चिका);
      यदि(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      प्रत्यागम ures_getStringWithAlias(resB, r, resB->f_सूचकांक, len, स्थितिः);
    पक्ष URES_ARRAY:
    पक्ष URES_ARRAY16:
      r = res_getArrayItem(&resB->get_Res_दत्त(), resB->fRes, resB->f_सूचकांक);
      यदि(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      प्रत्यागम ures_getStringWithAlias(resB, r, resB->f_सूचकांक, len, स्थितिः);
    पक्ष URES_ALIAS:
      प्रत्यागम ures_getStringWithAlias(resB, resB->fRes, resB->f_सूचकांक, len, स्थितिः);
    पक्ष URES_INT:
    पक्ष URES_द्विमान:
    पक्ष URES_INT_सदिशम्:
        *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    स्वभाव:
      प्रत्यागम शून्यसूचकः;
    }
  }

  प्रत्यागम शून्यसूचकः;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_अग्र_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                     UError_संहिता* स्थितिः) {
    ध्रुव वर्ण *कुञ्चिका = शून्यसूचकः;
    Resource r = RES_BOGUS;

    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
            /*return nullptr;*/
            प्रत्यागम fill_अन्तर्;
    }
    यदि(resB == शून्यसूचकः) {
            *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            /*return nullptr;*/
            प्रत्यागम fill_अन्तर्;
    }

    यदि(resB->f_सूचकांक == resB->f_आकार-1) {
      *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } अन्यथा {
        resB->f_सूचकांक++;
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_INT:
        पक्ष URES_द्विमान:
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
        पक्ष URES_INT_सदिशम्:
            प्रत्यागम ures_copyResb(fill_अन्तर्, resB, स्थितिः);
        पक्ष URES_सारणी:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_get_सारणी_Item_By_सूचकांक(&resB->get_Res_दत्त(), resB->fRes, resB->f_सूचकांक, &कुञ्चिका);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम init_resb_परिणाम(resB->f_दत्त, r, कुञ्चिका, resB->f_सूचकांक, resB, fill_अन्तर्, स्थितिः);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_दत्त(), resB->fRes, resB->f_सूचकांक);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम init_resb_परिणाम(resB->f_दत्त, r, कुञ्चिका, resB->f_सूचकांक, resB, fill_अन्तर्, स्थितिः);
        स्वभाव:
            /*return nullptr;*/
            प्रत्यागम fill_अन्तर्;
        }
    }
    /*return nullptr;*/
    प्रत्यागम fill_अन्तर्;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_सूचकांक(ध्रुव UResourceBundle* resB,
                int32_t सूचकांक_R,
                UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                UError_संहिता* स्थितिः) {
    ध्रुव वर्ण* कुञ्चिका = शून्यसूचकः;
    Resource r = RES_BOGUS;

    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        /*return nullptr;*/
        प्रत्यागम fill_अन्तर्;
    }
    यदि(resB == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        /*return nullptr;*/
        प्रत्यागम fill_अन्तर्;
    }

    यदि(सूचकांक_R >= 0 && resB->f_आकार > सूचकांक_R) {
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_INT:
        पक्ष URES_द्विमान:
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
        पक्ष URES_INT_सदिशम्:
            प्रत्यागम ures_copyResb(fill_अन्तर्, resB, स्थितिः);
        पक्ष URES_सारणी:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_get_सारणी_Item_By_सूचकांक(&resB->get_Res_दत्त(), resB->fRes, सूचकांक_R, &कुञ्चिका);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम init_resb_परिणाम(resB->f_दत्त, r, कुञ्चिका, सूचकांक_R, resB, fill_अन्तर्, स्थितिः);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_दत्त(), resB->fRes, सूचकांक_R);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम init_resb_परिणाम(resB->f_दत्त, r, कुञ्चिका, सूचकांक_R, resB, fill_अन्तर्, स्थितिः);
        स्वभाव:
            /*return nullptr;*/
            प्रत्यागम fill_अन्तर्;
        }
    } अन्यथा {
        *स्थितिः = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    प्रत्यागम fill_अन्तर्;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ures_get_String_By_सूचकांक(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t सूचकांक_S,
                      int32_t* len,
                      UError_संहिता* स्थितिः) {
    ध्रुव वर्ण* कुञ्चिका = शून्यसूचकः;
    Resource r = RES_BOGUS;

    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(resB == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(सूचकांक_S >= 0 && resB->f_आकार > सूचकांक_S) {
        विकल्प(RES_GET_TYPE(resB->fRes)) {
        पक्ष URES_STRING:
        पक्ष URES_STRING_V2:
            प्रत्यागम res_getString({resB}, &resB->get_Res_दत्त(), resB->fRes, len);
        पक्ष URES_सारणी:
        पक्ष URES_TABLE16:
        पक्ष URES_TABLE32:
            r = res_get_सारणी_Item_By_सूचकांक(&resB->get_Res_दत्त(), resB->fRes, सूचकांक_S, &कुञ्चिका);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम ures_getStringWithAlias(resB, r, सूचकांक_S, len, स्थितिः);
        पक्ष URES_ARRAY:
        पक्ष URES_ARRAY16:
            r = res_getArrayItem(&resB->get_Res_दत्त(), resB->fRes, सूचकांक_S);
            यदि(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            प्रत्यागम ures_getStringWithAlias(resB, r, सूचकांक_S, len, स्थितिः);
        पक्ष URES_ALIAS:
            प्रत्यागम ures_getStringWithAlias(resB, resB->fRes, सूचकांक_S, len, स्थितिः);
        पक्ष URES_INT:
        पक्ष URES_द्विमान:
        पक्ष URES_INT_सदिशम्:
            *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
            भङ्ग;
        स्वभाव:
          /* must not occur */
          *स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
          भङ्ग;
        }
    } अन्यथा {
        *स्थितिः = U_MISSING_RESOURCE_ERROR;
    }
    प्रत्यागम शून्यसूचकः;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_UTF8_String_By_सूचकांक(ध्रुव UResourceBundle* resB,
                          int32_t idx,
                          वर्ण* dest U_LIFETIME_BOUND,
                          int32_t* p_दीर्घता,
                          UBool forceCopy,
                          UError_संहिता* स्थितिः) {
    int32_t length16;
    ध्रुव char16_t *s16 = ures_get_String_By_सूचकांक(resB, idx, &length16, स्थितिः);
    प्रत्यागम ures_गन्तव्यम्_UTF8_String(s16, length16, dest, p_दीर्घता, forceCopy, स्थितिः);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(ध्रुव वर्ण* मार्गः, UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND, UError_संहिता* स्थितिः)
{
  UResourceBundle *first = शून्यसूचकः; 
  UResourceBundle *परिणाम = fill_अन्तर्;
  वर्ण *packageName = शून्यसूचकः;
  वर्ण *मार्गः_गन्तव्यम्_Resource = शून्यसूचकः, *save = शून्यसूचकः;
  वर्ण *locale = शून्यसूचकः, *locale_अन्तः = शून्यसूचकः;
  int32_t length;

  यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)) {
    प्रत्यागम परिणाम;
  }

  length = (int32_t)(uprv_strlen(मार्गः)+1);
  save = मार्गः_गन्तव्यम्_Resource = (वर्ण *)uprv_malloc(length*परिमाण(वर्ण));
  /* test for nullptr */
  यदि(मार्गः_गन्तव्यम्_Resource == शून्यसूचकः) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम परिणाम;
  }
  uprv_memcpy(मार्गः_गन्तव्यम्_Resource, मार्गः, length);

  locale = मार्गः_गन्तव्यम्_Resource;
  यदि(*मार्गः_गन्तव्यम्_Resource == RES_मार्गः_SEPARATOR) { /* there is a path specification */
    मार्गः_गन्तव्यम्_Resource++;
    packageName = मार्गः_गन्तव्यम्_Resource;
    मार्गः_गन्तव्यम्_Resource = uprv_strchr(मार्गः_गन्तव्यम्_Resource, RES_मार्गः_SEPARATOR);
    यदि(मार्गः_गन्तव्यम्_Resource == शून्यसूचकः) {
      *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    } अन्यथा {
      *मार्गः_गन्तव्यम्_Resource = 0;
      locale = मार्गः_गन्तव्यम्_Resource+1;
    }
  }

  locale_अन्तः = uprv_strchr(locale, RES_मार्गः_SEPARATOR);
  यदि(locale_अन्तः != शून्यसूचकः) {
    *locale_अन्तः = 0;
  }

  first = ures_open(packageName, locale, स्थितिः);

  यदि(U_SUCCESS(*स्थितिः)) {
    यदि(locale_अन्तः) {
      परिणाम = ures_findSubResource(first, locale_अन्तः+1, fill_अन्तर्, स्थितिः);
    } अन्यथा {
      परिणाम = ures_copyResb(fill_अन्तर्, first, स्थितिः);
    }
    ures_close(first);
  }
  uprv_free(save);
  प्रत्यागम परिणाम;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(ध्रुव UResourceBundle* resB,
                     वर्ण* मार्गः,
                     UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                     UError_संहिता* स्थितिः)
{
  Resource res = RES_BOGUS;
  UResourceBundle *परिणाम = fill_अन्तर्;
  ध्रुव वर्ण *कुञ्चिका;

  यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)) {
    प्रत्यागम परिणाम;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  कुरु {
    res = res_findResource(&resB->get_Res_दत्त(), resB->fRes, &मार्गः, &कुञ्चिका); 
    यदि(res != RES_BOGUS) {
        परिणाम = init_resb_परिणाम(resB->f_दत्त, res, कुञ्चिका, -1, resB, fill_अन्तर्, स्थितिः);
        resB = परिणाम;
    } अन्यथा {
        *स्थितिः = U_MISSING_RESOURCE_ERROR;
        भङ्ग;
    }
  } यावत्(*मार्गः); /* there is more stuff in the path */

  प्रत्यागम परिणाम;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ures_get_String_By_कुञ्चिका_With_Fallback(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                                ध्रुव वर्ण* अन्तर्_कुञ्चिका,
                                int32_t* len,
                                UError_संहिता* स्थितिः) {
    UResourceBundle राशिः_2;
    ध्रुव char16_t* retVal = शून्यसूचकः;
    ures_init_राशिः_Object(&राशिः_2);
    ures_get_By_कुञ्चिका_With_Fallback(resB, अन्तर्_कुञ्चिका, &राशिः_2, स्थितिः);
    int32_t length;
    retVal = ures_getString(&राशिः_2, &length, स्थितिः);
    ures_close(&राशिः_2);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (length == 3 && retVal[0] == रिक्तानि_समुच्चयः && retVal[1] == रिक्तानि_समुच्चयः && retVal[2] == रिक्तानि_समुच्चयः ) {
        retVal = शून्यसूचकः;
        length = 0;
        *स्थितिः = U_MISSING_RESOURCE_ERROR;
    }
    यदि (len != शून्यसूचकः) {
        *len = length;
    }
    प्रत्यागम retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
स्थिर Resource get_सारणी_Item_By_कुञ्चिका_मार्गः(ध्रुव Resource_दत्त *p_Res_दत्त, Resource सारणी, ध्रुव वर्ण *कुञ्चिका) {
  Resource resource = सारणी;  /* The current resource */
  icu::CharString मार्गः;
  UError_संहिता error_संहिता = U_ZERO_ERROR;
  मार्गः.append(कुञ्चिका, error_संहिता);
  यदि (U_FAILURE(error_संहिता)) { प्रत्यागम RES_BOGUS; }
  वर्ण *मार्गः_Part = मार्गः.दत्त();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  यावत् (*मार्गः_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    वर्ण *अग्र_मार्गः_Part = uprv_strchr(मार्गः_Part, RES_मार्गः_SEPARATOR);
    यदि (अग्र_मार्गः_Part != शून्यसूचकः) {
      *अग्र_मार्गः_Part = 0;  /* Terminating null for this part of path. */
      अग्र_मार्गः_Part++;
    } अन्यथा {
      अग्र_मार्गः_Part = uprv_strchr(मार्गः_Part, 0);
    }
    int32_t t;
    ध्रुव वर्ण *मार्गः_P = मार्गः_Part;
    resource = res_get_सारणी_Item_By_कुञ्चिका(p_Res_दत्त, resource, &t, &मार्गः_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    मार्गः_Part = अग्र_मार्गः_Part; 
  }
  यदि (*मार्गः_Part) {
    प्रत्यागम RES_BOGUS;
  }
  प्रत्यागम resource;
}

स्थिर शून्य create_मार्गः(ध्रुव वर्ण* orig_Res_मार्गः,
                       int32_t     orig_Res_मार्गः_Len,
                       ध्रुव वर्ण* res_मार्गः,
                       int32_t     res_मार्गः_Len,
                       ध्रुव वर्ण* अन्तर्_कुञ्चिका,
                       CharString& मार्गः,
                       UError_संहिता* स्थितिः) {
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
    मार्गः.clear();
    ध्रुव वर्ण* कुञ्चिका = अन्तर्_कुञ्चिका;
    यदि (res_मार्गः_Len > 0) {
        मार्गः.append(res_मार्गः, res_मार्गः_Len, *स्थितिः);
        यदि (U_SUCCESS(*स्थितिः)) {
            ध्रुव वर्ण* res_मार्गः_सीमा = res_मार्गः + res_मार्गः_Len;
            ध्रुव वर्ण* orig_Res_मार्गः_सीमा = orig_Res_मार्गः + orig_Res_मार्गः_Len;
            ध्रुव वर्ण* res_मार्गः_Ptr = res_मार्गः;
            ध्रुव वर्ण* orig_Res_मार्गः_Ptr = orig_Res_मार्गः;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            यावत् (orig_Res_मार्गः_Ptr < orig_Res_मार्गः_सीमा && res_मार्गः_Ptr < res_मार्गः_सीमा) {
                यावत् (orig_Res_मार्गः_Ptr < orig_Res_मार्गः_सीमा && *orig_Res_मार्गः_Ptr != RES_मार्गः_SEPARATOR) {
                    ++orig_Res_मार्गः_Ptr;
                }
                यदि (orig_Res_मार्गः_Ptr < orig_Res_मार्गः_सीमा && *orig_Res_मार्गः_Ptr == RES_मार्गः_SEPARATOR) {
                    ++orig_Res_मार्गः_Ptr;
                }
                यावत् (res_मार्गः_Ptr < res_मार्गः_सीमा && *res_मार्गः_Ptr != RES_मार्गः_SEPARATOR) {
                    ++res_मार्गः_Ptr;
                }
                यदि (res_मार्गः_Ptr < res_मार्गः_सीमा && *res_मार्गः_Ptr == RES_मार्गः_SEPARATOR) {
                    ++res_मार्गः_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            यावत् (res_मार्गः_Ptr < res_मार्गः_सीमा && *कुञ्चिका != '\0') {
                यावत् (res_मार्गः_Ptr < res_मार्गः_सीमा && *res_मार्गः_Ptr != RES_मार्गः_SEPARATOR) {
                    ++res_मार्गः_Ptr;
                }
                यदि (res_मार्गः_Ptr < res_मार्गः_सीमा && *res_मार्गः_Ptr == RES_मार्गः_SEPARATOR) {
                    ++res_मार्गः_Ptr;
                }
                यावत् (*कुञ्चिका != '\0' && *कुञ्चिका != RES_मार्गः_SEPARATOR) {
                    ++कुञ्चिका;
                }
                यदि (*कुञ्चिका == RES_मार्गः_SEPARATOR) {
                    ++कुञ्चिका;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        मार्गः.append(कुञ्चिका, *स्थितिः);
    } अन्यथा {
        मार्गः.append(अन्तर्_कुञ्चिका, *स्थितिः);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_कुञ्चिका_With_Fallback(ध्रुव UResourceBundle* resB,
                          ध्रुव वर्ण* अन्तर्_कुञ्चिका,
                          UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                          UError_संहिता* स्थितिः) {
    Resource res = RES_BOGUS, मूलम्_Res = RES_BOGUS;
    UResourceBundle *helper = शून्यसूचकः;

    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम fill_अन्तर्;
    }
    यदि(resB == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम fill_अन्तर्;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    यदि(URES_IS_सारणी(type)) {
        ध्रुव वर्ण* orig_Res_मार्गः = resB->f_Res_मार्गः;
        int32_t orig_Res_मार्गः_Len = resB->f_Res_मार्गः_Len;
        res = get_सारणी_Item_By_कुञ्चिका_मार्गः(&resB->get_Res_दत्त(), resB->fRes, अन्तर्_कुञ्चिका);
        ध्रुव वर्ण* कुञ्चिका = अन्तर्_कुञ्चिका;
        तार्किक did_मूलम्_Once = असत्य;
        यदि(res == RES_BOGUS) {
            UResource_दत्त_प्रविष्टिः *दत्त_प्रविष्टिः = resB->f_दत्त;
            CharString मार्गः;
            वर्ण *my_मार्गः = शून्यसूचकः;
            ध्रुव वर्ण* res_मार्गः = resB->f_Res_मार्गः;
            int32_t len = resB->f_Res_मार्गः_Len;
            यावत्(res == RES_BOGUS && (दत्त_प्रविष्टिः->fParent != शून्यसूचकः || !did_मूलम्_Once)) { /* Otherwise, we'll look in parents */
                यदि (दत्त_प्रविष्टिः->fParent != शून्यसूचकः) {
                    दत्त_प्रविष्टिः = दत्त_प्रविष्टिः->fParent;
                } अन्यथा {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_मूलम्_Once = सत्य;
                }
                मूलम्_Res = दत्त_प्रविष्टिः->f_दत्त.मूलम्_Res;

                यदि(दत्त_प्रविष्टिः->fBogus == U_ZERO_ERROR) {
                    create_मार्गः(orig_Res_मार्गः, orig_Res_मार्गः_Len, res_मार्गः, len, अन्तर्_कुञ्चिका, मार्गः, स्थितिः);
                    यदि (U_FAILURE(*स्थितिः)) {
                        ures_close(helper);
                        प्रत्यागम fill_अन्तर्;
                    }
                    my_मार्गः = मार्गः.दत्त();
                    कुञ्चिका = अन्तर्_कुञ्चिका;
                    कुरु {
                        res = res_findResource(&(दत्त_प्रविष्टिः->f_दत्त), मूलम्_Res, &my_मार्गः, &कुञ्चिका);
                        यदि (RES_GET_TYPE(res) == URES_ALIAS && *my_मार्गः) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_परिणाम(दत्त_प्रविष्टिः, res, शून्यसूचकः, -1, resB, helper, स्थितिः);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            यदि(helper) {
                              दत्त_प्रविष्टिः = helper->f_दत्त;
                              मूलम्_Res = helper->fRes;
                              res_मार्गः = helper->f_Res_मार्गः;
                              len = helper->f_Res_मार्गः_Len;

                            } अन्यथा {
                              भङ्ग;
                            }
                        } अन्यथा यदि (res == RES_BOGUS) {
                            भङ्ग;
                        }
                    } यावत्(*my_मार्गः); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            यदि(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                यदि(uprv_strcmp(दत्त_प्रविष्टिः->fName, uloc_getDefault())==0 || uprv_strcmp(दत्त_प्रविष्टिः->fName, k_मूलम्_Locale_Name)==0) {
                    *स्थितिः = U_USING_DEFAULT_WARNING;
                } अन्यथा {
                    *स्थितिः = U_USING_FALLBACK_WARNING;
                }

                fill_अन्तर् = init_resb_परिणाम(दत्त_प्रविष्टिः, res, कुञ्चिका, -1, resB, fill_अन्तर्, स्थितिः);
                यदि (res_मार्गः != शून्यसूचकः) {
                    create_मार्गः(orig_Res_मार्गः, orig_Res_मार्गः_Len, res_मार्गः, len, अन्तर्_कुञ्चिका, मार्गः, स्थितिः);
                } अन्यथा {
                    ध्रुव वर्ण* separator = शून्यसूचकः;
                    यदि (fill_अन्तर्->f_Res_मार्गः != शून्यसूचकः) {
                        separator = uprv_strchr(fill_अन्तर्->f_Res_मार्गः, RES_मार्गः_SEPARATOR);
                    }
                    यदि (separator != शून्यसूचकः && separator[1] != '\0') {
                        create_मार्गः(orig_Res_मार्गः, orig_Res_मार्गः_Len, fill_अन्तर्->f_Res_मार्गः,
                                   static_cast<int32_t>(uprv_strlen(fill_अन्तर्->f_Res_मार्गः)), अन्तर्_कुञ्चिका, मार्गः, स्थितिः);
                    } अन्यथा {
                        create_मार्गः(orig_Res_मार्गः, orig_Res_मार्गः_Len, "", 0, अन्तर्_कुञ्चिका, मार्गः, स्थितिः);
                    }
                }
                ures_free_Res_मार्गः(fill_अन्तर्);
                ures_append_Res_मार्गः(fill_अन्तर्, मार्गः.दत्त(), मार्गः.length(), स्थितिः);
                यदि(fill_अन्तर्->f_Res_मार्गः[fill_अन्तर्->f_Res_मार्गः_Len-1] != RES_मार्गः_SEPARATOR) {
                    ures_append_Res_मार्गः(fill_अन्तर्, RES_मार्गः_SEPARATOR_S, 1, स्थितिः);
                }
            } अन्यथा {
                *स्थितिः = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            fill_अन्तर् = init_resb_परिणाम(resB->f_दत्त, res, कुञ्चिका, -1, resB, fill_अन्तर्, स्थितिः);
        }
    } 
    अन्यथा {
        *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    प्रत्यागम fill_अन्तर्;
}

नामाकाश {

शून्य get_All_वस्तूनि_With_Fallback(
        ध्रुव UResourceBundle *bundle, Resource_दत्त_मानम् &value,
        ResourceSink &sink, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
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
    value.समुच्चयः_दत्त(bundle->get_Res_दत्त());
    value.समुच्चयः_Valid_Locale_दत्त_प्रविष्टिः(bundle->f_Valid_Locale_दत्त_प्रविष्टिः);
    UResource_दत्त_प्रविष्टिः *parent_प्रविष्टिः = bundle->f_दत्त->fParent;
    UBool hasParent = parent_प्रविष्टिः != शून्यसूचकः && U_SUCCESS(parent_प्रविष्टिः->fBogus);
    value.समुच्चयः_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_कुञ्चिका, value, !hasParent, error_संहिता);
    यदि (hasParent) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        राशिः_UResource_Bundle parentBundle;
        UResourceBundle &parentRef = parentBundle.ref();
        parentRef.f_दत्त = parent_प्रविष्टिः;
        parentRef.f_Valid_Locale_दत्त_प्रविष्टिः = bundle->f_Valid_Locale_दत्त_प्रविष्टिः;
        parentRef.fHasFallback = !parentRef.get_Res_दत्त().noFallback;
        parentRef.f_Is_शिखरम्_Level = सत्य;
        parentRef.fRes = parentRef.get_Res_दत्त().मूलम्_Res;
        parentRef.f_आकार = res_गणनम्_Array_वस्तूनि(&parentRef.get_Res_दत्त(), parentRef.fRes);
        parentRef.f_सूचकांक = -1;
        प्रविष्टिः_Increase(parent_प्रविष्टिः);

        // Look up the container item in the parent bundle.
        राशिः_UResource_Bundle containerBundle;
        ध्रुव UResourceBundle *rb;
        UError_संहिता मार्गः_Error_संहिता = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        यदि (bundle->f_Res_मार्गः == शून्यसूचकः || *bundle->f_Res_मार्गः == 0) {
            rb = parentBundle.getAlias();
        } अन्यथा {
            rb = ures_get_By_कुञ्चिका_With_Fallback(parentBundle.getAlias(), bundle->f_Res_मार्गः,
                                           containerBundle.getAlias(), &मार्गः_Error_संहिता);
        }
        यदि (U_SUCCESS(मार्गः_Error_संहिता)) {
            get_All_वस्तूनि_With_Fallback(rb, value, sink, error_संहिता);
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
           UError_संहिता &error_संहिता) override {
        Resource_सारणी वस्तूनि_सारणी = value.get_सारणी(error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        प्रति (int32_t i = 0; वस्तूनि_सारणी.get_कुञ्चिका_And_मानम्(i, कुञ्चिका, value); ++i) {
            यदि (value.getType() == URES_ALIAS) {
                Resource_दत्त_मानम्& rdv = static_cast<Resource_दत्त_मानम्&>(value);
                राशिः_UResource_Bundle राशिः_अस्थायी_Bundle;
                UResourceBundle* aliasRB = getAliasTargetAsResourceBundle(rdv.get_दत्त(), rdv.getResource(), शून्यसूचकः, -1,
                                                                          rdv.get_Valid_Locale_दत्त_प्रविष्टिः(), शून्यसूचकः, 0,
                                                                          राशिः_अस्थायी_Bundle.getAlias(), &error_संहिता);
                यदि (U_SUCCESS(error_संहिता)) {
                    Resource_दत्त_मानम् aliased_मानम्;
                    aliased_मानम्.समुच्चयः_दत्त(aliasRB->get_Res_दत्त());
                    aliased_मानम्.समुच्चयः_Valid_Locale_दत्त_प्रविष्टिः(aliasRB->f_Valid_Locale_दत्त_प्रविष्टिः);
                    aliased_मानम्.समुच्चयः_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    यदि (aliased_मानम्.getType() != URES_सारणी) {
                        dest.put(कुञ्चिका, aliased_मानम्, is_मूलम्, error_संहिता);
                    } अन्यथा {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_मानम्_Type = URES_सारणी;
                        CharString सारणी_मार्गः;
                        सारणी_मार्गः.append(aliasRB->f_Res_मार्गः, error_संहिता);
                        ध्रुव वर्ण* parent_कुञ्चिका = कुञ्चिका; // dest.put() changes the key
                        dest.put(parent_कुञ्चिका, aliased_मानम्, is_मूलम्, error_संहिता);
                        UResource_दत्त_प्रविष्टिः* प्रविष्टिः = aliasRB->f_दत्त;
                        Resource res = aliasRB->fRes;
                        यावत् (aliased_मानम्_Type == URES_सारणी && प्रविष्टिः->fParent != शून्यसूचकः) {
                            CharString local_मार्गः;
                            local_मार्गः.copyFrom(सारणी_मार्गः, error_संहिता);
                            वर्ण* local_मार्गः_As_Char_Ptr = local_मार्गः.दत्त();
                            ध्रुव वर्ण* child_कुञ्चिका;
                            प्रविष्टिः = प्रविष्टिः->fParent;
                            res = प्रविष्टिः->f_दत्त.मूलम्_Res;
                            Resource newRes = res_findResource(&प्रविष्टिः->f_दत्त, res, &local_मार्गः_As_Char_Ptr, &child_कुञ्चिका);
                            यदि (newRes != RES_BOGUS) {
                                aliased_मानम्.समुच्चयः_दत्त(प्रविष्टिः->f_दत्त);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_मानम्.समुच्चयः_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_मानम्_Type = aliased_मानम्.getType();
                                यदि (aliased_मानम्_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_दत्त_मानम्& rdv2 = static_cast<Resource_दत्त_मानम्&>(aliased_मानम्);
                                    aliasRB = getAliasTargetAsResourceBundle(rdv2.get_दत्त(), rdv2.getResource(), शून्यसूचकः, -1,
                                                                             rdv2.get_Valid_Locale_दत्त_प्रविष्टिः(), शून्यसूचकः, 0,
                                                                             राशिः_अस्थायी_Bundle.getAlias(), &error_संहिता);
                                    सारणी_मार्गः.clear();
                                    सारणी_मार्गः.append(aliasRB->f_Res_मार्गः, error_संहिता);
                                    प्रविष्टिः = aliasRB->f_दत्त;
                                    res = aliasRB->fRes;
                                    aliased_मानम्.समुच्चयः_दत्त(प्रविष्टिः->f_दत्त);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_मानम्.समुच्चयः_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_मानम्_Type = aliased_मानम्.getType();
                                }
                                यदि (aliased_मानम्_Type == URES_सारणी) {
                                    dest.put(parent_कुञ्चिका, aliased_मानम्, is_मूलम्, error_संहिता);
                                } अन्यथा {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_संहिता = U_आन्तरिकम्_क्रमादेशः_ERROR;
                                    प्रत्यागम;
                                }
                            }
                        }
                    }
                }
            } अन्यथा {
                dest.put(कुञ्चिका, value, is_मूलम्, error_संहिता);
            }
            यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI शून्य U_EXPORT2
ures_getAllChildrenWithFallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *मार्गः,
                                icu::ResourceSink &sink, UError_संहिता &error_संहिता) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_वस्तूनि_With_Fallback(bundle, मार्गः, allChildrenSink, error_संहिता);
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
ures_get_मानम्_With_Fallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *मार्गः,
                          UResourceBundle *अस्थायी_Fill_अन्तर्,
                          Resource_दत्त_मानम् &value, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    यदि (मार्गः == शून्यसूचकः) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    ध्रुव UResourceBundle *rb;
    यदि (*मार्गः == 0) {
        // empty path
        rb = bundle;
    } अन्यथा {
        rb = ures_get_By_कुञ्चिका_With_Fallback(bundle, मार्गः, अस्थायी_Fill_अन्तर्, &error_संहिता);
        यदि (U_FAILURE(error_संहिता)) {
            प्रत्यागम;
        }
    }
    value.समुच्चयः_दत्त(rb->get_Res_दत्त());
    value.समुच्चयः_Valid_Locale_दत्त_प्रविष्टिः(rb->f_Valid_Locale_दत्त_प्रविष्टिः);
    value.समुच्चयः_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI शून्य U_EXPORT2
ures_get_All_वस्तूनि_With_Fallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *मार्गः,
                             icu::ResourceSink &sink, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    यदि (मार्गः == शून्यसूचकः) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    राशिः_UResource_Bundle राशिः_Bundle;
    ध्रुव UResourceBundle *rb;
    यदि (*मार्गः == 0) {
        // empty path
        rb = bundle;
    } अन्यथा {
        rb = ures_get_By_कुञ्चिका_With_Fallback(bundle, मार्गः, राशिः_Bundle.getAlias(), &error_संहिता);
        यदि (U_FAILURE(error_संहिता)) {
            प्रत्यागम;
        }
    }
    // Get all table items with fallback.
    Resource_दत्त_मानम् value;
    get_All_वस्तूनि_With_Fallback(rb, value, sink, error_संहिता);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_कुञ्चिका(ध्रुव UResourceBundle* resB,
              ध्रुव वर्ण* अन्तर्_कुञ्चिका,
              UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
              UError_संहिता* स्थितिः) {
    Resource res = RES_BOGUS;
    UResource_दत्त_प्रविष्टिः *दत्त_प्रविष्टिः = शून्यसूचकः;
    ध्रुव वर्ण *कुञ्चिका = अन्तर्_कुञ्चिका;

    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम fill_अन्तर्;
    }
    यदि(resB == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम fill_अन्तर्;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    यदि(URES_IS_सारणी(type)) {
        int32_t t;
        res = res_get_सारणी_Item_By_कुञ्चिका(&resB->get_Res_दत्त(), resB->fRes, &t, &कुञ्चिका);
        यदि(res == RES_BOGUS) {
            कुञ्चिका = अन्तर्_कुञ्चिका;
            यदि(resB->fHasFallback) {
                दत्त_प्रविष्टिः = get_Fallback_दत्त(resB, &कुञ्चिका, &res, स्थितिः);
                यदि(U_SUCCESS(*स्थितिः)) {
                    /* check if resB->fResPath gives the right name here */
                    प्रत्यागम init_resb_परिणाम(दत्त_प्रविष्टिः, res, कुञ्चिका, -1, resB, fill_अन्तर्, स्थितिः);
                } अन्यथा {
                    *स्थितिः = U_MISSING_RESOURCE_ERROR;
                }
            } अन्यथा {
                *स्थितिः = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            प्रत्यागम init_resb_परिणाम(resB->f_दत्त, res, कुञ्चिका, -1, resB, fill_अन्तर्, स्थितिः);
        }
    } 
#यदि 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    अन्यथा यदि(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == सत्य) {
        /* here should go a first attempt to locate the key using index table */
        दत्त_प्रविष्टिः = get_Fallback_दत्त(resB, &कुञ्चिका, &res, स्थितिः);
        यदि(U_SUCCESS(*स्थितिः)) {
            प्रत्यागम init_resb_परिणाम(दत्त_प्रविष्टिः, res, कुञ्चिका, resB, fill_अन्तर्, स्थितिः);
        } अन्यथा {
            *स्थितिः = U_MISSING_RESOURCE_ERROR;
        }
    }
#यदि_अन्तः    
    अन्यथा {
        *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम fill_अन्तर्;
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ures_get_String_By_कुञ्चिका(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                    ध्रुव वर्ण* अन्तर्_कुञ्चिका,
                    int32_t* len,
                    UError_संहिता* स्थितिः) {
    Resource res = RES_BOGUS;
    UResource_दत्त_प्रविष्टिः *दत्त_प्रविष्टिः = शून्यसूचकः;
    ध्रुव वर्ण* कुञ्चिका = अन्तर्_कुञ्चिका;

    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(resB == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    यदि(URES_IS_सारणी(type)) {
        int32_t t=0;

        res = res_get_सारणी_Item_By_कुञ्चिका(&resB->get_Res_दत्त(), resB->fRes, &t, &कुञ्चिका);

        यदि(res == RES_BOGUS) {
            कुञ्चिका = अन्तर्_कुञ्चिका;
            यदि(resB->fHasFallback) {
                दत्त_प्रविष्टिः = get_Fallback_दत्त(resB, &कुञ्चिका, &res, स्थितिः);
                यदि(U_SUCCESS(*स्थितिः)) {
                    विकल्प (RES_GET_TYPE(res)) {
                    पक्ष URES_STRING:
                    पक्ष URES_STRING_V2:
                        प्रत्यागम res_getString({resB, कुञ्चिका}, &दत्त_प्रविष्टिः->f_दत्त, res, len);
                    पक्ष URES_ALIAS:
                      {
                        ध्रुव char16_t* परिणाम = शून्यसूचकः;
                        UResourceBundle *अस्थायी_Res = ures_get_By_कुञ्चिका(resB, अन्तर्_कुञ्चिका, शून्यसूचकः, स्थितिः);
                        परिणाम = ures_getString(अस्थायी_Res, len, स्थितिः);
                        ures_close(अस्थायी_Res);
                        प्रत्यागम परिणाम;
                      }
                    स्वभाव:
                        *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
                    }
                } अन्यथा {
                    *स्थितिः = U_MISSING_RESOURCE_ERROR;
                }
            } अन्यथा {
                *स्थितिः = U_MISSING_RESOURCE_ERROR;
            }
        } अन्यथा {
            विकल्प (RES_GET_TYPE(res)) {
            पक्ष URES_STRING:
            पक्ष URES_STRING_V2:
                प्रत्यागम res_getString({resB, कुञ्चिका}, &resB->get_Res_दत्त(), res, len);
            पक्ष URES_ALIAS:
              {
                ध्रुव char16_t* परिणाम = शून्यसूचकः;
                UResourceBundle *अस्थायी_Res = ures_get_By_कुञ्चिका(resB, अन्तर्_कुञ्चिका, शून्यसूचकः, स्थितिः);
                परिणाम = ures_getString(अस्थायी_Res, len, स्थितिः);
                ures_close(अस्थायी_Res);
                प्रत्यागम परिणाम;
              }
            स्वभाव:
                *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#यदि 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    अन्यथा यदि(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == सत्य) {
        /* here should go a first attempt to locate the key using index table */
        दत्त_प्रविष्टिः = get_Fallback_दत्त(resB, &कुञ्चिका, &res, स्थितिः);
        यदि(U_SUCCESS(*स्थितिः)) {
            // TODO: Tracing
            प्रत्यागम res_getString(rd, res, len);
        } अन्यथा {
            *स्थितिः = U_MISSING_RESOURCE_ERROR;
        }
    } 
#यदि_अन्तः    
    अन्यथा {
        *स्थितिः = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम शून्यसूचकः;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_UTF8_String_By_कुञ्चिका(ध्रुव UResourceBundle* resB,
                        ध्रुव वर्ण* कुञ्चिका,
                        वर्ण* dest U_LIFETIME_BOUND,
                        int32_t* p_दीर्घता,
                        UBool forceCopy,
                        UError_संहिता* स्थितिः) {
    int32_t length16;
    ध्रुव char16_t *s16 = ures_get_String_By_कुञ्चिका(resB, कुञ्चिका, &length16, स्थितिः);
    प्रत्यागम ures_गन्तव्यम्_UTF8_String(s16, length16, dest, p_दीर्घता, forceCopy, स्थितिः);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_Locale_आन्तरिकम्(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_संहिता* स्थितिः)
{
    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (!resourceBundle) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा {
      प्रत्यागम resourceBundle->f_दत्त->fName;
    }
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getLocale(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_संहिता* स्थितिः)
{
  प्रत्यागम ures_get_Locale_आन्तरिकम्(resourceBundle, स्थितिः);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getLocaleByType(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_दत्त_Locale_Type type,
                     UError_संहिता* स्थितिः) {
    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (!resourceBundle) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा {
        विकल्प(type) {
        पक्ष ULOC_ACTUAL_LOCALE:
            प्रत्यागम resourceBundle->f_दत्त->fName;
        पक्ष ULOC_VALID_LOCALE:
            प्रत्यागम resourceBundle->f_Valid_Locale_दत्त_प्रविष्टिः->fName;
        पक्ष ULOC_REQUESTED_LOCALE:
        स्वभाव:
            *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
    }
}

U_CFUNC ध्रुव वर्ण*
ures_getName(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND) {
  यदि(resB == शून्यसूचकः) {
    प्रत्यागम शून्यसूचकः;
  }

  प्रत्यागम resB->f_दत्त->fName;
}

#यदि_परिभाषितम् URES_DEBUG
U_CFUNC ध्रुव वर्ण*
ures_get_मार्गः(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND) {
  यदि(resB == शून्यसूचकः) {
    प्रत्यागम शून्यसूचकः;
  }

  प्रत्यागम resB->f_दत्त->f_मार्गः;
}
#यदि_अन्तः

स्थिर UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ध्रुव वर्ण* मार्गः,
                  ध्रुव वर्ण* localeID,
                  UResOpenType openType,
                  UError_संहिता* स्थितिः) {
    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    UResource_दत्त_प्रविष्टिः *प्रविष्टिः;
    यदि(openType != URES_OPEN_प्रत्यक्षम्) {
        यदि (localeID == शून्यसूचकः) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_आधारः_Name(localeID, *स्थितिः);
        यदि(U_FAILURE(*स्थितिः)) {
            *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        प्रविष्टिः = प्रविष्टिः_Open(मार्गः, canonLocaleID.दत्त(), openType, स्थितिः);
    } अन्यथा {
        प्रविष्टिः = प्रविष्टिः_Open_प्रत्यक्षम्(मार्गः, localeID, स्थितिः);
    }
    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(प्रविष्टिः == शून्यसूचकः) {
        *स्थितिः = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    UBool is_राशिः_Object;
    यदि(r == शून्यसूचकः) {
        r = static_cast<UResourceBundle*>(uprv_malloc(परिमाण(UResourceBundle)));
        यदि(r == शून्यसूचकः) {
            प्रविष्टिः_Close(प्रविष्टिः);
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        is_राशिः_Object = असत्य;
    } अन्यथा {  // fill-in
        is_राशिः_Object = ures_is_राशिः_Object(r);
        ures_closeBundle(r, असत्य);
    }
    uprv_memset(r, 0, परिमाण(UResourceBundle));
    ures_समुच्चयः_Is_राशिः_Object(r, is_राशिः_Object);

    r->f_Valid_Locale_दत्त_प्रविष्टिः = r->f_दत्त = प्रविष्टिः;
    r->fHasFallback = openType != URES_OPEN_प्रत्यक्षम् && !r->get_Res_दत्त().noFallback;
    r->f_Is_शिखरम्_Level = सत्य;
    r->fRes = r->get_Res_दत्त().मूलम्_Res;
    r->f_आकार = res_गणनम्_Array_वस्तूनि(&r->get_Res_दत्त(), r->fRes);
    r->f_सूचकांक = -1;

    ResourceTracer(r).traceOpen();

    प्रत्यागम r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* localeID, UError_संहिता* स्थितिः) {
    प्रत्यागम ures_openWithType(शून्यसूचकः, मार्गः, localeID, URES_OPEN_LOCALE_DEFAULT_मूलम्, स्थितिः);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* localeID, UError_संहिता* स्थितिः) {
    प्रत्यागम ures_openWithType(शून्यसूचकः, मार्गः, localeID, URES_OPEN_LOCALE_मूलम्, स्थितिः);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_प्रत्यक्षम्(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* localeID, UError_संहिता* स्थितिः) {
    प्रत्यागम ures_openWithType(शून्यसूचकः, मार्गः, localeID, URES_OPEN_प्रत्यक्षम्, स्थितिः);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI शून्य U_EXPORT2
ures_open_Fill_अन्तर्(UResourceBundle *r, ध्रुव वर्ण* मार्गः,
                ध्रुव वर्ण* localeID, UError_संहिता* स्थितिः) {
    यदि(U_SUCCESS(*स्थितिः) && r == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    ures_openWithType(r, मार्गः, localeID, URES_OPEN_LOCALE_DEFAULT_मूलम्, स्थितिः);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI शून्य U_EXPORT2
ures_open_प्रत्यक्षम्_Fill_अन्तर्(UResourceBundle *r, ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* localeID, UError_संहिता* स्थितिः) {
    यदि(U_SUCCESS(*स्थितिः) && r == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    ures_openWithType(r, मार्गः, localeID, URES_OPEN_प्रत्यक्षम्, स्थितिः);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_गणनम्_Array_वस्तूनि(ध्रुव UResourceBundle* resourceBundle,
                  ध्रुव वर्ण* resource_कुञ्चिका,
                  UError_संहिता* स्थितिः)
{
    UResourceBundle res_दत्त;
    ures_init_राशिः_Object(&res_दत्त);
    यदि (स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि(resourceBundle == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    ures_get_By_कुञ्चिका(resourceBundle, resource_कुञ्चिका, &res_दत्त, स्थितिः);
    
    यदि(res_दत्त.get_Res_दत्त().दत्त != शून्यसूचकः) {
        int32_t परिणाम = res_गणनम्_Array_वस्तूनि(&res_दत्त.get_Res_दत्त(), res_दत्त.fRes);
        ures_close(&res_दत्त);
        प्रत्यागम परिणाम;
    } अन्यथा {
        *स्थितिः = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_दत्त);
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
ures_get_संस्करणम्_सङ्ख्या_आन्तरिकम्(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    यदि (!resourceBundle) प्रत्यागम शून्यसूचकः;

    यदि(resourceBundle->f_संस्करणम् == शून्यसूचकः) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ध्रुव char16_t* minor_संस्करणम् = ures_get_String_By_कुञ्चिका(resourceBundle, k_संस्करणम्_चिह्नकम्, &minor_len, &स्थितिः);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_संस्करणम् = (वर्ण *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        यदि (((UResourceBundle *)resourceBundle)->f_संस्करणम् == शून्यसूचकः) {
            प्रत्यागम शून्यसूचकः;
        }
       
        यदि(minor_len > 0) {
            u_UChars_गन्तव्यम्_Chars(minor_संस्करणम्, resourceBundle->f_संस्करणम् , minor_len);
            resourceBundle->f_संस्करणम्[len] =  '\0';
        }
        अन्यथा {
            uprv_strcpy(resourceBundle->f_संस्करणम्, k_Default_Minor_संस्करणम्);
        }
    }

    प्रत्यागम resourceBundle->f_संस्करणम्;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_संस्करणम्_सङ्ख्या(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    प्रत्यागम ures_get_संस्करणम्_सङ्ख्या_आन्तरिकम्(resourceBundle);
}

U_CAPI शून्य U_EXPORT2 ures_get_संस्करणम्(ध्रुव UResourceBundle* resB, UVersionInfo संस्करणम्_Info) {
    यदि (!resB) प्रत्यागम;

    u_संस्करणम्_From_String(संस्करणम्_Info, ures_get_संस्करणम्_सङ्ख्या_आन्तरिकम्(resB));
}

/** Tree support functions *******************************/
#परिभाषय सूचकांक_LOCALE_NAME "res_index"
#परिभाषय सूचकांक_चिह्नकम्         "InstalledLocales"
#परिभाषय DEFAULT_चिह्नकम्       "default"

#यदि defined(URES_वृक्षः_DEBUG)
#अन्तर्भाव <stdio.h>
#यदि_अन्तः

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
ures_loc_गणनम्_Locales(UEnumeration *en, UError_संहिता * /*स्थितिः*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    प्रत्यागम ures_get_आकार(&ctx->installed);
}

U_CDECL_BEGIN


स्थिर ध्रुव वर्ण * U_CALLCONV
ures_loc_अग्र_Locale(UEnumeration* en,
                    int32_t* परिणाम_दीर्घता,
                    UError_संहिता* स्थितिः) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = शून्यसूचकः;
    ध्रुव वर्ण *परिणाम = शून्यसूचकः;
    int32_t len = 0;
    यदि (ures_has_अग्र(res) && (k = ures_get_अग्र_Resource(res, &ctx->curr, स्थितिः)) != शून्यसूचकः) {
        परिणाम = ures_get_कुञ्चिका(k);
        len = (int32_t)uprv_strlen(परिणाम);
    }
    यदि (परिणाम_दीर्घता) {
        *परिणाम_दीर्घता = len;
    }
    प्रत्यागम परिणाम;
}

स्थिर शून्य U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_संहिता* /*स्थितिः*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_अन्तः

स्थिर ध्रुव UEnumeration gLocalesEnum = {
    शून्यसूचकः,
        शून्यसूचकः,
        ures_loc_closeLocales,
        ures_loc_गणनम्_Locales,
        uenum_unextDefault,
        ures_loc_अग्र_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_उपलब्धम्_Locales(ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः)
{
    UResourceBundle *idx = शून्यसूचकः;
    UEnumeration *en = शून्यसूचकः;
    ULocalesContext *myContext = शून्यसूचकः;

    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(परिमाण(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(परिमाण(UEnumeration));
    यदि(!en || !myContext) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(en, &gLocalesEnum, परिमाण(UEnumeration));

    ures_init_राशिः_Object(&myContext->installed);
    ures_init_राशिः_Object(&myContext->curr);
    idx = ures_open_प्रत्यक्षम्(मार्गः, सूचकांक_LOCALE_NAME, स्थितिः);
    ures_get_By_कुञ्चिका(idx, सूचकांक_चिह्नकम्, &myContext->installed, स्थितिः);
    यदि(U_SUCCESS(*स्थितिः)) {
#यदि defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            मार्गः, सूचकांक_LOCALE_NAME, सूचकांक_चिह्नकम्, ures_get_कुञ्चिका(&myContext->installed));
#यदि_अन्तः
        en->context = myContext;
    } अन्यथा {
#यदि defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", मार्गः, u_errorName(*स्थितिः));
#यदि_अन्तः
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = शून्यसूचकः;
    }
    
    ures_close(idx);
    
    प्रत्यागम en;
}

स्थिर UBool is_Locale_अन्तर्_List(UEnumeration *locEnum, ध्रुव वर्ण *loc_गन्तव्यम्_अन्वेषणम्, UError_संहिता *स्थितिः) {
    ध्रुव वर्ण *loc;
    यावत् ((loc = uenum_अग्र(locEnum, शून्यसूचकः, स्थितिः)) != शून्यसूचकः) {
        यदि (uprv_strcmp(loc, loc_गन्तव्यम्_अन्वेषणम्) == 0) {
            प्रत्यागम सत्य;
        }
    }
    प्रत्यागम असत्य;
}

स्थिर शून्य getParentForFunctionalEquivalent(ध्रुव वर्ण*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      parent) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
    parent.clear();
    यदि (res != शून्यसूचकः) {
        ures_get_By_कुञ्चिका(res, "%%Parent", bund1, &sub_स्थितिः);
        यदि (U_SUCCESS(sub_स्थितिः)) {
            int32_t length16;
            ध्रुव char16_t* s16 = ures_getString(bund1, &length16, &sub_स्थितिः);
            parent.appendInvariantChars(s16, length16, sub_स्थितिः);
        }
    }
    
    // If none there, use normal truncation parent
    यदि (U_FAILURE(sub_स्थितिः) || parent.is_रिक्तानि()) {
        sub_स्थितिः = U_ZERO_ERROR;
        parent = ulocimp_getParent(localeID, sub_स्थितिः);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(वर्ण *परिणाम, int32_t परिणाम_क्षमता,
                             ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *resName, ध्रुव वर्ण *keyword, ध्रुव वर्ण *locid,
                             UBool *is_उपलब्धम्, UBool omitDefault, UError_संहिता *स्थितिः)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString प्राप्तम्;
    CharString parent;
    CharString पूर्णम्;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = शून्यसूचकः;
    UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
    int32_t length = 0;
    यदि(U_FAILURE(*स्थितिः)) प्रत्यागम 0;
    CharString kwVal;
    यदि (keyword != शून्यसूचकः && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_मानम्(locid, keyword, sub_स्थितिः);
        यदि (kwVal == DEFAULT_चिह्नकम्) {
            kwVal.clear();
        }
    }
    यदि (locid == शून्यसूचकः) {
        locid = uloc_getDefault();
    }
    CharString आधारः = ulocimp_get_आधारः_Name(locid, sub_स्थितिः);
#यदि defined(URES_वृक्षः_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.दत्त(), आधारः.दत्त(), u_errorName(sub_स्थितिः));
#यदि_अन्तः
    ures_init_राशिः_Object(&bund1);
    ures_init_राशिः_Object(&bund2);

    parent.copyFrom(आधारः, sub_स्थितिः);
    प्राप्तम्.copyFrom(आधारः, sub_स्थितिः);

    यदि(is_उपलब्धम्) {
        UEnumeration *locEnum = ures_open_उपलब्धम्_Locales(मार्गः, &sub_स्थितिः);
        *is_उपलब्धम् = सत्य;
        यदि (U_SUCCESS(sub_स्थितिः)) {
            *is_उपलब्धम् = is_Locale_अन्तर्_List(locEnum, parent.दत्त(), &sub_स्थितिः);
        }
        uenum_close(locEnum);
    }

    यदि(U_FAILURE(sub_स्थितिः)) {
        *स्थितिः = sub_स्थितिः;
        प्रत्यागम 0;
    }
    
    कुरु {
        sub_स्थितिः = U_ZERO_ERROR;
        res = ures_open(मार्गः, parent.दत्त(), &sub_स्थितिः);
        यदि(((sub_स्थितिः == U_USING_FALLBACK_WARNING) ||
            (sub_स्थितिः == U_USING_DEFAULT_WARNING)) && is_उपलब्धम्)
        {
            *is_उपलब्धम् = असत्य;
        }
        is_उपलब्धम् = शून्यसूचकः; /* only want to set this the first time around */
        
#यदि defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", मार्गः?मार्गः:"ICUDATA", parent.दत्त(), u_errorName(sub_स्थितिः), ures_getLocale(res, &sub_स्थितिः));
#यदि_अन्तः
        यदि(U_FAILURE(sub_स्थितिः)) {
            *स्थितिः = sub_स्थितिः;
        } अन्यथा यदि(sub_स्थितिः == U_ZERO_ERROR) {
            ures_get_By_कुञ्चिका(res,resName,&bund1, &sub_स्थितिः);
            यदि(sub_स्थितिः == U_ZERO_ERROR) {
                ध्रुव char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#यदि defined(URES_वृक्षः_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    मार्गः?मार्गः:"ICUDATA", parent.दत्त(), u_errorName(sub_स्थितिः));
#यदि_अन्तः
                defUstr = ures_get_String_By_कुञ्चिका(&bund1, DEFAULT_चिह्नकम्, &defLen, &sub_स्थितिः);
                यदि(U_SUCCESS(sub_स्थितिः) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_स्थितिः);
#यदि defined(URES_वृक्षः_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        मार्गः?मार्गः:"ICUDATA", parent.दत्त(), keyword, defVal.दत्त(), u_errorName(sub_स्थितिः));
#यदि_अन्तः
                    defLoc.copyFrom(parent, sub_स्थितिः);
                    यदि(kwVal.is_रिक्तानि()) {
                        kwVal.append(defVal, sub_स्थितिः);
#यदि defined(URES_वृक्षः_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            मार्गः?मार्गः:"ICUDATA", parent.दत्त(), keyword, kwVal.दत्त());
#यदि_अन्तः
                    }
                }
            }
        }
        
        sub_स्थितिः = U_ZERO_ERROR;

        यदि (res != शून्यसूचकः) {
            प्राप्तम्.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_स्थितिः), sub_स्थितिः);
        }

        यदि (प्राप्तम् != parent) {
            parent.copyFrom(प्राप्तम्, sub_स्थितिः);
        } अन्यथा {
            getParentForFunctionalEquivalent(प्राप्तम्.दत्त(),res,&bund1,parent);
        }
        ures_close(res);
    } यावत्(defVal.is_रिक्तानि() && !प्राप्तम्.is_रिक्तानि() && प्राप्तम् != "root" && U_SUCCESS(*स्थितिः));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    parent.copyFrom(आधारः, sub_स्थितिः);
    प्राप्तम्.copyFrom(आधारः, sub_स्थितिः);

    कुरु {
        res = ures_open(मार्गः, parent.दत्त(), &sub_स्थितिः);
        यदि((sub_स्थितिः == U_USING_FALLBACK_WARNING) && is_उपलब्धम्) {
            *is_उपलब्धम् = असत्य;
        }
        is_उपलब्धम् = शून्यसूचकः; /* only want to set this the first time around */
        
#यदि defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            मार्गः?मार्गः:"ICUDATA", parent.दत्त(), u_errorName(sub_स्थितिः), kwVal.दत्त());
#यदि_अन्तः
        यदि(U_FAILURE(sub_स्थितिः)) {
            *स्थितिः = sub_स्थितिः;
        } अन्यथा यदि(sub_स्थितिः == U_ZERO_ERROR) {
            ures_get_By_कुञ्चिका(res,resName,&bund1, &sub_स्थितिः);
#यदि defined(URES_वृक्षः_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_स्थितिः));
#यदि_अन्तः
            यदि(sub_स्थितिः == U_ZERO_ERROR) {
                ures_get_By_कुञ्चिका(&bund1, kwVal.दत्त(), &bund2, &sub_स्थितिः);
#यदि defined(URES_वृक्षः_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.दत्त(), u_errorName(sub_स्थितिः));
#यदि_अन्तः
                यदि(sub_स्थितिः == U_ZERO_ERROR) {
#यदि defined(URES_वृक्षः_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        मार्गः?मार्गः:"ICUDATA", parent.दत्त(), keyword, kwVal.दत्त(), u_errorName(sub_स्थितिः));
#यदि_अन्तः
                    यदि (parent.is_रिक्तानि()) {
                        पूर्णम्.clear().append("root", sub_स्थितिः);
                    } अन्यथा {
                        पूर्णम्.copyFrom(parent, sub_स्थितिः);
                    }
                        /* now, recalculate default kw if need be */
                        यदि(defLoc.length() > पूर्णम्.length()) {
                          ध्रुव char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#यदि defined(URES_वृक्षः_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    मार्गः?मार्गः:"ICUDATA", पूर्णम्.दत्त());
#यदि_अन्तः
                          defUstr = ures_get_String_By_कुञ्चिका(&bund1, DEFAULT_चिह्नकम्, &defLen, &sub_स्थितिः);
                          यदि(U_SUCCESS(sub_स्थितिः) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_स्थितिः);
#यदि defined(URES_वृक्षः_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    मार्गः?मार्गः:"ICUDATA", पूर्णम्.दत्त(), keyword, defVal.दत्त(), u_errorName(sub_स्थितिः));
#यदि_अन्तः
                            defLoc.copyFrom(पूर्णम्, sub_स्थितिः);
                          }
                        } /* end of recalculate default KW */
#यदि defined(URES_वृक्षः_DEBUG)
                        अन्यथा {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.दत्त(), पूर्णम्.दत्त());
                        }
#यदि_अन्तः
                } अन्यथा {
#यदि defined(URES_वृक्षः_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_स्थितिः), parent.दत्त(), kwVal.दत्त());
#यदि_अन्तः
                }
            }
        }
        
        sub_स्थितिः = U_ZERO_ERROR;
        UBool have_प्राप्तम् = असत्य;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        यदि (res != शून्यसूचकः && uprv_strcmp(resName, "collations") == 0) {
            ध्रुव वर्ण *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_स्थितिः);
            यदि (U_SUCCESS(sub_स्थितिः) && validLoc != शून्यसूचकः && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_स्थितिः);
                CharString parentLang = ulocimp_getLanguage(parent.गन्तव्यम्_String_Piece(), sub_स्थितिः);
                यदि (U_SUCCESS(sub_स्थितिः) && validLang != parentLang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    प्राप्तम्.clear().append(validLoc, sub_स्थितिः);
                    have_प्राप्तम् = सत्य;
                }
            }
            sub_स्थितिः = U_ZERO_ERROR;
        }
        यदि (!have_प्राप्तम्) {
            प्राप्तम्.copyFrom(parent, sub_स्थितिः);
        }

        getParentForFunctionalEquivalent(प्राप्तम्.दत्त(),res,&bund1,parent);
        ures_close(res);
        sub_स्थितिः = U_ZERO_ERROR;
    } यावत्(पूर्णम्.is_रिक्तानि() && !प्राप्तम्.is_रिक्तानि() && U_SUCCESS(*स्थितिः));

    यदि(पूर्णम्.is_रिक्तानि() && kwVal != defVal) {
#यदि defined(URES_वृक्षः_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.दत्त(), defVal.दत्त());
#यदि_अन्तः
        kwVal.clear().append(defVal, sub_स्थितिः);
        parent.copyFrom(आधारः, sub_स्थितिः);
        प्राप्तम्.copyFrom(आधारः, sub_स्थितिः);

        कुरु { /* search for 'default' named item */
            res = ures_open(मार्गः, parent.दत्त(), &sub_स्थितिः);
            यदि((sub_स्थितिः == U_USING_FALLBACK_WARNING) && is_उपलब्धम्) {
                *is_उपलब्धम् = असत्य;
            }
            is_उपलब्धम् = शून्यसूचकः; /* only want to set this the first time around */
            
#यदि defined(URES_वृक्षः_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                मार्गः?मार्गः:"ICUDATA", parent.दत्त(), u_errorName(sub_स्थितिः), kwVal.दत्त());
#यदि_अन्तः
            यदि(U_FAILURE(sub_स्थितिः)) {
                *स्थितिः = sub_स्थितिः;
            } अन्यथा यदि(sub_स्थितिः == U_ZERO_ERROR) {
                ures_get_By_कुञ्चिका(res,resName,&bund1, &sub_स्थितिः);
                यदि(sub_स्थितिः == U_ZERO_ERROR) {
                    ures_get_By_कुञ्चिका(&bund1, kwVal.दत्त(), &bund2, &sub_स्थितिः);
                    यदि(sub_स्थितिः == U_ZERO_ERROR) {
#यदि defined(URES_वृक्षः_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", मार्गः?मार्गः:"ICUDATA",
                            parent.दत्त(), keyword, kwVal.दत्त(), u_errorName(sub_स्थितिः));
#यदि_अन्तः
                        यदि (parent.is_रिक्तानि()) {
                            पूर्णम्.clear().append("root", sub_स्थितिः);
                        } अन्यथा {
                            पूर्णम्.copyFrom(parent, sub_स्थितिः);
                        }
                        
                        /* now, recalculate default kw if need be */
                        यदि(defLoc.length() > पूर्णम्.length()) {
                          ध्रुव char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#यदि defined(URES_वृक्षः_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    मार्गः?मार्गः:"ICUDATA", पूर्णम्.दत्त());
#यदि_अन्तः
                          defUstr = ures_get_String_By_कुञ्चिका(&bund1, DEFAULT_चिह्नकम्, &defLen, &sub_स्थितिः);
                          यदि(U_SUCCESS(sub_स्थितिः) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_स्थितिः);
#यदि defined(URES_वृक्षः_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    मार्गः?मार्गः:"ICUDATA", पूर्णम्.दत्त(), keyword, defVal.दत्त(), u_errorName(sub_स्थितिः));
#यदि_अन्तः
                            defLoc.copyFrom(पूर्णम्, sub_स्थितिः);
                          }
                        } /* end of recalculate default KW */
#यदि defined(URES_वृक्षः_DEBUG)
                        अन्यथा {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.दत्त(), पूर्णम्.दत्त());
                        }
#यदि_अन्तः
                    }
                }
            }
            
            sub_स्थितिः = U_ZERO_ERROR;
            प्राप्तम्.copyFrom(parent, sub_स्थितिः);
            getParentForFunctionalEquivalent(प्राप्तम्.दत्त(),res,&bund1,parent);
            ures_close(res);
            sub_स्थितिः = U_ZERO_ERROR;
        } यावत्(पूर्णम्.is_रिक्तानि() && !प्राप्तम्.is_रिक्तानि() && U_SUCCESS(*स्थितिः));
    }
    
    यदि(U_SUCCESS(*स्थितिः)) {
        यदि(पूर्णम्.is_रिक्तानि()) {
#यदि defined(URES_वृक्षः_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.दत्त());
#यदि_अन्तः
          *स्थितिः = U_MISSING_RESOURCE_ERROR;
        } अन्यथा यदि(omitDefault) {
#यदि defined(URES_वृक्षः_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", पूर्णम्.दत्त(), defLoc.दत्त(), प्राप्तम्.दत्त());
#यदि_अन्तः        
          यदि(defLoc.length() <= पूर्णम्.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            यदि(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#यदि defined(URES_वृक्षः_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.दत्त());
#यदि_अन्तः
              kwVal.clear();
            }
          }
        }
        प्राप्तम्.copyFrom(पूर्णम्, sub_स्थितिः);
        यदि(!kwVal.is_रिक्तानि()) {
            प्राप्तम्
                .append("@", sub_स्थितिः)
                .append(keyword, sub_स्थितिः)
                .append("=", sub_स्थितिः)
                .append(kwVal, sub_स्थितिः);
        } अन्यथा यदि(!omitDefault) {
            प्राप्तम्
                .append("@", sub_स्थितिः)
                .append(keyword, sub_स्थितिः)
                .append("=", sub_स्थितिः)
                .append(defVal, sub_स्थितिः);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = प्राप्तम्.length();

    यदि(U_SUCCESS(*स्थितिः)) {
        int32_t copy_दीर्घता = uprv_min(length, परिणाम_क्षमता);
        यदि(copy_दीर्घता>0) {
            प्राप्तम्.extract(परिणाम, copy_दीर्घता, sub_स्थितिः);
        }
        यदि(length == 0) {
          *स्थितिः = U_MISSING_RESOURCE_ERROR; 
        }
    } अन्यथा {
        length = 0;
        परिणाम[0]=0;
    }
    प्रत्यागम u_terminateChars(परिणाम, परिणाम_क्षमता, length, स्थितिः);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_मानानि(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *keyword, UError_संहिता *स्थितिः)
{
#परिभाषय मानानि_BUF_आकार 2048
#परिभाषय मानानि_LIST_आकार 512
    
    वर्ण       मानानि_Buf[मानानि_BUF_आकार];
    int32_t    मानानि_सूचकांक = 0;
    ध्रुव वर्ण *मानानि_List[मानानि_LIST_आकार];
    int32_t    मानानि_गणनम् = 0;
    
    ध्रुव वर्ण *locale;
    int32_t     locLen;
    
    UEnumeration *locs = शून्यसूचकः;
    
    UResourceBundle    item;
    UResourceBundle    subItem;
    
    ures_init_राशिः_Object(&item);
    ures_init_राशिः_Object(&subItem);
    locs = ures_open_उपलब्धम्_Locales(मार्गः, स्थितिः);
    
    यदि(U_FAILURE(*स्थितिः)) {
        ures_close(&item);
        ures_close(&subItem);
        प्रत्यागम शून्यसूचकः;
    }
    
    मानानि_Buf[0]=0;
    मानानि_Buf[1]=0;

    यावत् ((locale = uenum_अग्र(locs, &locLen, स्थितिः)) != शून्यसूचकः) {
        UResourceBundle   *bund = शून्यसूचकः;
        UResourceBundle   *subPtr = शून्यसूचकः;
        UError_संहिता sub_स्थितिः = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(मार्गः, locale, &sub_स्थितिः);
        
#यदि defined(URES_वृक्षः_DEBUG)
        यदि(!bund || U_FAILURE(sub_स्थितिः)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                मार्गः?मार्गः:"<ICUDATA>", keyword, locale, u_errorName(sub_स्थितिः));
        }
#यदि_अन्तः
        
        ures_get_By_कुञ्चिका(bund, keyword, &item, &sub_स्थितिः);
        
        यदि(!bund || U_FAILURE(sub_स्थितिः)) {
#यदि defined(URES_वृक्षः_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                मार्गः?मार्गः:"<ICUDATA>", keyword, locale, u_errorName(sub_स्थितिः));
#यदि_अन्तः
            ures_close(bund);
            bund = शून्यसूचकः;
            अनुवर्त;
        }

        यावत् ((subPtr = ures_get_अग्र_Resource(&item, &subItem, &sub_स्थितिः)) != शून्यसूचकः
            && U_SUCCESS(sub_स्थितिः)) {
            ध्रुव वर्ण *k;
            int32_t i;
            k = ures_get_कुञ्चिका(subPtr);

#यदि defined(URES_वृक्षः_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#यदि_अन्तः
            यदि(k == शून्यसूचकः || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_चिह्नकम्) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                अनुवर्त;
            }
            प्रति(i=0; i<मानानि_गणनम्; i++) {
                यदि(!uprv_strcmp(मानानि_List[i],k)) {
                    k = शून्यसूचकः; /* found duplicate */
                    भङ्ग;
                }
            }
            यदि(k != शून्यसूचकः) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                यदि((मानानि_गणनम् >= (मानानि_LIST_आकार-1)) ||       /* no more space in list .. */
                    ((मानानि_सूचकांक+kLen+1+1) >= मानानि_BUF_आकार)) { /* no more space in buffer (string + 2 nulls) */
                    *स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* out of space.. */
                } अन्यथा {
                    uprv_strcpy(मानानि_Buf+मानानि_सूचकांक, k);
                    मानानि_List[मानानि_गणनम्++] = मानानि_Buf+मानानि_सूचकांक;
                    मानानि_सूचकांक += kLen;
#यदि defined(URES_वृक्षः_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        मार्गः?मार्गः:"<ICUDATA>", keyword, locale, k);
#यदि_अन्तः
                    मानानि_Buf[मानानि_सूचकांक++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    मानानि_Buf[मानानि_सूचकांक++] = 0; /* terminate */
    
    ures_close(&item);
    ures_close(&subItem);
    uenum_close(locs);
#यदि defined(URES_वृक्षः_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*स्थितिः), 
        मानानि_सूचकांक, मानानि_गणनम्);
#यदि_अन्तः
    प्रत्यागम uloc_openKeywordList(मानानि_Buf, मानानि_सूचकांक, स्थितिः);
}
#यदि 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ध्रुव UResourceBundle* res1, ध्रुव UResourceBundle* res2){
    यदि(res1==शून्यसूचकः || res2==शून्यसूचकः){
        प्रत्यागम res1==res2; /* pointer comparison */
    }
    यदि(res1->f_कुञ्चिका==शून्यसूचकः||  res2->f_कुञ्चिका==शून्यसूचकः){
        प्रत्यागम (res1->f_कुञ्चिका==res2->f_कुञ्चिका);
    }अन्यथा{
        यदि(uprv_strcmp(res1->f_कुञ्चिका, res2->f_कुञ्चिका)!=0){
            प्रत्यागम असत्य;
        }
    }
    यदि(uprv_strcmp(res1->f_दत्त->fName, res2->f_दत्त->fName)!=0){
        प्रत्यागम असत्य;
    }
    यदि(res1->f_दत्त->f_मार्गः == शून्यसूचकः||  res2->f_दत्त->f_मार्गः==शून्यसूचकः){
        प्रत्यागम (res1->f_दत्त->f_मार्गः == res2->f_दत्त->f_मार्गः);
    }अन्यथा{
        यदि(uprv_strcmp(res1->f_दत्त->f_मार्गः, res2->f_दत्त->f_मार्गः)!=0){
            प्रत्यागम असत्य;
        }
    }
    यदि(uprv_strcmp(res1->f_दत्त->fParent->fName, res2->f_दत्त->fParent->fName)!=0){
        प्रत्यागम असत्य;
    }
    यदि(uprv_strcmp(res1->f_दत्त->fParent->f_मार्गः, res2->f_दत्त->fParent->f_मार्गः)!=0){
        प्रत्यागम असत्य;
    }
    यदि(uprv_strncmp(res1->f_Res_मार्गः, res2->f_Res_मार्गः, res1->f_Res_मार्गः_Len)!=0){
        प्रत्यागम असत्य;
    }
    यदि(res1->fRes != res2->fRes){
        प्रत्यागम असत्य;
    }
    प्रत्यागम सत्य;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ध्रुव UResourceBundle* res, UError_संहिता* स्थितिः){
    UResourceBundle* bundle = शून्यसूचकः;
    UResourceBundle* ret = शून्यसूचकः;
    यदि(U_FAILURE(*स्थितिः) || res == शून्यसूचकः){
        प्रत्यागम शून्यसूचकः;
    }
    bundle = ures_open(res->f_दत्त->f_मार्गः, res->f_दत्त->fName, स्थितिः);
    यदि(res->f_Res_मार्गः!=शून्यसूचकः){
        ret = ures_findSubResource(bundle, res->f_Res_मार्गः, शून्यसूचकः, स्थितिः);
        ures_close(bundle);
    }अन्यथा{
        ret = bundle;
    }
    प्रत्यागम ret;
}
U_CAPI ध्रुव UResourceBundle* U_EXPORT2
ures_getParentBundle(ध्रुव UResourceBundle* res){
    यदि(res==शून्यसूचकः){
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम res->fParentRes;
}
#यदि_अन्तः

U_CAPI शून्य U_EXPORT2
ures_get_संस्करणम्_By_कुञ्चिका(ध्रुव UResourceBundle* res, ध्रुव वर्ण *कुञ्चिका, UVersionInfo ver, UError_संहिता *स्थितिः) {
  ध्रुव char16_t *str;
  int32_t len;
  str = ures_get_String_By_कुञ्चिका(res, कुञ्चिका, &len, स्थितिः);
  यदि(U_SUCCESS(*स्थितिः)) {
    u_संस्करणम्_From_UString(ver, str);
  } 
}

/* eof */
