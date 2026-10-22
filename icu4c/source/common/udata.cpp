// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  udata.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999oct25
*   created by: Markus W. Scherer
*/

#ضمن "unicode/utypes.h"  /* U_PLATFORM etc. */

#جے_تعریف_شدہ __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#جے_انت

#ضمن "unicode/putil.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uversion.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "mutex.h"
#ضمن "putilimp.h"
#ضمن "restrace.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "ucmndata.h"
#ضمن "udatamem.h"
#ضمن "uhash.h"
#ضمن "umapfile.h"
#ضمن "umutex.h"

/***********************************************************************
*
*   Notes on the organization of the ICU data implementation
*
*      All of the public API is defined in udata.h
*
*      The implementation is split into several files...
*
*         - udata.c  (this file) contains higher level code that knows about
*                     the search paths for locating data, caching opened data, etc.
*
*         - umapfile.c  contains the low level platform-specific code for actually loading
*                     (memory mapping, file reading, whatever) data into memory.
*
*         - ucmndata.c  deals with the tables of contents of ICU data items within
*                     an ICU common format data file.  The implementation includes
*                     an abstract interface and support for multiple TOC formats.
*                     All knowledge of any specific TOC format is encapsulated here.
*
*         - udatamem.c has code for managing UDataMemory structs.  These are little
*                     descriptor objects for blocks of memory holding ICU data of
*                     various types.
*/

/* configuration ---------------------------------------------------------- */

/* If you are excruciatingly bored turn this on .. */
/* #define UDATA_DEBUG 1 */

#إذا defined(UDATA_DEBUG)
#   include <stdio.h>
#جے_انت

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
ساكن UData_حافظہ *udata_لبھو_Cached_انکڑے(ثابت حرف *رستہ, UError_کوڈ &err);

/***********************************************************************
*
*    static (Global) data
*
************************************************************************/

/*
 * Pointers to the common ICU data.
 *
 * We store multiple pointers to ICU data packages and iterate through them
 * when looking for a data item.
 *
 * It is possible to combine this with dependency inversion:
 * One or more data package libraries may export
 * functions that each return a pointer to their piece of the ICU data,
 * and this file would import them as weak functions, without a
 * strong linker dependency from the common library on the data library.
 *
 * Then we can have applications depend on only that part of ICU's data
 * that they really need, reducing the size of binaries that take advantage
 * of this.
 */
ساكن UData_حافظہ *gCommonICUDataArray[10] = { خالی_سوچک };   // Access protected by icu global mutex.

ساكن u_atomic_int32_t g_Have_Tried_منزل_لدو_Common_انکڑے {0};  //  See extendICUData().

ساكن UHashtable  *g_Common_انکڑے_Cache = خالی_سوچک;  /* Global hash table of opened ICU data files.  */
ساكن icu::UInitOnce g_Common_انکڑے_Cache_Init_Once {};

#إذا !defined(ICU_انکڑے_DIR_WINDOWS)
ساكن UDataFileAccess  g_انکڑے_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#وإلا
// If we are using the Windows data directory, then look in one spot only.
ساكن UDataFileAccess  g_انکڑے_File_Access = UDATA_NO_FILES;
#جے_انت

ساكن UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    إذا (g_Common_انکڑے_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_انکڑے_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_انکڑے_Cache = خالی_سوچک;        /*   Cleanup is not thread safe.                */
    }
    g_Common_انکڑے_Cache_Init_Once.reset();

    لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != خالی_سوچک; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = خالی_سوچک;
    }
    g_Have_Tried_منزل_لدو_Common_انکڑے = 0;

    أرجع سچ;                   /* Everything was cleaned up */
}

ساكن UBool U_CALLCONV
لبھو_Common_ICUData_By_Name(ثابت حرف *وچ_Basename, UError_کوڈ &err)
{
    UBool لبھیا = خطأ;
    int32_t i;

    UData_حافظہ  *p_انکڑے = udata_لبھو_Cached_انکڑے(وچ_Basename, err);
    إذا (U_FAILURE(err) || p_انکڑے == خالی_سوچک)
        أرجع خطأ;

    {
        Mutex جندرا;
        لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            إذا ((gCommonICUDataArray[i] != خالی_سوچک) && (gCommonICUDataArray[i]->p_سرخی == p_انکڑے->p_سرخی)) {
                /* The data pointer is already in the array. */
                لبھیا = سچ;
                اكسر;
            }
        }
    }
    أرجع لبھیا;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
ساكن UBool
سیٹ_Common_ICUData(UData_حافظہ *p_انکڑے,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_کوڈ *pErr)
{
    UData_حافظہ  *new_Common_انکڑے = UData_حافظہ_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = خطأ;
    إذا (U_FAILURE(*pErr)) {
        أرجع خطأ;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_سونپو(new_Common_انکڑے, p_انکڑے);
    umtx_جندرا(خالی_سوچک);
    لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        إذا (gCommonICUDataArray[i] == خالی_سوچک) {
            gCommonICUDataArray[i] = new_Common_انکڑے;
            didUpdate = سچ;
            اكسر;
        } وإلا إذا (gCommonICUDataArray[i]->p_سرخی == p_انکڑے->p_سرخی) {
            /* The same data pointer is already in the array. */
            اكسر;
        }
    }
    umtx_unlock(خالی_سوچک);

    إذا (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    إذا (didUpdate) {
        ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } وإلا {
        uprv_free(new_Common_انکڑے);
    }
    أرجع didUpdate;
}

#إذا !defined(ICU_انکڑے_DIR_WINDOWS)

ساكن UBool
سیٹ_Common_ICUData_اشارہ_گر(ثابت فراغ *p_انکڑے, UBool /*warn*/, UError_کوڈ *p_Error_کوڈ) {
    UData_حافظہ t_انکڑے;
    UData_حافظہ_init(&t_انکڑے);
    UData_حافظہ_سیٹ_انکڑے(&t_انکڑے, p_انکڑے);
    udata_check_Common_انکڑے(&t_انکڑے, p_Error_کوڈ);
    أرجع سیٹ_Common_ICUData(&t_انکڑے, خطأ, p_Error_کوڈ);
}

#جے_انت

ساكن ثابت حرف *
لبھو_Basename(ثابت حرف *رستہ) {
    ثابت حرف *basename=uprv_strrchr(رستہ, U_FILE_SEP_CHAR);
    إذا(basename==خالی_سوچک) {
        أرجع رستہ;
    } وإلا {
        أرجع basename+1;
    }
}

#جے_تعریف_شدہ UDATA_DEBUG
ساكن ثابت حرف *
package_Name_From_رستہ(ثابت حرف *رستہ)
{
    إذا((رستہ == خالی_سوچک) || (*رستہ == 0)) {
        أرجع U_ICUDATA_NAME;
    }

    رستہ = لبھو_Basename(رستہ);

    إذا((رستہ == خالی_سوچک) || (*رستہ == 0)) {
        أرجع U_ICUDATA_NAME;
    }

    أرجع رستہ;
}
#جے_انت

/*----------------------------------------------------------------------*
 *                                                                      *
 *   Cache for common data                                              *
 *      Functions for looking up or adding entries to a cache of        *
 *      data that has been previously opened.  Avoids a potentially     *
 *      expensive operation of re-opening the data for subsequent       *
 *      uses.                                                           *
 *                                                                      *
 *      Data remains cached for the duration of the process.            *
 *                                                                      *
 *----------------------------------------------------------------------*/

تعريفنوع بنية انکڑے_Cache_Element {
    حرف          *name;
    UData_حافظہ   *شے;
} انکڑے_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
ساكن فراغ U_CALLCONV انکڑے_Cache_Element_deleter(فراغ *pDCEl) {
    انکڑے_Cache_Element* p = static_cast<انکڑے_Cache_Element*>(pDCEl);
    udata_close(p->شے);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

ساكن فراغ U_CALLCONV udata_init_Hash_جدول(UError_کوڈ &err) {
    U_ASSERT(g_Common_انکڑے_Cache == خالی_سوچک);
    g_Common_انکڑے_Cache = uhash_open(uhash_hashChars, uhash_compareChars, خالی_سوچک, &err);
    إذا (U_FAILURE(err)) {
       أرجع;
    }
    U_ASSERT(g_Common_انکڑے_Cache != خالی_سوچک);
    uhash_سیٹ_قدر_Deleter(g_Common_انکڑے_Cache, انکڑے_Cache_Element_deleter);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
ساكن UHashtable *udata_get_Hash_جدول(UError_کوڈ &err) {
    umtx_initOnce(g_Common_انکڑے_Cache_Init_Once, &udata_init_Hash_جدول, err);
    أرجع g_Common_انکڑے_Cache;
}



ساكن UData_حافظہ *udata_لبھو_Cached_انکڑے(ثابت حرف *رستہ, UError_کوڈ &err)
{
    UHashtable        *htable;
    UData_حافظہ       *retVal = خالی_سوچک;
    انکڑے_Cache_Element  *el;
    ثابت حرف        *ادھار_Name;

    htable = udata_get_Hash_جدول(err);
    إذا (U_FAILURE(err)) {
        أرجع خالی_سوچک;
    }

    ادھار_Name = لبھو_Basename(رستہ);   /* Cache remembers only the base name, not the full path. */
    umtx_جندرا(خالی_سوچک);
    el = static_cast<انکڑے_Cache_Element*>(uhash_get(htable, ادھار_Name));
    umtx_unlock(خالی_سوچک);
    إذا (el != خالی_سوچک) {
        retVal = el->شے;
    }
#جے_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", ادھار_Name, (فراغ*) retVal);
#جے_انت
    أرجع retVal;
}


ساكن UData_حافظہ *udata_cache_انکڑے_شے(ثابت حرف *رستہ, UData_حافظہ *شے, UError_کوڈ *pErr) {
    انکڑے_Cache_Element *newElement;
    ثابت حرف       *ادھار_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    انکڑے_Cache_Element *old_قدر = خالی_سوچک;
    UError_کوڈ        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_جدول(*pErr);
    إذا (U_FAILURE(*pErr)) {
        أرجع خالی_سوچک;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<انکڑے_Cache_Element*>(uprv_malloc(حجم(انکڑے_Cache_Element)));
    إذا (newElement == خالی_سوچک) {
        *pErr = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    newElement->شے = UData_حافظہ_create_New_Instance(pErr);
    إذا (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        أرجع خالی_سوچک;
    }
    UDatamemory_سونپو(newElement->شے, شے);

    ادھار_Name = لبھو_Basename(رستہ);
    nameLen = static_cast<int32_t>(uprv_strlen(ادھار_Name));
    newElement->name = static_cast<حرف*>(uprv_malloc(nameLen + 1));
    إذا (newElement->name == خالی_سوچک) {
        *pErr = U_حافظہ_ونڈ_ERROR;
        uprv_free(newElement->شے);
        uprv_free(newElement);
        أرجع خالی_سوچک;
    }
    uprv_strcpy(newElement->name, ادھار_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_جندرا(خالی_سوچک);
    old_قدر = static_cast<انکڑے_Cache_Element*>(uhash_get(htable, رستہ));
    إذا (old_قدر != خالی_سوچک) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    وإلا {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(خالی_سوچک);

#جے_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (فراغ*) newElement->شے, u_errorName(subErr), (فراغ*) newElement->شے->vFuncs);
#جے_انت

    إذا (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->شے);
        uprv_free(newElement);
        أرجع old_قدر ? old_قدر->شے : خالی_سوچک;
    }

    أرجع newElement->شے;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

صنف UData_رستہ_Iterator
{
عام:
    UData_رستہ_Iterator(ثابت حرف *رستہ, ثابت حرف *pkg,
                      ثابت حرف *شے, ثابت حرف *suffix, UBool do_Check_آخری_Four,
                      UError_کوڈ *p_Error_کوڈ);
    ثابت حرف *next(UError_کوڈ *p_Error_کوڈ);

خاص:
    ثابت حرف *رستہ;                              /* working path (u_icudata_Dir) */
    ثابت حرف *اگلا_رستہ;                          /* path following this one */
    ثابت حرف *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  شے_رستہ;                          /* path passed in with item name */
    CharString  رستہ_بفر;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       check_آخری_Four;                     /* if true then allow paths such as '/foo/myapp.dat'
                                                    * to match, checks last 4 chars of suffix with
                                                    * last 4 of path, then previous chars. */
};

/**
 * @param iter    The iterator to be initialized. Its current state does not matter.
 * @param inPath  The full pathname to be iterated over.  If nullptr, defaults to U_ICUDATA_NAME 
 * @param pkg     Package which is being searched for, ex "icudt28l".  Will ignore leaf directories such as /icudt28l 
 * @param item    Item to be searched for.  Can include full path, such as /a/b/foo.dat 
 * @param inSuffix  Optional item suffix, if not-null (ex. ".dat") then 'path' can contain 'item' explicitly.
 *             Ex:   'stuff.dat' would be found in '/a/foo:/tmp/stuff.dat:/bar/baz' as item #2.   
 *                   '/blarg/stuff.dat' would also be found.
 *  Note: inSuffix may also be the 'item' being searched for as well, (ex: "ibm-5348_P100-1997.cnv"), in which case 
 *        the 'item' parameter is often the same as pkg. (Though sometimes might have a tree part as well, ex: "icudt62l-curr").
 */
UData_رستہ_Iterator::UData_رستہ_Iterator(ثابت حرف *وچ_رستہ, ثابت حرف *pkg,
                                     ثابت حرف *شے, ثابت حرف *وچ_Suffix, UBool do_Check_آخری_Four,
                                     UError_کوڈ *p_Error_کوڈ)
{
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", وچ_Suffix, وچ_رستہ);
#جے_انت
    /** Path **/
    إذا(وچ_رستہ == خالی_سوچک) {
        رستہ = u_get_انکڑے_Directory();
    } وإلا {
        رستہ = وچ_رستہ;
    }

    /** Package **/
    إذا(pkg != خالی_سوچک) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_کوڈ).append(pkg, *p_Error_کوڈ);
#جے_تعریف_شدہ UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.انکڑے(), packageStub.length());
#جے_انت
    }

    /** Item **/
    basename = لبھو_Basename(شے);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    إذا(basename == شے) {
        اگلا_رستہ = رستہ;
    } وإلا {
        شے_رستہ.append(شے, static_cast<int32_t>(basename - شے), *p_Error_کوڈ);
        اگلا_رستہ = شے_رستہ.انکڑے();
    }
#جے_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", وچ_Suffix, (فراغ*) وچ_Suffix);
#جے_انت

    /** Suffix  **/
    إذا(وچ_Suffix != خالی_سوچک) {
        suffix = وچ_Suffix;
    } وإلا {
        suffix = "";
    }

    check_آخری_Four = do_Check_آخری_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#جے_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            شے,
            رستہ,
            basename,
            suffix.انکڑے(),
            شے_رستہ.انکڑے(),
            اگلا_رستہ,
            check_آخری_Four?"true":"false");
#جے_انت
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ثابت حرف *UData_رستہ_Iterator::next(UError_کوڈ *p_Error_کوڈ)
{
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    ثابت حرف *current_رستہ = خالی_سوچک;
    int32_t     رستہ_Len = 0;
    ثابت حرف *رستہ_Basename;

    افعل
    {
        إذا( اگلا_رستہ == خالی_سوچک ) {
            اكسر;
        }
        current_رستہ = اگلا_رستہ;

        إذا(اگلا_رستہ == شے_رستہ.انکڑے()) { /* we were processing item's path. */
            اگلا_رستہ = رستہ; /* start with regular path next tm. */
            رستہ_Len = static_cast<int32_t>(uprv_strlen(current_رستہ));
        } وإلا {
            /* fix up next for next time */
            اگلا_رستہ = uprv_strchr(current_رستہ, U_رستہ_SEP_CHAR);
            إذا(اگلا_رستہ == خالی_سوچک) {
                /* segment: entire path */
                رستہ_Len = static_cast<int32_t>(uprv_strlen(current_رستہ));
            } وإلا {
                /* segment: until next segment */
                رستہ_Len = static_cast<int32_t>(اگلا_رستہ - current_رستہ);
                /* skip divider */
                اگلا_رستہ ++;
            }
        }

        إذا(رستہ_Len == 0) {
            استمر;
        }

#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_رستہ);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            لكل(qqq=0;qqq<رستہ_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#جے_انت
        رستہ_بفر.clear().append(current_رستہ, رستہ_Len, *p_Error_کوڈ);

        /* check for .dat files */
        رستہ_Basename = لبھو_Basename(رستہ_بفر.انکڑے());

        إذا(check_آخری_Four && 
           (رستہ_Len>=4) &&
           uprv_strncmp(رستہ_بفر.انکڑے() +(رستہ_Len-4), suffix.انکڑے(), 4)==0 && /* suffix matches */
           uprv_strncmp(لبھو_Basename(رستہ_بفر.انکڑے()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(رستہ_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#جے_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.انکڑے(), رستہ_بفر.انکڑے());
#جے_انت
            /* do nothing */
        }
        وإلا 
        {       /* regular dir path */
            إذا(رستہ_بفر[رستہ_Len-1] != U_FILE_SEP_CHAR) {
                إذا((رستہ_Len>=4) &&
                   uprv_strncmp(رستہ_بفر.انکڑے()+(رستہ_Len-4), ".dat", 4) == 0)
                {
#جے_تعریف_شدہ UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", رستہ_بفر.انکڑے());
#جے_انت
                    استمر;
                }

                /* Check if it is a directory with the same name as our package */
                إذا(!packageStub.is_خالی_تھاواں() &&
                   (رستہ_Len > packageStub.length()) &&
                   !uprv_strcmp(رستہ_بفر.انکڑے() + رستہ_Len - packageStub.length(), packageStub.انکڑے())) {
#جے_تعریف_شدہ UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.انکڑے(), basename, basenameLen);
#جے_انت
                  رستہ_بفر.truncate(رستہ_Len - packageStub.length());
                }
                رستہ_بفر.append(U_FILE_SEP_CHAR, *p_Error_کوڈ);
            }

            /* + basename */
            رستہ_بفر.append(packageStub.انکڑے()+1, packageStub.length()-1, *p_Error_کوڈ);

            إذا (!suffix.empty())  /* tack on suffix */
            {
                إذا (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    رستہ_بفر.ensure_سرے_With_File_Separator(*p_Error_کوڈ);
                }
                رستہ_بفر.append(suffix, *p_Error_کوڈ);
            }
        }

#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", رستہ_بفر.انکڑے());
#جے_انت

        أرجع رستہ_بفر.انکڑے();

    } طالما(رستہ);

    /* fell way off the end */
    أرجع خالی_سوچک;
}

U_NAMESPACE_سرا

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#إذا !defined(ICU_انکڑے_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
خارجي "C" ثابت انکڑے_سرخی U_انکڑے_API U_ICUDATA_اندراج_نقطہ;
#جے_انت

/*
 * This would be a good place for weak-linkage declarations of
 * partial-data-library access functions where each returns a pointer
 * to its data package, if it is linked in.
 */
/*
extern const void *uprv_getICUData_collation() ATTRIBUTE_WEAK;
extern const void *uprv_getICUData_conversion() ATTRIBUTE_WEAK;
*/

/*----------------------------------------------------------------------*
 *                                                                      *
 *   openCommonData   Attempt to open a common format (.dat) file       *
 *                    Map it into memory (if it's not there already)    *
 *                    and return a UDataMemory object for it.           *
 *                                                                      *
 *                    If the requested data is already open and cached  *
 *                       just return the cached UDataMem object.        *
 *                                                                      *
 *----------------------------------------------------------------------*/
ساكن UData_حافظہ *
open_Common_انکڑے(ثابت حرف *رستہ,          /*  Path from OpenChoice?          */
               int32_t common_انکڑے_اشاریہ,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_کوڈ *p_Error_کوڈ)
{
    UData_حافظہ t_انکڑے;
    ثابت حرف *رستہ_بفر;
    ثابت حرف *وچ_Basename;

    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    UData_حافظہ_init(&t_انکڑے);

    /* ??????? TODO revisit this */ 
    إذا (common_انکڑے_اشاریہ >= 0) {
        /* "mini-cache" for common ICU data */
        إذا(common_انکڑے_اشاریہ >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            أرجع خالی_سوچک;
        }
        {
            Mutex جندرا;
            إذا(gCommonICUDataArray[common_انکڑے_اشاریہ] != خالی_سوچک) {
                أرجع gCommonICUDataArray[common_انکڑے_اشاریہ];
            }
#إذا !defined(ICU_انکڑے_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            لكل(i = 0; i < common_انکڑے_اشاریہ; ++i) {
                إذا(gCommonICUDataArray[i]->p_سرخی == &U_ICUDATA_اندراج_نقطہ) {
                    /* The linked-in data is already in the list. */
                    أرجع خالی_سوچک;
                }
            }
#جے_انت
        }

        /* Add the linked-in data to the list. */
        /*
         * This is where we would check and call weakly linked partial-data-library
         * access functions.
         */
        /*
        if (uprv_getICUData_collation) {
            setCommonICUDataPointer(uprv_getICUData_collation(), false, pErrorCode);
        }
        if (uprv_getICUData_conversion) {
            setCommonICUDataPointer(uprv_getICUData_conversion(), false, pErrorCode);
        }
        */
#إذا !defined(ICU_انکڑے_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        سیٹ_Common_ICUData_اشارہ_گر(&U_ICUDATA_اندراج_نقطہ, خطأ, p_Error_کوڈ);
        {
            Mutex جندرا;
            أرجع gCommonICUDataArray[common_انکڑے_اشاریہ];
        }
#جے_انت
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    وچ_Basename = لبھو_Basename(رستہ);
#جے_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", وچ_Basename);
#جے_انت

    إذا(*وچ_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", رستہ);
#جے_انت
        إذا (U_SUCCESS(*p_Error_کوڈ)) {
            *p_Error_کوڈ=U_FILE_ACCESS_ERROR;
        }
        أرجع خالی_سوچک;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_حافظہ  *انکڑے_منزل_Return = udata_لبھو_Cached_انکڑے(وچ_Basename, *p_Error_کوڈ);
    إذا (انکڑے_منزل_Return != خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع انکڑے_منزل_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_رستہ_Iterator iter(u_get_انکڑے_Directory(), وچ_Basename, رستہ, ".dat", سچ, p_Error_کوڈ);

    طالما ((UData_حافظہ_is_Loaded(&t_انکڑے)==خطأ) && (رستہ_بفر = iter.next(p_Error_کوڈ)) != خالی_سوچک)
    {
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", رستہ_بفر);
#جے_انت
        uprv_mapFile(&t_انکڑے, رستہ_بفر, p_Error_کوڈ);
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_حافظہ_is_Loaded(&t_انکڑے)?"LOADED":"not loaded");
#جے_انت
    }
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا (!UData_حافظہ_is_Loaded(&t_انکڑے)) {
        /* no common data */
        *p_Error_کوڈ=U_FILE_ACCESS_ERROR;
        أرجع خالی_سوچک;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_انکڑے(&t_انکڑے, p_Error_کوڈ);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    أرجع udata_cache_انکڑے_شے(وچ_Basename, &t_انکڑے, p_Error_کوڈ);
}


/*----------------------------------------------------------------------*
 *                                                                      *
 *   extendICUData   If the full set of ICU data was not loaded at      *
 *                   program startup, load it now.  This function will  *
 *                   be called when the lookup of an ICU data item in   *
 *                   the common ICU data fails.                         *
 *                                                                      *
 *                   return true if new data is loaded, false otherwise.*
 *                                                                      *
 *----------------------------------------------------------------------*/
ساكن UBool extendICUData(UError_کوڈ *pErr)
{
    UData_حافظہ   *p_انکڑے;
    UData_حافظہ   copyPData;
    UBool         didUpdate = خطأ;

    /*
     * There is a chance for a race condition here.
     * Normally, ICU data is loaded from a DLL or via mmap() and
     * setCommonICUData() will detect if the same address is set twice.
     * If ICU is built with data loading via fread() then the address will
     * be different each time the common data is loaded and we may add
     * multiple copies of the data.
     * In this case, use a mutex to prevent the race.
     * Use a specific mutex to avoid nested locks of the global mutex.
     */
#إذا MAP_IMPLEMENTATION==MAP_STDIO
    ساكن UMutex extendICUDataMutex;
    umtx_جندرا(&extendICUDataMutex);
#جے_انت
    إذا(!umtx_لدو_Acquire(g_Have_Tried_منزل_لدو_Common_انکڑے)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_انکڑے = open_Common_انکڑے(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_حافظہ_init(&copyPData);
       إذا(p_انکڑے != خالی_سوچک) {
          UDatamemory_سونپو(&copyPData, p_انکڑے);
          copyPData.map = خالی_سوچک;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = خالی_سوچک; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              سیٹ_Common_ICUData(&copyPData,/*  The new common data.                                */
                       خطأ,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_منزل_لدو_Common_انکڑے, 1);
    }

    didUpdate = لبھو_Common_ICUData_By_Name(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#إذا MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#جے_انت
    أرجع didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI فراغ U_EXPORT2
udata_سیٹ_Common_انکڑے(ثابت فراغ *انکڑے, UError_کوڈ *p_Error_کوڈ) {
    UData_حافظہ انکڑے_حافظہ;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }

    إذا(انکڑے==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    /* set the data pointer and test for validity */
    UData_حافظہ_init(&انکڑے_حافظہ);
    UData_حافظہ_سیٹ_انکڑے(&انکڑے_حافظہ, انکڑے);
    udata_check_Common_انکڑے(&انکڑے_حافظہ, p_Error_کوڈ);
    إذا (U_FAILURE(*p_Error_کوڈ)) {أرجع;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    سیٹ_Common_ICUData(&انکڑے_حافظہ, سچ, p_Error_کوڈ);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI فراغ U_EXPORT2
udata_سیٹ_App_انکڑے(ثابت حرف *رستہ, ثابت فراغ *انکڑے, UError_کوڈ *err)
{
    UData_حافظہ     udm;

    إذا(err==خالی_سوچک || U_FAILURE(*err)) {
        أرجع;
    }
    إذا(انکڑے==خالی_سوچک) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    UData_حافظہ_init(&udm);
    UData_حافظہ_سیٹ_انکڑے(&udm, انکڑے);
    udata_check_Common_انکڑے(&udm, err);
    udata_cache_انکڑے_شے(رستہ, &udm, err);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  checkDataItem     Given a freshly located/loaded data item, either        *
 *                    an entry in a common file or a separately loaded file,  *
 *                    sanity check its header, and see if the data is         *
 *                    acceptable to the app.                                  *
 *                    If the data is good, create and return a UDataMemory    *
 *                    object that can be returned to the application.         *
 *                    Return nullptr on any sort of failure.                     *
 *                                                                            *
 *----------------------------------------------------------------------------*/
ساكن UData_حافظہ *
check_انکڑے_شے
(
 ثابت انکڑے_سرخی         *p_سرخی,         /* The data item to be checked.                */
 UData_حافظہ_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 فراغ                     *context,         /*   pass-thru param for above.                */
 ثابت حرف               *type,            /*   pass-thru param for above.                */
 ثابت حرف               *name,            /*   pass-thru param for above.                */
 UError_کوڈ               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_کوڈ               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_حافظہ  *r_انکڑے_Mem = خالی_سوچک;          /* the new UDataMemory, to be returned.        */

    إذا (U_FAILURE(*fatalErr)) {
        أرجع خالی_سوچک;
    }

    إذا(p_سرخی->انکڑے_سرخی_2.magic1==0xda &&
        p_سرخی->انکڑے_سرخی_2.magic2==0x27 &&
        (isAcceptable==خالی_سوچک || isAcceptable(context, type, name, &p_سرخی->info))
    ) {
        r_انکڑے_Mem=UData_حافظہ_create_New_Instance(fatalErr);
        إذا (U_FAILURE(*fatalErr)) {
            أرجع خالی_سوچک;
        }
        r_انکڑے_Mem->p_سرخی = p_سرخی;
    } وإلا {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    أرجع r_انکڑے_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساكن UData_حافظہ *do_لدو_From_Individual_Files(ثابت حرف *pkgName, 
        ثابت حرف *انکڑے_رستہ, ثابت حرف *toc_اندراج_رستہ_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *رستہ, ثابت حرف *type, ثابت حرف *name,
             UData_حافظہ_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کوڈ *sub_Error_کوڈ,
             UError_کوڈ *p_Error_کوڈ)
{
    ثابت حرف         *رستہ_بفر;
    UData_حافظہ         انکڑے_حافظہ;
    UData_حافظہ *p_اندراج_انکڑے;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_رستہ_Iterator iter(انکڑے_رستہ, pkgName, رستہ, toc_اندراج_رستہ_Suffix, خطأ, p_Error_کوڈ);

    طالما ((رستہ_بفر = iter.next(p_Error_کوڈ)) != خالی_سوچک)
    {
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", رستہ_بفر);
#جے_انت
        إذا (uprv_mapFile(&انکڑے_حافظہ, رستہ_بفر, p_Error_کوڈ))
        {
            p_اندراج_انکڑے = check_انکڑے_شے(انکڑے_حافظہ.p_سرخی, isAcceptable, context, type, name, sub_Error_کوڈ, p_Error_کوڈ);
            إذا (p_اندراج_انکڑے != خالی_سوچک) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_اندراج_انکڑے->mapAddr = انکڑے_حافظہ.mapAddr;
                p_اندراج_انکڑے->map     = انکڑے_حافظہ.map;
                p_اندراج_انکڑے->length  = انکڑے_حافظہ.length;

#جے_تعریف_شدہ UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", رستہ_بفر);
#جے_انت
                أرجع p_اندراج_انکڑے;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&انکڑے_حافظہ);

            /* If we had a nasty error, bail out completely.  */
            إذا (U_FAILURE(*p_Error_کوڈ)) {
                أرجع خالی_سوچک;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        }
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_حافظہ_is_Loaded(&انکڑے_حافظہ)?"LOADED":"not loaded");
#جے_انت
    }
    أرجع خالی_سوچک;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساكن UData_حافظہ *do_لدو_From_Common_انکڑے(UBool isICUData, ثابت حرف * /*pkgName*/, 
        ثابت حرف * /*dataPath*/, ثابت حرف * /*tocEntryPathSuffix*/, ثابت حرف *toc_اندراج_Name,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *رستہ, ثابت حرف *type, ثابت حرف *name,
             UData_حافظہ_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کوڈ *sub_Error_کوڈ,
             UError_کوڈ *p_Error_کوڈ)
{
    UData_حافظہ        *p_اندراج_انکڑے;
    ثابت انکڑے_سرخی   *p_سرخی;
    UData_حافظہ        *p_Common_انکڑے;
    int32_t            common_انکڑے_اشاریہ;
    UBool              checkedExtendedICUData = خطأ;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    لكل (common_انکڑے_اشاریہ = isICUData ? 0 : -1;;) {
        p_Common_انکڑے=open_Common_انکڑے(رستہ, common_انکڑے_اشاریہ, sub_Error_کوڈ); /** search for pkg **/

        إذا(U_SUCCESS(*sub_Error_کوڈ) && p_Common_انکڑے!=خالی_سوچک) {
            int32_t length;

            /* look up the data piece in the common data */
            p_سرخی=p_Common_انکڑے->vFuncs->Lookup(p_Common_انکڑے, toc_اندراج_Name, &length, sub_Error_کوڈ);
#جے_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_اندراج_Name, (فراغ*) p_سرخی, u_errorName(*sub_Error_کوڈ));
#جے_انت

            إذا(p_سرخی!=خالی_سوچک) {
                p_اندراج_انکڑے = check_انکڑے_شے(p_سرخی, isAcceptable, context, type, name, sub_Error_کوڈ, p_Error_کوڈ);
#جے_تعریف_شدہ UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (فراغ*) p_اندراج_انکڑے);
#جے_انت
                إذا (U_FAILURE(*p_Error_کوڈ)) {
                    أرجع خالی_سوچک;
                }
                إذا (p_اندراج_انکڑے != خالی_سوچک) {
                    p_اندراج_انکڑے->length = length;
                    أرجع p_اندراج_انکڑے;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        إذا (*sub_Error_کوڈ == U_حافظہ_ونڈ_ERROR) {
            *p_Error_کوڈ = *sub_Error_کوڈ;
            أرجع خالی_سوچک;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        إذا (!isICUData) {
            أرجع خالی_سوچک;
        } وإلا إذا (p_Common_انکڑے != خالی_سوچک) {
            ++common_انکڑے_اشاریہ;  /* try the next data package */
        } وإلا إذا ((!checkedExtendedICUData) && extendICUData(sub_Error_کوڈ)) {
            checkedExtendedICUData = سچ;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } وإلا {
            أرجع خالی_سوچک;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
ساكن UBool is_ویلا_Zone_File(ثابت حرف *name, ثابت حرف *type) {
    أرجع ((uprv_strcmp(type, "res") == 0) &&
            (uprv_strcmp(name, "zoneinfo64") == 0 ||
             uprv_strcmp(name, "timezoneTypes") == 0 ||
             uprv_strcmp(name, "windowsZones") == 0 ||
             uprv_strcmp(name, "metaZones") == 0));
}

/*
 *  A note on the ownership of Mapped Memory
 *
 *  For common format files, ownership resides with the UDataMemory object
 *    that lives in the cache of opened common data.  These UDataMemorys are private
 *    to the udata implementation, and are never seen directly by users.
 *
 *    The UDataMemory objects returned to users will have the address of some desired
 *    data within the mapped region, but they wont have the mapping info itself, and thus
 *    won't cause anything to be removed from memory when they are closed.
 *
 *  For individual data files, the UDataMemory returned to the user holds the
 *  information necessary to unmap the data on close.  If the user independently
 *  opens the same data file twice, two completely independent mappings will be made.
 *  (There is no cache of opened data items from individual files, only a cache of
 *   opened Common Data files, that is, files containing a collection of data items.)
 *
 *  For common data passed in from the user via udata_setAppData() or
 *  udata_setCommonData(), ownership remains with the user.
 *
 *  UDataMemory objects themselves, as opposed to the memory they describe,
 *  can be anywhere - heap, stack/local or global.
 *  They have a flag to indicate when they're heap allocated and thus
 *  must be deleted when closed.
 */


/*----------------------------------------------------------------------------*
 *                                                                            *
 * main data loading functions                                                *
 *                                                                            *
 *----------------------------------------------------------------------------*/
ساكن UData_حافظہ *
doOpenChoice(ثابت حرف *رستہ, ثابت حرف *type, ثابت حرف *name,
             UData_حافظہ_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کوڈ *p_Error_کوڈ)
{
    UData_حافظہ         *retVal = خالی_سوچک;

    ثابت حرف         *انکڑے_رستہ;

    int32_t             toc_اندراج_Suffix_اشاریہ;
    ثابت حرف         *toc_اندراج_رستہ_Suffix;
    UError_کوڈ          sub_Error_کوڈ=U_ZERO_ERROR;
    ثابت حرف         *درخت_Char;

    UBool               isICUData = خطأ;


    FileTracer::traceOpen(رستہ, type, name);


    /* Is this path ICU data? */
    إذا(رستہ == خالی_سوچک ||
       !strcmp(رستہ, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(رستہ, U_ICUDATA_NAME U_درخت_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_درخت_SEPARATOR_STRING)) ||  
       !uprv_strncmp(رستہ, U_ICUDATA_ALIAS U_درخت_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_درخت_SEPARATOR_STRING))) {
      isICUData = سچ;
    }

#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_رستہ;
    إذا(رستہ) {
        إذا(uprv_strchr(رستہ,U_FILE_ALT_SEP_CHAR) != خالی_سوچک) {
            alt_Sep_رستہ.append(رستہ, *p_Error_کوڈ);
            حرف *p;
            طالما ((p = uprv_strchr(alt_Sep_رستہ.انکڑے(), U_FILE_ALT_SEP_CHAR)) != خالی_سوچک) {
                *p = U_FILE_SEP_CHAR;
            }
#إذا defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", رستہ, alt_Sep_رستہ.انکڑے());
#جے_انت
            رستہ = alt_Sep_رستہ.انکڑے();
        }
    }
#جے_انت

    CharString toc_اندراج_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_اندراج_رستہ; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString درخت_Name;

    /* ======= Set up strings */
    إذا(رستہ==خالی_سوچک) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_کوڈ);
    } وإلا {
        ثابت حرف *pkg;
        ثابت حرف *first;
        pkg = uprv_strrchr(رستہ, U_FILE_SEP_CHAR);
        first = uprv_strchr(رستہ, U_FILE_SEP_CHAR);
        إذا(uprv_رستہ_Is_Absolute(رستہ) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            إذا(pkg) {
                pkgName.append(pkg+1, *p_Error_کوڈ);
            } وإلا {
                pkgName.append(رستہ, *p_Error_کوڈ);
            }
        } وإلا {
            درخت_Char = uprv_strchr(رستہ, U_درخت_SEPARATOR);
            إذا(درخت_Char) { 
                درخت_Name.append(درخت_Char+1, *p_Error_کوڈ); /* following '-' */
                إذا(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_کوڈ);
                } وإلا {
                    pkgName.append(رستہ, static_cast<int32_t>(درخت_Char - رستہ), *p_Error_کوڈ);
                    إذا (first == خالی_سوچک) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        رستہ = pkgName.انکڑے();
                    }
                }
            } وإلا {
                إذا(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_کوڈ);
                } وإلا {
                    pkgName.append(رستہ, *p_Error_کوڈ);
                }
            }
        }
    }

#جے_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.انکڑے(), درخت_Name.انکڑے());
#جے_انت

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_اندراج_Name.append(pkgName, *p_Error_کوڈ);
    toc_اندراج_رستہ.append(pkgName, *p_Error_کوڈ);
    toc_اندراج_Suffix_اشاریہ = toc_اندراج_Name.length();

    إذا(!درخت_Name.is_خالی_تھاواں()) {
        toc_اندراج_Name.append(U_درخت_اندراج_SEP_CHAR, *p_Error_کوڈ).append(درخت_Name, *p_Error_کوڈ);
        toc_اندراج_رستہ.append(U_FILE_SEP_CHAR, *p_Error_کوڈ).append(درخت_Name, *p_Error_کوڈ);
    }

    toc_اندراج_Name.append(U_درخت_اندراج_SEP_CHAR, *p_Error_کوڈ).append(name, *p_Error_کوڈ);
    toc_اندراج_رستہ.append(U_FILE_SEP_CHAR, *p_Error_کوڈ).append(name, *p_Error_کوڈ);
    إذا(type!=خالی_سوچک && *type!=0) {
        toc_اندراج_Name.append(".", *p_Error_کوڈ).append(type, *p_Error_کوڈ);
        toc_اندراج_رستہ.append(".", *p_Error_کوڈ).append(type, *p_Error_کوڈ);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_اندراج_رستہ_Suffix = toc_اندراج_رستہ.انکڑے() + toc_اندراج_Suffix_اشاریہ + 1; /* suffix starts here */

#جے_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_اندراج_Name.انکڑے());
    fprintf(stderr, " tocEntryPath = %s\n", toc_اندراج_رستہ.انکڑے());
#جے_انت

#إذا !defined(ICU_انکڑے_DIR_WINDOWS)
    إذا(رستہ == خالی_سوچک) {
        رستہ = COMMON_انکڑے_NAME; /* "icudt26e" */
    }
#وإلا
    // When using the Windows system data, we expects only a single data file.
    رستہ = COMMON_انکڑے_NAME; /* "icudt26e" */
#جے_انت

    /************************ Begin loop looking for ind. files ***************/
#جے_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_رستہ(رستہ));
#جے_انت

    /* End of dealing with a null basename */
    انکڑے_رستہ = u_get_انکڑے_Directory();

    /****    Time zone individual files override  */
    إذا (isICUData && type != خالی_سوچک && is_ویلا_Zone_File(name, type)) {
        ثابت حرف *tzFilesDir = u_get_ویلا_Zone_Files_Directory(p_Error_کوڈ);
        إذا (tzFilesDir[0] != 0) {
#جے_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#جے_انت
            retVal = do_لدو_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_اندراج_رستہ_Suffix,
                            /*رستہ*/ "", type, name, isAcceptable, context, &sub_Error_کوڈ, p_Error_کوڈ);
            إذا((retVal != خالی_سوچک) || U_FAILURE(*p_Error_کوڈ)) {
                أرجع retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    إذا(g_انکڑے_File_Access == UDATA_PACKAGES_پہلا) {
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#جے_انت
        /* #2 */
        retVal = do_لدو_From_Common_انکڑے(isICUData, 
                            pkgName.انکڑے(), انکڑے_رستہ, toc_اندراج_رستہ_Suffix, toc_اندراج_Name.انکڑے(),
                            رستہ, type, name, isAcceptable, context, &sub_Error_کوڈ, p_Error_کوڈ);
        إذا((retVal != خالی_سوچک) || U_FAILURE(*p_Error_کوڈ)) {
            أرجع retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    إذا((g_انکڑے_File_Access==UDATA_PACKAGES_پہلا) ||
       (g_انکڑے_File_Access==UDATA_FILES_پہلا)) {
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#جے_انت
        /* Check to make sure that there is a dataPath to iterate over */
        إذا ((انکڑے_رستہ && *انکڑے_رستہ) || !isICUData) {
            retVal = do_لدو_From_Individual_Files(pkgName.انکڑے(), انکڑے_رستہ, toc_اندراج_رستہ_Suffix,
                            رستہ, type, name, isAcceptable, context, &sub_Error_کوڈ, p_Error_کوڈ);
            إذا((retVal != خالی_سوچک) || U_FAILURE(*p_Error_کوڈ)) {
                أرجع retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    إذا((g_انکڑے_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_انکڑے_File_Access==UDATA_FILES_پہلا)) {
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#جے_انت
        retVal = do_لدو_From_Common_انکڑے(isICUData,
                            pkgName.انکڑے(), انکڑے_رستہ, toc_اندراج_رستہ_Suffix, toc_اندراج_Name.انکڑے(),
                            رستہ, type, name, isAcceptable, context, &sub_Error_کوڈ, p_Error_کوڈ);
        إذا((retVal != خالی_سوچک) || U_FAILURE(*p_Error_کوڈ)) {
            أرجع retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    إذا(g_انکڑے_File_Access==UDATA_NO_FILES) {
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#جے_انت
        retVal = do_لدو_From_Common_انکڑے(isICUData,
                            pkgName.انکڑے(), "", toc_اندراج_رستہ_Suffix, toc_اندراج_Name.انکڑے(),
                            رستہ, type, name, isAcceptable, context, &sub_Error_کوڈ, p_Error_کوڈ);
        إذا((retVal != خالی_سوچک) || U_FAILURE(*p_Error_کوڈ)) {
            أرجع retVal;
        }
    }

    /* data not found */
    إذا(U_SUCCESS(*p_Error_کوڈ)) {
        إذا(U_SUCCESS(sub_Error_کوڈ)) {
            /* file not found */
            *p_Error_کوڈ=U_FILE_ACCESS_ERROR;
        } وإلا {
            /* entry point not found or rejected */
            *p_Error_کوڈ=sub_Error_کوڈ;
        }
    }
    أرجع retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_حافظہ * U_EXPORT2
udata_open(ثابت حرف *رستہ, ثابت حرف *type, ثابت حرف *name,
           UError_کوڈ *p_Error_کوڈ) {
#جے_تعریف_شدہ UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (رستہ?رستہ:"nullptr"), name, type);
    fflush(stderr);
#جے_انت

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    } وإلا إذا(name==خالی_سوچک || *name==0) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    } وإلا {
        أرجع doOpenChoice(رستہ, type, name, خالی_سوچک, خالی_سوچک, p_Error_کوڈ);
    }
}



U_CAPI UData_حافظہ * U_EXPORT2
udata_openChoice(ثابت حرف *رستہ, ثابت حرف *type, ثابت حرف *name,
                 UData_حافظہ_Is_Acceptable *isAcceptable, فراغ *context,
                 UError_کوڈ *p_Error_کوڈ) {
#جے_تعریف_شدہ UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (رستہ?رستہ:"nullptr"), name, type);
#جے_انت

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    } وإلا إذا(name==خالی_سوچک || *name==0 || isAcceptable==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    } وإلا {
        أرجع doOpenChoice(رستہ, type, name, isAcceptable, context, p_Error_کوڈ);
    }
}



U_CAPI فراغ U_EXPORT2
udata_getInfo(UData_حافظہ *p_انکڑے, UDataInfo *pInfo) {
    إذا(pInfo!=خالی_سوچک) {
        إذا(p_انکڑے!=خالی_سوچک && p_انکڑے->p_سرخی!=خالی_سوچک) {
            ثابت UDataInfo *info=&p_انکڑے->p_سرخی->info;
            uint16_t انکڑے_Info_ناپ=udata_get_Info_ناپ(info);
            إذا(pInfo->size>انکڑے_Info_ناپ) {
                pInfo->size=انکڑے_Info_ناپ;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (ثابت uint16_t *)info+1, pInfo->size-2);
            إذا(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reserved_لفظ;
                pInfo->reserved_لفظ=(uint16_t)((x<<8)|(x>>8));
            }
        } وإلا {
            pInfo->size=0;
        }
    }
}


U_CAPI فراغ U_EXPORT2 udata_سیٹ_File_Access(UDataFileAccess access, UError_کوڈ * /*حالت*/)
{
    // Note: this function is documented as not thread safe.
    g_انکڑے_File_Access = access;
}
