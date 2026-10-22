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

کارول نطاق icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ساكن UHashtable *cache = تش_اشاره;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ساكن int32_t U_CALLCONV hash_ثبت(ثابت UHashTok parm) {
    UResource_معلومات_ثبت* b = static_cast<UResource_معلومات_ثبت*>(parm.اشاره_ګر);
    UHashTok namekey, pathkey;
    namekey.اشاره_ګر = b->fName;
    pathkey.اشاره_ګر = b->f_لار;
    أرجع uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ساكن UBool U_CALLCONV compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UResource_معلومات_ثبت* b1 = static_cast<UResource_معلومات_ثبت*>(p1.اشاره_ګر);
    UResource_معلومات_ثبت* b2 = static_cast<UResource_معلومات_ثبت*>(p2.اشاره_ګر);
    UHashTok name1, name2, path1, path2;
    name1.اشاره_ګر = b1->fName;
    name2.اشاره_ګر = b2->fName;
    path1.اشاره_ګر = b1->f_لار;
    path2.اشاره_ګر = b2->f_لار;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ساكن UBool chopLocale(حرف *name) {
    حرف *i = uprv_strrchr(name, '_');

    إذا(i != تش_اشاره) {
        *i = '\0';
        أرجع ریښتیا;
    }

    أرجع خطأ;
}

ساكن UBool hasVariant(ثابت حرف* localeID) {
    UError_کوډ err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(تش_اشاره, 0);
    ulocimp_getSubtags(
            localeID,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره,
            &sink,
            تش_اشاره,
            err);
    أرجع sink.شمېره_د_بایټونه_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#تعریف_کړه INCLUDED_FROM_URESBUND_CPP
#ضمن "localefallback_data.h"

ساكن ثابت حرف* performFallbackLookup(ثابت حرف* کلی,
                                         ثابت حرف* کلی_Strs,
                                         ثابت حرف* ارزښت_Strs U_LIFETIME_BOUND,
                                         ثابت int32_t* lookup_جدول,
                                         int32_t lookup_جدول_اوږدتیا) {
    ثابت int32_t* bottom = lookup_جدول;
    ثابت int32_t* سر_2 = lookup_جدول + lookup_جدول_اوږدتیا;

    طالما (bottom < سر_2) {
        // Effectively, divide by 2 and round down to an even index
        ثابت int32_t* منځ = bottom + (((سر_2 - bottom) / 4) * 2);
        ثابت حرف* ثبت_کلی = &(کلی_Strs[*منځ]);
        int32_t strcmp_پایله = uprv_strcmp(کلی, ثبت_کلی);
        إذا (strcmp_پایله == 0) {
            أرجع &(ارزښت_Strs[منځ[1]]);
        } وإلا إذا (strcmp_پایله < 0) {
            سر_2 = منځ;
        } وإلا {
            bottom = منځ + 2;
        }
    }
    أرجع تش_اشاره;
}

ساكن CharString getDefaultScript(ثابت CharString& language, ثابت CharString& region) {
    ثابت حرف* defaultScript = تش_اشاره;
    UError_کوډ err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString پایله("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    إذا (!region.is_تشه()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        إذا (U_FAILURE(err)) {
            أرجع پایله;
        }
        defaultScript = performFallbackLookup(localeID.معلومات(), dsLocaleIDChars, script_کوډ_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    إذا (defaultScript == تش_اشاره) {
        defaultScript = performFallbackLookup(language.معلومات(), dsLocaleIDChars, script_کوډ_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    إذا (defaultScript != تش_اشاره) {
        پایله.clear();
        پایله.append(defaultScript, err);
    }
    أرجع پایله;
}

تعداد UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_ریښه,
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
    URES_OPEN_LOCALE_ریښه,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_مستقیم
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
ساكن منطقي get_پلار_Locale_ID(حرف *name, ثابت حرف *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    إذا (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        أرجع chopLocale(name);
    }
    
    UError_کوډ err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, تش_اشاره, تش_اشاره, err);

    إذا (U_FAILURE(err)) {
        // hopefully this never happens...
        أرجع chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    إذا (openType == URES_OPEN_LOCALE_DEFAULT_ریښه) {
        ثابت حرف* پلار_ID = performFallbackLookup(name, پلار_Locale_Chars, پلار_Locale_Chars, پلار_Locale_جدول, UPRV_LENGTHOF(پلار_Locale_جدول));
        إذا (پلار_ID != تش_اشاره) {
            uprv_strcpy(name, پلار_ID);
            أرجع ریښتیا;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    إذا (!script.is_تشه() && !region.is_تشه()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        إذا (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } وإلا إذا (!region.is_تشه()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_کوډ err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, تش_اشاره, تش_اشاره, تش_اشاره, err);
        إذا (!origNameScript.is_تشه()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } وإلا إذا (!script.is_تشه()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        إذا (openType != URES_OPEN_LOCALE_DEFAULT_ریښه || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } وإلا {
            أرجع خطأ;
        }
    } وإلا {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        أرجع خطأ;
    }
    إذا (U_SUCCESS(err) && !workingLocale.is_تشه()) {
        uprv_strcpy(name, workingLocale.معلومات());
        أرجع ریښتیا;
    } وإلا {
        أرجع خطأ;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ساكن UBool may_Have_پلار(حرف *name) {
    أرجع (name[0] != 0 && uprv_strstr("nb nn",name) != تش_اشاره);
}

/**
 *  Internal function
 */
ساكن فراغ ثبت_Increase(UResource_معلومات_ثبت *ثبت) {
    Mutex قلف(&resbMutex);
    ثبت->f_شمېر_Existing++;
    طالما(ثبت->f_پلار != تش_اشاره) {
      ثبت = ثبت->f_پلار;
      ثبت->f_شمېر_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ساكن UResource_معلومات_ثبت *get_Fallback_معلومات(
        ثابت UResourceBundle *resBundle,
        ثابت حرف **res_نښان, Resource *res, UError_کوډ *حالت) {
    UResource_معلومات_ثبت *معلومات_ثبت = resBundle->f_معلومات;
    int32_t شاخص_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    إذا(معلومات_ثبت == تش_اشاره) {
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع تش_اشاره;
    }
    إذا(معلومات_ثبت->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_جدول_توکی_By_کلی(&(معلومات_ثبت->f_معلومات), معلومات_ثبت->f_معلومات.ریښه_Res, &شاخص_R, res_نښان); /* try to get data from there */
        i++;
    }
    إذا(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        طالما(*res == RES_BOGUS && معلومات_ثبت->f_پلار != تش_اشاره) {
            معلومات_ثبت = معلومات_ثبت->f_پلار;
            إذا(معلومات_ثبت->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_جدول_توکی_By_کلی(&(معلومات_ثبت->f_معلومات), معلومات_ثبت->f_معلومات.ریښه_Res, &شاخص_R, res_نښان);
            }
        }
    }

    إذا(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع تش_اشاره;
    }
    // If the resource is found in parents, we need to adjust the error.
    إذا(i>1) {
        إذا(uprv_strcmp(معلومات_ثبت->fName, uloc_getDefault())==0 || uprv_strcmp(معلومات_ثبت->fName, k_ریښه_Locale_Name)==0) {
            *حالت = U_USING_DEFAULT_WARNING;
        } وإلا {
            *حالت = U_USING_FALLBACK_WARNING;
        }
    }
    أرجع معلومات_ثبت;
}

ساكن فراغ
free_ثبت(UResource_معلومات_ثبت *ثبت) {
    UResource_معلومات_ثبت *alias;
    res_unload(&(ثبت->f_معلومات));
    إذا(ثبت->fName != تش_اشاره && ثبت->fName != ثبت->f_Name_بفر) {
        uprv_free(ثبت->fName);
    }
    إذا(ثبت->f_لار != تش_اشاره) {
        uprv_free(ثبت->f_لار);
    }
    إذا(ثبت->fPool != تش_اشاره) {
        --ثبت->fPool->f_شمېر_Existing;
    }
    alias = ثبت->fAlias;
    إذا(alias != تش_اشاره) {
        طالما(alias->fAlias != تش_اشاره) {
            alias = alias->fAlias;
        }
        --alias->f_شمېر_Existing;
    }
    uprv_free(ثبت);
}

/* Works just like ucnv_flushCache() */
ساكن int32_t ures_flushCache()
{
    UResource_معلومات_ثبت *resB;
    int32_t pos;
    int32_t rb_Deleted_عدد = 0;
    ثابت UHashElement *e;
    UBool deleted_نور;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex قلف(&resbMutex);
    إذا (cache == تش_اشاره) {
        أرجع 0;
    }

    افعل {
        deleted_نور = خطأ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_لومړی;
        طالما ((e = uhash_بل_Element(cache, &pos)) != تش_اشاره)
        {
            resB = static_cast<UResource_معلومات_ثبت*>(e->value.اشاره_ګر);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            إذا (resB->f_شمېر_Existing == 0) {
                rb_Deleted_عدد++;
                deleted_نور = ریښتیا;
                uhash_removeElement(cache, e);
                free_ثبت(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } طالما(deleted_نور);

    أرجع rb_Deleted_عدد;
}

#که_تعریف_شوی URES_DEBUG
#ضمن <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_تشه = خطأ;
  int32_t pos = UHASH_لومړی;
  ثابت UHashElement *e;
  UResource_معلومات_ثبت *resB;
  
    Mutex قلف(&resbMutex);
    إذا (cache == تش_اشاره) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      أرجع خطأ;
    }

    طالما ((e = uhash_بل_Element(cache, &pos)) != تش_اشاره) {
      cache_Not_تشه=ریښتیا;
      resB = (UResource_معلومات_ثبت *) e->value.اشاره_ګر;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (فراغ*)resB, resB->f_شمېر_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_لار?resB->f_لار:"nullptr",
              (فراغ*)resB->fPool,
              (فراغ*)resB->fAlias,
              (فراغ*)resB->f_پلار);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_شمېر(cache));
    أرجع cache_Not_تشه;
}

#د_که_پای

ساكن UBool U_CALLCONV ures_cleanup()
{
    إذا (cache != تش_اشاره) {
        ures_flushCache();
        uhash_close(cache);
        cache = تش_اشاره;
    }
    gCacheInitOnce.reset();
    أرجع ریښتیا;
}

/** INTERNAL: Initializes the cache for resources */
ساكن فراغ U_CALLCONV createCache(UError_کوډ &حالت) {
    U_ASSERT(cache == تش_اشاره);
    cache = uhash_open(hash_ثبت, compareEntries, تش_اشاره, &حالت);
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ساكن فراغ initCache(UError_کوډ *حالت) {
    umtx_initOnce(gCacheInitOnce, &createCache, *حالت);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ساكن فراغ سېټ_ثبت_Name(UResource_معلومات_ثبت *res, ثابت حرف *name, UError_کوډ *حالت) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    إذا(res->fName != تش_اشاره && res->fName != res->f_Name_بفر) {
        uprv_free(res->fName);
    }
    إذا (len < static_cast<int32_t>(حجم(res->f_Name_بفر))) {
        res->fName = res->f_Name_بفر;
    }
    وإلا {
        res->fName = static_cast<حرف*>(uprv_malloc(len + 1));
    }
    إذا(res->fName == تش_اشاره) {
        *حالت = U_حافظه_ویش_ERROR;
    } وإلا {
        uprv_strcpy(res->fName, name);
    }
}

ساكن UResource_معلومات_ثبت *
get_Pool_ثبت(ثابت حرف *لار, UError_کوډ *حالت);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ساكن UResource_معلومات_ثبت *init_ثبت(ثابت حرف *localeID, ثابت حرف *لار, UError_کوډ *حالت) {
    UResource_معلومات_ثبت *r = تش_اشاره;
    UResource_معلومات_ثبت find;
    /*int32_t hashValue;*/
    ثابت حرف *name;
    حرف aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    إذا(U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }

    /* here we try to deduce the right locale name */
    إذا(localeID == تش_اشاره) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } وإلا إذا(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_ریښه_Locale_Name;
    } وإلا { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<حرف*>(name);
    find.f_لار = const_cast<حرف*>(لار);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_معلومات_ثبت*>(uhash_get(cache, &find));
    إذا(r == تش_اشاره) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_معلومات_ثبت*>(uprv_malloc(حجم(UResource_معلومات_ثبت)));
        إذا(r == تش_اشاره) {
            *حالت = U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }

        uprv_memset(r, 0, حجم(UResource_معلومات_ثبت));
        /*r->fHashKey = hashValue;*/

        سېټ_ثبت_Name(r, name, حالت);
        إذا (U_FAILURE(*حالت)) {
            uprv_free(r);
            أرجع تش_اشاره;
        }

        إذا(لار != تش_اشاره) {
            r->f_لار = uprv_strdup(لار);
            إذا(r->f_لار == تش_اشاره) {
                *حالت = U_حافظه_ویش_ERROR;
                uprv_free(r);
                أرجع تش_اشاره;
            }
        }

        /* this is the actual loading */
        res_بارول(&(r->f_معلومات), r->f_لار, r->fName, حالت);

        إذا (U_FAILURE(*حالت)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            إذا (*حالت == U_حافظه_ویش_ERROR) {
                uprv_free(r);
                أرجع تش_اشاره;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *حالت = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } وإلا { /* if we have a regular entry */
            Resource aliasres;
            إذا (r->f_معلومات.کاروي_Pool_Bundle) {
                r->fPool = get_Pool_ثبت(r->f_لار, حالت);
                إذا (U_SUCCESS(*حالت)) {
                    ثابت int32_t *poolIndexes = r->fPool->f_معلومات.p_ریښه + 1;
                    إذا(r->f_معلومات.p_ریښه[1 + URES_شاخص_POOL_CHECKSUM] == poolIndexes[URES_شاخص_POOL_CHECKSUM]) {
                        r->f_معلومات.pool_Bundle_کلیګانې = reinterpret_cast<ثابت حرف*>(poolIndexes + (poolIndexes[URES_شاخص_اوږدتیا] & 0xff));
                        r->f_معلومات.pool_Bundle_مزي_ګانې = r->fPool->f_معلومات.p16_بټ_Units;
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
                    إذا(alias != تش_اشاره && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_منزل_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_ثبت(aliasName, لار, حالت);
                    }
                }
            }
        }

        {
            UResource_معلومات_ثبت *oldR = تش_اشاره;
            إذا ((oldR = static_cast<UResource_معلومات_ثبت*>(uhash_get(cache, r))) == تش_اشاره) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_کوډ cache_حالت = U_ZERO_ERROR;
                uhash_put(cache, (فراغ *)r, r, &cache_حالت);
                إذا (U_FAILURE(cache_حالت)) {
                    *حالت = cache_حالت;
                    free_ثبت(r);
                    r = تش_اشاره;
                }
            } وإلا {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_ثبت(r);
                r = oldR;
            }
        }

    }
    إذا(r != تش_اشاره) {
        /* return the real bundle */
        طالما(r->fAlias != تش_اشاره) {
            r = r->fAlias;
        }
        r->f_شمېر_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        إذا(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*حالت)) {
             *حالت = r->fBogus; /* set the returning status */
        }
    }
    أرجع r;
}

ساكن UResource_معلومات_ثبت *
get_Pool_ثبت(ثابت حرف *لار, UError_کوډ *حالت) {
    UResource_معلومات_ثبت *poolBundle = init_ثبت(kPoolBundleName, لار, حالت);
    إذا( U_SUCCESS(*حالت) &&
        (poolBundle == تش_اشاره || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_معلومات.isPoolBundle)
    ) {
        *حالت = U_INVALID_FORMAT_ERROR;
    }
    أرجع poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ساكن UResource_معلومات_ثبت *
پیدا_کړه_لومړی_Existing(ثابت حرف* لار, حرف* name, ثابت حرف* defaultLocale, UResOpenType openType,
                  UBool *is_ریښه, UBool *وموندل_پلار, UBool *isDefault, UError_کوډ* حالت) {
    UResource_معلومات_ثبت *r = تش_اشاره;
    UBool has_Real_معلومات = خطأ;
    *وموندل_پلار = ریښتیا; /* we're starting with a fresh name */
    حرف origName[ULOC_FULLNAME_ظرفیت];

    uprv_strcpy(origName, name);
    طالما(*وموندل_پلار && !has_Real_معلومات) {
        r = init_ثبت(name, لار, حالت);
        /* Null pointer test */
        إذا (U_FAILURE(*حالت)) {
            أرجع تش_اشاره;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_معلومات = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        إذا(!has_Real_معلومات) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_شمېر_Existing--;
            /*entryCloseInt(r);*/
            r = تش_اشاره;
            *حالت = U_USING_FALLBACK_WARNING;
        } وإلا {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_ریښه = static_cast<UBool>(uprv_strcmp(name, k_ریښه_Locale_Name) == 0);

        /*Fallback data stuff*/
        إذا (!has_Real_معلومات) {
            *وموندل_پلار = get_پلار_Locale_ID(name, origName, openType);
        } وإلا {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *وموندل_پلار = chopLocale(name);
        }
        إذا (*وموندل_پلار && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    أرجع r;
}

ساكن فراغ ures_سېټ_Is_ډېری_Object( UResourceBundle* resB, UBool state) {
    إذا(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } وإلا {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ساكن UBool ures_is_ډېری_Object(ثابت UResourceBundle* resB) {
  أرجع((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?خطأ:ریښتیا);
}


U_CFUNC فراغ ures_init_ډېری_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, حجم(UResourceBundle));
  ures_سېټ_Is_ډېری_Object(resB, ریښتیا);
}

U_NAMESPACE_BEGIN

ډېری_UResource_Bundle::ډېری_UResource_Bundle() {
    ures_init_ډېری_Object(&bundle);
}

ډېری_UResource_Bundle::~ډېری_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_پای_ټکی

ساكن UBool  // returns U_SUCCESS(*status)
بارول_Parents_Except_ریښه(UResource_معلومات_ثبت *&t1,
                      حرف name[], int32_t name_ظرفیت,
                      UBool usingUSRData, حرف usr_معلومات_لار[], UError_کوډ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع خطأ; }
    UBool check_پلار = ریښتیا;
    طالما (check_پلار && t1->f_پلار == تش_اشاره && !t1->f_معلومات.noFallback &&
            res_getResource(&t1->f_معلومات,"%%ParentIsRoot") == RES_BOGUS) {
        Resource پلار_Res = res_getResource(&t1->f_معلومات, "%%Parent");
        إذا (پلار_Res != RES_BOGUS) {  // An explicit parent was found.
            int32_t پلار_Locale_Len = 0;
            // No tracing: called during initial data loading
            ثابت char16_t *پلار_Locale_Name = res_getStringNoTrace(&(t1->f_معلومات), پلار_Res, &پلار_Locale_Len);
            إذا(پلار_Locale_Name != تش_اشاره && 0 < پلار_Locale_Len && پلار_Locale_Len < name_ظرفیت) {
                u_UChars_منزل_Chars(پلار_Locale_Name, name, پلار_Locale_Len + 1);
                إذا (uprv_strcmp(name, k_ریښه_Locale_Name) == 0) {
                    أرجع ریښتیا;
                }
            }
        }
        // Insert regular parents.
        UError_کوډ پلار_حالت = U_ZERO_ERROR;
        UResource_معلومات_ثبت *t2 = init_ثبت(name, t1->f_لار, &پلار_حالت);
        إذا (U_FAILURE(پلار_حالت)) {
            *حالت = پلار_حالت;
            أرجع خطأ;
        }
        UResource_معلومات_ثبت *u2 = تش_اشاره;
        UError_کوډ usr_حالت = U_ZERO_ERROR;
        إذا (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_ثبت(name, usr_معلومات_لار, &usr_حالت);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            إذا (usr_حالت == U_حافظه_ویش_ERROR) {
                *حالت = usr_حالت;
                أرجع خطأ;
            }
        }

        إذا (usingUSRData && U_SUCCESS(usr_حالت) && u2->fBogus == U_ZERO_ERROR) {
            t1->f_پلار = u2;
            u2->f_پلار = t2;
        } وإلا {
            t1->f_پلار = t2;
            إذا (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_شمېر_Existing = 0;
            }
        }
        t1 = t2;
        check_پلار = chopLocale(name) || may_Have_پلار(name);
    }
    أرجع ریښتیا;
}

ساكن UBool  // returns U_SUCCESS(*status)
ورننباسه_ریښه_Bundle(UResource_معلومات_ثبت *&t1, UError_کوډ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع خطأ; }
    UError_کوډ پلار_حالت = U_ZERO_ERROR;
    UResource_معلومات_ثبت *t2 = init_ثبت(k_ریښه_Locale_Name, t1->f_لار, &پلار_حالت);
    إذا (U_FAILURE(پلار_حالت)) {
        *حالت = پلار_حالت;
        أرجع خطأ;
    }
    t1->f_پلار = t2;
    t1 = t2;
    أرجع ریښتیا;
}

ساكن UResource_معلومات_ثبت *ثبت_Open(ثابت حرف* لار, ثابت حرف* localeID,
                                     UResOpenType openType, UError_کوډ* حالت) {
    U_ASSERT(openType != URES_OPEN_مستقیم);
    UError_کوډ int_حالت = U_ZERO_ERROR;
    UResource_معلومات_ثبت *r = تش_اشاره;
    UResource_معلومات_ثبت *t1 = تش_اشاره;
    UBool isDefault = خطأ;
    UBool is_ریښه = خطأ;
    UBool has_Real_معلومات = خطأ;
    UBool hasChopped = ریښتیا;
    UBool usingUSRData = U_USE_USRDATA && ( لار == تش_اشاره || uprv_strncmp(لار,U_ICUDATA_NAME,8) == 0);

    حرف name[ULOC_FULLNAME_ظرفیت];
    حرف usr_معلومات_لار[96];

    initCache(حالت);

    إذا(U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }

    uprv_strncpy(name, localeID, حجم(name) - 1);
    name[حجم(name) - 1] = 0;

    إذا ( usingUSRData ) {
        إذا ( لار == تش_اشاره ) {
            uprv_strcpy(usr_معلومات_لار, U_USRDATA_NAME);
        } وإلا {
            uprv_strncpy(usr_معلومات_لار, لار, حجم(usr_معلومات_لار) - 1);
            usr_معلومات_لار[0] = 'u';
            usr_معلومات_لار[1] = 's';
            usr_معلومات_لار[2] = 'r';
            usr_معلومات_لار[حجم(usr_معلومات_لار) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ثابت حرف *defaultLocale = uloc_getDefault();

    Mutex قلف(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = پیدا_کړه_لومړی_Existing(لار, name, defaultLocale, openType, &is_ریښه, &hasChopped, &isDefault, &int_حالت);

    // If we failed due to out-of-memory, report the failure and exit early.
    إذا (int_حالت == U_حافظه_ویش_ERROR) {
        *حالت = int_حالت;
        اذهب پای;
    }

    إذا(r != تش_اشاره) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_معلومات = ریښتیا;
        إذا ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_کوډ usr_حالت = U_ZERO_ERROR;
            UResource_معلومات_ثبت *u1 = init_ثبت(t1->fName, usr_معلومات_لار, &usr_حالت);
            // If we failed due to out-of-memory, report the failure and exit early.
            إذا (int_حالت == U_حافظه_ویش_ERROR) {
                *حالت = int_حالت;
                اذهب پای;
            }
            إذا ( u1 != تش_اشاره ) {
                إذا(u1->fBogus == U_ZERO_ERROR) {
                    u1->f_پلار = t1;
                    r = u1;
                } وإلا {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_شمېر_Existing = 0;
                }
            }
        }
        إذا ((hasChopped || may_Have_پلار(name)) && !is_ریښه) {
            إذا (!بارول_Parents_Except_ریښه(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_معلومات_لار, حالت)) {
                اذهب پای;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    إذا(r==تش_اشاره && openType == URES_OPEN_LOCALE_DEFAULT_ریښه && !isDefault && !is_ریښه) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = پیدا_کړه_لومړی_Existing(لار, name, defaultLocale, openType, &is_ریښه, &hasChopped, &isDefault, &int_حالت);
        // If we failed due to out-of-memory, report the failure and exit early.
        إذا (int_حالت == U_حافظه_ویش_ERROR) {
            *حالت = int_حالت;
            اذهب پای;
        }
        int_حالت = U_USING_DEFAULT_WARNING;
        إذا(r != تش_اشاره) { /* the default locale exists */
            t1 = r;
            has_Real_معلومات = ریښتیا;
            isDefault = ریښتیا;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            إذا ((hasChopped || may_Have_پلار(name)) && !is_ریښه) {
                إذا (!بارول_Parents_Except_ریښه(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_معلومات_لار, حالت)) {
                    اذهب پای;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    إذا(r == تش_اشاره) {
        uprv_strcpy(name, k_ریښه_Locale_Name);
        r = پیدا_کړه_لومړی_Existing(لار, name, defaultLocale, openType, &is_ریښه, &hasChopped, &isDefault, &int_حالت);
        // If we failed due to out-of-memory, report the failure and exit early.
        إذا (int_حالت == U_حافظه_ویش_ERROR) {
            *حالت = int_حالت;
            اذهب پای;
        }
        إذا(r != تش_اشاره) {
            t1 = r;
            int_حالت = U_USING_DEFAULT_WARNING;
            has_Real_معلومات = ریښتیا;
        } وإلا { /* we don't even have the root locale */
            *حالت = U_MISSING_RESOURCE_ERROR;
            اذهب پای;
        }
    } وإلا إذا(!is_ریښه && uprv_strcmp(t1->fName, k_ریښه_Locale_Name) != 0 &&
            t1->f_پلار == تش_اشاره && !r->f_معلومات.noFallback) {
        إذا (!ورننباسه_ریښه_Bundle(t1, حالت)) {
            اذهب پای;
        }
        إذا(!has_Real_معلومات) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    طالما(r != تش_اشاره && !is_ریښه && t1->f_پلار != تش_اشاره) {
        t1->f_پلار->f_شمېر_Existing++;
        t1 = t1->f_پلار;
    }

پای:
    إذا(U_SUCCESS(*حالت)) {
        إذا(int_حالت != U_ZERO_ERROR) {
            *حالت = int_حالت;  
        }
        أرجع r;
    } وإلا {
        أرجع تش_اشاره;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ساكن UResource_معلومات_ثبت *
ثبت_Open_مستقیم(ثابت حرف* لار, ثابت حرف* localeID, UError_کوډ* حالت) {
    initCache(حالت);
    إذا(U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    إذا (localeID == تش_اشاره) {
        localeID = uloc_getDefault();
    } وإلا إذا (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_ریښه_Locale_Name;
    }

    Mutex قلف(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_معلومات_ثبت *r = init_ثبت(localeID, لار, حالت);
    إذا(U_SUCCESS(*حالت)) {
        إذا(r->fBogus != U_ZERO_ERROR) {
            r->f_شمېر_Existing--;
            r = تش_اشاره;
        }
    } وإلا {
        r = تش_اشاره;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_معلومات_ثبت *t1 = r;
    إذا(r != تش_اشاره && uprv_strcmp(localeID, k_ریښه_Locale_Name) != 0 &&  // not root
            r->f_پلار == تش_اشاره && !r->f_معلومات.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_ظرفیت) {
        حرف name[ULOC_FULLNAME_ظرفیت];
        uprv_strcpy(name, localeID);
        إذا(!chopLocale(name) || uprv_strcmp(name, k_ریښه_Locale_Name) == 0 ||
                بارول_Parents_Except_ریښه(t1, name, UPRV_LENGTHOF(name), خطأ, تش_اشاره, حالت)) {
            إذا(uprv_strcmp(t1->fName, k_ریښه_Locale_Name) != 0 && t1->f_پلار == تش_اشاره) {
                ورننباسه_ریښه_Bundle(t1, حالت);
            }
        }
        إذا(U_FAILURE(*حالت)) {
            r = تش_اشاره;
        }
    }

    إذا(r != تش_اشاره) {
        // TODO: Does this ever loop?
        طالما(t1->f_پلار != تش_اشاره) {
            t1->f_پلار->f_شمېر_Existing++;
            t1 = t1->f_پلار;
        }
    }
    أرجع r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ساكن فراغ ثبت_Close_Int(UResource_معلومات_ثبت *resB) {
    UResource_معلومات_ثبت *p = resB;

    طالما(resB != تش_اشاره) {
        p = resB->f_پلار;
        resB->f_شمېر_Existing--;

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

ساكن فراغ ثبت_Close(UResource_معلومات_ثبت *resB) {
  Mutex قلف(&resbMutex);
  ثبت_Close_Int(resB);
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
ساكن فراغ ures_append_Res_لار(UResourceBundle *resB, ثابت حرف* منزل_جمع_کړه, int32_t len_منزل_جمع_کړه, UError_کوډ *حالت) {
    int32_t res_لار_Len_Orig = resB->f_Res_لار_Len;
    إذا(resB->f_Res_لار == تش_اشاره) {
        resB->f_Res_لار = resB->fResBuf;
        *(resB->f_Res_لار) = 0;
        resB->f_Res_لار_Len = 0;
    } 
    resB->f_Res_لار_Len += len_منزل_جمع_کړه;
    إذا(RES_BUFSIZE <= resB->f_Res_لار_Len+1) {
        إذا(resB->f_Res_لار == resB->fResBuf) {
            resB->f_Res_لار = static_cast<حرف*>(uprv_malloc((resB->f_Res_لار_Len + 1) * حجم(حرف)));
            /* Check that memory was allocated correctly. */
            إذا (resB->f_Res_لار == تش_اشاره) {
                *حالت = U_حافظه_ویش_ERROR;
                أرجع;
            }
            uprv_strcpy(resB->f_Res_لار, resB->fResBuf);
        } وإلا {
            حرف* لنډمهاله = static_cast<حرف*>(uprv_realloc(resB->f_Res_لار, (resB->f_Res_لار_Len + 1) * حجم(حرف)));
            /* Check that memory was reallocated correctly. */
            إذا (لنډمهاله == تش_اشاره) {
                *حالت = U_حافظه_ویش_ERROR;
                أرجع;
            }
            resB->f_Res_لار = لنډمهاله;
        }
    }
    uprv_strcpy(resB->f_Res_لار + res_لار_Len_Orig, منزل_جمع_کړه);
}

ساكن فراغ ures_free_Res_لار(UResourceBundle *resB) {
    إذا (resB->f_Res_لار && resB->f_Res_لار != resB->fResBuf) {
        uprv_free(resB->f_Res_لار);
    }
    resB->f_Res_لار = تش_اشاره;
    resB->f_Res_لار_Len = 0;
}

ساكن فراغ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    إذا(resB != تش_اشاره) {
        إذا(resB->f_معلومات != تش_اشاره) {
            ثبت_Close(resB->f_معلومات);
        }
        إذا(resB->f_نسخه != تش_اشاره) {
            uprv_free(resB->f_نسخه);
        }
        ures_free_Res_لار(resB);

        إذا(ures_is_ډېری_Object(resB) == خطأ && freeBundleObj) {
            uprv_free(resB);
        }
#إذا 0 /*U_DEBUG*/
        وإلا {
            /* poison the data */
            uprv_memset(resB, -1, حجم(UResourceBundle));
        }
#د_که_پای
    }
}

U_CAPI فراغ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, ریښتیا);
}

نطاق {

UResourceBundle* init_resb_پایله(UResource_معلومات_ثبت* معلومات_ثبت,
                                  Resource r,
                                  ثابت حرف* کلی,
                                  int32_t idx,
                                  UResource_معلومات_ثبت* valid_Locale_معلومات_ثبت,
                                  ثابت حرف* container_Res_لار,
                                  int32_t recursion_ژوروالی,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کوډ* حالت);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
get_Alias_موخه_As_Resource_Bundle(ثابت Resource_معلومات& res_معلومات,
                               Resource r,
                               ثابت حرف* کلی,
                               int32_t idx,
                               UResource_معلومات_ثبت* valid_Locale_معلومات_ثبت,
                               ثابت حرف* container_Res_لار,
                               int32_t recursion_ژوروالی,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_کوډ* حالت) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    إذا (U_FAILURE(*حالت)) { أرجع resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ثابت char16_t *alias = res_getAlias(&res_معلومات, r, &len);
    إذا(len <= 0) {
        // bad alias
        *حالت = U_ILLEGAL_دلیل_ERROR;
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
        أرجع تش_اشاره;
    }

    // We have an alias, now let's cut it up.
    ثابت حرف *لار = تش_اشاره, *locale = تش_اشاره, *کلی_لار = تش_اشاره;
    إذا(chAlias[0] == RES_لار_SEPARATOR) {
        // There is a path included.
        حرف *ch_Alias_معلومات = chAlias.معلومات();
        حرف *sep = ch_Alias_معلومات + 1;
        لار = sep;
        sep = uprv_strchr(sep, RES_لار_SEPARATOR);
        إذا(sep != تش_اشاره) {
            *sep++ = 0;
        }
        إذا(uprv_strcmp(لار, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            کلی_لار = sep;
            // Read from the valid locale which we already have.
            لار = locale = تش_اشاره;
        } وإلا {
            إذا(uprv_strcmp(لار, "ICUDATA") == 0) { /* want ICU data */
                لار = تش_اشاره;
            }
            إذا (sep == تش_اشاره) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } وإلا {
                locale = sep;
                sep = uprv_strchr(sep, RES_لار_SEPARATOR);
                إذا(sep != تش_اشاره) {
                    *sep++ = 0;
                }
                کلی_لار = sep;
            }
        }
    } وإلا {
        // No path, start with a locale.
        حرف *sep = chAlias.معلومات();
        locale = sep;
        sep = uprv_strchr(sep, RES_لار_SEPARATOR);
        إذا(sep != تش_اشاره) {
            *sep++ = 0;
        }
        کلی_لار = sep;
        لار = valid_Locale_معلومات_ثبت->f_لار;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    Local_UResource_Bundle_اشاره_ګر mainRes;
    UResource_معلومات_ثبت *معلومات_ثبت;
    إذا (locale == تش_اشاره) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        معلومات_ثبت = valid_Locale_معلومات_ثبت;
    } وإلا {
        UError_کوډ int_حالت = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_مستقیم(لار, locale, &int_حالت));
        إذا(U_FAILURE(int_حالت)) {
            // We failed to open the resource bundle we're aliasing to.
            *حالت = int_حالت;
            أرجع resB;
        }
        معلومات_ثبت = mainRes->f_معلومات;
    }

    ثابت حرف* لنډمهاله = تش_اشاره;
    إذا(کلی_لار == تش_اشاره) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = معلومات_ثبت->f_معلومات.ریښه_Res;
        إذا(container_Res_لار) {
            chAlias.clear().append(container_Res_لار, *حالت);
            إذا (U_FAILURE(*حالت)) {
                أرجع تش_اشاره;
            }
            حرف *a_کلی = chAlias.معلومات();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_پیدا_کړه_Resource(&معلومات_ثبت->f_معلومات, r, &a_کلی, &لنډمهاله);
        }
        إذا(کلی) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(کلی, *حالت);
            إذا (U_FAILURE(*حالت)) {
                أرجع تش_اشاره;
            }
            حرف *a_کلی = chAlias.معلومات();
            r = res_پیدا_کړه_Resource(&معلومات_ثبت->f_معلومات, r, &a_کلی, &لنډمهاله);
        } وإلا إذا(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            إذا(URES_IS_جدول(type)) {
                ثابت حرف *a_کلی;
                r = res_get_جدول_توکی_By_شاخص(&معلومات_ثبت->f_معلومات, r, idx, &a_کلی);
            } وإلا { /* array */
                r = res_get_Array_توکی(&معلومات_ثبت->f_معلومات, r, idx);
            }
        }
        إذا(r != RES_BOGUS) {
            resB = init_resb_پایله(
                معلومات_ثبت, r, لنډمهاله, -1, valid_Locale_معلومات_ثبت, تش_اشاره, recursion_ژوروالی+1,
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
        CharString لار_Buf(کلی_لار, *حالت);
        إذا (U_FAILURE(*حالت)) {
            أرجع تش_اشاره;
        }
        حرف *my_لار = لار_Buf.معلومات();
        container_Res_لار = تش_اشاره;
        // Now we have fallback following here.
        لكل(;;) {
            r = معلومات_ثبت->f_معلومات.ریښه_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            طالما(*my_لار && U_SUCCESS(*حالت)) {
                r = res_پیدا_کړه_Resource(&(معلومات_ثبت->f_معلومات), r, &my_لار, &لنډمهاله);
                إذا(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    اكسر;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_پایله(
                    معلومات_ثبت, r, لنډمهاله, -1,
                    valid_Locale_معلومات_ثبت, container_Res_لار, recursion_ژوروالی+1,
                    resB, حالت);
                إذا (U_FAILURE(*حالت)) {
                    اكسر;
                }
                إذا (لنډمهاله == تش_اشاره || uprv_strcmp(کلی_لار, لنډمهاله) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_لار(resB);
                    ures_append_Res_لار(resB, کلی_لار, static_cast<int32_t>(uprv_strlen(کلی_لار)), حالت);
                    إذا(resB->f_Res_لار[resB->f_Res_لار_Len-1] != RES_لار_SEPARATOR) {
                        ures_append_Res_لار(resB, RES_لار_SEPARATOR_S, 1, حالت);
                    }
                    إذا (U_FAILURE(*حالت)) {
                        اكسر;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                معلومات_ثبت = resB->f_معلومات;
                container_Res_لار = resB->f_Res_لار;
            }
            إذا (U_FAILURE(*حالت) || r != RES_BOGUS) {
                اكسر;
            }
            // Fall back to the parent bundle, if there is one.
            معلومات_ثبت = معلومات_ثبت->f_پلار;
            إذا (معلومات_ثبت == تش_اشاره) {
                *حالت = U_MISSING_RESOURCE_ERROR;
                اكسر;
            }
            // Copy the same keyPath again.
            my_لار = لار_Buf.معلومات();
            uprv_strcpy(my_لار, کلی_لار);
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
UResourceBundle* init_resb_پایله(UResource_معلومات_ثبت* معلومات_ثبت,
                                  Resource r,
                                  ثابت حرف* کلی,
                                  int32_t idx,
                                  UResource_معلومات_ثبت* valid_Locale_معلومات_ثبت,
                                  ثابت حرف* container_Res_لار,
                                  int32_t recursion_ژوروالی,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کوډ* حالت) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)) {
        أرجع resB;
    }
    إذا (valid_Locale_معلومات_ثبت == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    إذا(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        إذا(recursion_ژوروالی >= URES_MAX_ALIAS_LEVEL) {
            *حالت = U_TOO_MANY_ALIASES_ERROR;
            أرجع resB;
        }
        أرجع get_Alias_موخه_As_Resource_Bundle(
            معلومات_ثبت->f_معلومات, r, کلی, idx,
            valid_Locale_معلومات_ثبت, container_Res_لار, recursion_ژوروالی, resB, حالت);
    }
    إذا(resB == تش_اشاره) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        إذا (resB == تش_اشاره) {
            *حالت = U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }
        ures_سېټ_Is_ډېری_Object(resB, خطأ);
        resB->f_Res_لار = تش_اشاره;
        resB->f_Res_لار_Len = 0;
    } وإلا {
        إذا(resB->f_معلومات != تش_اشاره) {
            ثبت_Close(resB->f_معلومات);
        }
        إذا(resB->f_نسخه != تش_اشاره) {
            uprv_free(resB->f_نسخه);
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
        إذا(container_Res_لار != resB->f_Res_لار) {
            ures_free_Res_لار(resB);
        }
    }
    resB->f_معلومات = معلومات_ثبت;
    ثبت_Increase(resB->f_معلومات);
    resB->fHasFallback = خطأ;
    resB->f_Is_سر_Level = خطأ;
    resB->f_شاخص = -1;
    resB->f_کلی = کلی; 
    resB->f_Valid_Locale_معلومات_ثبت = valid_Locale_معلومات_ثبت;
    إذا(container_Res_لار != resB->f_Res_لار) {
        ures_append_Res_لار(
            resB, container_Res_لار, static_cast<int32_t>(uprv_strlen(container_Res_لار)), حالت);
    }
    إذا(کلی != تش_اشاره) {
        ures_append_Res_لار(resB, کلی, static_cast<int32_t>(uprv_strlen(کلی)), حالت);
        إذا(resB->f_Res_لار_Len > 0 && resB->f_Res_لار[resB->f_Res_لار_Len-1] != RES_لار_SEPARATOR) {
            ures_append_Res_لار(resB, RES_لار_SEPARATOR_S, 1, حالت);
        }
    } وإلا إذا(idx >= 0) {
        حرف buf[256];
        int32_t len = T_CString_integer_منزل_String(buf, idx, 10);
        ures_append_Res_لار(resB, buf, len, حالت);
        إذا(resB->f_Res_لار_Len > 0 && resB->f_Res_لار[resB->f_Res_لار_Len-1] != RES_لار_SEPARATOR) {
            ures_append_Res_لار(resB, RES_لار_SEPARATOR_S, 1, حالت);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t کارول_شوی_Len = ((resB->fResBuf == resB->f_Res_لار) ? resB->f_Res_لار_Len : 0);
        uprv_memset(resB->fResBuf + کارول_شوی_Len, 0, حجم(resB->fResBuf) - کارول_شوی_Len);
    }

    resB->f_نسخه = تش_اشاره;
    resB->fRes = r;
    resB->f_اندازه = res_شمېر_Array_توکي(&resB->get_Res_معلومات(), resB->fRes);
    ResourceTracer(resB).trace("get");
    أرجع resB;
}

UResourceBundle* init_resb_پایله(UResource_معلومات_ثبت* معلومات_ثبت,
                                  Resource r,
                                  ثابت حرف* کلی,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ثابت UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کوډ* حالت) {
    أرجع init_resb_پایله(
        معلومات_ثبت, r, کلی, idx,
        container->f_Valid_Locale_معلومات_ثبت, container->f_Res_لار, 0, resB, حالت);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ثابت UResourceBundle* original,
                               UError_کوډ* حالت) {
    UBool is_ډېری_Object;
    إذا(U_FAILURE(*حالت) || r == original) {
        أرجع r;
    }
    إذا(original != تش_اشاره) {
        إذا(r == تش_اشاره) {
            is_ډېری_Object = خطأ;
            r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
            /* test for nullptr */
            إذا (r == تش_اشاره) {
                *حالت = U_حافظه_ویش_ERROR;
                أرجع تش_اشاره;
            }
        } وإلا {
            is_ډېری_Object = ures_is_ډېری_Object(r);
            ures_closeBundle(r, خطأ);
        }
        uprv_memcpy(r, original, حجم(UResourceBundle));
        r->f_Res_لار = تش_اشاره;
        r->f_Res_لار_Len = 0;
        إذا(original->f_Res_لار) {
            ures_append_Res_لار(r, original->f_Res_لار, original->f_Res_لار_Len, حالت);
        }
        ures_سېټ_Is_ډېری_Object(r, is_ډېری_Object);
        إذا(r->f_معلومات != تش_اشاره) {
            ثبت_Increase(r->f_معلومات);
        }
    }
    أرجع r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ثابت char16_t* U_EXPORT2 ures_getString(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_کوډ* حالت) {
    ثابت char16_t *s;
    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا(resB == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    s = res_getString({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
    إذا (s == تش_اشاره) {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ساكن ثابت حرف*
ures_منزل_UTF8_String(ثابت char16_t* s16,
                  int32_t length16,
                  حرف* dest U_LIFETIME_BOUND,
                  int32_t* p_اوږدتیا,
                  UBool forceCopy,
                  UError_کوډ* حالت) {
    int32_t capacity;

    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا (p_اوږدتیا != تش_اشاره) {
        capacity = *p_اوږدتیا;
    } وإلا {
        capacity = 0;
    }
    إذا (capacity < 0 || (capacity > 0 && dest == تش_اشاره)) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا (length16 == 0) {
        /* empty string, return as read-only pointer */
        إذا (p_اوږدتیا != تش_اشاره) {
            *p_اوږدتیا = 0;
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
            أرجع u_str_منزل_UTF8(تش_اشاره, 0, p_اوږدتیا, s16, length16, حالت);
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
            int32_t max_اوږدتیا_2 = 3 * length16 + 1;
            إذا (capacity > max_اوږدتیا_2) {
                dest += capacity - max_اوږدتیا_2;
                capacity = max_اوږدتیا_2;
            }
        }
        أرجع u_str_منزل_UTF8(dest, capacity, p_اوږدتیا, s16, length16, حالت);
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getUTF8String(ثابت UResourceBundle* resB,
                   حرف* dest U_LIFETIME_BOUND,
                   int32_t* p_اوږدتیا,
                   UBool forceCopy,
                   UError_کوډ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getString(resB, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_اوږدتیا, forceCopy, حالت);
}

U_CAPI ثابت uint8_t* U_EXPORT2
ures_get_دوګونی(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_کوډ* حالت) {
  ثابت uint8_t *p;
  إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  إذا(resB == تش_اشاره) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع تش_اشاره;
  }
  p = res_get_دوګونی({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
  إذا (p == تش_اشاره) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

U_CAPI ثابت int32_t* U_EXPORT2
ures_get_Int_ویکټور(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_کوډ* حالت) {
  ثابت int32_t *p;
  إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  إذا(resB == تش_اشاره) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع تش_اشاره;
  }
  p = res_get_Int_ویکټور({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
  إذا (p == تش_اشاره) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ثابت UResourceBundle* resB, UError_کوډ *حالت) {
  إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
    أرجع 0xffffffff;
  }
  إذا(resB == تش_اشاره) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع 0xffffffff;
  }
  إذا(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ثابت UResourceBundle* resB, UError_کوډ *حالت) {
  إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
    أرجع 0xffffffff;
  }
  إذا(resB == تش_اشاره) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع 0xffffffff;
  }
  إذا(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ثابت UResourceBundle *resB) {
  إذا(resB == تش_اشاره) {
    أرجع URES_NONE;
  }
  أرجع res_get_عامه_Type(resB->fRes);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_کلی(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  إذا(resB == تش_اشاره) {
    أرجع تش_اشاره;
  }
  أرجع(resB->f_کلی);
}

U_CAPI int32_t U_EXPORT2 ures_get_اندازه(ثابت UResourceBundle *resB) {
  إذا(resB == تش_اشاره) {
    أرجع 0;
  }
  
  أرجع resB->f_اندازه;
}

ساكن ثابت char16_t*
ures_getStringWithAlias(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_شاخص,
                        int32_t* len,
                        UError_کوډ* حالت) {
  إذا(RES_GET_TYPE(r) == URES_ALIAS) {
    ثابت char16_t* پایله = تش_اشاره;
    UResourceBundle *لنډمهاله_Res = ures_get_By_شاخص(resB, s_شاخص, تش_اشاره, حالت);
    پایله = ures_getString(لنډمهاله_Res, len, حالت);
    ures_close(لنډمهاله_Res);
    أرجع پایله;
  } وإلا {
    أرجع res_getString({resB, s_شاخص}, &resB->get_Res_معلومات(), r, len); 
  }
}

U_CAPI فراغ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  إذا(resB == تش_اشاره) {
    أرجع;
  }
  resB->f_شاخص = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_بل(ثابت UResourceBundle *resB) {
  إذا(resB == تش_اشاره) {
    أرجع خطأ;
  }
  أرجع resB->f_شاخص < resB->f_اندازه-1;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_بل_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ثابت حرف** کلی,
                   UError_کوډ* حالت) {
  Resource r = RES_BOGUS;
  
  إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  إذا(resB == تش_اشاره) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع تش_اشاره;
  }
  
  إذا(resB->f_شاخص == resB->f_اندازه-1) {
    *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
  } وإلا {
    resB->f_شاخص++;
    بدل(RES_GET_TYPE(resB->fRes)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
      أرجع res_getString({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
    حالة URES_جدول:
    حالة URES_TABLE16:
    حالة URES_TABLE32:
      r = res_get_جدول_توکی_By_شاخص(&resB->get_Res_معلومات(), resB->fRes, resB->f_شاخص, کلی);
      إذا(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->f_شاخص, len, حالت);
    حالة URES_ARRAY:
    حالة URES_ARRAY16:
      r = res_get_Array_توکی(&resB->get_Res_معلومات(), resB->fRes, resB->f_شاخص);
      إذا(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->f_شاخص, len, حالت);
    حالة URES_ALIAS:
      أرجع ures_getStringWithAlias(resB, resB->fRes, resB->f_شاخص, len, حالت);
    حالة URES_INT:
    حالة URES_دوګونی:
    حالة URES_INT_ویکټور:
        *حالت = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    افتراضي:
      أرجع تش_اشاره;
    }
  }

  أرجع تش_اشاره;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_بل_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_دننه U_LIFETIME_BOUND,
                     UError_کوډ* حالت) {
    ثابت حرف *کلی = تش_اشاره;
    Resource r = RES_BOGUS;

    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
            /*return nullptr;*/
            أرجع fill_دننه;
    }
    إذا(resB == تش_اشاره) {
            *حالت = U_ILLEGAL_دلیل_ERROR;
            /*return nullptr;*/
            أرجع fill_دننه;
    }

    إذا(resB->f_شاخص == resB->f_اندازه-1) {
      *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } وإلا {
        resB->f_شاخص++;
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_دوګونی:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_ویکټور:
            أرجع ures_copyResb(fill_دننه, resB, حالت);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_توکی_By_شاخص(&resB->get_Res_معلومات(), resB->fRes, resB->f_شاخص, &کلی);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_پایله(resB->f_معلومات, r, کلی, resB->f_شاخص, resB, fill_دننه, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_توکی(&resB->get_Res_معلومات(), resB->fRes, resB->f_شاخص);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_پایله(resB->f_معلومات, r, کلی, resB->f_شاخص, resB, fill_دننه, حالت);
        افتراضي:
            /*return nullptr;*/
            أرجع fill_دننه;
        }
    }
    /*return nullptr;*/
    أرجع fill_دننه;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_شاخص(ثابت UResourceBundle* resB,
                int32_t شاخص_R,
                UResourceBundle* fill_دننه U_LIFETIME_BOUND,
                UError_کوډ* حالت) {
    ثابت حرف* کلی = تش_اشاره;
    Resource r = RES_BOGUS;

    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        /*return nullptr;*/
        أرجع fill_دننه;
    }
    إذا(resB == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        /*return nullptr;*/
        أرجع fill_دننه;
    }

    إذا(شاخص_R >= 0 && resB->f_اندازه > شاخص_R) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_دوګونی:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_ویکټور:
            أرجع ures_copyResb(fill_دننه, resB, حالت);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_توکی_By_شاخص(&resB->get_Res_معلومات(), resB->fRes, شاخص_R, &کلی);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_پایله(resB->f_معلومات, r, کلی, شاخص_R, resB, fill_دننه, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_توکی(&resB->get_Res_معلومات(), resB->fRes, شاخص_R);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_پایله(resB->f_معلومات, r, کلی, شاخص_R, resB, fill_دننه, حالت);
        افتراضي:
            /*return nullptr;*/
            أرجع fill_دننه;
        }
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    أرجع fill_دننه;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_شاخص(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t شاخص_S,
                      int32_t* len,
                      UError_کوډ* حالت) {
    ثابت حرف* کلی = تش_اشاره;
    Resource r = RES_BOGUS;

    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا(resB == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا(شاخص_S >= 0 && resB->f_اندازه > شاخص_S) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_STRING:
        حالة URES_STRING_V2:
            أرجع res_getString({resB}, &resB->get_Res_معلومات(), resB->fRes, len);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_توکی_By_شاخص(&resB->get_Res_معلومات(), resB->fRes, شاخص_S, &کلی);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, شاخص_S, len, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_توکی(&resB->get_Res_معلومات(), resB->fRes, شاخص_S);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, شاخص_S, len, حالت);
        حالة URES_ALIAS:
            أرجع ures_getStringWithAlias(resB, resB->fRes, شاخص_S, len, حالت);
        حالة URES_INT:
        حالة URES_دوګونی:
        حالة URES_INT_ویکټور:
            *حالت = U_RESOURCE_TYPE_MISMATCH;
            اكسر;
        افتراضي:
          /* must not occur */
          *حالت = U_دننی_پروګرام_ERROR;
          اكسر;
        }
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    أرجع تش_اشاره;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_شاخص(ثابت UResourceBundle* resB,
                          int32_t idx,
                          حرف* dest U_LIFETIME_BOUND,
                          int32_t* p_اوږدتیا,
                          UBool forceCopy,
                          UError_کوډ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_شاخص(resB, idx, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_اوږدتیا, forceCopy, حالت);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_پیدا_کړه_Resource(ثابت حرف* لار, UResourceBundle* fill_دننه U_LIFETIME_BOUND, UError_کوډ* حالت)
{
  UResourceBundle *first = تش_اشاره; 
  UResourceBundle *پایله = fill_دننه;
  حرف *packageName = تش_اشاره;
  حرف *لار_منزل_Resource = تش_اشاره, *save = تش_اشاره;
  حرف *locale = تش_اشاره, *locale_پای_ټکی = تش_اشاره;
  int32_t length;

  إذا(حالت == تش_اشاره || U_FAILURE(*حالت)) {
    أرجع پایله;
  }

  length = (int32_t)(uprv_strlen(لار)+1);
  save = لار_منزل_Resource = (حرف *)uprv_malloc(length*حجم(حرف));
  /* test for nullptr */
  إذا(لار_منزل_Resource == تش_اشاره) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع پایله;
  }
  uprv_memcpy(لار_منزل_Resource, لار, length);

  locale = لار_منزل_Resource;
  إذا(*لار_منزل_Resource == RES_لار_SEPARATOR) { /* there is a path specification */
    لار_منزل_Resource++;
    packageName = لار_منزل_Resource;
    لار_منزل_Resource = uprv_strchr(لار_منزل_Resource, RES_لار_SEPARATOR);
    إذا(لار_منزل_Resource == تش_اشاره) {
      *حالت = U_ILLEGAL_دلیل_ERROR;
    } وإلا {
      *لار_منزل_Resource = 0;
      locale = لار_منزل_Resource+1;
    }
  }

  locale_پای_ټکی = uprv_strchr(locale, RES_لار_SEPARATOR);
  إذا(locale_پای_ټکی != تش_اشاره) {
    *locale_پای_ټکی = 0;
  }

  first = ures_open(packageName, locale, حالت);

  إذا(U_SUCCESS(*حالت)) {
    إذا(locale_پای_ټکی) {
      پایله = ures_پیدا_کړه_Sub_Resource(first, locale_پای_ټکی+1, fill_دننه, حالت);
    } وإلا {
      پایله = ures_copyResb(fill_دننه, first, حالت);
    }
    ures_close(first);
  }
  uprv_free(save);
  أرجع پایله;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_پیدا_کړه_Sub_Resource(ثابت UResourceBundle* resB,
                     حرف* لار,
                     UResourceBundle* fill_دننه U_LIFETIME_BOUND,
                     UError_کوډ* حالت)
{
  Resource res = RES_BOGUS;
  UResourceBundle *پایله = fill_دننه;
  ثابت حرف *کلی;

  إذا(حالت == تش_اشاره || U_FAILURE(*حالت)) {
    أرجع پایله;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  افعل {
    res = res_پیدا_کړه_Resource(&resB->get_Res_معلومات(), resB->fRes, &لار, &کلی); 
    إذا(res != RES_BOGUS) {
        پایله = init_resb_پایله(resB->f_معلومات, res, کلی, -1, resB, fill_دننه, حالت);
        resB = پایله;
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
        اكسر;
    }
  } طالما(*لار); /* there is more stuff in the path */

  أرجع پایله;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلی_With_Fallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت حرف* دننه_کلی,
                                int32_t* len,
                                UError_کوډ* حالت) {
    UResourceBundle ډېری_2;
    ثابت char16_t* retVal = تش_اشاره;
    ures_init_ډېری_Object(&ډېری_2);
    ures_get_By_کلی_With_Fallback(resB, دننه_کلی, &ډېری_2, حالت);
    int32_t length;
    retVal = ures_getString(&ډېری_2, &length, حالت);
    ures_close(&ډېری_2);
    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا (length == 3 && retVal[0] == تشه_سېټ && retVal[1] == تشه_سېټ && retVal[2] == تشه_سېټ ) {
        retVal = تش_اشاره;
        length = 0;
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    إذا (len != تش_اشاره) {
        *len = length;
    }
    أرجع retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ساكن Resource get_جدول_توکی_By_کلی_لار(ثابت Resource_معلومات *p_Res_معلومات, Resource جدول, ثابت حرف *کلی) {
  Resource resource = جدول;  /* The current resource */
  icu::CharString لار;
  UError_کوډ error_کوډ = U_ZERO_ERROR;
  لار.append(کلی, error_کوډ);
  إذا (U_FAILURE(error_کوډ)) { أرجع RES_BOGUS; }
  حرف *لار_Part = لار.معلومات();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  طالما (*لار_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    حرف *بل_لار_Part = uprv_strchr(لار_Part, RES_لار_SEPARATOR);
    إذا (بل_لار_Part != تش_اشاره) {
      *بل_لار_Part = 0;  /* Terminating null for this part of path. */
      بل_لار_Part++;
    } وإلا {
      بل_لار_Part = uprv_strchr(لار_Part, 0);
    }
    int32_t t;
    ثابت حرف *لار_P = لار_Part;
    resource = res_get_جدول_توکی_By_کلی(p_Res_معلومات, resource, &t, &لار_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    لار_Part = بل_لار_Part; 
  }
  إذا (*لار_Part) {
    أرجع RES_BOGUS;
  }
  أرجع resource;
}

ساكن فراغ create_لار(ثابت حرف* orig_Res_لار,
                       int32_t     orig_Res_لار_Len,
                       ثابت حرف* res_لار,
                       int32_t     res_لار_Len,
                       ثابت حرف* دننه_کلی,
                       CharString& لار,
                       UError_کوډ* حالت) {
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
    لار.clear();
    ثابت حرف* کلی = دننه_کلی;
    إذا (res_لار_Len > 0) {
        لار.append(res_لار, res_لار_Len, *حالت);
        إذا (U_SUCCESS(*حالت)) {
            ثابت حرف* res_لار_حد = res_لار + res_لار_Len;
            ثابت حرف* orig_Res_لار_حد = orig_Res_لار + orig_Res_لار_Len;
            ثابت حرف* res_لار_Ptr = res_لار;
            ثابت حرف* orig_Res_لار_Ptr = orig_Res_لار;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            طالما (orig_Res_لار_Ptr < orig_Res_لار_حد && res_لار_Ptr < res_لار_حد) {
                طالما (orig_Res_لار_Ptr < orig_Res_لار_حد && *orig_Res_لار_Ptr != RES_لار_SEPARATOR) {
                    ++orig_Res_لار_Ptr;
                }
                إذا (orig_Res_لار_Ptr < orig_Res_لار_حد && *orig_Res_لار_Ptr == RES_لار_SEPARATOR) {
                    ++orig_Res_لار_Ptr;
                }
                طالما (res_لار_Ptr < res_لار_حد && *res_لار_Ptr != RES_لار_SEPARATOR) {
                    ++res_لار_Ptr;
                }
                إذا (res_لار_Ptr < res_لار_حد && *res_لار_Ptr == RES_لار_SEPARATOR) {
                    ++res_لار_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            طالما (res_لار_Ptr < res_لار_حد && *کلی != '\0') {
                طالما (res_لار_Ptr < res_لار_حد && *res_لار_Ptr != RES_لار_SEPARATOR) {
                    ++res_لار_Ptr;
                }
                إذا (res_لار_Ptr < res_لار_حد && *res_لار_Ptr == RES_لار_SEPARATOR) {
                    ++res_لار_Ptr;
                }
                طالما (*کلی != '\0' && *کلی != RES_لار_SEPARATOR) {
                    ++کلی;
                }
                إذا (*کلی == RES_لار_SEPARATOR) {
                    ++کلی;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        لار.append(کلی, *حالت);
    } وإلا {
        لار.append(دننه_کلی, *حالت);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلی_With_Fallback(ثابت UResourceBundle* resB,
                          ثابت حرف* دننه_کلی,
                          UResourceBundle* fill_دننه U_LIFETIME_BOUND,
                          UError_کوډ* حالت) {
    Resource res = RES_BOGUS, ریښه_Res = RES_BOGUS;
    UResourceBundle *helper = تش_اشاره;

    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع fill_دننه;
    }
    إذا(resB == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع fill_دننه;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        ثابت حرف* orig_Res_لار = resB->f_Res_لار;
        int32_t orig_Res_لار_Len = resB->f_Res_لار_Len;
        res = get_جدول_توکی_By_کلی_لار(&resB->get_Res_معلومات(), resB->fRes, دننه_کلی);
        ثابت حرف* کلی = دننه_کلی;
        منطقي did_ریښه_Once = خطأ;
        إذا(res == RES_BOGUS) {
            UResource_معلومات_ثبت *معلومات_ثبت = resB->f_معلومات;
            CharString لار;
            حرف *my_لار = تش_اشاره;
            ثابت حرف* res_لار = resB->f_Res_لار;
            int32_t len = resB->f_Res_لار_Len;
            طالما(res == RES_BOGUS && (معلومات_ثبت->f_پلار != تش_اشاره || !did_ریښه_Once)) { /* Otherwise, we'll look in parents */
                إذا (معلومات_ثبت->f_پلار != تش_اشاره) {
                    معلومات_ثبت = معلومات_ثبت->f_پلار;
                } وإلا {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_ریښه_Once = ریښتیا;
                }
                ریښه_Res = معلومات_ثبت->f_معلومات.ریښه_Res;

                إذا(معلومات_ثبت->fBogus == U_ZERO_ERROR) {
                    create_لار(orig_Res_لار, orig_Res_لار_Len, res_لار, len, دننه_کلی, لار, حالت);
                    إذا (U_FAILURE(*حالت)) {
                        ures_close(helper);
                        أرجع fill_دننه;
                    }
                    my_لار = لار.معلومات();
                    کلی = دننه_کلی;
                    افعل {
                        res = res_پیدا_کړه_Resource(&(معلومات_ثبت->f_معلومات), ریښه_Res, &my_لار, &کلی);
                        إذا (RES_GET_TYPE(res) == URES_ALIAS && *my_لار) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_پایله(معلومات_ثبت, res, تش_اشاره, -1, resB, helper, حالت);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            إذا(helper) {
                              معلومات_ثبت = helper->f_معلومات;
                              ریښه_Res = helper->fRes;
                              res_لار = helper->f_Res_لار;
                              len = helper->f_Res_لار_Len;

                            } وإلا {
                              اكسر;
                            }
                        } وإلا إذا (res == RES_BOGUS) {
                            اكسر;
                        }
                    } طالما(*my_لار); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            إذا(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                إذا(uprv_strcmp(معلومات_ثبت->fName, uloc_getDefault())==0 || uprv_strcmp(معلومات_ثبت->fName, k_ریښه_Locale_Name)==0) {
                    *حالت = U_USING_DEFAULT_WARNING;
                } وإلا {
                    *حالت = U_USING_FALLBACK_WARNING;
                }

                fill_دننه = init_resb_پایله(معلومات_ثبت, res, کلی, -1, resB, fill_دننه, حالت);
                إذا (res_لار != تش_اشاره) {
                    create_لار(orig_Res_لار, orig_Res_لار_Len, res_لار, len, دننه_کلی, لار, حالت);
                } وإلا {
                    ثابت حرف* separator = تش_اشاره;
                    إذا (fill_دننه->f_Res_لار != تش_اشاره) {
                        separator = uprv_strchr(fill_دننه->f_Res_لار, RES_لار_SEPARATOR);
                    }
                    إذا (separator != تش_اشاره && separator[1] != '\0') {
                        create_لار(orig_Res_لار, orig_Res_لار_Len, fill_دننه->f_Res_لار,
                                   static_cast<int32_t>(uprv_strlen(fill_دننه->f_Res_لار)), دننه_کلی, لار, حالت);
                    } وإلا {
                        create_لار(orig_Res_لار, orig_Res_لار_Len, "", 0, دننه_کلی, لار, حالت);
                    }
                }
                ures_free_Res_لار(fill_دننه);
                ures_append_Res_لار(fill_دننه, لار.معلومات(), لار.length(), حالت);
                إذا(fill_دننه->f_Res_لار[fill_دننه->f_Res_لار_Len-1] != RES_لار_SEPARATOR) {
                    ures_append_Res_لار(fill_دننه, RES_لار_SEPARATOR_S, 1, حالت);
                }
            } وإلا {
                *حالت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            fill_دننه = init_resb_پایله(resB->f_معلومات, res, کلی, -1, resB, fill_دننه, حالت);
        }
    } 
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    أرجع fill_دننه;
}

نطاق {

فراغ get_All_توکي_With_Fallback(
        ثابت UResourceBundle *bundle, Resource_معلومات_ارزښت &value,
        ResourceSink &sink, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
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
    value.سېټ_معلومات(bundle->get_Res_معلومات());
    value.سېټ_Valid_Locale_معلومات_ثبت(bundle->f_Valid_Locale_معلومات_ثبت);
    UResource_معلومات_ثبت *پلار_ثبت = bundle->f_معلومات->f_پلار;
    UBool has_پلار = پلار_ثبت != تش_اشاره && U_SUCCESS(پلار_ثبت->fBogus);
    value.سېټ_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_کلی, value, !has_پلار, error_کوډ);
    إذا (has_پلار) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ډېری_UResource_Bundle پلار_Bundle;
        UResourceBundle &پلار_Ref = پلار_Bundle.ref();
        پلار_Ref.f_معلومات = پلار_ثبت;
        پلار_Ref.f_Valid_Locale_معلومات_ثبت = bundle->f_Valid_Locale_معلومات_ثبت;
        پلار_Ref.fHasFallback = !پلار_Ref.get_Res_معلومات().noFallback;
        پلار_Ref.f_Is_سر_Level = ریښتیا;
        پلار_Ref.fRes = پلار_Ref.get_Res_معلومات().ریښه_Res;
        پلار_Ref.f_اندازه = res_شمېر_Array_توکي(&پلار_Ref.get_Res_معلومات(), پلار_Ref.fRes);
        پلار_Ref.f_شاخص = -1;
        ثبت_Increase(پلار_ثبت);

        // Look up the container item in the parent bundle.
        ډېری_UResource_Bundle containerBundle;
        ثابت UResourceBundle *rb;
        UError_کوډ لار_Error_کوډ = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        إذا (bundle->f_Res_لار == تش_اشاره || *bundle->f_Res_لار == 0) {
            rb = پلار_Bundle.getAlias();
        } وإلا {
            rb = ures_get_By_کلی_With_Fallback(پلار_Bundle.getAlias(), bundle->f_Res_لار,
                                           containerBundle.getAlias(), &لار_Error_کوډ);
        }
        إذا (U_SUCCESS(لار_Error_کوډ)) {
            get_All_توکي_With_Fallback(rb, value, sink, error_کوډ);
        }
    }
}

بنية GetAllChildrenSink : عام ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    مجازي ~GetAllChildrenSink() override;
    مجازي فراغ put(ثابت حرف *کلی, Resource_ارزښت &value, UBool is_ریښه,
           UError_کوډ &error_کوډ) override {
        Resource_جدول توکي_جدول = value.get_جدول(error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        لكل (int32_t i = 0; توکي_جدول.get_کلی_And_ارزښت(i, کلی, value); ++i) {
            إذا (value.getType() == URES_ALIAS) {
                Resource_معلومات_ارزښت& rdv = static_cast<Resource_معلومات_ارزښت&>(value);
                ډېری_UResource_Bundle ډېری_لنډمهاله_Bundle;
                UResourceBundle* aliasRB = get_Alias_موخه_As_Resource_Bundle(rdv.get_معلومات(), rdv.getResource(), تش_اشاره, -1,
                                                                          rdv.get_Valid_Locale_معلومات_ثبت(), تش_اشاره, 0,
                                                                          ډېری_لنډمهاله_Bundle.getAlias(), &error_کوډ);
                إذا (U_SUCCESS(error_کوډ)) {
                    Resource_معلومات_ارزښت aliased_ارزښت;
                    aliased_ارزښت.سېټ_معلومات(aliasRB->get_Res_معلومات());
                    aliased_ارزښت.سېټ_Valid_Locale_معلومات_ثبت(aliasRB->f_Valid_Locale_معلومات_ثبت);
                    aliased_ارزښت.سېټ_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    إذا (aliased_ارزښت.getType() != URES_جدول) {
                        dest.put(کلی, aliased_ارزښت, is_ریښه, error_کوډ);
                    } وإلا {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_ارزښت_Type = URES_جدول;
                        CharString جدول_لار;
                        جدول_لار.append(aliasRB->f_Res_لار, error_کوډ);
                        ثابت حرف* پلار_کلی = کلی; // dest.put() changes the key
                        dest.put(پلار_کلی, aliased_ارزښت, is_ریښه, error_کوډ);
                        UResource_معلومات_ثبت* ثبت = aliasRB->f_معلومات;
                        Resource res = aliasRB->fRes;
                        طالما (aliased_ارزښت_Type == URES_جدول && ثبت->f_پلار != تش_اشاره) {
                            CharString local_لار;
                            local_لار.copyFrom(جدول_لار, error_کوډ);
                            حرف* local_لار_As_Char_Ptr = local_لار.معلومات();
                            ثابت حرف* ماشوم_کلی;
                            ثبت = ثبت->f_پلار;
                            res = ثبت->f_معلومات.ریښه_Res;
                            Resource newRes = res_پیدا_کړه_Resource(&ثبت->f_معلومات, res, &local_لار_As_Char_Ptr, &ماشوم_کلی);
                            إذا (newRes != RES_BOGUS) {
                                aliased_ارزښت.سېټ_معلومات(ثبت->f_معلومات);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_ارزښت.سېټ_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_ارزښت_Type = aliased_ارزښت.getType();
                                إذا (aliased_ارزښت_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_معلومات_ارزښت& rdv2 = static_cast<Resource_معلومات_ارزښت&>(aliased_ارزښت);
                                    aliasRB = get_Alias_موخه_As_Resource_Bundle(rdv2.get_معلومات(), rdv2.getResource(), تش_اشاره, -1,
                                                                             rdv2.get_Valid_Locale_معلومات_ثبت(), تش_اشاره, 0,
                                                                             ډېری_لنډمهاله_Bundle.getAlias(), &error_کوډ);
                                    جدول_لار.clear();
                                    جدول_لار.append(aliasRB->f_Res_لار, error_کوډ);
                                    ثبت = aliasRB->f_معلومات;
                                    res = aliasRB->fRes;
                                    aliased_ارزښت.سېټ_معلومات(ثبت->f_معلومات);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_ارزښت.سېټ_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_ارزښت_Type = aliased_ارزښت.getType();
                                }
                                إذا (aliased_ارزښت_Type == URES_جدول) {
                                    dest.put(پلار_کلی, aliased_ارزښت, is_ریښه, error_کوډ);
                                } وإلا {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_کوډ = U_دننی_پروګرام_ERROR;
                                    أرجع;
                                }
                            }
                        }
                    }
                }
            } وإلا {
                dest.put(کلی, value, is_ریښه, error_کوډ);
            }
            إذا (U_FAILURE(error_کوډ)) { أرجع; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI فراغ U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *لار,
                                icu::ResourceSink &sink, UError_کوډ &error_کوډ) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_توکي_With_Fallback(bundle, لار, allChildrenSink, error_کوډ);
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
ures_get_ارزښت_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *لار,
                          UResourceBundle *لنډمهاله_Fill_دننه,
                          Resource_معلومات_ارزښت &value, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    إذا (لار == تش_اشاره) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ثابت UResourceBundle *rb;
    إذا (*لار == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_get_By_کلی_With_Fallback(bundle, لار, لنډمهاله_Fill_دننه, &error_کوډ);
        إذا (U_FAILURE(error_کوډ)) {
            أرجع;
        }
    }
    value.سېټ_معلومات(rb->get_Res_معلومات());
    value.سېټ_Valid_Locale_معلومات_ثبت(rb->f_Valid_Locale_معلومات_ثبت);
    value.سېټ_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI فراغ U_EXPORT2
ures_get_All_توکي_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *لار,
                             icu::ResourceSink &sink, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    إذا (لار == تش_اشاره) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ډېری_UResource_Bundle ډېری_Bundle;
    ثابت UResourceBundle *rb;
    إذا (*لار == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_get_By_کلی_With_Fallback(bundle, لار, ډېری_Bundle.getAlias(), &error_کوډ);
        إذا (U_FAILURE(error_کوډ)) {
            أرجع;
        }
    }
    // Get all table items with fallback.
    Resource_معلومات_ارزښت value;
    get_All_توکي_With_Fallback(rb, value, sink, error_کوډ);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلی(ثابت UResourceBundle* resB,
              ثابت حرف* دننه_کلی,
              UResourceBundle* fill_دننه U_LIFETIME_BOUND,
              UError_کوډ* حالت) {
    Resource res = RES_BOGUS;
    UResource_معلومات_ثبت *معلومات_ثبت = تش_اشاره;
    ثابت حرف *کلی = دننه_کلی;

    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع fill_دننه;
    }
    إذا(resB == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع fill_دننه;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        int32_t t;
        res = res_get_جدول_توکی_By_کلی(&resB->get_Res_معلومات(), resB->fRes, &t, &کلی);
        إذا(res == RES_BOGUS) {
            کلی = دننه_کلی;
            إذا(resB->fHasFallback) {
                معلومات_ثبت = get_Fallback_معلومات(resB, &کلی, &res, حالت);
                إذا(U_SUCCESS(*حالت)) {
                    /* check if resB->fResPath gives the right name here */
                    أرجع init_resb_پایله(معلومات_ثبت, res, کلی, -1, resB, fill_دننه, حالت);
                } وإلا {
                    *حالت = U_MISSING_RESOURCE_ERROR;
                }
            } وإلا {
                *حالت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            أرجع init_resb_پایله(resB->f_معلومات, res, کلی, -1, resB, fill_دننه, حالت);
        }
    } 
#إذا 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    وإلا إذا(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ریښتیا) {
        /* here should go a first attempt to locate the key using index table */
        معلومات_ثبت = get_Fallback_معلومات(resB, &کلی, &res, حالت);
        إذا(U_SUCCESS(*حالت)) {
            أرجع init_resb_پایله(معلومات_ثبت, res, کلی, resB, fill_دننه, حالت);
        } وإلا {
            *حالت = U_MISSING_RESOURCE_ERROR;
        }
    }
#د_که_پای    
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع fill_دننه;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلی(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                    ثابت حرف* دننه_کلی,
                    int32_t* len,
                    UError_کوډ* حالت) {
    Resource res = RES_BOGUS;
    UResource_معلومات_ثبت *معلومات_ثبت = تش_اشاره;
    ثابت حرف* کلی = دننه_کلی;

    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا(resB == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        int32_t t=0;

        res = res_get_جدول_توکی_By_کلی(&resB->get_Res_معلومات(), resB->fRes, &t, &کلی);

        إذا(res == RES_BOGUS) {
            کلی = دننه_کلی;
            إذا(resB->fHasFallback) {
                معلومات_ثبت = get_Fallback_معلومات(resB, &کلی, &res, حالت);
                إذا(U_SUCCESS(*حالت)) {
                    بدل (RES_GET_TYPE(res)) {
                    حالة URES_STRING:
                    حالة URES_STRING_V2:
                        أرجع res_getString({resB, کلی}, &معلومات_ثبت->f_معلومات, res, len);
                    حالة URES_ALIAS:
                      {
                        ثابت char16_t* پایله = تش_اشاره;
                        UResourceBundle *لنډمهاله_Res = ures_get_By_کلی(resB, دننه_کلی, تش_اشاره, حالت);
                        پایله = ures_getString(لنډمهاله_Res, len, حالت);
                        ures_close(لنډمهاله_Res);
                        أرجع پایله;
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
                أرجع res_getString({resB, کلی}, &resB->get_Res_معلومات(), res, len);
            حالة URES_ALIAS:
              {
                ثابت char16_t* پایله = تش_اشاره;
                UResourceBundle *لنډمهاله_Res = ures_get_By_کلی(resB, دننه_کلی, تش_اشاره, حالت);
                پایله = ures_getString(لنډمهاله_Res, len, حالت);
                ures_close(لنډمهاله_Res);
                أرجع پایله;
              }
            افتراضي:
                *حالت = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#إذا 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    وإلا إذا(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == ریښتیا) {
        /* here should go a first attempt to locate the key using index table */
        معلومات_ثبت = get_Fallback_معلومات(resB, &کلی, &res, حالت);
        إذا(U_SUCCESS(*حالت)) {
            // TODO: Tracing
            أرجع res_getString(rd, res, len);
        } وإلا {
            *حالت = U_MISSING_RESOURCE_ERROR;
        }
    } 
#د_که_پای    
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع تش_اشاره;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_کلی(ثابت UResourceBundle* resB,
                        ثابت حرف* کلی,
                        حرف* dest U_LIFETIME_BOUND,
                        int32_t* p_اوږدتیا,
                        UBool forceCopy,
                        UError_کوډ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_کلی(resB, کلی, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_اوږدتیا, forceCopy, حالت);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ثابت حرف* U_EXPORT2
ures_get_Locale_دننی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_کوډ* حالت)
{
    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا (!resourceBundle) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    } وإلا {
      أرجع resourceBundle->f_معلومات->fName;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocale(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_کوډ* حالت)
{
  أرجع ures_get_Locale_دننی(resourceBundle, حالت);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocaleByType(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_معلومات_Locale_Type type,
                     UError_کوډ* حالت) {
    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا (!resourceBundle) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    } وإلا {
        بدل(type) {
        حالة ULOC_ACTUAL_LOCALE:
            أرجع resourceBundle->f_معلومات->fName;
        حالة ULOC_VALID_LOCALE:
            أرجع resourceBundle->f_Valid_Locale_معلومات_ثبت->fName;
        حالة ULOC_REQUESTED_LOCALE:
        افتراضي:
            *حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع تش_اشاره;
        }
    }
}

U_CFUNC ثابت حرف*
ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  إذا(resB == تش_اشاره) {
    أرجع تش_اشاره;
  }

  أرجع resB->f_معلومات->fName;
}

#که_تعریف_شوی URES_DEBUG
U_CFUNC ثابت حرف*
ures_get_لار(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  إذا(resB == تش_اشاره) {
    أرجع تش_اشاره;
  }

  أرجع resB->f_معلومات->f_لار;
}
#د_که_پای

ساكن UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ثابت حرف* لار,
                  ثابت حرف* localeID,
                  UResOpenType openType,
                  UError_کوډ* حالت) {
    إذا(U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }

    UResource_معلومات_ثبت *ثبت;
    إذا(openType != URES_OPEN_مستقیم) {
        إذا (localeID == تش_اشاره) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_بنسټ_Name(localeID, *حالت);
        إذا(U_FAILURE(*حالت)) {
            *حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع تش_اشاره;
        }
        ثبت = ثبت_Open(لار, canonLocaleID.معلومات(), openType, حالت);
    } وإلا {
        ثبت = ثبت_Open_مستقیم(لار, localeID, حالت);
    }
    إذا(U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا(ثبت == تش_اشاره) {
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع تش_اشاره;
    }

    UBool is_ډېری_Object;
    إذا(r == تش_اشاره) {
        r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        إذا(r == تش_اشاره) {
            ثبت_Close(ثبت);
            *حالت = U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }
        is_ډېری_Object = خطأ;
    } وإلا {  // fill-in
        is_ډېری_Object = ures_is_ډېری_Object(r);
        ures_closeBundle(r, خطأ);
    }
    uprv_memset(r, 0, حجم(UResourceBundle));
    ures_سېټ_Is_ډېری_Object(r, is_ډېری_Object);

    r->f_Valid_Locale_معلومات_ثبت = r->f_معلومات = ثبت;
    r->fHasFallback = openType != URES_OPEN_مستقیم && !r->get_Res_معلومات().noFallback;
    r->f_Is_سر_Level = ریښتیا;
    r->fRes = r->get_Res_معلومات().ریښه_Res;
    r->f_اندازه = res_شمېر_Array_توکي(&r->get_Res_معلومات(), r->fRes);
    r->f_شاخص = -1;

    ResourceTracer(r).traceOpen();

    أرجع r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ثابت حرف* لار, ثابت حرف* localeID, UError_کوډ* حالت) {
    أرجع ures_openWithType(تش_اشاره, لار, localeID, URES_OPEN_LOCALE_DEFAULT_ریښه, حالت);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت حرف* لار, ثابت حرف* localeID, UError_کوډ* حالت) {
    أرجع ures_openWithType(تش_اشاره, لار, localeID, URES_OPEN_LOCALE_ریښه, حالت);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_مستقیم(ثابت حرف* لار, ثابت حرف* localeID, UError_کوډ* حالت) {
    أرجع ures_openWithType(تش_اشاره, لار, localeID, URES_OPEN_مستقیم, حالت);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_open_Fill_دننه(UResourceBundle *r, ثابت حرف* لار,
                ثابت حرف* localeID, UError_کوډ* حالت) {
    إذا(U_SUCCESS(*حالت) && r == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ures_openWithType(r, لار, localeID, URES_OPEN_LOCALE_DEFAULT_ریښه, حالت);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_open_مستقیم_Fill_دننه(UResourceBundle *r, ثابت حرف* لار, ثابت حرف* localeID, UError_کوډ* حالت) {
    إذا(U_SUCCESS(*حالت) && r == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ures_openWithType(r, لار, localeID, URES_OPEN_مستقیم, حالت);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_شمېر_Array_توکي(ثابت UResourceBundle* resourceBundle,
                  ثابت حرف* resource_کلی,
                  UError_کوډ* حالت)
{
    UResourceBundle res_معلومات;
    ures_init_ډېری_Object(&res_معلومات);
    إذا (حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(resourceBundle == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    ures_get_By_کلی(resourceBundle, resource_کلی, &res_معلومات, حالت);
    
    إذا(res_معلومات.get_Res_معلومات().معلومات != تش_اشاره) {
        int32_t پایله = res_شمېر_Array_توکي(&res_معلومات.get_Res_معلومات(), res_معلومات.fRes);
        ures_close(&res_معلومات);
        أرجع پایله;
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
ures_get_نسخه_شمېره_دننی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    إذا (!resourceBundle) أرجع تش_اشاره;

    إذا(resourceBundle->f_نسخه == تش_اشاره) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_کوډ حالت = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ثابت char16_t* minor_نسخه = ures_get_String_By_کلی(resourceBundle, k_نسخه_نښان, &minor_len, &حالت);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_نسخه = (حرف *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        إذا (((UResourceBundle *)resourceBundle)->f_نسخه == تش_اشاره) {
            أرجع تش_اشاره;
        }
       
        إذا(minor_len > 0) {
            u_UChars_منزل_Chars(minor_نسخه, resourceBundle->f_نسخه , minor_len);
            resourceBundle->f_نسخه[len] =  '\0';
        }
        وإلا {
            uprv_strcpy(resourceBundle->f_نسخه, k_Default_Minor_نسخه);
        }
    }

    أرجع resourceBundle->f_نسخه;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_نسخه_شمېره(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    أرجع ures_get_نسخه_شمېره_دننی(resourceBundle);
}

U_CAPI فراغ U_EXPORT2 ures_get_نسخه(ثابت UResourceBundle* resB, UVersionInfo نسخه_Info) {
    إذا (!resB) أرجع;

    u_نسخه_From_String(نسخه_Info, ures_get_نسخه_شمېره_دننی(resB));
}

/** Tree support functions *******************************/
#تعریف_کړه شاخص_LOCALE_NAME "res_index"
#تعریف_کړه شاخص_نښان         "InstalledLocales"
#تعریف_کړه DEFAULT_نښان       "default"

#إذا defined(URES_ونه_DEBUG)
#ضمن <stdio.h>
#د_که_پای

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
ures_loc_شمېر_Locales(UEnumeration *en, UError_کوډ * /*حالت*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    أرجع ures_get_اندازه(&ctx->installed);
}

U_CDECL_BEGIN


ساكن ثابت حرف * U_CALLCONV
ures_loc_بل_Locale(UEnumeration* en,
                    int32_t* پایله_اوږدتیا,
                    UError_کوډ* حالت) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = تش_اشاره;
    ثابت حرف *پایله = تش_اشاره;
    int32_t len = 0;
    إذا (ures_has_بل(res) && (k = ures_get_بل_Resource(res, &ctx->curr, حالت)) != تش_اشاره) {
        پایله = ures_get_کلی(k);
        len = (int32_t)uprv_strlen(پایله);
    }
    إذا (پایله_اوږدتیا) {
        *پایله_اوږدتیا = len;
    }
    أرجع پایله;
}

ساكن فراغ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_کوډ* /*حالت*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_پای_ټکی

ساكن ثابت UEnumeration gLocalesEnum = {
    تش_اشاره,
        تش_اشاره,
        ures_loc_closeLocales,
        ures_loc_شمېر_Locales,
        uenum_unextDefault,
        ures_loc_بل_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_شته_Locales(ثابت حرف *لار, UError_کوډ *حالت)
{
    UResourceBundle *idx = تش_اشاره;
    UEnumeration *en = تش_اشاره;
    ULocalesContext *myContext = تش_اشاره;

    إذا(U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(حجم(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    إذا(!en || !myContext) {
        *حالت = U_حافظه_ویش_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        أرجع تش_اشاره;
    }
    uprv_memcpy(en, &gLocalesEnum, حجم(UEnumeration));

    ures_init_ډېری_Object(&myContext->installed);
    ures_init_ډېری_Object(&myContext->curr);
    idx = ures_open_مستقیم(لار, شاخص_LOCALE_NAME, حالت);
    ures_get_By_کلی(idx, شاخص_نښان, &myContext->installed, حالت);
    إذا(U_SUCCESS(*حالت)) {
#إذا defined(URES_ونه_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            لار, شاخص_LOCALE_NAME, شاخص_نښان, ures_get_کلی(&myContext->installed));
#د_که_پای
        en->context = myContext;
    } وإلا {
#إذا defined(URES_ونه_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", لار, u_errorName(*حالت));
#د_که_پای
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = تش_اشاره;
    }
    
    ures_close(idx);
    
    أرجع en;
}

ساكن UBool is_Locale_دننه_List(UEnumeration *locEnum, ثابت حرف *loc_منزل_پلټنه, UError_کوډ *حالت) {
    ثابت حرف *loc;
    طالما ((loc = uenum_بل(locEnum, تش_اشاره, حالت)) != تش_اشاره) {
        إذا (uprv_strcmp(loc, loc_منزل_پلټنه) == 0) {
            أرجع ریښتیا;
        }
    }
    أرجع خطأ;
}

ساكن فراغ get_پلار_For_Functional_Equivalent(ثابت حرف*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      پلار) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UError_کوډ sub_حالت = U_ZERO_ERROR;
    پلار.clear();
    إذا (res != تش_اشاره) {
        ures_get_By_کلی(res, "%%Parent", bund1, &sub_حالت);
        إذا (U_SUCCESS(sub_حالت)) {
            int32_t length16;
            ثابت char16_t* s16 = ures_getString(bund1, &length16, &sub_حالت);
            پلار.appendInvariantChars(s16, length16, sub_حالت);
        }
    }
    
    // If none there, use normal truncation parent
    إذا (U_FAILURE(sub_حالت) || پلار.is_تشه()) {
        sub_حالت = U_ZERO_ERROR;
        پلار = ulocimp_get_پلار(localeID, sub_حالت);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(حرف *پایله, int32_t پایله_ظرفیت,
                             ثابت حرف *لار, ثابت حرف *resName, ثابت حرف *keyword, ثابت حرف *locid,
                             UBool *is_شته, UBool omitDefault, UError_کوډ *حالت)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString وموندل;
    CharString پلار;
    CharString ډک;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = تش_اشاره;
    UError_کوډ sub_حالت = U_ZERO_ERROR;
    int32_t length = 0;
    إذا(U_FAILURE(*حالت)) أرجع 0;
    CharString kwVal;
    إذا (keyword != تش_اشاره && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_ارزښت(locid, keyword, sub_حالت);
        إذا (kwVal == DEFAULT_نښان) {
            kwVal.clear();
        }
    }
    إذا (locid == تش_اشاره) {
        locid = uloc_getDefault();
    }
    CharString بنسټ = ulocimp_get_بنسټ_Name(locid, sub_حالت);
#إذا defined(URES_ونه_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.معلومات(), بنسټ.معلومات(), u_errorName(sub_حالت));
#د_که_پای
    ures_init_ډېری_Object(&bund1);
    ures_init_ډېری_Object(&bund2);

    پلار.copyFrom(بنسټ, sub_حالت);
    وموندل.copyFrom(بنسټ, sub_حالت);

    إذا(is_شته) {
        UEnumeration *locEnum = ures_open_شته_Locales(لار, &sub_حالت);
        *is_شته = ریښتیا;
        إذا (U_SUCCESS(sub_حالت)) {
            *is_شته = is_Locale_دننه_List(locEnum, پلار.معلومات(), &sub_حالت);
        }
        uenum_close(locEnum);
    }

    إذا(U_FAILURE(sub_حالت)) {
        *حالت = sub_حالت;
        أرجع 0;
    }
    
    افعل {
        sub_حالت = U_ZERO_ERROR;
        res = ures_open(لار, پلار.معلومات(), &sub_حالت);
        إذا(((sub_حالت == U_USING_FALLBACK_WARNING) ||
            (sub_حالت == U_USING_DEFAULT_WARNING)) && is_شته)
        {
            *is_شته = خطأ;
        }
        is_شته = تش_اشاره; /* only want to set this the first time around */
        
#إذا defined(URES_ونه_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", لار?لار:"ICUDATA", پلار.معلومات(), u_errorName(sub_حالت), ures_getLocale(res, &sub_حالت));
#د_که_پای
        إذا(U_FAILURE(sub_حالت)) {
            *حالت = sub_حالت;
        } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
            ures_get_By_کلی(res,resName,&bund1, &sub_حالت);
            إذا(sub_حالت == U_ZERO_ERROR) {
                ثابت char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#إذا defined(URES_ونه_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    لار?لار:"ICUDATA", پلار.معلومات(), u_errorName(sub_حالت));
#د_که_پای
                defUstr = ures_get_String_By_کلی(&bund1, DEFAULT_نښان, &defLen, &sub_حالت);
                إذا(U_SUCCESS(sub_حالت) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_ونه_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        لار?لار:"ICUDATA", پلار.معلومات(), keyword, defVal.معلومات(), u_errorName(sub_حالت));
#د_که_پای
                    defLoc.copyFrom(پلار, sub_حالت);
                    إذا(kwVal.is_تشه()) {
                        kwVal.append(defVal, sub_حالت);
#إذا defined(URES_ونه_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            لار?لار:"ICUDATA", پلار.معلومات(), keyword, kwVal.معلومات());
#د_که_پای
                    }
                }
            }
        }
        
        sub_حالت = U_ZERO_ERROR;

        إذا (res != تش_اشاره) {
            وموندل.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_حالت), sub_حالت);
        }

        إذا (وموندل != پلار) {
            پلار.copyFrom(وموندل, sub_حالت);
        } وإلا {
            get_پلار_For_Functional_Equivalent(وموندل.معلومات(),res,&bund1,پلار);
        }
        ures_close(res);
    } طالما(defVal.is_تشه() && !وموندل.is_تشه() && وموندل != "root" && U_SUCCESS(*حالت));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    پلار.copyFrom(بنسټ, sub_حالت);
    وموندل.copyFrom(بنسټ, sub_حالت);

    افعل {
        res = ures_open(لار, پلار.معلومات(), &sub_حالت);
        إذا((sub_حالت == U_USING_FALLBACK_WARNING) && is_شته) {
            *is_شته = خطأ;
        }
        is_شته = تش_اشاره; /* only want to set this the first time around */
        
#إذا defined(URES_ونه_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            لار?لار:"ICUDATA", پلار.معلومات(), u_errorName(sub_حالت), kwVal.معلومات());
#د_که_پای
        إذا(U_FAILURE(sub_حالت)) {
            *حالت = sub_حالت;
        } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
            ures_get_By_کلی(res,resName,&bund1, &sub_حالت);
#إذا defined(URES_ونه_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_حالت));
#د_که_پای
            إذا(sub_حالت == U_ZERO_ERROR) {
                ures_get_By_کلی(&bund1, kwVal.معلومات(), &bund2, &sub_حالت);
#إذا defined(URES_ونه_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.معلومات(), u_errorName(sub_حالت));
#د_که_پای
                إذا(sub_حالت == U_ZERO_ERROR) {
#إذا defined(URES_ونه_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        لار?لار:"ICUDATA", پلار.معلومات(), keyword, kwVal.معلومات(), u_errorName(sub_حالت));
#د_که_پای
                    إذا (پلار.is_تشه()) {
                        ډک.clear().append("root", sub_حالت);
                    } وإلا {
                        ډک.copyFrom(پلار, sub_حالت);
                    }
                        /* now, recalculate default kw if need be */
                        إذا(defLoc.length() > ډک.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#إذا defined(URES_ونه_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    لار?لار:"ICUDATA", ډک.معلومات());
#د_که_پای
                          defUstr = ures_get_String_By_کلی(&bund1, DEFAULT_نښان, &defLen, &sub_حالت);
                          إذا(U_SUCCESS(sub_حالت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_ونه_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    لار?لار:"ICUDATA", ډک.معلومات(), keyword, defVal.معلومات(), u_errorName(sub_حالت));
#د_که_پای
                            defLoc.copyFrom(ډک, sub_حالت);
                          }
                        } /* end of recalculate default KW */
#إذا defined(URES_ونه_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.معلومات(), ډک.معلومات());
                        }
#د_که_پای
                } وإلا {
#إذا defined(URES_ونه_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_حالت), پلار.معلومات(), kwVal.معلومات());
#د_که_پای
                }
            }
        }
        
        sub_حالت = U_ZERO_ERROR;
        UBool have_وموندل = خطأ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        إذا (res != تش_اشاره && uprv_strcmp(resName, "collations") == 0) {
            ثابت حرف *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_حالت);
            إذا (U_SUCCESS(sub_حالت) && validLoc != تش_اشاره && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_حالت);
                CharString پلار_Lang = ulocimp_getLanguage(پلار.منزل_String_Piece(), sub_حالت);
                إذا (U_SUCCESS(sub_حالت) && validLang != پلار_Lang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    وموندل.clear().append(validLoc, sub_حالت);
                    have_وموندل = ریښتیا;
                }
            }
            sub_حالت = U_ZERO_ERROR;
        }
        إذا (!have_وموندل) {
            وموندل.copyFrom(پلار, sub_حالت);
        }

        get_پلار_For_Functional_Equivalent(وموندل.معلومات(),res,&bund1,پلار);
        ures_close(res);
        sub_حالت = U_ZERO_ERROR;
    } طالما(ډک.is_تشه() && !وموندل.is_تشه() && U_SUCCESS(*حالت));

    إذا(ډک.is_تشه() && kwVal != defVal) {
#إذا defined(URES_ونه_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.معلومات(), defVal.معلومات());
#د_که_پای
        kwVal.clear().append(defVal, sub_حالت);
        پلار.copyFrom(بنسټ, sub_حالت);
        وموندل.copyFrom(بنسټ, sub_حالت);

        افعل { /* search for 'default' named item */
            res = ures_open(لار, پلار.معلومات(), &sub_حالت);
            إذا((sub_حالت == U_USING_FALLBACK_WARNING) && is_شته) {
                *is_شته = خطأ;
            }
            is_شته = تش_اشاره; /* only want to set this the first time around */
            
#إذا defined(URES_ونه_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                لار?لار:"ICUDATA", پلار.معلومات(), u_errorName(sub_حالت), kwVal.معلومات());
#د_که_پای
            إذا(U_FAILURE(sub_حالت)) {
                *حالت = sub_حالت;
            } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
                ures_get_By_کلی(res,resName,&bund1, &sub_حالت);
                إذا(sub_حالت == U_ZERO_ERROR) {
                    ures_get_By_کلی(&bund1, kwVal.معلومات(), &bund2, &sub_حالت);
                    إذا(sub_حالت == U_ZERO_ERROR) {
#إذا defined(URES_ونه_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", لار?لار:"ICUDATA",
                            پلار.معلومات(), keyword, kwVal.معلومات(), u_errorName(sub_حالت));
#د_که_پای
                        إذا (پلار.is_تشه()) {
                            ډک.clear().append("root", sub_حالت);
                        } وإلا {
                            ډک.copyFrom(پلار, sub_حالت);
                        }
                        
                        /* now, recalculate default kw if need be */
                        إذا(defLoc.length() > ډک.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#إذا defined(URES_ونه_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    لار?لار:"ICUDATA", ډک.معلومات());
#د_که_پای
                          defUstr = ures_get_String_By_کلی(&bund1, DEFAULT_نښان, &defLen, &sub_حالت);
                          إذا(U_SUCCESS(sub_حالت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_ونه_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    لار?لار:"ICUDATA", ډک.معلومات(), keyword, defVal.معلومات(), u_errorName(sub_حالت));
#د_که_پای
                            defLoc.copyFrom(ډک, sub_حالت);
                          }
                        } /* end of recalculate default KW */
#إذا defined(URES_ونه_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.معلومات(), ډک.معلومات());
                        }
#د_که_پای
                    }
                }
            }
            
            sub_حالت = U_ZERO_ERROR;
            وموندل.copyFrom(پلار, sub_حالت);
            get_پلار_For_Functional_Equivalent(وموندل.معلومات(),res,&bund1,پلار);
            ures_close(res);
            sub_حالت = U_ZERO_ERROR;
        } طالما(ډک.is_تشه() && !وموندل.is_تشه() && U_SUCCESS(*حالت));
    }
    
    إذا(U_SUCCESS(*حالت)) {
        إذا(ډک.is_تشه()) {
#إذا defined(URES_ونه_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.معلومات());
#د_که_پای
          *حالت = U_MISSING_RESOURCE_ERROR;
        } وإلا إذا(omitDefault) {
#إذا defined(URES_ونه_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", ډک.معلومات(), defLoc.معلومات(), وموندل.معلومات());
#د_که_پای        
          إذا(defLoc.length() <= ډک.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            إذا(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#إذا defined(URES_ونه_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.معلومات());
#د_که_پای
              kwVal.clear();
            }
          }
        }
        وموندل.copyFrom(ډک, sub_حالت);
        إذا(!kwVal.is_تشه()) {
            وموندل
                .append("@", sub_حالت)
                .append(keyword, sub_حالت)
                .append("=", sub_حالت)
                .append(kwVal, sub_حالت);
        } وإلا إذا(!omitDefault) {
            وموندل
                .append("@", sub_حالت)
                .append(keyword, sub_حالت)
                .append("=", sub_حالت)
                .append(defVal, sub_حالت);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = وموندل.length();

    إذا(U_SUCCESS(*حالت)) {
        int32_t copy_اوږدتیا = uprv_min(length, پایله_ظرفیت);
        إذا(copy_اوږدتیا>0) {
            وموندل.extract(پایله, copy_اوږدتیا, sub_حالت);
        }
        إذا(length == 0) {
          *حالت = U_MISSING_RESOURCE_ERROR; 
        }
    } وإلا {
        length = 0;
        پایله[0]=0;
    }
    أرجع u_terminateChars(پایله, پایله_ظرفیت, length, حالت);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_ارزښتونه(ثابت حرف *لار, ثابت حرف *keyword, UError_کوډ *حالت)
{
#تعریف_کړه ارزښتونه_BUF_اندازه 2048
#تعریف_کړه ارزښتونه_LIST_اندازه 512
    
    حرف       ارزښتونه_Buf[ارزښتونه_BUF_اندازه];
    int32_t    ارزښتونه_شاخص = 0;
    ثابت حرف *ارزښتونه_List[ارزښتونه_LIST_اندازه];
    int32_t    ارزښتونه_شمېر = 0;
    
    ثابت حرف *locale;
    int32_t     locLen;
    
    UEnumeration *locs = تش_اشاره;
    
    UResourceBundle    توکی;
    UResourceBundle    sub_توکی;
    
    ures_init_ډېری_Object(&توکی);
    ures_init_ډېری_Object(&sub_توکی);
    locs = ures_open_شته_Locales(لار, حالت);
    
    إذا(U_FAILURE(*حالت)) {
        ures_close(&توکی);
        ures_close(&sub_توکی);
        أرجع تش_اشاره;
    }
    
    ارزښتونه_Buf[0]=0;
    ارزښتونه_Buf[1]=0;

    طالما ((locale = uenum_بل(locs, &locLen, حالت)) != تش_اشاره) {
        UResourceBundle   *bund = تش_اشاره;
        UResourceBundle   *subPtr = تش_اشاره;
        UError_کوډ sub_حالت = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(لار, locale, &sub_حالت);
        
#إذا defined(URES_ونه_DEBUG)
        إذا(!bund || U_FAILURE(sub_حالت)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                لار?لار:"<ICUDATA>", keyword, locale, u_errorName(sub_حالت));
        }
#د_که_پای
        
        ures_get_By_کلی(bund, keyword, &توکی, &sub_حالت);
        
        إذا(!bund || U_FAILURE(sub_حالت)) {
#إذا defined(URES_ونه_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                لار?لار:"<ICUDATA>", keyword, locale, u_errorName(sub_حالت));
#د_که_پای
            ures_close(bund);
            bund = تش_اشاره;
            استمر;
        }

        طالما ((subPtr = ures_get_بل_Resource(&توکی, &sub_توکی, &sub_حالت)) != تش_اشاره
            && U_SUCCESS(sub_حالت)) {
            ثابت حرف *k;
            int32_t i;
            k = ures_get_کلی(subPtr);

#إذا defined(URES_ونه_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#د_که_پای
            إذا(k == تش_اشاره || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_نښان) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                استمر;
            }
            لكل(i=0; i<ارزښتونه_شمېر; i++) {
                إذا(!uprv_strcmp(ارزښتونه_List[i],k)) {
                    k = تش_اشاره; /* found duplicate */
                    اكسر;
                }
            }
            إذا(k != تش_اشاره) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                إذا((ارزښتونه_شمېر >= (ارزښتونه_LIST_اندازه-1)) ||       /* no more space in list .. */
                    ((ارزښتونه_شاخص+kLen+1+1) >= ارزښتونه_BUF_اندازه)) { /* no more space in buffer (string + 2 nulls) */
                    *حالت = U_ILLEGAL_دلیل_ERROR; /* out of space.. */
                } وإلا {
                    uprv_strcpy(ارزښتونه_Buf+ارزښتونه_شاخص, k);
                    ارزښتونه_List[ارزښتونه_شمېر++] = ارزښتونه_Buf+ارزښتونه_شاخص;
                    ارزښتونه_شاخص += kLen;
#إذا defined(URES_ونه_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        لار?لار:"<ICUDATA>", keyword, locale, k);
#د_که_پای
                    ارزښتونه_Buf[ارزښتونه_شاخص++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    ارزښتونه_Buf[ارزښتونه_شاخص++] = 0; /* terminate */
    
    ures_close(&توکی);
    ures_close(&sub_توکی);
    uenum_close(locs);
#إذا defined(URES_ونه_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*حالت), 
        ارزښتونه_شاخص, ارزښتونه_شمېر);
#د_که_پای
    أرجع uloc_openKeywordList(ارزښتونه_Buf, ارزښتونه_شاخص, حالت);
}
#إذا 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ثابت UResourceBundle* res1, ثابت UResourceBundle* res2){
    إذا(res1==تش_اشاره || res2==تش_اشاره){
        أرجع res1==res2; /* pointer comparison */
    }
    إذا(res1->f_کلی==تش_اشاره||  res2->f_کلی==تش_اشاره){
        أرجع (res1->f_کلی==res2->f_کلی);
    }وإلا{
        إذا(uprv_strcmp(res1->f_کلی, res2->f_کلی)!=0){
            أرجع خطأ;
        }
    }
    إذا(uprv_strcmp(res1->f_معلومات->fName, res2->f_معلومات->fName)!=0){
        أرجع خطأ;
    }
    إذا(res1->f_معلومات->f_لار == تش_اشاره||  res2->f_معلومات->f_لار==تش_اشاره){
        أرجع (res1->f_معلومات->f_لار == res2->f_معلومات->f_لار);
    }وإلا{
        إذا(uprv_strcmp(res1->f_معلومات->f_لار, res2->f_معلومات->f_لار)!=0){
            أرجع خطأ;
        }
    }
    إذا(uprv_strcmp(res1->f_معلومات->f_پلار->fName, res2->f_معلومات->f_پلار->fName)!=0){
        أرجع خطأ;
    }
    إذا(uprv_strcmp(res1->f_معلومات->f_پلار->f_لار, res2->f_معلومات->f_پلار->f_لار)!=0){
        أرجع خطأ;
    }
    إذا(uprv_strncmp(res1->f_Res_لار, res2->f_Res_لار, res1->f_Res_لار_Len)!=0){
        أرجع خطأ;
    }
    إذا(res1->fRes != res2->fRes){
        أرجع خطأ;
    }
    أرجع ریښتیا;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ثابت UResourceBundle* res, UError_کوډ* حالت){
    UResourceBundle* bundle = تش_اشاره;
    UResourceBundle* ret = تش_اشاره;
    إذا(U_FAILURE(*حالت) || res == تش_اشاره){
        أرجع تش_اشاره;
    }
    bundle = ures_open(res->f_معلومات->f_لار, res->f_معلومات->fName, حالت);
    إذا(res->f_Res_لار!=تش_اشاره){
        ret = ures_پیدا_کړه_Sub_Resource(bundle, res->f_Res_لار, تش_اشاره, حالت);
        ures_close(bundle);
    }وإلا{
        ret = bundle;
    }
    أرجع ret;
}
U_CAPI ثابت UResourceBundle* U_EXPORT2
ures_get_پلار_Bundle(ثابت UResourceBundle* res){
    إذا(res==تش_اشاره){
        أرجع تش_اشاره;
    }
    أرجع res->f_پلار_Res;
}
#د_که_پای

U_CAPI فراغ U_EXPORT2
ures_get_نسخه_By_کلی(ثابت UResourceBundle* res, ثابت حرف *کلی, UVersionInfo ver, UError_کوډ *حالت) {
  ثابت char16_t *str;
  int32_t len;
  str = ures_get_String_By_کلی(res, کلی, &len, حالت);
  إذا(U_SUCCESS(*حالت)) {
    u_نسخه_From_UString(ver, str);
  } 
}

/* eof */
