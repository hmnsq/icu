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

استفاده نطاق icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ساكن UHashtable *cache = اشاره_تهی;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ساكن int32_t U_CALLCONV hash_مدخل(ثابت UHashTok parm) {
    UResource_داده_مدخل* b = static_cast<UResource_داده_مدخل*>(parm.اشاره_گر);
    UHashTok namekey, pathkey;
    namekey.اشاره_گر = b->fName;
    pathkey.اشاره_گر = b->f_مسیر;
    أرجع uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ساكن UBool U_CALLCONV compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UResource_داده_مدخل* b1 = static_cast<UResource_داده_مدخل*>(p1.اشاره_گر);
    UResource_داده_مدخل* b2 = static_cast<UResource_داده_مدخل*>(p2.اشاره_گر);
    UHashTok name1, name2, path1, path2;
    name1.اشاره_گر = b1->fName;
    name2.اشاره_گر = b2->fName;
    path1.اشاره_گر = b1->f_مسیر;
    path2.اشاره_گر = b2->f_مسیر;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ساكن UBool chopLocale(حرف *name) {
    حرف *i = uprv_strrchr(name, '_');

    إذا(i != اشاره_تهی) {
        *i = '\0';
        أرجع درست;
    }

    أرجع خطأ;
}

ساكن UBool hasVariant(ثابت حرف* localeID) {
    UError_کد err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(اشاره_تهی, 0);
    ulocimp_getSubtags(
            localeID,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی,
            &sink,
            اشاره_تهی,
            err);
    أرجع sink.عدد_از_بایتها_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#تعریف_کن INCLUDED_FROM_URESBUND_CPP
#ضمن "localefallback_data.h"

ساكن ثابت حرف* performFallbackLookup(ثابت حرف* کلید,
                                         ثابت حرف* کلید_Strs,
                                         ثابت حرف* مقدار_Strs U_LIFETIME_BOUND,
                                         ثابت int32_t* lookup_جدول,
                                         int32_t lookup_جدول_درازی) {
    ثابت int32_t* bottom = lookup_جدول;
    ثابت int32_t* بالاترین = lookup_جدول + lookup_جدول_درازی;

    طالما (bottom < بالاترین) {
        // Effectively, divide by 2 and round down to an even index
        ثابت int32_t* میانه = bottom + (((بالاترین - bottom) / 4) * 2);
        ثابت حرف* مدخل_کلید = &(کلید_Strs[*میانه]);
        int32_t strcmp_نتیجه = uprv_strcmp(کلید, مدخل_کلید);
        إذا (strcmp_نتیجه == 0) {
            أرجع &(مقدار_Strs[میانه[1]]);
        } وإلا إذا (strcmp_نتیجه < 0) {
            بالاترین = میانه;
        } وإلا {
            bottom = میانه + 2;
        }
    }
    أرجع اشاره_تهی;
}

ساكن CharString getDefaultScript(ثابت CharString& language, ثابت CharString& region) {
    ثابت حرف* defaultScript = اشاره_تهی;
    UError_کد err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString نتیجه("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    إذا (!region.is_خالی()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        إذا (U_FAILURE(err)) {
            أرجع نتیجه;
        }
        defaultScript = performFallbackLookup(localeID.داده(), dsLocaleIDChars, script_کد_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    إذا (defaultScript == اشاره_تهی) {
        defaultScript = performFallbackLookup(language.داده(), dsLocaleIDChars, script_کد_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    إذا (defaultScript != اشاره_تهی) {
        نتیجه.clear();
        نتیجه.append(defaultScript, err);
    }
    أرجع نتیجه;
}

تعداد UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_ریشه,
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
    URES_OPEN_LOCALE_ریشه,
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
ساكن منطقي get_والد_Locale_ID(حرف *name, ثابت حرف *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    إذا (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        أرجع chopLocale(name);
    }
    
    UError_کد err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, اشاره_تهی, اشاره_تهی, err);

    إذا (U_FAILURE(err)) {
        // hopefully this never happens...
        أرجع chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    إذا (openType == URES_OPEN_LOCALE_DEFAULT_ریشه) {
        ثابت حرف* والد_ID = performFallbackLookup(name, والد_Locale_Chars, والد_Locale_Chars, والد_Locale_جدول, UPRV_LENGTHOF(والد_Locale_جدول));
        إذا (والد_ID != اشاره_تهی) {
            uprv_strcpy(name, والد_ID);
            أرجع درست;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    إذا (!script.is_خالی() && !region.is_خالی()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        إذا (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } وإلا إذا (!region.is_خالی()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_کد err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, اشاره_تهی, اشاره_تهی, اشاره_تهی, err);
        إذا (!origNameScript.is_خالی()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } وإلا إذا (!script.is_خالی()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        إذا (openType != URES_OPEN_LOCALE_DEFAULT_ریشه || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } وإلا {
            أرجع خطأ;
        }
    } وإلا {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        أرجع خطأ;
    }
    إذا (U_SUCCESS(err) && !workingLocale.is_خالی()) {
        uprv_strcpy(name, workingLocale.داده());
        أرجع درست;
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
    أرجع (name[0] != 0 && uprv_strstr("nb nn",name) != اشاره_تهی);
}

/**
 *  Internal function
 */
ساكن فراغ مدخل_Increase(UResource_داده_مدخل *مدخل) {
    Mutex قفل(&resbMutex);
    مدخل->f_شمار_Existing++;
    طالما(مدخل->f_والد != اشاره_تهی) {
      مدخل = مدخل->f_والد;
      مدخل->f_شمار_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ساكن UResource_داده_مدخل *get_Fallback_داده(
        ثابت UResourceBundle *resBundle,
        ثابت حرف **res_برچسب, Resource *res, UError_کد *وضعیت) {
    UResource_داده_مدخل *داده_مدخل = resBundle->f_داده;
    int32_t شاخص_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    إذا(داده_مدخل == اشاره_تهی) {
        *وضعیت = U_MISSING_RESOURCE_ERROR;
        أرجع اشاره_تهی;
    }
    إذا(داده_مدخل->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_جدول_قلم_By_کلید(&(داده_مدخل->f_داده), داده_مدخل->f_داده.ریشه_Res, &شاخص_R, res_برچسب); /* try to get data from there */
        i++;
    }
    إذا(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        طالما(*res == RES_BOGUS && داده_مدخل->f_والد != اشاره_تهی) {
            داده_مدخل = داده_مدخل->f_والد;
            إذا(داده_مدخل->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_جدول_قلم_By_کلید(&(داده_مدخل->f_داده), داده_مدخل->f_داده.ریشه_Res, &شاخص_R, res_برچسب);
            }
        }
    }

    إذا(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *وضعیت = U_MISSING_RESOURCE_ERROR;
        أرجع اشاره_تهی;
    }
    // If the resource is found in parents, we need to adjust the error.
    إذا(i>1) {
        إذا(uprv_strcmp(داده_مدخل->fName, uloc_getDefault())==0 || uprv_strcmp(داده_مدخل->fName, k_ریشه_Locale_Name)==0) {
            *وضعیت = U_USING_DEFAULT_WARNING;
        } وإلا {
            *وضعیت = U_USING_FALLBACK_WARNING;
        }
    }
    أرجع داده_مدخل;
}

ساكن فراغ
free_مدخل(UResource_داده_مدخل *مدخل) {
    UResource_داده_مدخل *alias;
    res_unload(&(مدخل->f_داده));
    إذا(مدخل->fName != اشاره_تهی && مدخل->fName != مدخل->f_Name_میانگیر) {
        uprv_free(مدخل->fName);
    }
    إذا(مدخل->f_مسیر != اشاره_تهی) {
        uprv_free(مدخل->f_مسیر);
    }
    إذا(مدخل->fPool != اشاره_تهی) {
        --مدخل->fPool->f_شمار_Existing;
    }
    alias = مدخل->fAlias;
    إذا(alias != اشاره_تهی) {
        طالما(alias->fAlias != اشاره_تهی) {
            alias = alias->fAlias;
        }
        --alias->f_شمار_Existing;
    }
    uprv_free(مدخل);
}

/* Works just like ucnv_flushCache() */
ساكن int32_t ures_flushCache()
{
    UResource_داده_مدخل *resB;
    int32_t pos;
    int32_t rb_Deleted_رقم = 0;
    ثابت UHashElement *e;
    UBool deleted_بیشتر;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex قفل(&resbMutex);
    إذا (cache == اشاره_تهی) {
        أرجع 0;
    }

    افعل {
        deleted_بیشتر = خطأ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_اولی;
        طالما ((e = uhash_بعدی_Element(cache, &pos)) != اشاره_تهی)
        {
            resB = static_cast<UResource_داده_مدخل*>(e->value.اشاره_گر);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            إذا (resB->f_شمار_Existing == 0) {
                rb_Deleted_رقم++;
                deleted_بیشتر = درست;
                uhash_removeElement(cache, e);
                free_مدخل(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } طالما(deleted_بیشتر);

    أرجع rb_Deleted_رقم;
}

#اگر_تعریف_شده URES_DEBUG
#ضمن <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_خالی = خطأ;
  int32_t pos = UHASH_اولی;
  ثابت UHashElement *e;
  UResource_داده_مدخل *resB;
  
    Mutex قفل(&resbMutex);
    إذا (cache == اشاره_تهی) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      أرجع خطأ;
    }

    طالما ((e = uhash_بعدی_Element(cache, &pos)) != اشاره_تهی) {
      cache_Not_خالی=درست;
      resB = (UResource_داده_مدخل *) e->value.اشاره_گر;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (فراغ*)resB, resB->f_شمار_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_مسیر?resB->f_مسیر:"nullptr",
              (فراغ*)resB->fPool,
              (فراغ*)resB->fAlias,
              (فراغ*)resB->f_والد);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_شمار(cache));
    أرجع cache_Not_خالی;
}

#پایان_اگر

ساكن UBool U_CALLCONV ures_cleanup()
{
    إذا (cache != اشاره_تهی) {
        ures_flushCache();
        uhash_close(cache);
        cache = اشاره_تهی;
    }
    gCacheInitOnce.reset();
    أرجع درست;
}

/** INTERNAL: Initializes the cache for resources */
ساكن فراغ U_CALLCONV createCache(UError_کد &وضعیت) {
    U_ASSERT(cache == اشاره_تهی);
    cache = uhash_open(hash_مدخل, compareEntries, اشاره_تهی, &وضعیت);
    ucln_common_ثبات_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ساكن فراغ initCache(UError_کد *وضعیت) {
    umtx_initOnce(gCacheInitOnce, &createCache, *وضعیت);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ساكن فراغ مجموعه_مدخل_Name(UResource_داده_مدخل *res, ثابت حرف *name, UError_کد *وضعیت) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    إذا(res->fName != اشاره_تهی && res->fName != res->f_Name_میانگیر) {
        uprv_free(res->fName);
    }
    إذا (len < static_cast<int32_t>(حجم(res->f_Name_میانگیر))) {
        res->fName = res->f_Name_میانگیر;
    }
    وإلا {
        res->fName = static_cast<حرف*>(uprv_malloc(len + 1));
    }
    إذا(res->fName == اشاره_تهی) {
        *وضعیت = U_حافظه_تخصیص_ERROR;
    } وإلا {
        uprv_strcpy(res->fName, name);
    }
}

ساكن UResource_داده_مدخل *
get_Pool_مدخل(ثابت حرف *مسیر, UError_کد *وضعیت);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ساكن UResource_داده_مدخل *init_مدخل(ثابت حرف *localeID, ثابت حرف *مسیر, UError_کد *وضعیت) {
    UResource_داده_مدخل *r = اشاره_تهی;
    UResource_داده_مدخل find;
    /*int32_t hashValue;*/
    ثابت حرف *name;
    حرف aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    /* here we try to deduce the right locale name */
    إذا(localeID == اشاره_تهی) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } وإلا إذا(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_ریشه_Locale_Name;
    } وإلا { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<حرف*>(name);
    find.f_مسیر = const_cast<حرف*>(مسیر);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_داده_مدخل*>(uhash_get(cache, &find));
    إذا(r == اشاره_تهی) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_داده_مدخل*>(uprv_malloc(حجم(UResource_داده_مدخل)));
        إذا(r == اشاره_تهی) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع اشاره_تهی;
        }

        uprv_memset(r, 0, حجم(UResource_داده_مدخل));
        /*r->fHashKey = hashValue;*/

        مجموعه_مدخل_Name(r, name, وضعیت);
        إذا (U_FAILURE(*وضعیت)) {
            uprv_free(r);
            أرجع اشاره_تهی;
        }

        إذا(مسیر != اشاره_تهی) {
            r->f_مسیر = uprv_strdup(مسیر);
            إذا(r->f_مسیر == اشاره_تهی) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
                uprv_free(r);
                أرجع اشاره_تهی;
            }
        }

        /* this is the actual loading */
        res_بارگیری(&(r->f_داده), r->f_مسیر, r->fName, وضعیت);

        إذا (U_FAILURE(*وضعیت)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            إذا (*وضعیت == U_حافظه_تخصیص_ERROR) {
                uprv_free(r);
                أرجع اشاره_تهی;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *وضعیت = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } وإلا { /* if we have a regular entry */
            Resource aliasres;
            إذا (r->f_داده.استفاده_میکند_Pool_Bundle) {
                r->fPool = get_Pool_مدخل(r->f_مسیر, وضعیت);
                إذا (U_SUCCESS(*وضعیت)) {
                    ثابت int32_t *poolIndexes = r->fPool->f_داده.p_ریشه + 1;
                    إذا(r->f_داده.p_ریشه[1 + URES_شاخص_POOL_CHECKSUM] == poolIndexes[URES_شاخص_POOL_CHECKSUM]) {
                        r->f_داده.pool_Bundle_کلیدها = reinterpret_cast<ثابت حرف*>(poolIndexes + (poolIndexes[URES_شاخص_درازی] & 0xff));
                        r->f_داده.pool_Bundle_رشتهها = r->fPool->f_داده.p16_بیت_Units;
                    } وإلا {
                        r->fBogus = *وضعیت = U_INVALID_FORMAT_ERROR;
                    }
                } وإلا {
                    r->fBogus = *وضعیت;
                }
            }
            إذا (U_SUCCESS(*وضعیت)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_داده), "%%ALIAS");
                إذا (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ثابت char16_t *alias = res_getStringNoTrace(&(r->f_داده), aliasres, &aliasLen);
                    إذا(alias != اشاره_تهی && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_مقصد_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_مدخل(aliasName, مسیر, وضعیت);
                    }
                }
            }
        }

        {
            UResource_داده_مدخل *oldR = اشاره_تهی;
            إذا ((oldR = static_cast<UResource_داده_مدخل*>(uhash_get(cache, r))) == اشاره_تهی) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_کد cache_وضعیت = U_ZERO_ERROR;
                uhash_put(cache, (فراغ *)r, r, &cache_وضعیت);
                إذا (U_FAILURE(cache_وضعیت)) {
                    *وضعیت = cache_وضعیت;
                    free_مدخل(r);
                    r = اشاره_تهی;
                }
            } وإلا {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_مدخل(r);
                r = oldR;
            }
        }

    }
    إذا(r != اشاره_تهی) {
        /* return the real bundle */
        طالما(r->fAlias != اشاره_تهی) {
            r = r->fAlias;
        }
        r->f_شمار_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        إذا(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*وضعیت)) {
             *وضعیت = r->fBogus; /* set the returning status */
        }
    }
    أرجع r;
}

ساكن UResource_داده_مدخل *
get_Pool_مدخل(ثابت حرف *مسیر, UError_کد *وضعیت) {
    UResource_داده_مدخل *poolBundle = init_مدخل(kPoolBundleName, مسیر, وضعیت);
    إذا( U_SUCCESS(*وضعیت) &&
        (poolBundle == اشاره_تهی || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_داده.isPoolBundle)
    ) {
        *وضعیت = U_INVALID_FORMAT_ERROR;
    }
    أرجع poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ساكن UResource_داده_مدخل *
پیدا_کن_اولی_Existing(ثابت حرف* مسیر, حرف* name, ثابت حرف* defaultLocale, UResOpenType openType,
                  UBool *is_ریشه, UBool *یافت_والد, UBool *isDefault, UError_کد* وضعیت) {
    UResource_داده_مدخل *r = اشاره_تهی;
    UBool has_Real_داده = خطأ;
    *یافت_والد = درست; /* we're starting with a fresh name */
    حرف origName[ULOC_FULLNAME_ظرفیت];

    uprv_strcpy(origName, name);
    طالما(*یافت_والد && !has_Real_داده) {
        r = init_مدخل(name, مسیر, وضعیت);
        /* Null pointer test */
        إذا (U_FAILURE(*وضعیت)) {
            أرجع اشاره_تهی;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_داده = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        إذا(!has_Real_داده) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_شمار_Existing--;
            /*entryCloseInt(r);*/
            r = اشاره_تهی;
            *وضعیت = U_USING_FALLBACK_WARNING;
        } وإلا {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_ریشه = static_cast<UBool>(uprv_strcmp(name, k_ریشه_Locale_Name) == 0);

        /*Fallback data stuff*/
        إذا (!has_Real_داده) {
            *یافت_والد = get_والد_Locale_ID(name, origName, openType);
        } وإلا {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *یافت_والد = chopLocale(name);
        }
        إذا (*یافت_والد && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    أرجع r;
}

ساكن فراغ ures_مجموعه_Is_پشته_Object( UResourceBundle* resB, UBool state) {
    إذا(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } وإلا {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ساكن UBool ures_is_پشته_Object(ثابت UResourceBundle* resB) {
  أرجع((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?خطأ:درست);
}


U_CFUNC فراغ ures_init_پشته_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, حجم(UResourceBundle));
  ures_مجموعه_Is_پشته_Object(resB, درست);
}

U_NAMESPACE_BEGIN

پشته_UResource_Bundle::پشته_UResource_Bundle() {
    ures_init_پشته_Object(&bundle);
}

پشته_UResource_Bundle::~پشته_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_انتها

ساكن UBool  // returns U_SUCCESS(*status)
بارگیری_Parents_Except_ریشه(UResource_داده_مدخل *&t1,
                      حرف name[], int32_t name_ظرفیت,
                      UBool usingUSRData, حرف usr_داده_مسیر[], UError_کد *وضعیت) {
    إذا (U_FAILURE(*وضعیت)) { أرجع خطأ; }
    UBool check_والد = درست;
    طالما (check_والد && t1->f_والد == اشاره_تهی && !t1->f_داده.noFallback &&
            res_getResource(&t1->f_داده,"%%ParentIsRoot") == RES_BOGUS) {
        Resource والد_Res = res_getResource(&t1->f_داده, "%%Parent");
        إذا (والد_Res != RES_BOGUS) {  // An explicit parent was found.
            int32_t والد_Locale_Len = 0;
            // No tracing: called during initial data loading
            ثابت char16_t *والد_Locale_Name = res_getStringNoTrace(&(t1->f_داده), والد_Res, &والد_Locale_Len);
            إذا(والد_Locale_Name != اشاره_تهی && 0 < والد_Locale_Len && والد_Locale_Len < name_ظرفیت) {
                u_UChars_مقصد_Chars(والد_Locale_Name, name, والد_Locale_Len + 1);
                إذا (uprv_strcmp(name, k_ریشه_Locale_Name) == 0) {
                    أرجع درست;
                }
            }
        }
        // Insert regular parents.
        UError_کد والد_وضعیت = U_ZERO_ERROR;
        UResource_داده_مدخل *t2 = init_مدخل(name, t1->f_مسیر, &والد_وضعیت);
        إذا (U_FAILURE(والد_وضعیت)) {
            *وضعیت = والد_وضعیت;
            أرجع خطأ;
        }
        UResource_داده_مدخل *u2 = اشاره_تهی;
        UError_کد usr_وضعیت = U_ZERO_ERROR;
        إذا (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_مدخل(name, usr_داده_مسیر, &usr_وضعیت);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            إذا (usr_وضعیت == U_حافظه_تخصیص_ERROR) {
                *وضعیت = usr_وضعیت;
                أرجع خطأ;
            }
        }

        إذا (usingUSRData && U_SUCCESS(usr_وضعیت) && u2->fBogus == U_ZERO_ERROR) {
            t1->f_والد = u2;
            u2->f_والد = t2;
        } وإلا {
            t1->f_والد = t2;
            إذا (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_شمار_Existing = 0;
            }
        }
        t1 = t2;
        check_والد = chopLocale(name) || may_Have_والد(name);
    }
    أرجع درست;
}

ساكن UBool  // returns U_SUCCESS(*status)
درج_ریشه_Bundle(UResource_داده_مدخل *&t1, UError_کد *وضعیت) {
    إذا (U_FAILURE(*وضعیت)) { أرجع خطأ; }
    UError_کد والد_وضعیت = U_ZERO_ERROR;
    UResource_داده_مدخل *t2 = init_مدخل(k_ریشه_Locale_Name, t1->f_مسیر, &والد_وضعیت);
    إذا (U_FAILURE(والد_وضعیت)) {
        *وضعیت = والد_وضعیت;
        أرجع خطأ;
    }
    t1->f_والد = t2;
    t1 = t2;
    أرجع درست;
}

ساكن UResource_داده_مدخل *مدخل_Open(ثابت حرف* مسیر, ثابت حرف* localeID,
                                     UResOpenType openType, UError_کد* وضعیت) {
    U_ASSERT(openType != URES_OPEN_مستقیم);
    UError_کد int_وضعیت = U_ZERO_ERROR;
    UResource_داده_مدخل *r = اشاره_تهی;
    UResource_داده_مدخل *t1 = اشاره_تهی;
    UBool isDefault = خطأ;
    UBool is_ریشه = خطأ;
    UBool has_Real_داده = خطأ;
    UBool hasChopped = درست;
    UBool usingUSRData = U_USE_USRDATA && ( مسیر == اشاره_تهی || uprv_strncmp(مسیر,U_ICUDATA_NAME,8) == 0);

    حرف name[ULOC_FULLNAME_ظرفیت];
    حرف usr_داده_مسیر[96];

    initCache(وضعیت);

    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    uprv_strncpy(name, localeID, حجم(name) - 1);
    name[حجم(name) - 1] = 0;

    إذا ( usingUSRData ) {
        إذا ( مسیر == اشاره_تهی ) {
            uprv_strcpy(usr_داده_مسیر, U_USRDATA_NAME);
        } وإلا {
            uprv_strncpy(usr_داده_مسیر, مسیر, حجم(usr_داده_مسیر) - 1);
            usr_داده_مسیر[0] = 'u';
            usr_داده_مسیر[1] = 's';
            usr_داده_مسیر[2] = 'r';
            usr_داده_مسیر[حجم(usr_داده_مسیر) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ثابت حرف *defaultLocale = uloc_getDefault();

    Mutex قفل(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = پیدا_کن_اولی_Existing(مسیر, name, defaultLocale, openType, &is_ریشه, &hasChopped, &isDefault, &int_وضعیت);

    // If we failed due to out-of-memory, report the failure and exit early.
    إذا (int_وضعیت == U_حافظه_تخصیص_ERROR) {
        *وضعیت = int_وضعیت;
        اذهب پایان;
    }

    إذا(r != اشاره_تهی) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_داده = درست;
        إذا ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_کد usr_وضعیت = U_ZERO_ERROR;
            UResource_داده_مدخل *u1 = init_مدخل(t1->fName, usr_داده_مسیر, &usr_وضعیت);
            // If we failed due to out-of-memory, report the failure and exit early.
            إذا (int_وضعیت == U_حافظه_تخصیص_ERROR) {
                *وضعیت = int_وضعیت;
                اذهب پایان;
            }
            إذا ( u1 != اشاره_تهی ) {
                إذا(u1->fBogus == U_ZERO_ERROR) {
                    u1->f_والد = t1;
                    r = u1;
                } وإلا {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_شمار_Existing = 0;
                }
            }
        }
        إذا ((hasChopped || may_Have_والد(name)) && !is_ریشه) {
            إذا (!بارگیری_Parents_Except_ریشه(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_داده_مسیر, وضعیت)) {
                اذهب پایان;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    إذا(r==اشاره_تهی && openType == URES_OPEN_LOCALE_DEFAULT_ریشه && !isDefault && !is_ریشه) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = پیدا_کن_اولی_Existing(مسیر, name, defaultLocale, openType, &is_ریشه, &hasChopped, &isDefault, &int_وضعیت);
        // If we failed due to out-of-memory, report the failure and exit early.
        إذا (int_وضعیت == U_حافظه_تخصیص_ERROR) {
            *وضعیت = int_وضعیت;
            اذهب پایان;
        }
        int_وضعیت = U_USING_DEFAULT_WARNING;
        إذا(r != اشاره_تهی) { /* the default locale exists */
            t1 = r;
            has_Real_داده = درست;
            isDefault = درست;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            إذا ((hasChopped || may_Have_والد(name)) && !is_ریشه) {
                إذا (!بارگیری_Parents_Except_ریشه(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_داده_مسیر, وضعیت)) {
                    اذهب پایان;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    إذا(r == اشاره_تهی) {
        uprv_strcpy(name, k_ریشه_Locale_Name);
        r = پیدا_کن_اولی_Existing(مسیر, name, defaultLocale, openType, &is_ریشه, &hasChopped, &isDefault, &int_وضعیت);
        // If we failed due to out-of-memory, report the failure and exit early.
        إذا (int_وضعیت == U_حافظه_تخصیص_ERROR) {
            *وضعیت = int_وضعیت;
            اذهب پایان;
        }
        إذا(r != اشاره_تهی) {
            t1 = r;
            int_وضعیت = U_USING_DEFAULT_WARNING;
            has_Real_داده = درست;
        } وإلا { /* we don't even have the root locale */
            *وضعیت = U_MISSING_RESOURCE_ERROR;
            اذهب پایان;
        }
    } وإلا إذا(!is_ریشه && uprv_strcmp(t1->fName, k_ریشه_Locale_Name) != 0 &&
            t1->f_والد == اشاره_تهی && !r->f_داده.noFallback) {
        إذا (!درج_ریشه_Bundle(t1, وضعیت)) {
            اذهب پایان;
        }
        إذا(!has_Real_داده) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    طالما(r != اشاره_تهی && !is_ریشه && t1->f_والد != اشاره_تهی) {
        t1->f_والد->f_شمار_Existing++;
        t1 = t1->f_والد;
    }

پایان:
    إذا(U_SUCCESS(*وضعیت)) {
        إذا(int_وضعیت != U_ZERO_ERROR) {
            *وضعیت = int_وضعیت;  
        }
        أرجع r;
    } وإلا {
        أرجع اشاره_تهی;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ساكن UResource_داده_مدخل *
مدخل_Open_مستقیم(ثابت حرف* مسیر, ثابت حرف* localeID, UError_کد* وضعیت) {
    initCache(وضعیت);
    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    إذا (localeID == اشاره_تهی) {
        localeID = uloc_getDefault();
    } وإلا إذا (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_ریشه_Locale_Name;
    }

    Mutex قفل(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_داده_مدخل *r = init_مدخل(localeID, مسیر, وضعیت);
    إذا(U_SUCCESS(*وضعیت)) {
        إذا(r->fBogus != U_ZERO_ERROR) {
            r->f_شمار_Existing--;
            r = اشاره_تهی;
        }
    } وإلا {
        r = اشاره_تهی;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_داده_مدخل *t1 = r;
    إذا(r != اشاره_تهی && uprv_strcmp(localeID, k_ریشه_Locale_Name) != 0 &&  // not root
            r->f_والد == اشاره_تهی && !r->f_داده.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_ظرفیت) {
        حرف name[ULOC_FULLNAME_ظرفیت];
        uprv_strcpy(name, localeID);
        إذا(!chopLocale(name) || uprv_strcmp(name, k_ریشه_Locale_Name) == 0 ||
                بارگیری_Parents_Except_ریشه(t1, name, UPRV_LENGTHOF(name), خطأ, اشاره_تهی, وضعیت)) {
            إذا(uprv_strcmp(t1->fName, k_ریشه_Locale_Name) != 0 && t1->f_والد == اشاره_تهی) {
                درج_ریشه_Bundle(t1, وضعیت);
            }
        }
        إذا(U_FAILURE(*وضعیت)) {
            r = اشاره_تهی;
        }
    }

    إذا(r != اشاره_تهی) {
        // TODO: Does this ever loop?
        طالما(t1->f_والد != اشاره_تهی) {
            t1->f_والد->f_شمار_Existing++;
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
ساكن فراغ مدخل_Close_Int(UResource_داده_مدخل *resB) {
    UResource_داده_مدخل *p = resB;

    طالما(resB != اشاره_تهی) {
        p = resB->f_والد;
        resB->f_شمار_Existing--;

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

ساكن فراغ مدخل_Close(UResource_داده_مدخل *resB) {
  Mutex قفل(&resbMutex);
  مدخل_Close_Int(resB);
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
ساكن فراغ ures_append_Res_مسیر(UResourceBundle *resB, ثابت حرف* مقصد_جمع_کن, int32_t len_مقصد_جمع_کن, UError_کد *وضعیت) {
    int32_t res_مسیر_Len_Orig = resB->f_Res_مسیر_Len;
    إذا(resB->f_Res_مسیر == اشاره_تهی) {
        resB->f_Res_مسیر = resB->fResBuf;
        *(resB->f_Res_مسیر) = 0;
        resB->f_Res_مسیر_Len = 0;
    } 
    resB->f_Res_مسیر_Len += len_مقصد_جمع_کن;
    إذا(RES_BUFSIZE <= resB->f_Res_مسیر_Len+1) {
        إذا(resB->f_Res_مسیر == resB->fResBuf) {
            resB->f_Res_مسیر = static_cast<حرف*>(uprv_malloc((resB->f_Res_مسیر_Len + 1) * حجم(حرف)));
            /* Check that memory was allocated correctly. */
            إذا (resB->f_Res_مسیر == اشاره_تهی) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            uprv_strcpy(resB->f_Res_مسیر, resB->fResBuf);
        } وإلا {
            حرف* موقت = static_cast<حرف*>(uprv_realloc(resB->f_Res_مسیر, (resB->f_Res_مسیر_Len + 1) * حجم(حرف)));
            /* Check that memory was reallocated correctly. */
            إذا (موقت == اشاره_تهی) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            resB->f_Res_مسیر = موقت;
        }
    }
    uprv_strcpy(resB->f_Res_مسیر + res_مسیر_Len_Orig, مقصد_جمع_کن);
}

ساكن فراغ ures_free_Res_مسیر(UResourceBundle *resB) {
    إذا (resB->f_Res_مسیر && resB->f_Res_مسیر != resB->fResBuf) {
        uprv_free(resB->f_Res_مسیر);
    }
    resB->f_Res_مسیر = اشاره_تهی;
    resB->f_Res_مسیر_Len = 0;
}

ساكن فراغ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    إذا(resB != اشاره_تهی) {
        إذا(resB->f_داده != اشاره_تهی) {
            مدخل_Close(resB->f_داده);
        }
        إذا(resB->f_نسخه != اشاره_تهی) {
            uprv_free(resB->f_نسخه);
        }
        ures_free_Res_مسیر(resB);

        إذا(ures_is_پشته_Object(resB) == خطأ && freeBundleObj) {
            uprv_free(resB);
        }
#إذا 0 /*U_DEBUG*/
        وإلا {
            /* poison the data */
            uprv_memset(resB, -1, حجم(UResourceBundle));
        }
#پایان_اگر
    }
}

U_CAPI فراغ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, درست);
}

نطاق {

UResourceBundle* init_resb_نتیجه(UResource_داده_مدخل* داده_مدخل,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  UResource_داده_مدخل* valid_Locale_داده_مدخل,
                                  ثابت حرف* container_Res_مسیر,
                                  int32_t recursion_عمق,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کد* وضعیت);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
get_Alias_هدف_As_Resource_Bundle(ثابت Resource_داده& res_داده,
                               Resource r,
                               ثابت حرف* کلید,
                               int32_t idx,
                               UResource_داده_مدخل* valid_Locale_داده_مدخل,
                               ثابت حرف* container_Res_مسیر,
                               int32_t recursion_عمق,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_کد* وضعیت) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    إذا (U_FAILURE(*وضعیت)) { أرجع resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ثابت char16_t *alias = res_getAlias(&res_داده, r, &len);
    إذا(len <= 0) {
        // bad alias
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
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
    chAlias.appendInvariantChars(alias, len, *وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    // We have an alias, now let's cut it up.
    ثابت حرف *مسیر = اشاره_تهی, *locale = اشاره_تهی, *کلید_مسیر = اشاره_تهی;
    إذا(chAlias[0] == RES_مسیر_SEPARATOR) {
        // There is a path included.
        حرف *ch_Alias_داده = chAlias.داده();
        حرف *sep = ch_Alias_داده + 1;
        مسیر = sep;
        sep = uprv_strchr(sep, RES_مسیر_SEPARATOR);
        إذا(sep != اشاره_تهی) {
            *sep++ = 0;
        }
        إذا(uprv_strcmp(مسیر, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            کلید_مسیر = sep;
            // Read from the valid locale which we already have.
            مسیر = locale = اشاره_تهی;
        } وإلا {
            إذا(uprv_strcmp(مسیر, "ICUDATA") == 0) { /* want ICU data */
                مسیر = اشاره_تهی;
            }
            إذا (sep == اشاره_تهی) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } وإلا {
                locale = sep;
                sep = uprv_strchr(sep, RES_مسیر_SEPARATOR);
                إذا(sep != اشاره_تهی) {
                    *sep++ = 0;
                }
                کلید_مسیر = sep;
            }
        }
    } وإلا {
        // No path, start with a locale.
        حرف *sep = chAlias.داده();
        locale = sep;
        sep = uprv_strchr(sep, RES_مسیر_SEPARATOR);
        إذا(sep != اشاره_تهی) {
            *sep++ = 0;
        }
        کلید_مسیر = sep;
        مسیر = valid_Locale_داده_مدخل->f_مسیر;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    Local_UResource_Bundle_اشاره_گر mainRes;
    UResource_داده_مدخل *داده_مدخل;
    إذا (locale == اشاره_تهی) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        داده_مدخل = valid_Locale_داده_مدخل;
    } وإلا {
        UError_کد int_وضعیت = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_مستقیم(مسیر, locale, &int_وضعیت));
        إذا(U_FAILURE(int_وضعیت)) {
            // We failed to open the resource bundle we're aliasing to.
            *وضعیت = int_وضعیت;
            أرجع resB;
        }
        داده_مدخل = mainRes->f_داده;
    }

    ثابت حرف* موقت = اشاره_تهی;
    إذا(کلید_مسیر == اشاره_تهی) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = داده_مدخل->f_داده.ریشه_Res;
        إذا(container_Res_مسیر) {
            chAlias.clear().append(container_Res_مسیر, *وضعیت);
            إذا (U_FAILURE(*وضعیت)) {
                أرجع اشاره_تهی;
            }
            حرف *a_کلید = chAlias.داده();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_پیدا_کن_Resource(&داده_مدخل->f_داده, r, &a_کلید, &موقت);
        }
        إذا(کلید) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(کلید, *وضعیت);
            إذا (U_FAILURE(*وضعیت)) {
                أرجع اشاره_تهی;
            }
            حرف *a_کلید = chAlias.داده();
            r = res_پیدا_کن_Resource(&داده_مدخل->f_داده, r, &a_کلید, &موقت);
        } وإلا إذا(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            إذا(URES_IS_جدول(type)) {
                ثابت حرف *a_کلید;
                r = res_get_جدول_قلم_By_شاخص(&داده_مدخل->f_داده, r, idx, &a_کلید);
            } وإلا { /* array */
                r = res_get_Array_قلم(&داده_مدخل->f_داده, r, idx);
            }
        }
        إذا(r != RES_BOGUS) {
            resB = init_resb_نتیجه(
                داده_مدخل, r, موقت, -1, valid_Locale_داده_مدخل, اشاره_تهی, recursion_عمق+1,
                resB, وضعیت);
        } وإلا {
            *وضعیت = U_MISSING_RESOURCE_ERROR;
        }
    } وإلا {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString مسیر_Buf(کلید_مسیر, *وضعیت);
        إذا (U_FAILURE(*وضعیت)) {
            أرجع اشاره_تهی;
        }
        حرف *my_مسیر = مسیر_Buf.داده();
        container_Res_مسیر = اشاره_تهی;
        // Now we have fallback following here.
        لكل(;;) {
            r = داده_مدخل->f_داده.ریشه_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            طالما(*my_مسیر && U_SUCCESS(*وضعیت)) {
                r = res_پیدا_کن_Resource(&(داده_مدخل->f_داده), r, &my_مسیر, &موقت);
                إذا(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    اكسر;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_نتیجه(
                    داده_مدخل, r, موقت, -1,
                    valid_Locale_داده_مدخل, container_Res_مسیر, recursion_عمق+1,
                    resB, وضعیت);
                إذا (U_FAILURE(*وضعیت)) {
                    اكسر;
                }
                إذا (موقت == اشاره_تهی || uprv_strcmp(کلید_مسیر, موقت) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_مسیر(resB);
                    ures_append_Res_مسیر(resB, کلید_مسیر, static_cast<int32_t>(uprv_strlen(کلید_مسیر)), وضعیت);
                    إذا(resB->f_Res_مسیر[resB->f_Res_مسیر_Len-1] != RES_مسیر_SEPARATOR) {
                        ures_append_Res_مسیر(resB, RES_مسیر_SEPARATOR_S, 1, وضعیت);
                    }
                    إذا (U_FAILURE(*وضعیت)) {
                        اكسر;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                داده_مدخل = resB->f_داده;
                container_Res_مسیر = resB->f_Res_مسیر;
            }
            إذا (U_FAILURE(*وضعیت) || r != RES_BOGUS) {
                اكسر;
            }
            // Fall back to the parent bundle, if there is one.
            داده_مدخل = داده_مدخل->f_والد;
            إذا (داده_مدخل == اشاره_تهی) {
                *وضعیت = U_MISSING_RESOURCE_ERROR;
                اكسر;
            }
            // Copy the same keyPath again.
            my_مسیر = مسیر_Buf.داده();
            uprv_strcpy(my_مسیر, کلید_مسیر);
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
UResourceBundle* init_resb_نتیجه(UResource_داده_مدخل* داده_مدخل,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  UResource_داده_مدخل* valid_Locale_داده_مدخل,
                                  ثابت حرف* container_Res_مسیر,
                                  int32_t recursion_عمق,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کد* وضعیت) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    إذا(وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع resB;
    }
    إذا (valid_Locale_داده_مدخل == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    إذا(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        إذا(recursion_عمق >= URES_MAX_ALIAS_LEVEL) {
            *وضعیت = U_TOO_MANY_ALIASES_ERROR;
            أرجع resB;
        }
        أرجع get_Alias_هدف_As_Resource_Bundle(
            داده_مدخل->f_داده, r, کلید, idx,
            valid_Locale_داده_مدخل, container_Res_مسیر, recursion_عمق, resB, وضعیت);
    }
    إذا(resB == اشاره_تهی) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        إذا (resB == اشاره_تهی) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع اشاره_تهی;
        }
        ures_مجموعه_Is_پشته_Object(resB, خطأ);
        resB->f_Res_مسیر = اشاره_تهی;
        resB->f_Res_مسیر_Len = 0;
    } وإلا {
        إذا(resB->f_داده != اشاره_تهی) {
            مدخل_Close(resB->f_داده);
        }
        إذا(resB->f_نسخه != اشاره_تهی) {
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
        إذا(container_Res_مسیر != resB->f_Res_مسیر) {
            ures_free_Res_مسیر(resB);
        }
    }
    resB->f_داده = داده_مدخل;
    مدخل_Increase(resB->f_داده);
    resB->fHasFallback = خطأ;
    resB->f_Is_بالاترین_Level = خطأ;
    resB->f_شاخص = -1;
    resB->f_کلید = کلید; 
    resB->f_Valid_Locale_داده_مدخل = valid_Locale_داده_مدخل;
    إذا(container_Res_مسیر != resB->f_Res_مسیر) {
        ures_append_Res_مسیر(
            resB, container_Res_مسیر, static_cast<int32_t>(uprv_strlen(container_Res_مسیر)), وضعیت);
    }
    إذا(کلید != اشاره_تهی) {
        ures_append_Res_مسیر(resB, کلید, static_cast<int32_t>(uprv_strlen(کلید)), وضعیت);
        إذا(resB->f_Res_مسیر_Len > 0 && resB->f_Res_مسیر[resB->f_Res_مسیر_Len-1] != RES_مسیر_SEPARATOR) {
            ures_append_Res_مسیر(resB, RES_مسیر_SEPARATOR_S, 1, وضعیت);
        }
    } وإلا إذا(idx >= 0) {
        حرف buf[256];
        int32_t len = T_CString_integer_مقصد_String(buf, idx, 10);
        ures_append_Res_مسیر(resB, buf, len, وضعیت);
        إذا(resB->f_Res_مسیر_Len > 0 && resB->f_Res_مسیر[resB->f_Res_مسیر_Len-1] != RES_مسیر_SEPARATOR) {
            ures_append_Res_مسیر(resB, RES_مسیر_SEPARATOR_S, 1, وضعیت);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t استفاده_شده_Len = ((resB->fResBuf == resB->f_Res_مسیر) ? resB->f_Res_مسیر_Len : 0);
        uprv_memset(resB->fResBuf + استفاده_شده_Len, 0, حجم(resB->fResBuf) - استفاده_شده_Len);
    }

    resB->f_نسخه = اشاره_تهی;
    resB->fRes = r;
    resB->f_اندازه = res_شمار_Array_اقلام(&resB->get_Res_داده(), resB->fRes);
    ResourceTracer(resB).trace("get");
    أرجع resB;
}

UResourceBundle* init_resb_نتیجه(UResource_داده_مدخل* داده_مدخل,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ثابت UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کد* وضعیت) {
    أرجع init_resb_نتیجه(
        داده_مدخل, r, کلید, idx,
        container->f_Valid_Locale_داده_مدخل, container->f_Res_مسیر, 0, resB, وضعیت);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ثابت UResourceBundle* original,
                               UError_کد* وضعیت) {
    UBool is_پشته_Object;
    إذا(U_FAILURE(*وضعیت) || r == original) {
        أرجع r;
    }
    إذا(original != اشاره_تهی) {
        إذا(r == اشاره_تهی) {
            is_پشته_Object = خطأ;
            r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
            /* test for nullptr */
            إذا (r == اشاره_تهی) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع اشاره_تهی;
            }
        } وإلا {
            is_پشته_Object = ures_is_پشته_Object(r);
            ures_closeBundle(r, خطأ);
        }
        uprv_memcpy(r, original, حجم(UResourceBundle));
        r->f_Res_مسیر = اشاره_تهی;
        r->f_Res_مسیر_Len = 0;
        إذا(original->f_Res_مسیر) {
            ures_append_Res_مسیر(r, original->f_Res_مسیر, original->f_Res_مسیر_Len, وضعیت);
        }
        ures_مجموعه_Is_پشته_Object(r, is_پشته_Object);
        إذا(r->f_داده != اشاره_تهی) {
            مدخل_Increase(r->f_داده);
        }
    }
    أرجع r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ثابت char16_t* U_EXPORT2 ures_getString(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_کد* وضعیت) {
    ثابت char16_t *s;
    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا(resB == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    s = res_getString({resB}, &resB->get_Res_داده(), resB->fRes, len);
    إذا (s == اشاره_تهی) {
        *وضعیت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ساكن ثابت حرف*
ures_مقصد_UTF8_String(ثابت char16_t* s16,
                  int32_t length16,
                  حرف* dest U_LIFETIME_BOUND,
                  int32_t* p_درازی,
                  UBool forceCopy,
                  UError_کد* وضعیت) {
    int32_t capacity;

    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (p_درازی != اشاره_تهی) {
        capacity = *p_درازی;
    } وإلا {
        capacity = 0;
    }
    إذا (capacity < 0 || (capacity > 0 && dest == اشاره_تهی)) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    إذا (length16 == 0) {
        /* empty string, return as read-only pointer */
        إذا (p_درازی != اشاره_تهی) {
            *p_درازی = 0;
        }
        إذا (forceCopy) {
            u_terminateChars(dest, capacity, 0, وضعیت);
            أرجع dest;
        } وإلا {
            أرجع "";
        }
    } وإلا {
        /* We need to transform the string to the destination buffer. */
        إذا (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            أرجع u_str_مقصد_UTF8(اشاره_تهی, 0, p_درازی, s16, length16, وضعیت);
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
            int32_t max_درازی_2 = 3 * length16 + 1;
            إذا (capacity > max_درازی_2) {
                dest += capacity - max_درازی_2;
                capacity = max_درازی_2;
            }
        }
        أرجع u_str_مقصد_UTF8(dest, capacity, p_درازی, s16, length16, وضعیت);
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getUTF8String(ثابت UResourceBundle* resB,
                   حرف* dest U_LIFETIME_BOUND,
                   int32_t* p_درازی,
                   UBool forceCopy,
                   UError_کد* وضعیت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getString(resB, &length16, وضعیت);
    أرجع ures_مقصد_UTF8_String(s16, length16, dest, p_درازی, forceCopy, وضعیت);
}

U_CAPI ثابت uint8_t* U_EXPORT2
ures_get_دودویی(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_کد* وضعیت) {
  ثابت uint8_t *p;
  إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  إذا(resB == اشاره_تهی) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع اشاره_تهی;
  }
  p = res_get_دودویی({resB}, &resB->get_Res_داده(), resB->fRes, len);
  إذا (p == اشاره_تهی) {
    *وضعیت = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

U_CAPI ثابت int32_t* U_EXPORT2
ures_get_Int_وکتور(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_کد* وضعیت) {
  ثابت int32_t *p;
  إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  إذا(resB == اشاره_تهی) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع اشاره_تهی;
  }
  p = res_get_Int_وکتور({resB}, &resB->get_Res_داده(), resB->fRes, len);
  إذا (p == اشاره_تهی) {
    *وضعیت = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ثابت UResourceBundle* resB, UError_کد *وضعیت) {
  إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
    أرجع 0xffffffff;
  }
  إذا(resB == اشاره_تهی) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0xffffffff;
  }
  إذا(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *وضعیت = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ثابت UResourceBundle* resB, UError_کد *وضعیت) {
  إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
    أرجع 0xffffffff;
  }
  إذا(resB == اشاره_تهی) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0xffffffff;
  }
  إذا(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *وضعیت = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ثابت UResourceBundle *resB) {
  إذا(resB == اشاره_تهی) {
    أرجع URES_NONE;
  }
  أرجع res_get_عمومی_Type(resB->fRes);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_کلید(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  //
  // TODO: Trace ures_getKey? I guess not usually.
  //
  // We usually get the key string to decide whether we want the value, or to
  // make a key-value pair. Tracing the value should suffice.
  //
  // However, I believe we have some data (e.g., in res_index) where the key
  // strings are the data. Tracing the enclosing table should suffice.
  //
  إذا(resB == اشاره_تهی) {
    أرجع اشاره_تهی;
  }
  أرجع(resB->f_کلید);
}

U_CAPI int32_t U_EXPORT2 ures_get_اندازه(ثابت UResourceBundle *resB) {
  إذا(resB == اشاره_تهی) {
    أرجع 0;
  }
  
  أرجع resB->f_اندازه;
}

ساكن ثابت char16_t*
ures_getStringWithAlias(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_شاخص,
                        int32_t* len,
                        UError_کد* وضعیت) {
  إذا(RES_GET_TYPE(r) == URES_ALIAS) {
    ثابت char16_t* نتیجه = اشاره_تهی;
    UResourceBundle *موقت_Res = ures_get_By_شاخص(resB, s_شاخص, اشاره_تهی, وضعیت);
    نتیجه = ures_getString(موقت_Res, len, وضعیت);
    ures_close(موقت_Res);
    أرجع نتیجه;
  } وإلا {
    أرجع res_getString({resB, s_شاخص}, &resB->get_Res_داده(), r, len); 
  }
}

U_CAPI فراغ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  إذا(resB == اشاره_تهی) {
    أرجع;
  }
  resB->f_شاخص = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_بعدی(ثابت UResourceBundle *resB) {
  إذا(resB == اشاره_تهی) {
    أرجع خطأ;
  }
  أرجع resB->f_شاخص < resB->f_اندازه-1;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_بعدی_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ثابت حرف** کلید,
                   UError_کد* وضعیت) {
  Resource r = RES_BOGUS;
  
  إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  إذا(resB == اشاره_تهی) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع اشاره_تهی;
  }
  
  إذا(resB->f_شاخص == resB->f_اندازه-1) {
    *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
  } وإلا {
    resB->f_شاخص++;
    بدل(RES_GET_TYPE(resB->fRes)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
      أرجع res_getString({resB}, &resB->get_Res_داده(), resB->fRes, len);
    حالة URES_جدول:
    حالة URES_TABLE16:
    حالة URES_TABLE32:
      r = res_get_جدول_قلم_By_شاخص(&resB->get_Res_داده(), resB->fRes, resB->f_شاخص, کلید);
      إذا(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->f_شاخص, len, وضعیت);
    حالة URES_ARRAY:
    حالة URES_ARRAY16:
      r = res_get_Array_قلم(&resB->get_Res_داده(), resB->fRes, resB->f_شاخص);
      إذا(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->f_شاخص, len, وضعیت);
    حالة URES_ALIAS:
      أرجع ures_getStringWithAlias(resB, resB->fRes, resB->f_شاخص, len, وضعیت);
    حالة URES_INT:
    حالة URES_دودویی:
    حالة URES_INT_وکتور:
        *وضعیت = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    افتراضي:
      أرجع اشاره_تهی;
    }
  }

  أرجع اشاره_تهی;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_بعدی_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_درون U_LIFETIME_BOUND,
                     UError_کد* وضعیت) {
    ثابت حرف *کلید = اشاره_تهی;
    Resource r = RES_BOGUS;

    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
            /*return nullptr;*/
            أرجع fill_درون;
    }
    إذا(resB == اشاره_تهی) {
            *وضعیت = U_ILLEGAL_آرگومان_ERROR;
            /*return nullptr;*/
            أرجع fill_درون;
    }

    إذا(resB->f_شاخص == resB->f_اندازه-1) {
      *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } وإلا {
        resB->f_شاخص++;
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_دودویی:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_وکتور:
            أرجع ures_copyResb(fill_درون, resB, وضعیت);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_قلم_By_شاخص(&resB->get_Res_داده(), resB->fRes, resB->f_شاخص, &کلید);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتیجه(resB->f_داده, r, کلید, resB->f_شاخص, resB, fill_درون, وضعیت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_قلم(&resB->get_Res_داده(), resB->fRes, resB->f_شاخص);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتیجه(resB->f_داده, r, کلید, resB->f_شاخص, resB, fill_درون, وضعیت);
        افتراضي:
            /*return nullptr;*/
            أرجع fill_درون;
        }
    }
    /*return nullptr;*/
    أرجع fill_درون;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_شاخص(ثابت UResourceBundle* resB,
                int32_t شاخص_R,
                UResourceBundle* fill_درون U_LIFETIME_BOUND,
                UError_کد* وضعیت) {
    ثابت حرف* کلید = اشاره_تهی;
    Resource r = RES_BOGUS;

    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        /*return nullptr;*/
        أرجع fill_درون;
    }
    إذا(resB == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        /*return nullptr;*/
        أرجع fill_درون;
    }

    إذا(شاخص_R >= 0 && resB->f_اندازه > شاخص_R) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_دودویی:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_وکتور:
            أرجع ures_copyResb(fill_درون, resB, وضعیت);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_قلم_By_شاخص(&resB->get_Res_داده(), resB->fRes, شاخص_R, &کلید);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتیجه(resB->f_داده, r, کلید, شاخص_R, resB, fill_درون, وضعیت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_قلم(&resB->get_Res_داده(), resB->fRes, شاخص_R);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتیجه(resB->f_داده, r, کلید, شاخص_R, resB, fill_درون, وضعیت);
        افتراضي:
            /*return nullptr;*/
            أرجع fill_درون;
        }
    } وإلا {
        *وضعیت = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    أرجع fill_درون;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_شاخص(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t شاخص_S,
                      int32_t* len,
                      UError_کد* وضعیت) {
    ثابت حرف* کلید = اشاره_تهی;
    Resource r = RES_BOGUS;

    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا(resB == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    إذا(شاخص_S >= 0 && resB->f_اندازه > شاخص_S) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_STRING:
        حالة URES_STRING_V2:
            أرجع res_getString({resB}, &resB->get_Res_داده(), resB->fRes, len);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_قلم_By_شاخص(&resB->get_Res_داده(), resB->fRes, شاخص_S, &کلید);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, شاخص_S, len, وضعیت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_قلم(&resB->get_Res_داده(), resB->fRes, شاخص_S);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, شاخص_S, len, وضعیت);
        حالة URES_ALIAS:
            أرجع ures_getStringWithAlias(resB, resB->fRes, شاخص_S, len, وضعیت);
        حالة URES_INT:
        حالة URES_دودویی:
        حالة URES_INT_وکتور:
            *وضعیت = U_RESOURCE_TYPE_MISMATCH;
            اكسر;
        افتراضي:
          /* must not occur */
          *وضعیت = U_داخلی_برنامه_ERROR;
          اكسر;
        }
    } وإلا {
        *وضعیت = U_MISSING_RESOURCE_ERROR;
    }
    أرجع اشاره_تهی;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_شاخص(ثابت UResourceBundle* resB,
                          int32_t idx,
                          حرف* dest U_LIFETIME_BOUND,
                          int32_t* p_درازی,
                          UBool forceCopy,
                          UError_کد* وضعیت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_شاخص(resB, idx, &length16, وضعیت);
    أرجع ures_مقصد_UTF8_String(s16, length16, dest, p_درازی, forceCopy, وضعیت);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_پیدا_کن_Resource(ثابت حرف* مسیر, UResourceBundle* fill_درون U_LIFETIME_BOUND, UError_کد* وضعیت)
{
  UResourceBundle *first = اشاره_تهی; 
  UResourceBundle *نتیجه = fill_درون;
  حرف *packageName = اشاره_تهی;
  حرف *مسیر_مقصد_Resource = اشاره_تهی, *save = اشاره_تهی;
  حرف *locale = اشاره_تهی, *locale_انتها = اشاره_تهی;
  int32_t length;

  إذا(وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)) {
    أرجع نتیجه;
  }

  length = (int32_t)(uprv_strlen(مسیر)+1);
  save = مسیر_مقصد_Resource = (حرف *)uprv_malloc(length*حجم(حرف));
  /* test for nullptr */
  إذا(مسیر_مقصد_Resource == اشاره_تهی) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع نتیجه;
  }
  uprv_memcpy(مسیر_مقصد_Resource, مسیر, length);

  locale = مسیر_مقصد_Resource;
  إذا(*مسیر_مقصد_Resource == RES_مسیر_SEPARATOR) { /* there is a path specification */
    مسیر_مقصد_Resource++;
    packageName = مسیر_مقصد_Resource;
    مسیر_مقصد_Resource = uprv_strchr(مسیر_مقصد_Resource, RES_مسیر_SEPARATOR);
    إذا(مسیر_مقصد_Resource == اشاره_تهی) {
      *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    } وإلا {
      *مسیر_مقصد_Resource = 0;
      locale = مسیر_مقصد_Resource+1;
    }
  }

  locale_انتها = uprv_strchr(locale, RES_مسیر_SEPARATOR);
  إذا(locale_انتها != اشاره_تهی) {
    *locale_انتها = 0;
  }

  first = ures_open(packageName, locale, وضعیت);

  إذا(U_SUCCESS(*وضعیت)) {
    إذا(locale_انتها) {
      نتیجه = ures_پیدا_کن_Sub_Resource(first, locale_انتها+1, fill_درون, وضعیت);
    } وإلا {
      نتیجه = ures_copyResb(fill_درون, first, وضعیت);
    }
    ures_close(first);
  }
  uprv_free(save);
  أرجع نتیجه;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_پیدا_کن_Sub_Resource(ثابت UResourceBundle* resB,
                     حرف* مسیر,
                     UResourceBundle* fill_درون U_LIFETIME_BOUND,
                     UError_کد* وضعیت)
{
  Resource res = RES_BOGUS;
  UResourceBundle *نتیجه = fill_درون;
  ثابت حرف *کلید;

  إذا(وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)) {
    أرجع نتیجه;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  افعل {
    res = res_پیدا_کن_Resource(&resB->get_Res_داده(), resB->fRes, &مسیر, &کلید); 
    إذا(res != RES_BOGUS) {
        نتیجه = init_resb_نتیجه(resB->f_داده, res, کلید, -1, resB, fill_درون, وضعیت);
        resB = نتیجه;
    } وإلا {
        *وضعیت = U_MISSING_RESOURCE_ERROR;
        اكسر;
    }
  } طالما(*مسیر); /* there is more stuff in the path */

  أرجع نتیجه;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلید_With_Fallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت حرف* درون_کلید,
                                int32_t* len,
                                UError_کد* وضعیت) {
    UResourceBundle پشته_2;
    ثابت char16_t* retVal = اشاره_تهی;
    ures_init_پشته_Object(&پشته_2);
    ures_get_By_کلید_With_Fallback(resB, درون_کلید, &پشته_2, وضعیت);
    int32_t length;
    retVal = ures_getString(&پشته_2, &length, وضعیت);
    ures_close(&پشته_2);
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (length == 3 && retVal[0] == خالی_مجموعه && retVal[1] == خالی_مجموعه && retVal[2] == خالی_مجموعه ) {
        retVal = اشاره_تهی;
        length = 0;
        *وضعیت = U_MISSING_RESOURCE_ERROR;
    }
    إذا (len != اشاره_تهی) {
        *len = length;
    }
    أرجع retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ساكن Resource get_جدول_قلم_By_کلید_مسیر(ثابت Resource_داده *p_Res_داده, Resource جدول, ثابت حرف *کلید) {
  Resource resource = جدول;  /* The current resource */
  icu::CharString مسیر;
  UError_کد error_کد = U_ZERO_ERROR;
  مسیر.append(کلید, error_کد);
  إذا (U_FAILURE(error_کد)) { أرجع RES_BOGUS; }
  حرف *مسیر_Part = مسیر.داده();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  طالما (*مسیر_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    حرف *بعدی_مسیر_Part = uprv_strchr(مسیر_Part, RES_مسیر_SEPARATOR);
    إذا (بعدی_مسیر_Part != اشاره_تهی) {
      *بعدی_مسیر_Part = 0;  /* Terminating null for this part of path. */
      بعدی_مسیر_Part++;
    } وإلا {
      بعدی_مسیر_Part = uprv_strchr(مسیر_Part, 0);
    }
    int32_t t;
    ثابت حرف *مسیر_P = مسیر_Part;
    resource = res_get_جدول_قلم_By_کلید(p_Res_داده, resource, &t, &مسیر_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    مسیر_Part = بعدی_مسیر_Part; 
  }
  إذا (*مسیر_Part) {
    أرجع RES_BOGUS;
  }
  أرجع resource;
}

ساكن فراغ create_مسیر(ثابت حرف* orig_Res_مسیر,
                       int32_t     orig_Res_مسیر_Len,
                       ثابت حرف* res_مسیر,
                       int32_t     res_مسیر_Len,
                       ثابت حرف* درون_کلید,
                       CharString& مسیر,
                       UError_کد* وضعیت) {
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
    مسیر.clear();
    ثابت حرف* کلید = درون_کلید;
    إذا (res_مسیر_Len > 0) {
        مسیر.append(res_مسیر, res_مسیر_Len, *وضعیت);
        إذا (U_SUCCESS(*وضعیت)) {
            ثابت حرف* res_مسیر_حد = res_مسیر + res_مسیر_Len;
            ثابت حرف* orig_Res_مسیر_حد = orig_Res_مسیر + orig_Res_مسیر_Len;
            ثابت حرف* res_مسیر_Ptr = res_مسیر;
            ثابت حرف* orig_Res_مسیر_Ptr = orig_Res_مسیر;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            طالما (orig_Res_مسیر_Ptr < orig_Res_مسیر_حد && res_مسیر_Ptr < res_مسیر_حد) {
                طالما (orig_Res_مسیر_Ptr < orig_Res_مسیر_حد && *orig_Res_مسیر_Ptr != RES_مسیر_SEPARATOR) {
                    ++orig_Res_مسیر_Ptr;
                }
                إذا (orig_Res_مسیر_Ptr < orig_Res_مسیر_حد && *orig_Res_مسیر_Ptr == RES_مسیر_SEPARATOR) {
                    ++orig_Res_مسیر_Ptr;
                }
                طالما (res_مسیر_Ptr < res_مسیر_حد && *res_مسیر_Ptr != RES_مسیر_SEPARATOR) {
                    ++res_مسیر_Ptr;
                }
                إذا (res_مسیر_Ptr < res_مسیر_حد && *res_مسیر_Ptr == RES_مسیر_SEPARATOR) {
                    ++res_مسیر_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            طالما (res_مسیر_Ptr < res_مسیر_حد && *کلید != '\0') {
                طالما (res_مسیر_Ptr < res_مسیر_حد && *res_مسیر_Ptr != RES_مسیر_SEPARATOR) {
                    ++res_مسیر_Ptr;
                }
                إذا (res_مسیر_Ptr < res_مسیر_حد && *res_مسیر_Ptr == RES_مسیر_SEPARATOR) {
                    ++res_مسیر_Ptr;
                }
                طالما (*کلید != '\0' && *کلید != RES_مسیر_SEPARATOR) {
                    ++کلید;
                }
                إذا (*کلید == RES_مسیر_SEPARATOR) {
                    ++کلید;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        مسیر.append(کلید, *وضعیت);
    } وإلا {
        مسیر.append(درون_کلید, *وضعیت);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلید_With_Fallback(ثابت UResourceBundle* resB,
                          ثابت حرف* درون_کلید,
                          UResourceBundle* fill_درون U_LIFETIME_BOUND,
                          UError_کد* وضعیت) {
    Resource res = RES_BOGUS, ریشه_Res = RES_BOGUS;
    UResourceBundle *helper = اشاره_تهی;

    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع fill_درون;
    }
    إذا(resB == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع fill_درون;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        ثابت حرف* orig_Res_مسیر = resB->f_Res_مسیر;
        int32_t orig_Res_مسیر_Len = resB->f_Res_مسیر_Len;
        res = get_جدول_قلم_By_کلید_مسیر(&resB->get_Res_داده(), resB->fRes, درون_کلید);
        ثابت حرف* کلید = درون_کلید;
        منطقي did_ریشه_Once = خطأ;
        إذا(res == RES_BOGUS) {
            UResource_داده_مدخل *داده_مدخل = resB->f_داده;
            CharString مسیر;
            حرف *my_مسیر = اشاره_تهی;
            ثابت حرف* res_مسیر = resB->f_Res_مسیر;
            int32_t len = resB->f_Res_مسیر_Len;
            طالما(res == RES_BOGUS && (داده_مدخل->f_والد != اشاره_تهی || !did_ریشه_Once)) { /* Otherwise, we'll look in parents */
                إذا (داده_مدخل->f_والد != اشاره_تهی) {
                    داده_مدخل = داده_مدخل->f_والد;
                } وإلا {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_ریشه_Once = درست;
                }
                ریشه_Res = داده_مدخل->f_داده.ریشه_Res;

                إذا(داده_مدخل->fBogus == U_ZERO_ERROR) {
                    create_مسیر(orig_Res_مسیر, orig_Res_مسیر_Len, res_مسیر, len, درون_کلید, مسیر, وضعیت);
                    إذا (U_FAILURE(*وضعیت)) {
                        ures_close(helper);
                        أرجع fill_درون;
                    }
                    my_مسیر = مسیر.داده();
                    کلید = درون_کلید;
                    افعل {
                        res = res_پیدا_کن_Resource(&(داده_مدخل->f_داده), ریشه_Res, &my_مسیر, &کلید);
                        إذا (RES_GET_TYPE(res) == URES_ALIAS && *my_مسیر) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_نتیجه(داده_مدخل, res, اشاره_تهی, -1, resB, helper, وضعیت);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            إذا(helper) {
                              داده_مدخل = helper->f_داده;
                              ریشه_Res = helper->fRes;
                              res_مسیر = helper->f_Res_مسیر;
                              len = helper->f_Res_مسیر_Len;

                            } وإلا {
                              اكسر;
                            }
                        } وإلا إذا (res == RES_BOGUS) {
                            اكسر;
                        }
                    } طالما(*my_مسیر); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            إذا(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                إذا(uprv_strcmp(داده_مدخل->fName, uloc_getDefault())==0 || uprv_strcmp(داده_مدخل->fName, k_ریشه_Locale_Name)==0) {
                    *وضعیت = U_USING_DEFAULT_WARNING;
                } وإلا {
                    *وضعیت = U_USING_FALLBACK_WARNING;
                }

                fill_درون = init_resb_نتیجه(داده_مدخل, res, کلید, -1, resB, fill_درون, وضعیت);
                إذا (res_مسیر != اشاره_تهی) {
                    create_مسیر(orig_Res_مسیر, orig_Res_مسیر_Len, res_مسیر, len, درون_کلید, مسیر, وضعیت);
                } وإلا {
                    ثابت حرف* separator = اشاره_تهی;
                    إذا (fill_درون->f_Res_مسیر != اشاره_تهی) {
                        separator = uprv_strchr(fill_درون->f_Res_مسیر, RES_مسیر_SEPARATOR);
                    }
                    إذا (separator != اشاره_تهی && separator[1] != '\0') {
                        create_مسیر(orig_Res_مسیر, orig_Res_مسیر_Len, fill_درون->f_Res_مسیر,
                                   static_cast<int32_t>(uprv_strlen(fill_درون->f_Res_مسیر)), درون_کلید, مسیر, وضعیت);
                    } وإلا {
                        create_مسیر(orig_Res_مسیر, orig_Res_مسیر_Len, "", 0, درون_کلید, مسیر, وضعیت);
                    }
                }
                ures_free_Res_مسیر(fill_درون);
                ures_append_Res_مسیر(fill_درون, مسیر.داده(), مسیر.length(), وضعیت);
                إذا(fill_درون->f_Res_مسیر[fill_درون->f_Res_مسیر_Len-1] != RES_مسیر_SEPARATOR) {
                    ures_append_Res_مسیر(fill_درون, RES_مسیر_SEPARATOR_S, 1, وضعیت);
                }
            } وإلا {
                *وضعیت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            fill_درون = init_resb_نتیجه(resB->f_داده, res, کلید, -1, resB, fill_درون, وضعیت);
        }
    } 
    وإلا {
        *وضعیت = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    أرجع fill_درون;
}

نطاق {

فراغ get_All_اقلام_With_Fallback(
        ثابت UResourceBundle *bundle, Resource_داده_مقدار &value,
        ResourceSink &sink, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع; }
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
    value.مجموعه_داده(bundle->get_Res_داده());
    value.مجموعه_Valid_Locale_داده_مدخل(bundle->f_Valid_Locale_داده_مدخل);
    UResource_داده_مدخل *والد_مدخل = bundle->f_داده->f_والد;
    UBool has_والد = والد_مدخل != اشاره_تهی && U_SUCCESS(والد_مدخل->fBogus);
    value.مجموعه_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_کلید, value, !has_والد, error_کد);
    إذا (has_والد) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        پشته_UResource_Bundle والد_Bundle;
        UResourceBundle &والد_Ref = والد_Bundle.ref();
        والد_Ref.f_داده = والد_مدخل;
        والد_Ref.f_Valid_Locale_داده_مدخل = bundle->f_Valid_Locale_داده_مدخل;
        والد_Ref.fHasFallback = !والد_Ref.get_Res_داده().noFallback;
        والد_Ref.f_Is_بالاترین_Level = درست;
        والد_Ref.fRes = والد_Ref.get_Res_داده().ریشه_Res;
        والد_Ref.f_اندازه = res_شمار_Array_اقلام(&والد_Ref.get_Res_داده(), والد_Ref.fRes);
        والد_Ref.f_شاخص = -1;
        مدخل_Increase(والد_مدخل);

        // Look up the container item in the parent bundle.
        پشته_UResource_Bundle containerBundle;
        ثابت UResourceBundle *rb;
        UError_کد مسیر_Error_کد = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        إذا (bundle->f_Res_مسیر == اشاره_تهی || *bundle->f_Res_مسیر == 0) {
            rb = والد_Bundle.getAlias();
        } وإلا {
            rb = ures_get_By_کلید_With_Fallback(والد_Bundle.getAlias(), bundle->f_Res_مسیر,
                                           containerBundle.getAlias(), &مسیر_Error_کد);
        }
        إذا (U_SUCCESS(مسیر_Error_کد)) {
            get_All_اقلام_With_Fallback(rb, value, sink, error_کد);
        }
    }
}

بنية GetAllChildrenSink : عام ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    مجازی ~GetAllChildrenSink() override;
    مجازی فراغ put(ثابت حرف *کلید, Resource_مقدار &value, UBool is_ریشه,
           UError_کد &error_کد) override {
        Resource_جدول اقلام_جدول = value.get_جدول(error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        لكل (int32_t i = 0; اقلام_جدول.get_کلید_And_مقدار(i, کلید, value); ++i) {
            إذا (value.getType() == URES_ALIAS) {
                Resource_داده_مقدار& rdv = static_cast<Resource_داده_مقدار&>(value);
                پشته_UResource_Bundle پشته_موقت_Bundle;
                UResourceBundle* aliasRB = get_Alias_هدف_As_Resource_Bundle(rdv.get_داده(), rdv.getResource(), اشاره_تهی, -1,
                                                                          rdv.get_Valid_Locale_داده_مدخل(), اشاره_تهی, 0,
                                                                          پشته_موقت_Bundle.getAlias(), &error_کد);
                إذا (U_SUCCESS(error_کد)) {
                    Resource_داده_مقدار aliased_مقدار;
                    aliased_مقدار.مجموعه_داده(aliasRB->get_Res_داده());
                    aliased_مقدار.مجموعه_Valid_Locale_داده_مدخل(aliasRB->f_Valid_Locale_داده_مدخل);
                    aliased_مقدار.مجموعه_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    إذا (aliased_مقدار.getType() != URES_جدول) {
                        dest.put(کلید, aliased_مقدار, is_ریشه, error_کد);
                    } وإلا {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_مقدار_Type = URES_جدول;
                        CharString جدول_مسیر;
                        جدول_مسیر.append(aliasRB->f_Res_مسیر, error_کد);
                        ثابت حرف* والد_کلید = کلید; // dest.put() changes the key
                        dest.put(والد_کلید, aliased_مقدار, is_ریشه, error_کد);
                        UResource_داده_مدخل* مدخل = aliasRB->f_داده;
                        Resource res = aliasRB->fRes;
                        طالما (aliased_مقدار_Type == URES_جدول && مدخل->f_والد != اشاره_تهی) {
                            CharString local_مسیر;
                            local_مسیر.copyFrom(جدول_مسیر, error_کد);
                            حرف* local_مسیر_As_Char_Ptr = local_مسیر.داده();
                            ثابت حرف* فرزند_کلید;
                            مدخل = مدخل->f_والد;
                            res = مدخل->f_داده.ریشه_Res;
                            Resource newRes = res_پیدا_کن_Resource(&مدخل->f_داده, res, &local_مسیر_As_Char_Ptr, &فرزند_کلید);
                            إذا (newRes != RES_BOGUS) {
                                aliased_مقدار.مجموعه_داده(مدخل->f_داده);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_مقدار.مجموعه_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_مقدار_Type = aliased_مقدار.getType();
                                إذا (aliased_مقدار_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_داده_مقدار& rdv2 = static_cast<Resource_داده_مقدار&>(aliased_مقدار);
                                    aliasRB = get_Alias_هدف_As_Resource_Bundle(rdv2.get_داده(), rdv2.getResource(), اشاره_تهی, -1,
                                                                             rdv2.get_Valid_Locale_داده_مدخل(), اشاره_تهی, 0,
                                                                             پشته_موقت_Bundle.getAlias(), &error_کد);
                                    جدول_مسیر.clear();
                                    جدول_مسیر.append(aliasRB->f_Res_مسیر, error_کد);
                                    مدخل = aliasRB->f_داده;
                                    res = aliasRB->fRes;
                                    aliased_مقدار.مجموعه_داده(مدخل->f_داده);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_مقدار.مجموعه_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_مقدار_Type = aliased_مقدار.getType();
                                }
                                إذا (aliased_مقدار_Type == URES_جدول) {
                                    dest.put(والد_کلید, aliased_مقدار, is_ریشه, error_کد);
                                } وإلا {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_کد = U_داخلی_برنامه_ERROR;
                                    أرجع;
                                }
                            }
                        }
                    }
                }
            } وإلا {
                dest.put(کلید, value, is_ریشه, error_کد);
            }
            إذا (U_FAILURE(error_کد)) { أرجع; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI فراغ U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *مسیر,
                                icu::ResourceSink &sink, UError_کد &error_کد) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_اقلام_With_Fallback(bundle, مسیر, allChildrenSink, error_کد);
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
ures_get_مقدار_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *مسیر,
                          UResourceBundle *موقت_Fill_درون,
                          Resource_داده_مقدار &value, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    إذا (مسیر == اشاره_تهی) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    ثابت UResourceBundle *rb;
    إذا (*مسیر == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_get_By_کلید_With_Fallback(bundle, مسیر, موقت_Fill_درون, &error_کد);
        إذا (U_FAILURE(error_کد)) {
            أرجع;
        }
    }
    value.مجموعه_داده(rb->get_Res_داده());
    value.مجموعه_Valid_Locale_داده_مدخل(rb->f_Valid_Locale_داده_مدخل);
    value.مجموعه_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI فراغ U_EXPORT2
ures_get_All_اقلام_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *مسیر,
                             icu::ResourceSink &sink, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    إذا (مسیر == اشاره_تهی) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    پشته_UResource_Bundle پشته_Bundle;
    ثابت UResourceBundle *rb;
    إذا (*مسیر == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_get_By_کلید_With_Fallback(bundle, مسیر, پشته_Bundle.getAlias(), &error_کد);
        إذا (U_FAILURE(error_کد)) {
            أرجع;
        }
    }
    // Get all table items with fallback.
    Resource_داده_مقدار value;
    get_All_اقلام_With_Fallback(rb, value, sink, error_کد);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلید(ثابت UResourceBundle* resB,
              ثابت حرف* درون_کلید,
              UResourceBundle* fill_درون U_LIFETIME_BOUND,
              UError_کد* وضعیت) {
    Resource res = RES_BOGUS;
    UResource_داده_مدخل *داده_مدخل = اشاره_تهی;
    ثابت حرف *کلید = درون_کلید;

    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع fill_درون;
    }
    إذا(resB == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع fill_درون;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        int32_t t;
        res = res_get_جدول_قلم_By_کلید(&resB->get_Res_داده(), resB->fRes, &t, &کلید);
        إذا(res == RES_BOGUS) {
            کلید = درون_کلید;
            إذا(resB->fHasFallback) {
                داده_مدخل = get_Fallback_داده(resB, &کلید, &res, وضعیت);
                إذا(U_SUCCESS(*وضعیت)) {
                    /* check if resB->fResPath gives the right name here */
                    أرجع init_resb_نتیجه(داده_مدخل, res, کلید, -1, resB, fill_درون, وضعیت);
                } وإلا {
                    *وضعیت = U_MISSING_RESOURCE_ERROR;
                }
            } وإلا {
                *وضعیت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            أرجع init_resb_نتیجه(resB->f_داده, res, کلید, -1, resB, fill_درون, وضعیت);
        }
    } 
#إذا 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    وإلا إذا(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == درست) {
        /* here should go a first attempt to locate the key using index table */
        داده_مدخل = get_Fallback_داده(resB, &کلید, &res, وضعیت);
        إذا(U_SUCCESS(*وضعیت)) {
            أرجع init_resb_نتیجه(داده_مدخل, res, کلید, resB, fill_درون, وضعیت);
        } وإلا {
            *وضعیت = U_MISSING_RESOURCE_ERROR;
        }
    }
#پایان_اگر    
    وإلا {
        *وضعیت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع fill_درون;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلید(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                    ثابت حرف* درون_کلید,
                    int32_t* len,
                    UError_کد* وضعیت) {
    Resource res = RES_BOGUS;
    UResource_داده_مدخل *داده_مدخل = اشاره_تهی;
    ثابت حرف* کلید = درون_کلید;

    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا(resB == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        int32_t t=0;

        res = res_get_جدول_قلم_By_کلید(&resB->get_Res_داده(), resB->fRes, &t, &کلید);

        إذا(res == RES_BOGUS) {
            کلید = درون_کلید;
            إذا(resB->fHasFallback) {
                داده_مدخل = get_Fallback_داده(resB, &کلید, &res, وضعیت);
                إذا(U_SUCCESS(*وضعیت)) {
                    بدل (RES_GET_TYPE(res)) {
                    حالة URES_STRING:
                    حالة URES_STRING_V2:
                        أرجع res_getString({resB, کلید}, &داده_مدخل->f_داده, res, len);
                    حالة URES_ALIAS:
                      {
                        ثابت char16_t* نتیجه = اشاره_تهی;
                        UResourceBundle *موقت_Res = ures_get_By_کلید(resB, درون_کلید, اشاره_تهی, وضعیت);
                        نتیجه = ures_getString(موقت_Res, len, وضعیت);
                        ures_close(موقت_Res);
                        أرجع نتیجه;
                      }
                    افتراضي:
                        *وضعیت = U_RESOURCE_TYPE_MISMATCH;
                    }
                } وإلا {
                    *وضعیت = U_MISSING_RESOURCE_ERROR;
                }
            } وإلا {
                *وضعیت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            بدل (RES_GET_TYPE(res)) {
            حالة URES_STRING:
            حالة URES_STRING_V2:
                أرجع res_getString({resB, کلید}, &resB->get_Res_داده(), res, len);
            حالة URES_ALIAS:
              {
                ثابت char16_t* نتیجه = اشاره_تهی;
                UResourceBundle *موقت_Res = ures_get_By_کلید(resB, درون_کلید, اشاره_تهی, وضعیت);
                نتیجه = ures_getString(موقت_Res, len, وضعیت);
                ures_close(موقت_Res);
                أرجع نتیجه;
              }
            افتراضي:
                *وضعیت = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#إذا 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    وإلا إذا(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == درست) {
        /* here should go a first attempt to locate the key using index table */
        داده_مدخل = get_Fallback_داده(resB, &کلید, &res, وضعیت);
        إذا(U_SUCCESS(*وضعیت)) {
            // TODO: Tracing
            أرجع res_getString(rd, res, len);
        } وإلا {
            *وضعیت = U_MISSING_RESOURCE_ERROR;
        }
    } 
#پایان_اگر    
    وإلا {
        *وضعیت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع اشاره_تهی;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_کلید(ثابت UResourceBundle* resB,
                        ثابت حرف* کلید,
                        حرف* dest U_LIFETIME_BOUND,
                        int32_t* p_درازی,
                        UBool forceCopy,
                        UError_کد* وضعیت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_کلید(resB, کلید, &length16, وضعیت);
    أرجع ures_مقصد_UTF8_String(s16, length16, dest, p_درازی, forceCopy, وضعیت);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ثابت حرف* U_EXPORT2
ures_get_Locale_داخلی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_کد* وضعیت)
{
    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (!resourceBundle) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    } وإلا {
      أرجع resourceBundle->f_داده->fName;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocale(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_کد* وضعیت)
{
  أرجع ures_get_Locale_داخلی(resourceBundle, وضعیت);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocaleByType(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_داده_Locale_Type type,
                     UError_کد* وضعیت) {
    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (!resourceBundle) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    } وإلا {
        بدل(type) {
        حالة ULOC_ACTUAL_LOCALE:
            أرجع resourceBundle->f_داده->fName;
        حالة ULOC_VALID_LOCALE:
            أرجع resourceBundle->f_Valid_Locale_داده_مدخل->fName;
        حالة ULOC_REQUESTED_LOCALE:
        افتراضي:
            *وضعیت = U_ILLEGAL_آرگومان_ERROR;
            أرجع اشاره_تهی;
        }
    }
}

U_CFUNC ثابت حرف*
ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  إذا(resB == اشاره_تهی) {
    أرجع اشاره_تهی;
  }

  أرجع resB->f_داده->fName;
}

#اگر_تعریف_شده URES_DEBUG
U_CFUNC ثابت حرف*
ures_get_مسیر(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  إذا(resB == اشاره_تهی) {
    أرجع اشاره_تهی;
  }

  أرجع resB->f_داده->f_مسیر;
}
#پایان_اگر

ساكن UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ثابت حرف* مسیر,
                  ثابت حرف* localeID,
                  UResOpenType openType,
                  UError_کد* وضعیت) {
    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    UResource_داده_مدخل *مدخل;
    إذا(openType != URES_OPEN_مستقیم) {
        إذا (localeID == اشاره_تهی) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_پایه_Name(localeID, *وضعیت);
        إذا(U_FAILURE(*وضعیت)) {
            *وضعیت = U_ILLEGAL_آرگومان_ERROR;
            أرجع اشاره_تهی;
        }
        مدخل = مدخل_Open(مسیر, canonLocaleID.داده(), openType, وضعیت);
    } وإلا {
        مدخل = مدخل_Open_مستقیم(مسیر, localeID, وضعیت);
    }
    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا(مدخل == اشاره_تهی) {
        *وضعیت = U_MISSING_RESOURCE_ERROR;
        أرجع اشاره_تهی;
    }

    UBool is_پشته_Object;
    إذا(r == اشاره_تهی) {
        r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        إذا(r == اشاره_تهی) {
            مدخل_Close(مدخل);
            *وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع اشاره_تهی;
        }
        is_پشته_Object = خطأ;
    } وإلا {  // fill-in
        is_پشته_Object = ures_is_پشته_Object(r);
        ures_closeBundle(r, خطأ);
    }
    uprv_memset(r, 0, حجم(UResourceBundle));
    ures_مجموعه_Is_پشته_Object(r, is_پشته_Object);

    r->f_Valid_Locale_داده_مدخل = r->f_داده = مدخل;
    r->fHasFallback = openType != URES_OPEN_مستقیم && !r->get_Res_داده().noFallback;
    r->f_Is_بالاترین_Level = درست;
    r->fRes = r->get_Res_داده().ریشه_Res;
    r->f_اندازه = res_شمار_Array_اقلام(&r->get_Res_داده(), r->fRes);
    r->f_شاخص = -1;

    ResourceTracer(r).traceOpen();

    أرجع r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ثابت حرف* مسیر, ثابت حرف* localeID, UError_کد* وضعیت) {
    أرجع ures_openWithType(اشاره_تهی, مسیر, localeID, URES_OPEN_LOCALE_DEFAULT_ریشه, وضعیت);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت حرف* مسیر, ثابت حرف* localeID, UError_کد* وضعیت) {
    أرجع ures_openWithType(اشاره_تهی, مسیر, localeID, URES_OPEN_LOCALE_ریشه, وضعیت);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_مستقیم(ثابت حرف* مسیر, ثابت حرف* localeID, UError_کد* وضعیت) {
    أرجع ures_openWithType(اشاره_تهی, مسیر, localeID, URES_OPEN_مستقیم, وضعیت);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_open_Fill_درون(UResourceBundle *r, ثابت حرف* مسیر,
                ثابت حرف* localeID, UError_کد* وضعیت) {
    إذا(U_SUCCESS(*وضعیت) && r == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    ures_openWithType(r, مسیر, localeID, URES_OPEN_LOCALE_DEFAULT_ریشه, وضعیت);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_open_مستقیم_Fill_درون(UResourceBundle *r, ثابت حرف* مسیر, ثابت حرف* localeID, UError_کد* وضعیت) {
    إذا(U_SUCCESS(*وضعیت) && r == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    ures_openWithType(r, مسیر, localeID, URES_OPEN_مستقیم, وضعیت);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_شمار_Array_اقلام(ثابت UResourceBundle* resourceBundle,
                  ثابت حرف* resource_کلید,
                  UError_کد* وضعیت)
{
    UResourceBundle res_داده;
    ures_init_پشته_Object(&res_داده);
    إذا (وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع 0;
    }
    إذا(resourceBundle == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    ures_get_By_کلید(resourceBundle, resource_کلید, &res_داده, وضعیت);
    
    إذا(res_داده.get_Res_داده().داده != اشاره_تهی) {
        int32_t نتیجه = res_شمار_Array_اقلام(&res_داده.get_Res_داده(), res_داده.fRes);
        ures_close(&res_داده);
        أرجع نتیجه;
    } وإلا {
        *وضعیت = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_داده);
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
ures_get_نسخه_عدد_داخلی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    إذا (!resourceBundle) أرجع اشاره_تهی;

    إذا(resourceBundle->f_نسخه == اشاره_تهی) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_کد وضعیت = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ثابت char16_t* minor_نسخه = ures_get_String_By_کلید(resourceBundle, k_نسخه_برچسب, &minor_len, &وضعیت);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_نسخه = (حرف *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        إذا (((UResourceBundle *)resourceBundle)->f_نسخه == اشاره_تهی) {
            أرجع اشاره_تهی;
        }
       
        إذا(minor_len > 0) {
            u_UChars_مقصد_Chars(minor_نسخه, resourceBundle->f_نسخه , minor_len);
            resourceBundle->f_نسخه[len] =  '\0';
        }
        وإلا {
            uprv_strcpy(resourceBundle->f_نسخه, k_Default_Minor_نسخه);
        }
    }

    أرجع resourceBundle->f_نسخه;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_نسخه_عدد(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    أرجع ures_get_نسخه_عدد_داخلی(resourceBundle);
}

U_CAPI فراغ U_EXPORT2 ures_get_نسخه(ثابت UResourceBundle* resB, UVersionInfo نسخه_Info) {
    إذا (!resB) أرجع;

    u_نسخه_From_String(نسخه_Info, ures_get_نسخه_عدد_داخلی(resB));
}

/** Tree support functions *******************************/
#تعریف_کن شاخص_LOCALE_NAME "res_index"
#تعریف_کن شاخص_برچسب         "InstalledLocales"
#تعریف_کن DEFAULT_برچسب       "default"

#إذا defined(URES_درخت_DEBUG)
#ضمن <stdio.h>
#پایان_اگر

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
ures_loc_شمار_Locales(UEnumeration *en, UError_کد * /*وضعیت*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    أرجع ures_get_اندازه(&ctx->installed);
}

U_CDECL_BEGIN


ساكن ثابت حرف * U_CALLCONV
ures_loc_بعدی_Locale(UEnumeration* en,
                    int32_t* نتیجه_درازی,
                    UError_کد* وضعیت) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = اشاره_تهی;
    ثابت حرف *نتیجه = اشاره_تهی;
    int32_t len = 0;
    إذا (ures_has_بعدی(res) && (k = ures_get_بعدی_Resource(res, &ctx->curr, وضعیت)) != اشاره_تهی) {
        نتیجه = ures_get_کلید(k);
        len = (int32_t)uprv_strlen(نتیجه);
    }
    إذا (نتیجه_درازی) {
        *نتیجه_درازی = len;
    }
    أرجع نتیجه;
}

ساكن فراغ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_کد* /*وضعیت*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_انتها

ساكن ثابت UEnumeration gLocalesEnum = {
    اشاره_تهی,
        اشاره_تهی,
        ures_loc_closeLocales,
        ures_loc_شمار_Locales,
        uenum_unextDefault,
        ures_loc_بعدی_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_موجود_Locales(ثابت حرف *مسیر, UError_کد *وضعیت)
{
    UResourceBundle *idx = اشاره_تهی;
    UEnumeration *en = اشاره_تهی;
    ULocalesContext *myContext = اشاره_تهی;

    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(حجم(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    إذا(!en || !myContext) {
        *وضعیت = U_حافظه_تخصیص_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        أرجع اشاره_تهی;
    }
    uprv_memcpy(en, &gLocalesEnum, حجم(UEnumeration));

    ures_init_پشته_Object(&myContext->installed);
    ures_init_پشته_Object(&myContext->curr);
    idx = ures_open_مستقیم(مسیر, شاخص_LOCALE_NAME, وضعیت);
    ures_get_By_کلید(idx, شاخص_برچسب, &myContext->installed, وضعیت);
    إذا(U_SUCCESS(*وضعیت)) {
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            مسیر, شاخص_LOCALE_NAME, شاخص_برچسب, ures_get_کلید(&myContext->installed));
#پایان_اگر
        en->context = myContext;
    } وإلا {
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", مسیر, u_errorName(*وضعیت));
#پایان_اگر
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = اشاره_تهی;
    }
    
    ures_close(idx);
    
    أرجع en;
}

ساكن UBool is_Locale_درون_List(UEnumeration *locEnum, ثابت حرف *loc_مقصد_جستجو, UError_کد *وضعیت) {
    ثابت حرف *loc;
    طالما ((loc = uenum_بعدی(locEnum, اشاره_تهی, وضعیت)) != اشاره_تهی) {
        إذا (uprv_strcmp(loc, loc_مقصد_جستجو) == 0) {
            أرجع درست;
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
    UError_کد sub_وضعیت = U_ZERO_ERROR;
    والد.clear();
    إذا (res != اشاره_تهی) {
        ures_get_By_کلید(res, "%%Parent", bund1, &sub_وضعیت);
        إذا (U_SUCCESS(sub_وضعیت)) {
            int32_t length16;
            ثابت char16_t* s16 = ures_getString(bund1, &length16, &sub_وضعیت);
            والد.appendInvariantChars(s16, length16, sub_وضعیت);
        }
    }
    
    // If none there, use normal truncation parent
    إذا (U_FAILURE(sub_وضعیت) || والد.is_خالی()) {
        sub_وضعیت = U_ZERO_ERROR;
        والد = ulocimp_get_والد(localeID, sub_وضعیت);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(حرف *نتیجه, int32_t نتیجه_ظرفیت,
                             ثابت حرف *مسیر, ثابت حرف *resName, ثابت حرف *keyword, ثابت حرف *locid,
                             UBool *is_موجود, UBool omitDefault, UError_کد *وضعیت)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString یافت;
    CharString والد;
    CharString پر;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = اشاره_تهی;
    UError_کد sub_وضعیت = U_ZERO_ERROR;
    int32_t length = 0;
    إذا(U_FAILURE(*وضعیت)) أرجع 0;
    CharString kwVal;
    إذا (keyword != اشاره_تهی && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_مقدار(locid, keyword, sub_وضعیت);
        إذا (kwVal == DEFAULT_برچسب) {
            kwVal.clear();
        }
    }
    إذا (locid == اشاره_تهی) {
        locid = uloc_getDefault();
    }
    CharString پایه = ulocimp_get_پایه_Name(locid, sub_وضعیت);
#إذا defined(URES_درخت_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.داده(), پایه.داده(), u_errorName(sub_وضعیت));
#پایان_اگر
    ures_init_پشته_Object(&bund1);
    ures_init_پشته_Object(&bund2);

    والد.copyFrom(پایه, sub_وضعیت);
    یافت.copyFrom(پایه, sub_وضعیت);

    إذا(is_موجود) {
        UEnumeration *locEnum = ures_open_موجود_Locales(مسیر, &sub_وضعیت);
        *is_موجود = درست;
        إذا (U_SUCCESS(sub_وضعیت)) {
            *is_موجود = is_Locale_درون_List(locEnum, والد.داده(), &sub_وضعیت);
        }
        uenum_close(locEnum);
    }

    إذا(U_FAILURE(sub_وضعیت)) {
        *وضعیت = sub_وضعیت;
        أرجع 0;
    }
    
    افعل {
        sub_وضعیت = U_ZERO_ERROR;
        res = ures_open(مسیر, والد.داده(), &sub_وضعیت);
        إذا(((sub_وضعیت == U_USING_FALLBACK_WARNING) ||
            (sub_وضعیت == U_USING_DEFAULT_WARNING)) && is_موجود)
        {
            *is_موجود = خطأ;
        }
        is_موجود = اشاره_تهی; /* only want to set this the first time around */
        
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", مسیر?مسیر:"ICUDATA", والد.داده(), u_errorName(sub_وضعیت), ures_getLocale(res, &sub_وضعیت));
#پایان_اگر
        إذا(U_FAILURE(sub_وضعیت)) {
            *وضعیت = sub_وضعیت;
        } وإلا إذا(sub_وضعیت == U_ZERO_ERROR) {
            ures_get_By_کلید(res,resName,&bund1, &sub_وضعیت);
            إذا(sub_وضعیت == U_ZERO_ERROR) {
                ثابت char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#إذا defined(URES_درخت_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    مسیر?مسیر:"ICUDATA", والد.داده(), u_errorName(sub_وضعیت));
#پایان_اگر
                defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_برچسب, &defLen, &sub_وضعیت);
                إذا(U_SUCCESS(sub_وضعیت) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_وضعیت);
#إذا defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        مسیر?مسیر:"ICUDATA", والد.داده(), keyword, defVal.داده(), u_errorName(sub_وضعیت));
#پایان_اگر
                    defLoc.copyFrom(والد, sub_وضعیت);
                    إذا(kwVal.is_خالی()) {
                        kwVal.append(defVal, sub_وضعیت);
#إذا defined(URES_درخت_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            مسیر?مسیر:"ICUDATA", والد.داده(), keyword, kwVal.داده());
#پایان_اگر
                    }
                }
            }
        }
        
        sub_وضعیت = U_ZERO_ERROR;

        إذا (res != اشاره_تهی) {
            یافت.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_وضعیت), sub_وضعیت);
        }

        إذا (یافت != والد) {
            والد.copyFrom(یافت, sub_وضعیت);
        } وإلا {
            get_والد_For_Functional_Equivalent(یافت.داده(),res,&bund1,والد);
        }
        ures_close(res);
    } طالما(defVal.is_خالی() && !یافت.is_خالی() && یافت != "root" && U_SUCCESS(*وضعیت));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    والد.copyFrom(پایه, sub_وضعیت);
    یافت.copyFrom(پایه, sub_وضعیت);

    افعل {
        res = ures_open(مسیر, والد.داده(), &sub_وضعیت);
        إذا((sub_وضعیت == U_USING_FALLBACK_WARNING) && is_موجود) {
            *is_موجود = خطأ;
        }
        is_موجود = اشاره_تهی; /* only want to set this the first time around */
        
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            مسیر?مسیر:"ICUDATA", والد.داده(), u_errorName(sub_وضعیت), kwVal.داده());
#پایان_اگر
        إذا(U_FAILURE(sub_وضعیت)) {
            *وضعیت = sub_وضعیت;
        } وإلا إذا(sub_وضعیت == U_ZERO_ERROR) {
            ures_get_By_کلید(res,resName,&bund1, &sub_وضعیت);
#إذا defined(URES_درخت_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_وضعیت));
#پایان_اگر
            إذا(sub_وضعیت == U_ZERO_ERROR) {
                ures_get_By_کلید(&bund1, kwVal.داده(), &bund2, &sub_وضعیت);
#إذا defined(URES_درخت_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.داده(), u_errorName(sub_وضعیت));
#پایان_اگر
                إذا(sub_وضعیت == U_ZERO_ERROR) {
#إذا defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        مسیر?مسیر:"ICUDATA", والد.داده(), keyword, kwVal.داده(), u_errorName(sub_وضعیت));
#پایان_اگر
                    إذا (والد.is_خالی()) {
                        پر.clear().append("root", sub_وضعیت);
                    } وإلا {
                        پر.copyFrom(والد, sub_وضعیت);
                    }
                        /* now, recalculate default kw if need be */
                        إذا(defLoc.length() > پر.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#إذا defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    مسیر?مسیر:"ICUDATA", پر.داده());
#پایان_اگر
                          defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_برچسب, &defLen, &sub_وضعیت);
                          إذا(U_SUCCESS(sub_وضعیت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_وضعیت);
#إذا defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    مسیر?مسیر:"ICUDATA", پر.داده(), keyword, defVal.داده(), u_errorName(sub_وضعیت));
#پایان_اگر
                            defLoc.copyFrom(پر, sub_وضعیت);
                          }
                        } /* end of recalculate default KW */
#إذا defined(URES_درخت_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.داده(), پر.داده());
                        }
#پایان_اگر
                } وإلا {
#إذا defined(URES_درخت_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_وضعیت), والد.داده(), kwVal.داده());
#پایان_اگر
                }
            }
        }
        
        sub_وضعیت = U_ZERO_ERROR;
        UBool have_یافت = خطأ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        إذا (res != اشاره_تهی && uprv_strcmp(resName, "collations") == 0) {
            ثابت حرف *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_وضعیت);
            إذا (U_SUCCESS(sub_وضعیت) && validLoc != اشاره_تهی && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_وضعیت);
                CharString والد_Lang = ulocimp_getLanguage(والد.مقصد_String_Piece(), sub_وضعیت);
                إذا (U_SUCCESS(sub_وضعیت) && validLang != والد_Lang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    یافت.clear().append(validLoc, sub_وضعیت);
                    have_یافت = درست;
                }
            }
            sub_وضعیت = U_ZERO_ERROR;
        }
        إذا (!have_یافت) {
            یافت.copyFrom(والد, sub_وضعیت);
        }

        get_والد_For_Functional_Equivalent(یافت.داده(),res,&bund1,والد);
        ures_close(res);
        sub_وضعیت = U_ZERO_ERROR;
    } طالما(پر.is_خالی() && !یافت.is_خالی() && U_SUCCESS(*وضعیت));

    إذا(پر.is_خالی() && kwVal != defVal) {
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.داده(), defVal.داده());
#پایان_اگر
        kwVal.clear().append(defVal, sub_وضعیت);
        والد.copyFrom(پایه, sub_وضعیت);
        یافت.copyFrom(پایه, sub_وضعیت);

        افعل { /* search for 'default' named item */
            res = ures_open(مسیر, والد.داده(), &sub_وضعیت);
            إذا((sub_وضعیت == U_USING_FALLBACK_WARNING) && is_موجود) {
                *is_موجود = خطأ;
            }
            is_موجود = اشاره_تهی; /* only want to set this the first time around */
            
#إذا defined(URES_درخت_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                مسیر?مسیر:"ICUDATA", والد.داده(), u_errorName(sub_وضعیت), kwVal.داده());
#پایان_اگر
            إذا(U_FAILURE(sub_وضعیت)) {
                *وضعیت = sub_وضعیت;
            } وإلا إذا(sub_وضعیت == U_ZERO_ERROR) {
                ures_get_By_کلید(res,resName,&bund1, &sub_وضعیت);
                إذا(sub_وضعیت == U_ZERO_ERROR) {
                    ures_get_By_کلید(&bund1, kwVal.داده(), &bund2, &sub_وضعیت);
                    إذا(sub_وضعیت == U_ZERO_ERROR) {
#إذا defined(URES_درخت_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", مسیر?مسیر:"ICUDATA",
                            والد.داده(), keyword, kwVal.داده(), u_errorName(sub_وضعیت));
#پایان_اگر
                        إذا (والد.is_خالی()) {
                            پر.clear().append("root", sub_وضعیت);
                        } وإلا {
                            پر.copyFrom(والد, sub_وضعیت);
                        }
                        
                        /* now, recalculate default kw if need be */
                        إذا(defLoc.length() > پر.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#إذا defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    مسیر?مسیر:"ICUDATA", پر.داده());
#پایان_اگر
                          defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_برچسب, &defLen, &sub_وضعیت);
                          إذا(U_SUCCESS(sub_وضعیت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_وضعیت);
#إذا defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    مسیر?مسیر:"ICUDATA", پر.داده(), keyword, defVal.داده(), u_errorName(sub_وضعیت));
#پایان_اگر
                            defLoc.copyFrom(پر, sub_وضعیت);
                          }
                        } /* end of recalculate default KW */
#إذا defined(URES_درخت_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.داده(), پر.داده());
                        }
#پایان_اگر
                    }
                }
            }
            
            sub_وضعیت = U_ZERO_ERROR;
            یافت.copyFrom(والد, sub_وضعیت);
            get_والد_For_Functional_Equivalent(یافت.داده(),res,&bund1,والد);
            ures_close(res);
            sub_وضعیت = U_ZERO_ERROR;
        } طالما(پر.is_خالی() && !یافت.is_خالی() && U_SUCCESS(*وضعیت));
    }
    
    إذا(U_SUCCESS(*وضعیت)) {
        إذا(پر.is_خالی()) {
#إذا defined(URES_درخت_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.داده());
#پایان_اگر
          *وضعیت = U_MISSING_RESOURCE_ERROR;
        } وإلا إذا(omitDefault) {
#إذا defined(URES_درخت_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", پر.داده(), defLoc.داده(), یافت.داده());
#پایان_اگر        
          إذا(defLoc.length() <= پر.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            إذا(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#إذا defined(URES_درخت_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.داده());
#پایان_اگر
              kwVal.clear();
            }
          }
        }
        یافت.copyFrom(پر, sub_وضعیت);
        إذا(!kwVal.is_خالی()) {
            یافت
                .append("@", sub_وضعیت)
                .append(keyword, sub_وضعیت)
                .append("=", sub_وضعیت)
                .append(kwVal, sub_وضعیت);
        } وإلا إذا(!omitDefault) {
            یافت
                .append("@", sub_وضعیت)
                .append(keyword, sub_وضعیت)
                .append("=", sub_وضعیت)
                .append(defVal, sub_وضعیت);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = یافت.length();

    إذا(U_SUCCESS(*وضعیت)) {
        int32_t copy_درازی = uprv_min(length, نتیجه_ظرفیت);
        إذا(copy_درازی>0) {
            یافت.extract(نتیجه, copy_درازی, sub_وضعیت);
        }
        إذا(length == 0) {
          *وضعیت = U_MISSING_RESOURCE_ERROR; 
        }
    } وإلا {
        length = 0;
        نتیجه[0]=0;
    }
    أرجع u_terminateChars(نتیجه, نتیجه_ظرفیت, length, وضعیت);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_مقادیر(ثابت حرف *مسیر, ثابت حرف *keyword, UError_کد *وضعیت)
{
#تعریف_کن مقادیر_BUF_اندازه 2048
#تعریف_کن مقادیر_LIST_اندازه 512
    
    حرف       مقادیر_Buf[مقادیر_BUF_اندازه];
    int32_t    مقادیر_شاخص = 0;
    ثابت حرف *مقادیر_List[مقادیر_LIST_اندازه];
    int32_t    مقادیر_شمار = 0;
    
    ثابت حرف *locale;
    int32_t     locLen;
    
    UEnumeration *locs = اشاره_تهی;
    
    UResourceBundle    قلم;
    UResourceBundle    sub_قلم;
    
    ures_init_پشته_Object(&قلم);
    ures_init_پشته_Object(&sub_قلم);
    locs = ures_open_موجود_Locales(مسیر, وضعیت);
    
    إذا(U_FAILURE(*وضعیت)) {
        ures_close(&قلم);
        ures_close(&sub_قلم);
        أرجع اشاره_تهی;
    }
    
    مقادیر_Buf[0]=0;
    مقادیر_Buf[1]=0;

    طالما ((locale = uenum_بعدی(locs, &locLen, وضعیت)) != اشاره_تهی) {
        UResourceBundle   *bund = اشاره_تهی;
        UResourceBundle   *subPtr = اشاره_تهی;
        UError_کد sub_وضعیت = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(مسیر, locale, &sub_وضعیت);
        
#إذا defined(URES_درخت_DEBUG)
        إذا(!bund || U_FAILURE(sub_وضعیت)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                مسیر?مسیر:"<ICUDATA>", keyword, locale, u_errorName(sub_وضعیت));
        }
#پایان_اگر
        
        ures_get_By_کلید(bund, keyword, &قلم, &sub_وضعیت);
        
        إذا(!bund || U_FAILURE(sub_وضعیت)) {
#إذا defined(URES_درخت_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                مسیر?مسیر:"<ICUDATA>", keyword, locale, u_errorName(sub_وضعیت));
#پایان_اگر
            ures_close(bund);
            bund = اشاره_تهی;
            استمر;
        }

        طالما ((subPtr = ures_get_بعدی_Resource(&قلم, &sub_قلم, &sub_وضعیت)) != اشاره_تهی
            && U_SUCCESS(sub_وضعیت)) {
            ثابت حرف *k;
            int32_t i;
            k = ures_get_کلید(subPtr);

#إذا defined(URES_درخت_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#پایان_اگر
            إذا(k == اشاره_تهی || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_برچسب) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                استمر;
            }
            لكل(i=0; i<مقادیر_شمار; i++) {
                إذا(!uprv_strcmp(مقادیر_List[i],k)) {
                    k = اشاره_تهی; /* found duplicate */
                    اكسر;
                }
            }
            إذا(k != اشاره_تهی) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                إذا((مقادیر_شمار >= (مقادیر_LIST_اندازه-1)) ||       /* no more space in list .. */
                    ((مقادیر_شاخص+kLen+1+1) >= مقادیر_BUF_اندازه)) { /* no more space in buffer (string + 2 nulls) */
                    *وضعیت = U_ILLEGAL_آرگومان_ERROR; /* out of space.. */
                } وإلا {
                    uprv_strcpy(مقادیر_Buf+مقادیر_شاخص, k);
                    مقادیر_List[مقادیر_شمار++] = مقادیر_Buf+مقادیر_شاخص;
                    مقادیر_شاخص += kLen;
#إذا defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        مسیر?مسیر:"<ICUDATA>", keyword, locale, k);
#پایان_اگر
                    مقادیر_Buf[مقادیر_شاخص++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    مقادیر_Buf[مقادیر_شاخص++] = 0; /* terminate */
    
    ures_close(&قلم);
    ures_close(&sub_قلم);
    uenum_close(locs);
#إذا defined(URES_درخت_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*وضعیت), 
        مقادیر_شاخص, مقادیر_شمار);
#پایان_اگر
    أرجع uloc_openKeywordList(مقادیر_Buf, مقادیر_شاخص, وضعیت);
}
#إذا 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ثابت UResourceBundle* res1, ثابت UResourceBundle* res2){
    إذا(res1==اشاره_تهی || res2==اشاره_تهی){
        أرجع res1==res2; /* pointer comparison */
    }
    إذا(res1->f_کلید==اشاره_تهی||  res2->f_کلید==اشاره_تهی){
        أرجع (res1->f_کلید==res2->f_کلید);
    }وإلا{
        إذا(uprv_strcmp(res1->f_کلید, res2->f_کلید)!=0){
            أرجع خطأ;
        }
    }
    إذا(uprv_strcmp(res1->f_داده->fName, res2->f_داده->fName)!=0){
        أرجع خطأ;
    }
    إذا(res1->f_داده->f_مسیر == اشاره_تهی||  res2->f_داده->f_مسیر==اشاره_تهی){
        أرجع (res1->f_داده->f_مسیر == res2->f_داده->f_مسیر);
    }وإلا{
        إذا(uprv_strcmp(res1->f_داده->f_مسیر, res2->f_داده->f_مسیر)!=0){
            أرجع خطأ;
        }
    }
    إذا(uprv_strcmp(res1->f_داده->f_والد->fName, res2->f_داده->f_والد->fName)!=0){
        أرجع خطأ;
    }
    إذا(uprv_strcmp(res1->f_داده->f_والد->f_مسیر, res2->f_داده->f_والد->f_مسیر)!=0){
        أرجع خطأ;
    }
    إذا(uprv_strncmp(res1->f_Res_مسیر, res2->f_Res_مسیر, res1->f_Res_مسیر_Len)!=0){
        أرجع خطأ;
    }
    إذا(res1->fRes != res2->fRes){
        أرجع خطأ;
    }
    أرجع درست;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ثابت UResourceBundle* res, UError_کد* وضعیت){
    UResourceBundle* bundle = اشاره_تهی;
    UResourceBundle* ret = اشاره_تهی;
    إذا(U_FAILURE(*وضعیت) || res == اشاره_تهی){
        أرجع اشاره_تهی;
    }
    bundle = ures_open(res->f_داده->f_مسیر, res->f_داده->fName, وضعیت);
    إذا(res->f_Res_مسیر!=اشاره_تهی){
        ret = ures_پیدا_کن_Sub_Resource(bundle, res->f_Res_مسیر, اشاره_تهی, وضعیت);
        ures_close(bundle);
    }وإلا{
        ret = bundle;
    }
    أرجع ret;
}
U_CAPI ثابت UResourceBundle* U_EXPORT2
ures_get_والد_Bundle(ثابت UResourceBundle* res){
    إذا(res==اشاره_تهی){
        أرجع اشاره_تهی;
    }
    أرجع res->f_والد_Res;
}
#پایان_اگر

U_CAPI فراغ U_EXPORT2
ures_get_نسخه_By_کلید(ثابت UResourceBundle* res, ثابت حرف *کلید, UVersionInfo ver, UError_کد *وضعیت) {
  ثابت char16_t *str;
  int32_t len;
  str = ures_get_String_By_کلید(res, کلید, &len, وضعیت);
  إذا(U_SUCCESS(*وضعیت)) {
    u_نسخه_From_UString(ver, str);
  } 
}

/* eof */
