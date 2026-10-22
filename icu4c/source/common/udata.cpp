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

#اگر_تعریف_شده __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#پایان_اگر

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
#پایان_اگر

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
ساكن UData_حافظه *udata_پیدا_کن_Cached_داده(ثابت حرف *مسیر, UError_کد &err);

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
ساكن UData_حافظه *gCommonICUDataArray[10] = { اشاره_تهی };   // Access protected by icu global mutex.

ساكن u_atomic_int32_t g_Have_Tried_مقصد_بارگیری_Common_داده {0};  //  See extendICUData().

ساكن UHashtable  *g_Common_داده_Cache = اشاره_تهی;  /* Global hash table of opened ICU data files.  */
ساكن icu::UInitOnce g_Common_داده_Cache_Init_Once {};

#إذا !defined(ICU_داده_DIR_WINDOWS)
ساكن UDataFileAccess  g_داده_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#وإلا
// If we are using the Windows data directory, then look in one spot only.
ساكن UDataFileAccess  g_داده_File_Access = UDATA_NO_FILES;
#پایان_اگر

ساكن UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    إذا (g_Common_داده_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_داده_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_داده_Cache = اشاره_تهی;        /*   Cleanup is not thread safe.                */
    }
    g_Common_داده_Cache_Init_Once.reset();

    لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != اشاره_تهی; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = اشاره_تهی;
    }
    g_Have_Tried_مقصد_بارگیری_Common_داده = 0;

    أرجع درست;                   /* Everything was cleaned up */
}

ساكن UBool U_CALLCONV
پیدا_کن_Common_ICUData_By_Name(ثابت حرف *درون_Basename, UError_کد &err)
{
    UBool یافت = خطأ;
    int32_t i;

    UData_حافظه  *p_داده = udata_پیدا_کن_Cached_داده(درون_Basename, err);
    إذا (U_FAILURE(err) || p_داده == اشاره_تهی)
        أرجع خطأ;

    {
        Mutex قفل;
        لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            إذا ((gCommonICUDataArray[i] != اشاره_تهی) && (gCommonICUDataArray[i]->p_سرآیند == p_داده->p_سرآیند)) {
                /* The data pointer is already in the array. */
                یافت = درست;
                اكسر;
            }
        }
    }
    أرجع یافت;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
ساكن UBool
مجموعه_Common_ICUData(UData_حافظه *p_داده,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_کد *pErr)
{
    UData_حافظه  *new_Common_داده = UData_حافظه_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = خطأ;
    إذا (U_FAILURE(*pErr)) {
        أرجع خطأ;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_تخصیص_بده(new_Common_داده, p_داده);
    umtx_قفل(اشاره_تهی);
    لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        إذا (gCommonICUDataArray[i] == اشاره_تهی) {
            gCommonICUDataArray[i] = new_Common_داده;
            didUpdate = درست;
            اكسر;
        } وإلا إذا (gCommonICUDataArray[i]->p_سرآیند == p_داده->p_سرآیند) {
            /* The same data pointer is already in the array. */
            اكسر;
        }
    }
    umtx_unlock(اشاره_تهی);

    إذا (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    إذا (didUpdate) {
        ucln_common_ثبات_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } وإلا {
        uprv_free(new_Common_داده);
    }
    أرجع didUpdate;
}

#إذا !defined(ICU_داده_DIR_WINDOWS)

ساكن UBool
مجموعه_Common_ICUData_اشاره_گر(ثابت فراغ *p_داده, UBool /*warn*/, UError_کد *p_Error_کد) {
    UData_حافظه t_داده;
    UData_حافظه_init(&t_داده);
    UData_حافظه_مجموعه_داده(&t_داده, p_داده);
    udata_check_Common_داده(&t_داده, p_Error_کد);
    أرجع مجموعه_Common_ICUData(&t_داده, خطأ, p_Error_کد);
}

#پایان_اگر

ساكن ثابت حرف *
پیدا_کن_Basename(ثابت حرف *مسیر) {
    ثابت حرف *basename=uprv_strrchr(مسیر, U_FILE_SEP_CHAR);
    إذا(basename==اشاره_تهی) {
        أرجع مسیر;
    } وإلا {
        أرجع basename+1;
    }
}

#اگر_تعریف_شده UDATA_DEBUG
ساكن ثابت حرف *
package_Name_From_مسیر(ثابت حرف *مسیر)
{
    إذا((مسیر == اشاره_تهی) || (*مسیر == 0)) {
        أرجع U_ICUDATA_NAME;
    }

    مسیر = پیدا_کن_Basename(مسیر);

    إذا((مسیر == اشاره_تهی) || (*مسیر == 0)) {
        أرجع U_ICUDATA_NAME;
    }

    أرجع مسیر;
}
#پایان_اگر

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

تعريفنوع بنية داده_Cache_Element {
    حرف          *name;
    UData_حافظه   *قلم;
} داده_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
ساكن فراغ U_CALLCONV داده_Cache_Element_deleter(فراغ *pDCEl) {
    داده_Cache_Element* p = static_cast<داده_Cache_Element*>(pDCEl);
    udata_close(p->قلم);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

ساكن فراغ U_CALLCONV udata_init_Hash_جدول(UError_کد &err) {
    U_ASSERT(g_Common_داده_Cache == اشاره_تهی);
    g_Common_داده_Cache = uhash_open(uhash_hashChars, uhash_compareChars, اشاره_تهی, &err);
    إذا (U_FAILURE(err)) {
       أرجع;
    }
    U_ASSERT(g_Common_داده_Cache != اشاره_تهی);
    uhash_مجموعه_مقدار_Deleter(g_Common_داده_Cache, داده_Cache_Element_deleter);
    ucln_common_ثبات_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
ساكن UHashtable *udata_get_Hash_جدول(UError_کد &err) {
    umtx_initOnce(g_Common_داده_Cache_Init_Once, &udata_init_Hash_جدول, err);
    أرجع g_Common_داده_Cache;
}



ساكن UData_حافظه *udata_پیدا_کن_Cached_داده(ثابت حرف *مسیر, UError_کد &err)
{
    UHashtable        *htable;
    UData_حافظه       *retVal = اشاره_تهی;
    داده_Cache_Element  *el;
    ثابت حرف        *پایه_Name;

    htable = udata_get_Hash_جدول(err);
    إذا (U_FAILURE(err)) {
        أرجع اشاره_تهی;
    }

    پایه_Name = پیدا_کن_Basename(مسیر);   /* Cache remembers only the base name, not the full path. */
    umtx_قفل(اشاره_تهی);
    el = static_cast<داده_Cache_Element*>(uhash_get(htable, پایه_Name));
    umtx_unlock(اشاره_تهی);
    إذا (el != اشاره_تهی) {
        retVal = el->قلم;
    }
#اگر_تعریف_شده UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", پایه_Name, (فراغ*) retVal);
#پایان_اگر
    أرجع retVal;
}


ساكن UData_حافظه *udata_cache_داده_قلم(ثابت حرف *مسیر, UData_حافظه *قلم, UError_کد *pErr) {
    داده_Cache_Element *newElement;
    ثابت حرف       *پایه_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    داده_Cache_Element *old_مقدار = اشاره_تهی;
    UError_کد        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_جدول(*pErr);
    إذا (U_FAILURE(*pErr)) {
        أرجع اشاره_تهی;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<داده_Cache_Element*>(uprv_malloc(حجم(داده_Cache_Element)));
    إذا (newElement == اشاره_تهی) {
        *pErr = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    newElement->قلم = UData_حافظه_create_New_Instance(pErr);
    إذا (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        أرجع اشاره_تهی;
    }
    UDatamemory_تخصیص_بده(newElement->قلم, قلم);

    پایه_Name = پیدا_کن_Basename(مسیر);
    nameLen = static_cast<int32_t>(uprv_strlen(پایه_Name));
    newElement->name = static_cast<حرف*>(uprv_malloc(nameLen + 1));
    إذا (newElement->name == اشاره_تهی) {
        *pErr = U_حافظه_تخصیص_ERROR;
        uprv_free(newElement->قلم);
        uprv_free(newElement);
        أرجع اشاره_تهی;
    }
    uprv_strcpy(newElement->name, پایه_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_قفل(اشاره_تهی);
    old_مقدار = static_cast<داده_Cache_Element*>(uhash_get(htable, مسیر));
    إذا (old_مقدار != اشاره_تهی) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    وإلا {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(اشاره_تهی);

#اگر_تعریف_شده UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (فراغ*) newElement->قلم, u_errorName(subErr), (فراغ*) newElement->قلم->vFuncs);
#پایان_اگر

    إذا (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->قلم);
        uprv_free(newElement);
        أرجع old_مقدار ? old_مقدار->قلم : اشاره_تهی;
    }

    أرجع newElement->قلم;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

صنف UData_مسیر_Iterator
{
عام:
    UData_مسیر_Iterator(ثابت حرف *مسیر, ثابت حرف *pkg,
                      ثابت حرف *قلم, ثابت حرف *suffix, UBool do_Check_آخرین_Four,
                      UError_کد *p_Error_کد);
    ثابت حرف *next(UError_کد *p_Error_کد);

خاص:
    ثابت حرف *مسیر;                              /* working path (u_icudata_Dir) */
    ثابت حرف *بعدی_مسیر;                          /* path following this one */
    ثابت حرف *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  قلم_مسیر;                          /* path passed in with item name */
    CharString  مسیر_میانگیر;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       check_آخرین_Four;                     /* if true then allow paths such as '/foo/myapp.dat'
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
UData_مسیر_Iterator::UData_مسیر_Iterator(ثابت حرف *درون_مسیر, ثابت حرف *pkg,
                                     ثابت حرف *قلم, ثابت حرف *درون_Suffix, UBool do_Check_آخرین_Four,
                                     UError_کد *p_Error_کد)
{
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", درون_Suffix, درون_مسیر);
#پایان_اگر
    /** Path **/
    إذا(درون_مسیر == اشاره_تهی) {
        مسیر = u_get_داده_Directory();
    } وإلا {
        مسیر = درون_مسیر;
    }

    /** Package **/
    إذا(pkg != اشاره_تهی) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_کد).append(pkg, *p_Error_کد);
#اگر_تعریف_شده UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.داده(), packageStub.length());
#پایان_اگر
    }

    /** Item **/
    basename = پیدا_کن_Basename(قلم);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    إذا(basename == قلم) {
        بعدی_مسیر = مسیر;
    } وإلا {
        قلم_مسیر.append(قلم, static_cast<int32_t>(basename - قلم), *p_Error_کد);
        بعدی_مسیر = قلم_مسیر.داده();
    }
#اگر_تعریف_شده UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", درون_Suffix, (فراغ*) درون_Suffix);
#پایان_اگر

    /** Suffix  **/
    إذا(درون_Suffix != اشاره_تهی) {
        suffix = درون_Suffix;
    } وإلا {
        suffix = "";
    }

    check_آخرین_Four = do_Check_آخرین_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#اگر_تعریف_شده UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            قلم,
            مسیر,
            basename,
            suffix.داده(),
            قلم_مسیر.داده(),
            بعدی_مسیر,
            check_آخرین_Four?"true":"false");
#پایان_اگر
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ثابت حرف *UData_مسیر_Iterator::next(UError_کد *p_Error_کد)
{
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }

    ثابت حرف *current_مسیر = اشاره_تهی;
    int32_t     مسیر_Len = 0;
    ثابت حرف *مسیر_Basename;

    افعل
    {
        إذا( بعدی_مسیر == اشاره_تهی ) {
            اكسر;
        }
        current_مسیر = بعدی_مسیر;

        إذا(بعدی_مسیر == قلم_مسیر.داده()) { /* we were processing item's path. */
            بعدی_مسیر = مسیر; /* start with regular path next tm. */
            مسیر_Len = static_cast<int32_t>(uprv_strlen(current_مسیر));
        } وإلا {
            /* fix up next for next time */
            بعدی_مسیر = uprv_strchr(current_مسیر, U_مسیر_SEP_CHAR);
            إذا(بعدی_مسیر == اشاره_تهی) {
                /* segment: entire path */
                مسیر_Len = static_cast<int32_t>(uprv_strlen(current_مسیر));
            } وإلا {
                /* segment: until next segment */
                مسیر_Len = static_cast<int32_t>(بعدی_مسیر - current_مسیر);
                /* skip divider */
                بعدی_مسیر ++;
            }
        }

        إذا(مسیر_Len == 0) {
            استمر;
        }

#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_مسیر);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            لكل(qqq=0;qqq<مسیر_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#پایان_اگر
        مسیر_میانگیر.clear().append(current_مسیر, مسیر_Len, *p_Error_کد);

        /* check for .dat files */
        مسیر_Basename = پیدا_کن_Basename(مسیر_میانگیر.داده());

        إذا(check_آخرین_Four && 
           (مسیر_Len>=4) &&
           uprv_strncmp(مسیر_میانگیر.داده() +(مسیر_Len-4), suffix.داده(), 4)==0 && /* suffix matches */
           uprv_strncmp(پیدا_کن_Basename(مسیر_میانگیر.داده()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(مسیر_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#اگر_تعریف_شده UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.داده(), مسیر_میانگیر.داده());
#پایان_اگر
            /* do nothing */
        }
        وإلا 
        {       /* regular dir path */
            إذا(مسیر_میانگیر[مسیر_Len-1] != U_FILE_SEP_CHAR) {
                إذا((مسیر_Len>=4) &&
                   uprv_strncmp(مسیر_میانگیر.داده()+(مسیر_Len-4), ".dat", 4) == 0)
                {
#اگر_تعریف_شده UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", مسیر_میانگیر.داده());
#پایان_اگر
                    استمر;
                }

                /* Check if it is a directory with the same name as our package */
                إذا(!packageStub.is_خالی() &&
                   (مسیر_Len > packageStub.length()) &&
                   !uprv_strcmp(مسیر_میانگیر.داده() + مسیر_Len - packageStub.length(), packageStub.داده())) {
#اگر_تعریف_شده UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.داده(), basename, basenameLen);
#پایان_اگر
                  مسیر_میانگیر.truncate(مسیر_Len - packageStub.length());
                }
                مسیر_میانگیر.append(U_FILE_SEP_CHAR, *p_Error_کد);
            }

            /* + basename */
            مسیر_میانگیر.append(packageStub.داده()+1, packageStub.length()-1, *p_Error_کد);

            إذا (!suffix.empty())  /* tack on suffix */
            {
                إذا (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    مسیر_میانگیر.ensure_دوسر_With_File_Separator(*p_Error_کد);
                }
                مسیر_میانگیر.append(suffix, *p_Error_کد);
            }
        }

#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", مسیر_میانگیر.داده());
#پایان_اگر

        أرجع مسیر_میانگیر.داده();

    } طالما(مسیر);

    /* fell way off the end */
    أرجع اشاره_تهی;
}

U_NAMESPACE_انتها

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#إذا !defined(ICU_داده_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
خارجي "C" ثابت داده_سرآیند U_داده_API U_ICUDATA_مدخل_نقطه;
#پایان_اگر

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
ساكن UData_حافظه *
open_Common_داده(ثابت حرف *مسیر,          /*  Path from OpenChoice?          */
               int32_t common_داده_شاخص,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_کد *p_Error_کد)
{
    UData_حافظه t_داده;
    ثابت حرف *مسیر_میانگیر;
    ثابت حرف *درون_Basename;

    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }

    UData_حافظه_init(&t_داده);

    /* ??????? TODO revisit this */ 
    إذا (common_داده_شاخص >= 0) {
        /* "mini-cache" for common ICU data */
        إذا(common_داده_شاخص >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            أرجع اشاره_تهی;
        }
        {
            Mutex قفل;
            إذا(gCommonICUDataArray[common_داده_شاخص] != اشاره_تهی) {
                أرجع gCommonICUDataArray[common_داده_شاخص];
            }
#إذا !defined(ICU_داده_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            لكل(i = 0; i < common_داده_شاخص; ++i) {
                إذا(gCommonICUDataArray[i]->p_سرآیند == &U_ICUDATA_مدخل_نقطه) {
                    /* The linked-in data is already in the list. */
                    أرجع اشاره_تهی;
                }
            }
#پایان_اگر
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
#إذا !defined(ICU_داده_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        مجموعه_Common_ICUData_اشاره_گر(&U_ICUDATA_مدخل_نقطه, خطأ, p_Error_کد);
        {
            Mutex قفل;
            أرجع gCommonICUDataArray[common_داده_شاخص];
        }
#پایان_اگر
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    درون_Basename = پیدا_کن_Basename(مسیر);
#اگر_تعریف_شده UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", درون_Basename);
#پایان_اگر

    إذا(*درون_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", مسیر);
#پایان_اگر
        إذا (U_SUCCESS(*p_Error_کد)) {
            *p_Error_کد=U_FILE_ACCESS_ERROR;
        }
        أرجع اشاره_تهی;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_حافظه  *داده_مقصد_Return = udata_پیدا_کن_Cached_داده(درون_Basename, *p_Error_کد);
    إذا (داده_مقصد_Return != اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع داده_مقصد_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_مسیر_Iterator iter(u_get_داده_Directory(), درون_Basename, مسیر, ".dat", درست, p_Error_کد);

    طالما ((UData_حافظه_is_Loaded(&t_داده)==خطأ) && (مسیر_میانگیر = iter.next(p_Error_کد)) != اشاره_تهی)
    {
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", مسیر_میانگیر);
#پایان_اگر
        uprv_mapFile(&t_داده, مسیر_میانگیر, p_Error_کد);
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_حافظه_is_Loaded(&t_داده)?"LOADED":"not loaded");
#پایان_اگر
    }
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }

    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا (!UData_حافظه_is_Loaded(&t_داده)) {
        /* no common data */
        *p_Error_کد=U_FILE_ACCESS_ERROR;
        أرجع اشاره_تهی;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_داده(&t_داده, p_Error_کد);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    أرجع udata_cache_داده_قلم(درون_Basename, &t_داده, p_Error_کد);
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
ساكن UBool extendICUData(UError_کد *pErr)
{
    UData_حافظه   *p_داده;
    UData_حافظه   copyPData;
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
    umtx_قفل(&extendICUDataMutex);
#پایان_اگر
    إذا(!umtx_بارگیری_Acquire(g_Have_Tried_مقصد_بارگیری_Common_داده)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_داده = open_Common_داده(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_حافظه_init(&copyPData);
       إذا(p_داده != اشاره_تهی) {
          UDatamemory_تخصیص_بده(&copyPData, p_داده);
          copyPData.map = اشاره_تهی;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = اشاره_تهی; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              مجموعه_Common_ICUData(&copyPData,/*  The new common data.                                */
                       خطأ,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_مقصد_بارگیری_Common_داده, 1);
    }

    didUpdate = پیدا_کن_Common_ICUData_By_Name(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#إذا MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#پایان_اگر
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
udata_مجموعه_Common_داده(ثابت فراغ *داده, UError_کد *p_Error_کد) {
    UData_حافظه داده_حافظه;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع;
    }

    إذا(داده==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    /* set the data pointer and test for validity */
    UData_حافظه_init(&داده_حافظه);
    UData_حافظه_مجموعه_داده(&داده_حافظه, داده);
    udata_check_Common_داده(&داده_حافظه, p_Error_کد);
    إذا (U_FAILURE(*p_Error_کد)) {أرجع;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    مجموعه_Common_ICUData(&داده_حافظه, درست, p_Error_کد);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI فراغ U_EXPORT2
udata_مجموعه_App_داده(ثابت حرف *مسیر, ثابت فراغ *داده, UError_کد *err)
{
    UData_حافظه     udm;

    إذا(err==اشاره_تهی || U_FAILURE(*err)) {
        أرجع;
    }
    إذا(داده==اشاره_تهی) {
        *err=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    UData_حافظه_init(&udm);
    UData_حافظه_مجموعه_داده(&udm, داده);
    udata_check_Common_داده(&udm, err);
    udata_cache_داده_قلم(مسیر, &udm, err);
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
ساكن UData_حافظه *
check_داده_قلم
(
 ثابت داده_سرآیند         *p_سرآیند,         /* The data item to be checked.                */
 UData_حافظه_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 فراغ                     *context,         /*   pass-thru param for above.                */
 ثابت حرف               *type,            /*   pass-thru param for above.                */
 ثابت حرف               *name,            /*   pass-thru param for above.                */
 UError_کد               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_کد               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_حافظه  *r_داده_Mem = اشاره_تهی;          /* the new UDataMemory, to be returned.        */

    إذا (U_FAILURE(*fatalErr)) {
        أرجع اشاره_تهی;
    }

    إذا(p_سرآیند->داده_سرآیند_2.magic1==0xda &&
        p_سرآیند->داده_سرآیند_2.magic2==0x27 &&
        (isAcceptable==اشاره_تهی || isAcceptable(context, type, name, &p_سرآیند->info))
    ) {
        r_داده_Mem=UData_حافظه_create_New_Instance(fatalErr);
        إذا (U_FAILURE(*fatalErr)) {
            أرجع اشاره_تهی;
        }
        r_داده_Mem->p_سرآیند = p_سرآیند;
    } وإلا {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    أرجع r_داده_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساكن UData_حافظه *do_بارگیری_From_Individual_Files(ثابت حرف *pkgName, 
        ثابت حرف *داده_مسیر, ثابت حرف *toc_مدخل_مسیر_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *مسیر, ثابت حرف *type, ثابت حرف *name,
             UData_حافظه_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کد *sub_Error_کد,
             UError_کد *p_Error_کد)
{
    ثابت حرف         *مسیر_میانگیر;
    UData_حافظه         داده_حافظه;
    UData_حافظه *p_مدخل_داده;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_مسیر_Iterator iter(داده_مسیر, pkgName, مسیر, toc_مدخل_مسیر_Suffix, خطأ, p_Error_کد);

    طالما ((مسیر_میانگیر = iter.next(p_Error_کد)) != اشاره_تهی)
    {
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", مسیر_میانگیر);
#پایان_اگر
        إذا (uprv_mapFile(&داده_حافظه, مسیر_میانگیر, p_Error_کد))
        {
            p_مدخل_داده = check_داده_قلم(داده_حافظه.p_سرآیند, isAcceptable, context, type, name, sub_Error_کد, p_Error_کد);
            إذا (p_مدخل_داده != اشاره_تهی) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_مدخل_داده->mapAddr = داده_حافظه.mapAddr;
                p_مدخل_داده->map     = داده_حافظه.map;
                p_مدخل_داده->length  = داده_حافظه.length;

#اگر_تعریف_شده UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", مسیر_میانگیر);
#پایان_اگر
                أرجع p_مدخل_داده;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&داده_حافظه);

            /* If we had a nasty error, bail out completely.  */
            إذا (U_FAILURE(*p_Error_کد)) {
                أرجع اشاره_تهی;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_کد=U_INVALID_FORMAT_ERROR;
        }
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_حافظه_is_Loaded(&داده_حافظه)?"LOADED":"not loaded");
#پایان_اگر
    }
    أرجع اشاره_تهی;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساكن UData_حافظه *do_بارگیری_From_Common_داده(UBool isICUData, ثابت حرف * /*pkgName*/, 
        ثابت حرف * /*dataPath*/, ثابت حرف * /*tocEntryPathSuffix*/, ثابت حرف *toc_مدخل_Name,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *مسیر, ثابت حرف *type, ثابت حرف *name,
             UData_حافظه_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کد *sub_Error_کد,
             UError_کد *p_Error_کد)
{
    UData_حافظه        *p_مدخل_داده;
    ثابت داده_سرآیند   *p_سرآیند;
    UData_حافظه        *p_Common_داده;
    int32_t            common_داده_شاخص;
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
    لكل (common_داده_شاخص = isICUData ? 0 : -1;;) {
        p_Common_داده=open_Common_داده(مسیر, common_داده_شاخص, sub_Error_کد); /** search for pkg **/

        إذا(U_SUCCESS(*sub_Error_کد) && p_Common_داده!=اشاره_تهی) {
            int32_t length;

            /* look up the data piece in the common data */
            p_سرآیند=p_Common_داده->vFuncs->Lookup(p_Common_داده, toc_مدخل_Name, &length, sub_Error_کد);
#اگر_تعریف_شده UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_مدخل_Name, (فراغ*) p_سرآیند, u_errorName(*sub_Error_کد));
#پایان_اگر

            إذا(p_سرآیند!=اشاره_تهی) {
                p_مدخل_داده = check_داده_قلم(p_سرآیند, isAcceptable, context, type, name, sub_Error_کد, p_Error_کد);
#اگر_تعریف_شده UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (فراغ*) p_مدخل_داده);
#پایان_اگر
                إذا (U_FAILURE(*p_Error_کد)) {
                    أرجع اشاره_تهی;
                }
                إذا (p_مدخل_داده != اشاره_تهی) {
                    p_مدخل_داده->length = length;
                    أرجع p_مدخل_داده;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        إذا (*sub_Error_کد == U_حافظه_تخصیص_ERROR) {
            *p_Error_کد = *sub_Error_کد;
            أرجع اشاره_تهی;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        إذا (!isICUData) {
            أرجع اشاره_تهی;
        } وإلا إذا (p_Common_داده != اشاره_تهی) {
            ++common_داده_شاخص;  /* try the next data package */
        } وإلا إذا ((!checkedExtendedICUData) && extendICUData(sub_Error_کد)) {
            checkedExtendedICUData = درست;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } وإلا {
            أرجع اشاره_تهی;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
ساكن UBool is_زمان_Zone_File(ثابت حرف *name, ثابت حرف *type) {
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
ساكن UData_حافظه *
doOpenChoice(ثابت حرف *مسیر, ثابت حرف *type, ثابت حرف *name,
             UData_حافظه_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کد *p_Error_کد)
{
    UData_حافظه         *retVal = اشاره_تهی;

    ثابت حرف         *داده_مسیر;

    int32_t             toc_مدخل_Suffix_شاخص;
    ثابت حرف         *toc_مدخل_مسیر_Suffix;
    UError_کد          sub_Error_کد=U_ZERO_ERROR;
    ثابت حرف         *درخت_Char;

    UBool               isICUData = خطأ;


    FileTracer::traceOpen(مسیر, type, name);


    /* Is this path ICU data? */
    إذا(مسیر == اشاره_تهی ||
       !strcmp(مسیر, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(مسیر, U_ICUDATA_NAME U_درخت_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_درخت_SEPARATOR_STRING)) ||  
       !uprv_strncmp(مسیر, U_ICUDATA_ALIAS U_درخت_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_درخت_SEPARATOR_STRING))) {
      isICUData = درست;
    }

#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_مسیر;
    إذا(مسیر) {
        إذا(uprv_strchr(مسیر,U_FILE_ALT_SEP_CHAR) != اشاره_تهی) {
            alt_Sep_مسیر.append(مسیر, *p_Error_کد);
            حرف *p;
            طالما ((p = uprv_strchr(alt_Sep_مسیر.داده(), U_FILE_ALT_SEP_CHAR)) != اشاره_تهی) {
                *p = U_FILE_SEP_CHAR;
            }
#إذا defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", مسیر, alt_Sep_مسیر.داده());
#پایان_اگر
            مسیر = alt_Sep_مسیر.داده();
        }
    }
#پایان_اگر

    CharString toc_مدخل_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_مدخل_مسیر; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString درخت_Name;

    /* ======= Set up strings */
    إذا(مسیر==اشاره_تهی) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_کد);
    } وإلا {
        ثابت حرف *pkg;
        ثابت حرف *first;
        pkg = uprv_strrchr(مسیر, U_FILE_SEP_CHAR);
        first = uprv_strchr(مسیر, U_FILE_SEP_CHAR);
        إذا(uprv_مسیر_Is_Absolute(مسیر) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            إذا(pkg) {
                pkgName.append(pkg+1, *p_Error_کد);
            } وإلا {
                pkgName.append(مسیر, *p_Error_کد);
            }
        } وإلا {
            درخت_Char = uprv_strchr(مسیر, U_درخت_SEPARATOR);
            إذا(درخت_Char) { 
                درخت_Name.append(درخت_Char+1, *p_Error_کد); /* following '-' */
                إذا(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_کد);
                } وإلا {
                    pkgName.append(مسیر, static_cast<int32_t>(درخت_Char - مسیر), *p_Error_کد);
                    إذا (first == اشاره_تهی) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        مسیر = pkgName.داده();
                    }
                }
            } وإلا {
                إذا(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_کد);
                } وإلا {
                    pkgName.append(مسیر, *p_Error_کد);
                }
            }
        }
    }

#اگر_تعریف_شده UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.داده(), درخت_Name.داده());
#پایان_اگر

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_مدخل_Name.append(pkgName, *p_Error_کد);
    toc_مدخل_مسیر.append(pkgName, *p_Error_کد);
    toc_مدخل_Suffix_شاخص = toc_مدخل_Name.length();

    إذا(!درخت_Name.is_خالی()) {
        toc_مدخل_Name.append(U_درخت_مدخل_SEP_CHAR, *p_Error_کد).append(درخت_Name, *p_Error_کد);
        toc_مدخل_مسیر.append(U_FILE_SEP_CHAR, *p_Error_کد).append(درخت_Name, *p_Error_کد);
    }

    toc_مدخل_Name.append(U_درخت_مدخل_SEP_CHAR, *p_Error_کد).append(name, *p_Error_کد);
    toc_مدخل_مسیر.append(U_FILE_SEP_CHAR, *p_Error_کد).append(name, *p_Error_کد);
    إذا(type!=اشاره_تهی && *type!=0) {
        toc_مدخل_Name.append(".", *p_Error_کد).append(type, *p_Error_کد);
        toc_مدخل_مسیر.append(".", *p_Error_کد).append(type, *p_Error_کد);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_مدخل_مسیر_Suffix = toc_مدخل_مسیر.داده() + toc_مدخل_Suffix_شاخص + 1; /* suffix starts here */

#اگر_تعریف_شده UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_مدخل_Name.داده());
    fprintf(stderr, " tocEntryPath = %s\n", toc_مدخل_مسیر.داده());
#پایان_اگر

#إذا !defined(ICU_داده_DIR_WINDOWS)
    إذا(مسیر == اشاره_تهی) {
        مسیر = COMMON_داده_NAME; /* "icudt26e" */
    }
#وإلا
    // When using the Windows system data, we expects only a single data file.
    مسیر = COMMON_داده_NAME; /* "icudt26e" */
#پایان_اگر

    /************************ Begin loop looking for ind. files ***************/
#اگر_تعریف_شده UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_مسیر(مسیر));
#پایان_اگر

    /* End of dealing with a null basename */
    داده_مسیر = u_get_داده_Directory();

    /****    Time zone individual files override  */
    إذا (isICUData && type != اشاره_تهی && is_زمان_Zone_File(name, type)) {
        ثابت حرف *tzFilesDir = u_get_زمان_Zone_Files_Directory(p_Error_کد);
        إذا (tzFilesDir[0] != 0) {
#اگر_تعریف_شده UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#پایان_اگر
            retVal = do_بارگیری_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_مدخل_مسیر_Suffix,
                            /*مسیر*/ "", type, name, isAcceptable, context, &sub_Error_کد, p_Error_کد);
            إذا((retVal != اشاره_تهی) || U_FAILURE(*p_Error_کد)) {
                أرجع retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    إذا(g_داده_File_Access == UDATA_PACKAGES_اولی) {
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#پایان_اگر
        /* #2 */
        retVal = do_بارگیری_From_Common_داده(isICUData, 
                            pkgName.داده(), داده_مسیر, toc_مدخل_مسیر_Suffix, toc_مدخل_Name.داده(),
                            مسیر, type, name, isAcceptable, context, &sub_Error_کد, p_Error_کد);
        إذا((retVal != اشاره_تهی) || U_FAILURE(*p_Error_کد)) {
            أرجع retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    إذا((g_داده_File_Access==UDATA_PACKAGES_اولی) ||
       (g_داده_File_Access==UDATA_FILES_اولی)) {
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#پایان_اگر
        /* Check to make sure that there is a dataPath to iterate over */
        إذا ((داده_مسیر && *داده_مسیر) || !isICUData) {
            retVal = do_بارگیری_From_Individual_Files(pkgName.داده(), داده_مسیر, toc_مدخل_مسیر_Suffix,
                            مسیر, type, name, isAcceptable, context, &sub_Error_کد, p_Error_کد);
            إذا((retVal != اشاره_تهی) || U_FAILURE(*p_Error_کد)) {
                أرجع retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    إذا((g_داده_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_داده_File_Access==UDATA_FILES_اولی)) {
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#پایان_اگر
        retVal = do_بارگیری_From_Common_داده(isICUData,
                            pkgName.داده(), داده_مسیر, toc_مدخل_مسیر_Suffix, toc_مدخل_Name.داده(),
                            مسیر, type, name, isAcceptable, context, &sub_Error_کد, p_Error_کد);
        إذا((retVal != اشاره_تهی) || U_FAILURE(*p_Error_کد)) {
            أرجع retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    إذا(g_داده_File_Access==UDATA_NO_FILES) {
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#پایان_اگر
        retVal = do_بارگیری_From_Common_داده(isICUData,
                            pkgName.داده(), "", toc_مدخل_مسیر_Suffix, toc_مدخل_Name.داده(),
                            مسیر, type, name, isAcceptable, context, &sub_Error_کد, p_Error_کد);
        إذا((retVal != اشاره_تهی) || U_FAILURE(*p_Error_کد)) {
            أرجع retVal;
        }
    }

    /* data not found */
    إذا(U_SUCCESS(*p_Error_کد)) {
        إذا(U_SUCCESS(sub_Error_کد)) {
            /* file not found */
            *p_Error_کد=U_FILE_ACCESS_ERROR;
        } وإلا {
            /* entry point not found or rejected */
            *p_Error_کد=sub_Error_کد;
        }
    }
    أرجع retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_حافظه * U_EXPORT2
udata_open(ثابت حرف *مسیر, ثابت حرف *type, ثابت حرف *name,
           UError_کد *p_Error_کد) {
#اگر_تعریف_شده UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (مسیر?مسیر:"nullptr"), name, type);
    fflush(stderr);
#پایان_اگر

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    } وإلا إذا(name==اشاره_تهی || *name==0) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    } وإلا {
        أرجع doOpenChoice(مسیر, type, name, اشاره_تهی, اشاره_تهی, p_Error_کد);
    }
}



U_CAPI UData_حافظه * U_EXPORT2
udata_openChoice(ثابت حرف *مسیر, ثابت حرف *type, ثابت حرف *name,
                 UData_حافظه_Is_Acceptable *isAcceptable, فراغ *context,
                 UError_کد *p_Error_کد) {
#اگر_تعریف_شده UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (مسیر?مسیر:"nullptr"), name, type);
#پایان_اگر

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    } وإلا إذا(name==اشاره_تهی || *name==0 || isAcceptable==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    } وإلا {
        أرجع doOpenChoice(مسیر, type, name, isAcceptable, context, p_Error_کد);
    }
}



U_CAPI فراغ U_EXPORT2
udata_getInfo(UData_حافظه *p_داده, UDataInfo *pInfo) {
    إذا(pInfo!=اشاره_تهی) {
        إذا(p_داده!=اشاره_تهی && p_داده->p_سرآیند!=اشاره_تهی) {
            ثابت UDataInfo *info=&p_داده->p_سرآیند->info;
            uint16_t داده_Info_اندازه=udata_get_Info_اندازه(info);
            إذا(pInfo->size>داده_Info_اندازه) {
                pInfo->size=داده_Info_اندازه;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (ثابت uint16_t *)info+1, pInfo->size-2);
            إذا(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reserved_کلمه;
                pInfo->reserved_کلمه=(uint16_t)((x<<8)|(x>>8));
            }
        } وإلا {
            pInfo->size=0;
        }
    }
}


U_CAPI فراغ U_EXPORT2 udata_مجموعه_File_Access(UDataFileAccess access, UError_کد * /*وضعیت*/)
{
    // Note: this function is documented as not thread safe.
    g_داده_File_Access = access;
}
