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

#که_تعریف_شوی __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#د_که_پای

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
#د_که_پای

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
ساكن UData_حافظه *udata_پیدا_کړه_Cached_معلومات(ثابت حرف *لار, UError_کوډ &err);

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
ساكن UData_حافظه *gCommonICUDataArray[10] = { تش_اشاره };   // Access protected by icu global mutex.

ساكن u_atomic_int32_t g_Have_Tried_منزل_بارول_Common_معلومات {0};  //  See extendICUData().

ساكن UHashtable  *g_Common_معلومات_Cache = تش_اشاره;  /* Global hash table of opened ICU data files.  */
ساكن icu::UInitOnce g_Common_معلومات_Cache_Init_Once {};

#إذا !defined(ICU_معلومات_DIR_WINDOWS)
ساكن UDataFileAccess  g_معلومات_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#وإلا
// If we are using the Windows data directory, then look in one spot only.
ساكن UDataFileAccess  g_معلومات_File_Access = UDATA_NO_FILES;
#د_که_پای

ساكن UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    إذا (g_Common_معلومات_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_معلومات_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_معلومات_Cache = تش_اشاره;        /*   Cleanup is not thread safe.                */
    }
    g_Common_معلومات_Cache_Init_Once.reset();

    لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != تش_اشاره; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = تش_اشاره;
    }
    g_Have_Tried_منزل_بارول_Common_معلومات = 0;

    أرجع ریښتیا;                   /* Everything was cleaned up */
}

ساكن UBool U_CALLCONV
پیدا_کړه_Common_ICUData_By_Name(ثابت حرف *دننه_Basename, UError_کوډ &err)
{
    UBool وموندل = خطأ;
    int32_t i;

    UData_حافظه  *p_معلومات = udata_پیدا_کړه_Cached_معلومات(دننه_Basename, err);
    إذا (U_FAILURE(err) || p_معلومات == تش_اشاره)
        أرجع خطأ;

    {
        Mutex قلف;
        لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            إذا ((gCommonICUDataArray[i] != تش_اشاره) && (gCommonICUDataArray[i]->p_سرلیک == p_معلومات->p_سرلیک)) {
                /* The data pointer is already in the array. */
                وموندل = ریښتیا;
                اكسر;
            }
        }
    }
    أرجع وموندل;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
ساكن UBool
سېټ_Common_ICUData(UData_حافظه *p_معلومات,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_کوډ *pErr)
{
    UData_حافظه  *new_Common_معلومات = UData_حافظه_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = خطأ;
    إذا (U_FAILURE(*pErr)) {
        أرجع خطأ;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_ټاکل(new_Common_معلومات, p_معلومات);
    umtx_قلف(تش_اشاره);
    لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        إذا (gCommonICUDataArray[i] == تش_اشاره) {
            gCommonICUDataArray[i] = new_Common_معلومات;
            didUpdate = ریښتیا;
            اكسر;
        } وإلا إذا (gCommonICUDataArray[i]->p_سرلیک == p_معلومات->p_سرلیک) {
            /* The same data pointer is already in the array. */
            اكسر;
        }
    }
    umtx_unlock(تش_اشاره);

    إذا (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    إذا (didUpdate) {
        ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } وإلا {
        uprv_free(new_Common_معلومات);
    }
    أرجع didUpdate;
}

#إذا !defined(ICU_معلومات_DIR_WINDOWS)

ساكن UBool
سېټ_Common_ICUData_اشاره_ګر(ثابت فراغ *p_معلومات, UBool /*warn*/, UError_کوډ *p_Error_کوډ) {
    UData_حافظه t_معلومات;
    UData_حافظه_init(&t_معلومات);
    UData_حافظه_سېټ_معلومات(&t_معلومات, p_معلومات);
    udata_check_Common_معلومات(&t_معلومات, p_Error_کوډ);
    أرجع سېټ_Common_ICUData(&t_معلومات, خطأ, p_Error_کوډ);
}

#د_که_پای

ساكن ثابت حرف *
پیدا_کړه_Basename(ثابت حرف *لار) {
    ثابت حرف *basename=uprv_strrchr(لار, U_FILE_SEP_CHAR);
    إذا(basename==تش_اشاره) {
        أرجع لار;
    } وإلا {
        أرجع basename+1;
    }
}

#که_تعریف_شوی UDATA_DEBUG
ساكن ثابت حرف *
package_Name_From_لار(ثابت حرف *لار)
{
    إذا((لار == تش_اشاره) || (*لار == 0)) {
        أرجع U_ICUDATA_NAME;
    }

    لار = پیدا_کړه_Basename(لار);

    إذا((لار == تش_اشاره) || (*لار == 0)) {
        أرجع U_ICUDATA_NAME;
    }

    أرجع لار;
}
#د_که_پای

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

تعريفنوع بنية معلومات_Cache_Element {
    حرف          *name;
    UData_حافظه   *توکی;
} معلومات_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
ساكن فراغ U_CALLCONV معلومات_Cache_Element_deleter(فراغ *pDCEl) {
    معلومات_Cache_Element* p = static_cast<معلومات_Cache_Element*>(pDCEl);
    udata_close(p->توکی);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

ساكن فراغ U_CALLCONV udata_init_Hash_جدول(UError_کوډ &err) {
    U_ASSERT(g_Common_معلومات_Cache == تش_اشاره);
    g_Common_معلومات_Cache = uhash_open(uhash_hashChars, uhash_compareChars, تش_اشاره, &err);
    إذا (U_FAILURE(err)) {
       أرجع;
    }
    U_ASSERT(g_Common_معلومات_Cache != تش_اشاره);
    uhash_سېټ_ارزښت_Deleter(g_Common_معلومات_Cache, معلومات_Cache_Element_deleter);
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
ساكن UHashtable *udata_get_Hash_جدول(UError_کوډ &err) {
    umtx_initOnce(g_Common_معلومات_Cache_Init_Once, &udata_init_Hash_جدول, err);
    أرجع g_Common_معلومات_Cache;
}



ساكن UData_حافظه *udata_پیدا_کړه_Cached_معلومات(ثابت حرف *لار, UError_کوډ &err)
{
    UHashtable        *htable;
    UData_حافظه       *retVal = تش_اشاره;
    معلومات_Cache_Element  *el;
    ثابت حرف        *بنسټ_Name;

    htable = udata_get_Hash_جدول(err);
    إذا (U_FAILURE(err)) {
        أرجع تش_اشاره;
    }

    بنسټ_Name = پیدا_کړه_Basename(لار);   /* Cache remembers only the base name, not the full path. */
    umtx_قلف(تش_اشاره);
    el = static_cast<معلومات_Cache_Element*>(uhash_get(htable, بنسټ_Name));
    umtx_unlock(تش_اشاره);
    إذا (el != تش_اشاره) {
        retVal = el->توکی;
    }
#که_تعریف_شوی UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", بنسټ_Name, (فراغ*) retVal);
#د_که_پای
    أرجع retVal;
}


ساكن UData_حافظه *udata_cache_معلومات_توکی(ثابت حرف *لار, UData_حافظه *توکی, UError_کوډ *pErr) {
    معلومات_Cache_Element *newElement;
    ثابت حرف       *بنسټ_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    معلومات_Cache_Element *old_ارزښت = تش_اشاره;
    UError_کوډ        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_جدول(*pErr);
    إذا (U_FAILURE(*pErr)) {
        أرجع تش_اشاره;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<معلومات_Cache_Element*>(uprv_malloc(حجم(معلومات_Cache_Element)));
    إذا (newElement == تش_اشاره) {
        *pErr = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    newElement->توکی = UData_حافظه_create_New_Instance(pErr);
    إذا (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        أرجع تش_اشاره;
    }
    UDatamemory_ټاکل(newElement->توکی, توکی);

    بنسټ_Name = پیدا_کړه_Basename(لار);
    nameLen = static_cast<int32_t>(uprv_strlen(بنسټ_Name));
    newElement->name = static_cast<حرف*>(uprv_malloc(nameLen + 1));
    إذا (newElement->name == تش_اشاره) {
        *pErr = U_حافظه_ویش_ERROR;
        uprv_free(newElement->توکی);
        uprv_free(newElement);
        أرجع تش_اشاره;
    }
    uprv_strcpy(newElement->name, بنسټ_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_قلف(تش_اشاره);
    old_ارزښت = static_cast<معلومات_Cache_Element*>(uhash_get(htable, لار));
    إذا (old_ارزښت != تش_اشاره) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    وإلا {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(تش_اشاره);

#که_تعریف_شوی UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (فراغ*) newElement->توکی, u_errorName(subErr), (فراغ*) newElement->توکی->vFuncs);
#د_که_پای

    إذا (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->توکی);
        uprv_free(newElement);
        أرجع old_ارزښت ? old_ارزښت->توکی : تش_اشاره;
    }

    أرجع newElement->توکی;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

صنف UData_لار_Iterator
{
عام:
    UData_لار_Iterator(ثابت حرف *لار, ثابت حرف *pkg,
                      ثابت حرف *توکی, ثابت حرف *suffix, UBool do_Check_وروستی_Four,
                      UError_کوډ *p_Error_کوډ);
    ثابت حرف *next(UError_کوډ *p_Error_کوډ);

خاص:
    ثابت حرف *لار;                              /* working path (u_icudata_Dir) */
    ثابت حرف *بل_لار;                          /* path following this one */
    ثابت حرف *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  توکی_لار;                          /* path passed in with item name */
    CharString  لار_بفر;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       check_وروستی_Four;                     /* if true then allow paths such as '/foo/myapp.dat'
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
UData_لار_Iterator::UData_لار_Iterator(ثابت حرف *دننه_لار, ثابت حرف *pkg,
                                     ثابت حرف *توکی, ثابت حرف *دننه_Suffix, UBool do_Check_وروستی_Four,
                                     UError_کوډ *p_Error_کوډ)
{
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", دننه_Suffix, دننه_لار);
#د_که_پای
    /** Path **/
    إذا(دننه_لار == تش_اشاره) {
        لار = u_get_معلومات_Directory();
    } وإلا {
        لار = دننه_لار;
    }

    /** Package **/
    إذا(pkg != تش_اشاره) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_کوډ).append(pkg, *p_Error_کوډ);
#که_تعریف_شوی UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.معلومات(), packageStub.length());
#د_که_پای
    }

    /** Item **/
    basename = پیدا_کړه_Basename(توکی);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    إذا(basename == توکی) {
        بل_لار = لار;
    } وإلا {
        توکی_لار.append(توکی, static_cast<int32_t>(basename - توکی), *p_Error_کوډ);
        بل_لار = توکی_لار.معلومات();
    }
#که_تعریف_شوی UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", دننه_Suffix, (فراغ*) دننه_Suffix);
#د_که_پای

    /** Suffix  **/
    إذا(دننه_Suffix != تش_اشاره) {
        suffix = دننه_Suffix;
    } وإلا {
        suffix = "";
    }

    check_وروستی_Four = do_Check_وروستی_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#که_تعریف_شوی UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            توکی,
            لار,
            basename,
            suffix.معلومات(),
            توکی_لار.معلومات(),
            بل_لار,
            check_وروستی_Four?"true":"false");
#د_که_پای
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ثابت حرف *UData_لار_Iterator::next(UError_کوډ *p_Error_کوډ)
{
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }

    ثابت حرف *current_لار = تش_اشاره;
    int32_t     لار_Len = 0;
    ثابت حرف *لار_Basename;

    افعل
    {
        إذا( بل_لار == تش_اشاره ) {
            اكسر;
        }
        current_لار = بل_لار;

        إذا(بل_لار == توکی_لار.معلومات()) { /* we were processing item's path. */
            بل_لار = لار; /* start with regular path next tm. */
            لار_Len = static_cast<int32_t>(uprv_strlen(current_لار));
        } وإلا {
            /* fix up next for next time */
            بل_لار = uprv_strchr(current_لار, U_لار_SEP_CHAR);
            إذا(بل_لار == تش_اشاره) {
                /* segment: entire path */
                لار_Len = static_cast<int32_t>(uprv_strlen(current_لار));
            } وإلا {
                /* segment: until next segment */
                لار_Len = static_cast<int32_t>(بل_لار - current_لار);
                /* skip divider */
                بل_لار ++;
            }
        }

        إذا(لار_Len == 0) {
            استمر;
        }

#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_لار);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            لكل(qqq=0;qqq<لار_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#د_که_پای
        لار_بفر.clear().append(current_لار, لار_Len, *p_Error_کوډ);

        /* check for .dat files */
        لار_Basename = پیدا_کړه_Basename(لار_بفر.معلومات());

        إذا(check_وروستی_Four && 
           (لار_Len>=4) &&
           uprv_strncmp(لار_بفر.معلومات() +(لار_Len-4), suffix.معلومات(), 4)==0 && /* suffix matches */
           uprv_strncmp(پیدا_کړه_Basename(لار_بفر.معلومات()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(لار_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#که_تعریف_شوی UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.معلومات(), لار_بفر.معلومات());
#د_که_پای
            /* do nothing */
        }
        وإلا 
        {       /* regular dir path */
            إذا(لار_بفر[لار_Len-1] != U_FILE_SEP_CHAR) {
                إذا((لار_Len>=4) &&
                   uprv_strncmp(لار_بفر.معلومات()+(لار_Len-4), ".dat", 4) == 0)
                {
#که_تعریف_شوی UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", لار_بفر.معلومات());
#د_که_پای
                    استمر;
                }

                /* Check if it is a directory with the same name as our package */
                إذا(!packageStub.is_تشه() &&
                   (لار_Len > packageStub.length()) &&
                   !uprv_strcmp(لار_بفر.معلومات() + لار_Len - packageStub.length(), packageStub.معلومات())) {
#که_تعریف_شوی UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.معلومات(), basename, basenameLen);
#د_که_پای
                  لار_بفر.truncate(لار_Len - packageStub.length());
                }
                لار_بفر.append(U_FILE_SEP_CHAR, *p_Error_کوډ);
            }

            /* + basename */
            لار_بفر.append(packageStub.معلومات()+1, packageStub.length()-1, *p_Error_کوډ);

            إذا (!suffix.empty())  /* tack on suffix */
            {
                إذا (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    لار_بفر.ensure_پایونه_With_File_Separator(*p_Error_کوډ);
                }
                لار_بفر.append(suffix, *p_Error_کوډ);
            }
        }

#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", لار_بفر.معلومات());
#د_که_پای

        أرجع لار_بفر.معلومات();

    } طالما(لار);

    /* fell way off the end */
    أرجع تش_اشاره;
}

U_NAMESPACE_پای_ټکی

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#إذا !defined(ICU_معلومات_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
خارجي "C" ثابت معلومات_سرلیک U_معلومات_API U_ICUDATA_ثبت_ټکی;
#د_که_پای

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
open_Common_معلومات(ثابت حرف *لار,          /*  Path from OpenChoice?          */
               int32_t common_معلومات_شاخص,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_کوډ *p_Error_کوډ)
{
    UData_حافظه t_معلومات;
    ثابت حرف *لار_بفر;
    ثابت حرف *دننه_Basename;

    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }

    UData_حافظه_init(&t_معلومات);

    /* ??????? TODO revisit this */ 
    إذا (common_معلومات_شاخص >= 0) {
        /* "mini-cache" for common ICU data */
        إذا(common_معلومات_شاخص >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            أرجع تش_اشاره;
        }
        {
            Mutex قلف;
            إذا(gCommonICUDataArray[common_معلومات_شاخص] != تش_اشاره) {
                أرجع gCommonICUDataArray[common_معلومات_شاخص];
            }
#إذا !defined(ICU_معلومات_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            لكل(i = 0; i < common_معلومات_شاخص; ++i) {
                إذا(gCommonICUDataArray[i]->p_سرلیک == &U_ICUDATA_ثبت_ټکی) {
                    /* The linked-in data is already in the list. */
                    أرجع تش_اشاره;
                }
            }
#د_که_پای
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
#إذا !defined(ICU_معلومات_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        سېټ_Common_ICUData_اشاره_ګر(&U_ICUDATA_ثبت_ټکی, خطأ, p_Error_کوډ);
        {
            Mutex قلف;
            أرجع gCommonICUDataArray[common_معلومات_شاخص];
        }
#د_که_پای
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    دننه_Basename = پیدا_کړه_Basename(لار);
#که_تعریف_شوی UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", دننه_Basename);
#د_که_پای

    إذا(*دننه_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", لار);
#د_که_پای
        إذا (U_SUCCESS(*p_Error_کوډ)) {
            *p_Error_کوډ=U_FILE_ACCESS_ERROR;
        }
        أرجع تش_اشاره;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_حافظه  *معلومات_منزل_Return = udata_پیدا_کړه_Cached_معلومات(دننه_Basename, *p_Error_کوډ);
    إذا (معلومات_منزل_Return != تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع معلومات_منزل_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_لار_Iterator iter(u_get_معلومات_Directory(), دننه_Basename, لار, ".dat", ریښتیا, p_Error_کوډ);

    طالما ((UData_حافظه_is_Loaded(&t_معلومات)==خطأ) && (لار_بفر = iter.next(p_Error_کوډ)) != تش_اشاره)
    {
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", لار_بفر);
#د_که_پای
        uprv_mapFile(&t_معلومات, لار_بفر, p_Error_کوډ);
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_حافظه_is_Loaded(&t_معلومات)?"LOADED":"not loaded");
#د_که_پای
    }
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }

    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا (!UData_حافظه_is_Loaded(&t_معلومات)) {
        /* no common data */
        *p_Error_کوډ=U_FILE_ACCESS_ERROR;
        أرجع تش_اشاره;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_معلومات(&t_معلومات, p_Error_کوډ);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    أرجع udata_cache_معلومات_توکی(دننه_Basename, &t_معلومات, p_Error_کوډ);
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
ساكن UBool extendICUData(UError_کوډ *pErr)
{
    UData_حافظه   *p_معلومات;
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
    umtx_قلف(&extendICUDataMutex);
#د_که_پای
    إذا(!umtx_بارول_Acquire(g_Have_Tried_منزل_بارول_Common_معلومات)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_معلومات = open_Common_معلومات(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_حافظه_init(&copyPData);
       إذا(p_معلومات != تش_اشاره) {
          UDatamemory_ټاکل(&copyPData, p_معلومات);
          copyPData.map = تش_اشاره;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = تش_اشاره; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              سېټ_Common_ICUData(&copyPData,/*  The new common data.                                */
                       خطأ,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_منزل_بارول_Common_معلومات, 1);
    }

    didUpdate = پیدا_کړه_Common_ICUData_By_Name(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#إذا MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#د_که_پای
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
udata_سېټ_Common_معلومات(ثابت فراغ *معلومات, UError_کوډ *p_Error_کوډ) {
    UData_حافظه معلومات_حافظه;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }

    إذا(معلومات==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    /* set the data pointer and test for validity */
    UData_حافظه_init(&معلومات_حافظه);
    UData_حافظه_سېټ_معلومات(&معلومات_حافظه, معلومات);
    udata_check_Common_معلومات(&معلومات_حافظه, p_Error_کوډ);
    إذا (U_FAILURE(*p_Error_کوډ)) {أرجع;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    سېټ_Common_ICUData(&معلومات_حافظه, ریښتیا, p_Error_کوډ);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI فراغ U_EXPORT2
udata_سېټ_App_معلومات(ثابت حرف *لار, ثابت فراغ *معلومات, UError_کوډ *err)
{
    UData_حافظه     udm;

    إذا(err==تش_اشاره || U_FAILURE(*err)) {
        أرجع;
    }
    إذا(معلومات==تش_اشاره) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    UData_حافظه_init(&udm);
    UData_حافظه_سېټ_معلومات(&udm, معلومات);
    udata_check_Common_معلومات(&udm, err);
    udata_cache_معلومات_توکی(لار, &udm, err);
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
check_معلومات_توکی
(
 ثابت معلومات_سرلیک         *p_سرلیک,         /* The data item to be checked.                */
 UData_حافظه_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 فراغ                     *context,         /*   pass-thru param for above.                */
 ثابت حرف               *type,            /*   pass-thru param for above.                */
 ثابت حرف               *name,            /*   pass-thru param for above.                */
 UError_کوډ               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_کوډ               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_حافظه  *r_معلومات_Mem = تش_اشاره;          /* the new UDataMemory, to be returned.        */

    إذا (U_FAILURE(*fatalErr)) {
        أرجع تش_اشاره;
    }

    إذا(p_سرلیک->معلومات_سرلیک_2.magic1==0xda &&
        p_سرلیک->معلومات_سرلیک_2.magic2==0x27 &&
        (isAcceptable==تش_اشاره || isAcceptable(context, type, name, &p_سرلیک->info))
    ) {
        r_معلومات_Mem=UData_حافظه_create_New_Instance(fatalErr);
        إذا (U_FAILURE(*fatalErr)) {
            أرجع تش_اشاره;
        }
        r_معلومات_Mem->p_سرلیک = p_سرلیک;
    } وإلا {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    أرجع r_معلومات_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساكن UData_حافظه *do_بارول_From_Individual_Files(ثابت حرف *pkgName, 
        ثابت حرف *معلومات_لار, ثابت حرف *toc_ثبت_لار_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *لار, ثابت حرف *type, ثابت حرف *name,
             UData_حافظه_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کوډ *sub_Error_کوډ,
             UError_کوډ *p_Error_کوډ)
{
    ثابت حرف         *لار_بفر;
    UData_حافظه         معلومات_حافظه;
    UData_حافظه *p_ثبت_معلومات;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_لار_Iterator iter(معلومات_لار, pkgName, لار, toc_ثبت_لار_Suffix, خطأ, p_Error_کوډ);

    طالما ((لار_بفر = iter.next(p_Error_کوډ)) != تش_اشاره)
    {
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", لار_بفر);
#د_که_پای
        إذا (uprv_mapFile(&معلومات_حافظه, لار_بفر, p_Error_کوډ))
        {
            p_ثبت_معلومات = check_معلومات_توکی(معلومات_حافظه.p_سرلیک, isAcceptable, context, type, name, sub_Error_کوډ, p_Error_کوډ);
            إذا (p_ثبت_معلومات != تش_اشاره) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_ثبت_معلومات->mapAddr = معلومات_حافظه.mapAddr;
                p_ثبت_معلومات->map     = معلومات_حافظه.map;
                p_ثبت_معلومات->length  = معلومات_حافظه.length;

#که_تعریف_شوی UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", لار_بفر);
#د_که_پای
                أرجع p_ثبت_معلومات;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&معلومات_حافظه);

            /* If we had a nasty error, bail out completely.  */
            إذا (U_FAILURE(*p_Error_کوډ)) {
                أرجع تش_اشاره;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_کوډ=U_INVALID_FORMAT_ERROR;
        }
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_حافظه_is_Loaded(&معلومات_حافظه)?"LOADED":"not loaded");
#د_که_پای
    }
    أرجع تش_اشاره;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساكن UData_حافظه *do_بارول_From_Common_معلومات(UBool isICUData, ثابت حرف * /*pkgName*/, 
        ثابت حرف * /*dataPath*/, ثابت حرف * /*tocEntryPathSuffix*/, ثابت حرف *toc_ثبت_Name,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *لار, ثابت حرف *type, ثابت حرف *name,
             UData_حافظه_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کوډ *sub_Error_کوډ,
             UError_کوډ *p_Error_کوډ)
{
    UData_حافظه        *p_ثبت_معلومات;
    ثابت معلومات_سرلیک   *p_سرلیک;
    UData_حافظه        *p_Common_معلومات;
    int32_t            common_معلومات_شاخص;
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
    لكل (common_معلومات_شاخص = isICUData ? 0 : -1;;) {
        p_Common_معلومات=open_Common_معلومات(لار, common_معلومات_شاخص, sub_Error_کوډ); /** search for pkg **/

        إذا(U_SUCCESS(*sub_Error_کوډ) && p_Common_معلومات!=تش_اشاره) {
            int32_t length;

            /* look up the data piece in the common data */
            p_سرلیک=p_Common_معلومات->vFuncs->Lookup(p_Common_معلومات, toc_ثبت_Name, &length, sub_Error_کوډ);
#که_تعریف_شوی UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_ثبت_Name, (فراغ*) p_سرلیک, u_errorName(*sub_Error_کوډ));
#د_که_پای

            إذا(p_سرلیک!=تش_اشاره) {
                p_ثبت_معلومات = check_معلومات_توکی(p_سرلیک, isAcceptable, context, type, name, sub_Error_کوډ, p_Error_کوډ);
#که_تعریف_شوی UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (فراغ*) p_ثبت_معلومات);
#د_که_پای
                إذا (U_FAILURE(*p_Error_کوډ)) {
                    أرجع تش_اشاره;
                }
                إذا (p_ثبت_معلومات != تش_اشاره) {
                    p_ثبت_معلومات->length = length;
                    أرجع p_ثبت_معلومات;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        إذا (*sub_Error_کوډ == U_حافظه_ویش_ERROR) {
            *p_Error_کوډ = *sub_Error_کوډ;
            أرجع تش_اشاره;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        إذا (!isICUData) {
            أرجع تش_اشاره;
        } وإلا إذا (p_Common_معلومات != تش_اشاره) {
            ++common_معلومات_شاخص;  /* try the next data package */
        } وإلا إذا ((!checkedExtendedICUData) && extendICUData(sub_Error_کوډ)) {
            checkedExtendedICUData = ریښتیا;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } وإلا {
            أرجع تش_اشاره;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
ساكن UBool is_وخت_Zone_File(ثابت حرف *name, ثابت حرف *type) {
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
doOpenChoice(ثابت حرف *لار, ثابت حرف *type, ثابت حرف *name,
             UData_حافظه_Is_Acceptable *isAcceptable, فراغ *context,
             UError_کوډ *p_Error_کوډ)
{
    UData_حافظه         *retVal = تش_اشاره;

    ثابت حرف         *معلومات_لار;

    int32_t             toc_ثبت_Suffix_شاخص;
    ثابت حرف         *toc_ثبت_لار_Suffix;
    UError_کوډ          sub_Error_کوډ=U_ZERO_ERROR;
    ثابت حرف         *ونه_Char;

    UBool               isICUData = خطأ;


    FileTracer::traceOpen(لار, type, name);


    /* Is this path ICU data? */
    إذا(لار == تش_اشاره ||
       !strcmp(لار, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(لار, U_ICUDATA_NAME U_ونه_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_ونه_SEPARATOR_STRING)) ||  
       !uprv_strncmp(لار, U_ICUDATA_ALIAS U_ونه_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_ونه_SEPARATOR_STRING))) {
      isICUData = ریښتیا;
    }

#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_لار;
    إذا(لار) {
        إذا(uprv_strchr(لار,U_FILE_ALT_SEP_CHAR) != تش_اشاره) {
            alt_Sep_لار.append(لار, *p_Error_کوډ);
            حرف *p;
            طالما ((p = uprv_strchr(alt_Sep_لار.معلومات(), U_FILE_ALT_SEP_CHAR)) != تش_اشاره) {
                *p = U_FILE_SEP_CHAR;
            }
#إذا defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", لار, alt_Sep_لار.معلومات());
#د_که_پای
            لار = alt_Sep_لار.معلومات();
        }
    }
#د_که_پای

    CharString toc_ثبت_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_ثبت_لار; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString ونه_Name;

    /* ======= Set up strings */
    إذا(لار==تش_اشاره) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_کوډ);
    } وإلا {
        ثابت حرف *pkg;
        ثابت حرف *first;
        pkg = uprv_strrchr(لار, U_FILE_SEP_CHAR);
        first = uprv_strchr(لار, U_FILE_SEP_CHAR);
        إذا(uprv_لار_Is_Absolute(لار) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            إذا(pkg) {
                pkgName.append(pkg+1, *p_Error_کوډ);
            } وإلا {
                pkgName.append(لار, *p_Error_کوډ);
            }
        } وإلا {
            ونه_Char = uprv_strchr(لار, U_ونه_SEPARATOR);
            إذا(ونه_Char) { 
                ونه_Name.append(ونه_Char+1, *p_Error_کوډ); /* following '-' */
                إذا(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_کوډ);
                } وإلا {
                    pkgName.append(لار, static_cast<int32_t>(ونه_Char - لار), *p_Error_کوډ);
                    إذا (first == تش_اشاره) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        لار = pkgName.معلومات();
                    }
                }
            } وإلا {
                إذا(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_کوډ);
                } وإلا {
                    pkgName.append(لار, *p_Error_کوډ);
                }
            }
        }
    }

#که_تعریف_شوی UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.معلومات(), ونه_Name.معلومات());
#د_که_پای

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_ثبت_Name.append(pkgName, *p_Error_کوډ);
    toc_ثبت_لار.append(pkgName, *p_Error_کوډ);
    toc_ثبت_Suffix_شاخص = toc_ثبت_Name.length();

    إذا(!ونه_Name.is_تشه()) {
        toc_ثبت_Name.append(U_ونه_ثبت_SEP_CHAR, *p_Error_کوډ).append(ونه_Name, *p_Error_کوډ);
        toc_ثبت_لار.append(U_FILE_SEP_CHAR, *p_Error_کوډ).append(ونه_Name, *p_Error_کوډ);
    }

    toc_ثبت_Name.append(U_ونه_ثبت_SEP_CHAR, *p_Error_کوډ).append(name, *p_Error_کوډ);
    toc_ثبت_لار.append(U_FILE_SEP_CHAR, *p_Error_کوډ).append(name, *p_Error_کوډ);
    إذا(type!=تش_اشاره && *type!=0) {
        toc_ثبت_Name.append(".", *p_Error_کوډ).append(type, *p_Error_کوډ);
        toc_ثبت_لار.append(".", *p_Error_کوډ).append(type, *p_Error_کوډ);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_ثبت_لار_Suffix = toc_ثبت_لار.معلومات() + toc_ثبت_Suffix_شاخص + 1; /* suffix starts here */

#که_تعریف_شوی UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_ثبت_Name.معلومات());
    fprintf(stderr, " tocEntryPath = %s\n", toc_ثبت_لار.معلومات());
#د_که_پای

#إذا !defined(ICU_معلومات_DIR_WINDOWS)
    إذا(لار == تش_اشاره) {
        لار = COMMON_معلومات_NAME; /* "icudt26e" */
    }
#وإلا
    // When using the Windows system data, we expects only a single data file.
    لار = COMMON_معلومات_NAME; /* "icudt26e" */
#د_که_پای

    /************************ Begin loop looking for ind. files ***************/
#که_تعریف_شوی UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_لار(لار));
#د_که_پای

    /* End of dealing with a null basename */
    معلومات_لار = u_get_معلومات_Directory();

    /****    Time zone individual files override  */
    إذا (isICUData && type != تش_اشاره && is_وخت_Zone_File(name, type)) {
        ثابت حرف *tzFilesDir = u_get_وخت_Zone_Files_Directory(p_Error_کوډ);
        إذا (tzFilesDir[0] != 0) {
#که_تعریف_شوی UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#د_که_پای
            retVal = do_بارول_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_ثبت_لار_Suffix,
                            /*لار*/ "", type, name, isAcceptable, context, &sub_Error_کوډ, p_Error_کوډ);
            إذا((retVal != تش_اشاره) || U_FAILURE(*p_Error_کوډ)) {
                أرجع retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    إذا(g_معلومات_File_Access == UDATA_PACKAGES_لومړی) {
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#د_که_پای
        /* #2 */
        retVal = do_بارول_From_Common_معلومات(isICUData, 
                            pkgName.معلومات(), معلومات_لار, toc_ثبت_لار_Suffix, toc_ثبت_Name.معلومات(),
                            لار, type, name, isAcceptable, context, &sub_Error_کوډ, p_Error_کوډ);
        إذا((retVal != تش_اشاره) || U_FAILURE(*p_Error_کوډ)) {
            أرجع retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    إذا((g_معلومات_File_Access==UDATA_PACKAGES_لومړی) ||
       (g_معلومات_File_Access==UDATA_FILES_لومړی)) {
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#د_که_پای
        /* Check to make sure that there is a dataPath to iterate over */
        إذا ((معلومات_لار && *معلومات_لار) || !isICUData) {
            retVal = do_بارول_From_Individual_Files(pkgName.معلومات(), معلومات_لار, toc_ثبت_لار_Suffix,
                            لار, type, name, isAcceptable, context, &sub_Error_کوډ, p_Error_کوډ);
            إذا((retVal != تش_اشاره) || U_FAILURE(*p_Error_کوډ)) {
                أرجع retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    إذا((g_معلومات_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_معلومات_File_Access==UDATA_FILES_لومړی)) {
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#د_که_پای
        retVal = do_بارول_From_Common_معلومات(isICUData,
                            pkgName.معلومات(), معلومات_لار, toc_ثبت_لار_Suffix, toc_ثبت_Name.معلومات(),
                            لار, type, name, isAcceptable, context, &sub_Error_کوډ, p_Error_کوډ);
        إذا((retVal != تش_اشاره) || U_FAILURE(*p_Error_کوډ)) {
            أرجع retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    إذا(g_معلومات_File_Access==UDATA_NO_FILES) {
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#د_که_پای
        retVal = do_بارول_From_Common_معلومات(isICUData,
                            pkgName.معلومات(), "", toc_ثبت_لار_Suffix, toc_ثبت_Name.معلومات(),
                            لار, type, name, isAcceptable, context, &sub_Error_کوډ, p_Error_کوډ);
        إذا((retVal != تش_اشاره) || U_FAILURE(*p_Error_کوډ)) {
            أرجع retVal;
        }
    }

    /* data not found */
    إذا(U_SUCCESS(*p_Error_کوډ)) {
        إذا(U_SUCCESS(sub_Error_کوډ)) {
            /* file not found */
            *p_Error_کوډ=U_FILE_ACCESS_ERROR;
        } وإلا {
            /* entry point not found or rejected */
            *p_Error_کوډ=sub_Error_کوډ;
        }
    }
    أرجع retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_حافظه * U_EXPORT2
udata_open(ثابت حرف *لار, ثابت حرف *type, ثابت حرف *name,
           UError_کوډ *p_Error_کوډ) {
#که_تعریف_شوی UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (لار?لار:"nullptr"), name, type);
    fflush(stderr);
#د_که_پای

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    } وإلا إذا(name==تش_اشاره || *name==0) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    } وإلا {
        أرجع doOpenChoice(لار, type, name, تش_اشاره, تش_اشاره, p_Error_کوډ);
    }
}



U_CAPI UData_حافظه * U_EXPORT2
udata_openChoice(ثابت حرف *لار, ثابت حرف *type, ثابت حرف *name,
                 UData_حافظه_Is_Acceptable *isAcceptable, فراغ *context,
                 UError_کوډ *p_Error_کوډ) {
#که_تعریف_شوی UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (لار?لار:"nullptr"), name, type);
#د_که_پای

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    } وإلا إذا(name==تش_اشاره || *name==0 || isAcceptable==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    } وإلا {
        أرجع doOpenChoice(لار, type, name, isAcceptable, context, p_Error_کوډ);
    }
}



U_CAPI فراغ U_EXPORT2
udata_getInfo(UData_حافظه *p_معلومات, UDataInfo *pInfo) {
    إذا(pInfo!=تش_اشاره) {
        إذا(p_معلومات!=تش_اشاره && p_معلومات->p_سرلیک!=تش_اشاره) {
            ثابت UDataInfo *info=&p_معلومات->p_سرلیک->info;
            uint16_t معلومات_Info_اندازه=udata_get_Info_اندازه(info);
            إذا(pInfo->size>معلومات_Info_اندازه) {
                pInfo->size=معلومات_Info_اندازه;
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


U_CAPI فراغ U_EXPORT2 udata_سېټ_File_Access(UDataFileAccess access, UError_کوډ * /*حالت*/)
{
    // Note: this function is documented as not thread safe.
    g_معلومات_File_Access = access;
}
