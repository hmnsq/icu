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

استعمال نطاق icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ساكن UHashtable *cache = خالي_اشارو;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ساكن int32_t U_CALLCONV hash_داخلا(ثابت UHashTok parm) {
    UResource_معلومات_داخلا* b = static_cast<UResource_معلومات_داخلا*>(parm.اشارو);
    UHashTok namekey, pathkey;
    namekey.اشارو = b->fName;
    pathkey.اشارو = b->f_رستو;
    أرجع uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ساكن UBool U_CALLCONV compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UResource_معلومات_داخلا* b1 = static_cast<UResource_معلومات_داخلا*>(p1.اشارو);
    UResource_معلومات_داخلا* b2 = static_cast<UResource_معلومات_داخلا*>(p2.اشارو);
    UHashTok name1, name2, path1, path2;
    name1.اشارو = b1->fName;
    name2.اشارو = b2->fName;
    path1.اشارو = b1->f_رستو;
    path2.اشارو = b2->f_رستو;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ساكن UBool chopLocale(حرف *name) {
    حرف *i = uprv_strrchr(name, '_');

    إذا(i != خالي_اشارو) {
        *i = '\0';
        أرجع سچ;
    }

    أرجع خطأ;
}

ساكن UBool hasVariant(ثابت حرف* localeID) {
    UError_ڪوڊ err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(خالي_اشارو, 0);
    ulocimp_getSubtags(
            localeID,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            &sink,
            خالي_اشارو,
            err);
    أرجع sink.عدد_جو_بائيٽون_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#وصف_ڪريو INCLUDED_FROM_URESBUND_CPP
#ضمن "localefallback_data.h"

ساكن ثابت حرف* performFallbackLookup(ثابت حرف* ڪنجي,
                                         ثابت حرف* ڪنجي_Strs,
                                         ثابت حرف* قدر_Strs U_LIFETIME_BOUND,
                                         ثابت int32_t* lookup_جدول,
                                         int32_t lookup_جدول_ڊيگهائي) {
    ثابت int32_t* bottom = lookup_جدول;
    ثابت int32_t* چوٽي = lookup_جدول + lookup_جدول_ڊيگهائي;

    طالما (bottom < چوٽي) {
        // Effectively, divide by 2 and round down to an even index
        ثابت int32_t* وچ = bottom + (((چوٽي - bottom) / 4) * 2);
        ثابت حرف* داخلا_ڪنجي = &(ڪنجي_Strs[*وچ]);
        int32_t strcmp_نتيجو = uprv_strcmp(ڪنجي, داخلا_ڪنجي);
        إذا (strcmp_نتيجو == 0) {
            أرجع &(قدر_Strs[وچ[1]]);
        } وإلا إذا (strcmp_نتيجو < 0) {
            چوٽي = وچ;
        } وإلا {
            bottom = وچ + 2;
        }
    }
    أرجع خالي_اشارو;
}

ساكن CharString getDefaultScript(ثابت CharString& language, ثابت CharString& region) {
    ثابت حرف* defaultScript = خالي_اشارو;
    UError_ڪوڊ err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString نتيجو("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    إذا (!region.is_خالي_جايون()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        إذا (U_FAILURE(err)) {
            أرجع نتيجو;
        }
        defaultScript = performFallbackLookup(localeID.معلومات(), dsLocaleIDChars, script_ڪوڊ_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    إذا (defaultScript == خالي_اشارو) {
        defaultScript = performFallbackLookup(language.معلومات(), dsLocaleIDChars, script_ڪوڊ_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    إذا (defaultScript != خالي_اشارو) {
        نتيجو.clear();
        نتيجو.append(defaultScript, err);
    }
    أرجع نتيجو;
}

تعداد UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_پاڙ,
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
    URES_OPEN_LOCALE_پاڙ,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_سڌو
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
ساكن منطقي get_والد_Locale_ID(حرف *name, ثابت حرف *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    إذا (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        أرجع chopLocale(name);
    }
    
    UError_ڪوڊ err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, خالي_اشارو, خالي_اشارو, err);

    إذا (U_FAILURE(err)) {
        // hopefully this never happens...
        أرجع chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    إذا (openType == URES_OPEN_LOCALE_DEFAULT_پاڙ) {
        ثابت حرف* والد_ID = performFallbackLookup(name, والد_Locale_Chars, والد_Locale_Chars, والد_Locale_جدول, UPRV_LENGTHOF(والد_Locale_جدول));
        إذا (والد_ID != خالي_اشارو) {
            uprv_strcpy(name, والد_ID);
            أرجع سچ;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    إذا (!script.is_خالي_جايون() && !region.is_خالي_جايون()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        إذا (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } وإلا إذا (!region.is_خالي_جايون()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_ڪوڊ err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, خالي_اشارو, خالي_اشارو, خالي_اشارو, err);
        إذا (!origNameScript.is_خالي_جايون()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } وإلا إذا (!script.is_خالي_جايون()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        إذا (openType != URES_OPEN_LOCALE_DEFAULT_پاڙ || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } وإلا {
            أرجع خطأ;
        }
    } وإلا {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        أرجع خطأ;
    }
    إذا (U_SUCCESS(err) && !workingLocale.is_خالي_جايون()) {
        uprv_strcpy(name, workingLocale.معلومات());
        أرجع سچ;
    } وإلا {
        أرجع خطأ;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ساكن UBool may_Have_والد(حرف *name) {
    أرجع (name[0] != 0 && uprv_strstr("nb nn",name) != خالي_اشارو);
}

/**
 *  Internal function
 */
ساكن فراغ داخلا_Increase(UResource_معلومات_داخلا *داخلا_2) {
    Mutex تالو(&resbMutex);
    داخلا_2->f_ڳڻپ_Existing++;
    طالما(داخلا_2->f_والد != خالي_اشارو) {
      داخلا_2 = داخلا_2->f_والد;
      داخلا_2->f_ڳڻپ_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ساكن UResource_معلومات_داخلا *get_Fallback_معلومات(
        ثابت UResourceBundle *resBundle,
        ثابت حرف **res_ٽيگ, Resource *res, UError_ڪوڊ *حالت) {
    UResource_معلومات_داخلا *معلومات_داخلا = resBundle->f_معلومات;
    int32_t اشاريو_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    إذا(معلومات_داخلا == خالي_اشارو) {
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع خالي_اشارو;
    }
    إذا(معلومات_داخلا->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_جدول_شي_By_ڪنجي(&(معلومات_داخلا->f_معلومات), معلومات_داخلا->f_معلومات.پاڙ_Res, &اشاريو_R, res_ٽيگ); /* try to get data from there */
        i++;
    }
    إذا(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        طالما(*res == RES_BOGUS && معلومات_داخلا->f_والد != خالي_اشارو) {
            معلومات_داخلا = معلومات_داخلا->f_والد;
            إذا(معلومات_داخلا->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_جدول_شي_By_ڪنجي(&(معلومات_داخلا->f_معلومات), معلومات_داخلا->f_معلومات.پاڙ_Res, &اشاريو_R, res_ٽيگ);
            }
        }
    }

    إذا(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع خالي_اشارو;
    }
    // If the resource is found in parents, we need to adjust the error.
    إذا(i>1) {
        إذا(uprv_strcmp(معلومات_داخلا->fName, uloc_getDefault())==0 || uprv_strcmp(معلومات_داخلا->fName, k_پاڙ_Locale_Name)==0) {
            *حالت = U_USING_DEFAULT_WARNING;
        } وإلا {
            *حالت = U_USING_FALLBACK_WARNING;
        }
    }
    أرجع معلومات_داخلا;
}

ساكن فراغ
free_داخلا(UResource_معلومات_داخلا *داخلا_2) {
    UResource_معلومات_داخلا *alias;
    res_unload(&(داخلا_2->f_معلومات));
    إذا(داخلا_2->fName != خالي_اشارو && داخلا_2->fName != داخلا_2->f_Name_بفر) {
        uprv_free(داخلا_2->fName);
    }
    إذا(داخلا_2->f_رستو != خالي_اشارو) {
        uprv_free(داخلا_2->f_رستو);
    }
    إذا(داخلا_2->fPool != خالي_اشارو) {
        --داخلا_2->fPool->f_ڳڻپ_Existing;
    }
    alias = داخلا_2->fAlias;
    إذا(alias != خالي_اشارو) {
        طالما(alias->fAlias != خالي_اشارو) {
            alias = alias->fAlias;
        }
        --alias->f_ڳڻپ_Existing;
    }
    uprv_free(داخلا_2);
}

/* Works just like ucnv_flushCache() */
ساكن int32_t ures_flushCache()
{
    UResource_معلومات_داخلا *resB;
    int32_t pos;
    int32_t rb_Deleted_انگ = 0;
    ثابت UHashElement *e;
    UBool deleted_وڌيڪ;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex تالو(&resbMutex);
    إذا (cache == خالي_اشارو) {
        أرجع 0;
    }

    افعل {
        deleted_وڌيڪ = خطأ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_پهريون;
        طالما ((e = uhash_ايندڙ_Element(cache, &pos)) != خالي_اشارو)
        {
            resB = static_cast<UResource_معلومات_داخلا*>(e->value.اشارو);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            إذا (resB->f_ڳڻپ_Existing == 0) {
                rb_Deleted_انگ++;
                deleted_وڌيڪ = سچ;
                uhash_removeElement(cache, e);
                free_داخلا(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } طالما(deleted_وڌيڪ);

    أرجع rb_Deleted_انگ;
}

#جيڪڏهن_وصف_ٿيل URES_DEBUG
#ضمن <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_خالي_جايون = خطأ;
  int32_t pos = UHASH_پهريون;
  ثابت UHashElement *e;
  UResource_معلومات_داخلا *resB;
  
    Mutex تالو(&resbMutex);
    إذا (cache == خالي_اشارو) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      أرجع خطأ;
    }

    طالما ((e = uhash_ايندڙ_Element(cache, &pos)) != خالي_اشارو) {
      cache_Not_خالي_جايون=سچ;
      resB = (UResource_معلومات_داخلا *) e->value.اشارو;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (فراغ*)resB, resB->f_ڳڻپ_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_رستو?resB->f_رستو:"nullptr",
              (فراغ*)resB->fPool,
              (فراغ*)resB->fAlias,
              (فراغ*)resB->f_والد);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_ڳڻپ(cache));
    أرجع cache_Not_خالي_جايون;
}

#جيڪڏهن_پورو

ساكن UBool U_CALLCONV ures_cleanup()
{
    إذا (cache != خالي_اشارو) {
        ures_flushCache();
        uhash_close(cache);
        cache = خالي_اشارو;
    }
    gCacheInitOnce.reset();
    أرجع سچ;
}

/** INTERNAL: Initializes the cache for resources */
ساكن فراغ U_CALLCONV createCache(UError_ڪوڊ &حالت) {
    U_ASSERT(cache == خالي_اشارو);
    cache = uhash_open(hash_داخلا, compareEntries, خالي_اشارو, &حالت);
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ساكن فراغ initCache(UError_ڪوڊ *حالت) {
    umtx_initOnce(gCacheInitOnce, &createCache, *حالت);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ساكن فراغ سيٽ_داخلا_Name(UResource_معلومات_داخلا *res, ثابت حرف *name, UError_ڪوڊ *حالت) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    إذا(res->fName != خالي_اشارو && res->fName != res->f_Name_بفر) {
        uprv_free(res->fName);
    }
    إذا (len < static_cast<int32_t>(حجم(res->f_Name_بفر))) {
        res->fName = res->f_Name_بفر;
    }
    وإلا {
        res->fName = static_cast<حرف*>(uprv_malloc(len + 1));
    }
    إذا(res->fName == خالي_اشارو) {
        *حالت = U_يادگيري_ورهاست_ERROR;
    } وإلا {
        uprv_strcpy(res->fName, name);
    }
}

ساكن UResource_معلومات_داخلا *
get_Pool_داخلا(ثابت حرف *رستو, UError_ڪوڊ *حالت);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ساكن UResource_معلومات_داخلا *init_داخلا(ثابت حرف *localeID, ثابت حرف *رستو, UError_ڪوڊ *حالت) {
    UResource_معلومات_داخلا *r = خالي_اشارو;
    UResource_معلومات_داخلا find;
    /*int32_t hashValue;*/
    ثابت حرف *name;
    حرف aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    إذا(U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    /* here we try to deduce the right locale name */
    إذا(localeID == خالي_اشارو) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } وإلا إذا(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_پاڙ_Locale_Name;
    } وإلا { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<حرف*>(name);
    find.f_رستو = const_cast<حرف*>(رستو);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_معلومات_داخلا*>(uhash_get(cache, &find));
    إذا(r == خالي_اشارو) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_معلومات_داخلا*>(uprv_malloc(حجم(UResource_معلومات_داخلا)));
        إذا(r == خالي_اشارو) {
            *حالت = U_يادگيري_ورهاست_ERROR;
            أرجع خالي_اشارو;
        }

        uprv_memset(r, 0, حجم(UResource_معلومات_داخلا));
        /*r->fHashKey = hashValue;*/

        سيٽ_داخلا_Name(r, name, حالت);
        إذا (U_FAILURE(*حالت)) {
            uprv_free(r);
            أرجع خالي_اشارو;
        }

        إذا(رستو != خالي_اشارو) {
            r->f_رستو = uprv_strdup(رستو);
            إذا(r->f_رستو == خالي_اشارو) {
                *حالت = U_يادگيري_ورهاست_ERROR;
                uprv_free(r);
                أرجع خالي_اشارو;
            }
        }

        /* this is the actual loading */
        res_ڀريو(&(r->f_معلومات), r->f_رستو, r->fName, حالت);

        إذا (U_FAILURE(*حالت)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            إذا (*حالت == U_يادگيري_ورهاست_ERROR) {
                uprv_free(r);
                أرجع خالي_اشارو;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *حالت = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } وإلا { /* if we have a regular entry */
            Resource aliasres;
            إذا (r->f_معلومات.استعمال_ڪري_Pool_Bundle) {
                r->fPool = get_Pool_داخلا(r->f_رستو, حالت);
                إذا (U_SUCCESS(*حالت)) {
                    ثابت int32_t *poolIndexes = r->fPool->f_معلومات.p_پاڙ + 1;
                    إذا(r->f_معلومات.p_پاڙ[1 + URES_اشاريو_POOL_CHECKSUM] == poolIndexes[URES_اشاريو_POOL_CHECKSUM]) {
                        r->f_معلومات.pool_Bundle_ڪنجيون = reinterpret_cast<ثابت حرف*>(poolIndexes + (poolIndexes[URES_اشاريو_ڊيگهائي] & 0xff));
                        r->f_معلومات.pool_Bundle_لڙيون = r->fPool->f_معلومات.p16_ٻٽو_انگ_Units;
                    } وإلا {
                        r->fBogus = *حالت = U_INVALID_FORMAT_ERROR;
                    }
                } وإلا {
                    r->fBogus = *حالت;
                }
            }
            إذا (U_SUCCESS(*حالت)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_معلومات), "%%ALIAS");
                إذا (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ثابت char16_t *alias = res_getStringNoTrace(&(r->f_معلومات), aliasres, &aliasLen);
                    إذا(alias != خالي_اشارو && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_منزل_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_داخلا(aliasName, رستو, حالت);
                    }
                }
            }
        }

        {
            UResource_معلومات_داخلا *oldR = خالي_اشارو;
            إذا ((oldR = static_cast<UResource_معلومات_داخلا*>(uhash_get(cache, r))) == خالي_اشارو) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_ڪوڊ cache_حالت = U_ZERO_ERROR;
                uhash_put(cache, (فراغ *)r, r, &cache_حالت);
                إذا (U_FAILURE(cache_حالت)) {
                    *حالت = cache_حالت;
                    free_داخلا(r);
                    r = خالي_اشارو;
                }
            } وإلا {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_داخلا(r);
                r = oldR;
            }
        }

    }
    إذا(r != خالي_اشارو) {
        /* return the real bundle */
        طالما(r->fAlias != خالي_اشارو) {
            r = r->fAlias;
        }
        r->f_ڳڻپ_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        إذا(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*حالت)) {
             *حالت = r->fBogus; /* set the returning status */
        }
    }
    أرجع r;
}

ساكن UResource_معلومات_داخلا *
get_Pool_داخلا(ثابت حرف *رستو, UError_ڪوڊ *حالت) {
    UResource_معلومات_داخلا *poolBundle = init_داخلا(kPoolBundleName, رستو, حالت);
    إذا( U_SUCCESS(*حالت) &&
        (poolBundle == خالي_اشارو || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_معلومات.isPoolBundle)
    ) {
        *حالت = U_INVALID_FORMAT_ERROR;
    }
    أرجع poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ساكن UResource_معلومات_داخلا *
ڳوليو_پهريون_Existing(ثابت حرف* رستو, حرف* name, ثابت حرف* defaultLocale, UResOpenType openType,
                  UBool *is_پاڙ, UBool *مليو_والد, UBool *isDefault, UError_ڪوڊ* حالت) {
    UResource_معلومات_داخلا *r = خالي_اشارو;
    UBool has_Real_معلومات = خطأ;
    *مليو_والد = سچ; /* we're starting with a fresh name */
    حرف origName[ULOC_FULLNAME_صلاحيت];

    uprv_strcpy(origName, name);
    طالما(*مليو_والد && !has_Real_معلومات) {
        r = init_داخلا(name, رستو, حالت);
        /* Null pointer test */
        إذا (U_FAILURE(*حالت)) {
            أرجع خالي_اشارو;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_معلومات = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        إذا(!has_Real_معلومات) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_ڳڻپ_Existing--;
            /*entryCloseInt(r);*/
            r = خالي_اشارو;
            *حالت = U_USING_FALLBACK_WARNING;
        } وإلا {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_پاڙ = static_cast<UBool>(uprv_strcmp(name, k_پاڙ_Locale_Name) == 0);

        /*Fallback data stuff*/
        إذا (!has_Real_معلومات) {
            *مليو_والد = get_والد_Locale_ID(name, origName, openType);
        } وإلا {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *مليو_والد = chopLocale(name);
        }
        إذا (*مليو_والد && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    أرجع r;
}

ساكن فراغ ures_سيٽ_Is_ڍير_Object( UResourceBundle* resB, UBool state) {
    إذا(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } وإلا {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ساكن UBool ures_is_ڍير_Object(ثابت UResourceBundle* resB) {
  أرجع((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?خطأ:سچ);
}


U_CFUNC فراغ ures_init_ڍير_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, حجم(UResourceBundle));
  ures_سيٽ_Is_ڍير_Object(resB, سچ);
}

U_NAMESPACE_BEGIN

ڍير_UResource_Bundle::ڍير_UResource_Bundle() {
    ures_init_ڍير_Object(&bundle);
}

ڍير_UResource_Bundle::~ڍير_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_پڇاڙي

ساكن UBool  // returns U_SUCCESS(*status)
ڀريو_Parents_Except_پاڙ(UResource_معلومات_داخلا *&t1,
                      حرف name[], int32_t name_صلاحيت,
                      UBool usingUSRData, حرف usr_معلومات_رستو[], UError_ڪوڊ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع خطأ; }
    UBool check_والد = سچ;
    طالما (check_والد && t1->f_والد == خالي_اشارو && !t1->f_معلومات.noFallback &&
            res_getResource(&t1->f_معلومات,"%%ParentIsRoot") == RES_BOGUS) {
        Resource والد_Res = res_getResource(&t1->f_معلومات, "%%Parent");
        إذا (والد_Res != RES_BOGUS) {  // An explicit parent was found.
            int32_t والد_Locale_Len = 0;
            // No tracing: called during initial data loading
            ثابت char16_t *والد_Locale_Name = res_getStringNoTrace(&(t1->f_معلومات), والد_Res, &والد_Locale_Len);
            إذا(والد_Locale_Name != خالي_اشارو && 0 < والد_Locale_Len && والد_Locale_Len < name_صلاحيت) {
                u_UChars_منزل_Chars(والد_Locale_Name, name, والد_Locale_Len + 1);
                إذا (uprv_strcmp(name, k_پاڙ_Locale_Name) == 0) {
                    أرجع سچ;
                }
            }
        }
        // Insert regular parents.
        UError_ڪوڊ والد_حالت = U_ZERO_ERROR;
        UResource_معلومات_داخلا *t2 = init_داخلا(name, t1->f_رستو, &والد_حالت);
        إذا (U_FAILURE(والد_حالت)) {
            *حالت = والد_حالت;
            أرجع خطأ;
        }
        UResource_معلومات_داخلا *u2 = خالي_اشارو;
        UError_ڪوڊ usr_حالت = U_ZERO_ERROR;
        إذا (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_داخلا(name, usr_معلومات_رستو, &usr_حالت);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            إذا (usr_حالت == U_يادگيري_ورهاست_ERROR) {
                *حالت = usr_حالت;
                أرجع خطأ;
            }
        }

        إذا (usingUSRData && U_SUCCESS(usr_حالت) && u2->fBogus == U_ZERO_ERROR) {
            t1->f_والد = u2;
            u2->f_والد = t2;
        } وإلا {
            t1->f_والد = t2;
            إذا (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_ڳڻپ_Existing = 0;
            }
        }
        t1 = t2;
        check_والد = chopLocale(name) || may_Have_والد(name);
    }
    أرجع سچ;
}

ساكن UBool  // returns U_SUCCESS(*status)
وجهو_پاڙ_Bundle(UResource_معلومات_داخلا *&t1, UError_ڪوڊ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع خطأ; }
    UError_ڪوڊ والد_حالت = U_ZERO_ERROR;
    UResource_معلومات_داخلا *t2 = init_داخلا(k_پاڙ_Locale_Name, t1->f_رستو, &والد_حالت);
    إذا (U_FAILURE(والد_حالت)) {
        *حالت = والد_حالت;
        أرجع خطأ;
    }
    t1->f_والد = t2;
    t1 = t2;
    أرجع سچ;
}

ساكن UResource_معلومات_داخلا *داخلا_Open(ثابت حرف* رستو, ثابت حرف* localeID,
                                     UResOpenType openType, UError_ڪوڊ* حالت) {
    U_ASSERT(openType != URES_OPEN_سڌو);
    UError_ڪوڊ int_حالت = U_ZERO_ERROR;
    UResource_معلومات_داخلا *r = خالي_اشارو;
    UResource_معلومات_داخلا *t1 = خالي_اشارو;
    UBool isDefault = خطأ;
    UBool is_پاڙ = خطأ;
    UBool has_Real_معلومات = خطأ;
    UBool hasChopped = سچ;
    UBool usingUSRData = U_USE_USRDATA && ( رستو == خالي_اشارو || uprv_strncmp(رستو,U_ICUDATA_NAME,8) == 0);

    حرف name[ULOC_FULLNAME_صلاحيت];
    حرف usr_معلومات_رستو[96];

    initCache(حالت);

    إذا(U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    uprv_strncpy(name, localeID, حجم(name) - 1);
    name[حجم(name) - 1] = 0;

    إذا ( usingUSRData ) {
        إذا ( رستو == خالي_اشارو ) {
            uprv_strcpy(usr_معلومات_رستو, U_USRDATA_NAME);
        } وإلا {
            uprv_strncpy(usr_معلومات_رستو, رستو, حجم(usr_معلومات_رستو) - 1);
            usr_معلومات_رستو[0] = 'u';
            usr_معلومات_رستو[1] = 's';
            usr_معلومات_رستو[2] = 'r';
            usr_معلومات_رستو[حجم(usr_معلومات_رستو) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ثابت حرف *defaultLocale = uloc_getDefault();

    Mutex تالو(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = ڳوليو_پهريون_Existing(رستو, name, defaultLocale, openType, &is_پاڙ, &hasChopped, &isDefault, &int_حالت);

    // If we failed due to out-of-memory, report the failure and exit early.
    إذا (int_حالت == U_يادگيري_ورهاست_ERROR) {
        *حالت = int_حالت;
        اذهب پڄاڻي;
    }

    إذا(r != خالي_اشارو) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_معلومات = سچ;
        إذا ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_ڪوڊ usr_حالت = U_ZERO_ERROR;
            UResource_معلومات_داخلا *u1 = init_داخلا(t1->fName, usr_معلومات_رستو, &usr_حالت);
            // If we failed due to out-of-memory, report the failure and exit early.
            إذا (int_حالت == U_يادگيري_ورهاست_ERROR) {
                *حالت = int_حالت;
                اذهب پڄاڻي;
            }
            إذا ( u1 != خالي_اشارو ) {
                إذا(u1->fBogus == U_ZERO_ERROR) {
                    u1->f_والد = t1;
                    r = u1;
                } وإلا {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_ڳڻپ_Existing = 0;
                }
            }
        }
        إذا ((hasChopped || may_Have_والد(name)) && !is_پاڙ) {
            إذا (!ڀريو_Parents_Except_پاڙ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_معلومات_رستو, حالت)) {
                اذهب پڄاڻي;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    إذا(r==خالي_اشارو && openType == URES_OPEN_LOCALE_DEFAULT_پاڙ && !isDefault && !is_پاڙ) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = ڳوليو_پهريون_Existing(رستو, name, defaultLocale, openType, &is_پاڙ, &hasChopped, &isDefault, &int_حالت);
        // If we failed due to out-of-memory, report the failure and exit early.
        إذا (int_حالت == U_يادگيري_ورهاست_ERROR) {
            *حالت = int_حالت;
            اذهب پڄاڻي;
        }
        int_حالت = U_USING_DEFAULT_WARNING;
        إذا(r != خالي_اشارو) { /* the default locale exists */
            t1 = r;
            has_Real_معلومات = سچ;
            isDefault = سچ;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            إذا ((hasChopped || may_Have_والد(name)) && !is_پاڙ) {
                إذا (!ڀريو_Parents_Except_پاڙ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_معلومات_رستو, حالت)) {
                    اذهب پڄاڻي;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    إذا(r == خالي_اشارو) {
        uprv_strcpy(name, k_پاڙ_Locale_Name);
        r = ڳوليو_پهريون_Existing(رستو, name, defaultLocale, openType, &is_پاڙ, &hasChopped, &isDefault, &int_حالت);
        // If we failed due to out-of-memory, report the failure and exit early.
        إذا (int_حالت == U_يادگيري_ورهاست_ERROR) {
            *حالت = int_حالت;
            اذهب پڄاڻي;
        }
        إذا(r != خالي_اشارو) {
            t1 = r;
            int_حالت = U_USING_DEFAULT_WARNING;
            has_Real_معلومات = سچ;
        } وإلا { /* we don't even have the root locale */
            *حالت = U_MISSING_RESOURCE_ERROR;
            اذهب پڄاڻي;
        }
    } وإلا إذا(!is_پاڙ && uprv_strcmp(t1->fName, k_پاڙ_Locale_Name) != 0 &&
            t1->f_والد == خالي_اشارو && !r->f_معلومات.noFallback) {
        إذا (!وجهو_پاڙ_Bundle(t1, حالت)) {
            اذهب پڄاڻي;
        }
        إذا(!has_Real_معلومات) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    طالما(r != خالي_اشارو && !is_پاڙ && t1->f_والد != خالي_اشارو) {
        t1->f_والد->f_ڳڻپ_Existing++;
        t1 = t1->f_والد;
    }

پڄاڻي:
    إذا(U_SUCCESS(*حالت)) {
        إذا(int_حالت != U_ZERO_ERROR) {
            *حالت = int_حالت;  
        }
        أرجع r;
    } وإلا {
        أرجع خالي_اشارو;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ساكن UResource_معلومات_داخلا *
داخلا_Open_سڌو(ثابت حرف* رستو, ثابت حرف* localeID, UError_ڪوڊ* حالت) {
    initCache(حالت);
    إذا(U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    إذا (localeID == خالي_اشارو) {
        localeID = uloc_getDefault();
    } وإلا إذا (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_پاڙ_Locale_Name;
    }

    Mutex تالو(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_معلومات_داخلا *r = init_داخلا(localeID, رستو, حالت);
    إذا(U_SUCCESS(*حالت)) {
        إذا(r->fBogus != U_ZERO_ERROR) {
            r->f_ڳڻپ_Existing--;
            r = خالي_اشارو;
        }
    } وإلا {
        r = خالي_اشارو;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_معلومات_داخلا *t1 = r;
    إذا(r != خالي_اشارو && uprv_strcmp(localeID, k_پاڙ_Locale_Name) != 0 &&  // not root
            r->f_والد == خالي_اشارو && !r->f_معلومات.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_صلاحيت) {
        حرف name[ULOC_FULLNAME_صلاحيت];
        uprv_strcpy(name, localeID);
        إذا(!chopLocale(name) || uprv_strcmp(name, k_پاڙ_Locale_Name) == 0 ||
                ڀريو_Parents_Except_پاڙ(t1, name, UPRV_LENGTHOF(name), خطأ, خالي_اشارو, حالت)) {
            إذا(uprv_strcmp(t1->fName, k_پاڙ_Locale_Name) != 0 && t1->f_والد == خالي_اشارو) {
                وجهو_پاڙ_Bundle(t1, حالت);
            }
        }
        إذا(U_FAILURE(*حالت)) {
            r = خالي_اشارو;
        }
    }

    إذا(r != خالي_اشارو) {
        // TODO: Does this ever loop?
        طالما(t1->f_والد != خالي_اشارو) {
            t1->f_والد->f_ڳڻپ_Existing++;
            t1 = t1->f_والد;
        }
    }
    أرجع r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ساكن فراغ داخلا_Close_Int(UResource_معلومات_داخلا *resB) {
    UResource_معلومات_داخلا *p = resB;

    طالما(resB != خالي_اشارو) {
        p = resB->f_والد;
        resB->f_ڳڻپ_Existing--;

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

ساكن فراغ داخلا_Close(UResource_معلومات_داخلا *resB) {
  Mutex تالو(&resbMutex);
  داخلا_Close_Int(resB);
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
ساكن فراغ ures_append_Res_رستو(UResourceBundle *resB, ثابت حرف* منزل_جوڙيو, int32_t len_منزل_جوڙيو, UError_ڪوڊ *حالت) {
    int32_t res_رستو_Len_Orig = resB->f_Res_رستو_Len;
    إذا(resB->f_Res_رستو == خالي_اشارو) {
        resB->f_Res_رستو = resB->fResBuf;
        *(resB->f_Res_رستو) = 0;
        resB->f_Res_رستو_Len = 0;
    } 
    resB->f_Res_رستو_Len += len_منزل_جوڙيو;
    إذا(RES_BUFSIZE <= resB->f_Res_رستو_Len+1) {
        إذا(resB->f_Res_رستو == resB->fResBuf) {
            resB->f_Res_رستو = static_cast<حرف*>(uprv_malloc((resB->f_Res_رستو_Len + 1) * حجم(حرف)));
            /* Check that memory was allocated correctly. */
            إذا (resB->f_Res_رستو == خالي_اشارو) {
                *حالت = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            uprv_strcpy(resB->f_Res_رستو, resB->fResBuf);
        } وإلا {
            حرف* عارضي = static_cast<حرف*>(uprv_realloc(resB->f_Res_رستو, (resB->f_Res_رستو_Len + 1) * حجم(حرف)));
            /* Check that memory was reallocated correctly. */
            إذا (عارضي == خالي_اشارو) {
                *حالت = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            resB->f_Res_رستو = عارضي;
        }
    }
    uprv_strcpy(resB->f_Res_رستو + res_رستو_Len_Orig, منزل_جوڙيو);
}

ساكن فراغ ures_free_Res_رستو(UResourceBundle *resB) {
    إذا (resB->f_Res_رستو && resB->f_Res_رستو != resB->fResBuf) {
        uprv_free(resB->f_Res_رستو);
    }
    resB->f_Res_رستو = خالي_اشارو;
    resB->f_Res_رستو_Len = 0;
}

ساكن فراغ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    إذا(resB != خالي_اشارو) {
        إذا(resB->f_معلومات != خالي_اشارو) {
            داخلا_Close(resB->f_معلومات);
        }
        إذا(resB->f_نسخو != خالي_اشارو) {
            uprv_free(resB->f_نسخو);
        }
        ures_free_Res_رستو(resB);

        إذا(ures_is_ڍير_Object(resB) == خطأ && freeBundleObj) {
            uprv_free(resB);
        }
#إذا 0 /*U_DEBUG*/
        وإلا {
            /* poison the data */
            uprv_memset(resB, -1, حجم(UResourceBundle));
        }
#جيڪڏهن_پورو
    }
}

U_CAPI فراغ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, سچ);
}

نطاق {

UResourceBundle* init_resb_نتيجو(UResource_معلومات_داخلا* معلومات_داخلا,
                                  Resource r,
                                  ثابت حرف* ڪنجي,
                                  int32_t idx,
                                  UResource_معلومات_داخلا* valid_Locale_معلومات_داخلا,
                                  ثابت حرف* container_Res_رستو,
                                  int32_t recursion_اونهائي,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ڪوڊ* حالت);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
get_Alias_هدف_As_Resource_Bundle(ثابت Resource_معلومات& res_معلومات,
                               Resource r,
                               ثابت حرف* ڪنجي,
                               int32_t idx,
                               UResource_معلومات_داخلا* valid_Locale_معلومات_داخلا,
                               ثابت حرف* container_Res_رستو,
                               int32_t recursion_اونهائي,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_ڪوڊ* حالت) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    إذا (U_FAILURE(*حالت)) { أرجع resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ثابت char16_t *alias = res_getAlias(&res_معلومات, r, &len);
    إذا(len <= 0) {
        // bad alias
        *حالت = U_ILLEGAL_دليل_ERROR;
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
    chAlias.appendInvariantChars(alias, len, *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    // We have an alias, now let's cut it up.
    ثابت حرف *رستو = خالي_اشارو, *locale = خالي_اشارو, *ڪنجي_رستو = خالي_اشارو;
    إذا(chAlias[0] == RES_رستو_SEPARATOR) {
        // There is a path included.
        حرف *ch_Alias_معلومات = chAlias.معلومات();
        حرف *sep = ch_Alias_معلومات + 1;
        رستو = sep;
        sep = uprv_strchr(sep, RES_رستو_SEPARATOR);
        إذا(sep != خالي_اشارو) {
            *sep++ = 0;
        }
        إذا(uprv_strcmp(رستو, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            ڪنجي_رستو = sep;
            // Read from the valid locale which we already have.
            رستو = locale = خالي_اشارو;
        } وإلا {
            إذا(uprv_strcmp(رستو, "ICUDATA") == 0) { /* want ICU data */
                رستو = خالي_اشارو;
            }
            إذا (sep == خالي_اشارو) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } وإلا {
                locale = sep;
                sep = uprv_strchr(sep, RES_رستو_SEPARATOR);
                إذا(sep != خالي_اشارو) {
                    *sep++ = 0;
                }
                ڪنجي_رستو = sep;
            }
        }
    } وإلا {
        // No path, start with a locale.
        حرف *sep = chAlias.معلومات();
        locale = sep;
        sep = uprv_strchr(sep, RES_رستو_SEPARATOR);
        إذا(sep != خالي_اشارو) {
            *sep++ = 0;
        }
        ڪنجي_رستو = sep;
        رستو = valid_Locale_معلومات_داخلا->f_رستو;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    Local_UResource_Bundle_اشارو mainRes;
    UResource_معلومات_داخلا *معلومات_داخلا;
    إذا (locale == خالي_اشارو) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        معلومات_داخلا = valid_Locale_معلومات_داخلا;
    } وإلا {
        UError_ڪوڊ int_حالت = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_سڌو(رستو, locale, &int_حالت));
        إذا(U_FAILURE(int_حالت)) {
            // We failed to open the resource bundle we're aliasing to.
            *حالت = int_حالت;
            أرجع resB;
        }
        معلومات_داخلا = mainRes->f_معلومات;
    }

    ثابت حرف* عارضي = خالي_اشارو;
    إذا(ڪنجي_رستو == خالي_اشارو) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = معلومات_داخلا->f_معلومات.پاڙ_Res;
        إذا(container_Res_رستو) {
            chAlias.clear().append(container_Res_رستو, *حالت);
            إذا (U_FAILURE(*حالت)) {
                أرجع خالي_اشارو;
            }
            حرف *a_ڪنجي = chAlias.معلومات();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_ڳوليو_Resource(&معلومات_داخلا->f_معلومات, r, &a_ڪنجي, &عارضي);
        }
        إذا(ڪنجي) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(ڪنجي, *حالت);
            إذا (U_FAILURE(*حالت)) {
                أرجع خالي_اشارو;
            }
            حرف *a_ڪنجي = chAlias.معلومات();
            r = res_ڳوليو_Resource(&معلومات_داخلا->f_معلومات, r, &a_ڪنجي, &عارضي);
        } وإلا إذا(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            إذا(URES_IS_جدول(type)) {
                ثابت حرف *a_ڪنجي;
                r = res_get_جدول_شي_By_اشاريو(&معلومات_داخلا->f_معلومات, r, idx, &a_ڪنجي);
            } وإلا { /* array */
                r = res_get_Array_شي(&معلومات_داخلا->f_معلومات, r, idx);
            }
        }
        إذا(r != RES_BOGUS) {
            resB = init_resb_نتيجو(
                معلومات_داخلا, r, عارضي, -1, valid_Locale_معلومات_داخلا, خالي_اشارو, recursion_اونهائي+1,
                resB, حالت);
        } وإلا {
            *حالت = U_MISSING_RESOURCE_ERROR;
        }
    } وإلا {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString رستو_Buf(ڪنجي_رستو, *حالت);
        إذا (U_FAILURE(*حالت)) {
            أرجع خالي_اشارو;
        }
        حرف *my_رستو = رستو_Buf.معلومات();
        container_Res_رستو = خالي_اشارو;
        // Now we have fallback following here.
        لكل(;;) {
            r = معلومات_داخلا->f_معلومات.پاڙ_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            طالما(*my_رستو && U_SUCCESS(*حالت)) {
                r = res_ڳوليو_Resource(&(معلومات_داخلا->f_معلومات), r, &my_رستو, &عارضي);
                إذا(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    اكسر;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_نتيجو(
                    معلومات_داخلا, r, عارضي, -1,
                    valid_Locale_معلومات_داخلا, container_Res_رستو, recursion_اونهائي+1,
                    resB, حالت);
                إذا (U_FAILURE(*حالت)) {
                    اكسر;
                }
                إذا (عارضي == خالي_اشارو || uprv_strcmp(ڪنجي_رستو, عارضي) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_رستو(resB);
                    ures_append_Res_رستو(resB, ڪنجي_رستو, static_cast<int32_t>(uprv_strlen(ڪنجي_رستو)), حالت);
                    إذا(resB->f_Res_رستو[resB->f_Res_رستو_Len-1] != RES_رستو_SEPARATOR) {
                        ures_append_Res_رستو(resB, RES_رستو_SEPARATOR_S, 1, حالت);
                    }
                    إذا (U_FAILURE(*حالت)) {
                        اكسر;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                معلومات_داخلا = resB->f_معلومات;
                container_Res_رستو = resB->f_Res_رستو;
            }
            إذا (U_FAILURE(*حالت) || r != RES_BOGUS) {
                اكسر;
            }
            // Fall back to the parent bundle, if there is one.
            معلومات_داخلا = معلومات_داخلا->f_والد;
            إذا (معلومات_داخلا == خالي_اشارو) {
                *حالت = U_MISSING_RESOURCE_ERROR;
                اكسر;
            }
            // Copy the same keyPath again.
            my_رستو = رستو_Buf.معلومات();
            uprv_strcpy(my_رستو, ڪنجي_رستو);
        }
    }
    إذا(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    أرجع resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_نتيجو(UResource_معلومات_داخلا* معلومات_داخلا,
                                  Resource r,
                                  ثابت حرف* ڪنجي,
                                  int32_t idx,
                                  UResource_معلومات_داخلا* valid_Locale_معلومات_داخلا,
                                  ثابت حرف* container_Res_رستو,
                                  int32_t recursion_اونهائي,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ڪوڊ* حالت) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    إذا(حالت == خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع resB;
    }
    إذا (valid_Locale_معلومات_داخلا == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    إذا(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        إذا(recursion_اونهائي >= URES_MAX_ALIAS_LEVEL) {
            *حالت = U_TOO_MANY_ALIASES_ERROR;
            أرجع resB;
        }
        أرجع get_Alias_هدف_As_Resource_Bundle(
            معلومات_داخلا->f_معلومات, r, ڪنجي, idx,
            valid_Locale_معلومات_داخلا, container_Res_رستو, recursion_اونهائي, resB, حالت);
    }
    إذا(resB == خالي_اشارو) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        إذا (resB == خالي_اشارو) {
            *حالت = U_يادگيري_ورهاست_ERROR;
            أرجع خالي_اشارو;
        }
        ures_سيٽ_Is_ڍير_Object(resB, خطأ);
        resB->f_Res_رستو = خالي_اشارو;
        resB->f_Res_رستو_Len = 0;
    } وإلا {
        إذا(resB->f_معلومات != خالي_اشارو) {
            داخلا_Close(resB->f_معلومات);
        }
        إذا(resB->f_نسخو != خالي_اشارو) {
            uprv_free(resB->f_نسخو);
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
        إذا(container_Res_رستو != resB->f_Res_رستو) {
            ures_free_Res_رستو(resB);
        }
    }
    resB->f_معلومات = معلومات_داخلا;
    داخلا_Increase(resB->f_معلومات);
    resB->fHasFallback = خطأ;
    resB->f_Is_چوٽي_Level = خطأ;
    resB->f_اشاريو = -1;
    resB->f_ڪنجي = ڪنجي; 
    resB->f_Valid_Locale_معلومات_داخلا = valid_Locale_معلومات_داخلا;
    إذا(container_Res_رستو != resB->f_Res_رستو) {
        ures_append_Res_رستو(
            resB, container_Res_رستو, static_cast<int32_t>(uprv_strlen(container_Res_رستو)), حالت);
    }
    إذا(ڪنجي != خالي_اشارو) {
        ures_append_Res_رستو(resB, ڪنجي, static_cast<int32_t>(uprv_strlen(ڪنجي)), حالت);
        إذا(resB->f_Res_رستو_Len > 0 && resB->f_Res_رستو[resB->f_Res_رستو_Len-1] != RES_رستو_SEPARATOR) {
            ures_append_Res_رستو(resB, RES_رستو_SEPARATOR_S, 1, حالت);
        }
    } وإلا إذا(idx >= 0) {
        حرف buf[256];
        int32_t len = T_CString_integer_منزل_String(buf, idx, 10);
        ures_append_Res_رستو(resB, buf, len, حالت);
        إذا(resB->f_Res_رستو_Len > 0 && resB->f_Res_رستو[resB->f_Res_رستو_Len-1] != RES_رستو_SEPARATOR) {
            ures_append_Res_رستو(resB, RES_رستو_SEPARATOR_S, 1, حالت);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t استعمال_ٿيل_Len = ((resB->fResBuf == resB->f_Res_رستو) ? resB->f_Res_رستو_Len : 0);
        uprv_memset(resB->fResBuf + استعمال_ٿيل_Len, 0, حجم(resB->fResBuf) - استعمال_ٿيل_Len);
    }

    resB->f_نسخو = خالي_اشارو;
    resB->fRes = r;
    resB->f_سائيز = res_ڳڻپ_Array_شيون(&resB->get_Res_معلومات(), resB->fRes);
    ResourceTracer(resB).trace("get");
    أرجع resB;
}

UResourceBundle* init_resb_نتيجو(UResource_معلومات_داخلا* معلومات_داخلا,
                                  Resource r,
                                  ثابت حرف* ڪنجي,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ثابت UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_ڪوڊ* حالت) {
    أرجع init_resb_نتيجو(
        معلومات_داخلا, r, ڪنجي, idx,
        container->f_Valid_Locale_معلومات_داخلا, container->f_Res_رستو, 0, resB, حالت);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ثابت UResourceBundle* original,
                               UError_ڪوڊ* حالت) {
    UBool is_ڍير_Object;
    إذا(U_FAILURE(*حالت) || r == original) {
        أرجع r;
    }
    إذا(original != خالي_اشارو) {
        إذا(r == خالي_اشارو) {
            is_ڍير_Object = خطأ;
            r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
            /* test for nullptr */
            إذا (r == خالي_اشارو) {
                *حالت = U_يادگيري_ورهاست_ERROR;
                أرجع خالي_اشارو;
            }
        } وإلا {
            is_ڍير_Object = ures_is_ڍير_Object(r);
            ures_closeBundle(r, خطأ);
        }
        uprv_memcpy(r, original, حجم(UResourceBundle));
        r->f_Res_رستو = خالي_اشارو;
        r->f_Res_رستو_Len = 0;
        إذا(original->f_Res_رستو) {
            ures_append_Res_رستو(r, original->f_Res_رستو, original->f_Res_رستو_Len, حالت);
        }
        ures_سيٽ_Is_ڍير_Object(r, is_ڍير_Object);
        إذا(r->f_معلومات != خالي_اشارو) {
            داخلا_Increase(r->f_معلومات);
        }
    }
    أرجع r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ثابت char16_t* U_EXPORT2 ures_getString(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_ڪوڊ* حالت) {
    ثابت char16_t *s;
    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا(resB == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    s = res_getString({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
    إذا (s == خالي_اشارو) {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ساكن ثابت حرف*
ures_منزل_UTF8_String(ثابت char16_t* s16,
                  int32_t length16,
                  حرف* dest U_LIFETIME_BOUND,
                  int32_t* p_ڊيگهائي,
                  UBool forceCopy,
                  UError_ڪوڊ* حالت) {
    int32_t capacity;

    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (p_ڊيگهائي != خالي_اشارو) {
        capacity = *p_ڊيگهائي;
    } وإلا {
        capacity = 0;
    }
    إذا (capacity < 0 || (capacity > 0 && dest == خالي_اشارو)) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    إذا (length16 == 0) {
        /* empty string, return as read-only pointer */
        إذا (p_ڊيگهائي != خالي_اشارو) {
            *p_ڊيگهائي = 0;
        }
        إذا (forceCopy) {
            u_terminateChars(dest, capacity, 0, حالت);
            أرجع dest;
        } وإلا {
            أرجع "";
        }
    } وإلا {
        /* We need to transform the string to the destination buffer. */
        إذا (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            أرجع u_str_منزل_UTF8(خالي_اشارو, 0, p_ڊيگهائي, s16, length16, حالت);
        }
        إذا (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            int32_t max_ڊيگهائي_2 = 3 * length16 + 1;
            إذا (capacity > max_ڊيگهائي_2) {
                dest += capacity - max_ڊيگهائي_2;
                capacity = max_ڊيگهائي_2;
            }
        }
        أرجع u_str_منزل_UTF8(dest, capacity, p_ڊيگهائي, s16, length16, حالت);
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getUTF8String(ثابت UResourceBundle* resB,
                   حرف* dest U_LIFETIME_BOUND,
                   int32_t* p_ڊيگهائي,
                   UBool forceCopy,
                   UError_ڪوڊ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getString(resB, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_ڊيگهائي, forceCopy, حالت);
}

U_CAPI ثابت uint8_t* U_EXPORT2
ures_get_ٻٽو(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_ڪوڊ* حالت) {
  ثابت uint8_t *p;
  إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  إذا(resB == خالي_اشارو) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع خالي_اشارو;
  }
  p = res_get_ٻٽو({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
  إذا (p == خالي_اشارو) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

U_CAPI ثابت int32_t* U_EXPORT2
ures_get_Int_ويڪٽر(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_ڪوڊ* حالت) {
  ثابت int32_t *p;
  إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  إذا(resB == خالي_اشارو) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع خالي_اشارو;
  }
  p = res_get_Int_ويڪٽر({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
  إذا (p == خالي_اشارو) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ثابت UResourceBundle* resB, UError_ڪوڊ *حالت) {
  إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
    أرجع 0xffffffff;
  }
  إذا(resB == خالي_اشارو) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع 0xffffffff;
  }
  إذا(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ثابت UResourceBundle* resB, UError_ڪوڊ *حالت) {
  إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
    أرجع 0xffffffff;
  }
  إذا(resB == خالي_اشارو) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع 0xffffffff;
  }
  إذا(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ثابت UResourceBundle *resB) {
  إذا(resB == خالي_اشارو) {
    أرجع URES_NONE;
  }
  أرجع res_get_عوامي_Type(resB->fRes);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_ڪنجي(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  إذا(resB == خالي_اشارو) {
    أرجع خالي_اشارو;
  }
  أرجع(resB->f_ڪنجي);
}

U_CAPI int32_t U_EXPORT2 ures_get_سائيز(ثابت UResourceBundle *resB) {
  إذا(resB == خالي_اشارو) {
    أرجع 0;
  }
  
  أرجع resB->f_سائيز;
}

ساكن ثابت char16_t*
ures_getStringWithAlias(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_اشاريو,
                        int32_t* len,
                        UError_ڪوڊ* حالت) {
  إذا(RES_GET_TYPE(r) == URES_ALIAS) {
    ثابت char16_t* نتيجو = خالي_اشارو;
    UResourceBundle *عارضي_Res = ures_get_By_اشاريو(resB, s_اشاريو, خالي_اشارو, حالت);
    نتيجو = ures_getString(عارضي_Res, len, حالت);
    ures_close(عارضي_Res);
    أرجع نتيجو;
  } وإلا {
    أرجع res_getString({resB, s_اشاريو}, &resB->get_Res_معلومات(), r, len); 
  }
}

U_CAPI فراغ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  إذا(resB == خالي_اشارو) {
    أرجع;
  }
  resB->f_اشاريو = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_ايندڙ(ثابت UResourceBundle *resB) {
  إذا(resB == خالي_اشارو) {
    أرجع خطأ;
  }
  أرجع resB->f_اشاريو < resB->f_سائيز-1;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_ايندڙ_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ثابت حرف** ڪنجي,
                   UError_ڪوڊ* حالت) {
  Resource r = RES_BOGUS;
  
  إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  إذا(resB == خالي_اشارو) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع خالي_اشارو;
  }
  
  إذا(resB->f_اشاريو == resB->f_سائيز-1) {
    *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
  } وإلا {
    resB->f_اشاريو++;
    بدل(RES_GET_TYPE(resB->fRes)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
      أرجع res_getString({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
    حالة URES_جدول:
    حالة URES_TABLE16:
    حالة URES_TABLE32:
      r = res_get_جدول_شي_By_اشاريو(&resB->get_Res_معلومات(), resB->fRes, resB->f_اشاريو, ڪنجي);
      إذا(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->f_اشاريو, len, حالت);
    حالة URES_ARRAY:
    حالة URES_ARRAY16:
      r = res_get_Array_شي(&resB->get_Res_معلومات(), resB->fRes, resB->f_اشاريو);
      إذا(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->f_اشاريو, len, حالت);
    حالة URES_ALIAS:
      أرجع ures_getStringWithAlias(resB, resB->fRes, resB->f_اشاريو, len, حالت);
    حالة URES_INT:
    حالة URES_ٻٽو:
    حالة URES_INT_ويڪٽر:
        *حالت = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    افتراضي:
      أرجع خالي_اشارو;
    }
  }

  أرجع خالي_اشارو;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_ايندڙ_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_اندريون U_LIFETIME_BOUND,
                     UError_ڪوڊ* حالت) {
    ثابت حرف *ڪنجي = خالي_اشارو;
    Resource r = RES_BOGUS;

    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
            /*return nullptr;*/
            أرجع fill_اندريون;
    }
    إذا(resB == خالي_اشارو) {
            *حالت = U_ILLEGAL_دليل_ERROR;
            /*return nullptr;*/
            أرجع fill_اندريون;
    }

    إذا(resB->f_اشاريو == resB->f_سائيز-1) {
      *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } وإلا {
        resB->f_اشاريو++;
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_ٻٽو:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_ويڪٽر:
            أرجع ures_copyResb(fill_اندريون, resB, حالت);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_شي_By_اشاريو(&resB->get_Res_معلومات(), resB->fRes, resB->f_اشاريو, &ڪنجي);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتيجو(resB->f_معلومات, r, ڪنجي, resB->f_اشاريو, resB, fill_اندريون, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_شي(&resB->get_Res_معلومات(), resB->fRes, resB->f_اشاريو);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتيجو(resB->f_معلومات, r, ڪنجي, resB->f_اشاريو, resB, fill_اندريون, حالت);
        افتراضي:
            /*return nullptr;*/
            أرجع fill_اندريون;
        }
    }
    /*return nullptr;*/
    أرجع fill_اندريون;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_اشاريو(ثابت UResourceBundle* resB,
                int32_t اشاريو_R,
                UResourceBundle* fill_اندريون U_LIFETIME_BOUND,
                UError_ڪوڊ* حالت) {
    ثابت حرف* ڪنجي = خالي_اشارو;
    Resource r = RES_BOGUS;

    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        /*return nullptr;*/
        أرجع fill_اندريون;
    }
    إذا(resB == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        /*return nullptr;*/
        أرجع fill_اندريون;
    }

    إذا(اشاريو_R >= 0 && resB->f_سائيز > اشاريو_R) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_ٻٽو:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_ويڪٽر:
            أرجع ures_copyResb(fill_اندريون, resB, حالت);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_شي_By_اشاريو(&resB->get_Res_معلومات(), resB->fRes, اشاريو_R, &ڪنجي);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتيجو(resB->f_معلومات, r, ڪنجي, اشاريو_R, resB, fill_اندريون, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_شي(&resB->get_Res_معلومات(), resB->fRes, اشاريو_R);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتيجو(resB->f_معلومات, r, ڪنجي, اشاريو_R, resB, fill_اندريون, حالت);
        افتراضي:
            /*return nullptr;*/
            أرجع fill_اندريون;
        }
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    أرجع fill_اندريون;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_اشاريو(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t اشاريو_S,
                      int32_t* len,
                      UError_ڪوڊ* حالت) {
    ثابت حرف* ڪنجي = خالي_اشارو;
    Resource r = RES_BOGUS;

    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا(resB == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    إذا(اشاريو_S >= 0 && resB->f_سائيز > اشاريو_S) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_STRING:
        حالة URES_STRING_V2:
            أرجع res_getString({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_شي_By_اشاريو(&resB->get_Res_معلومات(), resB->fRes, اشاريو_S, &ڪنجي);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, اشاريو_S, len, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_شي(&resB->get_Res_معلومات(), resB->fRes, اشاريو_S);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, اشاريو_S, len, حالت);
        حالة URES_ALIAS:
            أرجع ures_getStringWithAlias(resB, resB->fRes, اشاريو_S, len, حالت);
        حالة URES_INT:
        حالة URES_ٻٽو:
        حالة URES_INT_ويڪٽر:
            *حالت = U_RESOURCE_TYPE_MISMATCH;
            اكسر;
        افتراضي:
          /* must not occur */
          *حالت = U_اندروني_پروگرام_ERROR;
          اكسر;
        }
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    أرجع خالي_اشارو;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_اشاريو(ثابت UResourceBundle* resB,
                          int32_t idx,
                          حرف* dest U_LIFETIME_BOUND,
                          int32_t* p_ڊيگهائي,
                          UBool forceCopy,
                          UError_ڪوڊ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_اشاريو(resB, idx, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_ڊيگهائي, forceCopy, حالت);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_ڳوليو_Resource(ثابت حرف* رستو, UResourceBundle* fill_اندريون U_LIFETIME_BOUND, UError_ڪوڊ* حالت)
{
  UResourceBundle *first = خالي_اشارو; 
  UResourceBundle *نتيجو = fill_اندريون;
  حرف *packageName = خالي_اشارو;
  حرف *رستو_منزل_Resource = خالي_اشارو, *save = خالي_اشارو;
  حرف *locale = خالي_اشارو, *locale_پڇاڙي = خالي_اشارو;
  int32_t length;

  إذا(حالت == خالي_اشارو || U_FAILURE(*حالت)) {
    أرجع نتيجو;
  }

  length = (int32_t)(uprv_strlen(رستو)+1);
  save = رستو_منزل_Resource = (حرف *)uprv_malloc(length*حجم(حرف));
  /* test for nullptr */
  إذا(رستو_منزل_Resource == خالي_اشارو) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع نتيجو;
  }
  uprv_memcpy(رستو_منزل_Resource, رستو, length);

  locale = رستو_منزل_Resource;
  إذا(*رستو_منزل_Resource == RES_رستو_SEPARATOR) { /* there is a path specification */
    رستو_منزل_Resource++;
    packageName = رستو_منزل_Resource;
    رستو_منزل_Resource = uprv_strchr(رستو_منزل_Resource, RES_رستو_SEPARATOR);
    إذا(رستو_منزل_Resource == خالي_اشارو) {
      *حالت = U_ILLEGAL_دليل_ERROR;
    } وإلا {
      *رستو_منزل_Resource = 0;
      locale = رستو_منزل_Resource+1;
    }
  }

  locale_پڇاڙي = uprv_strchr(locale, RES_رستو_SEPARATOR);
  إذا(locale_پڇاڙي != خالي_اشارو) {
    *locale_پڇاڙي = 0;
  }

  first = ures_open(packageName, locale, حالت);

  إذا(U_SUCCESS(*حالت)) {
    إذا(locale_پڇاڙي) {
      نتيجو = ures_ڳوليو_Sub_Resource(first, locale_پڇاڙي+1, fill_اندريون, حالت);
    } وإلا {
      نتيجو = ures_copyResb(fill_اندريون, first, حالت);
    }
    ures_close(first);
  }
  uprv_free(save);
  أرجع نتيجو;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_ڳوليو_Sub_Resource(ثابت UResourceBundle* resB,
                     حرف* رستو,
                     UResourceBundle* fill_اندريون U_LIFETIME_BOUND,
                     UError_ڪوڊ* حالت)
{
  Resource res = RES_BOGUS;
  UResourceBundle *نتيجو = fill_اندريون;
  ثابت حرف *ڪنجي;

  إذا(حالت == خالي_اشارو || U_FAILURE(*حالت)) {
    أرجع نتيجو;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  افعل {
    res = res_ڳوليو_Resource(&resB->get_Res_معلومات(), resB->fRes, &رستو, &ڪنجي); 
    إذا(res != RES_BOGUS) {
        نتيجو = init_resb_نتيجو(resB->f_معلومات, res, ڪنجي, -1, resB, fill_اندريون, حالت);
        resB = نتيجو;
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
        اكسر;
    }
  } طالما(*رستو); /* there is more stuff in the path */

  أرجع نتيجو;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_ڪنجي_With_Fallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت حرف* اندريون_ڪنجي,
                                int32_t* len,
                                UError_ڪوڊ* حالت) {
    UResourceBundle ڍير_5;
    ثابت char16_t* retVal = خالي_اشارو;
    ures_init_ڍير_Object(&ڍير_5);
    ures_get_By_ڪنجي_With_Fallback(resB, اندريون_ڪنجي, &ڍير_5, حالت);
    int32_t length;
    retVal = ures_getString(&ڍير_5, &length, حالت);
    ures_close(&ڍير_5);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (length == 3 && retVal[0] == خالي_جايون_سيٽ && retVal[1] == خالي_جايون_سيٽ && retVal[2] == خالي_جايون_سيٽ ) {
        retVal = خالي_اشارو;
        length = 0;
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    إذا (len != خالي_اشارو) {
        *len = length;
    }
    أرجع retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ساكن Resource get_جدول_شي_By_ڪنجي_رستو(ثابت Resource_معلومات *p_Res_معلومات, Resource جدول, ثابت حرف *ڪنجي) {
  Resource resource = جدول;  /* The current resource */
  icu::CharString رستو;
  UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
  رستو.append(ڪنجي, error_ڪوڊ);
  إذا (U_FAILURE(error_ڪوڊ)) { أرجع RES_BOGUS; }
  حرف *رستو_Part = رستو.معلومات();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  طالما (*رستو_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    حرف *ايندڙ_رستو_Part = uprv_strchr(رستو_Part, RES_رستو_SEPARATOR);
    إذا (ايندڙ_رستو_Part != خالي_اشارو) {
      *ايندڙ_رستو_Part = 0;  /* Terminating null for this part of path. */
      ايندڙ_رستو_Part++;
    } وإلا {
      ايندڙ_رستو_Part = uprv_strchr(رستو_Part, 0);
    }
    int32_t t;
    ثابت حرف *رستو_P = رستو_Part;
    resource = res_get_جدول_شي_By_ڪنجي(p_Res_معلومات, resource, &t, &رستو_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    رستو_Part = ايندڙ_رستو_Part; 
  }
  إذا (*رستو_Part) {
    أرجع RES_BOGUS;
  }
  أرجع resource;
}

ساكن فراغ create_رستو(ثابت حرف* orig_Res_رستو,
                       int32_t     orig_Res_رستو_Len,
                       ثابت حرف* res_رستو,
                       int32_t     res_رستو_Len,
                       ثابت حرف* اندريون_ڪنجي,
                       CharString& رستو,
                       UError_ڪوڊ* حالت) {
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
    رستو.clear();
    ثابت حرف* ڪنجي = اندريون_ڪنجي;
    إذا (res_رستو_Len > 0) {
        رستو.append(res_رستو, res_رستو_Len, *حالت);
        إذا (U_SUCCESS(*حالت)) {
            ثابت حرف* res_رستو_حد_بندي = res_رستو + res_رستو_Len;
            ثابت حرف* orig_Res_رستو_حد_بندي = orig_Res_رستو + orig_Res_رستو_Len;
            ثابت حرف* res_رستو_Ptr = res_رستو;
            ثابت حرف* orig_Res_رستو_Ptr = orig_Res_رستو;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            طالما (orig_Res_رستو_Ptr < orig_Res_رستو_حد_بندي && res_رستو_Ptr < res_رستو_حد_بندي) {
                طالما (orig_Res_رستو_Ptr < orig_Res_رستو_حد_بندي && *orig_Res_رستو_Ptr != RES_رستو_SEPARATOR) {
                    ++orig_Res_رستو_Ptr;
                }
                إذا (orig_Res_رستو_Ptr < orig_Res_رستو_حد_بندي && *orig_Res_رستو_Ptr == RES_رستو_SEPARATOR) {
                    ++orig_Res_رستو_Ptr;
                }
                طالما (res_رستو_Ptr < res_رستو_حد_بندي && *res_رستو_Ptr != RES_رستو_SEPARATOR) {
                    ++res_رستو_Ptr;
                }
                إذا (res_رستو_Ptr < res_رستو_حد_بندي && *res_رستو_Ptr == RES_رستو_SEPARATOR) {
                    ++res_رستو_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            طالما (res_رستو_Ptr < res_رستو_حد_بندي && *ڪنجي != '\0') {
                طالما (res_رستو_Ptr < res_رستو_حد_بندي && *res_رستو_Ptr != RES_رستو_SEPARATOR) {
                    ++res_رستو_Ptr;
                }
                إذا (res_رستو_Ptr < res_رستو_حد_بندي && *res_رستو_Ptr == RES_رستو_SEPARATOR) {
                    ++res_رستو_Ptr;
                }
                طالما (*ڪنجي != '\0' && *ڪنجي != RES_رستو_SEPARATOR) {
                    ++ڪنجي;
                }
                إذا (*ڪنجي == RES_رستو_SEPARATOR) {
                    ++ڪنجي;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        رستو.append(ڪنجي, *حالت);
    } وإلا {
        رستو.append(اندريون_ڪنجي, *حالت);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ڪنجي_With_Fallback(ثابت UResourceBundle* resB,
                          ثابت حرف* اندريون_ڪنجي,
                          UResourceBundle* fill_اندريون U_LIFETIME_BOUND,
                          UError_ڪوڊ* حالت) {
    Resource res = RES_BOGUS, پاڙ_Res = RES_BOGUS;
    UResourceBundle *helper = خالي_اشارو;

    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع fill_اندريون;
    }
    إذا(resB == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع fill_اندريون;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        ثابت حرف* orig_Res_رستو = resB->f_Res_رستو;
        int32_t orig_Res_رستو_Len = resB->f_Res_رستو_Len;
        res = get_جدول_شي_By_ڪنجي_رستو(&resB->get_Res_معلومات(), resB->fRes, اندريون_ڪنجي);
        ثابت حرف* ڪنجي = اندريون_ڪنجي;
        منطقي did_پاڙ_Once = خطأ;
        إذا(res == RES_BOGUS) {
            UResource_معلومات_داخلا *معلومات_داخلا = resB->f_معلومات;
            CharString رستو;
            حرف *my_رستو = خالي_اشارو;
            ثابت حرف* res_رستو = resB->f_Res_رستو;
            int32_t len = resB->f_Res_رستو_Len;
            طالما(res == RES_BOGUS && (معلومات_داخلا->f_والد != خالي_اشارو || !did_پاڙ_Once)) { /* Otherwise, we'll look in parents */
                إذا (معلومات_داخلا->f_والد != خالي_اشارو) {
                    معلومات_داخلا = معلومات_داخلا->f_والد;
                } وإلا {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_پاڙ_Once = سچ;
                }
                پاڙ_Res = معلومات_داخلا->f_معلومات.پاڙ_Res;

                إذا(معلومات_داخلا->fBogus == U_ZERO_ERROR) {
                    create_رستو(orig_Res_رستو, orig_Res_رستو_Len, res_رستو, len, اندريون_ڪنجي, رستو, حالت);
                    إذا (U_FAILURE(*حالت)) {
                        ures_close(helper);
                        أرجع fill_اندريون;
                    }
                    my_رستو = رستو.معلومات();
                    ڪنجي = اندريون_ڪنجي;
                    افعل {
                        res = res_ڳوليو_Resource(&(معلومات_داخلا->f_معلومات), پاڙ_Res, &my_رستو, &ڪنجي);
                        إذا (RES_GET_TYPE(res) == URES_ALIAS && *my_رستو) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_نتيجو(معلومات_داخلا, res, خالي_اشارو, -1, resB, helper, حالت);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            إذا(helper) {
                              معلومات_داخلا = helper->f_معلومات;
                              پاڙ_Res = helper->fRes;
                              res_رستو = helper->f_Res_رستو;
                              len = helper->f_Res_رستو_Len;

                            } وإلا {
                              اكسر;
                            }
                        } وإلا إذا (res == RES_BOGUS) {
                            اكسر;
                        }
                    } طالما(*my_رستو); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            إذا(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                إذا(uprv_strcmp(معلومات_داخلا->fName, uloc_getDefault())==0 || uprv_strcmp(معلومات_داخلا->fName, k_پاڙ_Locale_Name)==0) {
                    *حالت = U_USING_DEFAULT_WARNING;
                } وإلا {
                    *حالت = U_USING_FALLBACK_WARNING;
                }

                fill_اندريون = init_resb_نتيجو(معلومات_داخلا, res, ڪنجي, -1, resB, fill_اندريون, حالت);
                إذا (res_رستو != خالي_اشارو) {
                    create_رستو(orig_Res_رستو, orig_Res_رستو_Len, res_رستو, len, اندريون_ڪنجي, رستو, حالت);
                } وإلا {
                    ثابت حرف* separator = خالي_اشارو;
                    إذا (fill_اندريون->f_Res_رستو != خالي_اشارو) {
                        separator = uprv_strchr(fill_اندريون->f_Res_رستو, RES_رستو_SEPARATOR);
                    }
                    إذا (separator != خالي_اشارو && separator[1] != '\0') {
                        create_رستو(orig_Res_رستو, orig_Res_رستو_Len, fill_اندريون->f_Res_رستو,
                                   static_cast<int32_t>(uprv_strlen(fill_اندريون->f_Res_رستو)), اندريون_ڪنجي, رستو, حالت);
                    } وإلا {
                        create_رستو(orig_Res_رستو, orig_Res_رستو_Len, "", 0, اندريون_ڪنجي, رستو, حالت);
                    }
                }
                ures_free_Res_رستو(fill_اندريون);
                ures_append_Res_رستو(fill_اندريون, رستو.معلومات(), رستو.length(), حالت);
                إذا(fill_اندريون->f_Res_رستو[fill_اندريون->f_Res_رستو_Len-1] != RES_رستو_SEPARATOR) {
                    ures_append_Res_رستو(fill_اندريون, RES_رستو_SEPARATOR_S, 1, حالت);
                }
            } وإلا {
                *حالت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            fill_اندريون = init_resb_نتيجو(resB->f_معلومات, res, ڪنجي, -1, resB, fill_اندريون, حالت);
        }
    } 
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    أرجع fill_اندريون;
}

نطاق {

فراغ get_All_شيون_With_Fallback(
        ثابت UResourceBundle *bundle, Resource_معلومات_قدر &value,
        ResourceSink &sink, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
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
    value.سيٽ_معلومات(bundle->get_Res_معلومات());
    value.سيٽ_Valid_Locale_معلومات_داخلا(bundle->f_Valid_Locale_معلومات_داخلا);
    UResource_معلومات_داخلا *والد_داخلا = bundle->f_معلومات->f_والد;
    UBool has_والد = والد_داخلا != خالي_اشارو && U_SUCCESS(والد_داخلا->fBogus);
    value.سيٽ_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_ڪنجي, value, !has_والد, error_ڪوڊ);
    إذا (has_والد) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ڍير_UResource_Bundle والد_Bundle;
        UResourceBundle &والد_Ref = والد_Bundle.ref();
        والد_Ref.f_معلومات = والد_داخلا;
        والد_Ref.f_Valid_Locale_معلومات_داخلا = bundle->f_Valid_Locale_معلومات_داخلا;
        والد_Ref.fHasFallback = !والد_Ref.get_Res_معلومات().noFallback;
        والد_Ref.f_Is_چوٽي_Level = سچ;
        والد_Ref.fRes = والد_Ref.get_Res_معلومات().پاڙ_Res;
        والد_Ref.f_سائيز = res_ڳڻپ_Array_شيون(&والد_Ref.get_Res_معلومات(), والد_Ref.fRes);
        والد_Ref.f_اشاريو = -1;
        داخلا_Increase(والد_داخلا);

        // Look up the container item in the parent bundle.
        ڍير_UResource_Bundle containerBundle;
        ثابت UResourceBundle *rb;
        UError_ڪوڊ رستو_Error_ڪوڊ = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        إذا (bundle->f_Res_رستو == خالي_اشارو || *bundle->f_Res_رستو == 0) {
            rb = والد_Bundle.getAlias();
        } وإلا {
            rb = ures_get_By_ڪنجي_With_Fallback(والد_Bundle.getAlias(), bundle->f_Res_رستو,
                                           containerBundle.getAlias(), &رستو_Error_ڪوڊ);
        }
        إذا (U_SUCCESS(رستو_Error_ڪوڊ)) {
            get_All_شيون_With_Fallback(rb, value, sink, error_ڪوڊ);
        }
    }
}

بنية GetAllChildrenSink : عام ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    مجازي ~GetAllChildrenSink() override;
    مجازي فراغ put(ثابت حرف *ڪنجي, Resource_قدر &value, UBool is_پاڙ,
           UError_ڪوڊ &error_ڪوڊ) override {
        Resource_جدول شيون_جدول = value.get_جدول(error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        لكل (int32_t i = 0; شيون_جدول.get_ڪنجي_And_قدر(i, ڪنجي, value); ++i) {
            إذا (value.getType() == URES_ALIAS) {
                Resource_معلومات_قدر& rdv = static_cast<Resource_معلومات_قدر&>(value);
                ڍير_UResource_Bundle ڍير_عارضي_Bundle;
                UResourceBundle* aliasRB = get_Alias_هدف_As_Resource_Bundle(rdv.get_معلومات(), rdv.getResource(), خالي_اشارو, -1,
                                                                          rdv.get_Valid_Locale_معلومات_داخلا(), خالي_اشارو, 0,
                                                                          ڍير_عارضي_Bundle.getAlias(), &error_ڪوڊ);
                إذا (U_SUCCESS(error_ڪوڊ)) {
                    Resource_معلومات_قدر aliased_قدر;
                    aliased_قدر.سيٽ_معلومات(aliasRB->get_Res_معلومات());
                    aliased_قدر.سيٽ_Valid_Locale_معلومات_داخلا(aliasRB->f_Valid_Locale_معلومات_داخلا);
                    aliased_قدر.سيٽ_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    إذا (aliased_قدر.getType() != URES_جدول) {
                        dest.put(ڪنجي, aliased_قدر, is_پاڙ, error_ڪوڊ);
                    } وإلا {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_قدر_Type = URES_جدول;
                        CharString جدول_رستو;
                        جدول_رستو.append(aliasRB->f_Res_رستو, error_ڪوڊ);
                        ثابت حرف* والد_ڪنجي = ڪنجي; // dest.put() changes the key
                        dest.put(والد_ڪنجي, aliased_قدر, is_پاڙ, error_ڪوڊ);
                        UResource_معلومات_داخلا* داخلا_2 = aliasRB->f_معلومات;
                        Resource res = aliasRB->fRes;
                        طالما (aliased_قدر_Type == URES_جدول && داخلا_2->f_والد != خالي_اشارو) {
                            CharString local_رستو;
                            local_رستو.copyFrom(جدول_رستو, error_ڪوڊ);
                            حرف* local_رستو_As_Char_Ptr = local_رستو.معلومات();
                            ثابت حرف* ٻار_ڪنجي;
                            داخلا_2 = داخلا_2->f_والد;
                            res = داخلا_2->f_معلومات.پاڙ_Res;
                            Resource newRes = res_ڳوليو_Resource(&داخلا_2->f_معلومات, res, &local_رستو_As_Char_Ptr, &ٻار_ڪنجي);
                            إذا (newRes != RES_BOGUS) {
                                aliased_قدر.سيٽ_معلومات(داخلا_2->f_معلومات);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_قدر.سيٽ_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_قدر_Type = aliased_قدر.getType();
                                إذا (aliased_قدر_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_معلومات_قدر& rdv2 = static_cast<Resource_معلومات_قدر&>(aliased_قدر);
                                    aliasRB = get_Alias_هدف_As_Resource_Bundle(rdv2.get_معلومات(), rdv2.getResource(), خالي_اشارو, -1,
                                                                             rdv2.get_Valid_Locale_معلومات_داخلا(), خالي_اشارو, 0,
                                                                             ڍير_عارضي_Bundle.getAlias(), &error_ڪوڊ);
                                    جدول_رستو.clear();
                                    جدول_رستو.append(aliasRB->f_Res_رستو, error_ڪوڊ);
                                    داخلا_2 = aliasRB->f_معلومات;
                                    res = aliasRB->fRes;
                                    aliased_قدر.سيٽ_معلومات(داخلا_2->f_معلومات);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_قدر.سيٽ_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_قدر_Type = aliased_قدر.getType();
                                }
                                إذا (aliased_قدر_Type == URES_جدول) {
                                    dest.put(والد_ڪنجي, aliased_قدر, is_پاڙ, error_ڪوڊ);
                                } وإلا {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_ڪوڊ = U_اندروني_پروگرام_ERROR;
                                    أرجع;
                                }
                            }
                        }
                    }
                }
            } وإلا {
                dest.put(ڪنجي, value, is_پاڙ, error_ڪوڊ);
            }
            إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI فراغ U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *رستو,
                                icu::ResourceSink &sink, UError_ڪوڊ &error_ڪوڊ) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_شيون_With_Fallback(bundle, رستو, allChildrenSink, error_ڪوڊ);
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
ures_get_قدر_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *رستو,
                          UResourceBundle *عارضي_Fill_اندريون,
                          Resource_معلومات_قدر &value, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    إذا (رستو == خالي_اشارو) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    ثابت UResourceBundle *rb;
    إذا (*رستو == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_get_By_ڪنجي_With_Fallback(bundle, رستو, عارضي_Fill_اندريون, &error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) {
            أرجع;
        }
    }
    value.سيٽ_معلومات(rb->get_Res_معلومات());
    value.سيٽ_Valid_Locale_معلومات_داخلا(rb->f_Valid_Locale_معلومات_داخلا);
    value.سيٽ_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI فراغ U_EXPORT2
ures_get_All_شيون_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *رستو,
                             icu::ResourceSink &sink, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    إذا (رستو == خالي_اشارو) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    ڍير_UResource_Bundle ڍير_Bundle;
    ثابت UResourceBundle *rb;
    إذا (*رستو == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_get_By_ڪنجي_With_Fallback(bundle, رستو, ڍير_Bundle.getAlias(), &error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) {
            أرجع;
        }
    }
    // Get all table items with fallback.
    Resource_معلومات_قدر value;
    get_All_شيون_With_Fallback(rb, value, sink, error_ڪوڊ);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_ڪنجي(ثابت UResourceBundle* resB,
              ثابت حرف* اندريون_ڪنجي,
              UResourceBundle* fill_اندريون U_LIFETIME_BOUND,
              UError_ڪوڊ* حالت) {
    Resource res = RES_BOGUS;
    UResource_معلومات_داخلا *معلومات_داخلا = خالي_اشارو;
    ثابت حرف *ڪنجي = اندريون_ڪنجي;

    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع fill_اندريون;
    }
    إذا(resB == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع fill_اندريون;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        int32_t t;
        res = res_get_جدول_شي_By_ڪنجي(&resB->get_Res_معلومات(), resB->fRes, &t, &ڪنجي);
        إذا(res == RES_BOGUS) {
            ڪنجي = اندريون_ڪنجي;
            إذا(resB->fHasFallback) {
                معلومات_داخلا = get_Fallback_معلومات(resB, &ڪنجي, &res, حالت);
                إذا(U_SUCCESS(*حالت)) {
                    /* check if resB->fResPath gives the right name here */
                    أرجع init_resb_نتيجو(معلومات_داخلا, res, ڪنجي, -1, resB, fill_اندريون, حالت);
                } وإلا {
                    *حالت = U_MISSING_RESOURCE_ERROR;
                }
            } وإلا {
                *حالت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            أرجع init_resb_نتيجو(resB->f_معلومات, res, ڪنجي, -1, resB, fill_اندريون, حالت);
        }
    } 
#إذا 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    وإلا إذا(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == سچ) {
        /* here should go a first attempt to locate the key using index table */
        معلومات_داخلا = get_Fallback_معلومات(resB, &ڪنجي, &res, حالت);
        إذا(U_SUCCESS(*حالت)) {
            أرجع init_resb_نتيجو(معلومات_داخلا, res, ڪنجي, resB, fill_اندريون, حالت);
        } وإلا {
            *حالت = U_MISSING_RESOURCE_ERROR;
        }
    }
#جيڪڏهن_پورو    
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع fill_اندريون;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_ڪنجي(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                    ثابت حرف* اندريون_ڪنجي,
                    int32_t* len,
                    UError_ڪوڊ* حالت) {
    Resource res = RES_BOGUS;
    UResource_معلومات_داخلا *معلومات_داخلا = خالي_اشارو;
    ثابت حرف* ڪنجي = اندريون_ڪنجي;

    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا(resB == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        int32_t t=0;

        res = res_get_جدول_شي_By_ڪنجي(&resB->get_Res_معلومات(), resB->fRes, &t, &ڪنجي);

        إذا(res == RES_BOGUS) {
            ڪنجي = اندريون_ڪنجي;
            إذا(resB->fHasFallback) {
                معلومات_داخلا = get_Fallback_معلومات(resB, &ڪنجي, &res, حالت);
                إذا(U_SUCCESS(*حالت)) {
                    بدل (RES_GET_TYPE(res)) {
                    حالة URES_STRING:
                    حالة URES_STRING_V2:
                        أرجع res_getString({resB, ڪنجي}, &معلومات_داخلا->f_معلومات, res, len);
                    حالة URES_ALIAS:
                      {
                        ثابت char16_t* نتيجو = خالي_اشارو;
                        UResourceBundle *عارضي_Res = ures_get_By_ڪنجي(resB, اندريون_ڪنجي, خالي_اشارو, حالت);
                        نتيجو = ures_getString(عارضي_Res, len, حالت);
                        ures_close(عارضي_Res);
                        أرجع نتيجو;
                      }
                    افتراضي:
                        *حالت = U_RESOURCE_TYPE_MISMATCH;
                    }
                } وإلا {
                    *حالت = U_MISSING_RESOURCE_ERROR;
                }
            } وإلا {
                *حالت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            بدل (RES_GET_TYPE(res)) {
            حالة URES_STRING:
            حالة URES_STRING_V2:
                أرجع res_getString({resB, ڪنجي}, &resB->get_Res_معلومات(), res, len);
            حالة URES_ALIAS:
              {
                ثابت char16_t* نتيجو = خالي_اشارو;
                UResourceBundle *عارضي_Res = ures_get_By_ڪنجي(resB, اندريون_ڪنجي, خالي_اشارو, حالت);
                نتيجو = ures_getString(عارضي_Res, len, حالت);
                ures_close(عارضي_Res);
                أرجع نتيجو;
              }
            افتراضي:
                *حالت = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#إذا 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    وإلا إذا(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == سچ) {
        /* here should go a first attempt to locate the key using index table */
        معلومات_داخلا = get_Fallback_معلومات(resB, &ڪنجي, &res, حالت);
        إذا(U_SUCCESS(*حالت)) {
            // TODO: Tracing
            أرجع res_getString(rd, res, len);
        } وإلا {
            *حالت = U_MISSING_RESOURCE_ERROR;
        }
    } 
#جيڪڏهن_پورو    
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع خالي_اشارو;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_ڪنجي(ثابت UResourceBundle* resB,
                        ثابت حرف* ڪنجي,
                        حرف* dest U_LIFETIME_BOUND,
                        int32_t* p_ڊيگهائي,
                        UBool forceCopy,
                        UError_ڪوڊ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_ڪنجي(resB, ڪنجي, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_ڊيگهائي, forceCopy, حالت);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ثابت حرف* U_EXPORT2
ures_get_Locale_اندروني(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_ڪوڊ* حالت)
{
    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (!resourceBundle) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    } وإلا {
      أرجع resourceBundle->f_معلومات->fName;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocale(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_ڪوڊ* حالت)
{
  أرجع ures_get_Locale_اندروني(resourceBundle, حالت);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocaleByType(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_معلومات_Locale_Type type,
                     UError_ڪوڊ* حالت) {
    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (!resourceBundle) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    } وإلا {
        بدل(type) {
        حالة ULOC_ACTUAL_LOCALE:
            أرجع resourceBundle->f_معلومات->fName;
        حالة ULOC_VALID_LOCALE:
            أرجع resourceBundle->f_Valid_Locale_معلومات_داخلا->fName;
        حالة ULOC_REQUESTED_LOCALE:
        افتراضي:
            *حالت = U_ILLEGAL_دليل_ERROR;
            أرجع خالي_اشارو;
        }
    }
}

U_CFUNC ثابت حرف*
ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  إذا(resB == خالي_اشارو) {
    أرجع خالي_اشارو;
  }

  أرجع resB->f_معلومات->fName;
}

#جيڪڏهن_وصف_ٿيل URES_DEBUG
U_CFUNC ثابت حرف*
ures_get_رستو(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  إذا(resB == خالي_اشارو) {
    أرجع خالي_اشارو;
  }

  أرجع resB->f_معلومات->f_رستو;
}
#جيڪڏهن_پورو

ساكن UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ثابت حرف* رستو,
                  ثابت حرف* localeID,
                  UResOpenType openType,
                  UError_ڪوڊ* حالت) {
    إذا(U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    UResource_معلومات_داخلا *داخلا_2;
    إذا(openType != URES_OPEN_سڌو) {
        إذا (localeID == خالي_اشارو) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_بنياد_Name(localeID, *حالت);
        إذا(U_FAILURE(*حالت)) {
            *حالت = U_ILLEGAL_دليل_ERROR;
            أرجع خالي_اشارو;
        }
        داخلا_2 = داخلا_Open(رستو, canonLocaleID.معلومات(), openType, حالت);
    } وإلا {
        داخلا_2 = داخلا_Open_سڌو(رستو, localeID, حالت);
    }
    إذا(U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا(داخلا_2 == خالي_اشارو) {
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع خالي_اشارو;
    }

    UBool is_ڍير_Object;
    إذا(r == خالي_اشارو) {
        r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        إذا(r == خالي_اشارو) {
            داخلا_Close(داخلا_2);
            *حالت = U_يادگيري_ورهاست_ERROR;
            أرجع خالي_اشارو;
        }
        is_ڍير_Object = خطأ;
    } وإلا {  // fill-in
        is_ڍير_Object = ures_is_ڍير_Object(r);
        ures_closeBundle(r, خطأ);
    }
    uprv_memset(r, 0, حجم(UResourceBundle));
    ures_سيٽ_Is_ڍير_Object(r, is_ڍير_Object);

    r->f_Valid_Locale_معلومات_داخلا = r->f_معلومات = داخلا_2;
    r->fHasFallback = openType != URES_OPEN_سڌو && !r->get_Res_معلومات().noFallback;
    r->f_Is_چوٽي_Level = سچ;
    r->fRes = r->get_Res_معلومات().پاڙ_Res;
    r->f_سائيز = res_ڳڻپ_Array_شيون(&r->get_Res_معلومات(), r->fRes);
    r->f_اشاريو = -1;

    ResourceTracer(r).traceOpen();

    أرجع r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ثابت حرف* رستو, ثابت حرف* localeID, UError_ڪوڊ* حالت) {
    أرجع ures_openWithType(خالي_اشارو, رستو, localeID, URES_OPEN_LOCALE_DEFAULT_پاڙ, حالت);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت حرف* رستو, ثابت حرف* localeID, UError_ڪوڊ* حالت) {
    أرجع ures_openWithType(خالي_اشارو, رستو, localeID, URES_OPEN_LOCALE_پاڙ, حالت);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_سڌو(ثابت حرف* رستو, ثابت حرف* localeID, UError_ڪوڊ* حالت) {
    أرجع ures_openWithType(خالي_اشارو, رستو, localeID, URES_OPEN_سڌو, حالت);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_open_Fill_اندريون(UResourceBundle *r, ثابت حرف* رستو,
                ثابت حرف* localeID, UError_ڪوڊ* حالت) {
    إذا(U_SUCCESS(*حالت) && r == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    ures_openWithType(r, رستو, localeID, URES_OPEN_LOCALE_DEFAULT_پاڙ, حالت);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_open_سڌو_Fill_اندريون(UResourceBundle *r, ثابت حرف* رستو, ثابت حرف* localeID, UError_ڪوڊ* حالت) {
    إذا(U_SUCCESS(*حالت) && r == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    ures_openWithType(r, رستو, localeID, URES_OPEN_سڌو, حالت);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_ڳڻپ_Array_شيون(ثابت UResourceBundle* resourceBundle,
                  ثابت حرف* resource_ڪنجي,
                  UError_ڪوڊ* حالت)
{
    UResourceBundle res_معلومات;
    ures_init_ڍير_Object(&res_معلومات);
    إذا (حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(resourceBundle == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    ures_get_By_ڪنجي(resourceBundle, resource_ڪنجي, &res_معلومات, حالت);
    
    إذا(res_معلومات.get_Res_معلومات().معلومات != خالي_اشارو) {
        int32_t نتيجو = res_ڳڻپ_Array_شيون(&res_معلومات.get_Res_معلومات(), res_معلومات.fRes);
        ures_close(&res_معلومات);
        أرجع نتيجو;
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_معلومات);
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
ures_get_نسخو_عدد_اندروني(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    إذا (!resourceBundle) أرجع خالي_اشارو;

    إذا(resourceBundle->f_نسخو == خالي_اشارو) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ثابت char16_t* minor_نسخو = ures_get_String_By_ڪنجي(resourceBundle, k_نسخو_ٽيگ, &minor_len, &حالت);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_نسخو = (حرف *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        إذا (((UResourceBundle *)resourceBundle)->f_نسخو == خالي_اشارو) {
            أرجع خالي_اشارو;
        }
       
        إذا(minor_len > 0) {
            u_UChars_منزل_Chars(minor_نسخو, resourceBundle->f_نسخو , minor_len);
            resourceBundle->f_نسخو[len] =  '\0';
        }
        وإلا {
            uprv_strcpy(resourceBundle->f_نسخو, k_Default_Minor_نسخو);
        }
    }

    أرجع resourceBundle->f_نسخو;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_نسخو_عدد(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    أرجع ures_get_نسخو_عدد_اندروني(resourceBundle);
}

U_CAPI فراغ U_EXPORT2 ures_get_نسخو(ثابت UResourceBundle* resB, UVersionInfo نسخو_Info) {
    إذا (!resB) أرجع;

    u_نسخو_From_String(نسخو_Info, ures_get_نسخو_عدد_اندروني(resB));
}

/** Tree support functions *******************************/
#وصف_ڪريو اشاريو_LOCALE_NAME "res_index"
#وصف_ڪريو اشاريو_ٽيگ         "InstalledLocales"
#وصف_ڪريو DEFAULT_ٽيگ       "default"

#إذا defined(URES_وڻ_DEBUG)
#ضمن <stdio.h>
#جيڪڏهن_پورو

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
ures_loc_ڳڻپ_Locales(UEnumeration *en, UError_ڪوڊ * /*حالت*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    أرجع ures_get_سائيز(&ctx->installed);
}

U_CDECL_BEGIN


ساكن ثابت حرف * U_CALLCONV
ures_loc_ايندڙ_Locale(UEnumeration* en,
                    int32_t* نتيجو_ڊيگهائي,
                    UError_ڪوڊ* حالت) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = خالي_اشارو;
    ثابت حرف *نتيجو = خالي_اشارو;
    int32_t len = 0;
    إذا (ures_has_ايندڙ(res) && (k = ures_get_ايندڙ_Resource(res, &ctx->curr, حالت)) != خالي_اشارو) {
        نتيجو = ures_get_ڪنجي(k);
        len = (int32_t)uprv_strlen(نتيجو);
    }
    إذا (نتيجو_ڊيگهائي) {
        *نتيجو_ڊيگهائي = len;
    }
    أرجع نتيجو;
}

ساكن فراغ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_ڪوڊ* /*حالت*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_پڇاڙي

ساكن ثابت UEnumeration gLocalesEnum = {
    خالي_اشارو,
        خالي_اشارو,
        ures_loc_closeLocales,
        ures_loc_ڳڻپ_Locales,
        uenum_unextDefault,
        ures_loc_ايندڙ_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_موجود_Locales(ثابت حرف *رستو, UError_ڪوڊ *حالت)
{
    UResourceBundle *idx = خالي_اشارو;
    UEnumeration *en = خالي_اشارو;
    ULocalesContext *myContext = خالي_اشارو;

    إذا(U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(حجم(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    إذا(!en || !myContext) {
        *حالت = U_يادگيري_ورهاست_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        أرجع خالي_اشارو;
    }
    uprv_memcpy(en, &gLocalesEnum, حجم(UEnumeration));

    ures_init_ڍير_Object(&myContext->installed);
    ures_init_ڍير_Object(&myContext->curr);
    idx = ures_open_سڌو(رستو, اشاريو_LOCALE_NAME, حالت);
    ures_get_By_ڪنجي(idx, اشاريو_ٽيگ, &myContext->installed, حالت);
    إذا(U_SUCCESS(*حالت)) {
#إذا defined(URES_وڻ_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            رستو, اشاريو_LOCALE_NAME, اشاريو_ٽيگ, ures_get_ڪنجي(&myContext->installed));
#جيڪڏهن_پورو
        en->context = myContext;
    } وإلا {
#إذا defined(URES_وڻ_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", رستو, u_errorName(*حالت));
#جيڪڏهن_پورو
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = خالي_اشارو;
    }
    
    ures_close(idx);
    
    أرجع en;
}

ساكن UBool is_Locale_اندريون_List(UEnumeration *locEnum, ثابت حرف *loc_منزل_ڳولا, UError_ڪوڊ *حالت) {
    ثابت حرف *loc;
    طالما ((loc = uenum_ايندڙ(locEnum, خالي_اشارو, حالت)) != خالي_اشارو) {
        إذا (uprv_strcmp(loc, loc_منزل_ڳولا) == 0) {
            أرجع سچ;
        }
    }
    أرجع خطأ;
}

ساكن فراغ get_والد_For_Functional_Equivalent(ثابت حرف*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      والد) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
    والد.clear();
    إذا (res != خالي_اشارو) {
        ures_get_By_ڪنجي(res, "%%Parent", bund1, &sub_حالت);
        إذا (U_SUCCESS(sub_حالت)) {
            int32_t length16;
            ثابت char16_t* s16 = ures_getString(bund1, &length16, &sub_حالت);
            والد.appendInvariantChars(s16, length16, sub_حالت);
        }
    }
    
    // If none there, use normal truncation parent
    إذا (U_FAILURE(sub_حالت) || والد.is_خالي_جايون()) {
        sub_حالت = U_ZERO_ERROR;
        والد = ulocimp_get_والد(localeID, sub_حالت);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(حرف *نتيجو, int32_t نتيجو_صلاحيت,
                             ثابت حرف *رستو, ثابت حرف *resName, ثابت حرف *keyword, ثابت حرف *locid,
                             UBool *is_موجود, UBool omitDefault, UError_ڪوڊ *حالت)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString مليو;
    CharString والد;
    CharString ڀريل;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = خالي_اشارو;
    UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
    int32_t length = 0;
    إذا(U_FAILURE(*حالت)) أرجع 0;
    CharString kwVal;
    إذا (keyword != خالي_اشارو && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_قدر(locid, keyword, sub_حالت);
        إذا (kwVal == DEFAULT_ٽيگ) {
            kwVal.clear();
        }
    }
    إذا (locid == خالي_اشارو) {
        locid = uloc_getDefault();
    }
    CharString بنياد = ulocimp_get_بنياد_Name(locid, sub_حالت);
#إذا defined(URES_وڻ_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.معلومات(), بنياد.معلومات(), u_errorName(sub_حالت));
#جيڪڏهن_پورو
    ures_init_ڍير_Object(&bund1);
    ures_init_ڍير_Object(&bund2);

    والد.copyFrom(بنياد, sub_حالت);
    مليو.copyFrom(بنياد, sub_حالت);

    إذا(is_موجود) {
        UEnumeration *locEnum = ures_open_موجود_Locales(رستو, &sub_حالت);
        *is_موجود = سچ;
        إذا (U_SUCCESS(sub_حالت)) {
            *is_موجود = is_Locale_اندريون_List(locEnum, والد.معلومات(), &sub_حالت);
        }
        uenum_close(locEnum);
    }

    إذا(U_FAILURE(sub_حالت)) {
        *حالت = sub_حالت;
        أرجع 0;
    }
    
    افعل {
        sub_حالت = U_ZERO_ERROR;
        res = ures_open(رستو, والد.معلومات(), &sub_حالت);
        إذا(((sub_حالت == U_USING_FALLBACK_WARNING) ||
            (sub_حالت == U_USING_DEFAULT_WARNING)) && is_موجود)
        {
            *is_موجود = خطأ;
        }
        is_موجود = خالي_اشارو; /* only want to set this the first time around */
        
#إذا defined(URES_وڻ_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", رستو?رستو:"ICUDATA", والد.معلومات(), u_errorName(sub_حالت), ures_getLocale(res, &sub_حالت));
#جيڪڏهن_پورو
        إذا(U_FAILURE(sub_حالت)) {
            *حالت = sub_حالت;
        } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
            ures_get_By_ڪنجي(res,resName,&bund1, &sub_حالت);
            إذا(sub_حالت == U_ZERO_ERROR) {
                ثابت char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#إذا defined(URES_وڻ_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    رستو?رستو:"ICUDATA", والد.معلومات(), u_errorName(sub_حالت));
#جيڪڏهن_پورو
                defUstr = ures_get_String_By_ڪنجي(&bund1, DEFAULT_ٽيگ, &defLen, &sub_حالت);
                إذا(U_SUCCESS(sub_حالت) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_وڻ_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        رستو?رستو:"ICUDATA", والد.معلومات(), keyword, defVal.معلومات(), u_errorName(sub_حالت));
#جيڪڏهن_پورو
                    defLoc.copyFrom(والد, sub_حالت);
                    إذا(kwVal.is_خالي_جايون()) {
                        kwVal.append(defVal, sub_حالت);
#إذا defined(URES_وڻ_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            رستو?رستو:"ICUDATA", والد.معلومات(), keyword, kwVal.معلومات());
#جيڪڏهن_پورو
                    }
                }
            }
        }
        
        sub_حالت = U_ZERO_ERROR;

        إذا (res != خالي_اشارو) {
            مليو.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_حالت), sub_حالت);
        }

        إذا (مليو != والد) {
            والد.copyFrom(مليو, sub_حالت);
        } وإلا {
            get_والد_For_Functional_Equivalent(مليو.معلومات(),res,&bund1,والد);
        }
        ures_close(res);
    } طالما(defVal.is_خالي_جايون() && !مليو.is_خالي_جايون() && مليو != "root" && U_SUCCESS(*حالت));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    والد.copyFrom(بنياد, sub_حالت);
    مليو.copyFrom(بنياد, sub_حالت);

    افعل {
        res = ures_open(رستو, والد.معلومات(), &sub_حالت);
        إذا((sub_حالت == U_USING_FALLBACK_WARNING) && is_موجود) {
            *is_موجود = خطأ;
        }
        is_موجود = خالي_اشارو; /* only want to set this the first time around */
        
#إذا defined(URES_وڻ_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            رستو?رستو:"ICUDATA", والد.معلومات(), u_errorName(sub_حالت), kwVal.معلومات());
#جيڪڏهن_پورو
        إذا(U_FAILURE(sub_حالت)) {
            *حالت = sub_حالت;
        } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
            ures_get_By_ڪنجي(res,resName,&bund1, &sub_حالت);
#إذا defined(URES_وڻ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_حالت));
#جيڪڏهن_پورو
            إذا(sub_حالت == U_ZERO_ERROR) {
                ures_get_By_ڪنجي(&bund1, kwVal.معلومات(), &bund2, &sub_حالت);
#إذا defined(URES_وڻ_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.معلومات(), u_errorName(sub_حالت));
#جيڪڏهن_پورو
                إذا(sub_حالت == U_ZERO_ERROR) {
#إذا defined(URES_وڻ_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        رستو?رستو:"ICUDATA", والد.معلومات(), keyword, kwVal.معلومات(), u_errorName(sub_حالت));
#جيڪڏهن_پورو
                    إذا (والد.is_خالي_جايون()) {
                        ڀريل.clear().append("root", sub_حالت);
                    } وإلا {
                        ڀريل.copyFrom(والد, sub_حالت);
                    }
                        /* now, recalculate default kw if need be */
                        إذا(defLoc.length() > ڀريل.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#إذا defined(URES_وڻ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    رستو?رستو:"ICUDATA", ڀريل.معلومات());
#جيڪڏهن_پورو
                          defUstr = ures_get_String_By_ڪنجي(&bund1, DEFAULT_ٽيگ, &defLen, &sub_حالت);
                          إذا(U_SUCCESS(sub_حالت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_وڻ_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    رستو?رستو:"ICUDATA", ڀريل.معلومات(), keyword, defVal.معلومات(), u_errorName(sub_حالت));
#جيڪڏهن_پورو
                            defLoc.copyFrom(ڀريل, sub_حالت);
                          }
                        } /* end of recalculate default KW */
#إذا defined(URES_وڻ_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.معلومات(), ڀريل.معلومات());
                        }
#جيڪڏهن_پورو
                } وإلا {
#إذا defined(URES_وڻ_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_حالت), والد.معلومات(), kwVal.معلومات());
#جيڪڏهن_پورو
                }
            }
        }
        
        sub_حالت = U_ZERO_ERROR;
        UBool have_مليو = خطأ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        إذا (res != خالي_اشارو && uprv_strcmp(resName, "collations") == 0) {
            ثابت حرف *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_حالت);
            إذا (U_SUCCESS(sub_حالت) && validLoc != خالي_اشارو && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_حالت);
                CharString والد_Lang = ulocimp_getLanguage(والد.منزل_String_Piece(), sub_حالت);
                إذا (U_SUCCESS(sub_حالت) && validLang != والد_Lang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    مليو.clear().append(validLoc, sub_حالت);
                    have_مليو = سچ;
                }
            }
            sub_حالت = U_ZERO_ERROR;
        }
        إذا (!have_مليو) {
            مليو.copyFrom(والد, sub_حالت);
        }

        get_والد_For_Functional_Equivalent(مليو.معلومات(),res,&bund1,والد);
        ures_close(res);
        sub_حالت = U_ZERO_ERROR;
    } طالما(ڀريل.is_خالي_جايون() && !مليو.is_خالي_جايون() && U_SUCCESS(*حالت));

    إذا(ڀريل.is_خالي_جايون() && kwVal != defVal) {
#إذا defined(URES_وڻ_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.معلومات(), defVal.معلومات());
#جيڪڏهن_پورو
        kwVal.clear().append(defVal, sub_حالت);
        والد.copyFrom(بنياد, sub_حالت);
        مليو.copyFrom(بنياد, sub_حالت);

        افعل { /* search for 'default' named item */
            res = ures_open(رستو, والد.معلومات(), &sub_حالت);
            إذا((sub_حالت == U_USING_FALLBACK_WARNING) && is_موجود) {
                *is_موجود = خطأ;
            }
            is_موجود = خالي_اشارو; /* only want to set this the first time around */
            
#إذا defined(URES_وڻ_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                رستو?رستو:"ICUDATA", والد.معلومات(), u_errorName(sub_حالت), kwVal.معلومات());
#جيڪڏهن_پورو
            إذا(U_FAILURE(sub_حالت)) {
                *حالت = sub_حالت;
            } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
                ures_get_By_ڪنجي(res,resName,&bund1, &sub_حالت);
                إذا(sub_حالت == U_ZERO_ERROR) {
                    ures_get_By_ڪنجي(&bund1, kwVal.معلومات(), &bund2, &sub_حالت);
                    إذا(sub_حالت == U_ZERO_ERROR) {
#إذا defined(URES_وڻ_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", رستو?رستو:"ICUDATA",
                            والد.معلومات(), keyword, kwVal.معلومات(), u_errorName(sub_حالت));
#جيڪڏهن_پورو
                        إذا (والد.is_خالي_جايون()) {
                            ڀريل.clear().append("root", sub_حالت);
                        } وإلا {
                            ڀريل.copyFrom(والد, sub_حالت);
                        }
                        
                        /* now, recalculate default kw if need be */
                        إذا(defLoc.length() > ڀريل.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#إذا defined(URES_وڻ_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    رستو?رستو:"ICUDATA", ڀريل.معلومات());
#جيڪڏهن_پورو
                          defUstr = ures_get_String_By_ڪنجي(&bund1, DEFAULT_ٽيگ, &defLen, &sub_حالت);
                          إذا(U_SUCCESS(sub_حالت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_وڻ_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    رستو?رستو:"ICUDATA", ڀريل.معلومات(), keyword, defVal.معلومات(), u_errorName(sub_حالت));
#جيڪڏهن_پورو
                            defLoc.copyFrom(ڀريل, sub_حالت);
                          }
                        } /* end of recalculate default KW */
#إذا defined(URES_وڻ_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.معلومات(), ڀريل.معلومات());
                        }
#جيڪڏهن_پورو
                    }
                }
            }
            
            sub_حالت = U_ZERO_ERROR;
            مليو.copyFrom(والد, sub_حالت);
            get_والد_For_Functional_Equivalent(مليو.معلومات(),res,&bund1,والد);
            ures_close(res);
            sub_حالت = U_ZERO_ERROR;
        } طالما(ڀريل.is_خالي_جايون() && !مليو.is_خالي_جايون() && U_SUCCESS(*حالت));
    }
    
    إذا(U_SUCCESS(*حالت)) {
        إذا(ڀريل.is_خالي_جايون()) {
#إذا defined(URES_وڻ_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.معلومات());
#جيڪڏهن_پورو
          *حالت = U_MISSING_RESOURCE_ERROR;
        } وإلا إذا(omitDefault) {
#إذا defined(URES_وڻ_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", ڀريل.معلومات(), defLoc.معلومات(), مليو.معلومات());
#جيڪڏهن_پورو        
          إذا(defLoc.length() <= ڀريل.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            إذا(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#إذا defined(URES_وڻ_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.معلومات());
#جيڪڏهن_پورو
              kwVal.clear();
            }
          }
        }
        مليو.copyFrom(ڀريل, sub_حالت);
        إذا(!kwVal.is_خالي_جايون()) {
            مليو
                .append("@", sub_حالت)
                .append(keyword, sub_حالت)
                .append("=", sub_حالت)
                .append(kwVal, sub_حالت);
        } وإلا إذا(!omitDefault) {
            مليو
                .append("@", sub_حالت)
                .append(keyword, sub_حالت)
                .append("=", sub_حالت)
                .append(defVal, sub_حالت);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = مليو.length();

    إذا(U_SUCCESS(*حالت)) {
        int32_t copy_ڊيگهائي = uprv_min(length, نتيجو_صلاحيت);
        إذا(copy_ڊيگهائي>0) {
            مليو.extract(نتيجو, copy_ڊيگهائي, sub_حالت);
        }
        إذا(length == 0) {
          *حالت = U_MISSING_RESOURCE_ERROR; 
        }
    } وإلا {
        length = 0;
        نتيجو[0]=0;
    }
    أرجع u_terminateChars(نتيجو, نتيجو_صلاحيت, length, حالت);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_قدرون(ثابت حرف *رستو, ثابت حرف *keyword, UError_ڪوڊ *حالت)
{
#وصف_ڪريو قدرون_BUF_سائيز 2048
#وصف_ڪريو قدرون_LIST_سائيز 512
    
    حرف       قدرون_Buf[قدرون_BUF_سائيز];
    int32_t    قدرون_اشاريو = 0;
    ثابت حرف *قدرون_List[قدرون_LIST_سائيز];
    int32_t    قدرون_ڳڻپ = 0;
    
    ثابت حرف *locale;
    int32_t     locLen;
    
    UEnumeration *locs = خالي_اشارو;
    
    UResourceBundle    شي;
    UResourceBundle    sub_شي;
    
    ures_init_ڍير_Object(&شي);
    ures_init_ڍير_Object(&sub_شي);
    locs = ures_open_موجود_Locales(رستو, حالت);
    
    إذا(U_FAILURE(*حالت)) {
        ures_close(&شي);
        ures_close(&sub_شي);
        أرجع خالي_اشارو;
    }
    
    قدرون_Buf[0]=0;
    قدرون_Buf[1]=0;

    طالما ((locale = uenum_ايندڙ(locs, &locLen, حالت)) != خالي_اشارو) {
        UResourceBundle   *bund = خالي_اشارو;
        UResourceBundle   *subPtr = خالي_اشارو;
        UError_ڪوڊ sub_حالت = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(رستو, locale, &sub_حالت);
        
#إذا defined(URES_وڻ_DEBUG)
        إذا(!bund || U_FAILURE(sub_حالت)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                رستو?رستو:"<ICUDATA>", keyword, locale, u_errorName(sub_حالت));
        }
#جيڪڏهن_پورو
        
        ures_get_By_ڪنجي(bund, keyword, &شي, &sub_حالت);
        
        إذا(!bund || U_FAILURE(sub_حالت)) {
#إذا defined(URES_وڻ_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                رستو?رستو:"<ICUDATA>", keyword, locale, u_errorName(sub_حالت));
#جيڪڏهن_پورو
            ures_close(bund);
            bund = خالي_اشارو;
            استمر;
        }

        طالما ((subPtr = ures_get_ايندڙ_Resource(&شي, &sub_شي, &sub_حالت)) != خالي_اشارو
            && U_SUCCESS(sub_حالت)) {
            ثابت حرف *k;
            int32_t i;
            k = ures_get_ڪنجي(subPtr);

#إذا defined(URES_وڻ_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#جيڪڏهن_پورو
            إذا(k == خالي_اشارو || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_ٽيگ) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                استمر;
            }
            لكل(i=0; i<قدرون_ڳڻپ; i++) {
                إذا(!uprv_strcmp(قدرون_List[i],k)) {
                    k = خالي_اشارو; /* found duplicate */
                    اكسر;
                }
            }
            إذا(k != خالي_اشارو) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                إذا((قدرون_ڳڻپ >= (قدرون_LIST_سائيز-1)) ||       /* no more space in list .. */
                    ((قدرون_اشاريو+kLen+1+1) >= قدرون_BUF_سائيز)) { /* no more space in buffer (string + 2 nulls) */
                    *حالت = U_ILLEGAL_دليل_ERROR; /* out of space.. */
                } وإلا {
                    uprv_strcpy(قدرون_Buf+قدرون_اشاريو, k);
                    قدرون_List[قدرون_ڳڻپ++] = قدرون_Buf+قدرون_اشاريو;
                    قدرون_اشاريو += kLen;
#إذا defined(URES_وڻ_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        رستو?رستو:"<ICUDATA>", keyword, locale, k);
#جيڪڏهن_پورو
                    قدرون_Buf[قدرون_اشاريو++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    قدرون_Buf[قدرون_اشاريو++] = 0; /* terminate */
    
    ures_close(&شي);
    ures_close(&sub_شي);
    uenum_close(locs);
#إذا defined(URES_وڻ_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*حالت), 
        قدرون_اشاريو, قدرون_ڳڻپ);
#جيڪڏهن_پورو
    أرجع uloc_openKeywordList(قدرون_Buf, قدرون_اشاريو, حالت);
}
#إذا 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ثابت UResourceBundle* res1, ثابت UResourceBundle* res2){
    إذا(res1==خالي_اشارو || res2==خالي_اشارو){
        أرجع res1==res2; /* pointer comparison */
    }
    إذا(res1->f_ڪنجي==خالي_اشارو||  res2->f_ڪنجي==خالي_اشارو){
        أرجع (res1->f_ڪنجي==res2->f_ڪنجي);
    }وإلا{
        إذا(uprv_strcmp(res1->f_ڪنجي, res2->f_ڪنجي)!=0){
            أرجع خطأ;
        }
    }
    إذا(uprv_strcmp(res1->f_معلومات->fName, res2->f_معلومات->fName)!=0){
        أرجع خطأ;
    }
    إذا(res1->f_معلومات->f_رستو == خالي_اشارو||  res2->f_معلومات->f_رستو==خالي_اشارو){
        أرجع (res1->f_معلومات->f_رستو == res2->f_معلومات->f_رستو);
    }وإلا{
        إذا(uprv_strcmp(res1->f_معلومات->f_رستو, res2->f_معلومات->f_رستو)!=0){
            أرجع خطأ;
        }
    }
    إذا(uprv_strcmp(res1->f_معلومات->f_والد->fName, res2->f_معلومات->f_والد->fName)!=0){
        أرجع خطأ;
    }
    إذا(uprv_strcmp(res1->f_معلومات->f_والد->f_رستو, res2->f_معلومات->f_والد->f_رستو)!=0){
        أرجع خطأ;
    }
    إذا(uprv_strncmp(res1->f_Res_رستو, res2->f_Res_رستو, res1->f_Res_رستو_Len)!=0){
        أرجع خطأ;
    }
    إذا(res1->fRes != res2->fRes){
        أرجع خطأ;
    }
    أرجع سچ;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ثابت UResourceBundle* res, UError_ڪوڊ* حالت){
    UResourceBundle* bundle = خالي_اشارو;
    UResourceBundle* ret = خالي_اشارو;
    إذا(U_FAILURE(*حالت) || res == خالي_اشارو){
        أرجع خالي_اشارو;
    }
    bundle = ures_open(res->f_معلومات->f_رستو, res->f_معلومات->fName, حالت);
    إذا(res->f_Res_رستو!=خالي_اشارو){
        ret = ures_ڳوليو_Sub_Resource(bundle, res->f_Res_رستو, خالي_اشارو, حالت);
        ures_close(bundle);
    }وإلا{
        ret = bundle;
    }
    أرجع ret;
}
U_CAPI ثابت UResourceBundle* U_EXPORT2
ures_get_والد_Bundle(ثابت UResourceBundle* res){
    إذا(res==خالي_اشارو){
        أرجع خالي_اشارو;
    }
    أرجع res->f_والد_Res;
}
#جيڪڏهن_پورو

U_CAPI فراغ U_EXPORT2
ures_get_نسخو_By_ڪنجي(ثابت UResourceBundle* res, ثابت حرف *ڪنجي, UVersionInfo ver, UError_ڪوڊ *حالت) {
  ثابت char16_t *str;
  int32_t len;
  str = ures_get_String_By_ڪنجي(res, ڪنجي, &len, حالت);
  إذا(U_SUCCESS(*حالت)) {
    u_نسخو_From_UString(ver, str);
  } 
}

/* eof */
