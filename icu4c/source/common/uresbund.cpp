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

#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "cwchar.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "unicode/uenum.h"
#అన్తర్భావ "uenumimp.h"
#అన్తర్భావ "ulocimp.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uresdata.h"

ఉపయోగించి నామాకాశ icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
స్థిర UHashtable *cache = శూన్యసూచిక;
స్థిర icu::UInitOnce gCacheInitOnce {};

స్థిర UMutex resbMutex;

/* INTERNAL: hashes an entry  */
స్థిర int32_t U_CALLCONV hash_నమోదు(స్థిరరాశి UHashTok parm) {
    UResource_దత్తాంశం_నమోదు* b = static_cast<UResource_దత్తాంశం_నమోదు*>(parm.సూచిక);
    UHashTok namekey, pathkey;
    namekey.సూచిక = b->fName;
    pathkey.సూచిక = b->f_మార్గం;
    తిరిగి uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
స్థిర UBool U_CALLCONV compareEntries(స్థిరరాశి UHashTok p1, స్థిరరాశి UHashTok p2) {
    UResource_దత్తాంశం_నమోదు* b1 = static_cast<UResource_దత్తాంశం_నమోదు*>(p1.సూచిక);
    UResource_దత్తాంశం_నమోదు* b2 = static_cast<UResource_దత్తాంశం_నమోదు*>(p2.సూచిక);
    UHashTok name1, name2, path1, path2;
    name1.సూచిక = b1->fName;
    name2.సూచిక = b2->fName;
    path1.సూచిక = b1->f_మార్గం;
    path2.సూచిక = b2->f_మార్గం;
    తిరిగి uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
స్థిర UBool chopLocale(అక్షరం *name) {
    అక్షరం *i = uprv_strrchr(name, '_');

    ఒకవేళ(i != శూన్యసూచిక) {
        *i = '\0';
        తిరిగి నిజం;
    }

    తిరిగి అబద్ధం;
}

స్థిర UBool hasVariant(స్థిరరాశి అక్షరం* localeID) {
    UError_సంకేతం err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(శూన్యసూచిక, 0);
    ulocimp_getSubtags(
            localeID,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            &sink,
            శూన్యసూచిక,
            err);
    తిరిగి sink.సంఖ్య_యొక్క_బైట్లు_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#నిర్వచించు INCLUDED_FROM_URESBUND_CPP
#అన్తర్భావ "localefallback_data.h"

స్థిర స్థిరరాశి అక్షరం* performFallbackLookup(స్థిరరాశి అక్షరం* కీలకం,
                                         స్థిరరాశి అక్షరం* కీలకం_Strs,
                                         స్థిరరాశి అక్షరం* విలువ_Strs U_LIFETIME_BOUND,
                                         స్థిరరాశి int32_t* lookup_పట్టిక,
                                         int32_t lookup_పట్టిక_నిడివి) {
    స్థిరరాశి int32_t* bottom = lookup_పట్టిక;
    స్థిరరాశి int32_t* శిఖరం = lookup_పట్టిక + lookup_పట్టిక_నిడివి;

    అయితే (bottom < శిఖరం) {
        // Effectively, divide by 2 and round down to an even index
        స్థిరరాశి int32_t* మధ్య = bottom + (((శిఖరం - bottom) / 4) * 2);
        స్థిరరాశి అక్షరం* నమోదు_కీలకం = &(కీలకం_Strs[*మధ్య]);
        int32_t strcmp_ఫలితం = uprv_strcmp(కీలకం, నమోదు_కీలకం);
        ఒకవేళ (strcmp_ఫలితం == 0) {
            తిరిగి &(విలువ_Strs[మధ్య[1]]);
        } లేకపోతే ఒకవేళ (strcmp_ఫలితం < 0) {
            శిఖరం = మధ్య;
        } లేకపోతే {
            bottom = మధ్య + 2;
        }
    }
    తిరిగి శూన్యసూచిక;
}

స్థిర CharString getDefaultScript(స్థిరరాశి CharString& language, స్థిరరాశి CharString& region) {
    స్థిరరాశి అక్షరం* defaultScript = శూన్యసూచిక;
    UError_సంకేతం err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString ఫలితం("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    ఒకవేళ (!region.is_ఖాళీలు()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        ఒకవేళ (U_FAILURE(err)) {
            తిరిగి ఫలితం;
        }
        defaultScript = performFallbackLookup(localeID.దత్తాంశం(), dsLocaleIDChars, script_సంకేతం_Chars, default_Script_పట్టిక, UPRV_LENGTHOF(default_Script_పట్టిక));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    ఒకవేళ (defaultScript == శూన్యసూచిక) {
        defaultScript = performFallbackLookup(language.దత్తాంశం(), dsLocaleIDChars, script_సంకేతం_Chars, default_Script_పట్టిక, UPRV_LENGTHOF(default_Script_పట్టిక));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    ఒకవేళ (defaultScript != శూన్యసూచిక) {
        ఫలితం.clear();
        ఫలితం.append(defaultScript, err);
    }
    తిరిగి ఫలితం;
}

గణన UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_మూలం,
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
    URES_OPEN_LOCALE_మూలం,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_ప్రత్యక్ష
};
రకనిర్వచనం గణన UResOpenType UResOpenType;

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
స్థిర తార్కిక get_మాతృ_Locale_ID(అక్షరం *name, స్థిరరాశి అక్షరం *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    ఒకవేళ (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        తిరిగి chopLocale(name);
    }
    
    UError_సంకేతం err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, శూన్యసూచిక, శూన్యసూచిక, err);

    ఒకవేళ (U_FAILURE(err)) {
        // hopefully this never happens...
        తిరిగి chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    ఒకవేళ (openType == URES_OPEN_LOCALE_DEFAULT_మూలం) {
        స్థిరరాశి అక్షరం* మాతృ_ID = performFallbackLookup(name, మాతృ_Locale_Chars, మాతృ_Locale_Chars, మాతృ_Locale_పట్టిక, UPRV_LENGTHOF(మాతృ_Locale_పట్టిక));
        ఒకవేళ (మాతృ_ID != శూన్యసూచిక) {
            uprv_strcpy(name, మాతృ_ID);
            తిరిగి నిజం;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    ఒకవేళ (!script.is_ఖాళీలు() && !region.is_ఖాళీలు()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        ఒకవేళ (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } లేకపోతే {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } లేకపోతే ఒకవేళ (!region.is_ఖాళీలు()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_సంకేతం err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, err);
        ఒకవేళ (!origNameScript.is_ఖాళీలు()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } లేకపోతే {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } లేకపోతే ఒకవేళ (!script.is_ఖాళీలు()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        ఒకవేళ (openType != URES_OPEN_LOCALE_DEFAULT_మూలం || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } లేకపోతే {
            తిరిగి అబద్ధం;
        }
    } లేకపోతే {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (U_SUCCESS(err) && !workingLocale.is_ఖాళీలు()) {
        uprv_strcpy(name, workingLocale.దత్తాంశం());
        తిరిగి నిజం;
    } లేకపోతే {
        తిరిగి అబద్ధం;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
స్థిర UBool may_Have_మాతృ(అక్షరం *name) {
    తిరిగి (name[0] != 0 && uprv_strstr("nb nn",name) != శూన్యసూచిక);
}

/**
 *  Internal function
 */
స్థిర శూన్య నమోదు_Increase(UResource_దత్తాంశం_నమోదు *నమోదు_2) {
    Mutex తాళం(&resbMutex);
    నమోదు_2->f_లెక్క_Existing++;
    అయితే(నమోదు_2->f_మాతృ != శూన్యసూచిక) {
      నమోదు_2 = నమోదు_2->f_మాతృ;
      నమోదు_2->f_లెక్క_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
స్థిర UResource_దత్తాంశం_నమోదు *get_Fallback_దత్తాంశం(
        స్థిరరాశి UResourceBundle *resBundle,
        స్థిరరాశి అక్షరం **res_ట్యాగ్, Resource *res, UError_సంకేతం *స్థితి_2) {
    UResource_దత్తాంశం_నమోదు *దత్తాంశం_నమోదు = resBundle->f_దత్తాంశం;
    int32_t సూచీ_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    ఒకవేళ(దత్తాంశం_నమోదు == శూన్యసూచిక) {
        *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(దత్తాంశం_నమోదు->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_పట్టిక_అంశం_By_కీలకం(&(దత్తాంశం_నమోదు->f_దత్తాంశం), దత్తాంశం_నమోదు->f_దత్తాంశం.మూలం_Res, &సూచీ_R, res_ట్యాగ్); /* try to get data from there */
        i++;
    }
    ఒకవేళ(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        అయితే(*res == RES_BOGUS && దత్తాంశం_నమోదు->f_మాతృ != శూన్యసూచిక) {
            దత్తాంశం_నమోదు = దత్తాంశం_నమోదు->f_మాతృ;
            ఒకవేళ(దత్తాంశం_నమోదు->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_పట్టిక_అంశం_By_కీలకం(&(దత్తాంశం_నమోదు->f_దత్తాంశం), దత్తాంశం_నమోదు->f_దత్తాంశం.మూలం_Res, &సూచీ_R, res_ట్యాగ్);
            }
        }
    }

    ఒకవేళ(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    // If the resource is found in parents, we need to adjust the error.
    ఒకవేళ(i>1) {
        ఒకవేళ(uprv_strcmp(దత్తాంశం_నమోదు->fName, uloc_getDefault())==0 || uprv_strcmp(దత్తాంశం_నమోదు->fName, k_మూలం_Locale_Name)==0) {
            *స్థితి_2 = U_USING_DEFAULT_WARNING;
        } లేకపోతే {
            *స్థితి_2 = U_USING_FALLBACK_WARNING;
        }
    }
    తిరిగి దత్తాంశం_నమోదు;
}

స్థిర శూన్య
free_నమోదు(UResource_దత్తాంశం_నమోదు *నమోదు_2) {
    UResource_దత్తాంశం_నమోదు *alias;
    res_unload(&(నమోదు_2->f_దత్తాంశం));
    ఒకవేళ(నమోదు_2->fName != శూన్యసూచిక && నమోదు_2->fName != నమోదు_2->f_Name_బఫరు) {
        uprv_free(నమోదు_2->fName);
    }
    ఒకవేళ(నమోదు_2->f_మార్గం != శూన్యసూచిక) {
        uprv_free(నమోదు_2->f_మార్గం);
    }
    ఒకవేళ(నమోదు_2->fPool != శూన్యసూచిక) {
        --నమోదు_2->fPool->f_లెక్క_Existing;
    }
    alias = నమోదు_2->fAlias;
    ఒకవేళ(alias != శూన్యసూచిక) {
        అయితే(alias->fAlias != శూన్యసూచిక) {
            alias = alias->fAlias;
        }
        --alias->f_లెక్క_Existing;
    }
    uprv_free(నమోదు_2);
}

/* Works just like ucnv_flushCache() */
స్థిర int32_t ures_flushCache()
{
    UResource_దత్తాంశం_నమోదు *resB;
    int32_t pos;
    int32_t rb_Deleted_అంకె = 0;
    స్థిరరాశి UHashElement *e;
    UBool deleted_మరిన్ని;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex తాళం(&resbMutex);
    ఒకవేళ (cache == శూన్యసూచిక) {
        తిరిగి 0;
    }

    చేయి {
        deleted_మరిన్ని = అబద్ధం;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_మొదటి;
        అయితే ((e = uhash_తదుపరి_Element(cache, &pos)) != శూన్యసూచిక)
        {
            resB = static_cast<UResource_దత్తాంశం_నమోదు*>(e->value.సూచిక);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            ఒకవేళ (resB->f_లెక్క_Existing == 0) {
                rb_Deleted_అంకె++;
                deleted_మరిన్ని = నిజం;
                uhash_removeElement(cache, e);
                free_నమోదు(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } అయితే(deleted_మరిన్ని);

    తిరిగి rb_Deleted_అంకె;
}

#నిర్వచితమైతే URES_DEBUG
#అన్తర్భావ <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_ఖాళీలు = అబద్ధం;
  int32_t pos = UHASH_మొదటి;
  స్థిరరాశి UHashElement *e;
  UResource_దత్తాంశం_నమోదు *resB;
  
    Mutex తాళం(&resbMutex);
    ఒకవేళ (cache == శూన్యసూచిక) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      తిరిగి అబద్ధం;
    }

    అయితే ((e = uhash_తదుపరి_Element(cache, &pos)) != శూన్యసూచిక) {
      cache_Not_ఖాళీలు=నిజం;
      resB = (UResource_దత్తాంశం_నమోదు *) e->value.సూచిక;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (శూన్య*)resB, resB->f_లెక్క_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_మార్గం?resB->f_మార్గం:"nullptr",
              (శూన్య*)resB->fPool,
              (శూన్య*)resB->fAlias,
              (శూన్య*)resB->f_మాతృ);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_లెక్క(cache));
    తిరిగి cache_Not_ఖాళీలు;
}

#అయితే_ముగింపు

స్థిర UBool U_CALLCONV ures_cleanup()
{
    ఒకవేళ (cache != శూన్యసూచిక) {
        ures_flushCache();
        uhash_close(cache);
        cache = శూన్యసూచిక;
    }
    gCacheInitOnce.reset();
    తిరిగి నిజం;
}

/** INTERNAL: Initializes the cache for resources */
స్థిర శూన్య U_CALLCONV createCache(UError_సంకేతం &స్థితి_2) {
    U_ASSERT(cache == శూన్యసూచిక);
    cache = uhash_open(hash_నమోదు, compareEntries, శూన్యసూచిక, &స్థితి_2);
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
స్థిర శూన్య initCache(UError_సంకేతం *స్థితి_2) {
    umtx_initOnce(gCacheInitOnce, &createCache, *స్థితి_2);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

స్థిర శూన్య సమితి_నమోదు_Name(UResource_దత్తాంశం_నమోదు *res, స్థిరరాశి అక్షరం *name, UError_సంకేతం *స్థితి_2) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    ఒకవేళ(res->fName != శూన్యసూచిక && res->fName != res->f_Name_బఫరు) {
        uprv_free(res->fName);
    }
    ఒకవేళ (len < static_cast<int32_t>(పరిమాణం(res->f_Name_బఫరు))) {
        res->fName = res->f_Name_బఫరు;
    }
    లేకపోతే {
        res->fName = static_cast<అక్షరం*>(uprv_malloc(len + 1));
    }
    ఒకవేళ(res->fName == శూన్యసూచిక) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే {
        uprv_strcpy(res->fName, name);
    }
}

స్థిర UResource_దత్తాంశం_నమోదు *
get_Pool_నమోదు(స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
స్థిర UResource_దత్తాంశం_నమోదు *init_నమోదు(స్థిరరాశి అక్షరం *localeID, స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2) {
    UResource_దత్తాంశం_నమోదు *r = శూన్యసూచిక;
    UResource_దత్తాంశం_నమోదు find;
    /*int32_t hashValue;*/
    స్థిరరాశి అక్షరం *name;
    అక్షరం aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    /* here we try to deduce the right locale name */
    ఒకవేళ(localeID == శూన్యసూచిక) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } లేకపోతే ఒకవేళ(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_మూలం_Locale_Name;
    } లేకపోతే { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<అక్షరం*>(name);
    find.f_మార్గం = const_cast<అక్షరం*>(మార్గం);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_దత్తాంశం_నమోదు*>(uhash_get(cache, &find));
    ఒకవేళ(r == శూన్యసూచిక) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_దత్తాంశం_నమోదు*>(uprv_malloc(పరిమాణం(UResource_దత్తాంశం_నమోదు)));
        ఒకవేళ(r == శూన్యసూచిక) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }

        uprv_memset(r, 0, పరిమాణం(UResource_దత్తాంశం_నమోదు));
        /*r->fHashKey = hashValue;*/

        సమితి_నమోదు_Name(r, name, స్థితి_2);
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            uprv_free(r);
            తిరిగి శూన్యసూచిక;
        }

        ఒకవేళ(మార్గం != శూన్యసూచిక) {
            r->f_మార్గం = uprv_strdup(మార్గం);
            ఒకవేళ(r->f_మార్గం == శూన్యసూచిక) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                uprv_free(r);
                తిరిగి శూన్యసూచిక;
            }
        }

        /* this is the actual loading */
        res_ఎక్కించు(&(r->f_దత్తాంశం), r->f_మార్గం, r->fName, స్థితి_2);

        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            ఒకవేళ (*స్థితి_2 == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
                uprv_free(r);
                తిరిగి శూన్యసూచిక;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *స్థితి_2 = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } లేకపోతే { /* if we have a regular entry */
            Resource aliasres;
            ఒకవేళ (r->f_దత్తాంశం.ఉపయోగిస్తుంది_Pool_Bundle) {
                r->fPool = get_Pool_నమోదు(r->f_మార్గం, స్థితి_2);
                ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
                    స్థిరరాశి int32_t *poolIndexes = r->fPool->f_దత్తాంశం.p_మూలం + 1;
                    ఒకవేళ(r->f_దత్తాంశం.p_మూలం[1 + URES_సూచీ_POOL_CHECKSUM] == poolIndexes[URES_సూచీ_POOL_CHECKSUM]) {
                        r->f_దత్తాంశం.pool_Bundle_కీలకాలు = reinterpret_cast<స్థిరరాశి అక్షరం*>(poolIndexes + (poolIndexes[URES_సూచీ_నిడివి] & 0xff));
                        r->f_దత్తాంశం.pool_Bundle_అక్షరశ్రేణులు = r->fPool->f_దత్తాంశం.p16_బిట్టు_Units;
                    } లేకపోతే {
                        r->fBogus = *స్థితి_2 = U_INVALID_FORMAT_ERROR;
                    }
                } లేకపోతే {
                    r->fBogus = *స్థితి_2;
                }
            }
            ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_దత్తాంశం), "%%ALIAS");
                ఒకవేళ (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    స్థిరరాశి char16_t *alias = res_getStringNoTrace(&(r->f_దత్తాంశం), aliasres, &aliasLen);
                    ఒకవేళ(alias != శూన్యసూచిక && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_గమ్యం_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_నమోదు(aliasName, మార్గం, స్థితి_2);
                    }
                }
            }
        }

        {
            UResource_దత్తాంశం_నమోదు *oldR = శూన్యసూచిక;
            ఒకవేళ ((oldR = static_cast<UResource_దత్తాంశం_నమోదు*>(uhash_get(cache, r))) == శూన్యసూచిక) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_సంకేతం cache_స్థితి = U_ZERO_ERROR;
                uhash_put(cache, (శూన్య *)r, r, &cache_స్థితి);
                ఒకవేళ (U_FAILURE(cache_స్థితి)) {
                    *స్థితి_2 = cache_స్థితి;
                    free_నమోదు(r);
                    r = శూన్యసూచిక;
                }
            } లేకపోతే {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_నమోదు(r);
                r = oldR;
            }
        }

    }
    ఒకవేళ(r != శూన్యసూచిక) {
        /* return the real bundle */
        అయితే(r->fAlias != శూన్యసూచిక) {
            r = r->fAlias;
        }
        r->f_లెక్క_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        ఒకవేళ(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*స్థితి_2)) {
             *స్థితి_2 = r->fBogus; /* set the returning status */
        }
    }
    తిరిగి r;
}

స్థిర UResource_దత్తాంశం_నమోదు *
get_Pool_నమోదు(స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2) {
    UResource_దత్తాంశం_నమోదు *poolBundle = init_నమోదు(kPoolBundleName, మార్గం, స్థితి_2);
    ఒకవేళ( U_SUCCESS(*స్థితి_2) &&
        (poolBundle == శూన్యసూచిక || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_దత్తాంశం.isPoolBundle)
    ) {
        *స్థితి_2 = U_INVALID_FORMAT_ERROR;
    }
    తిరిగి poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
స్థిర UResource_దత్తాంశం_నమోదు *
కనుగొను_మొదటి_Existing(స్థిరరాశి అక్షరం* మార్గం, అక్షరం* name, స్థిరరాశి అక్షరం* defaultLocale, UResOpenType openType,
                  UBool *is_మూలం, UBool *దొరికింది_మాతృ, UBool *isDefault, UError_సంకేతం* స్థితి_2) {
    UResource_దత్తాంశం_నమోదు *r = శూన్యసూచిక;
    UBool has_Real_దత్తాంశం = అబద్ధం;
    *దొరికింది_మాతృ = నిజం; /* we're starting with a fresh name */
    అక్షరం origName[ULOC_FULLNAME_సామర్థ్యం];

    uprv_strcpy(origName, name);
    అయితే(*దొరికింది_మాతృ && !has_Real_దత్తాంశం) {
        r = init_నమోదు(name, మార్గం, స్థితి_2);
        /* Null pointer test */
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి శూన్యసూచిక;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_దత్తాంశం = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        ఒకవేళ(!has_Real_దత్తాంశం) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_లెక్క_Existing--;
            /*entryCloseInt(r);*/
            r = శూన్యసూచిక;
            *స్థితి_2 = U_USING_FALLBACK_WARNING;
        } లేకపోతే {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_మూలం = static_cast<UBool>(uprv_strcmp(name, k_మూలం_Locale_Name) == 0);

        /*Fallback data stuff*/
        ఒకవేళ (!has_Real_దత్తాంశం) {
            *దొరికింది_మాతృ = get_మాతృ_Locale_ID(name, origName, openType);
        } లేకపోతే {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *దొరికింది_మాతృ = chopLocale(name);
        }
        ఒకవేళ (*దొరికింది_మాతృ && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    తిరిగి r;
}

స్థిర శూన్య ures_సమితి_Is_దొంతర_Object( UResourceBundle* resB, UBool state) {
    ఒకవేళ(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } లేకపోతే {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

స్థిర UBool ures_is_దొంతర_Object(స్థిరరాశి UResourceBundle* resB) {
  తిరిగి((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?అబద్ధం:నిజం);
}


U_CFUNC శూన్య ures_init_దొంతర_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, పరిమాణం(UResourceBundle));
  ures_సమితి_Is_దొంతర_Object(resB, నిజం);
}

U_NAMESPACE_BEGIN

దొంతర_UResource_Bundle::దొంతర_UResource_Bundle() {
    ures_init_దొంతర_Object(&bundle);
}

దొంతర_UResource_Bundle::~దొంతర_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_చివర

స్థిర UBool  // returns U_SUCCESS(*status)
ఎక్కించు_Parents_Except_మూలం(UResource_దత్తాంశం_నమోదు *&t1,
                      అక్షరం name[], int32_t name_సామర్థ్యం,
                      UBool usingUSRData, అక్షరం usr_దత్తాంశం_మార్గం[], UError_సంకేతం *స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) { తిరిగి అబద్ధం; }
    UBool check_మాతృ = నిజం;
    అయితే (check_మాతృ && t1->f_మాతృ == శూన్యసూచిక && !t1->f_దత్తాంశం.noFallback &&
            res_getResource(&t1->f_దత్తాంశం,"%%ParentIsRoot") == RES_BOGUS) {
        Resource మాతృ_Res = res_getResource(&t1->f_దత్తాంశం, "%%Parent");
        ఒకవేళ (మాతృ_Res != RES_BOGUS) {  // An explicit parent was found.
            int32_t మాతృ_Locale_Len = 0;
            // No tracing: called during initial data loading
            స్థిరరాశి char16_t *మాతృ_Locale_Name = res_getStringNoTrace(&(t1->f_దత్తాంశం), మాతృ_Res, &మాతృ_Locale_Len);
            ఒకవేళ(మాతృ_Locale_Name != శూన్యసూచిక && 0 < మాతృ_Locale_Len && మాతృ_Locale_Len < name_సామర్థ్యం) {
                u_UChars_గమ్యం_Chars(మాతృ_Locale_Name, name, మాతృ_Locale_Len + 1);
                ఒకవేళ (uprv_strcmp(name, k_మూలం_Locale_Name) == 0) {
                    తిరిగి నిజం;
                }
            }
        }
        // Insert regular parents.
        UError_సంకేతం మాతృ_స్థితి = U_ZERO_ERROR;
        UResource_దత్తాంశం_నమోదు *t2 = init_నమోదు(name, t1->f_మార్గం, &మాతృ_స్థితి);
        ఒకవేళ (U_FAILURE(మాతృ_స్థితి)) {
            *స్థితి_2 = మాతృ_స్థితి;
            తిరిగి అబద్ధం;
        }
        UResource_దత్తాంశం_నమోదు *u2 = శూన్యసూచిక;
        UError_సంకేతం usr_స్థితి = U_ZERO_ERROR;
        ఒకవేళ (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_నమోదు(name, usr_దత్తాంశం_మార్గం, &usr_స్థితి);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            ఒకవేళ (usr_స్థితి == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
                *స్థితి_2 = usr_స్థితి;
                తిరిగి అబద్ధం;
            }
        }

        ఒకవేళ (usingUSRData && U_SUCCESS(usr_స్థితి) && u2->fBogus == U_ZERO_ERROR) {
            t1->f_మాతృ = u2;
            u2->f_మాతృ = t2;
        } లేకపోతే {
            t1->f_మాతృ = t2;
            ఒకవేళ (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_లెక్క_Existing = 0;
            }
        }
        t1 = t2;
        check_మాతృ = chopLocale(name) || may_Have_మాతృ(name);
    }
    తిరిగి నిజం;
}

స్థిర UBool  // returns U_SUCCESS(*status)
చొప్పించు_మూలం_Bundle(UResource_దత్తాంశం_నమోదు *&t1, UError_సంకేతం *స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) { తిరిగి అబద్ధం; }
    UError_సంకేతం మాతృ_స్థితి = U_ZERO_ERROR;
    UResource_దత్తాంశం_నమోదు *t2 = init_నమోదు(k_మూలం_Locale_Name, t1->f_మార్గం, &మాతృ_స్థితి);
    ఒకవేళ (U_FAILURE(మాతృ_స్థితి)) {
        *స్థితి_2 = మాతృ_స్థితి;
        తిరిగి అబద్ధం;
    }
    t1->f_మాతృ = t2;
    t1 = t2;
    తిరిగి నిజం;
}

స్థిర UResource_దత్తాంశం_నమోదు *నమోదు_Open(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* localeID,
                                     UResOpenType openType, UError_సంకేతం* స్థితి_2) {
    U_ASSERT(openType != URES_OPEN_ప్రత్యక్ష);
    UError_సంకేతం int_స్థితి = U_ZERO_ERROR;
    UResource_దత్తాంశం_నమోదు *r = శూన్యసూచిక;
    UResource_దత్తాంశం_నమోదు *t1 = శూన్యసూచిక;
    UBool isDefault = అబద్ధం;
    UBool is_మూలం = అబద్ధం;
    UBool has_Real_దత్తాంశం = అబద్ధం;
    UBool hasChopped = నిజం;
    UBool usingUSRData = U_USE_USRDATA && ( మార్గం == శూన్యసూచిక || uprv_strncmp(మార్గం,U_ICUDATA_NAME,8) == 0);

    అక్షరం name[ULOC_FULLNAME_సామర్థ్యం];
    అక్షరం usr_దత్తాంశం_మార్గం[96];

    initCache(స్థితి_2);

    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    uprv_strncpy(name, localeID, పరిమాణం(name) - 1);
    name[పరిమాణం(name) - 1] = 0;

    ఒకవేళ ( usingUSRData ) {
        ఒకవేళ ( మార్గం == శూన్యసూచిక ) {
            uprv_strcpy(usr_దత్తాంశం_మార్గం, U_USRDATA_NAME);
        } లేకపోతే {
            uprv_strncpy(usr_దత్తాంశం_మార్గం, మార్గం, పరిమాణం(usr_దత్తాంశం_మార్గం) - 1);
            usr_దత్తాంశం_మార్గం[0] = 'u';
            usr_దత్తాంశం_మార్గం[1] = 's';
            usr_దత్తాంశం_మార్గం[2] = 'r';
            usr_దత్తాంశం_మార్గం[పరిమాణం(usr_దత్తాంశం_మార్గం) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    స్థిరరాశి అక్షరం *defaultLocale = uloc_getDefault();

    Mutex తాళం(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = కనుగొను_మొదటి_Existing(మార్గం, name, defaultLocale, openType, &is_మూలం, &hasChopped, &isDefault, &int_స్థితి);

    // If we failed due to out-of-memory, report the failure and exit early.
    ఒకవేళ (int_స్థితి == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
        *స్థితి_2 = int_స్థితి;
        వెళ్ళు ముగింపు;
    }

    ఒకవేళ(r != శూన్యసూచిక) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_దత్తాంశం = నిజం;
        ఒకవేళ ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_సంకేతం usr_స్థితి = U_ZERO_ERROR;
            UResource_దత్తాంశం_నమోదు *u1 = init_నమోదు(t1->fName, usr_దత్తాంశం_మార్గం, &usr_స్థితి);
            // If we failed due to out-of-memory, report the failure and exit early.
            ఒకవేళ (int_స్థితి == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
                *స్థితి_2 = int_స్థితి;
                వెళ్ళు ముగింపు;
            }
            ఒకవేళ ( u1 != శూన్యసూచిక ) {
                ఒకవేళ(u1->fBogus == U_ZERO_ERROR) {
                    u1->f_మాతృ = t1;
                    r = u1;
                } లేకపోతే {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_లెక్క_Existing = 0;
                }
            }
        }
        ఒకవేళ ((hasChopped || may_Have_మాతృ(name)) && !is_మూలం) {
            ఒకవేళ (!ఎక్కించు_Parents_Except_మూలం(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_దత్తాంశం_మార్గం, స్థితి_2)) {
                వెళ్ళు ముగింపు;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    ఒకవేళ(r==శూన్యసూచిక && openType == URES_OPEN_LOCALE_DEFAULT_మూలం && !isDefault && !is_మూలం) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = కనుగొను_మొదటి_Existing(మార్గం, name, defaultLocale, openType, &is_మూలం, &hasChopped, &isDefault, &int_స్థితి);
        // If we failed due to out-of-memory, report the failure and exit early.
        ఒకవేళ (int_స్థితి == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
            *స్థితి_2 = int_స్థితి;
            వెళ్ళు ముగింపు;
        }
        int_స్థితి = U_USING_DEFAULT_WARNING;
        ఒకవేళ(r != శూన్యసూచిక) { /* the default locale exists */
            t1 = r;
            has_Real_దత్తాంశం = నిజం;
            isDefault = నిజం;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            ఒకవేళ ((hasChopped || may_Have_మాతృ(name)) && !is_మూలం) {
                ఒకవేళ (!ఎక్కించు_Parents_Except_మూలం(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_దత్తాంశం_మార్గం, స్థితి_2)) {
                    వెళ్ళు ముగింపు;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    ఒకవేళ(r == శూన్యసూచిక) {
        uprv_strcpy(name, k_మూలం_Locale_Name);
        r = కనుగొను_మొదటి_Existing(మార్గం, name, defaultLocale, openType, &is_మూలం, &hasChopped, &isDefault, &int_స్థితి);
        // If we failed due to out-of-memory, report the failure and exit early.
        ఒకవేళ (int_స్థితి == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
            *స్థితి_2 = int_స్థితి;
            వెళ్ళు ముగింపు;
        }
        ఒకవేళ(r != శూన్యసూచిక) {
            t1 = r;
            int_స్థితి = U_USING_DEFAULT_WARNING;
            has_Real_దత్తాంశం = నిజం;
        } లేకపోతే { /* we don't even have the root locale */
            *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
            వెళ్ళు ముగింపు;
        }
    } లేకపోతే ఒకవేళ(!is_మూలం && uprv_strcmp(t1->fName, k_మూలం_Locale_Name) != 0 &&
            t1->f_మాతృ == శూన్యసూచిక && !r->f_దత్తాంశం.noFallback) {
        ఒకవేళ (!చొప్పించు_మూలం_Bundle(t1, స్థితి_2)) {
            వెళ్ళు ముగింపు;
        }
        ఒకవేళ(!has_Real_దత్తాంశం) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    అయితే(r != శూన్యసూచిక && !is_మూలం && t1->f_మాతృ != శూన్యసూచిక) {
        t1->f_మాతృ->f_లెక్క_Existing++;
        t1 = t1->f_మాతృ;
    }

ముగింపు:
    ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
        ఒకవేళ(int_స్థితి != U_ZERO_ERROR) {
            *స్థితి_2 = int_స్థితి;  
        }
        తిరిగి r;
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
స్థిర UResource_దత్తాంశం_నమోదు *
నమోదు_Open_ప్రత్యక్ష(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* localeID, UError_సంకేతం* స్థితి_2) {
    initCache(స్థితి_2);
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    } లేకపోతే ఒకవేళ (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_మూలం_Locale_Name;
    }

    Mutex తాళం(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_దత్తాంశం_నమోదు *r = init_నమోదు(localeID, మార్గం, స్థితి_2);
    ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
        ఒకవేళ(r->fBogus != U_ZERO_ERROR) {
            r->f_లెక్క_Existing--;
            r = శూన్యసూచిక;
        }
    } లేకపోతే {
        r = శూన్యసూచిక;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_దత్తాంశం_నమోదు *t1 = r;
    ఒకవేళ(r != శూన్యసూచిక && uprv_strcmp(localeID, k_మూలం_Locale_Name) != 0 &&  // not root
            r->f_మాతృ == శూన్యసూచిక && !r->f_దత్తాంశం.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_సామర్థ్యం) {
        అక్షరం name[ULOC_FULLNAME_సామర్థ్యం];
        uprv_strcpy(name, localeID);
        ఒకవేళ(!chopLocale(name) || uprv_strcmp(name, k_మూలం_Locale_Name) == 0 ||
                ఎక్కించు_Parents_Except_మూలం(t1, name, UPRV_LENGTHOF(name), అబద్ధం, శూన్యసూచిక, స్థితి_2)) {
            ఒకవేళ(uprv_strcmp(t1->fName, k_మూలం_Locale_Name) != 0 && t1->f_మాతృ == శూన్యసూచిక) {
                చొప్పించు_మూలం_Bundle(t1, స్థితి_2);
            }
        }
        ఒకవేళ(U_FAILURE(*స్థితి_2)) {
            r = శూన్యసూచిక;
        }
    }

    ఒకవేళ(r != శూన్యసూచిక) {
        // TODO: Does this ever loop?
        అయితే(t1->f_మాతృ != శూన్యసూచిక) {
            t1->f_మాతృ->f_లెక్క_Existing++;
            t1 = t1->f_మాతృ;
        }
    }
    తిరిగి r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
స్థిర శూన్య నమోదు_Close_Int(UResource_దత్తాంశం_నమోదు *resB) {
    UResource_దత్తాంశం_నమోదు *p = resB;

    అయితే(resB != శూన్యసూచిక) {
        p = resB->f_మాతృ;
        resB->f_లెక్క_Existing--;

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

స్థిర శూన్య నమోదు_Close(UResource_దత్తాంశం_నమోదు *resB) {
  Mutex తాళం(&resbMutex);
  నమోదు_Close_Int(resB);
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
స్థిర శూన్య ures_append_Res_మార్గం(UResourceBundle *resB, స్థిరరాశి అక్షరం* గమ్యం_కూడు, int32_t len_గమ్యం_కూడు, UError_సంకేతం *స్థితి_2) {
    int32_t res_మార్గం_Len_Orig = resB->f_Res_మార్గం_Len;
    ఒకవేళ(resB->f_Res_మార్గం == శూన్యసూచిక) {
        resB->f_Res_మార్గం = resB->fResBuf;
        *(resB->f_Res_మార్గం) = 0;
        resB->f_Res_మార్గం_Len = 0;
    } 
    resB->f_Res_మార్గం_Len += len_గమ్యం_కూడు;
    ఒకవేళ(RES_BUFSIZE <= resB->f_Res_మార్గం_Len+1) {
        ఒకవేళ(resB->f_Res_మార్గం == resB->fResBuf) {
            resB->f_Res_మార్గం = static_cast<అక్షరం*>(uprv_malloc((resB->f_Res_మార్గం_Len + 1) * పరిమాణం(అక్షరం)));
            /* Check that memory was allocated correctly. */
            ఒకవేళ (resB->f_Res_మార్గం == శూన్యసూచిక) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            uprv_strcpy(resB->f_Res_మార్గం, resB->fResBuf);
        } లేకపోతే {
            అక్షరం* తాత్కాలిక = static_cast<అక్షరం*>(uprv_realloc(resB->f_Res_మార్గం, (resB->f_Res_మార్గం_Len + 1) * పరిమాణం(అక్షరం)));
            /* Check that memory was reallocated correctly. */
            ఒకవేళ (తాత్కాలిక == శూన్యసూచిక) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            resB->f_Res_మార్గం = తాత్కాలిక;
        }
    }
    uprv_strcpy(resB->f_Res_మార్గం + res_మార్గం_Len_Orig, గమ్యం_కూడు);
}

స్థిర శూన్య ures_free_Res_మార్గం(UResourceBundle *resB) {
    ఒకవేళ (resB->f_Res_మార్గం && resB->f_Res_మార్గం != resB->fResBuf) {
        uprv_free(resB->f_Res_మార్గం);
    }
    resB->f_Res_మార్గం = శూన్యసూచిక;
    resB->f_Res_మార్గం_Len = 0;
}

స్థిర శూన్య
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    ఒకవేళ(resB != శూన్యసూచిక) {
        ఒకవేళ(resB->f_దత్తాంశం != శూన్యసూచిక) {
            నమోదు_Close(resB->f_దత్తాంశం);
        }
        ఒకవేళ(resB->f_సంస్కరణ != శూన్యసూచిక) {
            uprv_free(resB->f_సంస్కరణ);
        }
        ures_free_Res_మార్గం(resB);

        ఒకవేళ(ures_is_దొంతర_Object(resB) == అబద్ధం && freeBundleObj) {
            uprv_free(resB);
        }
#ఒకవేళ 0 /*U_DEBUG*/
        లేకపోతే {
            /* poison the data */
            uprv_memset(resB, -1, పరిమాణం(UResourceBundle));
        }
#అయితే_ముగింపు
    }
}

U_CAPI శూన్య  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, నిజం);
}

నామాకాశ {

UResourceBundle* init_resb_ఫలితం(UResource_దత్తాంశం_నమోదు* దత్తాంశం_నమోదు,
                                  Resource r,
                                  స్థిరరాశి అక్షరం* కీలకం,
                                  int32_t idx,
                                  UResource_దత్తాంశం_నమోదు* valid_Locale_దత్తాంశం_నమోదు,
                                  స్థిరరాశి అక్షరం* container_Res_మార్గం,
                                  int32_t recursion_లోతు,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_సంకేతం* స్థితి_2);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
get_Alias_లక్ష్యం_As_Resource_Bundle(స్థిరరాశి Resource_దత్తాంశం& res_దత్తాంశం,
                               Resource r,
                               స్థిరరాశి అక్షరం* కీలకం,
                               int32_t idx,
                               UResource_దత్తాంశం_నమోదు* valid_Locale_దత్తాంశం_నమోదు,
                               స్థిరరాశి అక్షరం* container_Res_మార్గం,
                               int32_t recursion_లోతు,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_సంకేతం* స్థితి_2) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ఒకవేళ (U_FAILURE(*స్థితి_2)) { తిరిగి resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    స్థిరరాశి char16_t *alias = res_getAlias(&res_దత్తాంశం, r, &len);
    ఒకవేళ(len <= 0) {
        // bad alias
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి resB;
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
    chAlias.appendInvariantChars(alias, len, *స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    // We have an alias, now let's cut it up.
    స్థిరరాశి అక్షరం *మార్గం = శూన్యసూచిక, *locale = శూన్యసూచిక, *కీలకం_మార్గం = శూన్యసూచిక;
    ఒకవేళ(chAlias[0] == RES_మార్గం_SEPARATOR) {
        // There is a path included.
        అక్షరం *ch_Alias_దత్తాంశం = chAlias.దత్తాంశం();
        అక్షరం *sep = ch_Alias_దత్తాంశం + 1;
        మార్గం = sep;
        sep = uprv_strchr(sep, RES_మార్గం_SEPARATOR);
        ఒకవేళ(sep != శూన్యసూచిక) {
            *sep++ = 0;
        }
        ఒకవేళ(uprv_strcmp(మార్గం, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            కీలకం_మార్గం = sep;
            // Read from the valid locale which we already have.
            మార్గం = locale = శూన్యసూచిక;
        } లేకపోతే {
            ఒకవేళ(uprv_strcmp(మార్గం, "ICUDATA") == 0) { /* want ICU data */
                మార్గం = శూన్యసూచిక;
            }
            ఒకవేళ (sep == శూన్యసూచిక) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } లేకపోతే {
                locale = sep;
                sep = uprv_strchr(sep, RES_మార్గం_SEPARATOR);
                ఒకవేళ(sep != శూన్యసూచిక) {
                    *sep++ = 0;
                }
                కీలకం_మార్గం = sep;
            }
        }
    } లేకపోతే {
        // No path, start with a locale.
        అక్షరం *sep = chAlias.దత్తాంశం();
        locale = sep;
        sep = uprv_strchr(sep, RES_మార్గం_SEPARATOR);
        ఒకవేళ(sep != శూన్యసూచిక) {
            *sep++ = 0;
        }
        కీలకం_మార్గం = sep;
        మార్గం = valid_Locale_దత్తాంశం_నమోదు->f_మార్గం;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    Local_UResource_Bundle_సూచిక mainRes;
    UResource_దత్తాంశం_నమోదు *దత్తాంశం_నమోదు;
    ఒకవేళ (locale == శూన్యసూచిక) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        దత్తాంశం_నమోదు = valid_Locale_దత్తాంశం_నమోదు;
    } లేకపోతే {
        UError_సంకేతం int_స్థితి = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_ప్రత్యక్ష(మార్గం, locale, &int_స్థితి));
        ఒకవేళ(U_FAILURE(int_స్థితి)) {
            // We failed to open the resource bundle we're aliasing to.
            *స్థితి_2 = int_స్థితి;
            తిరిగి resB;
        }
        దత్తాంశం_నమోదు = mainRes->f_దత్తాంశం;
    }

    స్థిరరాశి అక్షరం* తాత్కాలిక = శూన్యసూచిక;
    ఒకవేళ(కీలకం_మార్గం == శూన్యసూచిక) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = దత్తాంశం_నమోదు->f_దత్తాంశం.మూలం_Res;
        ఒకవేళ(container_Res_మార్గం) {
            chAlias.clear().append(container_Res_మార్గం, *స్థితి_2);
            ఒకవేళ (U_FAILURE(*స్థితి_2)) {
                తిరిగి శూన్యసూచిక;
            }
            అక్షరం *a_కీలకం = chAlias.దత్తాంశం();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_కనుగొను_Resource(&దత్తాంశం_నమోదు->f_దత్తాంశం, r, &a_కీలకం, &తాత్కాలిక);
        }
        ఒకవేళ(కీలకం) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(కీలకం, *స్థితి_2);
            ఒకవేళ (U_FAILURE(*స్థితి_2)) {
                తిరిగి శూన్యసూచిక;
            }
            అక్షరం *a_కీలకం = chAlias.దత్తాంశం();
            r = res_కనుగొను_Resource(&దత్తాంశం_నమోదు->f_దత్తాంశం, r, &a_కీలకం, &తాత్కాలిక);
        } లేకపోతే ఒకవేళ(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            ఒకవేళ(URES_IS_పట్టిక(type)) {
                స్థిరరాశి అక్షరం *a_కీలకం;
                r = res_get_పట్టిక_అంశం_By_సూచీ(&దత్తాంశం_నమోదు->f_దత్తాంశం, r, idx, &a_కీలకం);
            } లేకపోతే { /* array */
                r = res_get_Array_అంశం(&దత్తాంశం_నమోదు->f_దత్తాంశం, r, idx);
            }
        }
        ఒకవేళ(r != RES_BOGUS) {
            resB = init_resb_ఫలితం(
                దత్తాంశం_నమోదు, r, తాత్కాలిక, -1, valid_Locale_దత్తాంశం_నమోదు, శూన్యసూచిక, recursion_లోతు+1,
                resB, స్థితి_2);
        } లేకపోతే {
            *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        }
    } లేకపోతే {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString మార్గం_Buf(కీలకం_మార్గం, *స్థితి_2);
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి శూన్యసూచిక;
        }
        అక్షరం *my_మార్గం = మార్గం_Buf.దత్తాంశం();
        container_Res_మార్గం = శూన్యసూచిక;
        // Now we have fallback following here.
        కొరకు(;;) {
            r = దత్తాంశం_నమోదు->f_దత్తాంశం.మూలం_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            అయితే(*my_మార్గం && U_SUCCESS(*స్థితి_2)) {
                r = res_కనుగొను_Resource(&(దత్తాంశం_నమోదు->f_దత్తాంశం), r, &my_మార్గం, &తాత్కాలిక);
                ఒకవేళ(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    ఆపు;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_ఫలితం(
                    దత్తాంశం_నమోదు, r, తాత్కాలిక, -1,
                    valid_Locale_దత్తాంశం_నమోదు, container_Res_మార్గం, recursion_లోతు+1,
                    resB, స్థితి_2);
                ఒకవేళ (U_FAILURE(*స్థితి_2)) {
                    ఆపు;
                }
                ఒకవేళ (తాత్కాలిక == శూన్యసూచిక || uprv_strcmp(కీలకం_మార్గం, తాత్కాలిక) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_మార్గం(resB);
                    ures_append_Res_మార్గం(resB, కీలకం_మార్గం, static_cast<int32_t>(uprv_strlen(కీలకం_మార్గం)), స్థితి_2);
                    ఒకవేళ(resB->f_Res_మార్గం[resB->f_Res_మార్గం_Len-1] != RES_మార్గం_SEPARATOR) {
                        ures_append_Res_మార్గం(resB, RES_మార్గం_SEPARATOR_S, 1, స్థితి_2);
                    }
                    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
                        ఆపు;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                దత్తాంశం_నమోదు = resB->f_దత్తాంశం;
                container_Res_మార్గం = resB->f_Res_మార్గం;
            }
            ఒకవేళ (U_FAILURE(*స్థితి_2) || r != RES_BOGUS) {
                ఆపు;
            }
            // Fall back to the parent bundle, if there is one.
            దత్తాంశం_నమోదు = దత్తాంశం_నమోదు->f_మాతృ;
            ఒకవేళ (దత్తాంశం_నమోదు == శూన్యసూచిక) {
                *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
                ఆపు;
            }
            // Copy the same keyPath again.
            my_మార్గం = మార్గం_Buf.దత్తాంశం();
            uprv_strcpy(my_మార్గం, కీలకం_మార్గం);
        }
    }
    ఒకవేళ(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    తిరిగి resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_ఫలితం(UResource_దత్తాంశం_నమోదు* దత్తాంశం_నమోదు,
                                  Resource r,
                                  స్థిరరాశి అక్షరం* కీలకం,
                                  int32_t idx,
                                  UResource_దత్తాంశం_నమోదు* valid_Locale_దత్తాంశం_నమోదు,
                                  స్థిరరాశి అక్షరం* container_Res_మార్గం,
                                  int32_t recursion_లోతు,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_సంకేతం* స్థితి_2) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి resB;
    }
    ఒకవేళ (valid_Locale_దత్తాంశం_నమోదు == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        ఒకవేళ(recursion_లోతు >= URES_MAX_ALIAS_LEVEL) {
            *స్థితి_2 = U_TOO_MANY_ALIASES_ERROR;
            తిరిగి resB;
        }
        తిరిగి get_Alias_లక్ష్యం_As_Resource_Bundle(
            దత్తాంశం_నమోదు->f_దత్తాంశం, r, కీలకం, idx,
            valid_Locale_దత్తాంశం_నమోదు, container_Res_మార్గం, recursion_లోతు, resB, స్థితి_2);
    }
    ఒకవేళ(resB == శూన్యసూచిక) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(పరిమాణం(UResourceBundle)));
        ఒకవేళ (resB == శూన్యసూచిక) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        ures_సమితి_Is_దొంతర_Object(resB, అబద్ధం);
        resB->f_Res_మార్గం = శూన్యసూచిక;
        resB->f_Res_మార్గం_Len = 0;
    } లేకపోతే {
        ఒకవేళ(resB->f_దత్తాంశం != శూన్యసూచిక) {
            నమోదు_Close(resB->f_దత్తాంశం);
        }
        ఒకవేళ(resB->f_సంస్కరణ != శూన్యసూచిక) {
            uprv_free(resB->f_సంస్కరణ);
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
        ఒకవేళ(container_Res_మార్గం != resB->f_Res_మార్గం) {
            ures_free_Res_మార్గం(resB);
        }
    }
    resB->f_దత్తాంశం = దత్తాంశం_నమోదు;
    నమోదు_Increase(resB->f_దత్తాంశం);
    resB->fHasFallback = అబద్ధం;
    resB->f_Is_శిఖరం_Level = అబద్ధం;
    resB->f_సూచీ = -1;
    resB->f_కీలకం = కీలకం; 
    resB->f_Valid_Locale_దత్తాంశం_నమోదు = valid_Locale_దత్తాంశం_నమోదు;
    ఒకవేళ(container_Res_మార్గం != resB->f_Res_మార్గం) {
        ures_append_Res_మార్గం(
            resB, container_Res_మార్గం, static_cast<int32_t>(uprv_strlen(container_Res_మార్గం)), స్థితి_2);
    }
    ఒకవేళ(కీలకం != శూన్యసూచిక) {
        ures_append_Res_మార్గం(resB, కీలకం, static_cast<int32_t>(uprv_strlen(కీలకం)), స్థితి_2);
        ఒకవేళ(resB->f_Res_మార్గం_Len > 0 && resB->f_Res_మార్గం[resB->f_Res_మార్గం_Len-1] != RES_మార్గం_SEPARATOR) {
            ures_append_Res_మార్గం(resB, RES_మార్గం_SEPARATOR_S, 1, స్థితి_2);
        }
    } లేకపోతే ఒకవేళ(idx >= 0) {
        అక్షరం buf[256];
        int32_t len = T_CString_integer_గమ్యం_String(buf, idx, 10);
        ures_append_Res_మార్గం(resB, buf, len, స్థితి_2);
        ఒకవేళ(resB->f_Res_మార్గం_Len > 0 && resB->f_Res_మార్గం[resB->f_Res_మార్గం_Len-1] != RES_మార్గం_SEPARATOR) {
            ures_append_Res_మార్గం(resB, RES_మార్గం_SEPARATOR_S, 1, స్థితి_2);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t వాడిన_Len = ((resB->fResBuf == resB->f_Res_మార్గం) ? resB->f_Res_మార్గం_Len : 0);
        uprv_memset(resB->fResBuf + వాడిన_Len, 0, పరిమాణం(resB->fResBuf) - వాడిన_Len);
    }

    resB->f_సంస్కరణ = శూన్యసూచిక;
    resB->fRes = r;
    resB->f_కొలత = res_లెక్క_Array_వస్తువులు(&resB->get_Res_దత్తాంశం(), resB->fRes);
    ResourceTracer(resB).trace("get");
    తిరిగి resB;
}

UResourceBundle* init_resb_ఫలితం(UResource_దత్తాంశం_నమోదు* దత్తాంశం_నమోదు,
                                  Resource r,
                                  స్థిరరాశి అక్షరం* కీలకం,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  స్థిరరాశి UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_సంకేతం* స్థితి_2) {
    తిరిగి init_resb_ఫలితం(
        దత్తాంశం_నమోదు, r, కీలకం, idx,
        container->f_Valid_Locale_దత్తాంశం_నమోదు, container->f_Res_మార్గం, 0, resB, స్థితి_2);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               స్థిరరాశి UResourceBundle* original,
                               UError_సంకేతం* స్థితి_2) {
    UBool is_దొంతర_Object;
    ఒకవేళ(U_FAILURE(*స్థితి_2) || r == original) {
        తిరిగి r;
    }
    ఒకవేళ(original != శూన్యసూచిక) {
        ఒకవేళ(r == శూన్యసూచిక) {
            is_దొంతర_Object = అబద్ధం;
            r = static_cast<UResourceBundle*>(uprv_malloc(పరిమాణం(UResourceBundle)));
            /* test for nullptr */
            ఒకవేళ (r == శూన్యసూచిక) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి శూన్యసూచిక;
            }
        } లేకపోతే {
            is_దొంతర_Object = ures_is_దొంతర_Object(r);
            ures_closeBundle(r, అబద్ధం);
        }
        uprv_memcpy(r, original, పరిమాణం(UResourceBundle));
        r->f_Res_మార్గం = శూన్యసూచిక;
        r->f_Res_మార్గం_Len = 0;
        ఒకవేళ(original->f_Res_మార్గం) {
            ures_append_Res_మార్గం(r, original->f_Res_మార్గం, original->f_Res_మార్గం_Len, స్థితి_2);
        }
        ures_సమితి_Is_దొంతర_Object(r, is_దొంతర_Object);
        ఒకవేళ(r->f_దత్తాంశం != శూన్యసూచిక) {
            నమోదు_Increase(r->f_దత్తాంశం);
        }
    }
    తిరిగి r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI స్థిరరాశి char16_t* U_EXPORT2 ures_getString(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_సంకేతం* స్థితి_2) {
    స్థిరరాశి char16_t *s;
    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(resB == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    s = res_getString({resB}, &resB->get_Res_దత్తాంశం(), resB->fRes, len);
    ఒకవేళ (s == శూన్యసూచిక) {
        *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి s;
}

స్థిర స్థిరరాశి అక్షరం*
ures_గమ్యం_UTF8_String(స్థిరరాశి char16_t* s16,
                  int32_t length16,
                  అక్షరం* dest U_LIFETIME_BOUND,
                  int32_t* p_నిడివి,
                  UBool forceCopy,
                  UError_సంకేతం* స్థితి_2) {
    int32_t capacity;

    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (p_నిడివి != శూన్యసూచిక) {
        capacity = *p_నిడివి;
    } లేకపోతే {
        capacity = 0;
    }
    ఒకవేళ (capacity < 0 || (capacity > 0 && dest == శూన్యసూచిక)) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (length16 == 0) {
        /* empty string, return as read-only pointer */
        ఒకవేళ (p_నిడివి != శూన్యసూచిక) {
            *p_నిడివి = 0;
        }
        ఒకవేళ (forceCopy) {
            u_terminateChars(dest, capacity, 0, స్థితి_2);
            తిరిగి dest;
        } లేకపోతే {
            తిరిగి "";
        }
    } లేకపోతే {
        /* We need to transform the string to the destination buffer. */
        ఒకవేళ (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            తిరిగి u_str_గమ్యం_UTF8(శూన్యసూచిక, 0, p_నిడివి, s16, length16, స్థితి_2);
        }
        ఒకవేళ (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            int32_t max_నిడివి_2 = 3 * length16 + 1;
            ఒకవేళ (capacity > max_నిడివి_2) {
                dest += capacity - max_నిడివి_2;
                capacity = max_నిడివి_2;
            }
        }
        తిరిగి u_str_గమ్యం_UTF8(dest, capacity, p_నిడివి, s16, length16, స్థితి_2);
    }
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_getUTF8String(స్థిరరాశి UResourceBundle* resB,
                   అక్షరం* dest U_LIFETIME_BOUND,
                   int32_t* p_నిడివి,
                   UBool forceCopy,
                   UError_సంకేతం* స్థితి_2) {
    int32_t length16;
    స్థిరరాశి char16_t *s16 = ures_getString(resB, &length16, స్థితి_2);
    తిరిగి ures_గమ్యం_UTF8_String(s16, length16, dest, p_నిడివి, forceCopy, స్థితి_2);
}

U_CAPI స్థిరరాశి uint8_t* U_EXPORT2
ures_get_ద్విమాన(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_సంకేతం* స్థితి_2) {
  స్థిరరాశి uint8_t *p;
  ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  ఒకవేళ(resB == శూన్యసూచిక) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  p = res_get_ద్విమాన({resB}, &resB->get_Res_దత్తాంశం(), resB->fRes, len);
  ఒకవేళ (p == శూన్యసూచిక) {
    *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
  }
  తిరిగి p;
}

U_CAPI స్థిరరాశి int32_t* U_EXPORT2
ures_get_Int_సదిశ(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_సంకేతం* స్థితి_2) {
  స్థిరరాశి int32_t *p;
  ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  ఒకవేళ(resB == శూన్యసూచిక) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  p = res_get_Int_సదిశ({resB}, &resB->get_Res_దత్తాంశం(), resB->fRes, len);
  ఒకవేళ (p == శూన్యసూచిక) {
    *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
  }
  తిరిగి p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(స్థిరరాశి UResourceBundle* resB, UError_సంకేతం *స్థితి_2) {
  ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
    తిరిగి 0xffffffff;
  }
  ఒకవేళ(resB == శూన్యసూచిక) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0xffffffff;
  }
  ఒకవేళ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
    తిరిగి 0xffffffff;
  }
  తిరిగి res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(స్థిరరాశి UResourceBundle* resB, UError_సంకేతం *స్థితి_2) {
  ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
    తిరిగి 0xffffffff;
  }
  ఒకవేళ(resB == శూన్యసూచిక) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0xffffffff;
  }
  ఒకవేళ(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
    తిరిగి 0xffffffff;
  }
  తిరిగి res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(స్థిరరాశి UResourceBundle *resB) {
  ఒకవేళ(resB == శూన్యసూచిక) {
    తిరిగి URES_NONE;
  }
  తిరిగి res_get_ప్రజా_Type(resB->fRes);
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_get_కీలకం(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  ఒకవేళ(resB == శూన్యసూచిక) {
    తిరిగి శూన్యసూచిక;
  }
  తిరిగి(resB->f_కీలకం);
}

U_CAPI int32_t U_EXPORT2 ures_get_కొలత(స్థిరరాశి UResourceBundle *resB) {
  ఒకవేళ(resB == శూన్యసూచిక) {
    తిరిగి 0;
  }
  
  తిరిగి resB->f_కొలత;
}

స్థిర స్థిరరాశి char16_t*
ures_getStringWithAlias(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_సూచీ,
                        int32_t* len,
                        UError_సంకేతం* స్థితి_2) {
  ఒకవేళ(RES_GET_TYPE(r) == URES_ALIAS) {
    స్థిరరాశి char16_t* ఫలితం = శూన్యసూచిక;
    UResourceBundle *తాత్కాలిక_Res = ures_get_By_సూచీ(resB, s_సూచీ, శూన్యసూచిక, స్థితి_2);
    ఫలితం = ures_getString(తాత్కాలిక_Res, len, స్థితి_2);
    ures_close(తాత్కాలిక_Res);
    తిరిగి ఫలితం;
  } లేకపోతే {
    తిరిగి res_getString({resB, s_సూచీ}, &resB->get_Res_దత్తాంశం(), r, len); 
  }
}

U_CAPI శూన్య U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  ఒకవేళ(resB == శూన్యసూచిక) {
    తిరిగి;
  }
  resB->f_సూచీ = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_తదుపరి(స్థిరరాశి UResourceBundle *resB) {
  ఒకవేళ(resB == శూన్యసూచిక) {
    తిరిగి అబద్ధం;
  }
  తిరిగి resB->f_సూచీ < resB->f_కొలత-1;
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2
ures_get_తదుపరి_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   స్థిరరాశి అక్షరం** కీలకం,
                   UError_సంకేతం* స్థితి_2) {
  Resource r = RES_BOGUS;
  
  ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  ఒకవేళ(resB == శూన్యసూచిక) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  
  ఒకవేళ(resB->f_సూచీ == resB->f_కొలత-1) {
    *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
  } లేకపోతే {
    resB->f_సూచీ++;
    ఎంపిక(RES_GET_TYPE(resB->fRes)) {
    స్థితి URES_STRING:
    స్థితి URES_STRING_V2:
      తిరిగి res_getString({resB}, &resB->get_Res_దత్తాంశం(), resB->fRes, len);
    స్థితి URES_పట్టిక:
    స్థితి URES_TABLE16:
    స్థితి URES_TABLE32:
      r = res_get_పట్టిక_అంశం_By_సూచీ(&resB->get_Res_దత్తాంశం(), resB->fRes, resB->f_సూచీ, కీలకం);
      ఒకవేళ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      తిరిగి ures_getStringWithAlias(resB, r, resB->f_సూచీ, len, స్థితి_2);
    స్థితి URES_ARRAY:
    స్థితి URES_ARRAY16:
      r = res_get_Array_అంశం(&resB->get_Res_దత్తాంశం(), resB->fRes, resB->f_సూచీ);
      ఒకవేళ(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      తిరిగి ures_getStringWithAlias(resB, r, resB->f_సూచీ, len, స్థితి_2);
    స్థితి URES_ALIAS:
      తిరిగి ures_getStringWithAlias(resB, resB->fRes, resB->f_సూచీ, len, స్థితి_2);
    స్థితి URES_INT:
    స్థితి URES_ద్విమాన:
    స్థితి URES_INT_సదిశ:
        *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    అప్రమేయ:
      తిరిగి శూన్యసూచిక;
    }
  }

  తిరిగి శూన్యసూచిక;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_తదుపరి_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_లోపల U_LIFETIME_BOUND,
                     UError_సంకేతం* స్థితి_2) {
    స్థిరరాశి అక్షరం *కీలకం = శూన్యసూచిక;
    Resource r = RES_BOGUS;

    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
            /*return nullptr;*/
            తిరిగి fill_లోపల;
    }
    ఒకవేళ(resB == శూన్యసూచిక) {
            *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            /*return nullptr;*/
            తిరిగి fill_లోపల;
    }

    ఒకవేళ(resB->f_సూచీ == resB->f_కొలత-1) {
      *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } లేకపోతే {
        resB->f_సూచీ++;
        ఎంపిక(RES_GET_TYPE(resB->fRes)) {
        స్థితి URES_INT:
        స్థితి URES_ద్విమాన:
        స్థితి URES_STRING:
        స్థితి URES_STRING_V2:
        స్థితి URES_INT_సదిశ:
            తిరిగి ures_copyResb(fill_లోపల, resB, స్థితి_2);
        స్థితి URES_పట్టిక:
        స్థితి URES_TABLE16:
        స్థితి URES_TABLE32:
            r = res_get_పట్టిక_అంశం_By_సూచీ(&resB->get_Res_దత్తాంశం(), resB->fRes, resB->f_సూచీ, &కీలకం);
            ఒకవేళ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            తిరిగి init_resb_ఫలితం(resB->f_దత్తాంశం, r, కీలకం, resB->f_సూచీ, resB, fill_లోపల, స్థితి_2);
        స్థితి URES_ARRAY:
        స్థితి URES_ARRAY16:
            r = res_get_Array_అంశం(&resB->get_Res_దత్తాంశం(), resB->fRes, resB->f_సూచీ);
            ఒకవేళ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            తిరిగి init_resb_ఫలితం(resB->f_దత్తాంశం, r, కీలకం, resB->f_సూచీ, resB, fill_లోపల, స్థితి_2);
        అప్రమేయ:
            /*return nullptr;*/
            తిరిగి fill_లోపల;
        }
    }
    /*return nullptr;*/
    తిరిగి fill_లోపల;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_సూచీ(స్థిరరాశి UResourceBundle* resB,
                int32_t సూచీ_R,
                UResourceBundle* fill_లోపల U_LIFETIME_BOUND,
                UError_సంకేతం* స్థితి_2) {
    స్థిరరాశి అక్షరం* కీలకం = శూన్యసూచిక;
    Resource r = RES_BOGUS;

    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        /*return nullptr;*/
        తిరిగి fill_లోపల;
    }
    ఒకవేళ(resB == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        /*return nullptr;*/
        తిరిగి fill_లోపల;
    }

    ఒకవేళ(సూచీ_R >= 0 && resB->f_కొలత > సూచీ_R) {
        ఎంపిక(RES_GET_TYPE(resB->fRes)) {
        స్థితి URES_INT:
        స్థితి URES_ద్విమాన:
        స్థితి URES_STRING:
        స్థితి URES_STRING_V2:
        స్థితి URES_INT_సదిశ:
            తిరిగి ures_copyResb(fill_లోపల, resB, స్థితి_2);
        స్థితి URES_పట్టిక:
        స్థితి URES_TABLE16:
        స్థితి URES_TABLE32:
            r = res_get_పట్టిక_అంశం_By_సూచీ(&resB->get_Res_దత్తాంశం(), resB->fRes, సూచీ_R, &కీలకం);
            ఒకవేళ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            తిరిగి init_resb_ఫలితం(resB->f_దత్తాంశం, r, కీలకం, సూచీ_R, resB, fill_లోపల, స్థితి_2);
        స్థితి URES_ARRAY:
        స్థితి URES_ARRAY16:
            r = res_get_Array_అంశం(&resB->get_Res_దత్తాంశం(), resB->fRes, సూచీ_R);
            ఒకవేళ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            తిరిగి init_resb_ఫలితం(resB->f_దత్తాంశం, r, కీలకం, సూచీ_R, resB, fill_లోపల, స్థితి_2);
        అప్రమేయ:
            /*return nullptr;*/
            తిరిగి fill_లోపల;
        }
    } లేకపోతే {
        *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    తిరిగి fill_లోపల;
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2
ures_get_String_By_సూచీ(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t సూచీ_S,
                      int32_t* len,
                      UError_సంకేతం* స్థితి_2) {
    స్థిరరాశి అక్షరం* కీలకం = శూన్యసూచిక;
    Resource r = RES_BOGUS;

    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(resB == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(సూచీ_S >= 0 && resB->f_కొలత > సూచీ_S) {
        ఎంపిక(RES_GET_TYPE(resB->fRes)) {
        స్థితి URES_STRING:
        స్థితి URES_STRING_V2:
            తిరిగి res_getString({resB}, &resB->get_Res_దత్తాంశం(), resB->fRes, len);
        స్థితి URES_పట్టిక:
        స్థితి URES_TABLE16:
        స్థితి URES_TABLE32:
            r = res_get_పట్టిక_అంశం_By_సూచీ(&resB->get_Res_దత్తాంశం(), resB->fRes, సూచీ_S, &కీలకం);
            ఒకవేళ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            తిరిగి ures_getStringWithAlias(resB, r, సూచీ_S, len, స్థితి_2);
        స్థితి URES_ARRAY:
        స్థితి URES_ARRAY16:
            r = res_get_Array_అంశం(&resB->get_Res_దత్తాంశం(), resB->fRes, సూచీ_S);
            ఒకవేళ(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            తిరిగి ures_getStringWithAlias(resB, r, సూచీ_S, len, స్థితి_2);
        స్థితి URES_ALIAS:
            తిరిగి ures_getStringWithAlias(resB, resB->fRes, సూచీ_S, len, స్థితి_2);
        స్థితి URES_INT:
        స్థితి URES_ద్విమాన:
        స్థితి URES_INT_సదిశ:
            *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
            ఆపు;
        అప్రమేయ:
          /* must not occur */
          *స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR;
          ఆపు;
        }
    } లేకపోతే {
        *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
    }
    తిరిగి శూన్యసూచిక;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_get_UTF8_String_By_సూచీ(స్థిరరాశి UResourceBundle* resB,
                          int32_t idx,
                          అక్షరం* dest U_LIFETIME_BOUND,
                          int32_t* p_నిడివి,
                          UBool forceCopy,
                          UError_సంకేతం* స్థితి_2) {
    int32_t length16;
    స్థిరరాశి char16_t *s16 = ures_get_String_By_సూచీ(resB, idx, &length16, స్థితి_2);
    తిరిగి ures_గమ్యం_UTF8_String(s16, length16, dest, p_నిడివి, forceCopy, స్థితి_2);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_కనుగొను_Resource(స్థిరరాశి అక్షరం* మార్గం, UResourceBundle* fill_లోపల U_LIFETIME_BOUND, UError_సంకేతం* స్థితి_2)
{
  UResourceBundle *first = శూన్యసూచిక; 
  UResourceBundle *ఫలితం = fill_లోపల;
  అక్షరం *packageName = శూన్యసూచిక;
  అక్షరం *మార్గం_గమ్యం_Resource = శూన్యసూచిక, *save = శూన్యసూచిక;
  అక్షరం *locale = శూన్యసూచిక, *locale_చివర = శూన్యసూచిక;
  int32_t length;

  ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
    తిరిగి ఫలితం;
  }

  length = (int32_t)(uprv_strlen(మార్గం)+1);
  save = మార్గం_గమ్యం_Resource = (అక్షరం *)uprv_malloc(length*పరిమాణం(అక్షరం));
  /* test for nullptr */
  ఒకవేళ(మార్గం_గమ్యం_Resource == శూన్యసూచిక) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి ఫలితం;
  }
  uprv_memcpy(మార్గం_గమ్యం_Resource, మార్గం, length);

  locale = మార్గం_గమ్యం_Resource;
  ఒకవేళ(*మార్గం_గమ్యం_Resource == RES_మార్గం_SEPARATOR) { /* there is a path specification */
    మార్గం_గమ్యం_Resource++;
    packageName = మార్గం_గమ్యం_Resource;
    మార్గం_గమ్యం_Resource = uprv_strchr(మార్గం_గమ్యం_Resource, RES_మార్గం_SEPARATOR);
    ఒకవేళ(మార్గం_గమ్యం_Resource == శూన్యసూచిక) {
      *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    } లేకపోతే {
      *మార్గం_గమ్యం_Resource = 0;
      locale = మార్గం_గమ్యం_Resource+1;
    }
  }

  locale_చివర = uprv_strchr(locale, RES_మార్గం_SEPARATOR);
  ఒకవేళ(locale_చివర != శూన్యసూచిక) {
    *locale_చివర = 0;
  }

  first = ures_open(packageName, locale, స్థితి_2);

  ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
    ఒకవేళ(locale_చివర) {
      ఫలితం = ures_కనుగొను_Sub_Resource(first, locale_చివర+1, fill_లోపల, స్థితి_2);
    } లేకపోతే {
      ఫలితం = ures_copyResb(fill_లోపల, first, స్థితి_2);
    }
    ures_close(first);
  }
  uprv_free(save);
  తిరిగి ఫలితం;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_కనుగొను_Sub_Resource(స్థిరరాశి UResourceBundle* resB,
                     అక్షరం* మార్గం,
                     UResourceBundle* fill_లోపల U_LIFETIME_BOUND,
                     UError_సంకేతం* స్థితి_2)
{
  Resource res = RES_BOGUS;
  UResourceBundle *ఫలితం = fill_లోపల;
  స్థిరరాశి అక్షరం *కీలకం;

  ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
    తిరిగి ఫలితం;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  చేయి {
    res = res_కనుగొను_Resource(&resB->get_Res_దత్తాంశం(), resB->fRes, &మార్గం, &కీలకం); 
    ఒకవేళ(res != RES_BOGUS) {
        ఫలితం = init_resb_ఫలితం(resB->f_దత్తాంశం, res, కీలకం, -1, resB, fill_లోపల, స్థితి_2);
        resB = ఫలితం;
    } లేకపోతే {
        *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        ఆపు;
    }
  } అయితే(*మార్గం); /* there is more stuff in the path */

  తిరిగి ఫలితం;
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2
ures_get_String_By_కీలకం_With_Fallback(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND,
                                స్థిరరాశి అక్షరం* లోపల_కీలకం,
                                int32_t* len,
                                UError_సంకేతం* స్థితి_2) {
    UResourceBundle దొంతర_2;
    స్థిరరాశి char16_t* retVal = శూన్యసూచిక;
    ures_init_దొంతర_Object(&దొంతర_2);
    ures_get_By_కీలకం_With_Fallback(resB, లోపల_కీలకం, &దొంతర_2, స్థితి_2);
    int32_t length;
    retVal = ures_getString(&దొంతర_2, &length, స్థితి_2);
    ures_close(&దొంతర_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (length == 3 && retVal[0] == ఖాళీలు_సమితి && retVal[1] == ఖాళీలు_సమితి && retVal[2] == ఖాళీలు_సమితి ) {
        retVal = శూన్యసూచిక;
        length = 0;
        *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
    }
    ఒకవేళ (len != శూన్యసూచిక) {
        *len = length;
    }
    తిరిగి retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
స్థిర Resource get_పట్టిక_అంశం_By_కీలకం_మార్గం(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource పట్టిక, స్థిరరాశి అక్షరం *కీలకం) {
  Resource resource = పట్టిక;  /* The current resource */
  icu::CharString మార్గం;
  UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
  మార్గం.append(కీలకం, error_సంకేతం);
  ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి RES_BOGUS; }
  అక్షరం *మార్గం_Part = మార్గం.దత్తాంశం();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  అయితే (*మార్గం_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    అక్షరం *తదుపరి_మార్గం_Part = uprv_strchr(మార్గం_Part, RES_మార్గం_SEPARATOR);
    ఒకవేళ (తదుపరి_మార్గం_Part != శూన్యసూచిక) {
      *తదుపరి_మార్గం_Part = 0;  /* Terminating null for this part of path. */
      తదుపరి_మార్గం_Part++;
    } లేకపోతే {
      తదుపరి_మార్గం_Part = uprv_strchr(మార్గం_Part, 0);
    }
    int32_t t;
    స్థిరరాశి అక్షరం *మార్గం_P = మార్గం_Part;
    resource = res_get_పట్టిక_అంశం_By_కీలకం(p_Res_దత్తాంశం, resource, &t, &మార్గం_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    మార్గం_Part = తదుపరి_మార్గం_Part; 
  }
  ఒకవేళ (*మార్గం_Part) {
    తిరిగి RES_BOGUS;
  }
  తిరిగి resource;
}

స్థిర శూన్య create_మార్గం(స్థిరరాశి అక్షరం* orig_Res_మార్గం,
                       int32_t     orig_Res_మార్గం_Len,
                       స్థిరరాశి అక్షరం* res_మార్గం,
                       int32_t     res_మార్గం_Len,
                       స్థిరరాశి అక్షరం* లోపల_కీలకం,
                       CharString& మార్గం,
                       UError_సంకేతం* స్థితి_2) {
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
    మార్గం.clear();
    స్థిరరాశి అక్షరం* కీలకం = లోపల_కీలకం;
    ఒకవేళ (res_మార్గం_Len > 0) {
        మార్గం.append(res_మార్గం, res_మార్గం_Len, *స్థితి_2);
        ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
            స్థిరరాశి అక్షరం* res_మార్గం_హద్దు = res_మార్గం + res_మార్గం_Len;
            స్థిరరాశి అక్షరం* orig_Res_మార్గం_హద్దు = orig_Res_మార్గం + orig_Res_మార్గం_Len;
            స్థిరరాశి అక్షరం* res_మార్గం_Ptr = res_మార్గం;
            స్థిరరాశి అక్షరం* orig_Res_మార్గం_Ptr = orig_Res_మార్గం;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            అయితే (orig_Res_మార్గం_Ptr < orig_Res_మార్గం_హద్దు && res_మార్గం_Ptr < res_మార్గం_హద్దు) {
                అయితే (orig_Res_మార్గం_Ptr < orig_Res_మార్గం_హద్దు && *orig_Res_మార్గం_Ptr != RES_మార్గం_SEPARATOR) {
                    ++orig_Res_మార్గం_Ptr;
                }
                ఒకవేళ (orig_Res_మార్గం_Ptr < orig_Res_మార్గం_హద్దు && *orig_Res_మార్గం_Ptr == RES_మార్గం_SEPARATOR) {
                    ++orig_Res_మార్గం_Ptr;
                }
                అయితే (res_మార్గం_Ptr < res_మార్గం_హద్దు && *res_మార్గం_Ptr != RES_మార్గం_SEPARATOR) {
                    ++res_మార్గం_Ptr;
                }
                ఒకవేళ (res_మార్గం_Ptr < res_మార్గం_హద్దు && *res_మార్గం_Ptr == RES_మార్గం_SEPARATOR) {
                    ++res_మార్గం_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            అయితే (res_మార్గం_Ptr < res_మార్గం_హద్దు && *కీలకం != '\0') {
                అయితే (res_మార్గం_Ptr < res_మార్గం_హద్దు && *res_మార్గం_Ptr != RES_మార్గం_SEPARATOR) {
                    ++res_మార్గం_Ptr;
                }
                ఒకవేళ (res_మార్గం_Ptr < res_మార్గం_హద్దు && *res_మార్గం_Ptr == RES_మార్గం_SEPARATOR) {
                    ++res_మార్గం_Ptr;
                }
                అయితే (*కీలకం != '\0' && *కీలకం != RES_మార్గం_SEPARATOR) {
                    ++కీలకం;
                }
                ఒకవేళ (*కీలకం == RES_మార్గం_SEPARATOR) {
                    ++కీలకం;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        మార్గం.append(కీలకం, *స్థితి_2);
    } లేకపోతే {
        మార్గం.append(లోపల_కీలకం, *స్థితి_2);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_కీలకం_With_Fallback(స్థిరరాశి UResourceBundle* resB,
                          స్థిరరాశి అక్షరం* లోపల_కీలకం,
                          UResourceBundle* fill_లోపల U_LIFETIME_BOUND,
                          UError_సంకేతం* స్థితి_2) {
    Resource res = RES_BOGUS, మూలం_Res = RES_BOGUS;
    UResourceBundle *helper = శూన్యసూచిక;

    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి fill_లోపల;
    }
    ఒకవేళ(resB == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి fill_లోపల;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ఒకవేళ(URES_IS_పట్టిక(type)) {
        స్థిరరాశి అక్షరం* orig_Res_మార్గం = resB->f_Res_మార్గం;
        int32_t orig_Res_మార్గం_Len = resB->f_Res_మార్గం_Len;
        res = get_పట్టిక_అంశం_By_కీలకం_మార్గం(&resB->get_Res_దత్తాంశం(), resB->fRes, లోపల_కీలకం);
        స్థిరరాశి అక్షరం* కీలకం = లోపల_కీలకం;
        తార్కిక did_మూలం_Once = అబద్ధం;
        ఒకవేళ(res == RES_BOGUS) {
            UResource_దత్తాంశం_నమోదు *దత్తాంశం_నమోదు = resB->f_దత్తాంశం;
            CharString మార్గం;
            అక్షరం *my_మార్గం = శూన్యసూచిక;
            స్థిరరాశి అక్షరం* res_మార్గం = resB->f_Res_మార్గం;
            int32_t len = resB->f_Res_మార్గం_Len;
            అయితే(res == RES_BOGUS && (దత్తాంశం_నమోదు->f_మాతృ != శూన్యసూచిక || !did_మూలం_Once)) { /* Otherwise, we'll look in parents */
                ఒకవేళ (దత్తాంశం_నమోదు->f_మాతృ != శూన్యసూచిక) {
                    దత్తాంశం_నమోదు = దత్తాంశం_నమోదు->f_మాతృ;
                } లేకపోతే {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_మూలం_Once = నిజం;
                }
                మూలం_Res = దత్తాంశం_నమోదు->f_దత్తాంశం.మూలం_Res;

                ఒకవేళ(దత్తాంశం_నమోదు->fBogus == U_ZERO_ERROR) {
                    create_మార్గం(orig_Res_మార్గం, orig_Res_మార్గం_Len, res_మార్గం, len, లోపల_కీలకం, మార్గం, స్థితి_2);
                    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
                        ures_close(helper);
                        తిరిగి fill_లోపల;
                    }
                    my_మార్గం = మార్గం.దత్తాంశం();
                    కీలకం = లోపల_కీలకం;
                    చేయి {
                        res = res_కనుగొను_Resource(&(దత్తాంశం_నమోదు->f_దత్తాంశం), మూలం_Res, &my_మార్గం, &కీలకం);
                        ఒకవేళ (RES_GET_TYPE(res) == URES_ALIAS && *my_మార్గం) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_ఫలితం(దత్తాంశం_నమోదు, res, శూన్యసూచిక, -1, resB, helper, స్థితి_2);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            ఒకవేళ(helper) {
                              దత్తాంశం_నమోదు = helper->f_దత్తాంశం;
                              మూలం_Res = helper->fRes;
                              res_మార్గం = helper->f_Res_మార్గం;
                              len = helper->f_Res_మార్గం_Len;

                            } లేకపోతే {
                              ఆపు;
                            }
                        } లేకపోతే ఒకవేళ (res == RES_BOGUS) {
                            ఆపు;
                        }
                    } అయితే(*my_మార్గం); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            ఒకవేళ(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                ఒకవేళ(uprv_strcmp(దత్తాంశం_నమోదు->fName, uloc_getDefault())==0 || uprv_strcmp(దత్తాంశం_నమోదు->fName, k_మూలం_Locale_Name)==0) {
                    *స్థితి_2 = U_USING_DEFAULT_WARNING;
                } లేకపోతే {
                    *స్థితి_2 = U_USING_FALLBACK_WARNING;
                }

                fill_లోపల = init_resb_ఫలితం(దత్తాంశం_నమోదు, res, కీలకం, -1, resB, fill_లోపల, స్థితి_2);
                ఒకవేళ (res_మార్గం != శూన్యసూచిక) {
                    create_మార్గం(orig_Res_మార్గం, orig_Res_మార్గం_Len, res_మార్గం, len, లోపల_కీలకం, మార్గం, స్థితి_2);
                } లేకపోతే {
                    స్థిరరాశి అక్షరం* separator = శూన్యసూచిక;
                    ఒకవేళ (fill_లోపల->f_Res_మార్గం != శూన్యసూచిక) {
                        separator = uprv_strchr(fill_లోపల->f_Res_మార్గం, RES_మార్గం_SEPARATOR);
                    }
                    ఒకవేళ (separator != శూన్యసూచిక && separator[1] != '\0') {
                        create_మార్గం(orig_Res_మార్గం, orig_Res_మార్గం_Len, fill_లోపల->f_Res_మార్గం,
                                   static_cast<int32_t>(uprv_strlen(fill_లోపల->f_Res_మార్గం)), లోపల_కీలకం, మార్గం, స్థితి_2);
                    } లేకపోతే {
                        create_మార్గం(orig_Res_మార్గం, orig_Res_మార్గం_Len, "", 0, లోపల_కీలకం, మార్గం, స్థితి_2);
                    }
                }
                ures_free_Res_మార్గం(fill_లోపల);
                ures_append_Res_మార్గం(fill_లోపల, మార్గం.దత్తాంశం(), మార్గం.length(), స్థితి_2);
                ఒకవేళ(fill_లోపల->f_Res_మార్గం[fill_లోపల->f_Res_మార్గం_Len-1] != RES_మార్గం_SEPARATOR) {
                    ures_append_Res_మార్గం(fill_లోపల, RES_మార్గం_SEPARATOR_S, 1, స్థితి_2);
                }
            } లేకపోతే {
                *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
            }
        } లేకపోతే {
            fill_లోపల = init_resb_ఫలితం(resB->f_దత్తాంశం, res, కీలకం, -1, resB, fill_లోపల, స్థితి_2);
        }
    } 
    లేకపోతే {
        *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    తిరిగి fill_లోపల;
}

నామాకాశ {

శూన్య get_All_వస్తువులు_With_Fallback(
        స్థిరరాశి UResourceBundle *bundle, Resource_దత్తాంశం_విలువ &value,
        ResourceSink &sink, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
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
    value.సమితి_దత్తాంశం(bundle->get_Res_దత్తాంశం());
    value.సమితి_Valid_Locale_దత్తాంశం_నమోదు(bundle->f_Valid_Locale_దత్తాంశం_నమోదు);
    UResource_దత్తాంశం_నమోదు *మాతృ_నమోదు = bundle->f_దత్తాంశం->f_మాతృ;
    UBool has_మాతృ = మాతృ_నమోదు != శూన్యసూచిక && U_SUCCESS(మాతృ_నమోదు->fBogus);
    value.సమితి_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_కీలకం, value, !has_మాతృ, error_సంకేతం);
    ఒకవేళ (has_మాతృ) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        దొంతర_UResource_Bundle మాతృ_Bundle;
        UResourceBundle &మాతృ_Ref = మాతృ_Bundle.ref();
        మాతృ_Ref.f_దత్తాంశం = మాతృ_నమోదు;
        మాతృ_Ref.f_Valid_Locale_దత్తాంశం_నమోదు = bundle->f_Valid_Locale_దత్తాంశం_నమోదు;
        మాతృ_Ref.fHasFallback = !మాతృ_Ref.get_Res_దత్తాంశం().noFallback;
        మాతృ_Ref.f_Is_శిఖరం_Level = నిజం;
        మాతృ_Ref.fRes = మాతృ_Ref.get_Res_దత్తాంశం().మూలం_Res;
        మాతృ_Ref.f_కొలత = res_లెక్క_Array_వస్తువులు(&మాతృ_Ref.get_Res_దత్తాంశం(), మాతృ_Ref.fRes);
        మాతృ_Ref.f_సూచీ = -1;
        నమోదు_Increase(మాతృ_నమోదు);

        // Look up the container item in the parent bundle.
        దొంతర_UResource_Bundle containerBundle;
        స్థిరరాశి UResourceBundle *rb;
        UError_సంకేతం మార్గం_Error_సంకేతం = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        ఒకవేళ (bundle->f_Res_మార్గం == శూన్యసూచిక || *bundle->f_Res_మార్గం == 0) {
            rb = మాతృ_Bundle.getAlias();
        } లేకపోతే {
            rb = ures_get_By_కీలకం_With_Fallback(మాతృ_Bundle.getAlias(), bundle->f_Res_మార్గం,
                                           containerBundle.getAlias(), &మార్గం_Error_సంకేతం);
        }
        ఒకవేళ (U_SUCCESS(మార్గం_Error_సంకేతం)) {
            get_All_వస్తువులు_With_Fallback(rb, value, sink, error_సంకేతం);
        }
    }
}

నిర్మాణం GetAllChildrenSink : బహిరంగ ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    మాయా ~GetAllChildrenSink() override;
    మాయా శూన్య put(స్థిరరాశి అక్షరం *కీలకం, Resource_విలువ &value, UBool is_మూలం,
           UError_సంకేతం &error_సంకేతం) override {
        Resource_పట్టిక వస్తువులు_పట్టిక = value.get_పట్టిక(error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        కొరకు (int32_t i = 0; వస్తువులు_పట్టిక.get_కీలకం_And_విలువ(i, కీలకం, value); ++i) {
            ఒకవేళ (value.getType() == URES_ALIAS) {
                Resource_దత్తాంశం_విలువ& rdv = static_cast<Resource_దత్తాంశం_విలువ&>(value);
                దొంతర_UResource_Bundle దొంతర_తాత్కాలిక_Bundle;
                UResourceBundle* aliasRB = get_Alias_లక్ష్యం_As_Resource_Bundle(rdv.get_దత్తాంశం(), rdv.getResource(), శూన్యసూచిక, -1,
                                                                          rdv.get_Valid_Locale_దత్తాంశం_నమోదు(), శూన్యసూచిక, 0,
                                                                          దొంతర_తాత్కాలిక_Bundle.getAlias(), &error_సంకేతం);
                ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
                    Resource_దత్తాంశం_విలువ aliased_విలువ;
                    aliased_విలువ.సమితి_దత్తాంశం(aliasRB->get_Res_దత్తాంశం());
                    aliased_విలువ.సమితి_Valid_Locale_దత్తాంశం_నమోదు(aliasRB->f_Valid_Locale_దత్తాంశం_నమోదు);
                    aliased_విలువ.సమితి_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    ఒకవేళ (aliased_విలువ.getType() != URES_పట్టిక) {
                        dest.put(కీలకం, aliased_విలువ, is_మూలం, error_సంకేతం);
                    } లేకపోతే {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_విలువ_Type = URES_పట్టిక;
                        CharString పట్టిక_మార్గం;
                        పట్టిక_మార్గం.append(aliasRB->f_Res_మార్గం, error_సంకేతం);
                        స్థిరరాశి అక్షరం* మాతృ_కీలకం = కీలకం; // dest.put() changes the key
                        dest.put(మాతృ_కీలకం, aliased_విలువ, is_మూలం, error_సంకేతం);
                        UResource_దత్తాంశం_నమోదు* నమోదు_2 = aliasRB->f_దత్తాంశం;
                        Resource res = aliasRB->fRes;
                        అయితే (aliased_విలువ_Type == URES_పట్టిక && నమోదు_2->f_మాతృ != శూన్యసూచిక) {
                            CharString local_మార్గం;
                            local_మార్గం.copyFrom(పట్టిక_మార్గం, error_సంకేతం);
                            అక్షరం* local_మార్గం_As_Char_Ptr = local_మార్గం.దత్తాంశం();
                            స్థిరరాశి అక్షరం* బిడ్డ_కీలకం;
                            నమోదు_2 = నమోదు_2->f_మాతృ;
                            res = నమోదు_2->f_దత్తాంశం.మూలం_Res;
                            Resource newRes = res_కనుగొను_Resource(&నమోదు_2->f_దత్తాంశం, res, &local_మార్గం_As_Char_Ptr, &బిడ్డ_కీలకం);
                            ఒకవేళ (newRes != RES_BOGUS) {
                                aliased_విలువ.సమితి_దత్తాంశం(నమోదు_2->f_దత్తాంశం);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_విలువ.సమితి_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_విలువ_Type = aliased_విలువ.getType();
                                ఒకవేళ (aliased_విలువ_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_దత్తాంశం_విలువ& rdv2 = static_cast<Resource_దత్తాంశం_విలువ&>(aliased_విలువ);
                                    aliasRB = get_Alias_లక్ష్యం_As_Resource_Bundle(rdv2.get_దత్తాంశం(), rdv2.getResource(), శూన్యసూచిక, -1,
                                                                             rdv2.get_Valid_Locale_దత్తాంశం_నమోదు(), శూన్యసూచిక, 0,
                                                                             దొంతర_తాత్కాలిక_Bundle.getAlias(), &error_సంకేతం);
                                    పట్టిక_మార్గం.clear();
                                    పట్టిక_మార్గం.append(aliasRB->f_Res_మార్గం, error_సంకేతం);
                                    నమోదు_2 = aliasRB->f_దత్తాంశం;
                                    res = aliasRB->fRes;
                                    aliased_విలువ.సమితి_దత్తాంశం(నమోదు_2->f_దత్తాంశం);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_విలువ.సమితి_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_విలువ_Type = aliased_విలువ.getType();
                                }
                                ఒకవేళ (aliased_విలువ_Type == URES_పట్టిక) {
                                    dest.put(మాతృ_కీలకం, aliased_విలువ, is_మూలం, error_సంకేతం);
                                } లేకపోతే {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_సంకేతం = U_అంతర్గత_కార్యక్రమం_ERROR;
                                    తిరిగి;
                                }
                            }
                        }
                    }
                }
            } లేకపోతే {
                dest.put(కీలకం, value, is_మూలం, error_సంకేతం);
            }
            ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI శూన్య U_EXPORT2
ures_getAllChildrenWithFallback(స్థిరరాశి UResourceBundle *bundle, స్థిరరాశి అక్షరం *మార్గం,
                                icu::ResourceSink &sink, UError_సంకేతం &error_సంకేతం) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_వస్తువులు_With_Fallback(bundle, మార్గం, allChildrenSink, error_సంకేతం);
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
U_CAPI శూన్య U_EXPORT2
ures_get_విలువ_With_Fallback(స్థిరరాశి UResourceBundle *bundle, స్థిరరాశి అక్షరం *మార్గం,
                          UResourceBundle *తాత్కాలిక_Fill_లోపల,
                          Resource_దత్తాంశం_విలువ &value, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    ఒకవేళ (మార్గం == శూన్యసూచిక) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    స్థిరరాశి UResourceBundle *rb;
    ఒకవేళ (*మార్గం == 0) {
        // empty path
        rb = bundle;
    } లేకపోతే {
        rb = ures_get_By_కీలకం_With_Fallback(bundle, మార్గం, తాత్కాలిక_Fill_లోపల, &error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            తిరిగి;
        }
    }
    value.సమితి_దత్తాంశం(rb->get_Res_దత్తాంశం());
    value.సమితి_Valid_Locale_దత్తాంశం_నమోదు(rb->f_Valid_Locale_దత్తాంశం_నమోదు);
    value.సమితి_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI శూన్య U_EXPORT2
ures_get_All_వస్తువులు_With_Fallback(స్థిరరాశి UResourceBundle *bundle, స్థిరరాశి అక్షరం *మార్గం,
                             icu::ResourceSink &sink, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    ఒకవేళ (మార్గం == శూన్యసూచిక) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    దొంతర_UResource_Bundle దొంతర_Bundle;
    స్థిరరాశి UResourceBundle *rb;
    ఒకవేళ (*మార్గం == 0) {
        // empty path
        rb = bundle;
    } లేకపోతే {
        rb = ures_get_By_కీలకం_With_Fallback(bundle, మార్గం, దొంతర_Bundle.getAlias(), &error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            తిరిగి;
        }
    }
    // Get all table items with fallback.
    Resource_దత్తాంశం_విలువ value;
    get_All_వస్తువులు_With_Fallback(rb, value, sink, error_సంకేతం);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_కీలకం(స్థిరరాశి UResourceBundle* resB,
              స్థిరరాశి అక్షరం* లోపల_కీలకం,
              UResourceBundle* fill_లోపల U_LIFETIME_BOUND,
              UError_సంకేతం* స్థితి_2) {
    Resource res = RES_BOGUS;
    UResource_దత్తాంశం_నమోదు *దత్తాంశం_నమోదు = శూన్యసూచిక;
    స్థిరరాశి అక్షరం *కీలకం = లోపల_కీలకం;

    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి fill_లోపల;
    }
    ఒకవేళ(resB == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి fill_లోపల;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ఒకవేళ(URES_IS_పట్టిక(type)) {
        int32_t t;
        res = res_get_పట్టిక_అంశం_By_కీలకం(&resB->get_Res_దత్తాంశం(), resB->fRes, &t, &కీలకం);
        ఒకవేళ(res == RES_BOGUS) {
            కీలకం = లోపల_కీలకం;
            ఒకవేళ(resB->fHasFallback) {
                దత్తాంశం_నమోదు = get_Fallback_దత్తాంశం(resB, &కీలకం, &res, స్థితి_2);
                ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
                    /* check if resB->fResPath gives the right name here */
                    తిరిగి init_resb_ఫలితం(దత్తాంశం_నమోదు, res, కీలకం, -1, resB, fill_లోపల, స్థితి_2);
                } లేకపోతే {
                    *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
                }
            } లేకపోతే {
                *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
            }
        } లేకపోతే {
            తిరిగి init_resb_ఫలితం(resB->f_దత్తాంశం, res, కీలకం, -1, resB, fill_లోపల, స్థితి_2);
        }
    } 
#ఒకవేళ 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    లేకపోతే ఒకవేళ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == నిజం) {
        /* here should go a first attempt to locate the key using index table */
        దత్తాంశం_నమోదు = get_Fallback_దత్తాంశం(resB, &కీలకం, &res, స్థితి_2);
        ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
            తిరిగి init_resb_ఫలితం(దత్తాంశం_నమోదు, res, కీలకం, resB, fill_లోపల, స్థితి_2);
        } లేకపోతే {
            *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        }
    }
#అయితే_ముగింపు    
    లేకపోతే {
        *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి fill_లోపల;
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2
ures_get_String_By_కీలకం(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND,
                    స్థిరరాశి అక్షరం* లోపల_కీలకం,
                    int32_t* len,
                    UError_సంకేతం* స్థితి_2) {
    Resource res = RES_BOGUS;
    UResource_దత్తాంశం_నమోదు *దత్తాంశం_నమోదు = శూన్యసూచిక;
    స్థిరరాశి అక్షరం* కీలకం = లోపల_కీలకం;

    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(resB == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    ఒకవేళ(URES_IS_పట్టిక(type)) {
        int32_t t=0;

        res = res_get_పట్టిక_అంశం_By_కీలకం(&resB->get_Res_దత్తాంశం(), resB->fRes, &t, &కీలకం);

        ఒకవేళ(res == RES_BOGUS) {
            కీలకం = లోపల_కీలకం;
            ఒకవేళ(resB->fHasFallback) {
                దత్తాంశం_నమోదు = get_Fallback_దత్తాంశం(resB, &కీలకం, &res, స్థితి_2);
                ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
                    ఎంపిక (RES_GET_TYPE(res)) {
                    స్థితి URES_STRING:
                    స్థితి URES_STRING_V2:
                        తిరిగి res_getString({resB, కీలకం}, &దత్తాంశం_నమోదు->f_దత్తాంశం, res, len);
                    స్థితి URES_ALIAS:
                      {
                        స్థిరరాశి char16_t* ఫలితం = శూన్యసూచిక;
                        UResourceBundle *తాత్కాలిక_Res = ures_get_By_కీలకం(resB, లోపల_కీలకం, శూన్యసూచిక, స్థితి_2);
                        ఫలితం = ures_getString(తాత్కాలిక_Res, len, స్థితి_2);
                        ures_close(తాత్కాలిక_Res);
                        తిరిగి ఫలితం;
                      }
                    అప్రమేయ:
                        *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
                    }
                } లేకపోతే {
                    *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
                }
            } లేకపోతే {
                *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
            }
        } లేకపోతే {
            ఎంపిక (RES_GET_TYPE(res)) {
            స్థితి URES_STRING:
            స్థితి URES_STRING_V2:
                తిరిగి res_getString({resB, కీలకం}, &resB->get_Res_దత్తాంశం(), res, len);
            స్థితి URES_ALIAS:
              {
                స్థిరరాశి char16_t* ఫలితం = శూన్యసూచిక;
                UResourceBundle *తాత్కాలిక_Res = ures_get_By_కీలకం(resB, లోపల_కీలకం, శూన్యసూచిక, స్థితి_2);
                ఫలితం = ures_getString(తాత్కాలిక_Res, len, స్థితి_2);
                ures_close(తాత్కాలిక_Res);
                తిరిగి ఫలితం;
              }
            అప్రమేయ:
                *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#ఒకవేళ 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    లేకపోతే ఒకవేళ(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == నిజం) {
        /* here should go a first attempt to locate the key using index table */
        దత్తాంశం_నమోదు = get_Fallback_దత్తాంశం(resB, &కీలకం, &res, స్థితి_2);
        ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
            // TODO: Tracing
            తిరిగి res_getString(rd, res, len);
        } లేకపోతే {
            *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        }
    } 
#అయితే_ముగింపు    
    లేకపోతే {
        *స్థితి_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి శూన్యసూచిక;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_get_UTF8_String_By_కీలకం(స్థిరరాశి UResourceBundle* resB,
                        స్థిరరాశి అక్షరం* కీలకం,
                        అక్షరం* dest U_LIFETIME_BOUND,
                        int32_t* p_నిడివి,
                        UBool forceCopy,
                        UError_సంకేతం* స్థితి_2) {
    int32_t length16;
    స్థిరరాశి char16_t *s16 = ures_get_String_By_కీలకం(resB, కీలకం, &length16, స్థితి_2);
    తిరిగి ures_గమ్యం_UTF8_String(s16, length16, dest, p_నిడివి, forceCopy, స్థితి_2);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_get_Locale_అంతర్గత(స్థిరరాశి UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (!resourceBundle) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
      తిరిగి resourceBundle->f_దత్తాంశం->fName;
    }
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_getLocale(స్థిరరాశి UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_సంకేతం* స్థితి_2)
{
  తిరిగి ures_get_Locale_అంతర్గత(resourceBundle, స్థితి_2);
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_getLocaleByType(స్థిరరాశి UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_దత్తాంశం_Locale_Type type,
                     UError_సంకేతం* స్థితి_2) {
    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (!resourceBundle) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        ఎంపిక(type) {
        స్థితి ULOC_ACTUAL_LOCALE:
            తిరిగి resourceBundle->f_దత్తాంశం->fName;
        స్థితి ULOC_VALID_LOCALE:
            తిరిగి resourceBundle->f_Valid_Locale_దత్తాంశం_నమోదు->fName;
        స్థితి ULOC_REQUESTED_LOCALE:
        అప్రమేయ:
            *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి శూన్యసూచిక;
        }
    }
}

U_CFUNC స్థిరరాశి అక్షరం*
ures_getName(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND) {
  ఒకవేళ(resB == శూన్యసూచిక) {
    తిరిగి శూన్యసూచిక;
  }

  తిరిగి resB->f_దత్తాంశం->fName;
}

#నిర్వచితమైతే URES_DEBUG
U_CFUNC స్థిరరాశి అక్షరం*
ures_get_మార్గం(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND) {
  ఒకవేళ(resB == శూన్యసూచిక) {
    తిరిగి శూన్యసూచిక;
  }

  తిరిగి resB->f_దత్తాంశం->f_మార్గం;
}
#అయితే_ముగింపు

స్థిర UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  స్థిరరాశి అక్షరం* మార్గం,
                  స్థిరరాశి అక్షరం* localeID,
                  UResOpenType openType,
                  UError_సంకేతం* స్థితి_2) {
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    UResource_దత్తాంశం_నమోదు *నమోదు_2;
    ఒకవేళ(openType != URES_OPEN_ప్రత్యక్ష) {
        ఒకవేళ (localeID == శూన్యసూచిక) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_పునాది_Name(localeID, *స్థితి_2);
        ఒకవేళ(U_FAILURE(*స్థితి_2)) {
            *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        నమోదు_2 = నమోదు_Open(మార్గం, canonLocaleID.దత్తాంశం(), openType, స్థితి_2);
    } లేకపోతే {
        నమోదు_2 = నమోదు_Open_ప్రత్యక్ష(మార్గం, localeID, స్థితి_2);
    }
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(నమోదు_2 == శూన్యసూచిక) {
        *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    UBool is_దొంతర_Object;
    ఒకవేళ(r == శూన్యసూచిక) {
        r = static_cast<UResourceBundle*>(uprv_malloc(పరిమాణం(UResourceBundle)));
        ఒకవేళ(r == శూన్యసూచిక) {
            నమోదు_Close(నమోదు_2);
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        is_దొంతర_Object = అబద్ధం;
    } లేకపోతే {  // fill-in
        is_దొంతర_Object = ures_is_దొంతర_Object(r);
        ures_closeBundle(r, అబద్ధం);
    }
    uprv_memset(r, 0, పరిమాణం(UResourceBundle));
    ures_సమితి_Is_దొంతర_Object(r, is_దొంతర_Object);

    r->f_Valid_Locale_దత్తాంశం_నమోదు = r->f_దత్తాంశం = నమోదు_2;
    r->fHasFallback = openType != URES_OPEN_ప్రత్యక్ష && !r->get_Res_దత్తాంశం().noFallback;
    r->f_Is_శిఖరం_Level = నిజం;
    r->fRes = r->get_Res_దత్తాంశం().మూలం_Res;
    r->f_కొలత = res_లెక్క_Array_వస్తువులు(&r->get_Res_దత్తాంశం(), r->fRes);
    r->f_సూచీ = -1;

    ResourceTracer(r).traceOpen();

    తిరిగి r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* localeID, UError_సంకేతం* స్థితి_2) {
    తిరిగి ures_openWithType(శూన్యసూచిక, మార్గం, localeID, URES_OPEN_LOCALE_DEFAULT_మూలం, స్థితి_2);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* localeID, UError_సంకేతం* స్థితి_2) {
    తిరిగి ures_openWithType(శూన్యసూచిక, మార్గం, localeID, URES_OPEN_LOCALE_మూలం, స్థితి_2);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_ప్రత్యక్ష(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* localeID, UError_సంకేతం* స్థితి_2) {
    తిరిగి ures_openWithType(శూన్యసూచిక, మార్గం, localeID, URES_OPEN_ప్రత్యక్ష, స్థితి_2);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI శూన్య U_EXPORT2
ures_open_Fill_లోపల(UResourceBundle *r, స్థిరరాశి అక్షరం* మార్గం,
                స్థిరరాశి అక్షరం* localeID, UError_సంకేతం* స్థితి_2) {
    ఒకవేళ(U_SUCCESS(*స్థితి_2) && r == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ures_openWithType(r, మార్గం, localeID, URES_OPEN_LOCALE_DEFAULT_మూలం, స్థితి_2);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI శూన్య U_EXPORT2
ures_open_ప్రత్యక్ష_Fill_లోపల(UResourceBundle *r, స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* localeID, UError_సంకేతం* స్థితి_2) {
    ఒకవేళ(U_SUCCESS(*స్థితి_2) && r == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ures_openWithType(r, మార్గం, localeID, URES_OPEN_ప్రత్యక్ష, స్థితి_2);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_లెక్క_Array_వస్తువులు(స్థిరరాశి UResourceBundle* resourceBundle,
                  స్థిరరాశి అక్షరం* resource_కీలకం,
                  UError_సంకేతం* స్థితి_2)
{
    UResourceBundle res_దత్తాంశం;
    ures_init_దొంతర_Object(&res_దత్తాంశం);
    ఒకవేళ (స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ(resourceBundle == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    ures_get_By_కీలకం(resourceBundle, resource_కీలకం, &res_దత్తాంశం, స్థితి_2);
    
    ఒకవేళ(res_దత్తాంశం.get_Res_దత్తాంశం().దత్తాంశం != శూన్యసూచిక) {
        int32_t ఫలితం = res_లెక్క_Array_వస్తువులు(&res_దత్తాంశం.get_Res_దత్తాంశం(), res_దత్తాంశం.fRes);
        ures_close(&res_దత్తాంశం);
        తిరిగి ఫలితం;
    } లేకపోతే {
        *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_దత్తాంశం);
        తిరిగి 0;
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
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_get_సంస్కరణ_సంఖ్య_అంతర్గత(స్థిరరాశి UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    ఒకవేళ (!resourceBundle) తిరిగి శూన్యసూచిక;

    ఒకవేళ(resourceBundle->f_సంస్కరణ == శూన్యసూచిక) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        స్థిరరాశి char16_t* minor_సంస్కరణ = ures_get_String_By_కీలకం(resourceBundle, k_సంస్కరణ_ట్యాగ్, &minor_len, &స్థితి_2);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_సంస్కరణ = (అక్షరం *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        ఒకవేళ (((UResourceBundle *)resourceBundle)->f_సంస్కరణ == శూన్యసూచిక) {
            తిరిగి శూన్యసూచిక;
        }
       
        ఒకవేళ(minor_len > 0) {
            u_UChars_గమ్యం_Chars(minor_సంస్కరణ, resourceBundle->f_సంస్కరణ , minor_len);
            resourceBundle->f_సంస్కరణ[len] =  '\0';
        }
        లేకపోతే {
            uprv_strcpy(resourceBundle->f_సంస్కరణ, k_Default_Minor_సంస్కరణ);
        }
    }

    తిరిగి resourceBundle->f_సంస్కరణ;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_get_సంస్కరణ_సంఖ్య(స్థిరరాశి UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    తిరిగి ures_get_సంస్కరణ_సంఖ్య_అంతర్గత(resourceBundle);
}

U_CAPI శూన్య U_EXPORT2 ures_get_సంస్కరణ(స్థిరరాశి UResourceBundle* resB, UVersionInfo సంస్కరణ_Info) {
    ఒకవేళ (!resB) తిరిగి;

    u_సంస్కరణ_From_String(సంస్కరణ_Info, ures_get_సంస్కరణ_సంఖ్య_అంతర్గత(resB));
}

/** Tree support functions *******************************/
#నిర్వచించు సూచీ_LOCALE_NAME "res_index"
#నిర్వచించు సూచీ_ట్యాగ్         "InstalledLocales"
#నిర్వచించు DEFAULT_ట్యాగ్       "default"

#ఒకవేళ defined(URES_వృక్షం_DEBUG)
#అన్తర్భావ <stdio.h>
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

స్థిర శూన్య U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

స్థిర int32_t U_CALLCONV
ures_loc_లెక్క_Locales(UEnumeration *en, UError_సంకేతం * /*స్థితి*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    తిరిగి ures_get_కొలత(&ctx->installed);
}

U_CDECL_BEGIN


స్థిర స్థిరరాశి అక్షరం * U_CALLCONV
ures_loc_తదుపరి_Locale(UEnumeration* en,
                    int32_t* ఫలితం_నిడివి,
                    UError_సంకేతం* స్థితి_2) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = శూన్యసూచిక;
    స్థిరరాశి అక్షరం *ఫలితం = శూన్యసూచిక;
    int32_t len = 0;
    ఒకవేళ (ures_has_తదుపరి(res) && (k = ures_get_తదుపరి_Resource(res, &ctx->curr, స్థితి_2)) != శూన్యసూచిక) {
        ఫలితం = ures_get_కీలకం(k);
        len = (int32_t)uprv_strlen(ఫలితం);
    }
    ఒకవేళ (ఫలితం_నిడివి) {
        *ఫలితం_నిడివి = len;
    }
    తిరిగి ఫలితం;
}

స్థిర శూన్య U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_సంకేతం* /*స్థితి*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_చివర

స్థిర స్థిరరాశి UEnumeration gLocalesEnum = {
    శూన్యసూచిక,
        శూన్యసూచిక,
        ures_loc_closeLocales,
        ures_loc_లెక్క_Locales,
        uenum_unextDefault,
        ures_loc_తదుపరి_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_అందుబాటు_Locales(స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2)
{
    UResourceBundle *idx = శూన్యసూచిక;
    UEnumeration *en = శూన్యసూచిక;
    ULocalesContext *myContext = శూన్యసూచిక;

    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(పరిమాణం(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(పరిమాణం(UEnumeration));
    ఒకవేళ(!en || !myContext) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(en, &gLocalesEnum, పరిమాణం(UEnumeration));

    ures_init_దొంతర_Object(&myContext->installed);
    ures_init_దొంతర_Object(&myContext->curr);
    idx = ures_open_ప్రత్యక్ష(మార్గం, సూచీ_LOCALE_NAME, స్థితి_2);
    ures_get_By_కీలకం(idx, సూచీ_ట్యాగ్, &myContext->installed, స్థితి_2);
    ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            మార్గం, సూచీ_LOCALE_NAME, సూచీ_ట్యాగ్, ures_get_కీలకం(&myContext->installed));
#అయితే_ముగింపు
        en->context = myContext;
    } లేకపోతే {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", మార్గం, u_errorName(*స్థితి_2));
#అయితే_ముగింపు
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = శూన్యసూచిక;
    }
    
    ures_close(idx);
    
    తిరిగి en;
}

స్థిర UBool is_Locale_లోపల_List(UEnumeration *locEnum, స్థిరరాశి అక్షరం *loc_గమ్యం_శోధన, UError_సంకేతం *స్థితి_2) {
    స్థిరరాశి అక్షరం *loc;
    అయితే ((loc = uenum_తదుపరి(locEnum, శూన్యసూచిక, స్థితి_2)) != శూన్యసూచిక) {
        ఒకవేళ (uprv_strcmp(loc, loc_గమ్యం_శోధన) == 0) {
            తిరిగి నిజం;
        }
    }
    తిరిగి అబద్ధం;
}

స్థిర శూన్య get_మాతృ_For_Functional_Equivalent(స్థిరరాశి అక్షరం*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      మాతృ) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
    మాతృ.clear();
    ఒకవేళ (res != శూన్యసూచిక) {
        ures_get_By_కీలకం(res, "%%Parent", bund1, &sub_స్థితి);
        ఒకవేళ (U_SUCCESS(sub_స్థితి)) {
            int32_t length16;
            స్థిరరాశి char16_t* s16 = ures_getString(bund1, &length16, &sub_స్థితి);
            మాతృ.appendInvariantChars(s16, length16, sub_స్థితి);
        }
    }
    
    // If none there, use normal truncation parent
    ఒకవేళ (U_FAILURE(sub_స్థితి) || మాతృ.is_ఖాళీలు()) {
        sub_స్థితి = U_ZERO_ERROR;
        మాతృ = ulocimp_get_మాతృ(localeID, sub_స్థితి);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(అక్షరం *ఫలితం, int32_t ఫలితం_సామర్థ్యం,
                             స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *resName, స్థిరరాశి అక్షరం *keyword, స్థిరరాశి అక్షరం *locid,
                             UBool *is_అందుబాటు, UBool omitDefault, UError_సంకేతం *స్థితి_2)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString దొరికింది;
    CharString మాతృ;
    CharString నిండిన;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = శూన్యసూచిక;
    UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
    int32_t length = 0;
    ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి 0;
    CharString kwVal;
    ఒకవేళ (keyword != శూన్యసూచిక && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_విలువ(locid, keyword, sub_స్థితి);
        ఒకవేళ (kwVal == DEFAULT_ట్యాగ్) {
            kwVal.clear();
        }
    }
    ఒకవేళ (locid == శూన్యసూచిక) {
        locid = uloc_getDefault();
    }
    CharString పునాది = ulocimp_get_పునాది_Name(locid, sub_స్థితి);
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.దత్తాంశం(), పునాది.దత్తాంశం(), u_errorName(sub_స్థితి));
#అయితే_ముగింపు
    ures_init_దొంతర_Object(&bund1);
    ures_init_దొంతర_Object(&bund2);

    మాతృ.copyFrom(పునాది, sub_స్థితి);
    దొరికింది.copyFrom(పునాది, sub_స్థితి);

    ఒకవేళ(is_అందుబాటు) {
        UEnumeration *locEnum = ures_open_అందుబాటు_Locales(మార్గం, &sub_స్థితి);
        *is_అందుబాటు = నిజం;
        ఒకవేళ (U_SUCCESS(sub_స్థితి)) {
            *is_అందుబాటు = is_Locale_లోపల_List(locEnum, మాతృ.దత్తాంశం(), &sub_స్థితి);
        }
        uenum_close(locEnum);
    }

    ఒకవేళ(U_FAILURE(sub_స్థితి)) {
        *స్థితి_2 = sub_స్థితి;
        తిరిగి 0;
    }
    
    చేయి {
        sub_స్థితి = U_ZERO_ERROR;
        res = ures_open(మార్గం, మాతృ.దత్తాంశం(), &sub_స్థితి);
        ఒకవేళ(((sub_స్థితి == U_USING_FALLBACK_WARNING) ||
            (sub_స్థితి == U_USING_DEFAULT_WARNING)) && is_అందుబాటు)
        {
            *is_అందుబాటు = అబద్ధం;
        }
        is_అందుబాటు = శూన్యసూచిక; /* only want to set this the first time around */
        
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", మార్గం?మార్గం:"ICUDATA", మాతృ.దత్తాంశం(), u_errorName(sub_స్థితి), ures_getLocale(res, &sub_స్థితి));
#అయితే_ముగింపు
        ఒకవేళ(U_FAILURE(sub_స్థితి)) {
            *స్థితి_2 = sub_స్థితి;
        } లేకపోతే ఒకవేళ(sub_స్థితి == U_ZERO_ERROR) {
            ures_get_By_కీలకం(res,resName,&bund1, &sub_స్థితి);
            ఒకవేళ(sub_స్థితి == U_ZERO_ERROR) {
                స్థిరరాశి char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    మార్గం?మార్గం:"ICUDATA", మాతృ.దత్తాంశం(), u_errorName(sub_స్థితి));
#అయితే_ముగింపు
                defUstr = ures_get_String_By_కీలకం(&bund1, DEFAULT_ట్యాగ్, &defLen, &sub_స్థితి);
                ఒకవేళ(U_SUCCESS(sub_స్థితి) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_స్థితి);
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        మార్గం?మార్గం:"ICUDATA", మాతృ.దత్తాంశం(), keyword, defVal.దత్తాంశం(), u_errorName(sub_స్థితి));
#అయితే_ముగింపు
                    defLoc.copyFrom(మాతృ, sub_స్థితి);
                    ఒకవేళ(kwVal.is_ఖాళీలు()) {
                        kwVal.append(defVal, sub_స్థితి);
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            మార్గం?మార్గం:"ICUDATA", మాతృ.దత్తాంశం(), keyword, kwVal.దత్తాంశం());
#అయితే_ముగింపు
                    }
                }
            }
        }
        
        sub_స్థితి = U_ZERO_ERROR;

        ఒకవేళ (res != శూన్యసూచిక) {
            దొరికింది.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_స్థితి), sub_స్థితి);
        }

        ఒకవేళ (దొరికింది != మాతృ) {
            మాతృ.copyFrom(దొరికింది, sub_స్థితి);
        } లేకపోతే {
            get_మాతృ_For_Functional_Equivalent(దొరికింది.దత్తాంశం(),res,&bund1,మాతృ);
        }
        ures_close(res);
    } అయితే(defVal.is_ఖాళీలు() && !దొరికింది.is_ఖాళీలు() && దొరికింది != "root" && U_SUCCESS(*స్థితి_2));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    మాతృ.copyFrom(పునాది, sub_స్థితి);
    దొరికింది.copyFrom(పునాది, sub_స్థితి);

    చేయి {
        res = ures_open(మార్గం, మాతృ.దత్తాంశం(), &sub_స్థితి);
        ఒకవేళ((sub_స్థితి == U_USING_FALLBACK_WARNING) && is_అందుబాటు) {
            *is_అందుబాటు = అబద్ధం;
        }
        is_అందుబాటు = శూన్యసూచిక; /* only want to set this the first time around */
        
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            మార్గం?మార్గం:"ICUDATA", మాతృ.దత్తాంశం(), u_errorName(sub_స్థితి), kwVal.దత్తాంశం());
#అయితే_ముగింపు
        ఒకవేళ(U_FAILURE(sub_స్థితి)) {
            *స్థితి_2 = sub_స్థితి;
        } లేకపోతే ఒకవేళ(sub_స్థితి == U_ZERO_ERROR) {
            ures_get_By_కీలకం(res,resName,&bund1, &sub_స్థితి);
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_స్థితి));
#అయితే_ముగింపు
            ఒకవేళ(sub_స్థితి == U_ZERO_ERROR) {
                ures_get_By_కీలకం(&bund1, kwVal.దత్తాంశం(), &bund2, &sub_స్థితి);
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.దత్తాంశం(), u_errorName(sub_స్థితి));
#అయితే_ముగింపు
                ఒకవేళ(sub_స్థితి == U_ZERO_ERROR) {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        మార్గం?మార్గం:"ICUDATA", మాతృ.దత్తాంశం(), keyword, kwVal.దత్తాంశం(), u_errorName(sub_స్థితి));
#అయితే_ముగింపు
                    ఒకవేళ (మాతృ.is_ఖాళీలు()) {
                        నిండిన.clear().append("root", sub_స్థితి);
                    } లేకపోతే {
                        నిండిన.copyFrom(మాతృ, sub_స్థితి);
                    }
                        /* now, recalculate default kw if need be */
                        ఒకవేళ(defLoc.length() > నిండిన.length()) {
                          స్థిరరాశి char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    మార్గం?మార్గం:"ICUDATA", నిండిన.దత్తాంశం());
#అయితే_ముగింపు
                          defUstr = ures_get_String_By_కీలకం(&bund1, DEFAULT_ట్యాగ్, &defLen, &sub_స్థితి);
                          ఒకవేళ(U_SUCCESS(sub_స్థితి) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_స్థితి);
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    మార్గం?మార్గం:"ICUDATA", నిండిన.దత్తాంశం(), keyword, defVal.దత్తాంశం(), u_errorName(sub_స్థితి));
#అయితే_ముగింపు
                            defLoc.copyFrom(నిండిన, sub_స్థితి);
                          }
                        } /* end of recalculate default KW */
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                        లేకపోతే {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.దత్తాంశం(), నిండిన.దత్తాంశం());
                        }
#అయితే_ముగింపు
                } లేకపోతే {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_స్థితి), మాతృ.దత్తాంశం(), kwVal.దత్తాంశం());
#అయితే_ముగింపు
                }
            }
        }
        
        sub_స్థితి = U_ZERO_ERROR;
        UBool have_దొరికింది = అబద్ధం;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        ఒకవేళ (res != శూన్యసూచిక && uprv_strcmp(resName, "collations") == 0) {
            స్థిరరాశి అక్షరం *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_స్థితి);
            ఒకవేళ (U_SUCCESS(sub_స్థితి) && validLoc != శూన్యసూచిక && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_స్థితి);
                CharString మాతృ_Lang = ulocimp_getLanguage(మాతృ.గమ్యం_String_Piece(), sub_స్థితి);
                ఒకవేళ (U_SUCCESS(sub_స్థితి) && validLang != మాతృ_Lang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    దొరికింది.clear().append(validLoc, sub_స్థితి);
                    have_దొరికింది = నిజం;
                }
            }
            sub_స్థితి = U_ZERO_ERROR;
        }
        ఒకవేళ (!have_దొరికింది) {
            దొరికింది.copyFrom(మాతృ, sub_స్థితి);
        }

        get_మాతృ_For_Functional_Equivalent(దొరికింది.దత్తాంశం(),res,&bund1,మాతృ);
        ures_close(res);
        sub_స్థితి = U_ZERO_ERROR;
    } అయితే(నిండిన.is_ఖాళీలు() && !దొరికింది.is_ఖాళీలు() && U_SUCCESS(*స్థితి_2));

    ఒకవేళ(నిండిన.is_ఖాళీలు() && kwVal != defVal) {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.దత్తాంశం(), defVal.దత్తాంశం());
#అయితే_ముగింపు
        kwVal.clear().append(defVal, sub_స్థితి);
        మాతృ.copyFrom(పునాది, sub_స్థితి);
        దొరికింది.copyFrom(పునాది, sub_స్థితి);

        చేయి { /* search for 'default' named item */
            res = ures_open(మార్గం, మాతృ.దత్తాంశం(), &sub_స్థితి);
            ఒకవేళ((sub_స్థితి == U_USING_FALLBACK_WARNING) && is_అందుబాటు) {
                *is_అందుబాటు = అబద్ధం;
            }
            is_అందుబాటు = శూన్యసూచిక; /* only want to set this the first time around */
            
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                మార్గం?మార్గం:"ICUDATA", మాతృ.దత్తాంశం(), u_errorName(sub_స్థితి), kwVal.దత్తాంశం());
#అయితే_ముగింపు
            ఒకవేళ(U_FAILURE(sub_స్థితి)) {
                *స్థితి_2 = sub_స్థితి;
            } లేకపోతే ఒకవేళ(sub_స్థితి == U_ZERO_ERROR) {
                ures_get_By_కీలకం(res,resName,&bund1, &sub_స్థితి);
                ఒకవేళ(sub_స్థితి == U_ZERO_ERROR) {
                    ures_get_By_కీలకం(&bund1, kwVal.దత్తాంశం(), &bund2, &sub_స్థితి);
                    ఒకవేళ(sub_స్థితి == U_ZERO_ERROR) {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", మార్గం?మార్గం:"ICUDATA",
                            మాతృ.దత్తాంశం(), keyword, kwVal.దత్తాంశం(), u_errorName(sub_స్థితి));
#అయితే_ముగింపు
                        ఒకవేళ (మాతృ.is_ఖాళీలు()) {
                            నిండిన.clear().append("root", sub_స్థితి);
                        } లేకపోతే {
                            నిండిన.copyFrom(మాతృ, sub_స్థితి);
                        }
                        
                        /* now, recalculate default kw if need be */
                        ఒకవేళ(defLoc.length() > నిండిన.length()) {
                          స్థిరరాశి char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    మార్గం?మార్గం:"ICUDATA", నిండిన.దత్తాంశం());
#అయితే_ముగింపు
                          defUstr = ures_get_String_By_కీలకం(&bund1, DEFAULT_ట్యాగ్, &defLen, &sub_స్థితి);
                          ఒకవేళ(U_SUCCESS(sub_స్థితి) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_స్థితి);
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    మార్గం?మార్గం:"ICUDATA", నిండిన.దత్తాంశం(), keyword, defVal.దత్తాంశం(), u_errorName(sub_స్థితి));
#అయితే_ముగింపు
                            defLoc.copyFrom(నిండిన, sub_స్థితి);
                          }
                        } /* end of recalculate default KW */
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                        లేకపోతే {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.దత్తాంశం(), నిండిన.దత్తాంశం());
                        }
#అయితే_ముగింపు
                    }
                }
            }
            
            sub_స్థితి = U_ZERO_ERROR;
            దొరికింది.copyFrom(మాతృ, sub_స్థితి);
            get_మాతృ_For_Functional_Equivalent(దొరికింది.దత్తాంశం(),res,&bund1,మాతృ);
            ures_close(res);
            sub_స్థితి = U_ZERO_ERROR;
        } అయితే(నిండిన.is_ఖాళీలు() && !దొరికింది.is_ఖాళీలు() && U_SUCCESS(*స్థితి_2));
    }
    
    ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
        ఒకవేళ(నిండిన.is_ఖాళీలు()) {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.దత్తాంశం());
#అయితే_ముగింపు
          *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
        } లేకపోతే ఒకవేళ(omitDefault) {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", నిండిన.దత్తాంశం(), defLoc.దత్తాంశం(), దొరికింది.దత్తాంశం());
#అయితే_ముగింపు        
          ఒకవేళ(defLoc.length() <= నిండిన.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            ఒకవేళ(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.దత్తాంశం());
#అయితే_ముగింపు
              kwVal.clear();
            }
          }
        }
        దొరికింది.copyFrom(నిండిన, sub_స్థితి);
        ఒకవేళ(!kwVal.is_ఖాళీలు()) {
            దొరికింది
                .append("@", sub_స్థితి)
                .append(keyword, sub_స్థితి)
                .append("=", sub_స్థితి)
                .append(kwVal, sub_స్థితి);
        } లేకపోతే ఒకవేళ(!omitDefault) {
            దొరికింది
                .append("@", sub_స్థితి)
                .append(keyword, sub_స్థితి)
                .append("=", sub_స్థితి)
                .append(defVal, sub_స్థితి);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = దొరికింది.length();

    ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
        int32_t copy_నిడివి = uprv_min(length, ఫలితం_సామర్థ్యం);
        ఒకవేళ(copy_నిడివి>0) {
            దొరికింది.extract(ఫలితం, copy_నిడివి, sub_స్థితి);
        }
        ఒకవేళ(length == 0) {
          *స్థితి_2 = U_MISSING_RESOURCE_ERROR; 
        }
    } లేకపోతే {
        length = 0;
        ఫలితం[0]=0;
    }
    తిరిగి u_terminateChars(ఫలితం, ఫలితం_సామర్థ్యం, length, స్థితి_2);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_విలువలు(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *keyword, UError_సంకేతం *స్థితి_2)
{
#నిర్వచించు విలువలు_BUF_కొలత 2048
#నిర్వచించు విలువలు_LIST_కొలత 512
    
    అక్షరం       విలువలు_Buf[విలువలు_BUF_కొలత];
    int32_t    విలువలు_సూచీ = 0;
    స్థిరరాశి అక్షరం *విలువలు_List[విలువలు_LIST_కొలత];
    int32_t    విలువలు_లెక్క = 0;
    
    స్థిరరాశి అక్షరం *locale;
    int32_t     locLen;
    
    UEnumeration *locs = శూన్యసూచిక;
    
    UResourceBundle    అంశం;
    UResourceBundle    sub_అంశం;
    
    ures_init_దొంతర_Object(&అంశం);
    ures_init_దొంతర_Object(&sub_అంశం);
    locs = ures_open_అందుబాటు_Locales(మార్గం, స్థితి_2);
    
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        ures_close(&అంశం);
        ures_close(&sub_అంశం);
        తిరిగి శూన్యసూచిక;
    }
    
    విలువలు_Buf[0]=0;
    విలువలు_Buf[1]=0;

    అయితే ((locale = uenum_తదుపరి(locs, &locLen, స్థితి_2)) != శూన్యసూచిక) {
        UResourceBundle   *bund = శూన్యసూచిక;
        UResourceBundle   *subPtr = శూన్యసూచిక;
        UError_సంకేతం sub_స్థితి = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(మార్గం, locale, &sub_స్థితి);
        
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
        ఒకవేళ(!bund || U_FAILURE(sub_స్థితి)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                మార్గం?మార్గం:"<ICUDATA>", keyword, locale, u_errorName(sub_స్థితి));
        }
#అయితే_ముగింపు
        
        ures_get_By_కీలకం(bund, keyword, &అంశం, &sub_స్థితి);
        
        ఒకవేళ(!bund || U_FAILURE(sub_స్థితి)) {
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                మార్గం?మార్గం:"<ICUDATA>", keyword, locale, u_errorName(sub_స్థితి));
#అయితే_ముగింపు
            ures_close(bund);
            bund = శూన్యసూచిక;
            కొనసాగు;
        }

        అయితే ((subPtr = ures_get_తదుపరి_Resource(&అంశం, &sub_అంశం, &sub_స్థితి)) != శూన్యసూచిక
            && U_SUCCESS(sub_స్థితి)) {
            స్థిరరాశి అక్షరం *k;
            int32_t i;
            k = ures_get_కీలకం(subPtr);

#ఒకవేళ defined(URES_వృక్షం_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#అయితే_ముగింపు
            ఒకవేళ(k == శూన్యసూచిక || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_ట్యాగ్) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                కొనసాగు;
            }
            కొరకు(i=0; i<విలువలు_లెక్క; i++) {
                ఒకవేళ(!uprv_strcmp(విలువలు_List[i],k)) {
                    k = శూన్యసూచిక; /* found duplicate */
                    ఆపు;
                }
            }
            ఒకవేళ(k != శూన్యసూచిక) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                ఒకవేళ((విలువలు_లెక్క >= (విలువలు_LIST_కొలత-1)) ||       /* no more space in list .. */
                    ((విలువలు_సూచీ+kLen+1+1) >= విలువలు_BUF_కొలత)) { /* no more space in buffer (string + 2 nulls) */
                    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* out of space.. */
                } లేకపోతే {
                    uprv_strcpy(విలువలు_Buf+విలువలు_సూచీ, k);
                    విలువలు_List[విలువలు_లెక్క++] = విలువలు_Buf+విలువలు_సూచీ;
                    విలువలు_సూచీ += kLen;
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        మార్గం?మార్గం:"<ICUDATA>", keyword, locale, k);
#అయితే_ముగింపు
                    విలువలు_Buf[విలువలు_సూచీ++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    విలువలు_Buf[విలువలు_సూచీ++] = 0; /* terminate */
    
    ures_close(&అంశం);
    ures_close(&sub_అంశం);
    uenum_close(locs);
#ఒకవేళ defined(URES_వృక్షం_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*స్థితి_2), 
        విలువలు_సూచీ, విలువలు_లెక్క);
#అయితే_ముగింపు
    తిరిగి uloc_openKeywordList(విలువలు_Buf, విలువలు_సూచీ, స్థితి_2);
}
#ఒకవేళ 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(స్థిరరాశి UResourceBundle* res1, స్థిరరాశి UResourceBundle* res2){
    ఒకవేళ(res1==శూన్యసూచిక || res2==శూన్యసూచిక){
        తిరిగి res1==res2; /* pointer comparison */
    }
    ఒకవేళ(res1->f_కీలకం==శూన్యసూచిక||  res2->f_కీలకం==శూన్యసూచిక){
        తిరిగి (res1->f_కీలకం==res2->f_కీలకం);
    }లేకపోతే{
        ఒకవేళ(uprv_strcmp(res1->f_కీలకం, res2->f_కీలకం)!=0){
            తిరిగి అబద్ధం;
        }
    }
    ఒకవేళ(uprv_strcmp(res1->f_దత్తాంశం->fName, res2->f_దత్తాంశం->fName)!=0){
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(res1->f_దత్తాంశం->f_మార్గం == శూన్యసూచిక||  res2->f_దత్తాంశం->f_మార్గం==శూన్యసూచిక){
        తిరిగి (res1->f_దత్తాంశం->f_మార్గం == res2->f_దత్తాంశం->f_మార్గం);
    }లేకపోతే{
        ఒకవేళ(uprv_strcmp(res1->f_దత్తాంశం->f_మార్గం, res2->f_దత్తాంశం->f_మార్గం)!=0){
            తిరిగి అబద్ధం;
        }
    }
    ఒకవేళ(uprv_strcmp(res1->f_దత్తాంశం->f_మాతృ->fName, res2->f_దత్తాంశం->f_మాతృ->fName)!=0){
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(uprv_strcmp(res1->f_దత్తాంశం->f_మాతృ->f_మార్గం, res2->f_దత్తాంశం->f_మాతృ->f_మార్గం)!=0){
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(uprv_strncmp(res1->f_Res_మార్గం, res2->f_Res_మార్గం, res1->f_Res_మార్గం_Len)!=0){
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(res1->fRes != res2->fRes){
        తిరిగి అబద్ధం;
    }
    తిరిగి నిజం;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(స్థిరరాశి UResourceBundle* res, UError_సంకేతం* స్థితి_2){
    UResourceBundle* bundle = శూన్యసూచిక;
    UResourceBundle* ret = శూన్యసూచిక;
    ఒకవేళ(U_FAILURE(*స్థితి_2) || res == శూన్యసూచిక){
        తిరిగి శూన్యసూచిక;
    }
    bundle = ures_open(res->f_దత్తాంశం->f_మార్గం, res->f_దత్తాంశం->fName, స్థితి_2);
    ఒకవేళ(res->f_Res_మార్గం!=శూన్యసూచిక){
        ret = ures_కనుగొను_Sub_Resource(bundle, res->f_Res_మార్గం, శూన్యసూచిక, స్థితి_2);
        ures_close(bundle);
    }లేకపోతే{
        ret = bundle;
    }
    తిరిగి ret;
}
U_CAPI స్థిరరాశి UResourceBundle* U_EXPORT2
ures_get_మాతృ_Bundle(స్థిరరాశి UResourceBundle* res){
    ఒకవేళ(res==శూన్యసూచిక){
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి res->f_మాతృ_Res;
}
#అయితే_ముగింపు

U_CAPI శూన్య U_EXPORT2
ures_get_సంస్కరణ_By_కీలకం(స్థిరరాశి UResourceBundle* res, స్థిరరాశి అక్షరం *కీలకం, UVersionInfo ver, UError_సంకేతం *స్థితి_2) {
  స్థిరరాశి char16_t *str;
  int32_t len;
  str = ures_get_String_By_కీలకం(res, కీలకం, &len, స్థితి_2);
  ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
    u_సంస్కరణ_From_UString(ver, str);
  } 
}

/* eof */
