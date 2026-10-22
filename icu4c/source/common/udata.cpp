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

#جيڪڏهن_وصف_ٿيل __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#جيڪڏهن_پورو

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
#جيڪڏهن_پورو

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
ساكن UData_يادگيري *udata_ڳوليو_Cached_معلومات(ثابت حرف *رستو, UError_ڪوڊ &err);

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
ساكن UData_يادگيري *gCommonICUDataArray[10] = { خالي_اشارو };   // Access protected by icu global mutex.

ساكن u_atomic_int32_t g_Have_Tried_منزل_ڀريو_Common_معلومات {0};  //  See extendICUData().

ساكن UHashtable  *g_Common_معلومات_Cache = خالي_اشارو;  /* Global hash table of opened ICU data files.  */
ساكن icu::UInitOnce g_Common_معلومات_Cache_Init_Once {};

#إذا !defined(ICU_معلومات_DIR_WINDOWS)
ساكن UDataFileAccess  g_معلومات_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#وإلا
// If we are using the Windows data directory, then look in one spot only.
ساكن UDataFileAccess  g_معلومات_File_Access = UDATA_NO_FILES;
#جيڪڏهن_پورو

ساكن UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    إذا (g_Common_معلومات_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_معلومات_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_معلومات_Cache = خالي_اشارو;        /*   Cleanup is not thread safe.                */
    }
    g_Common_معلومات_Cache_Init_Once.reset();

    لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != خالي_اشارو; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = خالي_اشارو;
    }
    g_Have_Tried_منزل_ڀريو_Common_معلومات = 0;

    أرجع سچ;                   /* Everything was cleaned up */
}

ساكن UBool U_CALLCONV
ڳوليو_Common_ICUData_By_Name(ثابت حرف *اندريون_Basename, UError_ڪوڊ &err)
{
    UBool مليو = خطأ;
    int32_t i;

    UData_يادگيري  *p_معلومات = udata_ڳوليو_Cached_معلومات(اندريون_Basename, err);
    إذا (U_FAILURE(err) || p_معلومات == خالي_اشارو)
        أرجع خطأ;

    {
        Mutex تالو;
        لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            إذا ((gCommonICUDataArray[i] != خالي_اشارو) && (gCommonICUDataArray[i]->p_سرو == p_معلومات->p_سرو)) {
                /* The data pointer is already in the array. */
                مليو = سچ;
                اكسر;
            }
        }
    }
    أرجع مليو;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
ساكن UBool
سيٽ_Common_ICUData(UData_يادگيري *p_معلومات,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_ڪوڊ *pErr)
{
    UData_يادگيري  *new_Common_معلومات = UData_يادگيري_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = خطأ;
    إذا (U_FAILURE(*pErr)) {
        أرجع خطأ;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_سونپيو(new_Common_معلومات, p_معلومات);
    umtx_تالو(خالي_اشارو);
    لكل (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        إذا (gCommonICUDataArray[i] == خالي_اشارو) {
            gCommonICUDataArray[i] = new_Common_معلومات;
            didUpdate = سچ;
            اكسر;
        } وإلا إذا (gCommonICUDataArray[i]->p_سرو == p_معلومات->p_سرو) {
            /* The same data pointer is already in the array. */
            اكسر;
        }
    }
    umtx_unlock(خالي_اشارو);

    إذا (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    إذا (didUpdate) {
        ucln_common_رجسٽر_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } وإلا {
        uprv_free(new_Common_معلومات);
    }
    أرجع didUpdate;
}

#إذا !defined(ICU_معلومات_DIR_WINDOWS)

ساكن UBool
سيٽ_Common_ICUData_اشارو(ثابت فراغ *p_معلومات, UBool /*warn*/, UError_ڪوڊ *p_Error_ڪوڊ) {
    UData_يادگيري t_معلومات;
    UData_يادگيري_init(&t_معلومات);
    UData_يادگيري_سيٽ_معلومات(&t_معلومات, p_معلومات);
    udata_check_Common_معلومات(&t_معلومات, p_Error_ڪوڊ);
    أرجع سيٽ_Common_ICUData(&t_معلومات, خطأ, p_Error_ڪوڊ);
}

#جيڪڏهن_پورو

ساكن ثابت حرف *
ڳوليو_Basename(ثابت حرف *رستو) {
    ثابت حرف *basename=uprv_strrchr(رستو, U_FILE_SEP_CHAR);
    إذا(basename==خالي_اشارو) {
        أرجع رستو;
    } وإلا {
        أرجع basename+1;
    }
}

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
ساكن ثابت حرف *
package_Name_From_رستو(ثابت حرف *رستو)
{
    إذا((رستو == خالي_اشارو) || (*رستو == 0)) {
        أرجع U_ICUDATA_NAME;
    }

    رستو = ڳوليو_Basename(رستو);

    إذا((رستو == خالي_اشارو) || (*رستو == 0)) {
        أرجع U_ICUDATA_NAME;
    }

    أرجع رستو;
}
#جيڪڏهن_پورو

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
    UData_يادگيري   *شي;
} معلومات_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
ساكن فراغ U_CALLCONV معلومات_Cache_Element_deleter(فراغ *pDCEl) {
    معلومات_Cache_Element* p = static_cast<معلومات_Cache_Element*>(pDCEl);
    udata_close(p->شي);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

ساكن فراغ U_CALLCONV udata_init_Hash_جدول(UError_ڪوڊ &err) {
    U_ASSERT(g_Common_معلومات_Cache == خالي_اشارو);
    g_Common_معلومات_Cache = uhash_open(uhash_hashChars, uhash_compareChars, خالي_اشارو, &err);
    إذا (U_FAILURE(err)) {
       أرجع;
    }
    U_ASSERT(g_Common_معلومات_Cache != خالي_اشارو);
    uhash_سيٽ_قدر_Deleter(g_Common_معلومات_Cache, معلومات_Cache_Element_deleter);
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
ساكن UHashtable *udata_get_Hash_جدول(UError_ڪوڊ &err) {
    umtx_initOnce(g_Common_معلومات_Cache_Init_Once, &udata_init_Hash_جدول, err);
    أرجع g_Common_معلومات_Cache;
}



ساكن UData_يادگيري *udata_ڳوليو_Cached_معلومات(ثابت حرف *رستو, UError_ڪوڊ &err)
{
    UHashtable        *htable;
    UData_يادگيري       *retVal = خالي_اشارو;
    معلومات_Cache_Element  *el;
    ثابت حرف        *بنياد_Name;

    htable = udata_get_Hash_جدول(err);
    إذا (U_FAILURE(err)) {
        أرجع خالي_اشارو;
    }

    بنياد_Name = ڳوليو_Basename(رستو);   /* Cache remembers only the base name, not the full path. */
    umtx_تالو(خالي_اشارو);
    el = static_cast<معلومات_Cache_Element*>(uhash_get(htable, بنياد_Name));
    umtx_unlock(خالي_اشارو);
    إذا (el != خالي_اشارو) {
        retVal = el->شي;
    }
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", بنياد_Name, (فراغ*) retVal);
#جيڪڏهن_پورو
    أرجع retVal;
}


ساكن UData_يادگيري *udata_cache_معلومات_شي(ثابت حرف *رستو, UData_يادگيري *شي, UError_ڪوڊ *pErr) {
    معلومات_Cache_Element *newElement;
    ثابت حرف       *بنياد_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    معلومات_Cache_Element *old_قدر = خالي_اشارو;
    UError_ڪوڊ        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_جدول(*pErr);
    إذا (U_FAILURE(*pErr)) {
        أرجع خالي_اشارو;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<معلومات_Cache_Element*>(uprv_malloc(حجم(معلومات_Cache_Element)));
    إذا (newElement == خالي_اشارو) {
        *pErr = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    newElement->شي = UData_يادگيري_create_New_Instance(pErr);
    إذا (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        أرجع خالي_اشارو;
    }
    UDatamemory_سونپيو(newElement->شي, شي);

    بنياد_Name = ڳوليو_Basename(رستو);
    nameLen = static_cast<int32_t>(uprv_strlen(بنياد_Name));
    newElement->name = static_cast<حرف*>(uprv_malloc(nameLen + 1));
    إذا (newElement->name == خالي_اشارو) {
        *pErr = U_يادگيري_ورهاست_ERROR;
        uprv_free(newElement->شي);
        uprv_free(newElement);
        أرجع خالي_اشارو;
    }
    uprv_strcpy(newElement->name, بنياد_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_تالو(خالي_اشارو);
    old_قدر = static_cast<معلومات_Cache_Element*>(uhash_get(htable, رستو));
    إذا (old_قدر != خالي_اشارو) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    وإلا {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(خالي_اشارو);

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (فراغ*) newElement->شي, u_errorName(subErr), (فراغ*) newElement->شي->vFuncs);
#جيڪڏهن_پورو

    إذا (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->شي);
        uprv_free(newElement);
        أرجع old_قدر ? old_قدر->شي : خالي_اشارو;
    }

    أرجع newElement->شي;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

صنف UData_رستو_Iterator
{
عام:
    UData_رستو_Iterator(ثابت حرف *رستو, ثابت حرف *pkg,
                      ثابت حرف *شي, ثابت حرف *suffix, UBool do_Check_آخري_Four,
                      UError_ڪوڊ *p_Error_ڪوڊ);
    ثابت حرف *next(UError_ڪوڊ *p_Error_ڪوڊ);

خاص:
    ثابت حرف *رستو;                              /* working path (u_icudata_Dir) */
    ثابت حرف *ايندڙ_رستو;                          /* path following this one */
    ثابت حرف *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  شي_رستو;                          /* path passed in with item name */
    CharString  رستو_بفر;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       check_آخري_Four;                     /* if true then allow paths such as '/foo/myapp.dat'
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
UData_رستو_Iterator::UData_رستو_Iterator(ثابت حرف *اندريون_رستو, ثابت حرف *pkg,
                                     ثابت حرف *شي, ثابت حرف *اندريون_Suffix, UBool do_Check_آخري_Four,
                                     UError_ڪوڊ *p_Error_ڪوڊ)
{
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", اندريون_Suffix, اندريون_رستو);
#جيڪڏهن_پورو
    /** Path **/
    إذا(اندريون_رستو == خالي_اشارو) {
        رستو = u_get_معلومات_Directory();
    } وإلا {
        رستو = اندريون_رستو;
    }

    /** Package **/
    إذا(pkg != خالي_اشارو) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_ڪوڊ).append(pkg, *p_Error_ڪوڊ);
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.معلومات(), packageStub.length());
#جيڪڏهن_پورو
    }

    /** Item **/
    basename = ڳوليو_Basename(شي);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    إذا(basename == شي) {
        ايندڙ_رستو = رستو;
    } وإلا {
        شي_رستو.append(شي, static_cast<int32_t>(basename - شي), *p_Error_ڪوڊ);
        ايندڙ_رستو = شي_رستو.معلومات();
    }
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", اندريون_Suffix, (فراغ*) اندريون_Suffix);
#جيڪڏهن_پورو

    /** Suffix  **/
    إذا(اندريون_Suffix != خالي_اشارو) {
        suffix = اندريون_Suffix;
    } وإلا {
        suffix = "";
    }

    check_آخري_Four = do_Check_آخري_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            شي,
            رستو,
            basename,
            suffix.معلومات(),
            شي_رستو.معلومات(),
            ايندڙ_رستو,
            check_آخري_Four?"true":"false");
#جيڪڏهن_پورو
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ثابت حرف *UData_رستو_Iterator::next(UError_ڪوڊ *p_Error_ڪوڊ)
{
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    ثابت حرف *current_رستو = خالي_اشارو;
    int32_t     رستو_Len = 0;
    ثابت حرف *رستو_Basename;

    افعل
    {
        إذا( ايندڙ_رستو == خالي_اشارو ) {
            اكسر;
        }
        current_رستو = ايندڙ_رستو;

        إذا(ايندڙ_رستو == شي_رستو.معلومات()) { /* we were processing item's path. */
            ايندڙ_رستو = رستو; /* start with regular path next tm. */
            رستو_Len = static_cast<int32_t>(uprv_strlen(current_رستو));
        } وإلا {
            /* fix up next for next time */
            ايندڙ_رستو = uprv_strchr(current_رستو, U_رستو_SEP_CHAR);
            إذا(ايندڙ_رستو == خالي_اشارو) {
                /* segment: entire path */
                رستو_Len = static_cast<int32_t>(uprv_strlen(current_رستو));
            } وإلا {
                /* segment: until next segment */
                رستو_Len = static_cast<int32_t>(ايندڙ_رستو - current_رستو);
                /* skip divider */
                ايندڙ_رستو ++;
            }
        }

        إذا(رستو_Len == 0) {
            استمر;
        }

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_رستو);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            لكل(qqq=0;qqq<رستو_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#جيڪڏهن_پورو
        رستو_بفر.clear().append(current_رستو, رستو_Len, *p_Error_ڪوڊ);

        /* check for .dat files */
        رستو_Basename = ڳوليو_Basename(رستو_بفر.معلومات());

        إذا(check_آخري_Four && 
           (رستو_Len>=4) &&
           uprv_strncmp(رستو_بفر.معلومات() +(رستو_Len-4), suffix.معلومات(), 4)==0 && /* suffix matches */
           uprv_strncmp(ڳوليو_Basename(رستو_بفر.معلومات()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(رستو_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.معلومات(), رستو_بفر.معلومات());
#جيڪڏهن_پورو
            /* do nothing */
        }
        وإلا 
        {       /* regular dir path */
            إذا(رستو_بفر[رستو_Len-1] != U_FILE_SEP_CHAR) {
                إذا((رستو_Len>=4) &&
                   uprv_strncmp(رستو_بفر.معلومات()+(رستو_Len-4), ".dat", 4) == 0)
                {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", رستو_بفر.معلومات());
#جيڪڏهن_پورو
                    استمر;
                }

                /* Check if it is a directory with the same name as our package */
                إذا(!packageStub.is_خالي_جايون() &&
                   (رستو_Len > packageStub.length()) &&
                   !uprv_strcmp(رستو_بفر.معلومات() + رستو_Len - packageStub.length(), packageStub.معلومات())) {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.معلومات(), basename, basenameLen);
#جيڪڏهن_پورو
                  رستو_بفر.truncate(رستو_Len - packageStub.length());
                }
                رستو_بفر.append(U_FILE_SEP_CHAR, *p_Error_ڪوڊ);
            }

            /* + basename */
            رستو_بفر.append(packageStub.معلومات()+1, packageStub.length()-1, *p_Error_ڪوڊ);

            إذا (!suffix.empty())  /* tack on suffix */
            {
                إذا (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    رستو_بفر.ensure_پڇاڙيون_With_File_Separator(*p_Error_ڪوڊ);
                }
                رستو_بفر.append(suffix, *p_Error_ڪوڊ);
            }
        }

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", رستو_بفر.معلومات());
#جيڪڏهن_پورو

        أرجع رستو_بفر.معلومات();

    } طالما(رستو);

    /* fell way off the end */
    أرجع خالي_اشارو;
}

U_NAMESPACE_پڇاڙي

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
خارجي "C" ثابت معلومات_سرو U_معلومات_API U_ICUDATA_داخلا_نقطو;
#جيڪڏهن_پورو

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
ساكن UData_يادگيري *
open_Common_معلومات(ثابت حرف *رستو,          /*  Path from OpenChoice?          */
               int32_t common_معلومات_اشاريو,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_ڪوڊ *p_Error_ڪوڊ)
{
    UData_يادگيري t_معلومات;
    ثابت حرف *رستو_بفر;
    ثابت حرف *اندريون_Basename;

    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    UData_يادگيري_init(&t_معلومات);

    /* ??????? TODO revisit this */ 
    إذا (common_معلومات_اشاريو >= 0) {
        /* "mini-cache" for common ICU data */
        إذا(common_معلومات_اشاريو >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            أرجع خالي_اشارو;
        }
        {
            Mutex تالو;
            إذا(gCommonICUDataArray[common_معلومات_اشاريو] != خالي_اشارو) {
                أرجع gCommonICUDataArray[common_معلومات_اشاريو];
            }
#إذا !defined(ICU_معلومات_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            لكل(i = 0; i < common_معلومات_اشاريو; ++i) {
                إذا(gCommonICUDataArray[i]->p_سرو == &U_ICUDATA_داخلا_نقطو) {
                    /* The linked-in data is already in the list. */
                    أرجع خالي_اشارو;
                }
            }
#جيڪڏهن_پورو
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
        سيٽ_Common_ICUData_اشارو(&U_ICUDATA_داخلا_نقطو, خطأ, p_Error_ڪوڊ);
        {
            Mutex تالو;
            أرجع gCommonICUDataArray[common_معلومات_اشاريو];
        }
#جيڪڏهن_پورو
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    اندريون_Basename = ڳوليو_Basename(رستو);
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", اندريون_Basename);
#جيڪڏهن_پورو

    إذا(*اندريون_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", رستو);
#جيڪڏهن_پورو
        إذا (U_SUCCESS(*p_Error_ڪوڊ)) {
            *p_Error_ڪوڊ=U_FILE_ACCESS_ERROR;
        }
        أرجع خالي_اشارو;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_يادگيري  *معلومات_منزل_Return = udata_ڳوليو_Cached_معلومات(اندريون_Basename, *p_Error_ڪوڊ);
    إذا (معلومات_منزل_Return != خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع معلومات_منزل_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_رستو_Iterator iter(u_get_معلومات_Directory(), اندريون_Basename, رستو, ".dat", سچ, p_Error_ڪوڊ);

    طالما ((UData_يادگيري_is_Loaded(&t_معلومات)==خطأ) && (رستو_بفر = iter.next(p_Error_ڪوڊ)) != خالي_اشارو)
    {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", رستو_بفر);
#جيڪڏهن_پورو
        uprv_mapFile(&t_معلومات, رستو_بفر, p_Error_ڪوڊ);
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_يادگيري_is_Loaded(&t_معلومات)?"LOADED":"not loaded");
#جيڪڏهن_پورو
    }
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا (!UData_يادگيري_is_Loaded(&t_معلومات)) {
        /* no common data */
        *p_Error_ڪوڊ=U_FILE_ACCESS_ERROR;
        أرجع خالي_اشارو;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_معلومات(&t_معلومات, p_Error_ڪوڊ);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    أرجع udata_cache_معلومات_شي(اندريون_Basename, &t_معلومات, p_Error_ڪوڊ);
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
ساكن UBool extendICUData(UError_ڪوڊ *pErr)
{
    UData_يادگيري   *p_معلومات;
    UData_يادگيري   copyPData;
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
    umtx_تالو(&extendICUDataMutex);
#جيڪڏهن_پورو
    إذا(!umtx_ڀريو_Acquire(g_Have_Tried_منزل_ڀريو_Common_معلومات)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_معلومات = open_Common_معلومات(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_يادگيري_init(&copyPData);
       إذا(p_معلومات != خالي_اشارو) {
          UDatamemory_سونپيو(&copyPData, p_معلومات);
          copyPData.map = خالي_اشارو;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = خالي_اشارو; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              سيٽ_Common_ICUData(&copyPData,/*  The new common data.                                */
                       خطأ,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_منزل_ڀريو_Common_معلومات, 1);
    }

    didUpdate = ڳوليو_Common_ICUData_By_Name(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#إذا MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#جيڪڏهن_پورو
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
udata_سيٽ_Common_معلومات(ثابت فراغ *معلومات, UError_ڪوڊ *p_Error_ڪوڊ) {
    UData_يادگيري معلومات_يادگيري;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }

    إذا(معلومات==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    /* set the data pointer and test for validity */
    UData_يادگيري_init(&معلومات_يادگيري);
    UData_يادگيري_سيٽ_معلومات(&معلومات_يادگيري, معلومات);
    udata_check_Common_معلومات(&معلومات_يادگيري, p_Error_ڪوڊ);
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {أرجع;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    سيٽ_Common_ICUData(&معلومات_يادگيري, سچ, p_Error_ڪوڊ);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI فراغ U_EXPORT2
udata_سيٽ_App_معلومات(ثابت حرف *رستو, ثابت فراغ *معلومات, UError_ڪوڊ *err)
{
    UData_يادگيري     udm;

    إذا(err==خالي_اشارو || U_FAILURE(*err)) {
        أرجع;
    }
    إذا(معلومات==خالي_اشارو) {
        *err=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    UData_يادگيري_init(&udm);
    UData_يادگيري_سيٽ_معلومات(&udm, معلومات);
    udata_check_Common_معلومات(&udm, err);
    udata_cache_معلومات_شي(رستو, &udm, err);
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
ساكن UData_يادگيري *
check_معلومات_شي
(
 ثابت معلومات_سرو         *p_سرو,         /* The data item to be checked.                */
 UData_يادگيري_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 فراغ                     *context,         /*   pass-thru param for above.                */
 ثابت حرف               *type,            /*   pass-thru param for above.                */
 ثابت حرف               *name,            /*   pass-thru param for above.                */
 UError_ڪوڊ               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_ڪوڊ               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_يادگيري  *r_معلومات_Mem = خالي_اشارو;          /* the new UDataMemory, to be returned.        */

    إذا (U_FAILURE(*fatalErr)) {
        أرجع خالي_اشارو;
    }

    إذا(p_سرو->معلومات_سرو_2.magic1==0xda &&
        p_سرو->معلومات_سرو_2.magic2==0x27 &&
        (isAcceptable==خالي_اشارو || isAcceptable(context, type, name, &p_سرو->info))
    ) {
        r_معلومات_Mem=UData_يادگيري_create_New_Instance(fatalErr);
        إذا (U_FAILURE(*fatalErr)) {
            أرجع خالي_اشارو;
        }
        r_معلومات_Mem->p_سرو = p_سرو;
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
ساكن UData_يادگيري *do_ڀريو_From_Individual_Files(ثابت حرف *pkgName, 
        ثابت حرف *معلومات_رستو, ثابت حرف *toc_داخلا_رستو_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *رستو, ثابت حرف *type, ثابت حرف *name,
             UData_يادگيري_Is_Acceptable *isAcceptable, فراغ *context,
             UError_ڪوڊ *sub_Error_ڪوڊ,
             UError_ڪوڊ *p_Error_ڪوڊ)
{
    ثابت حرف         *رستو_بفر;
    UData_يادگيري         معلومات_يادگيري;
    UData_يادگيري *p_داخلا_معلومات;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_رستو_Iterator iter(معلومات_رستو, pkgName, رستو, toc_داخلا_رستو_Suffix, خطأ, p_Error_ڪوڊ);

    طالما ((رستو_بفر = iter.next(p_Error_ڪوڊ)) != خالي_اشارو)
    {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", رستو_بفر);
#جيڪڏهن_پورو
        إذا (uprv_mapFile(&معلومات_يادگيري, رستو_بفر, p_Error_ڪوڊ))
        {
            p_داخلا_معلومات = check_معلومات_شي(معلومات_يادگيري.p_سرو, isAcceptable, context, type, name, sub_Error_ڪوڊ, p_Error_ڪوڊ);
            إذا (p_داخلا_معلومات != خالي_اشارو) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_داخلا_معلومات->mapAddr = معلومات_يادگيري.mapAddr;
                p_داخلا_معلومات->map     = معلومات_يادگيري.map;
                p_داخلا_معلومات->length  = معلومات_يادگيري.length;

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", رستو_بفر);
#جيڪڏهن_پورو
                أرجع p_داخلا_معلومات;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&معلومات_يادگيري);

            /* If we had a nasty error, bail out completely.  */
            إذا (U_FAILURE(*p_Error_ڪوڊ)) {
                أرجع خالي_اشارو;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        }
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_يادگيري_is_Loaded(&معلومات_يادگيري)?"LOADED":"not loaded");
#جيڪڏهن_پورو
    }
    أرجع خالي_اشارو;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساكن UData_يادگيري *do_ڀريو_From_Common_معلومات(UBool isICUData, ثابت حرف * /*pkgName*/, 
        ثابت حرف * /*dataPath*/, ثابت حرف * /*tocEntryPathSuffix*/, ثابت حرف *toc_داخلا_Name,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *رستو, ثابت حرف *type, ثابت حرف *name,
             UData_يادگيري_Is_Acceptable *isAcceptable, فراغ *context,
             UError_ڪوڊ *sub_Error_ڪوڊ,
             UError_ڪوڊ *p_Error_ڪوڊ)
{
    UData_يادگيري        *p_داخلا_معلومات;
    ثابت معلومات_سرو   *p_سرو;
    UData_يادگيري        *p_Common_معلومات;
    int32_t            common_معلومات_اشاريو;
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
    لكل (common_معلومات_اشاريو = isICUData ? 0 : -1;;) {
        p_Common_معلومات=open_Common_معلومات(رستو, common_معلومات_اشاريو, sub_Error_ڪوڊ); /** search for pkg **/

        إذا(U_SUCCESS(*sub_Error_ڪوڊ) && p_Common_معلومات!=خالي_اشارو) {
            int32_t length;

            /* look up the data piece in the common data */
            p_سرو=p_Common_معلومات->vFuncs->Lookup(p_Common_معلومات, toc_داخلا_Name, &length, sub_Error_ڪوڊ);
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_داخلا_Name, (فراغ*) p_سرو, u_errorName(*sub_Error_ڪوڊ));
#جيڪڏهن_پورو

            إذا(p_سرو!=خالي_اشارو) {
                p_داخلا_معلومات = check_معلومات_شي(p_سرو, isAcceptable, context, type, name, sub_Error_ڪوڊ, p_Error_ڪوڊ);
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (فراغ*) p_داخلا_معلومات);
#جيڪڏهن_پورو
                إذا (U_FAILURE(*p_Error_ڪوڊ)) {
                    أرجع خالي_اشارو;
                }
                إذا (p_داخلا_معلومات != خالي_اشارو) {
                    p_داخلا_معلومات->length = length;
                    أرجع p_داخلا_معلومات;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        إذا (*sub_Error_ڪوڊ == U_يادگيري_ورهاست_ERROR) {
            *p_Error_ڪوڊ = *sub_Error_ڪوڊ;
            أرجع خالي_اشارو;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        إذا (!isICUData) {
            أرجع خالي_اشارو;
        } وإلا إذا (p_Common_معلومات != خالي_اشارو) {
            ++common_معلومات_اشاريو;  /* try the next data package */
        } وإلا إذا ((!checkedExtendedICUData) && extendICUData(sub_Error_ڪوڊ)) {
            checkedExtendedICUData = سچ;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } وإلا {
            أرجع خالي_اشارو;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
ساكن UBool is_وقت_Zone_File(ثابت حرف *name, ثابت حرف *type) {
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
ساكن UData_يادگيري *
doOpenChoice(ثابت حرف *رستو, ثابت حرف *type, ثابت حرف *name,
             UData_يادگيري_Is_Acceptable *isAcceptable, فراغ *context,
             UError_ڪوڊ *p_Error_ڪوڊ)
{
    UData_يادگيري         *retVal = خالي_اشارو;

    ثابت حرف         *معلومات_رستو;

    int32_t             toc_داخلا_Suffix_اشاريو;
    ثابت حرف         *toc_داخلا_رستو_Suffix;
    UError_ڪوڊ          sub_Error_ڪوڊ=U_ZERO_ERROR;
    ثابت حرف         *وڻ_Char;

    UBool               isICUData = خطأ;


    FileTracer::traceOpen(رستو, type, name);


    /* Is this path ICU data? */
    إذا(رستو == خالي_اشارو ||
       !strcmp(رستو, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(رستو, U_ICUDATA_NAME U_وڻ_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_وڻ_SEPARATOR_STRING)) ||  
       !uprv_strncmp(رستو, U_ICUDATA_ALIAS U_وڻ_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_وڻ_SEPARATOR_STRING))) {
      isICUData = سچ;
    }

#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_رستو;
    إذا(رستو) {
        إذا(uprv_strchr(رستو,U_FILE_ALT_SEP_CHAR) != خالي_اشارو) {
            alt_Sep_رستو.append(رستو, *p_Error_ڪوڊ);
            حرف *p;
            طالما ((p = uprv_strchr(alt_Sep_رستو.معلومات(), U_FILE_ALT_SEP_CHAR)) != خالي_اشارو) {
                *p = U_FILE_SEP_CHAR;
            }
#إذا defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", رستو, alt_Sep_رستو.معلومات());
#جيڪڏهن_پورو
            رستو = alt_Sep_رستو.معلومات();
        }
    }
#جيڪڏهن_پورو

    CharString toc_داخلا_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_داخلا_رستو; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString وڻ_Name;

    /* ======= Set up strings */
    إذا(رستو==خالي_اشارو) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_ڪوڊ);
    } وإلا {
        ثابت حرف *pkg;
        ثابت حرف *first;
        pkg = uprv_strrchr(رستو, U_FILE_SEP_CHAR);
        first = uprv_strchr(رستو, U_FILE_SEP_CHAR);
        إذا(uprv_رستو_Is_Absolute(رستو) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            إذا(pkg) {
                pkgName.append(pkg+1, *p_Error_ڪوڊ);
            } وإلا {
                pkgName.append(رستو, *p_Error_ڪوڊ);
            }
        } وإلا {
            وڻ_Char = uprv_strchr(رستو, U_وڻ_SEPARATOR);
            إذا(وڻ_Char) { 
                وڻ_Name.append(وڻ_Char+1, *p_Error_ڪوڊ); /* following '-' */
                إذا(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_ڪوڊ);
                } وإلا {
                    pkgName.append(رستو, static_cast<int32_t>(وڻ_Char - رستو), *p_Error_ڪوڊ);
                    إذا (first == خالي_اشارو) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        رستو = pkgName.معلومات();
                    }
                }
            } وإلا {
                إذا(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_ڪوڊ);
                } وإلا {
                    pkgName.append(رستو, *p_Error_ڪوڊ);
                }
            }
        }
    }

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.معلومات(), وڻ_Name.معلومات());
#جيڪڏهن_پورو

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_داخلا_Name.append(pkgName, *p_Error_ڪوڊ);
    toc_داخلا_رستو.append(pkgName, *p_Error_ڪوڊ);
    toc_داخلا_Suffix_اشاريو = toc_داخلا_Name.length();

    إذا(!وڻ_Name.is_خالي_جايون()) {
        toc_داخلا_Name.append(U_وڻ_داخلا_SEP_CHAR, *p_Error_ڪوڊ).append(وڻ_Name, *p_Error_ڪوڊ);
        toc_داخلا_رستو.append(U_FILE_SEP_CHAR, *p_Error_ڪوڊ).append(وڻ_Name, *p_Error_ڪوڊ);
    }

    toc_داخلا_Name.append(U_وڻ_داخلا_SEP_CHAR, *p_Error_ڪوڊ).append(name, *p_Error_ڪوڊ);
    toc_داخلا_رستو.append(U_FILE_SEP_CHAR, *p_Error_ڪوڊ).append(name, *p_Error_ڪوڊ);
    إذا(type!=خالي_اشارو && *type!=0) {
        toc_داخلا_Name.append(".", *p_Error_ڪوڊ).append(type, *p_Error_ڪوڊ);
        toc_داخلا_رستو.append(".", *p_Error_ڪوڊ).append(type, *p_Error_ڪوڊ);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_داخلا_رستو_Suffix = toc_داخلا_رستو.معلومات() + toc_داخلا_Suffix_اشاريو + 1; /* suffix starts here */

#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_داخلا_Name.معلومات());
    fprintf(stderr, " tocEntryPath = %s\n", toc_داخلا_رستو.معلومات());
#جيڪڏهن_پورو

#إذا !defined(ICU_معلومات_DIR_WINDOWS)
    إذا(رستو == خالي_اشارو) {
        رستو = COMMON_معلومات_NAME; /* "icudt26e" */
    }
#وإلا
    // When using the Windows system data, we expects only a single data file.
    رستو = COMMON_معلومات_NAME; /* "icudt26e" */
#جيڪڏهن_پورو

    /************************ Begin loop looking for ind. files ***************/
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_رستو(رستو));
#جيڪڏهن_پورو

    /* End of dealing with a null basename */
    معلومات_رستو = u_get_معلومات_Directory();

    /****    Time zone individual files override  */
    إذا (isICUData && type != خالي_اشارو && is_وقت_Zone_File(name, type)) {
        ثابت حرف *tzFilesDir = u_get_وقت_Zone_Files_Directory(p_Error_ڪوڊ);
        إذا (tzFilesDir[0] != 0) {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#جيڪڏهن_پورو
            retVal = do_ڀريو_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_داخلا_رستو_Suffix,
                            /*رستو*/ "", type, name, isAcceptable, context, &sub_Error_ڪوڊ, p_Error_ڪوڊ);
            إذا((retVal != خالي_اشارو) || U_FAILURE(*p_Error_ڪوڊ)) {
                أرجع retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    إذا(g_معلومات_File_Access == UDATA_PACKAGES_پهريون) {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#جيڪڏهن_پورو
        /* #2 */
        retVal = do_ڀريو_From_Common_معلومات(isICUData, 
                            pkgName.معلومات(), معلومات_رستو, toc_داخلا_رستو_Suffix, toc_داخلا_Name.معلومات(),
                            رستو, type, name, isAcceptable, context, &sub_Error_ڪوڊ, p_Error_ڪوڊ);
        إذا((retVal != خالي_اشارو) || U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    إذا((g_معلومات_File_Access==UDATA_PACKAGES_پهريون) ||
       (g_معلومات_File_Access==UDATA_FILES_پهريون)) {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#جيڪڏهن_پورو
        /* Check to make sure that there is a dataPath to iterate over */
        إذا ((معلومات_رستو && *معلومات_رستو) || !isICUData) {
            retVal = do_ڀريو_From_Individual_Files(pkgName.معلومات(), معلومات_رستو, toc_داخلا_رستو_Suffix,
                            رستو, type, name, isAcceptable, context, &sub_Error_ڪوڊ, p_Error_ڪوڊ);
            إذا((retVal != خالي_اشارو) || U_FAILURE(*p_Error_ڪوڊ)) {
                أرجع retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    إذا((g_معلومات_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_معلومات_File_Access==UDATA_FILES_پهريون)) {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#جيڪڏهن_پورو
        retVal = do_ڀريو_From_Common_معلومات(isICUData,
                            pkgName.معلومات(), معلومات_رستو, toc_داخلا_رستو_Suffix, toc_داخلا_Name.معلومات(),
                            رستو, type, name, isAcceptable, context, &sub_Error_ڪوڊ, p_Error_ڪوڊ);
        إذا((retVal != خالي_اشارو) || U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    إذا(g_معلومات_File_Access==UDATA_NO_FILES) {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#جيڪڏهن_پورو
        retVal = do_ڀريو_From_Common_معلومات(isICUData,
                            pkgName.معلومات(), "", toc_داخلا_رستو_Suffix, toc_داخلا_Name.معلومات(),
                            رستو, type, name, isAcceptable, context, &sub_Error_ڪوڊ, p_Error_ڪوڊ);
        إذا((retVal != خالي_اشارو) || U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع retVal;
        }
    }

    /* data not found */
    إذا(U_SUCCESS(*p_Error_ڪوڊ)) {
        إذا(U_SUCCESS(sub_Error_ڪوڊ)) {
            /* file not found */
            *p_Error_ڪوڊ=U_FILE_ACCESS_ERROR;
        } وإلا {
            /* entry point not found or rejected */
            *p_Error_ڪوڊ=sub_Error_ڪوڊ;
        }
    }
    أرجع retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_يادگيري * U_EXPORT2
udata_open(ثابت حرف *رستو, ثابت حرف *type, ثابت حرف *name,
           UError_ڪوڊ *p_Error_ڪوڊ) {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (رستو?رستو:"nullptr"), name, type);
    fflush(stderr);
#جيڪڏهن_پورو

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    } وإلا إذا(name==خالي_اشارو || *name==0) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    } وإلا {
        أرجع doOpenChoice(رستو, type, name, خالي_اشارو, خالي_اشارو, p_Error_ڪوڊ);
    }
}



U_CAPI UData_يادگيري * U_EXPORT2
udata_openChoice(ثابت حرف *رستو, ثابت حرف *type, ثابت حرف *name,
                 UData_يادگيري_Is_Acceptable *isAcceptable, فراغ *context,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (رستو?رستو:"nullptr"), name, type);
#جيڪڏهن_پورو

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    } وإلا إذا(name==خالي_اشارو || *name==0 || isAcceptable==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    } وإلا {
        أرجع doOpenChoice(رستو, type, name, isAcceptable, context, p_Error_ڪوڊ);
    }
}



U_CAPI فراغ U_EXPORT2
udata_getInfo(UData_يادگيري *p_معلومات, UDataInfo *pInfo) {
    إذا(pInfo!=خالي_اشارو) {
        إذا(p_معلومات!=خالي_اشارو && p_معلومات->p_سرو!=خالي_اشارو) {
            ثابت UDataInfo *info=&p_معلومات->p_سرو->info;
            uint16_t معلومات_Info_سائيز=udata_get_Info_سائيز(info);
            إذا(pInfo->size>معلومات_Info_سائيز) {
                pInfo->size=معلومات_Info_سائيز;
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


U_CAPI فراغ U_EXPORT2 udata_سيٽ_File_Access(UDataFileAccess access, UError_ڪوڊ * /*حالت*/)
{
    // Note: this function is documented as not thread safe.
    g_معلومات_File_Access = access;
}
