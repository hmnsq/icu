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

ورتوں نطاق icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ساكن UHashtable *cache = خالی_سوچک;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ساكن int32_t U_CALLCONV hash_اندراج(ثابت UHashTok parm) {
    UResource_انکڑے_اندراج* b = static_cast<UResource_انکڑے_اندراج*>(parm.اشارہ_گر);
    UHashTok namekey, pathkey;
    namekey.اشارہ_گر = b->fName;
    pathkey.اشارہ_گر = b->f_رستہ;
    أرجع uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ساكن UBool U_CALLCONV compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UResource_انکڑے_اندراج* b1 = static_cast<UResource_انکڑے_اندراج*>(p1.اشارہ_گر);
    UResource_انکڑے_اندراج* b2 = static_cast<UResource_انکڑے_اندراج*>(p2.اشارہ_گر);
    UHashTok name1, name2, path1, path2;
    name1.اشارہ_گر = b1->fName;
    name2.اشارہ_گر = b2->fName;
    path1.اشارہ_گر = b1->f_رستہ;
    path2.اشارہ_گر = b2->f_رستہ;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ساكن UBool chopLocale(حرف *name) {
    حرف *i = uprv_strrchr(name, '_');

    إذا(i != خالی_سوچک) {
        *i = '\0';
        أرجع سچ;
    }

    أرجع خطأ;
}

ساكن UBool hasVariant(ثابت حرف* localeID) {
    UError_کوڈ err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(خالی_سوچک, 0);
    ulocimp_getSubtags(
            localeID,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            &sink,
            خالی_سوچک,
            err);
    أرجع sink.نمبر_دا_بائٹاں_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#تعریف_کرو INCLUDED_FROM_URESBUND_CPP
#ضمن "localefallback_data.h"

ساكن ثابت حرف* performFallbackLookup(ثابت حرف* کلید,
                                         ثابت حرف* کلید_Strs,
                                         ثابت حرف* قدر_Strs U_LIFETIME_BOUND,
                                         ثابت int32_t* lookup_جدول,
                                         int32_t lookup_جدول_لمائی) {
    ثابت int32_t* bottom = lookup_جدول;
    ثابت int32_t* چوٹی = lookup_جدول + lookup_جدول_لمائی;

    طالما (bottom < چوٹی) {
        // Effectively, divide by 2 and round down to an even index
        ثابت int32_t* درمیان = bottom + (((چوٹی - bottom) / 4) * 2);
        ثابت حرف* اندراج_کلید = &(کلید_Strs[*درمیان]);
        int32_t strcmp_نتیجہ = uprv_strcmp(کلید, اندراج_کلید);
        إذا (strcmp_نتیجہ == 0) {
            أرجع &(قدر_Strs[درمیان[1]]);
        } وإلا إذا (strcmp_نتیجہ < 0) {
            چوٹی = درمیان;
        } وإلا {
            bottom = درمیان + 2;
        }
    }
    أرجع خالی_سوچک;
}

ساكن CharString getDefaultScript(ثابت CharString& language, ثابت CharString& region) {
    ثابت حرف* defaultScript = خالی_سوچک;
    UError_کوڈ err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString نتیجہ("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    إذا (!region.is_خالی_تھاواں()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        إذا (U_FAILURE(err)) {
            أرجع نتیجہ;
        }
        defaultScript = performFallbackLookup(localeID.انکڑے(), dsLocaleIDChars, script_کوڈ_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    إذا (defaultScript == خالی_سوچک) {
        defaultScript = performFallbackLookup(language.انکڑے(), dsLocaleIDChars, script_کوڈ_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    إذا (defaultScript != خالی_سوچک) {
        نتیجہ.clear();
        نتیجہ.append(defaultScript, err);
    }
    أرجع نتیجہ;
}

تعداد UResOpenType {
    /**
     * Open a resource bundle for the locale;
     * if there is not even a base language bundle, then fall back to the default locale;
     * if there is no bundle for that either, then load the root bundle.
     *
     * This is the default bundle loading behavior.
     */
    URES_OPEN_LOCALE_DEFAULT_جڑ,
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
    URES_OPEN_LOCALE_جڑ,
    /**
     * Open a resource bundle for the exact bundle name as requested;
     * no fallbacks, do not load parent bundles.
     *
     * This is used for supplemental (non-locale) data.
     */
    URES_OPEN_سدھا
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
ساكن منطقي get_ماپا_Locale_ID(حرف *name, ثابت حرف *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    إذا (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        أرجع chopLocale(name);
    }
    
    UError_کوڈ err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, خالی_سوچک, خالی_سوچک, err);

    إذا (U_FAILURE(err)) {
        // hopefully this never happens...
        أرجع chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    إذا (openType == URES_OPEN_LOCALE_DEFAULT_جڑ) {
        ثابت حرف* ماپا_ID = performFallbackLookup(name, ماپا_Locale_Chars, ماپا_Locale_Chars, ماپا_Locale_جدول, UPRV_LENGTHOF(ماپا_Locale_جدول));
        إذا (ماپا_ID != خالی_سوچک) {
            uprv_strcpy(name, ماپا_ID);
            أرجع سچ;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    إذا (!script.is_خالی_تھاواں() && !region.is_خالی_تھاواں()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        إذا (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } وإلا إذا (!region.is_خالی_تھاواں()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_کوڈ err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, خالی_سوچک, خالی_سوچک, خالی_سوچک, err);
        إذا (!origNameScript.is_خالی_تھاواں()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } وإلا {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } وإلا إذا (!script.is_خالی_تھاواں()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        إذا (openType != URES_OPEN_LOCALE_DEFAULT_جڑ || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } وإلا {
            أرجع خطأ;
        }
    } وإلا {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        أرجع خطأ;
    }
    إذا (U_SUCCESS(err) && !workingLocale.is_خالی_تھاواں()) {
        uprv_strcpy(name, workingLocale.انکڑے());
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
ساكن UBool may_Have_ماپا(حرف *name) {
    أرجع (name[0] != 0 && uprv_strstr("nb nn",name) != خالی_سوچک);
}

/**
 *  Internal function
 */
ساكن فراغ اندراج_Increase(UResource_انکڑے_اندراج *اندراج) {
    Mutex جندرا(&resbMutex);
    اندراج->f_گنتی_Existing++;
    طالما(اندراج->f_ماپا != خالی_سوچک) {
      اندراج = اندراج->f_ماپا;
      اندراج->f_گنتی_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ساكن UResource_انکڑے_اندراج *get_Fallback_انکڑے(
        ثابت UResourceBundle *resBundle,
        ثابت حرف **res_ٹیگ, Resource *res, UError_کوڈ *حالت) {
    UResource_انکڑے_اندراج *انکڑے_اندراج = resBundle->f_انکڑے;
    int32_t اشاریہ_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    إذا(انکڑے_اندراج == خالی_سوچک) {
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع خالی_سوچک;
    }
    إذا(انکڑے_اندراج->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_جدول_شے_By_کلید(&(انکڑے_اندراج->f_انکڑے), انکڑے_اندراج->f_انکڑے.جڑ_Res, &اشاریہ_R, res_ٹیگ); /* try to get data from there */
        i++;
    }
    إذا(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        طالما(*res == RES_BOGUS && انکڑے_اندراج->f_ماپا != خالی_سوچک) {
            انکڑے_اندراج = انکڑے_اندراج->f_ماپا;
            إذا(انکڑے_اندراج->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_جدول_شے_By_کلید(&(انکڑے_اندراج->f_انکڑے), انکڑے_اندراج->f_انکڑے.جڑ_Res, &اشاریہ_R, res_ٹیگ);
            }
        }
    }

    إذا(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع خالی_سوچک;
    }
    // If the resource is found in parents, we need to adjust the error.
    إذا(i>1) {
        إذا(uprv_strcmp(انکڑے_اندراج->fName, uloc_getDefault())==0 || uprv_strcmp(انکڑے_اندراج->fName, k_جڑ_Locale_Name)==0) {
            *حالت = U_USING_DEFAULT_WARNING;
        } وإلا {
            *حالت = U_USING_FALLBACK_WARNING;
        }
    }
    أرجع انکڑے_اندراج;
}

ساكن فراغ
free_اندراج(UResource_انکڑے_اندراج *اندراج) {
    UResource_انکڑے_اندراج *alias;
    res_unload(&(اندراج->f_انکڑے));
    إذا(اندراج->fName != خالی_سوچک && اندراج->fName != اندراج->f_Name_بفر) {
        uprv_free(اندراج->fName);
    }
    إذا(اندراج->f_رستہ != خالی_سوچک) {
        uprv_free(اندراج->f_رستہ);
    }
    إذا(اندراج->fPool != خالی_سوچک) {
        --اندراج->fPool->f_گنتی_Existing;
    }
    alias = اندراج->fAlias;
    إذا(alias != خالی_سوچک) {
        طالما(alias->fAlias != خالی_سوچک) {
            alias = alias->fAlias;
        }
        --alias->f_گنتی_Existing;
    }
    uprv_free(اندراج);
}

/* Works just like ucnv_flushCache() */
ساكن int32_t ures_flushCache()
{
    UResource_انکڑے_اندراج *resB;
    int32_t pos;
    int32_t rb_Deleted_عدد = 0;
    ثابت UHashElement *e;
    UBool deleted_ہور;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex جندرا(&resbMutex);
    إذا (cache == خالی_سوچک) {
        أرجع 0;
    }

    افعل {
        deleted_ہور = خطأ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_پہلا;
        طالما ((e = uhash_اگلا_Element(cache, &pos)) != خالی_سوچک)
        {
            resB = static_cast<UResource_انکڑے_اندراج*>(e->value.اشارہ_گر);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            إذا (resB->f_گنتی_Existing == 0) {
                rb_Deleted_عدد++;
                deleted_ہور = سچ;
                uhash_removeElement(cache, e);
                free_اندراج(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } طالما(deleted_ہور);

    أرجع rb_Deleted_عدد;
}

#جے_تعریف_شدہ URES_DEBUG
#ضمن <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_خالی_تھاواں = خطأ;
  int32_t pos = UHASH_پہلا;
  ثابت UHashElement *e;
  UResource_انکڑے_اندراج *resB;
  
    Mutex جندرا(&resbMutex);
    إذا (cache == خالی_سوچک) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      أرجع خطأ;
    }

    طالما ((e = uhash_اگلا_Element(cache, &pos)) != خالی_سوچک) {
      cache_Not_خالی_تھاواں=سچ;
      resB = (UResource_انکڑے_اندراج *) e->value.اشارہ_گر;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (فراغ*)resB, resB->f_گنتی_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_رستہ?resB->f_رستہ:"nullptr",
              (فراغ*)resB->fPool,
              (فراغ*)resB->fAlias,
              (فراغ*)resB->f_ماپا);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_گنتی(cache));
    أرجع cache_Not_خالی_تھاواں;
}

#جے_انت

ساكن UBool U_CALLCONV ures_cleanup()
{
    إذا (cache != خالی_سوچک) {
        ures_flushCache();
        uhash_close(cache);
        cache = خالی_سوچک;
    }
    gCacheInitOnce.reset();
    أرجع سچ;
}

/** INTERNAL: Initializes the cache for resources */
ساكن فراغ U_CALLCONV createCache(UError_کوڈ &حالت) {
    U_ASSERT(cache == خالی_سوچک);
    cache = uhash_open(hash_اندراج, compareEntries, خالی_سوچک, &حالت);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ساكن فراغ initCache(UError_کوڈ *حالت) {
    umtx_initOnce(gCacheInitOnce, &createCache, *حالت);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ساكن فراغ سیٹ_اندراج_Name(UResource_انکڑے_اندراج *res, ثابت حرف *name, UError_کوڈ *حالت) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    إذا(res->fName != خالی_سوچک && res->fName != res->f_Name_بفر) {
        uprv_free(res->fName);
    }
    إذا (len < static_cast<int32_t>(حجم(res->f_Name_بفر))) {
        res->fName = res->f_Name_بفر;
    }
    وإلا {
        res->fName = static_cast<حرف*>(uprv_malloc(len + 1));
    }
    إذا(res->fName == خالی_سوچک) {
        *حالت = U_حافظہ_ونڈ_ERROR;
    } وإلا {
        uprv_strcpy(res->fName, name);
    }
}

ساكن UResource_انکڑے_اندراج *
get_Pool_اندراج(ثابت حرف *رستہ, UError_کوڈ *حالت);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ساكن UResource_انکڑے_اندراج *init_اندراج(ثابت حرف *localeID, ثابت حرف *رستہ, UError_کوڈ *حالت) {
    UResource_انکڑے_اندراج *r = خالی_سوچک;
    UResource_انکڑے_اندراج find;
    /*int32_t hashValue;*/
    ثابت حرف *name;
    حرف aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    /* here we try to deduce the right locale name */
    إذا(localeID == خالی_سوچک) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } وإلا إذا(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_جڑ_Locale_Name;
    } وإلا { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<حرف*>(name);
    find.f_رستہ = const_cast<حرف*>(رستہ);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_انکڑے_اندراج*>(uhash_get(cache, &find));
    إذا(r == خالی_سوچک) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_انکڑے_اندراج*>(uprv_malloc(حجم(UResource_انکڑے_اندراج)));
        إذا(r == خالی_سوچک) {
            *حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }

        uprv_memset(r, 0, حجم(UResource_انکڑے_اندراج));
        /*r->fHashKey = hashValue;*/

        سیٹ_اندراج_Name(r, name, حالت);
        إذا (U_FAILURE(*حالت)) {
            uprv_free(r);
            أرجع خالی_سوچک;
        }

        إذا(رستہ != خالی_سوچک) {
            r->f_رستہ = uprv_strdup(رستہ);
            إذا(r->f_رستہ == خالی_سوچک) {
                *حالت = U_حافظہ_ونڈ_ERROR;
                uprv_free(r);
                أرجع خالی_سوچک;
            }
        }

        /* this is the actual loading */
        res_لدو(&(r->f_انکڑے), r->f_رستہ, r->fName, حالت);

        إذا (U_FAILURE(*حالت)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            إذا (*حالت == U_حافظہ_ونڈ_ERROR) {
                uprv_free(r);
                أرجع خالی_سوچک;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *حالت = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } وإلا { /* if we have a regular entry */
            Resource aliasres;
            إذا (r->f_انکڑے.ورتدا_Pool_Bundle) {
                r->fPool = get_Pool_اندراج(r->f_رستہ, حالت);
                إذا (U_SUCCESS(*حالت)) {
                    ثابت int32_t *poolIndexes = r->fPool->f_انکڑے.p_جڑ + 1;
                    إذا(r->f_انکڑے.p_جڑ[1 + URES_اشاریہ_POOL_CHECKSUM] == poolIndexes[URES_اشاریہ_POOL_CHECKSUM]) {
                        r->f_انکڑے.pool_Bundle_کلیدیں = reinterpret_cast<ثابت حرف*>(poolIndexes + (poolIndexes[URES_اشاریہ_لمائی] & 0xff));
                        r->f_انکڑے.pool_Bundle_لڑیاں = r->fPool->f_انکڑے.p16_ثنائی_ہندسہ_Units;
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
                aliasres = res_getResource(&(r->f_انکڑے), "%%ALIAS");
                إذا (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ثابت char16_t *alias = res_getStringNoTrace(&(r->f_انکڑے), aliasres, &aliasLen);
                    إذا(alias != خالی_سوچک && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_منزل_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_اندراج(aliasName, رستہ, حالت);
                    }
                }
            }
        }

        {
            UResource_انکڑے_اندراج *oldR = خالی_سوچک;
            إذا ((oldR = static_cast<UResource_انکڑے_اندراج*>(uhash_get(cache, r))) == خالی_سوچک) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_کوڈ cache_حالت = U_ZERO_ERROR;
                uhash_put(cache, (فراغ *)r, r, &cache_حالت);
                إذا (U_FAILURE(cache_حالت)) {
                    *حالت = cache_حالت;
                    free_اندراج(r);
                    r = خالی_سوچک;
                }
            } وإلا {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_اندراج(r);
                r = oldR;
            }
        }

    }
    إذا(r != خالی_سوچک) {
        /* return the real bundle */
        طالما(r->fAlias != خالی_سوچک) {
            r = r->fAlias;
        }
        r->f_گنتی_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        إذا(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*حالت)) {
             *حالت = r->fBogus; /* set the returning status */
        }
    }
    أرجع r;
}

ساكن UResource_انکڑے_اندراج *
get_Pool_اندراج(ثابت حرف *رستہ, UError_کوڈ *حالت) {
    UResource_انکڑے_اندراج *poolBundle = init_اندراج(kPoolBundleName, رستہ, حالت);
    إذا( U_SUCCESS(*حالت) &&
        (poolBundle == خالی_سوچک || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_انکڑے.isPoolBundle)
    ) {
        *حالت = U_INVALID_FORMAT_ERROR;
    }
    أرجع poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ساكن UResource_انکڑے_اندراج *
لبھو_پہلا_Existing(ثابت حرف* رستہ, حرف* name, ثابت حرف* defaultLocale, UResOpenType openType,
                  UBool *is_جڑ, UBool *لبھیا_ماپا, UBool *isDefault, UError_کوڈ* حالت) {
    UResource_انکڑے_اندراج *r = خالی_سوچک;
    UBool has_Real_انکڑے = خطأ;
    *لبھیا_ماپا = سچ; /* we're starting with a fresh name */
    حرف origName[ULOC_FULLNAME_سمرتھا];

    uprv_strcpy(origName, name);
    طالما(*لبھیا_ماپا && !has_Real_انکڑے) {
        r = init_اندراج(name, رستہ, حالت);
        /* Null pointer test */
        إذا (U_FAILURE(*حالت)) {
            أرجع خالی_سوچک;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_انکڑے = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        إذا(!has_Real_انکڑے) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_گنتی_Existing--;
            /*entryCloseInt(r);*/
            r = خالی_سوچک;
            *حالت = U_USING_FALLBACK_WARNING;
        } وإلا {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_جڑ = static_cast<UBool>(uprv_strcmp(name, k_جڑ_Locale_Name) == 0);

        /*Fallback data stuff*/
        إذا (!has_Real_انکڑے) {
            *لبھیا_ماپا = get_ماپا_Locale_ID(name, origName, openType);
        } وإلا {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *لبھیا_ماپا = chopLocale(name);
        }
        إذا (*لبھیا_ماپا && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    أرجع r;
}

ساكن فراغ ures_سیٹ_Is_ڈھیر_Object( UResourceBundle* resB, UBool state) {
    إذا(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } وإلا {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ساكن UBool ures_is_ڈھیر_Object(ثابت UResourceBundle* resB) {
  أرجع((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?خطأ:سچ);
}


U_CFUNC فراغ ures_init_ڈھیر_Object(UResourceBundle* resB) {
  uprv_memset(resB, 0, حجم(UResourceBundle));
  ures_سیٹ_Is_ڈھیر_Object(resB, سچ);
}

U_NAMESPACE_BEGIN

ڈھیر_UResource_Bundle::ڈھیر_UResource_Bundle() {
    ures_init_ڈھیر_Object(&bundle);
}

ڈھیر_UResource_Bundle::~ڈھیر_UResource_Bundle() {
    ures_close(&bundle);
}

U_NAMESPACE_سرا

ساكن UBool  // returns U_SUCCESS(*status)
لدو_Parents_Except_جڑ(UResource_انکڑے_اندراج *&t1,
                      حرف name[], int32_t name_سمرتھا,
                      UBool usingUSRData, حرف usr_انکڑے_رستہ[], UError_کوڈ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع خطأ; }
    UBool check_ماپا = سچ;
    طالما (check_ماپا && t1->f_ماپا == خالی_سوچک && !t1->f_انکڑے.noFallback &&
            res_getResource(&t1->f_انکڑے,"%%ParentIsRoot") == RES_BOGUS) {
        Resource ماپا_Res = res_getResource(&t1->f_انکڑے, "%%Parent");
        إذا (ماپا_Res != RES_BOGUS) {  // An explicit parent was found.
            int32_t ماپا_Locale_Len = 0;
            // No tracing: called during initial data loading
            ثابت char16_t *ماپا_Locale_Name = res_getStringNoTrace(&(t1->f_انکڑے), ماپا_Res, &ماپا_Locale_Len);
            إذا(ماپا_Locale_Name != خالی_سوچک && 0 < ماپا_Locale_Len && ماپا_Locale_Len < name_سمرتھا) {
                u_UChars_منزل_Chars(ماپا_Locale_Name, name, ماپا_Locale_Len + 1);
                إذا (uprv_strcmp(name, k_جڑ_Locale_Name) == 0) {
                    أرجع سچ;
                }
            }
        }
        // Insert regular parents.
        UError_کوڈ ماپا_حالت = U_ZERO_ERROR;
        UResource_انکڑے_اندراج *t2 = init_اندراج(name, t1->f_رستہ, &ماپا_حالت);
        إذا (U_FAILURE(ماپا_حالت)) {
            *حالت = ماپا_حالت;
            أرجع خطأ;
        }
        UResource_انکڑے_اندراج *u2 = خالی_سوچک;
        UError_کوڈ usr_حالت = U_ZERO_ERROR;
        إذا (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_اندراج(name, usr_انکڑے_رستہ, &usr_حالت);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            إذا (usr_حالت == U_حافظہ_ونڈ_ERROR) {
                *حالت = usr_حالت;
                أرجع خطأ;
            }
        }

        إذا (usingUSRData && U_SUCCESS(usr_حالت) && u2->fBogus == U_ZERO_ERROR) {
            t1->f_ماپا = u2;
            u2->f_ماپا = t2;
        } وإلا {
            t1->f_ماپا = t2;
            إذا (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_گنتی_Existing = 0;
            }
        }
        t1 = t2;
        check_ماپا = chopLocale(name) || may_Have_ماپا(name);
    }
    أرجع سچ;
}

ساكن UBool  // returns U_SUCCESS(*status)
داخل_کرو_جڑ_Bundle(UResource_انکڑے_اندراج *&t1, UError_کوڈ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع خطأ; }
    UError_کوڈ ماپا_حالت = U_ZERO_ERROR;
    UResource_انکڑے_اندراج *t2 = init_اندراج(k_جڑ_Locale_Name, t1->f_رستہ, &ماپا_حالت);
    إذا (U_FAILURE(ماپا_حالت)) {
        *حالت = ماپا_حالت;
        أرجع خطأ;
    }
    t1->f_ماپا = t2;
    t1 = t2;
    أرجع سچ;
}

ساكن UResource_انکڑے_اندراج *اندراج_Open(ثابت حرف* رستہ, ثابت حرف* localeID,
                                     UResOpenType openType, UError_کوڈ* حالت) {
    U_ASSERT(openType != URES_OPEN_سدھا);
    UError_کوڈ int_حالت = U_ZERO_ERROR;
    UResource_انکڑے_اندراج *r = خالی_سوچک;
    UResource_انکڑے_اندراج *t1 = خالی_سوچک;
    UBool isDefault = خطأ;
    UBool is_جڑ = خطأ;
    UBool has_Real_انکڑے = خطأ;
    UBool hasChopped = سچ;
    UBool usingUSRData = U_USE_USRDATA && ( رستہ == خالی_سوچک || uprv_strncmp(رستہ,U_ICUDATA_NAME,8) == 0);

    حرف name[ULOC_FULLNAME_سمرتھا];
    حرف usr_انکڑے_رستہ[96];

    initCache(حالت);

    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    uprv_strncpy(name, localeID, حجم(name) - 1);
    name[حجم(name) - 1] = 0;

    إذا ( usingUSRData ) {
        إذا ( رستہ == خالی_سوچک ) {
            uprv_strcpy(usr_انکڑے_رستہ, U_USRDATA_NAME);
        } وإلا {
            uprv_strncpy(usr_انکڑے_رستہ, رستہ, حجم(usr_انکڑے_رستہ) - 1);
            usr_انکڑے_رستہ[0] = 'u';
            usr_انکڑے_رستہ[1] = 's';
            usr_انکڑے_رستہ[2] = 'r';
            usr_انکڑے_رستہ[حجم(usr_انکڑے_رستہ) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ثابت حرف *defaultLocale = uloc_getDefault();

    Mutex جندرا(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = لبھو_پہلا_Existing(رستہ, name, defaultLocale, openType, &is_جڑ, &hasChopped, &isDefault, &int_حالت);

    // If we failed due to out-of-memory, report the failure and exit early.
    إذا (int_حالت == U_حافظہ_ونڈ_ERROR) {
        *حالت = int_حالت;
        اذهب انت;
    }

    إذا(r != خالی_سوچک) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_انکڑے = سچ;
        إذا ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_کوڈ usr_حالت = U_ZERO_ERROR;
            UResource_انکڑے_اندراج *u1 = init_اندراج(t1->fName, usr_انکڑے_رستہ, &usr_حالت);
            // If we failed due to out-of-memory, report the failure and exit early.
            إذا (int_حالت == U_حافظہ_ونڈ_ERROR) {
                *حالت = int_حالت;
                اذهب انت;
            }
            إذا ( u1 != خالی_سوچک ) {
                إذا(u1->fBogus == U_ZERO_ERROR) {
                    u1->f_ماپا = t1;
                    r = u1;
                } وإلا {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_گنتی_Existing = 0;
                }
            }
        }
        إذا ((hasChopped || may_Have_ماپا(name)) && !is_جڑ) {
            إذا (!لدو_Parents_Except_جڑ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_انکڑے_رستہ, حالت)) {
                اذهب انت;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    إذا(r==خالی_سوچک && openType == URES_OPEN_LOCALE_DEFAULT_جڑ && !isDefault && !is_جڑ) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = لبھو_پہلا_Existing(رستہ, name, defaultLocale, openType, &is_جڑ, &hasChopped, &isDefault, &int_حالت);
        // If we failed due to out-of-memory, report the failure and exit early.
        إذا (int_حالت == U_حافظہ_ونڈ_ERROR) {
            *حالت = int_حالت;
            اذهب انت;
        }
        int_حالت = U_USING_DEFAULT_WARNING;
        إذا(r != خالی_سوچک) { /* the default locale exists */
            t1 = r;
            has_Real_انکڑے = سچ;
            isDefault = سچ;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            إذا ((hasChopped || may_Have_ماپا(name)) && !is_جڑ) {
                إذا (!لدو_Parents_Except_جڑ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_انکڑے_رستہ, حالت)) {
                    اذهب انت;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    إذا(r == خالی_سوچک) {
        uprv_strcpy(name, k_جڑ_Locale_Name);
        r = لبھو_پہلا_Existing(رستہ, name, defaultLocale, openType, &is_جڑ, &hasChopped, &isDefault, &int_حالت);
        // If we failed due to out-of-memory, report the failure and exit early.
        إذا (int_حالت == U_حافظہ_ونڈ_ERROR) {
            *حالت = int_حالت;
            اذهب انت;
        }
        إذا(r != خالی_سوچک) {
            t1 = r;
            int_حالت = U_USING_DEFAULT_WARNING;
            has_Real_انکڑے = سچ;
        } وإلا { /* we don't even have the root locale */
            *حالت = U_MISSING_RESOURCE_ERROR;
            اذهب انت;
        }
    } وإلا إذا(!is_جڑ && uprv_strcmp(t1->fName, k_جڑ_Locale_Name) != 0 &&
            t1->f_ماپا == خالی_سوچک && !r->f_انکڑے.noFallback) {
        إذا (!داخل_کرو_جڑ_Bundle(t1, حالت)) {
            اذهب انت;
        }
        إذا(!has_Real_انکڑے) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    طالما(r != خالی_سوچک && !is_جڑ && t1->f_ماپا != خالی_سوچک) {
        t1->f_ماپا->f_گنتی_Existing++;
        t1 = t1->f_ماپا;
    }

انت:
    إذا(U_SUCCESS(*حالت)) {
        إذا(int_حالت != U_ZERO_ERROR) {
            *حالت = int_حالت;  
        }
        أرجع r;
    } وإلا {
        أرجع خالی_سوچک;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ساكن UResource_انکڑے_اندراج *
اندراج_Open_سدھا(ثابت حرف* رستہ, ثابت حرف* localeID, UError_کوڈ* حالت) {
    initCache(حالت);
    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    } وإلا إذا (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_جڑ_Locale_Name;
    }

    Mutex جندرا(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_انکڑے_اندراج *r = init_اندراج(localeID, رستہ, حالت);
    إذا(U_SUCCESS(*حالت)) {
        إذا(r->fBogus != U_ZERO_ERROR) {
            r->f_گنتی_Existing--;
            r = خالی_سوچک;
        }
    } وإلا {
        r = خالی_سوچک;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_انکڑے_اندراج *t1 = r;
    إذا(r != خالی_سوچک && uprv_strcmp(localeID, k_جڑ_Locale_Name) != 0 &&  // not root
            r->f_ماپا == خالی_سوچک && !r->f_انکڑے.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_سمرتھا) {
        حرف name[ULOC_FULLNAME_سمرتھا];
        uprv_strcpy(name, localeID);
        إذا(!chopLocale(name) || uprv_strcmp(name, k_جڑ_Locale_Name) == 0 ||
                لدو_Parents_Except_جڑ(t1, name, UPRV_LENGTHOF(name), خطأ, خالی_سوچک, حالت)) {
            إذا(uprv_strcmp(t1->fName, k_جڑ_Locale_Name) != 0 && t1->f_ماپا == خالی_سوچک) {
                داخل_کرو_جڑ_Bundle(t1, حالت);
            }
        }
        إذا(U_FAILURE(*حالت)) {
            r = خالی_سوچک;
        }
    }

    إذا(r != خالی_سوچک) {
        // TODO: Does this ever loop?
        طالما(t1->f_ماپا != خالی_سوچک) {
            t1->f_ماپا->f_گنتی_Existing++;
            t1 = t1->f_ماپا;
        }
    }
    أرجع r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ساكن فراغ اندراج_Close_Int(UResource_انکڑے_اندراج *resB) {
    UResource_انکڑے_اندراج *p = resB;

    طالما(resB != خالی_سوچک) {
        p = resB->f_ماپا;
        resB->f_گنتی_Existing--;

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

ساكن فراغ اندراج_Close(UResource_انکڑے_اندراج *resB) {
  Mutex جندرا(&resbMutex);
  اندراج_Close_Int(resB);
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
ساكن فراغ ures_append_Res_رستہ(UResourceBundle *resB, ثابت حرف* منزل_جوڑو, int32_t len_منزل_جوڑو, UError_کوڈ *حالت) {
    int32_t res_رستہ_Len_Orig = resB->f_Res_رستہ_Len;
    إذا(resB->f_Res_رستہ == خالی_سوچک) {
        resB->f_Res_رستہ = resB->fResBuf;
        *(resB->f_Res_رستہ) = 0;
        resB->f_Res_رستہ_Len = 0;
    } 
    resB->f_Res_رستہ_Len += len_منزل_جوڑو;
    إذا(RES_BUFSIZE <= resB->f_Res_رستہ_Len+1) {
        إذا(resB->f_Res_رستہ == resB->fResBuf) {
            resB->f_Res_رستہ = static_cast<حرف*>(uprv_malloc((resB->f_Res_رستہ_Len + 1) * حجم(حرف)));
            /* Check that memory was allocated correctly. */
            إذا (resB->f_Res_رستہ == خالی_سوچک) {
                *حالت = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            uprv_strcpy(resB->f_Res_رستہ, resB->fResBuf);
        } وإلا {
            حرف* عارضی = static_cast<حرف*>(uprv_realloc(resB->f_Res_رستہ, (resB->f_Res_رستہ_Len + 1) * حجم(حرف)));
            /* Check that memory was reallocated correctly. */
            إذا (عارضی == خالی_سوچک) {
                *حالت = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            resB->f_Res_رستہ = عارضی;
        }
    }
    uprv_strcpy(resB->f_Res_رستہ + res_رستہ_Len_Orig, منزل_جوڑو);
}

ساكن فراغ ures_free_Res_رستہ(UResourceBundle *resB) {
    إذا (resB->f_Res_رستہ && resB->f_Res_رستہ != resB->fResBuf) {
        uprv_free(resB->f_Res_رستہ);
    }
    resB->f_Res_رستہ = خالی_سوچک;
    resB->f_Res_رستہ_Len = 0;
}

ساكن فراغ
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    إذا(resB != خالی_سوچک) {
        إذا(resB->f_انکڑے != خالی_سوچک) {
            اندراج_Close(resB->f_انکڑے);
        }
        إذا(resB->f_نسخہ != خالی_سوچک) {
            uprv_free(resB->f_نسخہ);
        }
        ures_free_Res_رستہ(resB);

        إذا(ures_is_ڈھیر_Object(resB) == خطأ && freeBundleObj) {
            uprv_free(resB);
        }
#إذا 0 /*U_DEBUG*/
        وإلا {
            /* poison the data */
            uprv_memset(resB, -1, حجم(UResourceBundle));
        }
#جے_انت
    }
}

U_CAPI فراغ  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, سچ);
}

نطاق {

UResourceBundle* init_resb_نتیجہ(UResource_انکڑے_اندراج* انکڑے_اندراج,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  UResource_انکڑے_اندراج* valid_Locale_انکڑے_اندراج,
                                  ثابت حرف* container_Res_رستہ,
                                  int32_t recursion_ڈونگھائی,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کوڈ* حالت);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
get_Alias_نشانہ_As_Resource_Bundle(ثابت Resource_انکڑے& res_انکڑے,
                               Resource r,
                               ثابت حرف* کلید,
                               int32_t idx,
                               UResource_انکڑے_اندراج* valid_Locale_انکڑے_اندراج,
                               ثابت حرف* container_Res_رستہ,
                               int32_t recursion_ڈونگھائی,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_کوڈ* حالت) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    إذا (U_FAILURE(*حالت)) { أرجع resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ثابت char16_t *alias = res_getAlias(&res_انکڑے, r, &len);
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
        أرجع خالی_سوچک;
    }

    // We have an alias, now let's cut it up.
    ثابت حرف *رستہ = خالی_سوچک, *locale = خالی_سوچک, *کلید_رستہ = خالی_سوچک;
    إذا(chAlias[0] == RES_رستہ_SEPARATOR) {
        // There is a path included.
        حرف *ch_Alias_انکڑے = chAlias.انکڑے();
        حرف *sep = ch_Alias_انکڑے + 1;
        رستہ = sep;
        sep = uprv_strchr(sep, RES_رستہ_SEPARATOR);
        إذا(sep != خالی_سوچک) {
            *sep++ = 0;
        }
        إذا(uprv_strcmp(رستہ, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            کلید_رستہ = sep;
            // Read from the valid locale which we already have.
            رستہ = locale = خالی_سوچک;
        } وإلا {
            إذا(uprv_strcmp(رستہ, "ICUDATA") == 0) { /* want ICU data */
                رستہ = خالی_سوچک;
            }
            إذا (sep == خالی_سوچک) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } وإلا {
                locale = sep;
                sep = uprv_strchr(sep, RES_رستہ_SEPARATOR);
                إذا(sep != خالی_سوچک) {
                    *sep++ = 0;
                }
                کلید_رستہ = sep;
            }
        }
    } وإلا {
        // No path, start with a locale.
        حرف *sep = chAlias.انکڑے();
        locale = sep;
        sep = uprv_strchr(sep, RES_رستہ_SEPARATOR);
        إذا(sep != خالی_سوچک) {
            *sep++ = 0;
        }
        کلید_رستہ = sep;
        رستہ = valid_Locale_انکڑے_اندراج->f_رستہ;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    Local_UResource_Bundle_اشارہ_گر mainRes;
    UResource_انکڑے_اندراج *انکڑے_اندراج;
    إذا (locale == خالی_سوچک) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        انکڑے_اندراج = valid_Locale_انکڑے_اندراج;
    } وإلا {
        UError_کوڈ int_حالت = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_سدھا(رستہ, locale, &int_حالت));
        إذا(U_FAILURE(int_حالت)) {
            // We failed to open the resource bundle we're aliasing to.
            *حالت = int_حالت;
            أرجع resB;
        }
        انکڑے_اندراج = mainRes->f_انکڑے;
    }

    ثابت حرف* عارضی = خالی_سوچک;
    إذا(کلید_رستہ == خالی_سوچک) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = انکڑے_اندراج->f_انکڑے.جڑ_Res;
        إذا(container_Res_رستہ) {
            chAlias.clear().append(container_Res_رستہ, *حالت);
            إذا (U_FAILURE(*حالت)) {
                أرجع خالی_سوچک;
            }
            حرف *a_کلید = chAlias.انکڑے();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_لبھو_Resource(&انکڑے_اندراج->f_انکڑے, r, &a_کلید, &عارضی);
        }
        إذا(کلید) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(کلید, *حالت);
            إذا (U_FAILURE(*حالت)) {
                أرجع خالی_سوچک;
            }
            حرف *a_کلید = chAlias.انکڑے();
            r = res_لبھو_Resource(&انکڑے_اندراج->f_انکڑے, r, &a_کلید, &عارضی);
        } وإلا إذا(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            إذا(URES_IS_جدول(type)) {
                ثابت حرف *a_کلید;
                r = res_get_جدول_شے_By_اشاریہ(&انکڑے_اندراج->f_انکڑے, r, idx, &a_کلید);
            } وإلا { /* array */
                r = res_get_Array_شے(&انکڑے_اندراج->f_انکڑے, r, idx);
            }
        }
        إذا(r != RES_BOGUS) {
            resB = init_resb_نتیجہ(
                انکڑے_اندراج, r, عارضی, -1, valid_Locale_انکڑے_اندراج, خالی_سوچک, recursion_ڈونگھائی+1,
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
        CharString رستہ_Buf(کلید_رستہ, *حالت);
        إذا (U_FAILURE(*حالت)) {
            أرجع خالی_سوچک;
        }
        حرف *my_رستہ = رستہ_Buf.انکڑے();
        container_Res_رستہ = خالی_سوچک;
        // Now we have fallback following here.
        لكل(;;) {
            r = انکڑے_اندراج->f_انکڑے.جڑ_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            طالما(*my_رستہ && U_SUCCESS(*حالت)) {
                r = res_لبھو_Resource(&(انکڑے_اندراج->f_انکڑے), r, &my_رستہ, &عارضی);
                إذا(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    اكسر;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_نتیجہ(
                    انکڑے_اندراج, r, عارضی, -1,
                    valid_Locale_انکڑے_اندراج, container_Res_رستہ, recursion_ڈونگھائی+1,
                    resB, حالت);
                إذا (U_FAILURE(*حالت)) {
                    اكسر;
                }
                إذا (عارضی == خالی_سوچک || uprv_strcmp(کلید_رستہ, عارضی) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_رستہ(resB);
                    ures_append_Res_رستہ(resB, کلید_رستہ, static_cast<int32_t>(uprv_strlen(کلید_رستہ)), حالت);
                    إذا(resB->f_Res_رستہ[resB->f_Res_رستہ_Len-1] != RES_رستہ_SEPARATOR) {
                        ures_append_Res_رستہ(resB, RES_رستہ_SEPARATOR_S, 1, حالت);
                    }
                    إذا (U_FAILURE(*حالت)) {
                        اكسر;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                انکڑے_اندراج = resB->f_انکڑے;
                container_Res_رستہ = resB->f_Res_رستہ;
            }
            إذا (U_FAILURE(*حالت) || r != RES_BOGUS) {
                اكسر;
            }
            // Fall back to the parent bundle, if there is one.
            انکڑے_اندراج = انکڑے_اندراج->f_ماپا;
            إذا (انکڑے_اندراج == خالی_سوچک) {
                *حالت = U_MISSING_RESOURCE_ERROR;
                اكسر;
            }
            // Copy the same keyPath again.
            my_رستہ = رستہ_Buf.انکڑے();
            uprv_strcpy(my_رستہ, کلید_رستہ);
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
UResourceBundle* init_resb_نتیجہ(UResource_انکڑے_اندراج* انکڑے_اندراج,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  UResource_انکڑے_اندراج* valid_Locale_انکڑے_اندراج,
                                  ثابت حرف* container_Res_رستہ,
                                  int32_t recursion_ڈونگھائی,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کوڈ* حالت) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع resB;
    }
    إذا (valid_Locale_انکڑے_اندراج == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    إذا(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        إذا(recursion_ڈونگھائی >= URES_MAX_ALIAS_LEVEL) {
            *حالت = U_TOO_MANY_ALIASES_ERROR;
            أرجع resB;
        }
        أرجع get_Alias_نشانہ_As_Resource_Bundle(
            انکڑے_اندراج->f_انکڑے, r, کلید, idx,
            valid_Locale_انکڑے_اندراج, container_Res_رستہ, recursion_ڈونگھائی, resB, حالت);
    }
    إذا(resB == خالی_سوچک) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        إذا (resB == خالی_سوچک) {
            *حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }
        ures_سیٹ_Is_ڈھیر_Object(resB, خطأ);
        resB->f_Res_رستہ = خالی_سوچک;
        resB->f_Res_رستہ_Len = 0;
    } وإلا {
        إذا(resB->f_انکڑے != خالی_سوچک) {
            اندراج_Close(resB->f_انکڑے);
        }
        إذا(resB->f_نسخہ != خالی_سوچک) {
            uprv_free(resB->f_نسخہ);
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
        إذا(container_Res_رستہ != resB->f_Res_رستہ) {
            ures_free_Res_رستہ(resB);
        }
    }
    resB->f_انکڑے = انکڑے_اندراج;
    اندراج_Increase(resB->f_انکڑے);
    resB->fHasFallback = خطأ;
    resB->f_Is_چوٹی_Level = خطأ;
    resB->f_اشاریہ = -1;
    resB->f_کلید = کلید; 
    resB->f_Valid_Locale_انکڑے_اندراج = valid_Locale_انکڑے_اندراج;
    إذا(container_Res_رستہ != resB->f_Res_رستہ) {
        ures_append_Res_رستہ(
            resB, container_Res_رستہ, static_cast<int32_t>(uprv_strlen(container_Res_رستہ)), حالت);
    }
    إذا(کلید != خالی_سوچک) {
        ures_append_Res_رستہ(resB, کلید, static_cast<int32_t>(uprv_strlen(کلید)), حالت);
        إذا(resB->f_Res_رستہ_Len > 0 && resB->f_Res_رستہ[resB->f_Res_رستہ_Len-1] != RES_رستہ_SEPARATOR) {
            ures_append_Res_رستہ(resB, RES_رستہ_SEPARATOR_S, 1, حالت);
        }
    } وإلا إذا(idx >= 0) {
        حرف buf[256];
        int32_t len = T_CString_integer_منزل_String(buf, idx, 10);
        ures_append_Res_رستہ(resB, buf, len, حالت);
        إذا(resB->f_Res_رستہ_Len > 0 && resB->f_Res_رستہ[resB->f_Res_رستہ_Len-1] != RES_رستہ_SEPARATOR) {
            ures_append_Res_رستہ(resB, RES_رستہ_SEPARATOR_S, 1, حالت);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t ورتیا_Len = ((resB->fResBuf == resB->f_Res_رستہ) ? resB->f_Res_رستہ_Len : 0);
        uprv_memset(resB->fResBuf + ورتیا_Len, 0, حجم(resB->fResBuf) - ورتیا_Len);
    }

    resB->f_نسخہ = خالی_سوچک;
    resB->fRes = r;
    resB->f_ناپ = res_گنتی_Array_اشیاء(&resB->get_Res_انکڑے(), resB->fRes);
    ResourceTracer(resB).trace("get");
    أرجع resB;
}

UResourceBundle* init_resb_نتیجہ(UResource_انکڑے_اندراج* انکڑے_اندراج,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ثابت UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_کوڈ* حالت) {
    أرجع init_resb_نتیجہ(
        انکڑے_اندراج, r, کلید, idx,
        container->f_Valid_Locale_انکڑے_اندراج, container->f_Res_رستہ, 0, resB, حالت);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ثابت UResourceBundle* original,
                               UError_کوڈ* حالت) {
    UBool is_ڈھیر_Object;
    إذا(U_FAILURE(*حالت) || r == original) {
        أرجع r;
    }
    إذا(original != خالی_سوچک) {
        إذا(r == خالی_سوچک) {
            is_ڈھیر_Object = خطأ;
            r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
            /* test for nullptr */
            إذا (r == خالی_سوچک) {
                *حالت = U_حافظہ_ونڈ_ERROR;
                أرجع خالی_سوچک;
            }
        } وإلا {
            is_ڈھیر_Object = ures_is_ڈھیر_Object(r);
            ures_closeBundle(r, خطأ);
        }
        uprv_memcpy(r, original, حجم(UResourceBundle));
        r->f_Res_رستہ = خالی_سوچک;
        r->f_Res_رستہ_Len = 0;
        إذا(original->f_Res_رستہ) {
            ures_append_Res_رستہ(r, original->f_Res_رستہ, original->f_Res_رستہ_Len, حالت);
        }
        ures_سیٹ_Is_ڈھیر_Object(r, is_ڈھیر_Object);
        إذا(r->f_انکڑے != خالی_سوچک) {
            اندراج_Increase(r->f_انکڑے);
        }
    }
    أرجع r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ثابت char16_t* U_EXPORT2 ures_getString(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_کوڈ* حالت) {
    ثابت char16_t *s;
    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا(resB == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    s = res_getString({resB}, &resB->get_Res_انکڑے(), resB->fRes, len);
    إذا (s == خالی_سوچک) {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ساكن ثابت حرف*
ures_منزل_UTF8_String(ثابت char16_t* s16,
                  int32_t length16,
                  حرف* dest U_LIFETIME_BOUND,
                  int32_t* p_لمائی,
                  UBool forceCopy,
                  UError_کوڈ* حالت) {
    int32_t capacity;

    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (p_لمائی != خالی_سوچک) {
        capacity = *p_لمائی;
    } وإلا {
        capacity = 0;
    }
    إذا (capacity < 0 || (capacity > 0 && dest == خالی_سوچک)) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    إذا (length16 == 0) {
        /* empty string, return as read-only pointer */
        إذا (p_لمائی != خالی_سوچک) {
            *p_لمائی = 0;
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
            أرجع u_str_منزل_UTF8(خالی_سوچک, 0, p_لمائی, s16, length16, حالت);
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
            int32_t max_لمائی_2 = 3 * length16 + 1;
            إذا (capacity > max_لمائی_2) {
                dest += capacity - max_لمائی_2;
                capacity = max_لمائی_2;
            }
        }
        أرجع u_str_منزل_UTF8(dest, capacity, p_لمائی, s16, length16, حالت);
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getUTF8String(ثابت UResourceBundle* resB,
                   حرف* dest U_LIFETIME_BOUND,
                   int32_t* p_لمائی,
                   UBool forceCopy,
                   UError_کوڈ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getString(resB, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_لمائی, forceCopy, حالت);
}

U_CAPI ثابت uint8_t* U_EXPORT2
ures_get_ثنائی(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_کوڈ* حالت) {
  ثابت uint8_t *p;
  إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  إذا(resB == خالی_سوچک) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع خالی_سوچک;
  }
  p = res_get_ثنائی({resB}, &resB->get_Res_انکڑے(), resB->fRes, len);
  إذا (p == خالی_سوچک) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

U_CAPI ثابت int32_t* U_EXPORT2
ures_get_Int_سمتیہ(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_کوڈ* حالت) {
  ثابت int32_t *p;
  إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  إذا(resB == خالی_سوچک) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع خالی_سوچک;
  }
  p = res_get_Int_سمتیہ({resB}, &resB->get_Res_انکڑے(), resB->fRes, len);
  إذا (p == خالی_سوچک) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
  }
  أرجع p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ثابت UResourceBundle* resB, UError_کوڈ *حالت) {
  إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
    أرجع 0xffffffff;
  }
  إذا(resB == خالی_سوچک) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع 0xffffffff;
  }
  إذا(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *حالت = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0xffffffff;
  }
  أرجع res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ثابت UResourceBundle* resB, UError_کوڈ *حالت) {
  إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
    أرجع 0xffffffff;
  }
  إذا(resB == خالی_سوچک) {
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
  إذا(resB == خالی_سوچک) {
    أرجع URES_NONE;
  }
  أرجع res_get_عوامی_Type(resB->fRes);
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
  إذا(resB == خالی_سوچک) {
    أرجع خالی_سوچک;
  }
  أرجع(resB->f_کلید);
}

U_CAPI int32_t U_EXPORT2 ures_get_ناپ(ثابت UResourceBundle *resB) {
  إذا(resB == خالی_سوچک) {
    أرجع 0;
  }
  
  أرجع resB->f_ناپ;
}

ساكن ثابت char16_t*
ures_getStringWithAlias(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_اشاریہ,
                        int32_t* len,
                        UError_کوڈ* حالت) {
  إذا(RES_GET_TYPE(r) == URES_ALIAS) {
    ثابت char16_t* نتیجہ = خالی_سوچک;
    UResourceBundle *عارضی_Res = ures_get_By_اشاریہ(resB, s_اشاریہ, خالی_سوچک, حالت);
    نتیجہ = ures_getString(عارضی_Res, len, حالت);
    ures_close(عارضی_Res);
    أرجع نتیجہ;
  } وإلا {
    أرجع res_getString({resB, s_اشاریہ}, &resB->get_Res_انکڑے(), r, len); 
  }
}

U_CAPI فراغ U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  إذا(resB == خالی_سوچک) {
    أرجع;
  }
  resB->f_اشاریہ = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_اگلا(ثابت UResourceBundle *resB) {
  إذا(resB == خالی_سوچک) {
    أرجع خطأ;
  }
  أرجع resB->f_اشاریہ < resB->f_ناپ-1;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_اگلا_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ثابت حرف** کلید,
                   UError_کوڈ* حالت) {
  Resource r = RES_BOGUS;
  
  إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  إذا(resB == خالی_سوچک) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع خالی_سوچک;
  }
  
  إذا(resB->f_اشاریہ == resB->f_ناپ-1) {
    *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
  } وإلا {
    resB->f_اشاریہ++;
    بدل(RES_GET_TYPE(resB->fRes)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
      أرجع res_getString({resB}, &resB->get_Res_انکڑے(), resB->fRes, len);
    حالة URES_جدول:
    حالة URES_TABLE16:
    حالة URES_TABLE32:
      r = res_get_جدول_شے_By_اشاریہ(&resB->get_Res_انکڑے(), resB->fRes, resB->f_اشاریہ, کلید);
      إذا(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->f_اشاریہ, len, حالت);
    حالة URES_ARRAY:
    حالة URES_ARRAY16:
      r = res_get_Array_شے(&resB->get_Res_انکڑے(), resB->fRes, resB->f_اشاریہ);
      إذا(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      أرجع ures_getStringWithAlias(resB, r, resB->f_اشاریہ, len, حالت);
    حالة URES_ALIAS:
      أرجع ures_getStringWithAlias(resB, resB->fRes, resB->f_اشاریہ, len, حالت);
    حالة URES_INT:
    حالة URES_ثنائی:
    حالة URES_INT_سمتیہ:
        *حالت = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    افتراضي:
      أرجع خالی_سوچک;
    }
  }

  أرجع خالی_سوچک;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_اگلا_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_وچ U_LIFETIME_BOUND,
                     UError_کوڈ* حالت) {
    ثابت حرف *کلید = خالی_سوچک;
    Resource r = RES_BOGUS;

    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
            /*return nullptr;*/
            أرجع fill_وچ;
    }
    إذا(resB == خالی_سوچک) {
            *حالت = U_ILLEGAL_دلیل_ERROR;
            /*return nullptr;*/
            أرجع fill_وچ;
    }

    إذا(resB->f_اشاریہ == resB->f_ناپ-1) {
      *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } وإلا {
        resB->f_اشاریہ++;
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_ثنائی:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_سمتیہ:
            أرجع ures_copyResb(fill_وچ, resB, حالت);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_شے_By_اشاریہ(&resB->get_Res_انکڑے(), resB->fRes, resB->f_اشاریہ, &کلید);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتیجہ(resB->f_انکڑے, r, کلید, resB->f_اشاریہ, resB, fill_وچ, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_شے(&resB->get_Res_انکڑے(), resB->fRes, resB->f_اشاریہ);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتیجہ(resB->f_انکڑے, r, کلید, resB->f_اشاریہ, resB, fill_وچ, حالت);
        افتراضي:
            /*return nullptr;*/
            أرجع fill_وچ;
        }
    }
    /*return nullptr;*/
    أرجع fill_وچ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_اشاریہ(ثابت UResourceBundle* resB,
                int32_t اشاریہ_R,
                UResourceBundle* fill_وچ U_LIFETIME_BOUND,
                UError_کوڈ* حالت) {
    ثابت حرف* کلید = خالی_سوچک;
    Resource r = RES_BOGUS;

    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        /*return nullptr;*/
        أرجع fill_وچ;
    }
    إذا(resB == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        /*return nullptr;*/
        أرجع fill_وچ;
    }

    إذا(اشاریہ_R >= 0 && resB->f_ناپ > اشاریہ_R) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_INT:
        حالة URES_ثنائی:
        حالة URES_STRING:
        حالة URES_STRING_V2:
        حالة URES_INT_سمتیہ:
            أرجع ures_copyResb(fill_وچ, resB, حالت);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_شے_By_اشاریہ(&resB->get_Res_انکڑے(), resB->fRes, اشاریہ_R, &کلید);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتیجہ(resB->f_انکڑے, r, کلید, اشاریہ_R, resB, fill_وچ, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_شے(&resB->get_Res_انکڑے(), resB->fRes, اشاریہ_R);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع init_resb_نتیجہ(resB->f_انکڑے, r, کلید, اشاریہ_R, resB, fill_وچ, حالت);
        افتراضي:
            /*return nullptr;*/
            أرجع fill_وچ;
        }
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    أرجع fill_وچ;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_اشاریہ(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t اشاریہ_S,
                      int32_t* len,
                      UError_کوڈ* حالت) {
    ثابت حرف* کلید = خالی_سوچک;
    Resource r = RES_BOGUS;

    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا(resB == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    إذا(اشاریہ_S >= 0 && resB->f_ناپ > اشاریہ_S) {
        بدل(RES_GET_TYPE(resB->fRes)) {
        حالة URES_STRING:
        حالة URES_STRING_V2:
            أرجع res_getString({resB}, &resB->get_Res_انکڑے(), resB->fRes, len);
        حالة URES_جدول:
        حالة URES_TABLE16:
        حالة URES_TABLE32:
            r = res_get_جدول_شے_By_اشاریہ(&resB->get_Res_انکڑے(), resB->fRes, اشاریہ_S, &کلید);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, اشاریہ_S, len, حالت);
        حالة URES_ARRAY:
        حالة URES_ARRAY16:
            r = res_get_Array_شے(&resB->get_Res_انکڑے(), resB->fRes, اشاریہ_S);
            إذا(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            أرجع ures_getStringWithAlias(resB, r, اشاریہ_S, len, حالت);
        حالة URES_ALIAS:
            أرجع ures_getStringWithAlias(resB, resB->fRes, اشاریہ_S, len, حالت);
        حالة URES_INT:
        حالة URES_ثنائی:
        حالة URES_INT_سمتیہ:
            *حالت = U_RESOURCE_TYPE_MISMATCH;
            اكسر;
        افتراضي:
          /* must not occur */
          *حالت = U_اندرونی_پروگرام_ERROR;
          اكسر;
        }
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    أرجع خالی_سوچک;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_اشاریہ(ثابت UResourceBundle* resB,
                          int32_t idx,
                          حرف* dest U_LIFETIME_BOUND,
                          int32_t* p_لمائی,
                          UBool forceCopy,
                          UError_کوڈ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_اشاریہ(resB, idx, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_لمائی, forceCopy, حالت);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_لبھو_Resource(ثابت حرف* رستہ, UResourceBundle* fill_وچ U_LIFETIME_BOUND, UError_کوڈ* حالت)
{
  UResourceBundle *first = خالی_سوچک; 
  UResourceBundle *نتیجہ = fill_وچ;
  حرف *packageName = خالی_سوچک;
  حرف *رستہ_منزل_Resource = خالی_سوچک, *save = خالی_سوچک;
  حرف *locale = خالی_سوچک, *locale_سرا = خالی_سوچک;
  int32_t length;

  إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)) {
    أرجع نتیجہ;
  }

  length = (int32_t)(uprv_strlen(رستہ)+1);
  save = رستہ_منزل_Resource = (حرف *)uprv_malloc(length*حجم(حرف));
  /* test for nullptr */
  إذا(رستہ_منزل_Resource == خالی_سوچک) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع نتیجہ;
  }
  uprv_memcpy(رستہ_منزل_Resource, رستہ, length);

  locale = رستہ_منزل_Resource;
  إذا(*رستہ_منزل_Resource == RES_رستہ_SEPARATOR) { /* there is a path specification */
    رستہ_منزل_Resource++;
    packageName = رستہ_منزل_Resource;
    رستہ_منزل_Resource = uprv_strchr(رستہ_منزل_Resource, RES_رستہ_SEPARATOR);
    إذا(رستہ_منزل_Resource == خالی_سوچک) {
      *حالت = U_ILLEGAL_دلیل_ERROR;
    } وإلا {
      *رستہ_منزل_Resource = 0;
      locale = رستہ_منزل_Resource+1;
    }
  }

  locale_سرا = uprv_strchr(locale, RES_رستہ_SEPARATOR);
  إذا(locale_سرا != خالی_سوچک) {
    *locale_سرا = 0;
  }

  first = ures_open(packageName, locale, حالت);

  إذا(U_SUCCESS(*حالت)) {
    إذا(locale_سرا) {
      نتیجہ = ures_لبھو_Sub_Resource(first, locale_سرا+1, fill_وچ, حالت);
    } وإلا {
      نتیجہ = ures_copyResb(fill_وچ, first, حالت);
    }
    ures_close(first);
  }
  uprv_free(save);
  أرجع نتیجہ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_لبھو_Sub_Resource(ثابت UResourceBundle* resB,
                     حرف* رستہ,
                     UResourceBundle* fill_وچ U_LIFETIME_BOUND,
                     UError_کوڈ* حالت)
{
  Resource res = RES_BOGUS;
  UResourceBundle *نتیجہ = fill_وچ;
  ثابت حرف *کلید;

  إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)) {
    أرجع نتیجہ;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  افعل {
    res = res_لبھو_Resource(&resB->get_Res_انکڑے(), resB->fRes, &رستہ, &کلید); 
    إذا(res != RES_BOGUS) {
        نتیجہ = init_resb_نتیجہ(resB->f_انکڑے, res, کلید, -1, resB, fill_وچ, حالت);
        resB = نتیجہ;
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
        اكسر;
    }
  } طالما(*رستہ); /* there is more stuff in the path */

  أرجع نتیجہ;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلید_With_Fallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت حرف* وچ_کلید,
                                int32_t* len,
                                UError_کوڈ* حالت) {
    UResourceBundle ڈھیر_5;
    ثابت char16_t* retVal = خالی_سوچک;
    ures_init_ڈھیر_Object(&ڈھیر_5);
    ures_get_By_کلید_With_Fallback(resB, وچ_کلید, &ڈھیر_5, حالت);
    int32_t length;
    retVal = ures_getString(&ڈھیر_5, &length, حالت);
    ures_close(&ڈھیر_5);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (length == 3 && retVal[0] == خالی_تھاواں_سیٹ && retVal[1] == خالی_تھاواں_سیٹ && retVal[2] == خالی_تھاواں_سیٹ ) {
        retVal = خالی_سوچک;
        length = 0;
        *حالت = U_MISSING_RESOURCE_ERROR;
    }
    إذا (len != خالی_سوچک) {
        *len = length;
    }
    أرجع retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ساكن Resource get_جدول_شے_By_کلید_رستہ(ثابت Resource_انکڑے *p_Res_انکڑے, Resource جدول, ثابت حرف *کلید) {
  Resource resource = جدول;  /* The current resource */
  icu::CharString رستہ;
  UError_کوڈ error_کوڈ = U_ZERO_ERROR;
  رستہ.append(کلید, error_کوڈ);
  إذا (U_FAILURE(error_کوڈ)) { أرجع RES_BOGUS; }
  حرف *رستہ_Part = رستہ.انکڑے();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  طالما (*رستہ_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    حرف *اگلا_رستہ_Part = uprv_strchr(رستہ_Part, RES_رستہ_SEPARATOR);
    إذا (اگلا_رستہ_Part != خالی_سوچک) {
      *اگلا_رستہ_Part = 0;  /* Terminating null for this part of path. */
      اگلا_رستہ_Part++;
    } وإلا {
      اگلا_رستہ_Part = uprv_strchr(رستہ_Part, 0);
    }
    int32_t t;
    ثابت حرف *رستہ_P = رستہ_Part;
    resource = res_get_جدول_شے_By_کلید(p_Res_انکڑے, resource, &t, &رستہ_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    رستہ_Part = اگلا_رستہ_Part; 
  }
  إذا (*رستہ_Part) {
    أرجع RES_BOGUS;
  }
  أرجع resource;
}

ساكن فراغ create_رستہ(ثابت حرف* orig_Res_رستہ,
                       int32_t     orig_Res_رستہ_Len,
                       ثابت حرف* res_رستہ,
                       int32_t     res_رستہ_Len,
                       ثابت حرف* وچ_کلید,
                       CharString& رستہ,
                       UError_کوڈ* حالت) {
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
    رستہ.clear();
    ثابت حرف* کلید = وچ_کلید;
    إذا (res_رستہ_Len > 0) {
        رستہ.append(res_رستہ, res_رستہ_Len, *حالت);
        إذا (U_SUCCESS(*حالت)) {
            ثابت حرف* res_رستہ_حد_بندی = res_رستہ + res_رستہ_Len;
            ثابت حرف* orig_Res_رستہ_حد_بندی = orig_Res_رستہ + orig_Res_رستہ_Len;
            ثابت حرف* res_رستہ_Ptr = res_رستہ;
            ثابت حرف* orig_Res_رستہ_Ptr = orig_Res_رستہ;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            طالما (orig_Res_رستہ_Ptr < orig_Res_رستہ_حد_بندی && res_رستہ_Ptr < res_رستہ_حد_بندی) {
                طالما (orig_Res_رستہ_Ptr < orig_Res_رستہ_حد_بندی && *orig_Res_رستہ_Ptr != RES_رستہ_SEPARATOR) {
                    ++orig_Res_رستہ_Ptr;
                }
                إذا (orig_Res_رستہ_Ptr < orig_Res_رستہ_حد_بندی && *orig_Res_رستہ_Ptr == RES_رستہ_SEPARATOR) {
                    ++orig_Res_رستہ_Ptr;
                }
                طالما (res_رستہ_Ptr < res_رستہ_حد_بندی && *res_رستہ_Ptr != RES_رستہ_SEPARATOR) {
                    ++res_رستہ_Ptr;
                }
                إذا (res_رستہ_Ptr < res_رستہ_حد_بندی && *res_رستہ_Ptr == RES_رستہ_SEPARATOR) {
                    ++res_رستہ_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            طالما (res_رستہ_Ptr < res_رستہ_حد_بندی && *کلید != '\0') {
                طالما (res_رستہ_Ptr < res_رستہ_حد_بندی && *res_رستہ_Ptr != RES_رستہ_SEPARATOR) {
                    ++res_رستہ_Ptr;
                }
                إذا (res_رستہ_Ptr < res_رستہ_حد_بندی && *res_رستہ_Ptr == RES_رستہ_SEPARATOR) {
                    ++res_رستہ_Ptr;
                }
                طالما (*کلید != '\0' && *کلید != RES_رستہ_SEPARATOR) {
                    ++کلید;
                }
                إذا (*کلید == RES_رستہ_SEPARATOR) {
                    ++کلید;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        رستہ.append(کلید, *حالت);
    } وإلا {
        رستہ.append(وچ_کلید, *حالت);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلید_With_Fallback(ثابت UResourceBundle* resB,
                          ثابت حرف* وچ_کلید,
                          UResourceBundle* fill_وچ U_LIFETIME_BOUND,
                          UError_کوڈ* حالت) {
    Resource res = RES_BOGUS, جڑ_Res = RES_BOGUS;
    UResourceBundle *helper = خالی_سوچک;

    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع fill_وچ;
    }
    إذا(resB == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع fill_وچ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        ثابت حرف* orig_Res_رستہ = resB->f_Res_رستہ;
        int32_t orig_Res_رستہ_Len = resB->f_Res_رستہ_Len;
        res = get_جدول_شے_By_کلید_رستہ(&resB->get_Res_انکڑے(), resB->fRes, وچ_کلید);
        ثابت حرف* کلید = وچ_کلید;
        منطقي did_جڑ_Once = خطأ;
        إذا(res == RES_BOGUS) {
            UResource_انکڑے_اندراج *انکڑے_اندراج = resB->f_انکڑے;
            CharString رستہ;
            حرف *my_رستہ = خالی_سوچک;
            ثابت حرف* res_رستہ = resB->f_Res_رستہ;
            int32_t len = resB->f_Res_رستہ_Len;
            طالما(res == RES_BOGUS && (انکڑے_اندراج->f_ماپا != خالی_سوچک || !did_جڑ_Once)) { /* Otherwise, we'll look in parents */
                إذا (انکڑے_اندراج->f_ماپا != خالی_سوچک) {
                    انکڑے_اندراج = انکڑے_اندراج->f_ماپا;
                } وإلا {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_جڑ_Once = سچ;
                }
                جڑ_Res = انکڑے_اندراج->f_انکڑے.جڑ_Res;

                إذا(انکڑے_اندراج->fBogus == U_ZERO_ERROR) {
                    create_رستہ(orig_Res_رستہ, orig_Res_رستہ_Len, res_رستہ, len, وچ_کلید, رستہ, حالت);
                    إذا (U_FAILURE(*حالت)) {
                        ures_close(helper);
                        أرجع fill_وچ;
                    }
                    my_رستہ = رستہ.انکڑے();
                    کلید = وچ_کلید;
                    افعل {
                        res = res_لبھو_Resource(&(انکڑے_اندراج->f_انکڑے), جڑ_Res, &my_رستہ, &کلید);
                        إذا (RES_GET_TYPE(res) == URES_ALIAS && *my_رستہ) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_نتیجہ(انکڑے_اندراج, res, خالی_سوچک, -1, resB, helper, حالت);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            إذا(helper) {
                              انکڑے_اندراج = helper->f_انکڑے;
                              جڑ_Res = helper->fRes;
                              res_رستہ = helper->f_Res_رستہ;
                              len = helper->f_Res_رستہ_Len;

                            } وإلا {
                              اكسر;
                            }
                        } وإلا إذا (res == RES_BOGUS) {
                            اكسر;
                        }
                    } طالما(*my_رستہ); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            إذا(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                إذا(uprv_strcmp(انکڑے_اندراج->fName, uloc_getDefault())==0 || uprv_strcmp(انکڑے_اندراج->fName, k_جڑ_Locale_Name)==0) {
                    *حالت = U_USING_DEFAULT_WARNING;
                } وإلا {
                    *حالت = U_USING_FALLBACK_WARNING;
                }

                fill_وچ = init_resb_نتیجہ(انکڑے_اندراج, res, کلید, -1, resB, fill_وچ, حالت);
                إذا (res_رستہ != خالی_سوچک) {
                    create_رستہ(orig_Res_رستہ, orig_Res_رستہ_Len, res_رستہ, len, وچ_کلید, رستہ, حالت);
                } وإلا {
                    ثابت حرف* separator = خالی_سوچک;
                    إذا (fill_وچ->f_Res_رستہ != خالی_سوچک) {
                        separator = uprv_strchr(fill_وچ->f_Res_رستہ, RES_رستہ_SEPARATOR);
                    }
                    إذا (separator != خالی_سوچک && separator[1] != '\0') {
                        create_رستہ(orig_Res_رستہ, orig_Res_رستہ_Len, fill_وچ->f_Res_رستہ,
                                   static_cast<int32_t>(uprv_strlen(fill_وچ->f_Res_رستہ)), وچ_کلید, رستہ, حالت);
                    } وإلا {
                        create_رستہ(orig_Res_رستہ, orig_Res_رستہ_Len, "", 0, وچ_کلید, رستہ, حالت);
                    }
                }
                ures_free_Res_رستہ(fill_وچ);
                ures_append_Res_رستہ(fill_وچ, رستہ.انکڑے(), رستہ.length(), حالت);
                إذا(fill_وچ->f_Res_رستہ[fill_وچ->f_Res_رستہ_Len-1] != RES_رستہ_SEPARATOR) {
                    ures_append_Res_رستہ(fill_وچ, RES_رستہ_SEPARATOR_S, 1, حالت);
                }
            } وإلا {
                *حالت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            fill_وچ = init_resb_نتیجہ(resB->f_انکڑے, res, کلید, -1, resB, fill_وچ, حالت);
        }
    } 
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    أرجع fill_وچ;
}

نطاق {

فراغ get_All_اشیاء_With_Fallback(
        ثابت UResourceBundle *bundle, Resource_انکڑے_قدر &value,
        ResourceSink &sink, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
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
    value.سیٹ_انکڑے(bundle->get_Res_انکڑے());
    value.سیٹ_Valid_Locale_انکڑے_اندراج(bundle->f_Valid_Locale_انکڑے_اندراج);
    UResource_انکڑے_اندراج *ماپا_اندراج = bundle->f_انکڑے->f_ماپا;
    UBool has_ماپا = ماپا_اندراج != خالی_سوچک && U_SUCCESS(ماپا_اندراج->fBogus);
    value.سیٹ_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_کلید, value, !has_ماپا, error_کوڈ);
    إذا (has_ماپا) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ڈھیر_UResource_Bundle ماپا_Bundle;
        UResourceBundle &ماپا_Ref = ماپا_Bundle.ref();
        ماپا_Ref.f_انکڑے = ماپا_اندراج;
        ماپا_Ref.f_Valid_Locale_انکڑے_اندراج = bundle->f_Valid_Locale_انکڑے_اندراج;
        ماپا_Ref.fHasFallback = !ماپا_Ref.get_Res_انکڑے().noFallback;
        ماپا_Ref.f_Is_چوٹی_Level = سچ;
        ماپا_Ref.fRes = ماپا_Ref.get_Res_انکڑے().جڑ_Res;
        ماپا_Ref.f_ناپ = res_گنتی_Array_اشیاء(&ماپا_Ref.get_Res_انکڑے(), ماپا_Ref.fRes);
        ماپا_Ref.f_اشاریہ = -1;
        اندراج_Increase(ماپا_اندراج);

        // Look up the container item in the parent bundle.
        ڈھیر_UResource_Bundle containerBundle;
        ثابت UResourceBundle *rb;
        UError_کوڈ رستہ_Error_کوڈ = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        إذا (bundle->f_Res_رستہ == خالی_سوچک || *bundle->f_Res_رستہ == 0) {
            rb = ماپا_Bundle.getAlias();
        } وإلا {
            rb = ures_get_By_کلید_With_Fallback(ماپا_Bundle.getAlias(), bundle->f_Res_رستہ,
                                           containerBundle.getAlias(), &رستہ_Error_کوڈ);
        }
        إذا (U_SUCCESS(رستہ_Error_کوڈ)) {
            get_All_اشیاء_With_Fallback(rb, value, sink, error_کوڈ);
        }
    }
}

بنية GetAllChildrenSink : عام ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    مجازی ~GetAllChildrenSink() override;
    مجازی فراغ put(ثابت حرف *کلید, Resource_قدر &value, UBool is_جڑ,
           UError_کوڈ &error_کوڈ) override {
        Resource_جدول اشیاء_جدول = value.get_جدول(error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        لكل (int32_t i = 0; اشیاء_جدول.get_کلید_And_قدر(i, کلید, value); ++i) {
            إذا (value.getType() == URES_ALIAS) {
                Resource_انکڑے_قدر& rdv = static_cast<Resource_انکڑے_قدر&>(value);
                ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
                UResourceBundle* aliasRB = get_Alias_نشانہ_As_Resource_Bundle(rdv.get_انکڑے(), rdv.getResource(), خالی_سوچک, -1,
                                                                          rdv.get_Valid_Locale_انکڑے_اندراج(), خالی_سوچک, 0,
                                                                          ڈھیر_عارضی_Bundle.getAlias(), &error_کوڈ);
                إذا (U_SUCCESS(error_کوڈ)) {
                    Resource_انکڑے_قدر aliased_قدر;
                    aliased_قدر.سیٹ_انکڑے(aliasRB->get_Res_انکڑے());
                    aliased_قدر.سیٹ_Valid_Locale_انکڑے_اندراج(aliasRB->f_Valid_Locale_انکڑے_اندراج);
                    aliased_قدر.سیٹ_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    إذا (aliased_قدر.getType() != URES_جدول) {
                        dest.put(کلید, aliased_قدر, is_جڑ, error_کوڈ);
                    } وإلا {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_قدر_Type = URES_جدول;
                        CharString جدول_رستہ;
                        جدول_رستہ.append(aliasRB->f_Res_رستہ, error_کوڈ);
                        ثابت حرف* ماپا_کلید = کلید; // dest.put() changes the key
                        dest.put(ماپا_کلید, aliased_قدر, is_جڑ, error_کوڈ);
                        UResource_انکڑے_اندراج* اندراج = aliasRB->f_انکڑے;
                        Resource res = aliasRB->fRes;
                        طالما (aliased_قدر_Type == URES_جدول && اندراج->f_ماپا != خالی_سوچک) {
                            CharString local_رستہ;
                            local_رستہ.copyFrom(جدول_رستہ, error_کوڈ);
                            حرف* local_رستہ_As_Char_Ptr = local_رستہ.انکڑے();
                            ثابت حرف* بچہ_کلید;
                            اندراج = اندراج->f_ماپا;
                            res = اندراج->f_انکڑے.جڑ_Res;
                            Resource newRes = res_لبھو_Resource(&اندراج->f_انکڑے, res, &local_رستہ_As_Char_Ptr, &بچہ_کلید);
                            إذا (newRes != RES_BOGUS) {
                                aliased_قدر.سیٹ_انکڑے(اندراج->f_انکڑے);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_قدر.سیٹ_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_قدر_Type = aliased_قدر.getType();
                                إذا (aliased_قدر_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_انکڑے_قدر& rdv2 = static_cast<Resource_انکڑے_قدر&>(aliased_قدر);
                                    aliasRB = get_Alias_نشانہ_As_Resource_Bundle(rdv2.get_انکڑے(), rdv2.getResource(), خالی_سوچک, -1,
                                                                             rdv2.get_Valid_Locale_انکڑے_اندراج(), خالی_سوچک, 0,
                                                                             ڈھیر_عارضی_Bundle.getAlias(), &error_کوڈ);
                                    جدول_رستہ.clear();
                                    جدول_رستہ.append(aliasRB->f_Res_رستہ, error_کوڈ);
                                    اندراج = aliasRB->f_انکڑے;
                                    res = aliasRB->fRes;
                                    aliased_قدر.سیٹ_انکڑے(اندراج->f_انکڑے);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_قدر.سیٹ_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_قدر_Type = aliased_قدر.getType();
                                }
                                إذا (aliased_قدر_Type == URES_جدول) {
                                    dest.put(ماپا_کلید, aliased_قدر, is_جڑ, error_کوڈ);
                                } وإلا {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_کوڈ = U_اندرونی_پروگرام_ERROR;
                                    أرجع;
                                }
                            }
                        }
                    }
                }
            } وإلا {
                dest.put(کلید, value, is_جڑ, error_کوڈ);
            }
            إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI فراغ U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *رستہ,
                                icu::ResourceSink &sink, UError_کوڈ &error_کوڈ) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_اشیاء_With_Fallback(bundle, رستہ, allChildrenSink, error_کوڈ);
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
ures_get_قدر_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *رستہ,
                          UResourceBundle *عارضی_Fill_وچ,
                          Resource_انکڑے_قدر &value, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    إذا (رستہ == خالی_سوچک) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ثابت UResourceBundle *rb;
    إذا (*رستہ == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_get_By_کلید_With_Fallback(bundle, رستہ, عارضی_Fill_وچ, &error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) {
            أرجع;
        }
    }
    value.سیٹ_انکڑے(rb->get_Res_انکڑے());
    value.سیٹ_Valid_Locale_انکڑے_اندراج(rb->f_Valid_Locale_انکڑے_اندراج);
    value.سیٹ_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI فراغ U_EXPORT2
ures_get_All_اشیاء_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *رستہ,
                             icu::ResourceSink &sink, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    إذا (رستہ == خالی_سوچک) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ڈھیر_UResource_Bundle ڈھیر_Bundle;
    ثابت UResourceBundle *rb;
    إذا (*رستہ == 0) {
        // empty path
        rb = bundle;
    } وإلا {
        rb = ures_get_By_کلید_With_Fallback(bundle, رستہ, ڈھیر_Bundle.getAlias(), &error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) {
            أرجع;
        }
    }
    // Get all table items with fallback.
    Resource_انکڑے_قدر value;
    get_All_اشیاء_With_Fallback(rb, value, sink, error_کوڈ);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلید(ثابت UResourceBundle* resB,
              ثابت حرف* وچ_کلید,
              UResourceBundle* fill_وچ U_LIFETIME_BOUND,
              UError_کوڈ* حالت) {
    Resource res = RES_BOGUS;
    UResource_انکڑے_اندراج *انکڑے_اندراج = خالی_سوچک;
    ثابت حرف *کلید = وچ_کلید;

    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع fill_وچ;
    }
    إذا(resB == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع fill_وچ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        int32_t t;
        res = res_get_جدول_شے_By_کلید(&resB->get_Res_انکڑے(), resB->fRes, &t, &کلید);
        إذا(res == RES_BOGUS) {
            کلید = وچ_کلید;
            إذا(resB->fHasFallback) {
                انکڑے_اندراج = get_Fallback_انکڑے(resB, &کلید, &res, حالت);
                إذا(U_SUCCESS(*حالت)) {
                    /* check if resB->fResPath gives the right name here */
                    أرجع init_resb_نتیجہ(انکڑے_اندراج, res, کلید, -1, resB, fill_وچ, حالت);
                } وإلا {
                    *حالت = U_MISSING_RESOURCE_ERROR;
                }
            } وإلا {
                *حالت = U_MISSING_RESOURCE_ERROR;
            }
        } وإلا {
            أرجع init_resb_نتیجہ(resB->f_انکڑے, res, کلید, -1, resB, fill_وچ, حالت);
        }
    } 
#إذا 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    وإلا إذا(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == سچ) {
        /* here should go a first attempt to locate the key using index table */
        انکڑے_اندراج = get_Fallback_انکڑے(resB, &کلید, &res, حالت);
        إذا(U_SUCCESS(*حالت)) {
            أرجع init_resb_نتیجہ(انکڑے_اندراج, res, کلید, resB, fill_وچ, حالت);
        } وإلا {
            *حالت = U_MISSING_RESOURCE_ERROR;
        }
    }
#جے_انت    
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع fill_وچ;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلید(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                    ثابت حرف* وچ_کلید,
                    int32_t* len,
                    UError_کوڈ* حالت) {
    Resource res = RES_BOGUS;
    UResource_انکڑے_اندراج *انکڑے_اندراج = خالی_سوچک;
    ثابت حرف* کلید = وچ_کلید;

    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا(resB == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    إذا(URES_IS_جدول(type)) {
        int32_t t=0;

        res = res_get_جدول_شے_By_کلید(&resB->get_Res_انکڑے(), resB->fRes, &t, &کلید);

        إذا(res == RES_BOGUS) {
            کلید = وچ_کلید;
            إذا(resB->fHasFallback) {
                انکڑے_اندراج = get_Fallback_انکڑے(resB, &کلید, &res, حالت);
                إذا(U_SUCCESS(*حالت)) {
                    بدل (RES_GET_TYPE(res)) {
                    حالة URES_STRING:
                    حالة URES_STRING_V2:
                        أرجع res_getString({resB, کلید}, &انکڑے_اندراج->f_انکڑے, res, len);
                    حالة URES_ALIAS:
                      {
                        ثابت char16_t* نتیجہ = خالی_سوچک;
                        UResourceBundle *عارضی_Res = ures_get_By_کلید(resB, وچ_کلید, خالی_سوچک, حالت);
                        نتیجہ = ures_getString(عارضی_Res, len, حالت);
                        ures_close(عارضی_Res);
                        أرجع نتیجہ;
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
                أرجع res_getString({resB, کلید}, &resB->get_Res_انکڑے(), res, len);
            حالة URES_ALIAS:
              {
                ثابت char16_t* نتیجہ = خالی_سوچک;
                UResourceBundle *عارضی_Res = ures_get_By_کلید(resB, وچ_کلید, خالی_سوچک, حالت);
                نتیجہ = ures_getString(عارضی_Res, len, حالت);
                ures_close(عارضی_Res);
                أرجع نتیجہ;
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
        انکڑے_اندراج = get_Fallback_انکڑے(resB, &کلید, &res, حالت);
        إذا(U_SUCCESS(*حالت)) {
            // TODO: Tracing
            أرجع res_getString(rd, res, len);
        } وإلا {
            *حالت = U_MISSING_RESOURCE_ERROR;
        }
    } 
#جے_انت    
    وإلا {
        *حالت = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع خالی_سوچک;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_کلید(ثابت UResourceBundle* resB,
                        ثابت حرف* کلید,
                        حرف* dest U_LIFETIME_BOUND,
                        int32_t* p_لمائی,
                        UBool forceCopy,
                        UError_کوڈ* حالت) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_کلید(resB, کلید, &length16, حالت);
    أرجع ures_منزل_UTF8_String(s16, length16, dest, p_لمائی, forceCopy, حالت);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ثابت حرف* U_EXPORT2
ures_get_Locale_اندرونی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_کوڈ* حالت)
{
    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (!resourceBundle) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    } وإلا {
      أرجع resourceBundle->f_انکڑے->fName;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocale(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_کوڈ* حالت)
{
  أرجع ures_get_Locale_اندرونی(resourceBundle, حالت);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocaleByType(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_انکڑے_Locale_Type type,
                     UError_کوڈ* حالت) {
    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (!resourceBundle) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    } وإلا {
        بدل(type) {
        حالة ULOC_ACTUAL_LOCALE:
            أرجع resourceBundle->f_انکڑے->fName;
        حالة ULOC_VALID_LOCALE:
            أرجع resourceBundle->f_Valid_Locale_انکڑے_اندراج->fName;
        حالة ULOC_REQUESTED_LOCALE:
        افتراضي:
            *حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع خالی_سوچک;
        }
    }
}

U_CFUNC ثابت حرف*
ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  إذا(resB == خالی_سوچک) {
    أرجع خالی_سوچک;
  }

  أرجع resB->f_انکڑے->fName;
}

#جے_تعریف_شدہ URES_DEBUG
U_CFUNC ثابت حرف*
ures_get_رستہ(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  إذا(resB == خالی_سوچک) {
    أرجع خالی_سوچک;
  }

  أرجع resB->f_انکڑے->f_رستہ;
}
#جے_انت

ساكن UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ثابت حرف* رستہ,
                  ثابت حرف* localeID,
                  UResOpenType openType,
                  UError_کوڈ* حالت) {
    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    UResource_انکڑے_اندراج *اندراج;
    إذا(openType != URES_OPEN_سدھا) {
        إذا (localeID == خالی_سوچک) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_ادھار_Name(localeID, *حالت);
        إذا(U_FAILURE(*حالت)) {
            *حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع خالی_سوچک;
        }
        اندراج = اندراج_Open(رستہ, canonLocaleID.انکڑے(), openType, حالت);
    } وإلا {
        اندراج = اندراج_Open_سدھا(رستہ, localeID, حالت);
    }
    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا(اندراج == خالی_سوچک) {
        *حالت = U_MISSING_RESOURCE_ERROR;
        أرجع خالی_سوچک;
    }

    UBool is_ڈھیر_Object;
    إذا(r == خالی_سوچک) {
        r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        إذا(r == خالی_سوچک) {
            اندراج_Close(اندراج);
            *حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }
        is_ڈھیر_Object = خطأ;
    } وإلا {  // fill-in
        is_ڈھیر_Object = ures_is_ڈھیر_Object(r);
        ures_closeBundle(r, خطأ);
    }
    uprv_memset(r, 0, حجم(UResourceBundle));
    ures_سیٹ_Is_ڈھیر_Object(r, is_ڈھیر_Object);

    r->f_Valid_Locale_انکڑے_اندراج = r->f_انکڑے = اندراج;
    r->fHasFallback = openType != URES_OPEN_سدھا && !r->get_Res_انکڑے().noFallback;
    r->f_Is_چوٹی_Level = سچ;
    r->fRes = r->get_Res_انکڑے().جڑ_Res;
    r->f_ناپ = res_گنتی_Array_اشیاء(&r->get_Res_انکڑے(), r->fRes);
    r->f_اشاریہ = -1;

    ResourceTracer(r).traceOpen();

    أرجع r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ثابت حرف* رستہ, ثابت حرف* localeID, UError_کوڈ* حالت) {
    أرجع ures_openWithType(خالی_سوچک, رستہ, localeID, URES_OPEN_LOCALE_DEFAULT_جڑ, حالت);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت حرف* رستہ, ثابت حرف* localeID, UError_کوڈ* حالت) {
    أرجع ures_openWithType(خالی_سوچک, رستہ, localeID, URES_OPEN_LOCALE_جڑ, حالت);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_سدھا(ثابت حرف* رستہ, ثابت حرف* localeID, UError_کوڈ* حالت) {
    أرجع ures_openWithType(خالی_سوچک, رستہ, localeID, URES_OPEN_سدھا, حالت);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_open_Fill_وچ(UResourceBundle *r, ثابت حرف* رستہ,
                ثابت حرف* localeID, UError_کوڈ* حالت) {
    إذا(U_SUCCESS(*حالت) && r == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ures_openWithType(r, رستہ, localeID, URES_OPEN_LOCALE_DEFAULT_جڑ, حالت);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI فراغ U_EXPORT2
ures_open_سدھا_Fill_وچ(UResourceBundle *r, ثابت حرف* رستہ, ثابت حرف* localeID, UError_کوڈ* حالت) {
    إذا(U_SUCCESS(*حالت) && r == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ures_openWithType(r, رستہ, localeID, URES_OPEN_سدھا, حالت);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_گنتی_Array_اشیاء(ثابت UResourceBundle* resourceBundle,
                  ثابت حرف* resource_کلید,
                  UError_کوڈ* حالت)
{
    UResourceBundle res_انکڑے;
    ures_init_ڈھیر_Object(&res_انکڑے);
    إذا (حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(resourceBundle == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    ures_get_By_کلید(resourceBundle, resource_کلید, &res_انکڑے, حالت);
    
    إذا(res_انکڑے.get_Res_انکڑے().انکڑے != خالی_سوچک) {
        int32_t نتیجہ = res_گنتی_Array_اشیاء(&res_انکڑے.get_Res_انکڑے(), res_انکڑے.fRes);
        ures_close(&res_انکڑے);
        أرجع نتیجہ;
    } وإلا {
        *حالت = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_انکڑے);
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
ures_get_نسخہ_نمبر_اندرونی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    إذا (!resourceBundle) أرجع خالی_سوچک;

    إذا(resourceBundle->f_نسخہ == خالی_سوچک) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_کوڈ حالت = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ثابت char16_t* minor_نسخہ = ures_get_String_By_کلید(resourceBundle, k_نسخہ_ٹیگ, &minor_len, &حالت);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_نسخہ = (حرف *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        إذا (((UResourceBundle *)resourceBundle)->f_نسخہ == خالی_سوچک) {
            أرجع خالی_سوچک;
        }
       
        إذا(minor_len > 0) {
            u_UChars_منزل_Chars(minor_نسخہ, resourceBundle->f_نسخہ , minor_len);
            resourceBundle->f_نسخہ[len] =  '\0';
        }
        وإلا {
            uprv_strcpy(resourceBundle->f_نسخہ, k_Default_Minor_نسخہ);
        }
    }

    أرجع resourceBundle->f_نسخہ;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_نسخہ_نمبر(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    أرجع ures_get_نسخہ_نمبر_اندرونی(resourceBundle);
}

U_CAPI فراغ U_EXPORT2 ures_get_نسخہ(ثابت UResourceBundle* resB, UVersionInfo نسخہ_Info) {
    إذا (!resB) أرجع;

    u_نسخہ_From_String(نسخہ_Info, ures_get_نسخہ_نمبر_اندرونی(resB));
}

/** Tree support functions *******************************/
#تعریف_کرو اشاریہ_LOCALE_NAME "res_index"
#تعریف_کرو اشاریہ_ٹیگ         "InstalledLocales"
#تعریف_کرو DEFAULT_ٹیگ       "default"

#إذا defined(URES_درخت_DEBUG)
#ضمن <stdio.h>
#جے_انت

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
ures_loc_گنتی_Locales(UEnumeration *en, UError_کوڈ * /*حالت*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    أرجع ures_get_ناپ(&ctx->installed);
}

U_CDECL_BEGIN


ساكن ثابت حرف * U_CALLCONV
ures_loc_اگلا_Locale(UEnumeration* en,
                    int32_t* نتیجہ_لمائی,
                    UError_کوڈ* حالت) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = خالی_سوچک;
    ثابت حرف *نتیجہ = خالی_سوچک;
    int32_t len = 0;
    إذا (ures_has_اگلا(res) && (k = ures_get_اگلا_Resource(res, &ctx->curr, حالت)) != خالی_سوچک) {
        نتیجہ = ures_get_کلید(k);
        len = (int32_t)uprv_strlen(نتیجہ);
    }
    إذا (نتیجہ_لمائی) {
        *نتیجہ_لمائی = len;
    }
    أرجع نتیجہ;
}

ساكن فراغ U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_کوڈ* /*حالت*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_سرا

ساكن ثابت UEnumeration gLocalesEnum = {
    خالی_سوچک,
        خالی_سوچک,
        ures_loc_closeLocales,
        ures_loc_گنتی_Locales,
        uenum_unextDefault,
        ures_loc_اگلا_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_دستیاب_Locales(ثابت حرف *رستہ, UError_کوڈ *حالت)
{
    UResourceBundle *idx = خالی_سوچک;
    UEnumeration *en = خالی_سوچک;
    ULocalesContext *myContext = خالی_سوچک;

    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(حجم(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    إذا(!en || !myContext) {
        *حالت = U_حافظہ_ونڈ_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        أرجع خالی_سوچک;
    }
    uprv_memcpy(en, &gLocalesEnum, حجم(UEnumeration));

    ures_init_ڈھیر_Object(&myContext->installed);
    ures_init_ڈھیر_Object(&myContext->curr);
    idx = ures_open_سدھا(رستہ, اشاریہ_LOCALE_NAME, حالت);
    ures_get_By_کلید(idx, اشاریہ_ٹیگ, &myContext->installed, حالت);
    إذا(U_SUCCESS(*حالت)) {
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            رستہ, اشاریہ_LOCALE_NAME, اشاریہ_ٹیگ, ures_get_کلید(&myContext->installed));
#جے_انت
        en->context = myContext;
    } وإلا {
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", رستہ, u_errorName(*حالت));
#جے_انت
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = خالی_سوچک;
    }
    
    ures_close(idx);
    
    أرجع en;
}

ساكن UBool is_Locale_وچ_List(UEnumeration *locEnum, ثابت حرف *loc_منزل_تلاش, UError_کوڈ *حالت) {
    ثابت حرف *loc;
    طالما ((loc = uenum_اگلا(locEnum, خالی_سوچک, حالت)) != خالی_سوچک) {
        إذا (uprv_strcmp(loc, loc_منزل_تلاش) == 0) {
            أرجع سچ;
        }
    }
    أرجع خطأ;
}

ساكن فراغ get_ماپا_For_Functional_Equivalent(ثابت حرف*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      ماپا) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UError_کوڈ sub_حالت = U_ZERO_ERROR;
    ماپا.clear();
    إذا (res != خالی_سوچک) {
        ures_get_By_کلید(res, "%%Parent", bund1, &sub_حالت);
        إذا (U_SUCCESS(sub_حالت)) {
            int32_t length16;
            ثابت char16_t* s16 = ures_getString(bund1, &length16, &sub_حالت);
            ماپا.appendInvariantChars(s16, length16, sub_حالت);
        }
    }
    
    // If none there, use normal truncation parent
    إذا (U_FAILURE(sub_حالت) || ماپا.is_خالی_تھاواں()) {
        sub_حالت = U_ZERO_ERROR;
        ماپا = ulocimp_get_ماپا(localeID, sub_حالت);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(حرف *نتیجہ, int32_t نتیجہ_سمرتھا,
                             ثابت حرف *رستہ, ثابت حرف *resName, ثابت حرف *keyword, ثابت حرف *locid,
                             UBool *is_دستیاب, UBool omitDefault, UError_کوڈ *حالت)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString لبھیا;
    CharString ماپا;
    CharString بھریا;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = خالی_سوچک;
    UError_کوڈ sub_حالت = U_ZERO_ERROR;
    int32_t length = 0;
    إذا(U_FAILURE(*حالت)) أرجع 0;
    CharString kwVal;
    إذا (keyword != خالی_سوچک && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_قدر(locid, keyword, sub_حالت);
        إذا (kwVal == DEFAULT_ٹیگ) {
            kwVal.clear();
        }
    }
    إذا (locid == خالی_سوچک) {
        locid = uloc_getDefault();
    }
    CharString ادھار = ulocimp_get_ادھار_Name(locid, sub_حالت);
#إذا defined(URES_درخت_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.انکڑے(), ادھار.انکڑے(), u_errorName(sub_حالت));
#جے_انت
    ures_init_ڈھیر_Object(&bund1);
    ures_init_ڈھیر_Object(&bund2);

    ماپا.copyFrom(ادھار, sub_حالت);
    لبھیا.copyFrom(ادھار, sub_حالت);

    إذا(is_دستیاب) {
        UEnumeration *locEnum = ures_open_دستیاب_Locales(رستہ, &sub_حالت);
        *is_دستیاب = سچ;
        إذا (U_SUCCESS(sub_حالت)) {
            *is_دستیاب = is_Locale_وچ_List(locEnum, ماپا.انکڑے(), &sub_حالت);
        }
        uenum_close(locEnum);
    }

    إذا(U_FAILURE(sub_حالت)) {
        *حالت = sub_حالت;
        أرجع 0;
    }
    
    افعل {
        sub_حالت = U_ZERO_ERROR;
        res = ures_open(رستہ, ماپا.انکڑے(), &sub_حالت);
        إذا(((sub_حالت == U_USING_FALLBACK_WARNING) ||
            (sub_حالت == U_USING_DEFAULT_WARNING)) && is_دستیاب)
        {
            *is_دستیاب = خطأ;
        }
        is_دستیاب = خالی_سوچک; /* only want to set this the first time around */
        
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", رستہ?رستہ:"ICUDATA", ماپا.انکڑے(), u_errorName(sub_حالت), ures_getLocale(res, &sub_حالت));
#جے_انت
        إذا(U_FAILURE(sub_حالت)) {
            *حالت = sub_حالت;
        } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
            ures_get_By_کلید(res,resName,&bund1, &sub_حالت);
            إذا(sub_حالت == U_ZERO_ERROR) {
                ثابت char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#إذا defined(URES_درخت_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    رستہ?رستہ:"ICUDATA", ماپا.انکڑے(), u_errorName(sub_حالت));
#جے_انت
                defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_ٹیگ, &defLen, &sub_حالت);
                إذا(U_SUCCESS(sub_حالت) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        رستہ?رستہ:"ICUDATA", ماپا.انکڑے(), keyword, defVal.انکڑے(), u_errorName(sub_حالت));
#جے_انت
                    defLoc.copyFrom(ماپا, sub_حالت);
                    إذا(kwVal.is_خالی_تھاواں()) {
                        kwVal.append(defVal, sub_حالت);
#إذا defined(URES_درخت_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            رستہ?رستہ:"ICUDATA", ماپا.انکڑے(), keyword, kwVal.انکڑے());
#جے_انت
                    }
                }
            }
        }
        
        sub_حالت = U_ZERO_ERROR;

        إذا (res != خالی_سوچک) {
            لبھیا.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_حالت), sub_حالت);
        }

        إذا (لبھیا != ماپا) {
            ماپا.copyFrom(لبھیا, sub_حالت);
        } وإلا {
            get_ماپا_For_Functional_Equivalent(لبھیا.انکڑے(),res,&bund1,ماپا);
        }
        ures_close(res);
    } طالما(defVal.is_خالی_تھاواں() && !لبھیا.is_خالی_تھاواں() && لبھیا != "root" && U_SUCCESS(*حالت));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    ماپا.copyFrom(ادھار, sub_حالت);
    لبھیا.copyFrom(ادھار, sub_حالت);

    افعل {
        res = ures_open(رستہ, ماپا.انکڑے(), &sub_حالت);
        إذا((sub_حالت == U_USING_FALLBACK_WARNING) && is_دستیاب) {
            *is_دستیاب = خطأ;
        }
        is_دستیاب = خالی_سوچک; /* only want to set this the first time around */
        
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            رستہ?رستہ:"ICUDATA", ماپا.انکڑے(), u_errorName(sub_حالت), kwVal.انکڑے());
#جے_انت
        إذا(U_FAILURE(sub_حالت)) {
            *حالت = sub_حالت;
        } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
            ures_get_By_کلید(res,resName,&bund1, &sub_حالت);
#إذا defined(URES_درخت_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_حالت));
#جے_انت
            إذا(sub_حالت == U_ZERO_ERROR) {
                ures_get_By_کلید(&bund1, kwVal.انکڑے(), &bund2, &sub_حالت);
#إذا defined(URES_درخت_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.انکڑے(), u_errorName(sub_حالت));
#جے_انت
                إذا(sub_حالت == U_ZERO_ERROR) {
#إذا defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        رستہ?رستہ:"ICUDATA", ماپا.انکڑے(), keyword, kwVal.انکڑے(), u_errorName(sub_حالت));
#جے_انت
                    إذا (ماپا.is_خالی_تھاواں()) {
                        بھریا.clear().append("root", sub_حالت);
                    } وإلا {
                        بھریا.copyFrom(ماپا, sub_حالت);
                    }
                        /* now, recalculate default kw if need be */
                        إذا(defLoc.length() > بھریا.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#إذا defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    رستہ?رستہ:"ICUDATA", بھریا.انکڑے());
#جے_انت
                          defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_ٹیگ, &defLen, &sub_حالت);
                          إذا(U_SUCCESS(sub_حالت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    رستہ?رستہ:"ICUDATA", بھریا.انکڑے(), keyword, defVal.انکڑے(), u_errorName(sub_حالت));
#جے_انت
                            defLoc.copyFrom(بھریا, sub_حالت);
                          }
                        } /* end of recalculate default KW */
#إذا defined(URES_درخت_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.انکڑے(), بھریا.انکڑے());
                        }
#جے_انت
                } وإلا {
#إذا defined(URES_درخت_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_حالت), ماپا.انکڑے(), kwVal.انکڑے());
#جے_انت
                }
            }
        }
        
        sub_حالت = U_ZERO_ERROR;
        UBool have_لبھیا = خطأ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        إذا (res != خالی_سوچک && uprv_strcmp(resName, "collations") == 0) {
            ثابت حرف *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_حالت);
            إذا (U_SUCCESS(sub_حالت) && validLoc != خالی_سوچک && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_حالت);
                CharString ماپا_Lang = ulocimp_getLanguage(ماپا.منزل_String_Piece(), sub_حالت);
                إذا (U_SUCCESS(sub_حالت) && validLang != ماپا_Lang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    لبھیا.clear().append(validLoc, sub_حالت);
                    have_لبھیا = سچ;
                }
            }
            sub_حالت = U_ZERO_ERROR;
        }
        إذا (!have_لبھیا) {
            لبھیا.copyFrom(ماپا, sub_حالت);
        }

        get_ماپا_For_Functional_Equivalent(لبھیا.انکڑے(),res,&bund1,ماپا);
        ures_close(res);
        sub_حالت = U_ZERO_ERROR;
    } طالما(بھریا.is_خالی_تھاواں() && !لبھیا.is_خالی_تھاواں() && U_SUCCESS(*حالت));

    إذا(بھریا.is_خالی_تھاواں() && kwVal != defVal) {
#إذا defined(URES_درخت_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.انکڑے(), defVal.انکڑے());
#جے_انت
        kwVal.clear().append(defVal, sub_حالت);
        ماپا.copyFrom(ادھار, sub_حالت);
        لبھیا.copyFrom(ادھار, sub_حالت);

        افعل { /* search for 'default' named item */
            res = ures_open(رستہ, ماپا.انکڑے(), &sub_حالت);
            إذا((sub_حالت == U_USING_FALLBACK_WARNING) && is_دستیاب) {
                *is_دستیاب = خطأ;
            }
            is_دستیاب = خالی_سوچک; /* only want to set this the first time around */
            
#إذا defined(URES_درخت_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                رستہ?رستہ:"ICUDATA", ماپا.انکڑے(), u_errorName(sub_حالت), kwVal.انکڑے());
#جے_انت
            إذا(U_FAILURE(sub_حالت)) {
                *حالت = sub_حالت;
            } وإلا إذا(sub_حالت == U_ZERO_ERROR) {
                ures_get_By_کلید(res,resName,&bund1, &sub_حالت);
                إذا(sub_حالت == U_ZERO_ERROR) {
                    ures_get_By_کلید(&bund1, kwVal.انکڑے(), &bund2, &sub_حالت);
                    إذا(sub_حالت == U_ZERO_ERROR) {
#إذا defined(URES_درخت_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", رستہ?رستہ:"ICUDATA",
                            ماپا.انکڑے(), keyword, kwVal.انکڑے(), u_errorName(sub_حالت));
#جے_انت
                        إذا (ماپا.is_خالی_تھاواں()) {
                            بھریا.clear().append("root", sub_حالت);
                        } وإلا {
                            بھریا.copyFrom(ماپا, sub_حالت);
                        }
                        
                        /* now, recalculate default kw if need be */
                        إذا(defLoc.length() > بھریا.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#إذا defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    رستہ?رستہ:"ICUDATA", بھریا.انکڑے());
#جے_انت
                          defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_ٹیگ, &defLen, &sub_حالت);
                          إذا(U_SUCCESS(sub_حالت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#إذا defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    رستہ?رستہ:"ICUDATA", بھریا.انکڑے(), keyword, defVal.انکڑے(), u_errorName(sub_حالت));
#جے_انت
                            defLoc.copyFrom(بھریا, sub_حالت);
                          }
                        } /* end of recalculate default KW */
#إذا defined(URES_درخت_DEBUG)
                        وإلا {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.انکڑے(), بھریا.انکڑے());
                        }
#جے_انت
                    }
                }
            }
            
            sub_حالت = U_ZERO_ERROR;
            لبھیا.copyFrom(ماپا, sub_حالت);
            get_ماپا_For_Functional_Equivalent(لبھیا.انکڑے(),res,&bund1,ماپا);
            ures_close(res);
            sub_حالت = U_ZERO_ERROR;
        } طالما(بھریا.is_خالی_تھاواں() && !لبھیا.is_خالی_تھاواں() && U_SUCCESS(*حالت));
    }
    
    إذا(U_SUCCESS(*حالت)) {
        إذا(بھریا.is_خالی_تھاواں()) {
#إذا defined(URES_درخت_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.انکڑے());
#جے_انت
          *حالت = U_MISSING_RESOURCE_ERROR;
        } وإلا إذا(omitDefault) {
#إذا defined(URES_درخت_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", بھریا.انکڑے(), defLoc.انکڑے(), لبھیا.انکڑے());
#جے_انت        
          إذا(defLoc.length() <= بھریا.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            إذا(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#إذا defined(URES_درخت_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.انکڑے());
#جے_انت
              kwVal.clear();
            }
          }
        }
        لبھیا.copyFrom(بھریا, sub_حالت);
        إذا(!kwVal.is_خالی_تھاواں()) {
            لبھیا
                .append("@", sub_حالت)
                .append(keyword, sub_حالت)
                .append("=", sub_حالت)
                .append(kwVal, sub_حالت);
        } وإلا إذا(!omitDefault) {
            لبھیا
                .append("@", sub_حالت)
                .append(keyword, sub_حالت)
                .append("=", sub_حالت)
                .append(defVal, sub_حالت);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = لبھیا.length();

    إذا(U_SUCCESS(*حالت)) {
        int32_t copy_لمائی = uprv_min(length, نتیجہ_سمرتھا);
        إذا(copy_لمائی>0) {
            لبھیا.extract(نتیجہ, copy_لمائی, sub_حالت);
        }
        إذا(length == 0) {
          *حالت = U_MISSING_RESOURCE_ERROR; 
        }
    } وإلا {
        length = 0;
        نتیجہ[0]=0;
    }
    أرجع u_terminateChars(نتیجہ, نتیجہ_سمرتھا, length, حالت);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_اقدار(ثابت حرف *رستہ, ثابت حرف *keyword, UError_کوڈ *حالت)
{
#تعریف_کرو اقدار_BUF_ناپ 2048
#تعریف_کرو اقدار_LIST_ناپ 512
    
    حرف       اقدار_Buf[اقدار_BUF_ناپ];
    int32_t    اقدار_اشاریہ = 0;
    ثابت حرف *اقدار_List[اقدار_LIST_ناپ];
    int32_t    اقدار_گنتی = 0;
    
    ثابت حرف *locale;
    int32_t     locLen;
    
    UEnumeration *locs = خالی_سوچک;
    
    UResourceBundle    شے;
    UResourceBundle    sub_شے;
    
    ures_init_ڈھیر_Object(&شے);
    ures_init_ڈھیر_Object(&sub_شے);
    locs = ures_open_دستیاب_Locales(رستہ, حالت);
    
    إذا(U_FAILURE(*حالت)) {
        ures_close(&شے);
        ures_close(&sub_شے);
        أرجع خالی_سوچک;
    }
    
    اقدار_Buf[0]=0;
    اقدار_Buf[1]=0;

    طالما ((locale = uenum_اگلا(locs, &locLen, حالت)) != خالی_سوچک) {
        UResourceBundle   *bund = خالی_سوچک;
        UResourceBundle   *subPtr = خالی_سوچک;
        UError_کوڈ sub_حالت = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(رستہ, locale, &sub_حالت);
        
#إذا defined(URES_درخت_DEBUG)
        إذا(!bund || U_FAILURE(sub_حالت)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                رستہ?رستہ:"<ICUDATA>", keyword, locale, u_errorName(sub_حالت));
        }
#جے_انت
        
        ures_get_By_کلید(bund, keyword, &شے, &sub_حالت);
        
        إذا(!bund || U_FAILURE(sub_حالت)) {
#إذا defined(URES_درخت_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                رستہ?رستہ:"<ICUDATA>", keyword, locale, u_errorName(sub_حالت));
#جے_انت
            ures_close(bund);
            bund = خالی_سوچک;
            استمر;
        }

        طالما ((subPtr = ures_get_اگلا_Resource(&شے, &sub_شے, &sub_حالت)) != خالی_سوچک
            && U_SUCCESS(sub_حالت)) {
            ثابت حرف *k;
            int32_t i;
            k = ures_get_کلید(subPtr);

#إذا defined(URES_درخت_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#جے_انت
            إذا(k == خالی_سوچک || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_ٹیگ) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                استمر;
            }
            لكل(i=0; i<اقدار_گنتی; i++) {
                إذا(!uprv_strcmp(اقدار_List[i],k)) {
                    k = خالی_سوچک; /* found duplicate */
                    اكسر;
                }
            }
            إذا(k != خالی_سوچک) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                إذا((اقدار_گنتی >= (اقدار_LIST_ناپ-1)) ||       /* no more space in list .. */
                    ((اقدار_اشاریہ+kLen+1+1) >= اقدار_BUF_ناپ)) { /* no more space in buffer (string + 2 nulls) */
                    *حالت = U_ILLEGAL_دلیل_ERROR; /* out of space.. */
                } وإلا {
                    uprv_strcpy(اقدار_Buf+اقدار_اشاریہ, k);
                    اقدار_List[اقدار_گنتی++] = اقدار_Buf+اقدار_اشاریہ;
                    اقدار_اشاریہ += kLen;
#إذا defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        رستہ?رستہ:"<ICUDATA>", keyword, locale, k);
#جے_انت
                    اقدار_Buf[اقدار_اشاریہ++] = 0; /* terminate */
                }
            }
        }
        ures_close(bund);
    }
    اقدار_Buf[اقدار_اشاریہ++] = 0; /* terminate */
    
    ures_close(&شے);
    ures_close(&sub_شے);
    uenum_close(locs);
#إذا defined(URES_درخت_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*حالت), 
        اقدار_اشاریہ, اقدار_گنتی);
#جے_انت
    أرجع uloc_openKeywordList(اقدار_Buf, اقدار_اشاریہ, حالت);
}
#إذا 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ثابت UResourceBundle* res1, ثابت UResourceBundle* res2){
    إذا(res1==خالی_سوچک || res2==خالی_سوچک){
        أرجع res1==res2; /* pointer comparison */
    }
    إذا(res1->f_کلید==خالی_سوچک||  res2->f_کلید==خالی_سوچک){
        أرجع (res1->f_کلید==res2->f_کلید);
    }وإلا{
        إذا(uprv_strcmp(res1->f_کلید, res2->f_کلید)!=0){
            أرجع خطأ;
        }
    }
    إذا(uprv_strcmp(res1->f_انکڑے->fName, res2->f_انکڑے->fName)!=0){
        أرجع خطأ;
    }
    إذا(res1->f_انکڑے->f_رستہ == خالی_سوچک||  res2->f_انکڑے->f_رستہ==خالی_سوچک){
        أرجع (res1->f_انکڑے->f_رستہ == res2->f_انکڑے->f_رستہ);
    }وإلا{
        إذا(uprv_strcmp(res1->f_انکڑے->f_رستہ, res2->f_انکڑے->f_رستہ)!=0){
            أرجع خطأ;
        }
    }
    إذا(uprv_strcmp(res1->f_انکڑے->f_ماپا->fName, res2->f_انکڑے->f_ماپا->fName)!=0){
        أرجع خطأ;
    }
    إذا(uprv_strcmp(res1->f_انکڑے->f_ماپا->f_رستہ, res2->f_انکڑے->f_ماپا->f_رستہ)!=0){
        أرجع خطأ;
    }
    إذا(uprv_strncmp(res1->f_Res_رستہ, res2->f_Res_رستہ, res1->f_Res_رستہ_Len)!=0){
        أرجع خطأ;
    }
    إذا(res1->fRes != res2->fRes){
        أرجع خطأ;
    }
    أرجع سچ;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ثابت UResourceBundle* res, UError_کوڈ* حالت){
    UResourceBundle* bundle = خالی_سوچک;
    UResourceBundle* ret = خالی_سوچک;
    إذا(U_FAILURE(*حالت) || res == خالی_سوچک){
        أرجع خالی_سوچک;
    }
    bundle = ures_open(res->f_انکڑے->f_رستہ, res->f_انکڑے->fName, حالت);
    إذا(res->f_Res_رستہ!=خالی_سوچک){
        ret = ures_لبھو_Sub_Resource(bundle, res->f_Res_رستہ, خالی_سوچک, حالت);
        ures_close(bundle);
    }وإلا{
        ret = bundle;
    }
    أرجع ret;
}
U_CAPI ثابت UResourceBundle* U_EXPORT2
ures_get_ماپا_Bundle(ثابت UResourceBundle* res){
    إذا(res==خالی_سوچک){
        أرجع خالی_سوچک;
    }
    أرجع res->f_ماپا_Res;
}
#جے_انت

U_CAPI فراغ U_EXPORT2
ures_get_نسخہ_By_کلید(ثابت UResourceBundle* res, ثابت حرف *کلید, UVersionInfo ver, UError_کوڈ *حالت) {
  ثابت char16_t *str;
  int32_t len;
  str = ures_get_String_By_کلید(res, کلید, &len, حالت);
  إذا(U_SUCCESS(*حالت)) {
    u_نسخہ_From_UString(ver, str);
  } 
}

/* eof */
