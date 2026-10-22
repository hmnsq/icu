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

#شامل "unicode/ures.h"
#شامل "unicode/ustring.h"
#شامل "unicode/ucnv.h"
#شامل "bytesinkutil.h"
#شامل "charstr.h"
#شامل "uresimp.h"
#شامل "ustr_imp.h"
#شامل "cwchar.h"
#شامل "ucln_cmn.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "uhash.h"
#شامل "unicode/uenum.h"
#شامل "uenumimp.h"
#شامل "ulocimp.h"
#شامل "umutex.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل "uresdata.h"

استعمال نامخلا icu;

/*
Static cache for already opened resource bundles - mostly for keeping fallback info
TODO: This cache should probably be removed when the deprecated code is
      completely removed.
*/
ساکن UHashtable *cache = خالی_اشاریہ;
ساکن icu::UInitOnce gCacheInitOnce {};

ساکن UMutex resbMutex;

/* INTERNAL: hashes an entry  */
ساکن int32_t U_CALLCONV hash_اندراج(ثابت UHashTok parm) {
    UResource_ڈیٹا_اندراج* b = static_cast<UResource_ڈیٹا_اندراج*>(parm.اشارہ_گر);
    UHashTok namekey, pathkey;
    namekey.اشارہ_گر = b->fName;
    pathkey.اشارہ_گر = b->f_راستہ;
    واپس uhash_hashChars(namekey)+37u*uhash_hashChars(pathkey);
}

/* INTERNAL: compares two entries */
ساکن UBool U_CALLCONV compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UResource_ڈیٹا_اندراج* b1 = static_cast<UResource_ڈیٹا_اندراج*>(p1.اشارہ_گر);
    UResource_ڈیٹا_اندراج* b2 = static_cast<UResource_ڈیٹا_اندراج*>(p2.اشارہ_گر);
    UHashTok name1, name2, path1, path2;
    name1.اشارہ_گر = b1->fName;
    name2.اشارہ_گر = b2->fName;
    path1.اشارہ_گر = b1->f_راستہ;
    path2.اشارہ_گر = b2->f_راستہ;
    واپس uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}


/**
 *  Internal function, gets parts of locale name according 
 *  to the position of '_' character
 */
ساکن UBool chopLocale(حرف *name) {
    حرف *i = uprv_strrchr(name, '_');

    اگر(i != خالی_اشاریہ) {
        *i = '\0';
        واپس سچ;
    }

    واپس جھوٹ;
}

ساکن UBool hasVariant(ثابت حرف* localeID) {
    UError_رمز err = U_ZERO_ERROR;
    CheckedArrayByteSink sink(خالی_اشاریہ, 0);
    ulocimp_getSubtags(
            localeID,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            &sink,
            خالی_اشاریہ,
            err);
    واپس sink.نمبر_کا_بائٹیں_Appended() != 0;
}

// This file contains the tables for doing locale fallback, which are generated
// by the CLDR-to-ICU process directly from the CLDR data.  This file should only
// ever be included from here.
#تعریف_کرو INCLUDED_FROM_URESBUND_CPP
#شامل "localefallback_data.h"

ساکن ثابت حرف* performFallbackLookup(ثابت حرف* کلید,
                                         ثابت حرف* کلید_Strs,
                                         ثابت حرف* قدر_Strs U_LIFETIME_BOUND,
                                         ثابت int32_t* lookup_جدول,
                                         int32_t lookup_جدول_لمبائی) {
    ثابت int32_t* bottom = lookup_جدول;
    ثابت int32_t* چوٹی = lookup_جدول + lookup_جدول_لمبائی;

    جب (bottom < چوٹی) {
        // Effectively, divide by 2 and round down to an even index
        ثابت int32_t* درمیان = bottom + (((چوٹی - bottom) / 4) * 2);
        ثابت حرف* اندراج_کلید = &(کلید_Strs[*درمیان]);
        int32_t strcmp_نتیجہ = uprv_strcmp(کلید, اندراج_کلید);
        اگر (strcmp_نتیجہ == 0) {
            واپس &(قدر_Strs[درمیان[1]]);
        } ورنہ اگر (strcmp_نتیجہ < 0) {
            چوٹی = درمیان;
        } ورنہ {
            bottom = درمیان + 2;
        }
    }
    واپس خالی_اشاریہ;
}

ساکن CharString getDefaultScript(ثابت CharString& language, ثابت CharString& region) {
    ثابت حرف* defaultScript = خالی_اشاریہ;
    UError_رمز err = U_ZERO_ERROR;
    
    // the default script will be "Latn" if we don't find the locale ID in the tables
    CharString نتیجہ("Latn", err);
    
    // if we were passed both language and region, make them into a locale ID and look that up in the default
    // script table
    اگر (!region.is_خالی_خانے()) {
        CharString localeID;
        localeID.append(language, err).append("_", err).append(region, err);
        اگر (U_FAILURE(err)) {
            واپس نتیجہ;
        }
        defaultScript = performFallbackLookup(localeID.ڈیٹا(), dsLocaleIDChars, script_رمز_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if we didn't find anything, look up just the language in the default script table
    اگر (defaultScript == خالی_اشاریہ) {
        defaultScript = performFallbackLookup(language.ڈیٹا(), dsLocaleIDChars, script_رمز_Chars, default_Script_جدول, UPRV_LENGTHOF(default_Script_جدول));
    }
    
    // if either lookup above succeeded, copy the result from "defaultScript" into "result"; otherwise, return "Latn"
    اگر (defaultScript != خالی_اشاریہ) {
        نتیجہ.clear();
        نتیجہ.append(defaultScript, err);
    }
    واپس نتیجہ;
}

شمار UResOpenType {
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
    URES_OPEN_براہ_راست
};
قسمتعریف شمار UResOpenType UResOpenType;

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
ساکن منطقی get_والد_Locale_ID(حرف *name, ثابت حرف *origName, UResOpenType openType) {
    // early out if the locale ID has a variant code or ends with _
    size_t nameLen = uprv_strlen(name);
    اگر (!nameLen || name[nameLen - 1] == '_' || hasVariant(name)) {
        واپس chopLocale(name);
    }
    
    UError_رمز err = U_ZERO_ERROR;
    CharString language;
    CharString script;
    CharString region;
    ulocimp_getSubtags(name, &language, &script, &region, خالی_اشاریہ, خالی_اشاریہ, err);

    اگر (U_FAILURE(err)) {
        // hopefully this never happens...
        واپس chopLocale(name);
    }
    
    // if the open type is URES_OPEN_LOCALE_DEFAULT_ROOT, first look the locale ID up in the parent locale table;
    // if that table specifies a parent for it, return that  (we don't do this for the other open types-- if we're not
    // falling back through the system default locale, we also want to do straight truncation fallback instead
    // of looking things up in the parent locale table-- see https://www.unicode.org/reports/tr35/tr35.html#Parent_Locales:
    // "Collation data, however, is an exception...")
    اگر (openType == URES_OPEN_LOCALE_DEFAULT_جڑ) {
        ثابت حرف* والد_ID = performFallbackLookup(name, والد_Locale_Chars, والد_Locale_Chars, والد_Locale_جدول, UPRV_LENGTHOF(والد_Locale_جدول));
        اگر (والد_ID != خالی_اشاریہ) {
            uprv_strcpy(name, والد_ID);
            واپس سچ;
        }
    }

    CharString workingLocale;

    // if it's not in the parent locale table, figure out the fallback script algorithmically
    // (see CLDR-15265 for an explanation of the algorithm)
    اگر (!script.is_خالی_خانے() && !region.is_خالی_خانے()) {
        // if "name" has both script and region, is the script the default script?
        // - if so, remove it and keep the region
        // - if not, remove the region and keep the script
        اگر (getDefaultScript(language, region) == script) {
            workingLocale.append(language, err).append("_", err).append(region, err);
        } ورنہ {
            workingLocale.append(language, err).append("_", err).append(script, err);
        }
    } ورنہ اگر (!region.is_خالی_خانے()) {
        // if "name" has region but not script, did the original locale ID specify a script?
        // - if yes, replace the region with the script from the original locale ID
        // - if no, replace the region with the default script for that language and region
        UError_رمز err = U_ZERO_ERROR;
        CharString origNameLanguage;
        CharString origNameScript;
        ulocimp_getSubtags(origName, &origNameLanguage, &origNameScript, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, err);
        اگر (!origNameScript.is_خالی_خانے()) {
            workingLocale.append(language, err).append("_", err).append(origNameScript, err);
        } ورنہ {
            workingLocale.append(language, err).append("_", err).append(getDefaultScript(language, region), err);
        }
    } ورنہ اگر (!script.is_خالی_خانے()) {
        // if "name" has script but not region (and our open type if URES_OPEN_LOCALE_DEFAULT_ROOT), is the script
        // the default script for the language?
        // - if so, remove it from the locale ID
        // - if not, return false to continue up the chain
        // (we don't do this for other open types for the same reason we don't look things up in the parent
        // locale table for other open types-- see the reference to UTS #35 above)
        اگر (openType != URES_OPEN_LOCALE_DEFAULT_جڑ || getDefaultScript(language, CharString()) == script) {
            workingLocale.append(language, err);
        } ورنہ {
            واپس جھوٹ;
        }
    } ورنہ {
        // if "name" just contains a language code, return false so the calling code falls back to "root"
        واپس جھوٹ;
    }
    اگر (U_SUCCESS(err) && !workingLocale.is_خالی_خانے()) {
        uprv_strcpy(name, workingLocale.ڈیٹا());
        واپس سچ;
    } ورنہ {
        واپس جھوٹ;
    }
}

/**
 *  Called to check whether a name without '_' needs to be checked for a parent.
 *  Some code had assumed that locale IDs with '_' could not have a non-root parent.
 *  We may want a better way of doing this.
 */
ساکن UBool may_Have_والد(حرف *name) {
    واپس (name[0] != 0 && uprv_strstr("nb nn",name) != خالی_اشاریہ);
}

/**
 *  Internal function
 */
ساکن خالی اندراج_Increase(UResource_ڈیٹا_اندراج *اندراج_2) {
    Mutex تالا(&resbMutex);
    اندراج_2->f_گنتی_Existing++;
    جب(اندراج_2->f_والد != خالی_اشاریہ) {
      اندراج_2 = اندراج_2->f_والد;
      اندراج_2->f_گنتی_Existing++;
    }
}

/**
 *  Internal function. Tries to find a resource in given Resource
 *  Bundle, as well as in its parents
 */
ساکن UResource_ڈیٹا_اندراج *get_Fallback_ڈیٹا(
        ثابت UResourceBundle *resBundle,
        ثابت حرف **res_لیبل, Resource *res, UError_رمز *حالت_2) {
    UResource_ڈیٹا_اندراج *ڈیٹا_اندراج = resBundle->f_ڈیٹا;
    int32_t اشاریہ_R = -1;
    int32_t i = 0;
    *res = RES_BOGUS;
    اگر(ڈیٹا_اندراج == خالی_اشاریہ) {
        *حالت_2 = U_MISSING_RESOURCE_ERROR;
        واپس خالی_اشاریہ;
    }
    اگر(ڈیٹا_اندراج->fBogus == U_ZERO_ERROR) { /* if this resource is real, */
        *res = res_get_جدول_شے_By_کلید(&(ڈیٹا_اندراج->f_ڈیٹا), ڈیٹا_اندراج->f_ڈیٹا.جڑ_Res, &اشاریہ_R, res_لیبل); /* try to get data from there */
        i++;
    }
    اگر(resBundle->fHasFallback) {
        // Otherwise, we'll look in parents.
        جب(*res == RES_BOGUS && ڈیٹا_اندراج->f_والد != خالی_اشاریہ) {
            ڈیٹا_اندراج = ڈیٹا_اندراج->f_والد;
            اگر(ڈیٹا_اندراج->fBogus == U_ZERO_ERROR) {
                i++;
                *res = res_get_جدول_شے_By_کلید(&(ڈیٹا_اندراج->f_ڈیٹا), ڈیٹا_اندراج->f_ڈیٹا.جڑ_Res, &اشاریہ_R, res_لیبل);
            }
        }
    }

    اگر(*res == RES_BOGUS) {
        // If the resource is not found, we need to give an error.
        *حالت_2 = U_MISSING_RESOURCE_ERROR;
        واپس خالی_اشاریہ;
    }
    // If the resource is found in parents, we need to adjust the error.
    اگر(i>1) {
        اگر(uprv_strcmp(ڈیٹا_اندراج->fName, uloc_getDefault())==0 || uprv_strcmp(ڈیٹا_اندراج->fName, k_جڑ_Locale_Name)==0) {
            *حالت_2 = U_USING_DEFAULT_WARNING;
        } ورنہ {
            *حالت_2 = U_USING_FALLBACK_WARNING;
        }
    }
    واپس ڈیٹا_اندراج;
}

ساکن خالی
free_اندراج(UResource_ڈیٹا_اندراج *اندراج_2) {
    UResource_ڈیٹا_اندراج *alias;
    res_unload(&(اندراج_2->f_ڈیٹا));
    اگر(اندراج_2->fName != خالی_اشاریہ && اندراج_2->fName != اندراج_2->f_Name_بفر) {
        uprv_free(اندراج_2->fName);
    }
    اگر(اندراج_2->f_راستہ != خالی_اشاریہ) {
        uprv_free(اندراج_2->f_راستہ);
    }
    اگر(اندراج_2->fPool != خالی_اشاریہ) {
        --اندراج_2->fPool->f_گنتی_Existing;
    }
    alias = اندراج_2->fAlias;
    اگر(alias != خالی_اشاریہ) {
        جب(alias->fAlias != خالی_اشاریہ) {
            alias = alias->fAlias;
        }
        --alias->f_گنتی_Existing;
    }
    uprv_free(اندراج_2);
}

/* Works just like ucnv_flushCache() */
ساکن int32_t ures_flushCache()
{
    UResource_ڈیٹا_اندراج *resB;
    int32_t pos;
    int32_t rb_Deleted_عدد = 0;
    ثابت UHashElement *e;
    UBool deleted_مزید;

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    Mutex تالا(&resbMutex);
    اگر (cache == خالی_اشاریہ) {
        واپس 0;
    }

    کرو {
        deleted_مزید = جھوٹ;
        /*creates an enumeration to iterate through every element in the table */
        pos = UHASH_پہلا;
        جب ((e = uhash_اگلا_Element(cache, &pos)) != خالی_اشاریہ)
        {
            resB = static_cast<UResource_ڈیٹا_اندراج*>(e->value.اشارہ_گر);
            /* Deletes only if reference counter == 0
             * Don't worry about the children of this node.
             * Those will eventually get deleted too, if not already.
             * Don't worry about the parents of this node.
             * Those will eventually get deleted too, if not already.
             */
            /* 04/05/2002 [weiv] fCountExisting should now be accurate. If it's not zero, that means that    */
            /* some resource bundles are still open somewhere. */

            اگر (resB->f_گنتی_Existing == 0) {
                rb_Deleted_عدد++;
                deleted_مزید = سچ;
                uhash_removeElement(cache, e);
                free_اندراج(resB);
            }
        }
        /*
         * Do it again to catch bundles (aliases, pool bundle) whose fCountExisting
         * got decremented by free_entry().
         */
    } جب(deleted_مزید);

    واپس rb_Deleted_عدد;
}

#اگر_تعریف_شدہ URES_DEBUG
#شامل <stdio.h>

U_CAPI UBool U_EXPORT2 ures_dumpCacheContents() {
  UBool cache_Not_خالی_خانے = جھوٹ;
  int32_t pos = UHASH_پہلا;
  ثابت UHashElement *e;
  UResource_ڈیٹا_اندراج *resB;
  
    Mutex تالا(&resbMutex);
    اگر (cache == خالی_اشاریہ) {
      fprintf(stderr,"%s:%d: RB Cache is nullptr.\n", __FILE__, __LINE__);
      واپس جھوٹ;
    }

    جب ((e = uhash_اگلا_Element(cache, &pos)) != خالی_اشاریہ) {
      cache_Not_خالی_خانے=سچ;
      resB = (UResource_ڈیٹا_اندراج *) e->value.اشارہ_گر;
      fprintf(stderr,"%s:%d: RB Cache: Entry @0x%p, refcount %d, name %s:%s.  Pool 0x%p, alias 0x%p, parent 0x%p\n",
              __FILE__, __LINE__,
              (خالی*)resB, resB->f_گنتی_Existing,
              resB->fName?resB->fName:"nullptr",
              resB->f_راستہ?resB->f_راستہ:"nullptr",
              (خالی*)resB->fPool,
              (خالی*)resB->fAlias,
              (خالی*)resB->f_والد);       
    }
    
    fprintf(stderr,"%s:%d: RB Cache still contains %d items.\n", __FILE__, __LINE__, uhash_گنتی(cache));
    واپس cache_Not_خالی_خانے;
}

#اگر_ختم

ساکن UBool U_CALLCONV ures_cleanup()
{
    اگر (cache != خالی_اشاریہ) {
        ures_flushCache();
        uhash_close(cache);
        cache = خالی_اشاریہ;
    }
    gCacheInitOnce.reset();
    واپس سچ;
}

/** INTERNAL: Initializes the cache for resources */
ساکن خالی U_CALLCONV createCache(UError_رمز &حالت_2) {
    U_ASSERT(cache == خالی_اشاریہ);
    cache = uhash_open(hash_اندراج, compareEntries, خالی_اشاریہ, &حالت_2);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_URES, ures_cleanup);
}
     
ساکن خالی initCache(UError_رمز *حالت_2) {
    umtx_initOnce(gCacheInitOnce, &createCache, *حالت_2);
}

/** INTERNAL: sets the name (locale) of the resource bundle to given name */

ساکن خالی سیٹ_اندراج_Name(UResource_ڈیٹا_اندراج *res, ثابت حرف *name, UError_رمز *حالت_2) {
    int32_t len = static_cast<int32_t>(uprv_strlen(name));
    اگر(res->fName != خالی_اشاریہ && res->fName != res->f_Name_بفر) {
        uprv_free(res->fName);
    }
    اگر (len < static_cast<int32_t>(حجم(res->f_Name_بفر))) {
        res->fName = res->f_Name_بفر;
    }
    ورنہ {
        res->fName = static_cast<حرف*>(uprv_malloc(len + 1));
    }
    اگر(res->fName == خالی_اشاریہ) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
    } ورنہ {
        uprv_strcpy(res->fName, name);
    }
}

ساکن UResource_ڈیٹا_اندراج *
get_Pool_اندراج(ثابت حرف *راستہ, UError_رمز *حالت_2);

/**
 *  INTERNAL: Inits and opens an entry from a data DLL.
 *    CAUTION:  resbMutex must be locked when calling this function.
 */
ساکن UResource_ڈیٹا_اندراج *init_اندراج(ثابت حرف *localeID, ثابت حرف *راستہ, UError_رمز *حالت_2) {
    UResource_ڈیٹا_اندراج *r = خالی_اشاریہ;
    UResource_ڈیٹا_اندراج find;
    /*int32_t hashValue;*/
    ثابت حرف *name;
    حرف aliasName[100] = { 0 };
    int32_t aliasLen = 0;
    /*UBool isAlias = false;*/
    /*UHashTok hashkey; */

    اگر(U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    /* here we try to deduce the right locale name */
    اگر(localeID == خالی_اشاریہ) { /* if localeID is nullptr, we're trying to open default locale */
        name = uloc_getDefault();
    } ورنہ اگر(*localeID == 0) { /* if localeID is "" then we try to open root locale */
        name = k_جڑ_Locale_Name;
    } ورنہ { /* otherwise, we'll open what we're given */
        name = localeID;
    }

    find.fName = const_cast<حرف*>(name);
    find.f_راستہ = const_cast<حرف*>(راستہ);

    /* calculate the hash value of the entry */
    /*hashkey.pointer = (void *)&find;*/
    /*hashValue = hashEntry(hashkey);*/

    /* check to see if we already have this entry */
    r = static_cast<UResource_ڈیٹا_اندراج*>(uhash_get(cache, &find));
    اگر(r == خالی_اشاریہ) {
        /* if the entry is not yet in the hash table, we'll try to construct a new one */
        r = static_cast<UResource_ڈیٹا_اندراج*>(uprv_malloc(حجم(UResource_ڈیٹا_اندراج)));
        اگر(r == خالی_اشاریہ) {
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }

        uprv_memset(r, 0, حجم(UResource_ڈیٹا_اندراج));
        /*r->fHashKey = hashValue;*/

        سیٹ_اندراج_Name(r, name, حالت_2);
        اگر (U_FAILURE(*حالت_2)) {
            uprv_free(r);
            واپس خالی_اشاریہ;
        }

        اگر(راستہ != خالی_اشاریہ) {
            r->f_راستہ = uprv_strdup(راستہ);
            اگر(r->f_راستہ == خالی_اشاریہ) {
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                uprv_free(r);
                واپس خالی_اشاریہ;
            }
        }

        /* this is the actual loading */
        res_بھرو(&(r->f_ڈیٹا), r->f_راستہ, r->fName, حالت_2);

        اگر (U_FAILURE(*حالت_2)) {
            /* if we failed to load due to an out-of-memory error, exit early. */
            اگر (*حالت_2 == U_حافظہ_تخصیص_ERROR) {
                uprv_free(r);
                واپس خالی_اشاریہ;
            }
            /* we have no such entry in dll, so it will always use fallback */
            *حالت_2 = U_USING_FALLBACK_WARNING;
            r->fBogus = U_USING_FALLBACK_WARNING;
        } ورنہ { /* if we have a regular entry */
            Resource aliasres;
            اگر (r->f_ڈیٹا.استعمال_کرتا_Pool_Bundle) {
                r->fPool = get_Pool_اندراج(r->f_راستہ, حالت_2);
                اگر (U_SUCCESS(*حالت_2)) {
                    ثابت int32_t *poolIndexes = r->fPool->f_ڈیٹا.p_جڑ + 1;
                    اگر(r->f_ڈیٹا.p_جڑ[1 + URES_اشاریہ_POOL_CHECKSUM] == poolIndexes[URES_اشاریہ_POOL_CHECKSUM]) {
                        r->f_ڈیٹا.pool_Bundle_کلیدیں = reinterpret_cast<ثابت حرف*>(poolIndexes + (poolIndexes[URES_اشاریہ_لمبائی] & 0xff));
                        r->f_ڈیٹا.pool_Bundle_لڑیاں = r->fPool->f_ڈیٹا.p16_ثنائی_ہندسہ_Units;
                    } ورنہ {
                        r->fBogus = *حالت_2 = U_INVALID_FORMAT_ERROR;
                    }
                } ورنہ {
                    r->fBogus = *حالت_2;
                }
            }
            اگر (U_SUCCESS(*حالت_2)) {
                /* handle the alias by trying to get out the %%Alias tag.*/
                /* We'll try to get alias string from the bundle */
                aliasres = res_getResource(&(r->f_ڈیٹا), "%%ALIAS");
                اگر (aliasres != RES_BOGUS) {
                    // No tracing: called during initial data loading
                    ثابت char16_t *alias = res_getStringNoTrace(&(r->f_ڈیٹا), aliasres, &aliasLen);
                    اگر(alias != خالی_اشاریہ && aliasLen > 0) { /* if there is actual alias - unload and load new data */
                        u_UChars_مقصد_Chars(alias, aliasName, aliasLen+1);
                        r->fAlias = init_اندراج(aliasName, راستہ, حالت_2);
                    }
                }
            }
        }

        {
            UResource_ڈیٹا_اندراج *oldR = خالی_اشاریہ;
            اگر ((oldR = static_cast<UResource_ڈیٹا_اندراج*>(uhash_get(cache, r))) == خالی_اشاریہ) { /* if the data is not cached */
                /* just insert it in the cache */
                UError_رمز cache_حالت = U_ZERO_ERROR;
                uhash_put(cache, (خالی *)r, r, &cache_حالت);
                اگر (U_FAILURE(cache_حالت)) {
                    *حالت_2 = cache_حالت;
                    free_اندراج(r);
                    r = خالی_اشاریہ;
                }
            } ورنہ {
                /* somebody have already inserted it while we were working, discard newly opened data */
                /* Also, we could get here IF we opened an alias */
                free_اندراج(r);
                r = oldR;
            }
        }

    }
    اگر(r != خالی_اشاریہ) {
        /* return the real bundle */
        جب(r->fAlias != خالی_اشاریہ) {
            r = r->fAlias;
        }
        r->f_گنتی_Existing++; /* we increase its reference count */
        /* if the resource has a warning */
        /* we don't want to overwrite a status with no error */
        اگر(r->fBogus != U_ZERO_ERROR && U_SUCCESS(*حالت_2)) {
             *حالت_2 = r->fBogus; /* set the returning status */
        }
    }
    واپس r;
}

ساکن UResource_ڈیٹا_اندراج *
get_Pool_اندراج(ثابت حرف *راستہ, UError_رمز *حالت_2) {
    UResource_ڈیٹا_اندراج *poolBundle = init_اندراج(kPoolBundleName, راستہ, حالت_2);
    اگر( U_SUCCESS(*حالت_2) &&
        (poolBundle == خالی_اشاریہ || poolBundle->fBogus != U_ZERO_ERROR || !poolBundle->f_ڈیٹا.isPoolBundle)
    ) {
        *حالت_2 = U_INVALID_FORMAT_ERROR;
    }
    واپس poolBundle;
}

/* INTERNAL: */
/*   CAUTION:  resbMutex must be locked when calling this function! */
ساکن UResource_ڈیٹا_اندراج *
ڈھونڈو_پہلا_Existing(ثابت حرف* راستہ, حرف* name, ثابت حرف* defaultLocale, UResOpenType openType,
                  UBool *is_جڑ, UBool *ملا_والد, UBool *isDefault, UError_رمز* حالت_2) {
    UResource_ڈیٹا_اندراج *r = خالی_اشاریہ;
    UBool has_Real_ڈیٹا = جھوٹ;
    *ملا_والد = سچ; /* we're starting with a fresh name */
    حرف origName[ULOC_FULLNAME_استعداد];

    uprv_strcpy(origName, name);
    جب(*ملا_والد && !has_Real_ڈیٹا) {
        r = init_اندراج(name, راستہ, حالت_2);
        /* Null pointer test */
        اگر (U_FAILURE(*حالت_2)) {
            واپس خالی_اشاریہ;
        }
        *isDefault = static_cast<UBool>(uprv_strncmp(name, defaultLocale, uprv_strlen(name)) == 0);
        has_Real_ڈیٹا = static_cast<UBool>(r->fBogus == U_ZERO_ERROR);
        اگر(!has_Real_ڈیٹا) {
            /* this entry is not real. We will discard it. */
            /* However, the parent line for this entry is  */
            /* not to be used - as there might be parent   */
            /* lines in cache from previous openings that  */
            /* are not updated yet. */
            r->f_گنتی_Existing--;
            /*entryCloseInt(r);*/
            r = خالی_اشاریہ;
            *حالت_2 = U_USING_FALLBACK_WARNING;
        } ورنہ {
            uprv_strcpy(name, r->fName); /* this is needed for supporting aliases */
        }

        *is_جڑ = static_cast<UBool>(uprv_strcmp(name, k_جڑ_Locale_Name) == 0);

        /*Fallback data stuff*/
        اگر (!has_Real_ڈیٹا) {
            *ملا_والد = get_والد_Locale_ID(name, origName, openType);
        } ورنہ {
            // we've already found a real resource file; what we return to the caller is the parent
            // locale ID for inheritance, which should come from chopLocale(), not getParentLocaleID()
            *ملا_والد = chopLocale(name);
        }
        اگر (*ملا_والد && *name == '\0') {
            uprv_strcpy(name, "und");
        }
    }
    واپس r;
}

ساکن خالی ures_سیٹ_Is_ڈھیر_Object( UResourceBundle* resB, UBool state) {
    اگر(state) {
        resB->fMagic1 = 0;
        resB->fMagic2 = 0;
    } ورنہ {
        resB->fMagic1 = MAGIC1;
        resB->fMagic2 = MAGIC2;
    }
}

ساکن UBool ures_is_ڈھیر_Object(ثابت UResourceBundle* resB) {
  واپس((resB->fMagic1 == MAGIC1 && resB->fMagic2 == MAGIC2)?جھوٹ:سچ);
}


U_CFUNC خالی ures_init_ڈھیر_Object(UResourceBundle* resB) {
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

U_NAMESPACE_انتہا

ساکن UBool  // returns U_SUCCESS(*status)
بھرو_Parents_Except_جڑ(UResource_ڈیٹا_اندراج *&t1,
                      حرف name[], int32_t name_استعداد,
                      UBool usingUSRData, حرف usr_ڈیٹا_راستہ[], UError_رمز *حالت_2) {
    اگر (U_FAILURE(*حالت_2)) { واپس جھوٹ; }
    UBool check_والد = سچ;
    جب (check_والد && t1->f_والد == خالی_اشاریہ && !t1->f_ڈیٹا.noFallback &&
            res_getResource(&t1->f_ڈیٹا,"%%ParentIsRoot") == RES_BOGUS) {
        Resource والد_Res = res_getResource(&t1->f_ڈیٹا, "%%Parent");
        اگر (والد_Res != RES_BOGUS) {  // An explicit parent was found.
            int32_t والد_Locale_Len = 0;
            // No tracing: called during initial data loading
            ثابت char16_t *والد_Locale_Name = res_getStringNoTrace(&(t1->f_ڈیٹا), والد_Res, &والد_Locale_Len);
            اگر(والد_Locale_Name != خالی_اشاریہ && 0 < والد_Locale_Len && والد_Locale_Len < name_استعداد) {
                u_UChars_مقصد_Chars(والد_Locale_Name, name, والد_Locale_Len + 1);
                اگر (uprv_strcmp(name, k_جڑ_Locale_Name) == 0) {
                    واپس سچ;
                }
            }
        }
        // Insert regular parents.
        UError_رمز والد_حالت = U_ZERO_ERROR;
        UResource_ڈیٹا_اندراج *t2 = init_اندراج(name, t1->f_راستہ, &والد_حالت);
        اگر (U_FAILURE(والد_حالت)) {
            *حالت_2 = والد_حالت;
            واپس جھوٹ;
        }
        UResource_ڈیٹا_اندراج *u2 = خالی_اشاریہ;
        UError_رمز usr_حالت = U_ZERO_ERROR;
        اگر (usingUSRData) {  // This code inserts user override data into the inheritance chain.
            u2 = init_اندراج(name, usr_ڈیٹا_راستہ, &usr_حالت);
            // If we failed due to out-of-memory, report that to the caller and exit early.
            اگر (usr_حالت == U_حافظہ_تخصیص_ERROR) {
                *حالت_2 = usr_حالت;
                واپس جھوٹ;
            }
        }

        اگر (usingUSRData && U_SUCCESS(usr_حالت) && u2->fBogus == U_ZERO_ERROR) {
            t1->f_والد = u2;
            u2->f_والد = t2;
        } ورنہ {
            t1->f_والد = t2;
            اگر (usingUSRData) {
                // The USR override data wasn't found, set it to be deleted.
                u2->f_گنتی_Existing = 0;
            }
        }
        t1 = t2;
        check_والد = chopLocale(name) || may_Have_والد(name);
    }
    واپس سچ;
}

ساکن UBool  // returns U_SUCCESS(*status)
داخل_کرو_جڑ_Bundle(UResource_ڈیٹا_اندراج *&t1, UError_رمز *حالت_2) {
    اگر (U_FAILURE(*حالت_2)) { واپس جھوٹ; }
    UError_رمز والد_حالت = U_ZERO_ERROR;
    UResource_ڈیٹا_اندراج *t2 = init_اندراج(k_جڑ_Locale_Name, t1->f_راستہ, &والد_حالت);
    اگر (U_FAILURE(والد_حالت)) {
        *حالت_2 = والد_حالت;
        واپس جھوٹ;
    }
    t1->f_والد = t2;
    t1 = t2;
    واپس سچ;
}

ساکن UResource_ڈیٹا_اندراج *اندراج_Open(ثابت حرف* راستہ, ثابت حرف* localeID,
                                     UResOpenType openType, UError_رمز* حالت_2) {
    U_ASSERT(openType != URES_OPEN_براہ_راست);
    UError_رمز int_حالت = U_ZERO_ERROR;
    UResource_ڈیٹا_اندراج *r = خالی_اشاریہ;
    UResource_ڈیٹا_اندراج *t1 = خالی_اشاریہ;
    UBool isDefault = جھوٹ;
    UBool is_جڑ = جھوٹ;
    UBool has_Real_ڈیٹا = جھوٹ;
    UBool hasChopped = سچ;
    UBool usingUSRData = U_USE_USRDATA && ( راستہ == خالی_اشاریہ || uprv_strncmp(راستہ,U_ICUDATA_NAME,8) == 0);

    حرف name[ULOC_FULLNAME_استعداد];
    حرف usr_ڈیٹا_راستہ[96];

    initCache(حالت_2);

    اگر(U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    uprv_strncpy(name, localeID, حجم(name) - 1);
    name[حجم(name) - 1] = 0;

    اگر ( usingUSRData ) {
        اگر ( راستہ == خالی_اشاریہ ) {
            uprv_strcpy(usr_ڈیٹا_راستہ, U_USRDATA_NAME);
        } ورنہ {
            uprv_strncpy(usr_ڈیٹا_راستہ, راستہ, حجم(usr_ڈیٹا_راستہ) - 1);
            usr_ڈیٹا_راستہ[0] = 'u';
            usr_ڈیٹا_راستہ[1] = 's';
            usr_ڈیٹا_راستہ[2] = 'r';
            usr_ڈیٹا_راستہ[حجم(usr_ڈیٹا_راستہ) - 1] = 0;
        }
    }
 
    // Note: We need to query the default locale *before* locking resbMutex.
    ثابت حرف *defaultLocale = uloc_getDefault();

    Mutex تالا(&resbMutex);    // Lock resbMutex until the end of this function.

    /* We're going to skip all the locales that do not have any data */
    r = ڈھونڈو_پہلا_Existing(راستہ, name, defaultLocale, openType, &is_جڑ, &hasChopped, &isDefault, &int_حالت);

    // If we failed due to out-of-memory, report the failure and exit early.
    اگر (int_حالت == U_حافظہ_تخصیص_ERROR) {
        *حالت_2 = int_حالت;
        جاؤ اختتام;
    }

    اگر(r != خالی_اشاریہ) { /* if there is one real locale, we can look for parents. */
        t1 = r;
        has_Real_ڈیٹا = سچ;
        اگر ( usingUSRData ) {  /* This code inserts user override data into the inheritance chain */
            UError_رمز usr_حالت = U_ZERO_ERROR;
            UResource_ڈیٹا_اندراج *u1 = init_اندراج(t1->fName, usr_ڈیٹا_راستہ, &usr_حالت);
            // If we failed due to out-of-memory, report the failure and exit early.
            اگر (int_حالت == U_حافظہ_تخصیص_ERROR) {
                *حالت_2 = int_حالت;
                جاؤ اختتام;
            }
            اگر ( u1 != خالی_اشاریہ ) {
                اگر(u1->fBogus == U_ZERO_ERROR) {
                    u1->f_والد = t1;
                    r = u1;
                } ورنہ {
                    /* the USR override data wasn't found, set it to be deleted */
                    u1->f_گنتی_Existing = 0;
                }
            }
        }
        اگر ((hasChopped || may_Have_والد(name)) && !is_جڑ) {
            اگر (!بھرو_Parents_Except_جڑ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_ڈیٹا_راستہ, حالت_2)) {
                جاؤ اختتام;
            }
        }
    }

    /* we could have reached this point without having any real data */
    /* if that is the case, we need to chain in the default locale   */
    اگر(r==خالی_اشاریہ && openType == URES_OPEN_LOCALE_DEFAULT_جڑ && !isDefault && !is_جڑ) {
        /* insert default locale */
        uprv_strcpy(name, defaultLocale);
        r = ڈھونڈو_پہلا_Existing(راستہ, name, defaultLocale, openType, &is_جڑ, &hasChopped, &isDefault, &int_حالت);
        // If we failed due to out-of-memory, report the failure and exit early.
        اگر (int_حالت == U_حافظہ_تخصیص_ERROR) {
            *حالت_2 = int_حالت;
            جاؤ اختتام;
        }
        int_حالت = U_USING_DEFAULT_WARNING;
        اگر(r != خالی_اشاریہ) { /* the default locale exists */
            t1 = r;
            has_Real_ڈیٹا = سچ;
            isDefault = سچ;
            // TODO: Why not if (usingUSRData) { ... } like in the non-default-locale code path?
            اگر ((hasChopped || may_Have_والد(name)) && !is_جڑ) {
                اگر (!بھرو_Parents_Except_جڑ(t1, name, UPRV_LENGTHOF(name), usingUSRData, usr_ڈیٹا_راستہ, حالت_2)) {
                    جاؤ اختتام;
                }
            }
        }
    }

    /* we could still have r == nullptr at this point - maybe even default locale is not */
    /* present */
    اگر(r == خالی_اشاریہ) {
        uprv_strcpy(name, k_جڑ_Locale_Name);
        r = ڈھونڈو_پہلا_Existing(راستہ, name, defaultLocale, openType, &is_جڑ, &hasChopped, &isDefault, &int_حالت);
        // If we failed due to out-of-memory, report the failure and exit early.
        اگر (int_حالت == U_حافظہ_تخصیص_ERROR) {
            *حالت_2 = int_حالت;
            جاؤ اختتام;
        }
        اگر(r != خالی_اشاریہ) {
            t1 = r;
            int_حالت = U_USING_DEFAULT_WARNING;
            has_Real_ڈیٹا = سچ;
        } ورنہ { /* we don't even have the root locale */
            *حالت_2 = U_MISSING_RESOURCE_ERROR;
            جاؤ اختتام;
        }
    } ورنہ اگر(!is_جڑ && uprv_strcmp(t1->fName, k_جڑ_Locale_Name) != 0 &&
            t1->f_والد == خالی_اشاریہ && !r->f_ڈیٹا.noFallback) {
        اگر (!داخل_کرو_جڑ_Bundle(t1, حالت_2)) {
            جاؤ اختتام;
        }
        اگر(!has_Real_ڈیٹا) {
            r->fBogus = U_USING_DEFAULT_WARNING;
        }
    }

    // TODO: Does this ever loop?
    جب(r != خالی_اشاریہ && !is_جڑ && t1->f_والد != خالی_اشاریہ) {
        t1->f_والد->f_گنتی_Existing++;
        t1 = t1->f_والد;
    }

اختتام:
    اگر(U_SUCCESS(*حالت_2)) {
        اگر(int_حالت != U_ZERO_ERROR) {
            *حالت_2 = int_حالت;  
        }
        واپس r;
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}

/**
 * Version of entryOpen() and findFirstExisting() for ures_openDirect(),
 * with no fallbacks.
 * Parent and root locale bundles are loaded if
 * the requested bundle does not have the "nofallback" flag.
 */
ساکن UResource_ڈیٹا_اندراج *
اندراج_Open_براہ_راست(ثابت حرف* راستہ, ثابت حرف* localeID, UError_رمز* حالت_2) {
    initCache(حالت_2);
    اگر(U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    // Note: We need to query the default locale *before* locking resbMutex.
    // If the localeID is nullptr, then we want to use the default locale.
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    } ورنہ اگر (*localeID == 0) {
        // If the localeID is "", then we want to use the root locale.
        localeID = k_جڑ_Locale_Name;
    }

    Mutex تالا(&resbMutex);

    // findFirstExisting() without fallbacks.
    UResource_ڈیٹا_اندراج *r = init_اندراج(localeID, راستہ, حالت_2);
    اگر(U_SUCCESS(*حالت_2)) {
        اگر(r->fBogus != U_ZERO_ERROR) {
            r->f_گنتی_Existing--;
            r = خالی_اشاریہ;
        }
    } ورنہ {
        r = خالی_اشاریہ;
    }

    // Some code depends on the ures_openDirect() bundle to have a parent bundle chain,
    // unless it is marked with "nofallback".
    UResource_ڈیٹا_اندراج *t1 = r;
    اگر(r != خالی_اشاریہ && uprv_strcmp(localeID, k_جڑ_Locale_Name) != 0 &&  // not root
            r->f_والد == خالی_اشاریہ && !r->f_ڈیٹا.noFallback &&
            uprv_strlen(localeID) < ULOC_FULLNAME_استعداد) {
        حرف name[ULOC_FULLNAME_استعداد];
        uprv_strcpy(name, localeID);
        اگر(!chopLocale(name) || uprv_strcmp(name, k_جڑ_Locale_Name) == 0 ||
                بھرو_Parents_Except_جڑ(t1, name, UPRV_LENGTHOF(name), جھوٹ, خالی_اشاریہ, حالت_2)) {
            اگر(uprv_strcmp(t1->fName, k_جڑ_Locale_Name) != 0 && t1->f_والد == خالی_اشاریہ) {
                داخل_کرو_جڑ_Bundle(t1, حالت_2);
            }
        }
        اگر(U_FAILURE(*حالت_2)) {
            r = خالی_اشاریہ;
        }
    }

    اگر(r != خالی_اشاریہ) {
        // TODO: Does this ever loop?
        جب(t1->f_والد != خالی_اشاریہ) {
            t1->f_والد->f_گنتی_Existing++;
            t1 = t1->f_والد;
        }
    }
    واپس r;
}

/**
 * Functions to create and destroy resource bundles.
 *     CAUTION:  resbMutex must be locked when calling this function.
 */
/* INTERNAL: */
ساکن خالی اندراج_Close_Int(UResource_ڈیٹا_اندراج *resB) {
    UResource_ڈیٹا_اندراج *p = resB;

    جب(resB != خالی_اشاریہ) {
        p = resB->f_والد;
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

ساکن خالی اندراج_Close(UResource_ڈیٹا_اندراج *resB) {
  Mutex تالا(&resbMutex);
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
ساکن خالی ures_append_Res_راستہ(UResourceBundle *resB, ثابت حرف* مقصد_جوڑو, int32_t len_مقصد_جوڑو, UError_رمز *حالت_2) {
    int32_t res_راستہ_Len_Orig = resB->f_Res_راستہ_Len;
    اگر(resB->f_Res_راستہ == خالی_اشاریہ) {
        resB->f_Res_راستہ = resB->fResBuf;
        *(resB->f_Res_راستہ) = 0;
        resB->f_Res_راستہ_Len = 0;
    } 
    resB->f_Res_راستہ_Len += len_مقصد_جوڑو;
    اگر(RES_BUFSIZE <= resB->f_Res_راستہ_Len+1) {
        اگر(resB->f_Res_راستہ == resB->fResBuf) {
            resB->f_Res_راستہ = static_cast<حرف*>(uprv_malloc((resB->f_Res_راستہ_Len + 1) * حجم(حرف)));
            /* Check that memory was allocated correctly. */
            اگر (resB->f_Res_راستہ == خالی_اشاریہ) {
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            uprv_strcpy(resB->f_Res_راستہ, resB->fResBuf);
        } ورنہ {
            حرف* عارضی = static_cast<حرف*>(uprv_realloc(resB->f_Res_راستہ, (resB->f_Res_راستہ_Len + 1) * حجم(حرف)));
            /* Check that memory was reallocated correctly. */
            اگر (عارضی == خالی_اشاریہ) {
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            resB->f_Res_راستہ = عارضی;
        }
    }
    uprv_strcpy(resB->f_Res_راستہ + res_راستہ_Len_Orig, مقصد_جوڑو);
}

ساکن خالی ures_free_Res_راستہ(UResourceBundle *resB) {
    اگر (resB->f_Res_راستہ && resB->f_Res_راستہ != resB->fResBuf) {
        uprv_free(resB->f_Res_راستہ);
    }
    resB->f_Res_راستہ = خالی_اشاریہ;
    resB->f_Res_راستہ_Len = 0;
}

ساکن خالی
ures_closeBundle(UResourceBundle* resB, UBool freeBundleObj)
{
    اگر(resB != خالی_اشاریہ) {
        اگر(resB->f_ڈیٹا != خالی_اشاریہ) {
            اندراج_Close(resB->f_ڈیٹا);
        }
        اگر(resB->f_نسخہ != خالی_اشاریہ) {
            uprv_free(resB->f_نسخہ);
        }
        ures_free_Res_راستہ(resB);

        اگر(ures_is_ڈھیر_Object(resB) == جھوٹ && freeBundleObj) {
            uprv_free(resB);
        }
#اگر 0 /*U_DEBUG*/
        ورنہ {
            /* poison the data */
            uprv_memset(resB, -1, حجم(UResourceBundle));
        }
#اگر_ختم
    }
}

U_CAPI خالی  U_EXPORT2
ures_close(UResourceBundle* resB)
{
    ures_closeBundle(resB, سچ);
}

نامخلا {

UResourceBundle* init_resb_نتیجہ(UResource_ڈیٹا_اندراج* ڈیٹا_اندراج,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  UResource_ڈیٹا_اندراج* valid_Locale_ڈیٹا_اندراج,
                                  ثابت حرف* container_Res_راستہ,
                                  int32_t recursion_گہرائی,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_رمز* حالت_2);

// TODO: Try to refactor further, so that we output a dataEntry + Resource + (optionally) resPath,
// rather than a UResourceBundle.
// May need to entryIncrease() the resulting dataEntry.
UResourceBundle*
get_Alias_ہدف_As_Resource_Bundle(ثابت Resource_ڈیٹا& res_ڈیٹا,
                               Resource r,
                               ثابت حرف* کلید,
                               int32_t idx,
                               UResource_ڈیٹا_اندراج* valid_Locale_ڈیٹا_اندراج,
                               ثابت حرف* container_Res_راستہ,
                               int32_t recursion_گہرائی,
                               UResourceBundle* resB U_LIFETIME_BOUND,
                               UError_رمز* حالت_2) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    اگر (U_FAILURE(*حالت_2)) { واپس resB; }
    U_ASSERT(RES_GET_TYPE(r) == URES_ALIAS);
    int32_t len = 0;
    ثابت char16_t *alias = res_getAlias(&res_ڈیٹا, r, &len);
    اگر(len <= 0) {
        // bad alias
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس resB;
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
    chAlias.appendInvariantChars(alias, len, *حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    // We have an alias, now let's cut it up.
    ثابت حرف *راستہ = خالی_اشاریہ, *locale = خالی_اشاریہ, *کلید_راستہ = خالی_اشاریہ;
    اگر(chAlias[0] == RES_راستہ_SEPARATOR) {
        // There is a path included.
        حرف *ch_Alias_ڈیٹا = chAlias.ڈیٹا();
        حرف *sep = ch_Alias_ڈیٹا + 1;
        راستہ = sep;
        sep = uprv_strchr(sep, RES_راستہ_SEPARATOR);
        اگر(sep != خالی_اشاریہ) {
            *sep++ = 0;
        }
        اگر(uprv_strcmp(راستہ, "LOCALE") == 0) {
            // This is an XPath alias, starting with "/LOCALE/".
            // It contains the path to a resource which should be looked up
            // starting in the valid locale.
            // TODO: Can/should we forbid a /LOCALE alias without key path?
            //   It seems weird to alias to the same path, just starting from the valid locale.
            //   That will often yield an infinite loop.
            کلید_راستہ = sep;
            // Read from the valid locale which we already have.
            راستہ = locale = خالی_اشاریہ;
        } ورنہ {
            اگر(uprv_strcmp(راستہ, "ICUDATA") == 0) { /* want ICU data */
                راستہ = خالی_اشاریہ;
            }
            اگر (sep == خالی_اشاریہ) {
                // TODO: This ends up using the root bundle. Can/should we forbid this?
                locale = "";
            } ورنہ {
                locale = sep;
                sep = uprv_strchr(sep, RES_راستہ_SEPARATOR);
                اگر(sep != خالی_اشاریہ) {
                    *sep++ = 0;
                }
                کلید_راستہ = sep;
            }
        }
    } ورنہ {
        // No path, start with a locale.
        حرف *sep = chAlias.ڈیٹا();
        locale = sep;
        sep = uprv_strchr(sep, RES_راستہ_SEPARATOR);
        اگر(sep != خالی_اشاریہ) {
            *sep++ = 0;
        }
        کلید_راستہ = sep;
        راستہ = valid_Locale_ڈیٹا_اندراج->f_راستہ;
    }

    // Got almost everything, let's try to open.
    // First, open the bundle with real data.
    Local_UResource_Bundle_اشارہ_گر mainRes;
    UResource_ڈیٹا_اندراج *ڈیٹا_اندراج;
    اگر (locale == خالی_اشاریہ) {
        // alias = /LOCALE/keyPath
        // Read from the valid locale which we already have.
        ڈیٹا_اندراج = valid_Locale_ڈیٹا_اندراج;
    } ورنہ {
        UError_رمز int_حالت = U_ZERO_ERROR;
        // TODO: Shouldn't we use ures_open() for locale data bundles (!noFallback)?
        mainRes.adoptInstead(ures_open_براہ_راست(راستہ, locale, &int_حالت));
        اگر(U_FAILURE(int_حالت)) {
            // We failed to open the resource bundle we're aliasing to.
            *حالت_2 = int_حالت;
            واپس resB;
        }
        ڈیٹا_اندراج = mainRes->f_ڈیٹا;
    }

    ثابت حرف* عارضی = خالی_اشاریہ;
    اگر(کلید_راستہ == خالی_اشاریہ) {
        // No key path. This means that we are going to to use the corresponding resource from
        // another bundle.
        // TODO: Why the special code path?
        //   Why not put together a key path from containerResPath + key or idx,
        //   as a comment below suggests, and go into the regular code branch?
        // First, we are going to get a corresponding container
        // resource to the one we are searching.
        r = ڈیٹا_اندراج->f_ڈیٹا.جڑ_Res;
        اگر(container_Res_راستہ) {
            chAlias.clear().append(container_Res_راستہ, *حالت_2);
            اگر (U_FAILURE(*حالت_2)) {
                واپس خالی_اشاریہ;
            }
            حرف *a_کلید = chAlias.ڈیٹا();
            // TODO: should res_findResource() return a new dataEntry, too?
            r = res_ڈھونڈو_Resource(&ڈیٹا_اندراج->f_ڈیٹا, r, &a_کلید, &عارضی);
        }
        اگر(کلید) {
            // We need to make keyPath from the containerResPath and
            // current key, if there is a key associated.
            chAlias.clear().append(کلید, *حالت_2);
            اگر (U_FAILURE(*حالت_2)) {
                واپس خالی_اشاریہ;
            }
            حرف *a_کلید = chAlias.ڈیٹا();
            r = res_ڈھونڈو_Resource(&ڈیٹا_اندراج->f_ڈیٹا, r, &a_کلید, &عارضی);
        } ورنہ اگر(idx != -1) {
            // If there is no key, but there is an index, try to get by the index.
            // Here we have either a table or an array, so get the element.
            int32_t type = RES_GET_TYPE(r);
            اگر(URES_IS_جدول(type)) {
                ثابت حرف *a_کلید;
                r = res_get_جدول_شے_By_اشاریہ(&ڈیٹا_اندراج->f_ڈیٹا, r, idx, &a_کلید);
            } ورنہ { /* array */
                r = res_get_Array_شے(&ڈیٹا_اندراج->f_ڈیٹا, r, idx);
            }
        }
        اگر(r != RES_BOGUS) {
            resB = init_resb_نتیجہ(
                ڈیٹا_اندراج, r, عارضی, -1, valid_Locale_ڈیٹا_اندراج, خالی_اشاریہ, recursion_گہرائی+1,
                resB, حالت_2);
        } ورنہ {
            *حالت_2 = U_MISSING_RESOURCE_ERROR;
        }
    } ورنہ {
        // This one is a bit trickier.
        // We start finding keys, but after we resolve one alias, the path might continue.
        // Consider:
        //     aliastest:alias { "testtypes/anotheralias/Sequence" }
        //     anotheralias:alias { "/ICUDATA/sh/CollationElements" }
        // aliastest resource should finally have the sequence, not collation elements.
        CharString راستہ_Buf(کلید_راستہ, *حالت_2);
        اگر (U_FAILURE(*حالت_2)) {
            واپس خالی_اشاریہ;
        }
        حرف *my_راستہ = راستہ_Buf.ڈیٹا();
        container_Res_راستہ = خالی_اشاریہ;
        // Now we have fallback following here.
        برائے(;;) {
            r = ڈیٹا_اندراج->f_ڈیٹا.جڑ_Res;
            // TODO: Move  containerResPath = nullptr  to here,
            // consistent with restarting from the rootRes of another bundle?!

            // This loop handles 'found' resources over several levels.
            جب(*my_راستہ && U_SUCCESS(*حالت_2)) {
                r = res_ڈھونڈو_Resource(&(ڈیٹا_اندراج->f_ڈیٹا), r, &my_راستہ, &عارضی);
                اگر(r == RES_BOGUS) {
                    // No resource found, we don't really want to look anymore on this level.
                    توڑو;
                }
                // Found a resource, but it might be an indirection.
                resB = init_resb_نتیجہ(
                    ڈیٹا_اندراج, r, عارضی, -1,
                    valid_Locale_ڈیٹا_اندراج, container_Res_راستہ, recursion_گہرائی+1,
                    resB, حالت_2);
                اگر (U_FAILURE(*حالت_2)) {
                    توڑو;
                }
                اگر (عارضی == خالی_اشاریہ || uprv_strcmp(کلید_راستہ, عارضی) != 0) {
                    // The call to init_resb_result() above will set resB->fKeyPath to be
                    // the same as resB->fKey,
                    // throwing away any additional path elements if we had them --
                    // if the key path wasn't just a single resource ID, clear out
                    // the bundle's key path and re-set it to be equal to keyPath.
                    ures_free_Res_راستہ(resB);
                    ures_append_Res_راستہ(resB, کلید_راستہ, static_cast<int32_t>(uprv_strlen(کلید_راستہ)), حالت_2);
                    اگر(resB->f_Res_راستہ[resB->f_Res_راستہ_Len-1] != RES_راستہ_SEPARATOR) {
                        ures_append_Res_راستہ(resB, RES_راستہ_SEPARATOR_S, 1, حالت_2);
                    }
                    اگر (U_FAILURE(*حالت_2)) {
                        توڑو;
                    }
                }
                r = resB->fRes; /* switch to a new resource, possibly a new tree */
                ڈیٹا_اندراج = resB->f_ڈیٹا;
                container_Res_راستہ = resB->f_Res_راستہ;
            }
            اگر (U_FAILURE(*حالت_2) || r != RES_BOGUS) {
                توڑو;
            }
            // Fall back to the parent bundle, if there is one.
            ڈیٹا_اندراج = ڈیٹا_اندراج->f_والد;
            اگر (ڈیٹا_اندراج == خالی_اشاریہ) {
                *حالت_2 = U_MISSING_RESOURCE_ERROR;
                توڑو;
            }
            // Copy the same keyPath again.
            my_راستہ = راستہ_Buf.ڈیٹا();
            uprv_strcpy(my_راستہ, کلید_راستہ);
        }
    }
    اگر(mainRes.getAlias() == resB) {
        mainRes.orphan();
    }
    ResourceTracer(resB).maybeTrace("getalias");
    واپس resB;
}

// Recursive function, should be called only by itself, by its simpler wrapper,
// or by getAliasTargetAsResourceBundle().
UResourceBundle* init_resb_نتیجہ(UResource_ڈیٹا_اندراج* ڈیٹا_اندراج,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  UResource_ڈیٹا_اندراج* valid_Locale_ڈیٹا_اندراج,
                                  ثابت حرف* container_Res_راستہ,
                                  int32_t recursion_گہرائی,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_رمز* حالت_2) {
    // TODO: When an error occurs: Should we return nullptr vs. resB?
    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس resB;
    }
    اگر (valid_Locale_ڈیٹا_اندراج == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    اگر(RES_GET_TYPE(r) == URES_ALIAS) {
        // This is an alias, need to exchange with real data.
        اگر(recursion_گہرائی >= URES_MAX_ALIAS_LEVEL) {
            *حالت_2 = U_TOO_MANY_ALIASES_ERROR;
            واپس resB;
        }
        واپس get_Alias_ہدف_As_Resource_Bundle(
            ڈیٹا_اندراج->f_ڈیٹا, r, کلید, idx,
            valid_Locale_ڈیٹا_اندراج, container_Res_راستہ, recursion_گہرائی, resB, حالت_2);
    }
    اگر(resB == خالی_اشاریہ) {
        resB = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        اگر (resB == خالی_اشاریہ) {
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }
        ures_سیٹ_Is_ڈھیر_Object(resB, جھوٹ);
        resB->f_Res_راستہ = خالی_اشاریہ;
        resB->f_Res_راستہ_Len = 0;
    } ورنہ {
        اگر(resB->f_ڈیٹا != خالی_اشاریہ) {
            اندراج_Close(resB->f_ڈیٹا);
        }
        اگر(resB->f_نسخہ != خالی_اشاریہ) {
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
        اگر(container_Res_راستہ != resB->f_Res_راستہ) {
            ures_free_Res_راستہ(resB);
        }
    }
    resB->f_ڈیٹا = ڈیٹا_اندراج;
    اندراج_Increase(resB->f_ڈیٹا);
    resB->fHasFallback = جھوٹ;
    resB->f_Is_چوٹی_Level = جھوٹ;
    resB->f_اشاریہ = -1;
    resB->f_کلید = کلید; 
    resB->f_Valid_Locale_ڈیٹا_اندراج = valid_Locale_ڈیٹا_اندراج;
    اگر(container_Res_راستہ != resB->f_Res_راستہ) {
        ures_append_Res_راستہ(
            resB, container_Res_راستہ, static_cast<int32_t>(uprv_strlen(container_Res_راستہ)), حالت_2);
    }
    اگر(کلید != خالی_اشاریہ) {
        ures_append_Res_راستہ(resB, کلید, static_cast<int32_t>(uprv_strlen(کلید)), حالت_2);
        اگر(resB->f_Res_راستہ_Len > 0 && resB->f_Res_راستہ[resB->f_Res_راستہ_Len-1] != RES_راستہ_SEPARATOR) {
            ures_append_Res_راستہ(resB, RES_راستہ_SEPARATOR_S, 1, حالت_2);
        }
    } ورنہ اگر(idx >= 0) {
        حرف buf[256];
        int32_t len = T_CString_integer_مقصد_String(buf, idx, 10);
        ures_append_Res_راستہ(resB, buf, len, حالت_2);
        اگر(resB->f_Res_راستہ_Len > 0 && resB->f_Res_راستہ[resB->f_Res_راستہ_Len-1] != RES_راستہ_SEPARATOR) {
            ures_append_Res_راستہ(resB, RES_راستہ_SEPARATOR_S, 1, حالت_2);
        }
    }
    /* Make sure that Purify doesn't complain about uninitialized memory copies. */
    {
        int32_t مستعمل_Len = ((resB->fResBuf == resB->f_Res_راستہ) ? resB->f_Res_راستہ_Len : 0);
        uprv_memset(resB->fResBuf + مستعمل_Len, 0, حجم(resB->fResBuf) - مستعمل_Len);
    }

    resB->f_نسخہ = خالی_اشاریہ;
    resB->fRes = r;
    resB->f_سائز = res_گنتی_Array_اشیاء(&resB->get_Res_ڈیٹا(), resB->fRes);
    ResourceTracer(resB).trace("get");
    واپس resB;
}

UResourceBundle* init_resb_نتیجہ(UResource_ڈیٹا_اندراج* ڈیٹا_اندراج,
                                  Resource r,
                                  ثابت حرف* کلید,
                                  int32_t idx,
                                  // validLocaleDataEntry + containerResPath
                                  ثابت UResourceBundle* container,
                                  UResourceBundle* resB U_LIFETIME_BOUND,
                                  UError_رمز* حالت_2) {
    واپس init_resb_نتیجہ(
        ڈیٹا_اندراج, r, کلید, idx,
        container->f_Valid_Locale_ڈیٹا_اندراج, container->f_Res_راستہ, 0, resB, حالت_2);
}

}  // namespace

UResourceBundle* ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
                               ثابت UResourceBundle* original,
                               UError_رمز* حالت_2) {
    UBool is_ڈھیر_Object;
    اگر(U_FAILURE(*حالت_2) || r == original) {
        واپس r;
    }
    اگر(original != خالی_اشاریہ) {
        اگر(r == خالی_اشاریہ) {
            is_ڈھیر_Object = جھوٹ;
            r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
            /* test for nullptr */
            اگر (r == خالی_اشاریہ) {
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس خالی_اشاریہ;
            }
        } ورنہ {
            is_ڈھیر_Object = ures_is_ڈھیر_Object(r);
            ures_closeBundle(r, جھوٹ);
        }
        uprv_memcpy(r, original, حجم(UResourceBundle));
        r->f_Res_راستہ = خالی_اشاریہ;
        r->f_Res_راستہ_Len = 0;
        اگر(original->f_Res_راستہ) {
            ures_append_Res_راستہ(r, original->f_Res_راستہ, original->f_Res_راستہ_Len, حالت_2);
        }
        ures_سیٹ_Is_ڈھیر_Object(r, is_ڈھیر_Object);
        اگر(r->f_ڈیٹا != خالی_اشاریہ) {
            اندراج_Increase(r->f_ڈیٹا);
        }
    }
    واپس r;
}

/**
 * Functions to retrieve data from resource bundles.
 */

U_CAPI ثابت char16_t* U_EXPORT2 ures_getString(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                                int32_t* len,
                                                UError_رمز* حالت_2) {
    ثابت char16_t *s;
    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر(resB == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    s = res_getString({resB}, &resB->get_Res_ڈیٹا(), resB->fRes, len);
    اگر (s == خالی_اشاریہ) {
        *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس s;
}

ساکن ثابت حرف*
ures_مقصد_UTF8_String(ثابت char16_t* s16,
                  int32_t length16,
                  حرف* dest U_LIFETIME_BOUND,
                  int32_t* p_لمبائی,
                  UBool forceCopy,
                  UError_رمز* حالت_2) {
    int32_t capacity;

    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (p_لمبائی != خالی_اشاریہ) {
        capacity = *p_لمبائی;
    } ورنہ {
        capacity = 0;
    }
    اگر (capacity < 0 || (capacity > 0 && dest == خالی_اشاریہ)) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر (length16 == 0) {
        /* empty string, return as read-only pointer */
        اگر (p_لمبائی != خالی_اشاریہ) {
            *p_لمبائی = 0;
        }
        اگر (forceCopy) {
            u_terminateChars(dest, capacity, 0, حالت_2);
            واپس dest;
        } ورنہ {
            واپس "";
        }
    } ورنہ {
        /* We need to transform the string to the destination buffer. */
        اگر (capacity < length16) {
            /* No chance for the string to fit. Pure preflighting. */
            واپس u_str_مقصد_UTF8(خالی_اشاریہ, 0, p_لمبائی, s16, length16, حالت_2);
        }
        اگر (!forceCopy && (length16 <= 0x2aaaaaaa)) {
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
            int32_t max_لمبائی_2 = 3 * length16 + 1;
            اگر (capacity > max_لمبائی_2) {
                dest += capacity - max_لمبائی_2;
                capacity = max_لمبائی_2;
            }
        }
        واپس u_str_مقصد_UTF8(dest, capacity, p_لمبائی, s16, length16, حالت_2);
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getUTF8String(ثابت UResourceBundle* resB,
                   حرف* dest U_LIFETIME_BOUND,
                   int32_t* p_لمبائی,
                   UBool forceCopy,
                   UError_رمز* حالت_2) {
    int32_t length16;
    ثابت char16_t *s16 = ures_getString(resB, &length16, حالت_2);
    واپس ures_مقصد_UTF8_String(s16, length16, dest, p_لمبائی, forceCopy, حالت_2);
}

U_CAPI ثابت uint8_t* U_EXPORT2
ures_get_ثنائی(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_رمز* حالت_2) {
  ثابت uint8_t *p;
  اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  اگر(resB == خالی_اشاریہ) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس خالی_اشاریہ;
  }
  p = res_get_ثنائی({resB}, &resB->get_Res_ڈیٹا(), resB->fRes, len);
  اگر (p == خالی_اشاریہ) {
    *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
  }
  واپس p;
}

U_CAPI ثابت int32_t* U_EXPORT2
ures_get_Int_سمتیہ(ثابت UResourceBundle* resB U_LIFETIME_BOUND, int32_t* len, UError_رمز* حالت_2) {
  ثابت int32_t *p;
  اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  اگر(resB == خالی_اشاریہ) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس خالی_اشاریہ;
  }
  p = res_get_Int_سمتیہ({resB}, &resB->get_Res_ڈیٹا(), resB->fRes, len);
  اگر (p == خالی_اشاریہ) {
    *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
  }
  واپس p;
}

/* this function returns a signed integer */ 
/* it performs sign extension */
U_CAPI int32_t U_EXPORT2 ures_getInt(ثابت UResourceBundle* resB, UError_رمز *حالت_2) {
  اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
    واپس 0xffffffff;
  }
  اگر(resB == خالی_اشاریہ) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس 0xffffffff;
  }
  اگر(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
    واپس 0xffffffff;
  }
  واپس res_getInt({resB}, resB->fRes);
}

U_CAPI uint32_t U_EXPORT2 ures_getUInt(ثابت UResourceBundle* resB, UError_رمز *حالت_2) {
  اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
    واپس 0xffffffff;
  }
  اگر(resB == خالی_اشاریہ) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس 0xffffffff;
  }
  اگر(RES_GET_TYPE(resB->fRes) != URES_INT) {
    *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
    واپس 0xffffffff;
  }
  واپس res_getUInt({resB}, resB->fRes);
}

U_CAPI UResType U_EXPORT2 ures_getType(ثابت UResourceBundle *resB) {
  اگر(resB == خالی_اشاریہ) {
    واپس URES_NONE;
  }
  واپس res_get_عوامی_Type(resB->fRes);
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
  اگر(resB == خالی_اشاریہ) {
    واپس خالی_اشاریہ;
  }
  واپس(resB->f_کلید);
}

U_CAPI int32_t U_EXPORT2 ures_get_سائز(ثابت UResourceBundle *resB) {
  اگر(resB == خالی_اشاریہ) {
    واپس 0;
  }
  
  واپس resB->f_سائز;
}

ساکن ثابت char16_t*
ures_getStringWithAlias(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                        Resource r,
                        int32_t s_اشاریہ,
                        int32_t* len,
                        UError_رمز* حالت_2) {
  اگر(RES_GET_TYPE(r) == URES_ALIAS) {
    ثابت char16_t* نتیجہ = خالی_اشاریہ;
    UResourceBundle *عارضی_Res = ures_get_By_اشاریہ(resB, s_اشاریہ, خالی_اشاریہ, حالت_2);
    نتیجہ = ures_getString(عارضی_Res, len, حالت_2);
    ures_close(عارضی_Res);
    واپس نتیجہ;
  } ورنہ {
    واپس res_getString({resB, s_اشاریہ}, &resB->get_Res_ڈیٹا(), r, len); 
  }
}

U_CAPI خالی U_EXPORT2 ures_resetIterator(UResourceBundle *resB){
  اگر(resB == خالی_اشاریہ) {
    واپس;
  }
  resB->f_اشاریہ = -1;
}

U_CAPI UBool U_EXPORT2 ures_has_اگلا(ثابت UResourceBundle *resB) {
  اگر(resB == خالی_اشاریہ) {
    واپس جھوٹ;
  }
  واپس resB->f_اشاریہ < resB->f_سائز-1;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_اگلا_String(UResourceBundle* resB U_LIFETIME_BOUND,
                   int32_t* len,
                   ثابت حرف** کلید,
                   UError_رمز* حالت_2) {
  Resource r = RES_BOGUS;
  
  اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  اگر(resB == خالی_اشاریہ) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس خالی_اشاریہ;
  }
  
  اگر(resB->f_اشاریہ == resB->f_سائز-1) {
    *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
  } ورنہ {
    resB->f_اشاریہ++;
    انتخاب(RES_GET_TYPE(resB->fRes)) {
    حالت URES_STRING:
    حالت URES_STRING_V2:
      واپس res_getString({resB}, &resB->get_Res_ڈیٹا(), resB->fRes, len);
    حالت URES_جدول:
    حالت URES_TABLE16:
    حالت URES_TABLE32:
      r = res_get_جدول_شے_By_اشاریہ(&resB->get_Res_ڈیٹا(), resB->fRes, resB->f_اشاریہ, کلید);
      اگر(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      واپس ures_getStringWithAlias(resB, r, resB->f_اشاریہ, len, حالت_2);
    حالت URES_ARRAY:
    حالت URES_ARRAY16:
      r = res_get_Array_شے(&resB->get_Res_ڈیٹا(), resB->fRes, resB->f_اشاریہ);
      اگر(r == RES_BOGUS && resB->fHasFallback) {
        /* TODO: do the fallback */
      }
      واپس ures_getStringWithAlias(resB, r, resB->f_اشاریہ, len, حالت_2);
    حالت URES_ALIAS:
      واپس ures_getStringWithAlias(resB, resB->fRes, resB->f_اشاریہ, len, حالت_2);
    حالت URES_INT:
    حالت URES_ثنائی:
    حالت URES_INT_سمتیہ:
        *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
        U_FALLTHROUGH;
    طےشدہ:
      واپس خالی_اشاریہ;
    }
  }

  واپس خالی_اشاریہ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_اگلا_Resource(UResourceBundle* resB,
                     UResourceBundle* fill_اندر U_LIFETIME_BOUND,
                     UError_رمز* حالت_2) {
    ثابت حرف *کلید = خالی_اشاریہ;
    Resource r = RES_BOGUS;

    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
            /*return nullptr;*/
            واپس fill_اندر;
    }
    اگر(resB == خالی_اشاریہ) {
            *حالت_2 = U_ILLEGAL_دلیل_ERROR;
            /*return nullptr;*/
            واپس fill_اندر;
    }

    اگر(resB->f_اشاریہ == resB->f_سائز-1) {
      *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
      /*return nullptr;*/
    } ورنہ {
        resB->f_اشاریہ++;
        انتخاب(RES_GET_TYPE(resB->fRes)) {
        حالت URES_INT:
        حالت URES_ثنائی:
        حالت URES_STRING:
        حالت URES_STRING_V2:
        حالت URES_INT_سمتیہ:
            واپس ures_copyResb(fill_اندر, resB, حالت_2);
        حالت URES_جدول:
        حالت URES_TABLE16:
        حالت URES_TABLE32:
            r = res_get_جدول_شے_By_اشاریہ(&resB->get_Res_ڈیٹا(), resB->fRes, resB->f_اشاریہ, &کلید);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            واپس init_resb_نتیجہ(resB->f_ڈیٹا, r, کلید, resB->f_اشاریہ, resB, fill_اندر, حالت_2);
        حالت URES_ARRAY:
        حالت URES_ARRAY16:
            r = res_get_Array_شے(&resB->get_Res_ڈیٹا(), resB->fRes, resB->f_اشاریہ);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            واپس init_resb_نتیجہ(resB->f_ڈیٹا, r, کلید, resB->f_اشاریہ, resB, fill_اندر, حالت_2);
        طےشدہ:
            /*return nullptr;*/
            واپس fill_اندر;
        }
    }
    /*return nullptr;*/
    واپس fill_اندر;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_اشاریہ(ثابت UResourceBundle* resB,
                int32_t اشاریہ_R,
                UResourceBundle* fill_اندر U_LIFETIME_BOUND,
                UError_رمز* حالت_2) {
    ثابت حرف* کلید = خالی_اشاریہ;
    Resource r = RES_BOGUS;

    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        /*return nullptr;*/
        واپس fill_اندر;
    }
    اگر(resB == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        /*return nullptr;*/
        واپس fill_اندر;
    }

    اگر(اشاریہ_R >= 0 && resB->f_سائز > اشاریہ_R) {
        انتخاب(RES_GET_TYPE(resB->fRes)) {
        حالت URES_INT:
        حالت URES_ثنائی:
        حالت URES_STRING:
        حالت URES_STRING_V2:
        حالت URES_INT_سمتیہ:
            واپس ures_copyResb(fill_اندر, resB, حالت_2);
        حالت URES_جدول:
        حالت URES_TABLE16:
        حالت URES_TABLE32:
            r = res_get_جدول_شے_By_اشاریہ(&resB->get_Res_ڈیٹا(), resB->fRes, اشاریہ_R, &کلید);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            واپس init_resb_نتیجہ(resB->f_ڈیٹا, r, کلید, اشاریہ_R, resB, fill_اندر, حالت_2);
        حالت URES_ARRAY:
        حالت URES_ARRAY16:
            r = res_get_Array_شے(&resB->get_Res_ڈیٹا(), resB->fRes, اشاریہ_R);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            واپس init_resb_نتیجہ(resB->f_ڈیٹا, r, کلید, اشاریہ_R, resB, fill_اندر, حالت_2);
        طےشدہ:
            /*return nullptr;*/
            واپس fill_اندر;
        }
    } ورنہ {
        *حالت_2 = U_MISSING_RESOURCE_ERROR;
    }
    /*return nullptr;*/
    واپس fill_اندر;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_اشاریہ(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                      int32_t اشاریہ_S,
                      int32_t* len,
                      UError_رمز* حالت_2) {
    ثابت حرف* کلید = خالی_اشاریہ;
    Resource r = RES_BOGUS;

    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر(resB == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(اشاریہ_S >= 0 && resB->f_سائز > اشاریہ_S) {
        انتخاب(RES_GET_TYPE(resB->fRes)) {
        حالت URES_STRING:
        حالت URES_STRING_V2:
            واپس res_getString({resB}, &resB->get_Res_ڈیٹا(), resB->fRes, len);
        حالت URES_جدول:
        حالت URES_TABLE16:
        حالت URES_TABLE32:
            r = res_get_جدول_شے_By_اشاریہ(&resB->get_Res_ڈیٹا(), resB->fRes, اشاریہ_S, &کلید);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            واپس ures_getStringWithAlias(resB, r, اشاریہ_S, len, حالت_2);
        حالت URES_ARRAY:
        حالت URES_ARRAY16:
            r = res_get_Array_شے(&resB->get_Res_ڈیٹا(), resB->fRes, اشاریہ_S);
            اگر(r == RES_BOGUS && resB->fHasFallback) {
                /* TODO: do the fallback */
            }
            واپس ures_getStringWithAlias(resB, r, اشاریہ_S, len, حالت_2);
        حالت URES_ALIAS:
            واپس ures_getStringWithAlias(resB, resB->fRes, اشاریہ_S, len, حالت_2);
        حالت URES_INT:
        حالت URES_ثنائی:
        حالت URES_INT_سمتیہ:
            *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
            توڑو;
        طےشدہ:
          /* must not occur */
          *حالت_2 = U_اندرونی_پروگرام_ERROR;
          توڑو;
        }
    } ورنہ {
        *حالت_2 = U_MISSING_RESOURCE_ERROR;
    }
    واپس خالی_اشاریہ;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_اشاریہ(ثابت UResourceBundle* resB,
                          int32_t idx,
                          حرف* dest U_LIFETIME_BOUND,
                          int32_t* p_لمبائی,
                          UBool forceCopy,
                          UError_رمز* حالت_2) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_اشاریہ(resB, idx, &length16, حالت_2);
    واپس ures_مقصد_UTF8_String(s16, length16, dest, p_لمبائی, forceCopy, حالت_2);
}

/*U_CAPI const char *ures_getResPath(UResourceBundle *resB) {
  return resB->fResPath;
}*/

U_CAPI UResourceBundle* U_EXPORT2
ures_ڈھونڈو_Resource(ثابت حرف* راستہ, UResourceBundle* fill_اندر U_LIFETIME_BOUND, UError_رمز* حالت_2)
{
  UResourceBundle *first = خالی_اشاریہ; 
  UResourceBundle *نتیجہ = fill_اندر;
  حرف *packageName = خالی_اشاریہ;
  حرف *راستہ_مقصد_Resource = خالی_اشاریہ, *save = خالی_اشاریہ;
  حرف *locale = خالی_اشاریہ, *locale_انتہا = خالی_اشاریہ;
  int32_t length;

  اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)) {
    واپس نتیجہ;
  }

  length = (int32_t)(uprv_strlen(راستہ)+1);
  save = راستہ_مقصد_Resource = (حرف *)uprv_malloc(length*حجم(حرف));
  /* test for nullptr */
  اگر(راستہ_مقصد_Resource == خالی_اشاریہ) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس نتیجہ;
  }
  uprv_memcpy(راستہ_مقصد_Resource, راستہ, length);

  locale = راستہ_مقصد_Resource;
  اگر(*راستہ_مقصد_Resource == RES_راستہ_SEPARATOR) { /* there is a path specification */
    راستہ_مقصد_Resource++;
    packageName = راستہ_مقصد_Resource;
    راستہ_مقصد_Resource = uprv_strchr(راستہ_مقصد_Resource, RES_راستہ_SEPARATOR);
    اگر(راستہ_مقصد_Resource == خالی_اشاریہ) {
      *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    } ورنہ {
      *راستہ_مقصد_Resource = 0;
      locale = راستہ_مقصد_Resource+1;
    }
  }

  locale_انتہا = uprv_strchr(locale, RES_راستہ_SEPARATOR);
  اگر(locale_انتہا != خالی_اشاریہ) {
    *locale_انتہا = 0;
  }

  first = ures_open(packageName, locale, حالت_2);

  اگر(U_SUCCESS(*حالت_2)) {
    اگر(locale_انتہا) {
      نتیجہ = ures_ڈھونڈو_Sub_Resource(first, locale_انتہا+1, fill_اندر, حالت_2);
    } ورنہ {
      نتیجہ = ures_copyResb(fill_اندر, first, حالت_2);
    }
    ures_close(first);
  }
  uprv_free(save);
  واپس نتیجہ;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_ڈھونڈو_Sub_Resource(ثابت UResourceBundle* resB,
                     حرف* راستہ,
                     UResourceBundle* fill_اندر U_LIFETIME_BOUND,
                     UError_رمز* حالت_2)
{
  Resource res = RES_BOGUS;
  UResourceBundle *نتیجہ = fill_اندر;
  ثابت حرف *کلید;

  اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)) {
    واپس نتیجہ;
  }

  /* here we do looping and circular alias checking */
  /* this loop is here because aliasing is resolved on this level, not on res level */
  /* so, when we encounter an alias, it is not an aggregate resource, so we return */
  کرو {
    res = res_ڈھونڈو_Resource(&resB->get_Res_ڈیٹا(), resB->fRes, &راستہ, &کلید); 
    اگر(res != RES_BOGUS) {
        نتیجہ = init_resb_نتیجہ(resB->f_ڈیٹا, res, کلید, -1, resB, fill_اندر, حالت_2);
        resB = نتیجہ;
    } ورنہ {
        *حالت_2 = U_MISSING_RESOURCE_ERROR;
        توڑو;
    }
  } جب(*راستہ); /* there is more stuff in the path */

  واپس نتیجہ;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلید_With_Fallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت حرف* اندر_کلید,
                                int32_t* len,
                                UError_رمز* حالت_2) {
    UResourceBundle ڈھیر_5;
    ثابت char16_t* retVal = خالی_اشاریہ;
    ures_init_ڈھیر_Object(&ڈھیر_5);
    ures_get_By_کلید_With_Fallback(resB, اندر_کلید, &ڈھیر_5, حالت_2);
    int32_t length;
    retVal = ures_getString(&ڈھیر_5, &length, حالت_2);
    ures_close(&ڈھیر_5);
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (length == 3 && retVal[0] == خالی_خانے_سیٹ && retVal[1] == خالی_خانے_سیٹ && retVal[2] == خالی_خانے_سیٹ ) {
        retVal = خالی_اشاریہ;
        length = 0;
        *حالت_2 = U_MISSING_RESOURCE_ERROR;
    }
    اگر (len != خالی_اشاریہ) {
        *len = length;
    }
    واپس retVal;
}

/*
  Like res_getTableItemByKey but accepts full paths like "NumberElements/latn/patternsShort".
*/  
ساکن Resource get_جدول_شے_By_کلید_راستہ(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource جدول, ثابت حرف *کلید) {
  Resource resource = جدول;  /* The current resource */
  icu::CharString راستہ;
  UError_رمز error_رمز = U_ZERO_ERROR;
  راستہ.append(کلید, error_رمز);
  اگر (U_FAILURE(error_رمز)) { واپس RES_BOGUS; }
  حرف *راستہ_Part = راستہ.ڈیٹا();  /* Path from current resource to desired resource */
  UResType type = static_cast<UResType>(RES_GET_TYPE(resource)); /* the current resource type */
  جب (*راستہ_Part && resource != RES_BOGUS && URES_IS_CONTAINER(type)) {
    حرف *اگلا_راستہ_Part = uprv_strchr(راستہ_Part, RES_راستہ_SEPARATOR);
    اگر (اگلا_راستہ_Part != خالی_اشاریہ) {
      *اگلا_راستہ_Part = 0;  /* Terminating null for this part of path. */
      اگلا_راستہ_Part++;
    } ورنہ {
      اگلا_راستہ_Part = uprv_strchr(راستہ_Part, 0);
    }
    int32_t t;
    ثابت حرف *راستہ_P = راستہ_Part;
    resource = res_get_جدول_شے_By_کلید(p_Res_ڈیٹا, resource, &t, &راستہ_P);
    type = static_cast<UResType>(RES_GET_TYPE(resource));
    راستہ_Part = اگلا_راستہ_Part; 
  }
  اگر (*راستہ_Part) {
    واپس RES_BOGUS;
  }
  واپس resource;
}

ساکن خالی create_راستہ(ثابت حرف* orig_Res_راستہ,
                       int32_t     orig_Res_راستہ_Len,
                       ثابت حرف* res_راستہ,
                       int32_t     res_راستہ_Len,
                       ثابت حرف* اندر_کلید,
                       CharString& راستہ,
                       UError_رمز* حالت_2) {
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
    راستہ.clear();
    ثابت حرف* کلید = اندر_کلید;
    اگر (res_راستہ_Len > 0) {
        راستہ.append(res_راستہ, res_راستہ_Len, *حالت_2);
        اگر (U_SUCCESS(*حالت_2)) {
            ثابت حرف* res_راستہ_حد_بندی = res_راستہ + res_راستہ_Len;
            ثابت حرف* orig_Res_راستہ_حد_بندی = orig_Res_راستہ + orig_Res_راستہ_Len;
            ثابت حرف* res_راستہ_Ptr = res_راستہ;
            ثابت حرف* orig_Res_راستہ_Ptr = orig_Res_راستہ;
            
            // Remove from the beginning of resPath the number of segments that are contained in origResPath.
            // If origResPath has MORE segments than resPath, this will leave resPath as the empty string.
            جب (orig_Res_راستہ_Ptr < orig_Res_راستہ_حد_بندی && res_راستہ_Ptr < res_راستہ_حد_بندی) {
                جب (orig_Res_راستہ_Ptr < orig_Res_راستہ_حد_بندی && *orig_Res_راستہ_Ptr != RES_راستہ_SEPARATOR) {
                    ++orig_Res_راستہ_Ptr;
                }
                اگر (orig_Res_راستہ_Ptr < orig_Res_راستہ_حد_بندی && *orig_Res_راستہ_Ptr == RES_راستہ_SEPARATOR) {
                    ++orig_Res_راستہ_Ptr;
                }
                جب (res_راستہ_Ptr < res_راستہ_حد_بندی && *res_راستہ_Ptr != RES_راستہ_SEPARATOR) {
                    ++res_راستہ_Ptr;
                }
                اگر (res_راستہ_Ptr < res_راستہ_حد_بندی && *res_راستہ_Ptr == RES_راستہ_SEPARATOR) {
                    ++res_راستہ_Ptr;
                }
            }
            
            // New remove from the beginning of `key` the number of segments remaining in resPath.
            // If resPath has more segments than `key` does, `key` will end up empty.
            جب (res_راستہ_Ptr < res_راستہ_حد_بندی && *کلید != '\0') {
                جب (res_راستہ_Ptr < res_راستہ_حد_بندی && *res_راستہ_Ptr != RES_راستہ_SEPARATOR) {
                    ++res_راستہ_Ptr;
                }
                اگر (res_راستہ_Ptr < res_راستہ_حد_بندی && *res_راستہ_Ptr == RES_راستہ_SEPARATOR) {
                    ++res_راستہ_Ptr;
                }
                جب (*کلید != '\0' && *کلید != RES_راستہ_SEPARATOR) {
                    ++کلید;
                }
                اگر (*کلید == RES_راستہ_SEPARATOR) {
                    ++کلید;
                }
            }
        }
        // Finally, append what's left of `key` to `path`.  What you end up with here is `resPath`, plus
        // any pieces of `key` that aren't superseded by `resPath`.
        // Or, to put it another way, calculate <#-segments-in-key> - (<#-segments-in-resPath> - <#-segments-in-origResPath>),
        // and append that many segments from the end of `key` to `resPath` to produce the result.
        راستہ.append(کلید, *حالت_2);
    } ورنہ {
        راستہ.append(اندر_کلید, *حالت_2);
    }
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلید_With_Fallback(ثابت UResourceBundle* resB,
                          ثابت حرف* اندر_کلید,
                          UResourceBundle* fill_اندر U_LIFETIME_BOUND,
                          UError_رمز* حالت_2) {
    Resource res = RES_BOGUS, جڑ_Res = RES_BOGUS;
    UResourceBundle *helper = خالی_اشاریہ;

    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس fill_اندر;
    }
    اگر(resB == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس fill_اندر;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    اگر(URES_IS_جدول(type)) {
        ثابت حرف* orig_Res_راستہ = resB->f_Res_راستہ;
        int32_t orig_Res_راستہ_Len = resB->f_Res_راستہ_Len;
        res = get_جدول_شے_By_کلید_راستہ(&resB->get_Res_ڈیٹا(), resB->fRes, اندر_کلید);
        ثابت حرف* کلید = اندر_کلید;
        منطقی did_جڑ_Once = جھوٹ;
        اگر(res == RES_BOGUS) {
            UResource_ڈیٹا_اندراج *ڈیٹا_اندراج = resB->f_ڈیٹا;
            CharString راستہ;
            حرف *my_راستہ = خالی_اشاریہ;
            ثابت حرف* res_راستہ = resB->f_Res_راستہ;
            int32_t len = resB->f_Res_راستہ_Len;
            جب(res == RES_BOGUS && (ڈیٹا_اندراج->f_والد != خالی_اشاریہ || !did_جڑ_Once)) { /* Otherwise, we'll look in parents */
                اگر (ڈیٹا_اندراج->f_والد != خالی_اشاریہ) {
                    ڈیٹا_اندراج = ڈیٹا_اندراج->f_والد;
                } ورنہ {
                    // We can't just stop when we get to a bundle whose fParent is nullptr.  That'll work most of the time,
                    // but if the bundle that the caller passed to us was "root" (which happens in getAllItemsWithFallback(),
                    // this function will drop right out without doing anything if "root" doesn't contain the exact key path
                    // specified.  In that case, we need one extra time through this loop to make sure we follow any
                    // applicable aliases at the root level.
                    did_جڑ_Once = سچ;
                }
                جڑ_Res = ڈیٹا_اندراج->f_ڈیٹا.جڑ_Res;

                اگر(ڈیٹا_اندراج->fBogus == U_ZERO_ERROR) {
                    create_راستہ(orig_Res_راستہ, orig_Res_راستہ_Len, res_راستہ, len, اندر_کلید, راستہ, حالت_2);
                    اگر (U_FAILURE(*حالت_2)) {
                        ures_close(helper);
                        واپس fill_اندر;
                    }
                    my_راستہ = راستہ.ڈیٹا();
                    کلید = اندر_کلید;
                    کرو {
                        res = res_ڈھونڈو_Resource(&(ڈیٹا_اندراج->f_ڈیٹا), جڑ_Res, &my_راستہ, &کلید);
                        اگر (RES_GET_TYPE(res) == URES_ALIAS && *my_راستہ) {
                            /* We hit an alias, but we didn't finish following the path. */
                            helper = init_resb_نتیجہ(ڈیٹا_اندراج, res, خالی_اشاریہ, -1, resB, helper, حالت_2);
                            /*helper = init_resb_result(dataEntry, res, inKey, -1, resB, helper, status);*/
                            اگر(helper) {
                              ڈیٹا_اندراج = helper->f_ڈیٹا;
                              جڑ_Res = helper->fRes;
                              res_راستہ = helper->f_Res_راستہ;
                              len = helper->f_Res_راستہ_Len;

                            } ورنہ {
                              توڑو;
                            }
                        } ورنہ اگر (res == RES_BOGUS) {
                            توڑو;
                        }
                    } جب(*my_راستہ); /* Continue until the whole path is consumed */
                }
            }
            /*dataEntry = getFallbackData(resB, &key, &res, status);*/
            اگر(res != RES_BOGUS) {
              /* check if resB->fResPath gives the right name here */
                اگر(uprv_strcmp(ڈیٹا_اندراج->fName, uloc_getDefault())==0 || uprv_strcmp(ڈیٹا_اندراج->fName, k_جڑ_Locale_Name)==0) {
                    *حالت_2 = U_USING_DEFAULT_WARNING;
                } ورنہ {
                    *حالت_2 = U_USING_FALLBACK_WARNING;
                }

                fill_اندر = init_resb_نتیجہ(ڈیٹا_اندراج, res, کلید, -1, resB, fill_اندر, حالت_2);
                اگر (res_راستہ != خالی_اشاریہ) {
                    create_راستہ(orig_Res_راستہ, orig_Res_راستہ_Len, res_راستہ, len, اندر_کلید, راستہ, حالت_2);
                } ورنہ {
                    ثابت حرف* separator = خالی_اشاریہ;
                    اگر (fill_اندر->f_Res_راستہ != خالی_اشاریہ) {
                        separator = uprv_strchr(fill_اندر->f_Res_راستہ, RES_راستہ_SEPARATOR);
                    }
                    اگر (separator != خالی_اشاریہ && separator[1] != '\0') {
                        create_راستہ(orig_Res_راستہ, orig_Res_راستہ_Len, fill_اندر->f_Res_راستہ,
                                   static_cast<int32_t>(uprv_strlen(fill_اندر->f_Res_راستہ)), اندر_کلید, راستہ, حالت_2);
                    } ورنہ {
                        create_راستہ(orig_Res_راستہ, orig_Res_راستہ_Len, "", 0, اندر_کلید, راستہ, حالت_2);
                    }
                }
                ures_free_Res_راستہ(fill_اندر);
                ures_append_Res_راستہ(fill_اندر, راستہ.ڈیٹا(), راستہ.length(), حالت_2);
                اگر(fill_اندر->f_Res_راستہ[fill_اندر->f_Res_راستہ_Len-1] != RES_راستہ_SEPARATOR) {
                    ures_append_Res_راستہ(fill_اندر, RES_راستہ_SEPARATOR_S, 1, حالت_2);
                }
            } ورنہ {
                *حالت_2 = U_MISSING_RESOURCE_ERROR;
            }
        } ورنہ {
            fill_اندر = init_resb_نتیجہ(resB->f_ڈیٹا, res, کلید, -1, resB, fill_اندر, حالت_2);
        }
    } 
    ورنہ {
        *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    ures_close(helper);
    واپس fill_اندر;
}

نامخلا {

خالی get_All_اشیاء_With_Fallback(
        ثابت UResourceBundle *bundle, Resource_ڈیٹا_قدر &value,
        ResourceSink &sink, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس; }
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
    value.سیٹ_ڈیٹا(bundle->get_Res_ڈیٹا());
    value.سیٹ_Valid_Locale_ڈیٹا_اندراج(bundle->f_Valid_Locale_ڈیٹا_اندراج);
    UResource_ڈیٹا_اندراج *والد_اندراج = bundle->f_ڈیٹا->f_والد;
    UBool has_والد = والد_اندراج != خالی_اشاریہ && U_SUCCESS(والد_اندراج->fBogus);
    value.سیٹ_Resource(bundle->fRes, ResourceTracer(bundle));
    sink.put(bundle->f_کلید, value, !has_والد, error_رمز);
    اگر (has_والد) {
        // We might try to query the sink whether
        // any fallback from the parent bundle is still possible.

        // Turn the parent UResourceDataEntry into a UResourceBundle,
        // much like in ures_openWithType().
        // TODO: See if we can refactor ures_getByKeyWithFallback()
        // and pull out an inner function that takes and returns a UResourceDataEntry
        // so that we need not create UResourceBundle objects.
        ڈھیر_UResource_Bundle والد_Bundle;
        UResourceBundle &والد_Ref = والد_Bundle.ref();
        والد_Ref.f_ڈیٹا = والد_اندراج;
        والد_Ref.f_Valid_Locale_ڈیٹا_اندراج = bundle->f_Valid_Locale_ڈیٹا_اندراج;
        والد_Ref.fHasFallback = !والد_Ref.get_Res_ڈیٹا().noFallback;
        والد_Ref.f_Is_چوٹی_Level = سچ;
        والد_Ref.fRes = والد_Ref.get_Res_ڈیٹا().جڑ_Res;
        والد_Ref.f_سائز = res_گنتی_Array_اشیاء(&والد_Ref.get_Res_ڈیٹا(), والد_Ref.fRes);
        والد_Ref.f_اشاریہ = -1;
        اندراج_Increase(والد_اندراج);

        // Look up the container item in the parent bundle.
        ڈھیر_UResource_Bundle containerBundle;
        ثابت UResourceBundle *rb;
        UError_رمز راستہ_Error_رمز = U_ZERO_ERROR;  // Ignore if parents up to root do not have this path.
        اگر (bundle->f_Res_راستہ == خالی_اشاریہ || *bundle->f_Res_راستہ == 0) {
            rb = والد_Bundle.getAlias();
        } ورنہ {
            rb = ures_get_By_کلید_With_Fallback(والد_Bundle.getAlias(), bundle->f_Res_راستہ,
                                           containerBundle.getAlias(), &راستہ_Error_رمز);
        }
        اگر (U_SUCCESS(راستہ_Error_رمز)) {
            get_All_اشیاء_With_Fallback(rb, value, sink, error_رمز);
        }
    }
}

ساخت GetAllChildrenSink : عوامی ResourceSink {
    // Destination sink
    ResourceSink& dest;

    GetAllChildrenSink(ResourceSink& dest)
        : dest(dest) {}
    مجازی ~GetAllChildrenSink() override;
    مجازی خالی put(ثابت حرف *کلید, Resource_قدر &value, UBool is_جڑ,
           UError_رمز &error_رمز) override {
        Resource_جدول اشیاء_جدول = value.get_جدول(error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        برائے (int32_t i = 0; اشیاء_جدول.get_کلید_And_قدر(i, کلید, value); ++i) {
            اگر (value.getType() == URES_ALIAS) {
                Resource_ڈیٹا_قدر& rdv = static_cast<Resource_ڈیٹا_قدر&>(value);
                ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
                UResourceBundle* aliasRB = get_Alias_ہدف_As_Resource_Bundle(rdv.get_ڈیٹا(), rdv.getResource(), خالی_اشاریہ, -1,
                                                                          rdv.get_Valid_Locale_ڈیٹا_اندراج(), خالی_اشاریہ, 0,
                                                                          ڈھیر_عارضی_Bundle.getAlias(), &error_رمز);
                اگر (U_SUCCESS(error_رمز)) {
                    Resource_ڈیٹا_قدر aliased_قدر;
                    aliased_قدر.سیٹ_ڈیٹا(aliasRB->get_Res_ڈیٹا());
                    aliased_قدر.سیٹ_Valid_Locale_ڈیٹا_اندراج(aliasRB->f_Valid_Locale_ڈیٹا_اندراج);
                    aliased_قدر.سیٹ_Resource(aliasRB->fRes, ResourceTracer(aliasRB));
                    
                    اگر (aliased_قدر.getType() != URES_جدول) {
                        dest.put(کلید, aliased_قدر, is_جڑ, error_رمز);
                    } ورنہ {
                        // if the resource we're aliasing over to is a table, the sink might iterate over its contents.
                        // If it does, it'll get only the things defined in the actual alias target, not the things
                        // the target inherits from its parent resources.  So we walk the parent chain for the *alias target*,
                        // calling dest.put() for each of the parent tables we could be inheriting from.  This means
                        // that dest.put() has to iterate over the children of multiple tables to get all of the inherited
                        // resource values, but it already has to do that to handle normal vertical inheritance.
                        UResType aliased_قدر_Type = URES_جدول;
                        CharString جدول_راستہ;
                        جدول_راستہ.append(aliasRB->f_Res_راستہ, error_رمز);
                        ثابت حرف* والد_کلید = کلید; // dest.put() changes the key
                        dest.put(والد_کلید, aliased_قدر, is_جڑ, error_رمز);
                        UResource_ڈیٹا_اندراج* اندراج_2 = aliasRB->f_ڈیٹا;
                        Resource res = aliasRB->fRes;
                        جب (aliased_قدر_Type == URES_جدول && اندراج_2->f_والد != خالی_اشاریہ) {
                            CharString local_راستہ;
                            local_راستہ.copyFrom(جدول_راستہ, error_رمز);
                            حرف* local_راستہ_As_Char_Ptr = local_راستہ.ڈیٹا();
                            ثابت حرف* بچہ_کلید;
                            اندراج_2 = اندراج_2->f_والد;
                            res = اندراج_2->f_ڈیٹا.جڑ_Res;
                            Resource newRes = res_ڈھونڈو_Resource(&اندراج_2->f_ڈیٹا, res, &local_راستہ_As_Char_Ptr, &بچہ_کلید);
                            اگر (newRes != RES_BOGUS) {
                                aliased_قدر.سیٹ_ڈیٹا(اندراج_2->f_ڈیٹا);
                                // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                aliased_قدر.سیٹ_Resource(newRes, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                aliased_قدر_Type = aliased_قدر.getType();
                                اگر (aliased_قدر_Type == URES_ALIAS) {
                                    // in a few rare cases, when we get to the root resource bundle, the resource in question
                                    // won't be an actual table, but will instead be an alias to a table.  That is, we have
                                    // two aliases in the inheritance path.  (For some locales, such as Zulu, we see this with
                                    // children of the "fields" resource: "day-narrow" aliases to "day-short", which aliases
                                    // to "day".)  When this happens, we need to make sure we follow all the aliases.
                                    Resource_ڈیٹا_قدر& rdv2 = static_cast<Resource_ڈیٹا_قدر&>(aliased_قدر);
                                    aliasRB = get_Alias_ہدف_As_Resource_Bundle(rdv2.get_ڈیٹا(), rdv2.getResource(), خالی_اشاریہ, -1,
                                                                             rdv2.get_Valid_Locale_ڈیٹا_اندراج(), خالی_اشاریہ, 0,
                                                                             ڈھیر_عارضی_Bundle.getAlias(), &error_رمز);
                                    جدول_راستہ.clear();
                                    جدول_راستہ.append(aliasRB->f_Res_راستہ, error_رمز);
                                    اندراج_2 = aliasRB->f_ڈیٹا;
                                    res = aliasRB->fRes;
                                    aliased_قدر.سیٹ_ڈیٹا(اندراج_2->f_ڈیٹا);
                                    // TODO: do I also need to call aliasedValue.setValueLocaleDataEntry() ?
                                    aliased_قدر.سیٹ_Resource(res, ResourceTracer(aliasRB)); // probably wrong to use aliasRB here
                                    aliased_قدر_Type = aliased_قدر.getType();
                                }
                                اگر (aliased_قدر_Type == URES_جدول) {
                                    dest.put(والد_کلید, aliased_قدر, is_جڑ, error_رمز);
                                } ورنہ {
                                    // once we've followed the alias, the resource we're looking at really should
                                    // be a table
                                    error_رمز = U_اندرونی_پروگرام_ERROR;
                                    واپس;
                                }
                            }
                        }
                    }
                }
            } ورنہ {
                dest.put(کلید, value, is_جڑ, error_رمز);
            }
            اگر (U_FAILURE(error_رمز)) { واپس; }
        }
    }
};

// Virtual destructors must be defined out of line.
GetAllChildrenSink::~GetAllChildrenSink() {}

U_CAPI خالی U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *راستہ,
                                icu::ResourceSink &sink, UError_رمز &error_رمز) {
    GetAllChildrenSink allChildrenSink(sink);
    ures_get_All_اشیاء_With_Fallback(bundle, راستہ, allChildrenSink, error_رمز);
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
U_CAPI خالی U_EXPORT2
ures_get_قدر_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *راستہ,
                          UResourceBundle *عارضی_Fill_اندر,
                          Resource_ڈیٹا_قدر &value, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    اگر (راستہ == خالی_اشاریہ) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    ثابت UResourceBundle *rb;
    اگر (*راستہ == 0) {
        // empty path
        rb = bundle;
    } ورنہ {
        rb = ures_get_By_کلید_With_Fallback(bundle, راستہ, عارضی_Fill_اندر, &error_رمز);
        اگر (U_FAILURE(error_رمز)) {
            واپس;
        }
    }
    value.سیٹ_ڈیٹا(rb->get_Res_ڈیٹا());
    value.سیٹ_Valid_Locale_ڈیٹا_اندراج(rb->f_Valid_Locale_ڈیٹا_اندراج);
    value.سیٹ_Resource(rb->fRes, ResourceTracer(rb));
}

U_CAPI خالی U_EXPORT2
ures_get_All_اشیاء_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *راستہ,
                             icu::ResourceSink &sink, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    اگر (راستہ == خالی_اشاریہ) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    ڈھیر_UResource_Bundle ڈھیر_Bundle;
    ثابت UResourceBundle *rb;
    اگر (*راستہ == 0) {
        // empty path
        rb = bundle;
    } ورنہ {
        rb = ures_get_By_کلید_With_Fallback(bundle, راستہ, ڈھیر_Bundle.getAlias(), &error_رمز);
        اگر (U_FAILURE(error_رمز)) {
            واپس;
        }
    }
    // Get all table items with fallback.
    Resource_ڈیٹا_قدر value;
    get_All_اشیاء_With_Fallback(rb, value, sink, error_رمز);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_کلید(ثابت UResourceBundle* resB,
              ثابت حرف* اندر_کلید,
              UResourceBundle* fill_اندر U_LIFETIME_BOUND,
              UError_رمز* حالت_2) {
    Resource res = RES_BOGUS;
    UResource_ڈیٹا_اندراج *ڈیٹا_اندراج = خالی_اشاریہ;
    ثابت حرف *کلید = اندر_کلید;

    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس fill_اندر;
    }
    اگر(resB == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس fill_اندر;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    اگر(URES_IS_جدول(type)) {
        int32_t t;
        res = res_get_جدول_شے_By_کلید(&resB->get_Res_ڈیٹا(), resB->fRes, &t, &کلید);
        اگر(res == RES_BOGUS) {
            کلید = اندر_کلید;
            اگر(resB->fHasFallback) {
                ڈیٹا_اندراج = get_Fallback_ڈیٹا(resB, &کلید, &res, حالت_2);
                اگر(U_SUCCESS(*حالت_2)) {
                    /* check if resB->fResPath gives the right name here */
                    واپس init_resb_نتیجہ(ڈیٹا_اندراج, res, کلید, -1, resB, fill_اندر, حالت_2);
                } ورنہ {
                    *حالت_2 = U_MISSING_RESOURCE_ERROR;
                }
            } ورنہ {
                *حالت_2 = U_MISSING_RESOURCE_ERROR;
            }
        } ورنہ {
            واپس init_resb_نتیجہ(resB->f_ڈیٹا, res, کلید, -1, resB, fill_اندر, حالت_2);
        }
    } 
#اگر 0
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */
    ورنہ اگر(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == سچ) {
        /* here should go a first attempt to locate the key using index table */
        ڈیٹا_اندراج = get_Fallback_ڈیٹا(resB, &کلید, &res, حالت_2);
        اگر(U_SUCCESS(*حالت_2)) {
            واپس init_resb_نتیجہ(ڈیٹا_اندراج, res, کلید, resB, fill_اندر, حالت_2);
        } ورنہ {
            *حالت_2 = U_MISSING_RESOURCE_ERROR;
        }
    }
#اگر_ختم    
    ورنہ {
        *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس fill_اندر;
}

U_CAPI ثابت char16_t* U_EXPORT2
ures_get_String_By_کلید(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                    ثابت حرف* اندر_کلید,
                    int32_t* len,
                    UError_رمز* حالت_2) {
    Resource res = RES_BOGUS;
    UResource_ڈیٹا_اندراج *ڈیٹا_اندراج = خالی_اشاریہ;
    ثابت حرف* کلید = اندر_کلید;

    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر(resB == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    int32_t type = RES_GET_TYPE(resB->fRes);
    اگر(URES_IS_جدول(type)) {
        int32_t t=0;

        res = res_get_جدول_شے_By_کلید(&resB->get_Res_ڈیٹا(), resB->fRes, &t, &کلید);

        اگر(res == RES_BOGUS) {
            کلید = اندر_کلید;
            اگر(resB->fHasFallback) {
                ڈیٹا_اندراج = get_Fallback_ڈیٹا(resB, &کلید, &res, حالت_2);
                اگر(U_SUCCESS(*حالت_2)) {
                    انتخاب (RES_GET_TYPE(res)) {
                    حالت URES_STRING:
                    حالت URES_STRING_V2:
                        واپس res_getString({resB, کلید}, &ڈیٹا_اندراج->f_ڈیٹا, res, len);
                    حالت URES_ALIAS:
                      {
                        ثابت char16_t* نتیجہ = خالی_اشاریہ;
                        UResourceBundle *عارضی_Res = ures_get_By_کلید(resB, اندر_کلید, خالی_اشاریہ, حالت_2);
                        نتیجہ = ures_getString(عارضی_Res, len, حالت_2);
                        ures_close(عارضی_Res);
                        واپس نتیجہ;
                      }
                    طےشدہ:
                        *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
                    }
                } ورنہ {
                    *حالت_2 = U_MISSING_RESOURCE_ERROR;
                }
            } ورنہ {
                *حالت_2 = U_MISSING_RESOURCE_ERROR;
            }
        } ورنہ {
            انتخاب (RES_GET_TYPE(res)) {
            حالت URES_STRING:
            حالت URES_STRING_V2:
                واپس res_getString({resB, کلید}, &resB->get_Res_ڈیٹا(), res, len);
            حالت URES_ALIAS:
              {
                ثابت char16_t* نتیجہ = خالی_اشاریہ;
                UResourceBundle *عارضی_Res = ures_get_By_کلید(resB, اندر_کلید, خالی_اشاریہ, حالت_2);
                نتیجہ = ures_getString(عارضی_Res, len, حالت_2);
                ures_close(عارضی_Res);
                واپس نتیجہ;
              }
            طےشدہ:
                *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
            }
        }
    } 
#اگر 0 
    /* this is a kind of TODO item. If we have an array with an index table, we could do this. */
    /* not currently */   
    ورنہ اگر(RES_GET_TYPE(resB->fRes) == URES_ARRAY && resB->fHasFallback == سچ) {
        /* here should go a first attempt to locate the key using index table */
        ڈیٹا_اندراج = get_Fallback_ڈیٹا(resB, &کلید, &res, حالت_2);
        اگر(U_SUCCESS(*حالت_2)) {
            // TODO: Tracing
            واپس res_getString(rd, res, len);
        } ورنہ {
            *حالت_2 = U_MISSING_RESOURCE_ERROR;
        }
    } 
#اگر_ختم    
    ورنہ {
        *حالت_2 = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس خالی_اشاریہ;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_UTF8_String_By_کلید(ثابت UResourceBundle* resB,
                        ثابت حرف* کلید,
                        حرف* dest U_LIFETIME_BOUND,
                        int32_t* p_لمبائی,
                        UBool forceCopy,
                        UError_رمز* حالت_2) {
    int32_t length16;
    ثابت char16_t *s16 = ures_get_String_By_کلید(resB, کلید, &length16, حالت_2);
    واپس ures_مقصد_UTF8_String(s16, length16, dest, p_لمبائی, forceCopy, حالت_2);
}

/* TODO: clean from here down */

/**
 *  INTERNAL: Get the name of the first real locale (not placeholder) 
 *  that has resource bundle data.
 */
U_CAPI ثابت حرف* U_EXPORT2
ures_get_Locale_اندرونی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_رمز* حالت_2)
{
    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (!resourceBundle) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    } ورنہ {
      واپس resourceBundle->f_ڈیٹا->fName;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocale(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_رمز* حالت_2)
{
  واپس ures_get_Locale_اندرونی(resourceBundle, حالت_2);
}

U_CAPI ثابت حرف* U_EXPORT2
ures_getLocaleByType(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_ڈیٹا_Locale_Type type,
                     UError_رمز* حالت_2) {
    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (!resourceBundle) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    } ورنہ {
        انتخاب(type) {
        حالت ULOC_ACTUAL_LOCALE:
            واپس resourceBundle->f_ڈیٹا->fName;
        حالت ULOC_VALID_LOCALE:
            واپس resourceBundle->f_Valid_Locale_ڈیٹا_اندراج->fName;
        حالت ULOC_REQUESTED_LOCALE:
        طےشدہ:
            *حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس خالی_اشاریہ;
        }
    }
}

U_CFUNC ثابت حرف*
ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  اگر(resB == خالی_اشاریہ) {
    واپس خالی_اشاریہ;
  }

  واپس resB->f_ڈیٹا->fName;
}

#اگر_تعریف_شدہ URES_DEBUG
U_CFUNC ثابت حرف*
ures_get_راستہ(ثابت UResourceBundle* resB U_LIFETIME_BOUND) {
  اگر(resB == خالی_اشاریہ) {
    واپس خالی_اشاریہ;
  }

  واپس resB->f_ڈیٹا->f_راستہ;
}
#اگر_ختم

ساکن UResourceBundle*
ures_openWithType(UResourceBundle* r U_LIFETIME_BOUND,
                  ثابت حرف* راستہ,
                  ثابت حرف* localeID,
                  UResOpenType openType,
                  UError_رمز* حالت_2) {
    اگر(U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    UResource_ڈیٹا_اندراج *اندراج_2;
    اگر(openType != URES_OPEN_براہ_راست) {
        اگر (localeID == خالی_اشاریہ) {
            localeID = uloc_getDefault();
        }
        /* first "canonicalize" the locale ID */
        CharString canonLocaleID = ulocimp_get_بنیاد_Name(localeID, *حالت_2);
        اگر(U_FAILURE(*حالت_2)) {
            *حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس خالی_اشاریہ;
        }
        اندراج_2 = اندراج_Open(راستہ, canonLocaleID.ڈیٹا(), openType, حالت_2);
    } ورنہ {
        اندراج_2 = اندراج_Open_براہ_راست(راستہ, localeID, حالت_2);
    }
    اگر(U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر(اندراج_2 == خالی_اشاریہ) {
        *حالت_2 = U_MISSING_RESOURCE_ERROR;
        واپس خالی_اشاریہ;
    }

    UBool is_ڈھیر_Object;
    اگر(r == خالی_اشاریہ) {
        r = static_cast<UResourceBundle*>(uprv_malloc(حجم(UResourceBundle)));
        اگر(r == خالی_اشاریہ) {
            اندراج_Close(اندراج_2);
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }
        is_ڈھیر_Object = جھوٹ;
    } ورنہ {  // fill-in
        is_ڈھیر_Object = ures_is_ڈھیر_Object(r);
        ures_closeBundle(r, جھوٹ);
    }
    uprv_memset(r, 0, حجم(UResourceBundle));
    ures_سیٹ_Is_ڈھیر_Object(r, is_ڈھیر_Object);

    r->f_Valid_Locale_ڈیٹا_اندراج = r->f_ڈیٹا = اندراج_2;
    r->fHasFallback = openType != URES_OPEN_براہ_راست && !r->get_Res_ڈیٹا().noFallback;
    r->f_Is_چوٹی_Level = سچ;
    r->fRes = r->get_Res_ڈیٹا().جڑ_Res;
    r->f_سائز = res_گنتی_Array_اشیاء(&r->get_Res_ڈیٹا(), r->fRes);
    r->f_اشاریہ = -1;

    ResourceTracer(r).traceOpen();

    واپس r;
}

U_CAPI UResourceBundle* U_EXPORT2
ures_open(ثابت حرف* راستہ, ثابت حرف* localeID, UError_رمز* حالت_2) {
    واپس ures_openWithType(خالی_اشاریہ, راستہ, localeID, URES_OPEN_LOCALE_DEFAULT_جڑ, حالت_2);
}

U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت حرف* راستہ, ثابت حرف* localeID, UError_رمز* حالت_2) {
    واپس ures_openWithType(خالی_اشاریہ, راستہ, localeID, URES_OPEN_LOCALE_جڑ, حالت_2);
}

/**
 *  Opens a resource bundle without "canonicalizing" the locale name. No fallback will be performed 
 *  or sought. However, alias substitution will happen!
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open_براہ_راست(ثابت حرف* راستہ, ثابت حرف* localeID, UError_رمز* حالت_2) {
    واپس ures_openWithType(خالی_اشاریہ, راستہ, localeID, URES_OPEN_براہ_راست, حالت_2);
}

/**
 *  Internal API: This function is used to open a resource bundle 
 *  proper fallback chaining is executed while initialization. 
 *  The result is stored in cache for later fallback search.
 * 
 * Same as ures_open(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI خالی U_EXPORT2
ures_open_Fill_اندر(UResourceBundle *r, ثابت حرف* راستہ,
                ثابت حرف* localeID, UError_رمز* حالت_2) {
    اگر(U_SUCCESS(*حالت_2) && r == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    ures_openWithType(r, راستہ, localeID, URES_OPEN_LOCALE_DEFAULT_جڑ, حالت_2);
}

/**
 * Same as ures_openDirect(), but uses the fill-in parameter and does not allocate a new bundle.
 */
U_CAPI خالی U_EXPORT2
ures_open_براہ_راست_Fill_اندر(UResourceBundle *r, ثابت حرف* راستہ, ثابت حرف* localeID, UError_رمز* حالت_2) {
    اگر(U_SUCCESS(*حالت_2) && r == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    ures_openWithType(r, راستہ, localeID, URES_OPEN_براہ_راست, حالت_2);
}

/**
 *  API: Counts members. For arrays and tables, returns number of resources.
 *  For strings, returns 1.
 */
U_CAPI int32_t  U_EXPORT2
ures_گنتی_Array_اشیاء(ثابت UResourceBundle* resourceBundle,
                  ثابت حرف* resource_کلید,
                  UError_رمز* حالت_2)
{
    UResourceBundle res_ڈیٹا;
    ures_init_ڈھیر_Object(&res_ڈیٹا);
    اگر (حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس 0;
    }
    اگر(resourceBundle == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    ures_get_By_کلید(resourceBundle, resource_کلید, &res_ڈیٹا, حالت_2);
    
    اگر(res_ڈیٹا.get_Res_ڈیٹا().ڈیٹا != خالی_اشاریہ) {
        int32_t نتیجہ = res_گنتی_Array_اشیاء(&res_ڈیٹا.get_Res_ڈیٹا(), res_ڈیٹا.fRes);
        ures_close(&res_ڈیٹا);
        واپس نتیجہ;
    } ورنہ {
        *حالت_2 = U_MISSING_RESOURCE_ERROR;
        ures_close(&res_ڈیٹا);
        واپس 0;
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
    اگر (!resourceBundle) واپس خالی_اشاریہ;

    اگر(resourceBundle->f_نسخہ == خالی_اشاریہ) {

        /* If the version ID has not been built yet, then do so.  Retrieve */
        /* the minor version from the file. */
        UError_رمز حالت_2 = U_ZERO_ERROR;
        int32_t minor_len = 0;
        int32_t len;

        ثابت char16_t* minor_نسخہ = ures_get_String_By_کلید(resourceBundle, k_نسخہ_لیبل, &minor_len, &حالت_2);
        
        /* Determine the length of of the final version string.  This is */
        /* the length of the major part + the length of the separator */
        /* (==1) + the length of the minor part (+ 1 for the zero byte at */
        /* the end). */

        len = (minor_len > 0) ? minor_len : 1;
    
        /* Allocate the string, and build it up. */
        /* + 1 for zero byte */


        ((UResourceBundle *)resourceBundle)->f_نسخہ = (حرف *)uprv_malloc(1 + len); 
        /* Check for null pointer. */
        اگر (((UResourceBundle *)resourceBundle)->f_نسخہ == خالی_اشاریہ) {
            واپس خالی_اشاریہ;
        }
       
        اگر(minor_len > 0) {
            u_UChars_مقصد_Chars(minor_نسخہ, resourceBundle->f_نسخہ , minor_len);
            resourceBundle->f_نسخہ[len] =  '\0';
        }
        ورنہ {
            uprv_strcpy(resourceBundle->f_نسخہ, k_Default_Minor_نسخہ);
        }
    }

    واپس resourceBundle->f_نسخہ;
}

U_CAPI ثابت حرف* U_EXPORT2
ures_get_نسخہ_نمبر(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND)
{
    واپس ures_get_نسخہ_نمبر_اندرونی(resourceBundle);
}

U_CAPI خالی U_EXPORT2 ures_get_نسخہ(ثابت UResourceBundle* resB, UVersionInfo نسخہ_Info) {
    اگر (!resB) واپس;

    u_نسخہ_From_String(نسخہ_Info, ures_get_نسخہ_نمبر_اندرونی(resB));
}

/** Tree support functions *******************************/
#تعریف_کرو اشاریہ_LOCALE_NAME "res_index"
#تعریف_کرو اشاریہ_لیبل         "InstalledLocales"
#تعریف_کرو DEFAULT_لیبل       "default"

#اگر defined(URES_درخت_DEBUG)
#شامل <stdio.h>
#اگر_ختم

قسمتعریف ساخت ULocalesContext {
    UResourceBundle installed;
    UResourceBundle curr;
} ULocalesContext;

ساکن خالی U_CALLCONV
ures_loc_closeLocales(UEnumeration *enumerator) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(enumerator->context);
    ures_close(&ctx->curr);
    ures_close(&ctx->installed);
    uprv_free(ctx);
    uprv_free(enumerator);
}

ساکن int32_t U_CALLCONV
ures_loc_گنتی_Locales(UEnumeration *en, UError_رمز * /*حالت*/) {
    ULocalesContext* ctx = static_cast<ULocalesContext*>(en->context);
    واپس ures_get_سائز(&ctx->installed);
}

U_CDECL_BEGIN


ساکن ثابت حرف * U_CALLCONV
ures_loc_اگلا_Locale(UEnumeration* en,
                    int32_t* نتیجہ_لمبائی,
                    UError_رمز* حالت_2) {
    ULocalesContext *ctx = (ULocalesContext *)en->context;
    UResourceBundle *res = &(ctx->installed);
    UResourceBundle *k = خالی_اشاریہ;
    ثابت حرف *نتیجہ = خالی_اشاریہ;
    int32_t len = 0;
    اگر (ures_has_اگلا(res) && (k = ures_get_اگلا_Resource(res, &ctx->curr, حالت_2)) != خالی_اشاریہ) {
        نتیجہ = ures_get_کلید(k);
        len = (int32_t)uprv_strlen(نتیجہ);
    }
    اگر (نتیجہ_لمبائی) {
        *نتیجہ_لمبائی = len;
    }
    واپس نتیجہ;
}

ساکن خالی U_CALLCONV 
ures_loc_resetLocales(UEnumeration* en, 
                      UError_رمز* /*حالت*/) {
    UResourceBundle *res = &((ULocalesContext *)en->context)->installed;
    ures_resetIterator(res);
}

U_CDECL_انتہا

ساکن ثابت UEnumeration gLocalesEnum = {
    خالی_اشاریہ,
        خالی_اشاریہ,
        ures_loc_closeLocales,
        ures_loc_گنتی_Locales,
        uenum_unextDefault,
        ures_loc_اگلا_Locale,
        ures_loc_resetLocales
};


U_CAPI UEnumeration* U_EXPORT2
ures_open_دستیاب_Locales(ثابت حرف *راستہ, UError_رمز *حالت_2)
{
    UResourceBundle *idx = خالی_اشاریہ;
    UEnumeration *en = خالی_اشاریہ;
    ULocalesContext *myContext = خالی_اشاریہ;

    اگر(U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    myContext = static_cast<ULocalesContext *>(uprv_malloc(حجم(ULocalesContext)));
    en =  (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    اگر(!en || !myContext) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        uprv_free(en);
        uprv_free(myContext);
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(en, &gLocalesEnum, حجم(UEnumeration));

    ures_init_ڈھیر_Object(&myContext->installed);
    ures_init_ڈھیر_Object(&myContext->curr);
    idx = ures_open_براہ_راست(راستہ, اشاریہ_LOCALE_NAME, حالت_2);
    ures_get_By_کلید(idx, اشاریہ_لیبل, &myContext->installed, حالت_2);
    اگر(U_SUCCESS(*حالت_2)) {
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "Got %s::%s::[%s] : %s\n", 
            راستہ, اشاریہ_LOCALE_NAME, اشاریہ_لیبل, ures_get_کلید(&myContext->installed));
#اگر_ختم
        en->context = myContext;
    } ورنہ {
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s open failed - %s\n", راستہ, u_errorName(*حالت_2));
#اگر_ختم
        ures_close(&myContext->installed);
        uprv_free(myContext);
        uprv_free(en);
        en = خالی_اشاریہ;
    }
    
    ures_close(idx);
    
    واپس en;
}

ساکن UBool is_Locale_اندر_List(UEnumeration *locEnum, ثابت حرف *loc_مقصد_تلاش, UError_رمز *حالت_2) {
    ثابت حرف *loc;
    جب ((loc = uenum_اگلا(locEnum, خالی_اشاریہ, حالت_2)) != خالی_اشاریہ) {
        اگر (uprv_strcmp(loc, loc_مقصد_تلاش) == 0) {
            واپس سچ;
        }
    }
    واپس جھوٹ;
}

ساکن خالی get_والد_For_Functional_Equivalent(ثابت حرف*      localeID,
                                             UResourceBundle* res,
                                             UResourceBundle* bund1,
                                             CharString&      والد) {
    // Get parent.
    // First check for a parent from %%Parent resource (Note that in resource trees
    // such as collation, data may have different parents than in parentLocales).
    UError_رمز sub_حالت = U_ZERO_ERROR;
    والد.clear();
    اگر (res != خالی_اشاریہ) {
        ures_get_By_کلید(res, "%%Parent", bund1, &sub_حالت);
        اگر (U_SUCCESS(sub_حالت)) {
            int32_t length16;
            ثابت char16_t* s16 = ures_getString(bund1, &length16, &sub_حالت);
            والد.appendInvariantChars(s16, length16, sub_حالت);
        }
    }
    
    // If none there, use normal truncation parent
    اگر (U_FAILURE(sub_حالت) || والد.is_خالی_خانے()) {
        sub_حالت = U_ZERO_ERROR;
        والد = ulocimp_get_والد(localeID, sub_حالت);
    }
}

U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(حرف *نتیجہ, int32_t نتیجہ_استعداد,
                             ثابت حرف *راستہ, ثابت حرف *resName, ثابت حرف *keyword, ثابت حرف *locid,
                             UBool *is_دستیاب, UBool omitDefault, UError_رمز *حالت_2)
{
    CharString defVal; /* default value for given locale */
    CharString defLoc; /* default value for given locale */
    CharString ملا;
    CharString والد;
    CharString بھرا;
    UResourceBundle bund1, bund2;
    UResourceBundle *res = خالی_اشاریہ;
    UError_رمز sub_حالت = U_ZERO_ERROR;
    int32_t length = 0;
    اگر(U_FAILURE(*حالت_2)) واپس 0;
    CharString kwVal;
    اگر (keyword != خالی_اشاریہ && *keyword != '\0') {
        kwVal = ulocimp_get_Keyword_قدر(locid, keyword, sub_حالت);
        اگر (kwVal == DEFAULT_لیبل) {
            kwVal.clear();
        }
    }
    اگر (locid == خالی_اشاریہ) {
        locid = uloc_getDefault();
    }
    CharString بنیاد = ulocimp_get_بنیاد_Name(locid, sub_حالت);
#اگر defined(URES_درخت_DEBUG)
    fprintf(stderr, "getFunctionalEquivalent: \"%s\" [%s=%s] in %s - %s\n", 
            locid, keyword, kwVal.ڈیٹا(), بنیاد.ڈیٹا(), u_errorName(sub_حالت));
#اگر_ختم
    ures_init_ڈھیر_Object(&bund1);
    ures_init_ڈھیر_Object(&bund2);

    والد.copyFrom(بنیاد, sub_حالت);
    ملا.copyFrom(بنیاد, sub_حالت);

    اگر(is_دستیاب) {
        UEnumeration *locEnum = ures_open_دستیاب_Locales(راستہ, &sub_حالت);
        *is_دستیاب = سچ;
        اگر (U_SUCCESS(sub_حالت)) {
            *is_دستیاب = is_Locale_اندر_List(locEnum, والد.ڈیٹا(), &sub_حالت);
        }
        uenum_close(locEnum);
    }

    اگر(U_FAILURE(sub_حالت)) {
        *حالت_2 = sub_حالت;
        واپس 0;
    }
    
    کرو {
        sub_حالت = U_ZERO_ERROR;
        res = ures_open(راستہ, والد.ڈیٹا(), &sub_حالت);
        اگر(((sub_حالت == U_USING_FALLBACK_WARNING) ||
            (sub_حالت == U_USING_DEFAULT_WARNING)) && is_دستیاب)
        {
            *is_دستیاب = جھوٹ;
        }
        is_دستیاب = خالی_اشاریہ; /* only want to set this the first time around */
        
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s;%s -> %s [%s]\n", راستہ?راستہ:"ICUDATA", والد.ڈیٹا(), u_errorName(sub_حالت), ures_getLocale(res, &sub_حالت));
#اگر_ختم
        اگر(U_FAILURE(sub_حالت)) {
            *حالت_2 = sub_حالت;
        } ورنہ اگر(sub_حالت == U_ZERO_ERROR) {
            ures_get_By_کلید(res,resName,&bund1, &sub_حالت);
            اگر(sub_حالت == U_ZERO_ERROR) {
                ثابت char16_t *defUstr;
                int32_t defLen;
                /* look for default item */
#اگر defined(URES_درخت_DEBUG)
                fprintf(stderr, "%s;%s : loaded default -> %s\n",
                    راستہ?راستہ:"ICUDATA", والد.ڈیٹا(), u_errorName(sub_حالت));
#اگر_ختم
                defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_لیبل, &defLen, &sub_حالت);
                اگر(U_SUCCESS(sub_حالت) && defLen) {
                    defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#اگر defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                        راستہ?راستہ:"ICUDATA", والد.ڈیٹا(), keyword, defVal.ڈیٹا(), u_errorName(sub_حالت));
#اگر_ختم
                    defLoc.copyFrom(والد, sub_حالت);
                    اگر(kwVal.is_خالی_خانے()) {
                        kwVal.append(defVal, sub_حالت);
#اگر defined(URES_درخت_DEBUG)
                        fprintf(stderr, "%s;%s -> kwVal =  %s\n", 
                            راستہ?راستہ:"ICUDATA", والد.ڈیٹا(), keyword, kwVal.ڈیٹا());
#اگر_ختم
                    }
                }
            }
        }
        
        sub_حالت = U_ZERO_ERROR;

        اگر (res != خالی_اشاریہ) {
            ملا.clear().append(ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_حالت), sub_حالت);
        }

        اگر (ملا != والد) {
            والد.copyFrom(ملا, sub_حالت);
        } ورنہ {
            get_والد_For_Functional_Equivalent(ملا.ڈیٹا(),res,&bund1,والد);
        }
        ures_close(res);
    } جب(defVal.is_خالی_خانے() && !ملا.is_خالی_خانے() && ملا != "root" && U_SUCCESS(*حالت_2));
    
    /* Now, see if we can find the kwVal collator.. start the search over.. */
    والد.copyFrom(بنیاد, sub_حالت);
    ملا.copyFrom(بنیاد, sub_حالت);

    کرو {
        res = ures_open(راستہ, والد.ڈیٹا(), &sub_حالت);
        اگر((sub_حالت == U_USING_FALLBACK_WARNING) && is_دستیاب) {
            *is_دستیاب = جھوٹ;
        }
        is_دستیاب = خالی_اشاریہ; /* only want to set this the first time around */
        
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "%s;%s -> %s (looking for %s)\n", 
            راستہ?راستہ:"ICUDATA", والد.ڈیٹا(), u_errorName(sub_حالت), kwVal.ڈیٹا());
#اگر_ختم
        اگر(U_FAILURE(sub_حالت)) {
            *حالت_2 = sub_حالت;
        } ورنہ اگر(sub_حالت == U_ZERO_ERROR) {
            ures_get_By_کلید(res,resName,&bund1, &sub_حالت);
#اگر defined(URES_درخت_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, resName, u_errorName(sub_حالت));
#اگر_ختم
            اگر(sub_حالت == U_ZERO_ERROR) {
                ures_get_By_کلید(&bund1, kwVal.ڈیٹا(), &bund2, &sub_حالت);
#اگر defined(URES_درخت_DEBUG)
/**/ fprintf(stderr,"@%d [%s] %s\n", __LINE__, kwVal.ڈیٹا(), u_errorName(sub_حالت));
#اگر_ختم
                اگر(sub_حالت == U_ZERO_ERROR) {
#اگر defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s;%s -> full0 %s=%s,  %s\n", 
                        راستہ?راستہ:"ICUDATA", والد.ڈیٹا(), keyword, kwVal.ڈیٹا(), u_errorName(sub_حالت));
#اگر_ختم
                    اگر (والد.is_خالی_خانے()) {
                        بھرا.clear().append("root", sub_حالت);
                    } ورنہ {
                        بھرا.copyFrom(والد, sub_حالت);
                    }
                        /* now, recalculate default kw if need be */
                        اگر(defLoc.length() > بھرا.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#اگر defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default0\n", 
                                    راستہ?راستہ:"ICUDATA", بھرا.ڈیٹا());
#اگر_ختم
                          defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_لیبل, &defLen, &sub_حالت);
                          اگر(U_SUCCESS(sub_حالت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#اگر defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> default0 %s=%s,  %s\n", 
                                    راستہ?راستہ:"ICUDATA", بھرا.ڈیٹا(), keyword, defVal.ڈیٹا(), u_errorName(sub_حالت));
#اگر_ختم
                            defLoc.copyFrom(بھرا, sub_حالت);
                          }
                        } /* end of recalculate default KW */
#اگر defined(URES_درخت_DEBUG)
                        ورنہ {
                          fprintf(stderr, "No trim0,  %s <= %s\n", defLoc.ڈیٹا(), بھرا.ڈیٹا());
                        }
#اگر_ختم
                } ورنہ {
#اگر defined(URES_درخت_DEBUG)
                    fprintf(stderr, "err=%s in %s looking for %s\n", 
                        u_errorName(sub_حالت), والد.ڈیٹا(), kwVal.ڈیٹا());
#اگر_ختم
                }
            }
        }
        
        sub_حالت = U_ZERO_ERROR;
        UBool have_ملا = جھوٹ;
        // At least for collations which may be aliased, we need to use the VALID locale
        // as the parent instead of just truncating, as long as the VALID locale is not
        // root and has a different language than the parent. Use of the VALID locale
        // here is similar to the procedure used at the end of the previous do-while loop
        // for all resource types.
        اگر (res != خالی_اشاریہ && uprv_strcmp(resName, "collations") == 0) {
            ثابت حرف *validLoc = ures_getLocaleByType(res, ULOC_VALID_LOCALE, &sub_حالت);
            اگر (U_SUCCESS(sub_حالت) && validLoc != خالی_اشاریہ && validLoc[0] != 0 && uprv_strcmp(validLoc, "root") != 0) {
                CharString validLang = ulocimp_getLanguage(validLoc, sub_حالت);
                CharString والد_Lang = ulocimp_getLanguage(والد.مقصد_String_Piece(), sub_حالت);
                اگر (U_SUCCESS(sub_حالت) && validLang != والد_Lang) {
                    // validLoc is not root and has a different language than parent, use it instead
                    ملا.clear().append(validLoc, sub_حالت);
                    have_ملا = سچ;
                }
            }
            sub_حالت = U_ZERO_ERROR;
        }
        اگر (!have_ملا) {
            ملا.copyFrom(والد, sub_حالت);
        }

        get_والد_For_Functional_Equivalent(ملا.ڈیٹا(),res,&bund1,والد);
        ures_close(res);
        sub_حالت = U_ZERO_ERROR;
    } جب(بھرا.is_خالی_خانے() && !ملا.is_خالی_خانے() && U_SUCCESS(*حالت_2));

    اگر(بھرا.is_خالی_خانے() && kwVal != defVal) {
#اگر defined(URES_درخت_DEBUG)
        fprintf(stderr, "Failed to locate kw %s - try default %s\n", kwVal.ڈیٹا(), defVal.ڈیٹا());
#اگر_ختم
        kwVal.clear().append(defVal, sub_حالت);
        والد.copyFrom(بنیاد, sub_حالت);
        ملا.copyFrom(بنیاد, sub_حالت);

        کرو { /* search for 'default' named item */
            res = ures_open(راستہ, والد.ڈیٹا(), &sub_حالت);
            اگر((sub_حالت == U_USING_FALLBACK_WARNING) && is_دستیاب) {
                *is_دستیاب = جھوٹ;
            }
            is_دستیاب = خالی_اشاریہ; /* only want to set this the first time around */
            
#اگر defined(URES_درخت_DEBUG)
            fprintf(stderr, "%s;%s -> %s (looking for default %s)\n",
                راستہ?راستہ:"ICUDATA", والد.ڈیٹا(), u_errorName(sub_حالت), kwVal.ڈیٹا());
#اگر_ختم
            اگر(U_FAILURE(sub_حالت)) {
                *حالت_2 = sub_حالت;
            } ورنہ اگر(sub_حالت == U_ZERO_ERROR) {
                ures_get_By_کلید(res,resName,&bund1, &sub_حالت);
                اگر(sub_حالت == U_ZERO_ERROR) {
                    ures_get_By_کلید(&bund1, kwVal.ڈیٹا(), &bund2, &sub_حالت);
                    اگر(sub_حالت == U_ZERO_ERROR) {
#اگر defined(URES_درخت_DEBUG)
                        fprintf(stderr, "%s;%s -> full1 %s=%s,  %s\n", راستہ?راستہ:"ICUDATA",
                            والد.ڈیٹا(), keyword, kwVal.ڈیٹا(), u_errorName(sub_حالت));
#اگر_ختم
                        اگر (والد.is_خالی_خانے()) {
                            بھرا.clear().append("root", sub_حالت);
                        } ورنہ {
                            بھرا.copyFrom(والد, sub_حالت);
                        }
                        
                        /* now, recalculate default kw if need be */
                        اگر(defLoc.length() > بھرا.length()) {
                          ثابت char16_t *defUstr;
                          int32_t defLen;
                          /* look for default item */
#اگر defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> recalculating Default1\n", 
                                    راستہ?راستہ:"ICUDATA", بھرا.ڈیٹا());
#اگر_ختم
                          defUstr = ures_get_String_By_کلید(&bund1, DEFAULT_لیبل, &defLen, &sub_حالت);
                          اگر(U_SUCCESS(sub_حالت) && defLen) {
                            defVal.clear().appendInvariantChars(defUstr, defLen, sub_حالت);
#اگر defined(URES_درخت_DEBUG)
                            fprintf(stderr, "%s;%s -> default %s=%s,  %s\n", 
                                    راستہ?راستہ:"ICUDATA", بھرا.ڈیٹا(), keyword, defVal.ڈیٹا(), u_errorName(sub_حالت));
#اگر_ختم
                            defLoc.copyFrom(بھرا, sub_حالت);
                          }
                        } /* end of recalculate default KW */
#اگر defined(URES_درخت_DEBUG)
                        ورنہ {
                          fprintf(stderr, "No trim1,  %s <= %s\n", defLoc.ڈیٹا(), بھرا.ڈیٹا());
                        }
#اگر_ختم
                    }
                }
            }
            
            sub_حالت = U_ZERO_ERROR;
            ملا.copyFrom(والد, sub_حالت);
            get_والد_For_Functional_Equivalent(ملا.ڈیٹا(),res,&bund1,والد);
            ures_close(res);
            sub_حالت = U_ZERO_ERROR;
        } جب(بھرا.is_خالی_خانے() && !ملا.is_خالی_خانے() && U_SUCCESS(*حالت_2));
    }
    
    اگر(U_SUCCESS(*حالت_2)) {
        اگر(بھرا.is_خالی_خانے()) {
#اگر defined(URES_درخت_DEBUG)
          fprintf(stderr, "Still could not load keyword %s=%s\n", keyword, kwVal.ڈیٹا());
#اگر_ختم
          *حالت_2 = U_MISSING_RESOURCE_ERROR;
        } ورنہ اگر(omitDefault) {
#اگر defined(URES_درخت_DEBUG)
          fprintf(stderr,"Trim? full=%s, defLoc=%s, found=%s\n", بھرا.ڈیٹا(), defLoc.ڈیٹا(), ملا.ڈیٹا());
#اگر_ختم        
          اگر(defLoc.length() <= بھرا.length()) {
            /* found the keyword in a *child* of where the default tag was present. */
            اگر(kwVal == defVal) { /* if the requested kw is default, */
              /* and the default is in or in an ancestor of the current locale */
#اگر defined(URES_درخت_DEBUG)
              fprintf(stderr, "Removing unneeded var %s=%s\n", keyword, kwVal.ڈیٹا());
#اگر_ختم
              kwVal.clear();
            }
          }
        }
        ملا.copyFrom(بھرا, sub_حالت);
        اگر(!kwVal.is_خالی_خانے()) {
            ملا
                .append("@", sub_حالت)
                .append(keyword, sub_حالت)
                .append("=", sub_حالت)
                .append(kwVal, sub_حالت);
        } ورنہ اگر(!omitDefault) {
            ملا
                .append("@", sub_حالت)
                .append(keyword, sub_حالت)
                .append("=", sub_حالت)
                .append(defVal, sub_حالت);
        }
    }
    /* we found the default locale - no need to repeat it.*/
    
    ures_close(&bund1);
    ures_close(&bund2);
    
    length = ملا.length();

    اگر(U_SUCCESS(*حالت_2)) {
        int32_t copy_لمبائی = uprv_min(length, نتیجہ_استعداد);
        اگر(copy_لمبائی>0) {
            ملا.extract(نتیجہ, copy_لمبائی, sub_حالت);
        }
        اگر(length == 0) {
          *حالت_2 = U_MISSING_RESOURCE_ERROR; 
        }
    } ورنہ {
        length = 0;
        نتیجہ[0]=0;
    }
    واپس u_terminateChars(نتیجہ, نتیجہ_استعداد, length, حالت_2);
}

U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_اقدار(ثابت حرف *راستہ, ثابت حرف *keyword, UError_رمز *حالت_2)
{
#تعریف_کرو اقدار_BUF_سائز 2048
#تعریف_کرو اقدار_LIST_سائز 512
    
    حرف       اقدار_Buf[اقدار_BUF_سائز];
    int32_t    اقدار_اشاریہ = 0;
    ثابت حرف *اقدار_List[اقدار_LIST_سائز];
    int32_t    اقدار_گنتی = 0;
    
    ثابت حرف *locale;
    int32_t     locLen;
    
    UEnumeration *locs = خالی_اشاریہ;
    
    UResourceBundle    شے;
    UResourceBundle    sub_شے;
    
    ures_init_ڈھیر_Object(&شے);
    ures_init_ڈھیر_Object(&sub_شے);
    locs = ures_open_دستیاب_Locales(راستہ, حالت_2);
    
    اگر(U_FAILURE(*حالت_2)) {
        ures_close(&شے);
        ures_close(&sub_شے);
        واپس خالی_اشاریہ;
    }
    
    اقدار_Buf[0]=0;
    اقدار_Buf[1]=0;

    جب ((locale = uenum_اگلا(locs, &locLen, حالت_2)) != خالی_اشاریہ) {
        UResourceBundle   *bund = خالی_اشاریہ;
        UResourceBundle   *subPtr = خالی_اشاریہ;
        UError_رمز sub_حالت = U_ZERO_ERROR; /* don't fail if a bundle is unopenable */
        bund = ures_open(راستہ, locale, &sub_حالت);
        
#اگر defined(URES_درخت_DEBUG)
        اگر(!bund || U_FAILURE(sub_حالت)) {
            fprintf(stderr, "%s-%s values: Can't open %s locale - skipping. (%s)\n", 
                راستہ?راستہ:"<ICUDATA>", keyword, locale, u_errorName(sub_حالت));
        }
#اگر_ختم
        
        ures_get_By_کلید(bund, keyword, &شے, &sub_حالت);
        
        اگر(!bund || U_FAILURE(sub_حالت)) {
#اگر defined(URES_درخت_DEBUG)
            fprintf(stderr, "%s-%s values: Can't find in %s - skipping. (%s)\n", 
                راستہ?راستہ:"<ICUDATA>", keyword, locale, u_errorName(sub_حالت));
#اگر_ختم
            ures_close(bund);
            bund = خالی_اشاریہ;
            جاری;
        }

        جب ((subPtr = ures_get_اگلا_Resource(&شے, &sub_شے, &sub_حالت)) != خالی_اشاریہ
            && U_SUCCESS(sub_حالت)) {
            ثابت حرف *k;
            int32_t i;
            k = ures_get_کلید(subPtr);

#اگر defined(URES_درخت_DEBUG)
            /* fprintf(stderr, "%s | %s | %s | %s\n", path?path:"<ICUDATA>", keyword, locale, k); */
#اگر_ختم
            اگر(k == خالی_اشاریہ || *k == 0 ||
                    uprv_strcmp(k, DEFAULT_لیبل) == 0 || uprv_strncmp(k, "private-", 8) == 0) {
                // empty or "default" or unlisted type
                جاری;
            }
            برائے(i=0; i<اقدار_گنتی; i++) {
                اگر(!uprv_strcmp(اقدار_List[i],k)) {
                    k = خالی_اشاریہ; /* found duplicate */
                    توڑو;
                }
            }
            اگر(k != خالی_اشاریہ) {
                int32_t kLen = (int32_t)uprv_strlen(k);
                اگر((اقدار_گنتی >= (اقدار_LIST_سائز-1)) ||       /* no more space in list .. */
                    ((اقدار_اشاریہ+kLen+1+1) >= اقدار_BUF_سائز)) { /* no more space in buffer (string + 2 nulls) */
                    *حالت_2 = U_ILLEGAL_دلیل_ERROR; /* out of space.. */
                } ورنہ {
                    uprv_strcpy(اقدار_Buf+اقدار_اشاریہ, k);
                    اقدار_List[اقدار_گنتی++] = اقدار_Buf+اقدار_اشاریہ;
                    اقدار_اشاریہ += kLen;
#اگر defined(URES_درخت_DEBUG)
                    fprintf(stderr, "%s | %s | %s | [%s]   (UNIQUE)\n",
                        راستہ?راستہ:"<ICUDATA>", keyword, locale, k);
#اگر_ختم
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
#اگر defined(URES_درخت_DEBUG)
    fprintf(stderr, "%s:  size %d, #%d\n", u_errorName(*حالت_2), 
        اقدار_اشاریہ, اقدار_گنتی);
#اگر_ختم
    واپس uloc_openKeywordList(اقدار_Buf, اقدار_اشاریہ, حالت_2);
}
#اگر 0
/* This code isn't needed, and given the documentation warnings the implementation is suspect */
U_CAPI UBool U_EXPORT2
ures_equal(ثابت UResourceBundle* res1, ثابت UResourceBundle* res2){
    اگر(res1==خالی_اشاریہ || res2==خالی_اشاریہ){
        واپس res1==res2; /* pointer comparison */
    }
    اگر(res1->f_کلید==خالی_اشاریہ||  res2->f_کلید==خالی_اشاریہ){
        واپس (res1->f_کلید==res2->f_کلید);
    }ورنہ{
        اگر(uprv_strcmp(res1->f_کلید, res2->f_کلید)!=0){
            واپس جھوٹ;
        }
    }
    اگر(uprv_strcmp(res1->f_ڈیٹا->fName, res2->f_ڈیٹا->fName)!=0){
        واپس جھوٹ;
    }
    اگر(res1->f_ڈیٹا->f_راستہ == خالی_اشاریہ||  res2->f_ڈیٹا->f_راستہ==خالی_اشاریہ){
        واپس (res1->f_ڈیٹا->f_راستہ == res2->f_ڈیٹا->f_راستہ);
    }ورنہ{
        اگر(uprv_strcmp(res1->f_ڈیٹا->f_راستہ, res2->f_ڈیٹا->f_راستہ)!=0){
            واپس جھوٹ;
        }
    }
    اگر(uprv_strcmp(res1->f_ڈیٹا->f_والد->fName, res2->f_ڈیٹا->f_والد->fName)!=0){
        واپس جھوٹ;
    }
    اگر(uprv_strcmp(res1->f_ڈیٹا->f_والد->f_راستہ, res2->f_ڈیٹا->f_والد->f_راستہ)!=0){
        واپس جھوٹ;
    }
    اگر(uprv_strncmp(res1->f_Res_راستہ, res2->f_Res_راستہ, res1->f_Res_راستہ_Len)!=0){
        واپس جھوٹ;
    }
    اگر(res1->fRes != res2->fRes){
        واپس جھوٹ;
    }
    واپس سچ;
}
U_CAPI UResourceBundle* U_EXPORT2
ures_clone(ثابت UResourceBundle* res, UError_رمز* حالت_2){
    UResourceBundle* bundle = خالی_اشاریہ;
    UResourceBundle* ret = خالی_اشاریہ;
    اگر(U_FAILURE(*حالت_2) || res == خالی_اشاریہ){
        واپس خالی_اشاریہ;
    }
    bundle = ures_open(res->f_ڈیٹا->f_راستہ, res->f_ڈیٹا->fName, حالت_2);
    اگر(res->f_Res_راستہ!=خالی_اشاریہ){
        ret = ures_ڈھونڈو_Sub_Resource(bundle, res->f_Res_راستہ, خالی_اشاریہ, حالت_2);
        ures_close(bundle);
    }ورنہ{
        ret = bundle;
    }
    واپس ret;
}
U_CAPI ثابت UResourceBundle* U_EXPORT2
ures_get_والد_Bundle(ثابت UResourceBundle* res){
    اگر(res==خالی_اشاریہ){
        واپس خالی_اشاریہ;
    }
    واپس res->f_والد_Res;
}
#اگر_ختم

U_CAPI خالی U_EXPORT2
ures_get_نسخہ_By_کلید(ثابت UResourceBundle* res, ثابت حرف *کلید, UVersionInfo ver, UError_رمز *حالت_2) {
  ثابت char16_t *str;
  int32_t len;
  str = ures_get_String_By_کلید(res, کلید, &len, حالت_2);
  اگر(U_SUCCESS(*حالت_2)) {
    u_نسخہ_From_UString(ver, str);
  } 
}

/* eof */
