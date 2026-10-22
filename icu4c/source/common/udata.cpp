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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"  /* U_PLATFORM etc. */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uversion.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "restrace.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucmndata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udatamem.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umapfile.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

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

#ਜੇ defined(UDATA_DEBUG)
#   include <stdio.h>
#ਜੇ_ਅੰਤ

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *udata_ਲੱਭੋ_Cached_ਅੰਕੜੇ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ &err);

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
ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *gCommonICUDataArray[10] = { ਖਾਲੀ_ਸੂਚਕ };   // Access protected by icu global mutex.

ਸਥਿਰ u_atomic_int32_t g_Have_Tried_ਮੰਜ਼ਿਲ_ਲੱਦੋ_Common_ਅੰਕੜੇ {0};  //  See extendICUData().

ਸਥਿਰ UHashtable  *g_Common_ਅੰਕੜੇ_Cache = ਖਾਲੀ_ਸੂਚਕ;  /* Global hash table of opened ICU data files.  */
ਸਥਿਰ icu::UInitOnce g_Common_ਅੰਕੜੇ_Cache_Init_Once {};

#ਜੇ !defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)
ਸਥਿਰ UDataFileAccess  g_ਅੰਕੜੇ_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#ਨਹੀਂਤਾਂ
// If we are using the Windows data directory, then look in one spot only.
ਸਥਿਰ UDataFileAccess  g_ਅੰਕੜੇ_File_Access = UDATA_NO_FILES;
#ਜੇ_ਅੰਤ

ਸਥਿਰ UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    ਜੇ (g_Common_ਅੰਕੜੇ_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_ਅੰਕੜੇ_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_ਅੰਕੜੇ_Cache = ਖਾਲੀ_ਸੂਚਕ;        /*   Cleanup is not thread safe.                */
    }
    g_Common_ਅੰਕੜੇ_Cache_Init_Once.reset();

    ਲਈ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != ਖਾਲੀ_ਸੂਚਕ; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = ਖਾਲੀ_ਸੂਚਕ;
    }
    g_Have_Tried_ਮੰਜ਼ਿਲ_ਲੱਦੋ_Common_ਅੰਕੜੇ = 0;

    ਵਾਪਸ ਸੱਚ;                   /* Everything was cleaned up */
}

ਸਥਿਰ UBool U_CALLCONV
ਲੱਭੋ_Common_ICUData_By_Name(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_Basename, UError_ਕੋਡ &err)
{
    UBool ਮਿਲਿਆ = ਝੂਠ;
    int32_t i;

    UData_ਸਮ੍ਰਿਤੀ  *p_ਅੰਕੜੇ = udata_ਲੱਭੋ_Cached_ਅੰਕੜੇ(ਵਿੱਚ_Basename, err);
    ਜੇ (U_FAILURE(err) || p_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ)
        ਵਾਪਸ ਝੂਠ;

    {
        Mutex ਤਾਲਾ;
        ਲਈ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            ਜੇ ((gCommonICUDataArray[i] != ਖਾਲੀ_ਸੂਚਕ) && (gCommonICUDataArray[i]->p_ਸਿਰਲੇਖ == p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ)) {
                /* The data pointer is already in the array. */
                ਮਿਲਿਆ = ਸੱਚ;
                ਰੋਕੋ;
            }
        }
    }
    ਵਾਪਸ ਮਿਲਿਆ;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
ਸਥਿਰ UBool
ਸੈੱਟ_Common_ICUData(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_ਕੋਡ *pErr)
{
    UData_ਸਮ੍ਰਿਤੀ  *new_Common_ਅੰਕੜੇ = UData_ਸਮ੍ਰਿਤੀ_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = ਝੂਠ;
    ਜੇ (U_FAILURE(*pErr)) {
        ਵਾਪਸ ਝੂਠ;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_ਸੌਂਪੋ(new_Common_ਅੰਕੜੇ, p_ਅੰਕੜੇ);
    umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
    ਲਈ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        ਜੇ (gCommonICUDataArray[i] == ਖਾਲੀ_ਸੂਚਕ) {
            gCommonICUDataArray[i] = new_Common_ਅੰਕੜੇ;
            didUpdate = ਸੱਚ;
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ ਜੇ (gCommonICUDataArray[i]->p_ਸਿਰਲੇਖ == p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ) {
            /* The same data pointer is already in the array. */
            ਰੋਕੋ;
        }
    }
    umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);

    ਜੇ (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    ਜੇ (didUpdate) {
        ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } ਨਹੀਂਤਾਂ {
        uprv_free(new_Common_ਅੰਕੜੇ);
    }
    ਵਾਪਸ didUpdate;
}

#ਜੇ !defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)

ਸਥਿਰ UBool
ਸੈੱਟ_Common_ICUData_ਸੂਚਕ(ਅਚਲ ਖਾਲੀ *p_ਅੰਕੜੇ, UBool /*warn*/, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UData_ਸਮ੍ਰਿਤੀ t_ਅੰਕੜੇ;
    UData_ਸਮ੍ਰਿਤੀ_init(&t_ਅੰਕੜੇ);
    UData_ਸਮ੍ਰਿਤੀ_ਸੈੱਟ_ਅੰਕੜੇ(&t_ਅੰਕੜੇ, p_ਅੰਕੜੇ);
    udata_check_Common_ਅੰਕੜੇ(&t_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਵਾਪਸ ਸੈੱਟ_Common_ICUData(&t_ਅੰਕੜੇ, ਝੂਠ, p_Error_ਕੋਡ);
}

#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *
ਲੱਭੋ_Basename(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
    ਅਚਲ ਅੱਖਰ *basename=uprv_strrchr(ਰਸਤਾ, U_FILE_SEP_CHAR);
    ਜੇ(basename==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਰਸਤਾ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ basename+1;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *
package_Name_From_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
    ਜੇ((ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) || (*ਰਸਤਾ == 0)) {
        ਵਾਪਸ U_ICUDATA_NAME;
    }

    ਰਸਤਾ = ਲੱਭੋ_Basename(ਰਸਤਾ);

    ਜੇ((ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) || (*ਰਸਤਾ == 0)) {
        ਵਾਪਸ U_ICUDATA_NAME;
    }

    ਵਾਪਸ ਰਸਤਾ;
}
#ਜੇ_ਅੰਤ

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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਅੰਕੜੇ_Cache_Element {
    ਅੱਖਰ          *name;
    UData_ਸਮ੍ਰਿਤੀ   *ਵਸਤੂ;
} ਅੰਕੜੇ_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV ਅੰਕੜੇ_Cache_Element_deleter(ਖਾਲੀ *pDCEl) {
    ਅੰਕੜੇ_Cache_Element* p = static_cast<ਅੰਕੜੇ_Cache_Element*>(pDCEl);
    udata_close(p->ਵਸਤੂ);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV udata_init_Hash_ਸਾਰਣੀ(UError_ਕੋਡ &err) {
    U_ASSERT(g_Common_ਅੰਕੜੇ_Cache == ਖਾਲੀ_ਸੂਚਕ);
    g_Common_ਅੰਕੜੇ_Cache = uhash_open(uhash_hashChars, uhash_compareChars, ਖਾਲੀ_ਸੂਚਕ, &err);
    ਜੇ (U_FAILURE(err)) {
       ਵਾਪਸ;
    }
    U_ASSERT(g_Common_ਅੰਕੜੇ_Cache != ਖਾਲੀ_ਸੂਚਕ);
    uhash_ਸੈੱਟ_ਮੁੱਲ_Deleter(g_Common_ਅੰਕੜੇ_Cache, ਅੰਕੜੇ_Cache_Element_deleter);
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
ਸਥਿਰ UHashtable *udata_get_Hash_ਸਾਰਣੀ(UError_ਕੋਡ &err) {
    umtx_initOnce(g_Common_ਅੰਕੜੇ_Cache_Init_Once, &udata_init_Hash_ਸਾਰਣੀ, err);
    ਵਾਪਸ g_Common_ਅੰਕੜੇ_Cache;
}



ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *udata_ਲੱਭੋ_Cached_ਅੰਕੜੇ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ &err)
{
    UHashtable        *htable;
    UData_ਸਮ੍ਰਿਤੀ       *retVal = ਖਾਲੀ_ਸੂਚਕ;
    ਅੰਕੜੇ_Cache_Element  *el;
    ਅਚਲ ਅੱਖਰ        *ਅਧਾਰ_Name;

    htable = udata_get_Hash_ਸਾਰਣੀ(err);
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਅਧਾਰ_Name = ਲੱਭੋ_Basename(ਰਸਤਾ);   /* Cache remembers only the base name, not the full path. */
    umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
    el = static_cast<ਅੰਕੜੇ_Cache_Element*>(uhash_get(htable, ਅਧਾਰ_Name));
    umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ (el != ਖਾਲੀ_ਸੂਚਕ) {
        retVal = el->ਵਸਤੂ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", ਅਧਾਰ_Name, (ਖਾਲੀ*) retVal);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ retVal;
}


ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *udata_cache_ਅੰਕੜੇ_ਵਸਤੂ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UData_ਸਮ੍ਰਿਤੀ *ਵਸਤੂ, UError_ਕੋਡ *pErr) {
    ਅੰਕੜੇ_Cache_Element *newElement;
    ਅਚਲ ਅੱਖਰ       *ਅਧਾਰ_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    ਅੰਕੜੇ_Cache_Element *old_ਮੁੱਲ = ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_ਸਾਰਣੀ(*pErr);
    ਜੇ (U_FAILURE(*pErr)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<ਅੰਕੜੇ_Cache_Element*>(uprv_malloc(ਆਕਾਰ(ਅੰਕੜੇ_Cache_Element)));
    ਜੇ (newElement == ਖਾਲੀ_ਸੂਚਕ) {
        *pErr = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    newElement->ਵਸਤੂ = UData_ਸਮ੍ਰਿਤੀ_create_New_Instance(pErr);
    ਜੇ (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UDatamemory_ਸੌਂਪੋ(newElement->ਵਸਤੂ, ਵਸਤੂ);

    ਅਧਾਰ_Name = ਲੱਭੋ_Basename(ਰਸਤਾ);
    nameLen = static_cast<int32_t>(uprv_strlen(ਅਧਾਰ_Name));
    newElement->name = static_cast<ਅੱਖਰ*>(uprv_malloc(nameLen + 1));
    ਜੇ (newElement->name == ਖਾਲੀ_ਸੂਚਕ) {
        *pErr = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        uprv_free(newElement->ਵਸਤੂ);
        uprv_free(newElement);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_strcpy(newElement->name, ਅਧਾਰ_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
    old_ਮੁੱਲ = static_cast<ਅੰਕੜੇ_Cache_Element*>(uhash_get(htable, ਰਸਤਾ));
    ਜੇ (old_ਮੁੱਲ != ਖਾਲੀ_ਸੂਚਕ) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    ਨਹੀਂਤਾਂ {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (ਖਾਲੀ*) newElement->ਵਸਤੂ, u_errorName(subErr), (ਖਾਲੀ*) newElement->ਵਸਤੂ->vFuncs);
#ਜੇ_ਅੰਤ

    ਜੇ (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->ਵਸਤੂ);
        uprv_free(newElement);
        ਵਾਪਸ old_ਮੁੱਲ ? old_ਮੁੱਲ->ਵਸਤੂ : ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ newElement->ਵਸਤੂ;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

ਵਰਗ UData_ਰਸਤਾ_Iterator
{
ਜਨਤਕ:
    UData_ਰਸਤਾ_Iterator(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *pkg,
                      ਅਚਲ ਅੱਖਰ *ਵਸਤੂ, ਅਚਲ ਅੱਖਰ *suffix, UBool do_Check_ਆਖਰੀ_Four,
                      UError_ਕੋਡ *p_Error_ਕੋਡ);
    ਅਚਲ ਅੱਖਰ *next(UError_ਕੋਡ *p_Error_ਕੋਡ);

ਨਿਜੀ:
    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ;                              /* working path (u_icudata_Dir) */
    ਅਚਲ ਅੱਖਰ *ਅਗਲਾ_ਰਸਤਾ;                          /* path following this one */
    ਅਚਲ ਅੱਖਰ *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  ਵਸਤੂ_ਰਸਤਾ;                          /* path passed in with item name */
    CharString  ਰਸਤਾ_ਬਫਰ;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       check_ਆਖਰੀ_Four;                     /* if true then allow paths such as '/foo/myapp.dat'
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
UData_ਰਸਤਾ_Iterator::UData_ਰਸਤਾ_Iterator(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *pkg,
                                     ਅਚਲ ਅੱਖਰ *ਵਸਤੂ, ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_Suffix, UBool do_Check_ਆਖਰੀ_Four,
                                     UError_ਕੋਡ *p_Error_ਕੋਡ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", ਵਿੱਚ_Suffix, ਵਿੱਚ_ਰਸਤਾ);
#ਜੇ_ਅੰਤ
    /** Path **/
    ਜੇ(ਵਿੱਚ_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
        ਰਸਤਾ = u_get_ਅੰਕੜੇ_Directory();
    } ਨਹੀਂਤਾਂ {
        ਰਸਤਾ = ਵਿੱਚ_ਰਸਤਾ;
    }

    /** Package **/
    ਜੇ(pkg != ਖਾਲੀ_ਸੂਚਕ) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_ਕੋਡ).append(pkg, *p_Error_ਕੋਡ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.ਅੰਕੜੇ(), packageStub.length());
#ਜੇ_ਅੰਤ
    }

    /** Item **/
    basename = ਲੱਭੋ_Basename(ਵਸਤੂ);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    ਜੇ(basename == ਵਸਤੂ) {
        ਅਗਲਾ_ਰਸਤਾ = ਰਸਤਾ;
    } ਨਹੀਂਤਾਂ {
        ਵਸਤੂ_ਰਸਤਾ.append(ਵਸਤੂ, static_cast<int32_t>(basename - ਵਸਤੂ), *p_Error_ਕੋਡ);
        ਅਗਲਾ_ਰਸਤਾ = ਵਸਤੂ_ਰਸਤਾ.ਅੰਕੜੇ();
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", ਵਿੱਚ_Suffix, (ਖਾਲੀ*) ਵਿੱਚ_Suffix);
#ਜੇ_ਅੰਤ

    /** Suffix  **/
    ਜੇ(ਵਿੱਚ_Suffix != ਖਾਲੀ_ਸੂਚਕ) {
        suffix = ਵਿੱਚ_Suffix;
    } ਨਹੀਂਤਾਂ {
        suffix = "";
    }

    check_ਆਖਰੀ_Four = do_Check_ਆਖਰੀ_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            ਵਸਤੂ,
            ਰਸਤਾ,
            basename,
            suffix.ਅੰਕੜੇ(),
            ਵਸਤੂ_ਰਸਤਾ.ਅੰਕੜੇ(),
            ਅਗਲਾ_ਰਸਤਾ,
            check_ਆਖਰੀ_Four?"true":"false");
#ਜੇ_ਅੰਤ
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ਅਚਲ ਅੱਖਰ *UData_ਰਸਤਾ_Iterator::next(UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਅਚਲ ਅੱਖਰ *current_ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t     ਰਸਤਾ_Len = 0;
    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ_Basename;

    ਕਰੋ
    {
        ਜੇ( ਅਗਲਾ_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ ) {
            ਰੋਕੋ;
        }
        current_ਰਸਤਾ = ਅਗਲਾ_ਰਸਤਾ;

        ਜੇ(ਅਗਲਾ_ਰਸਤਾ == ਵਸਤੂ_ਰਸਤਾ.ਅੰਕੜੇ()) { /* we were processing item's path. */
            ਅਗਲਾ_ਰਸਤਾ = ਰਸਤਾ; /* start with regular path next tm. */
            ਰਸਤਾ_Len = static_cast<int32_t>(uprv_strlen(current_ਰਸਤਾ));
        } ਨਹੀਂਤਾਂ {
            /* fix up next for next time */
            ਅਗਲਾ_ਰਸਤਾ = uprv_strchr(current_ਰਸਤਾ, U_ਰਸਤਾ_SEP_CHAR);
            ਜੇ(ਅਗਲਾ_ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
                /* segment: entire path */
                ਰਸਤਾ_Len = static_cast<int32_t>(uprv_strlen(current_ਰਸਤਾ));
            } ਨਹੀਂਤਾਂ {
                /* segment: until next segment */
                ਰਸਤਾ_Len = static_cast<int32_t>(ਅਗਲਾ_ਰਸਤਾ - current_ਰਸਤਾ);
                /* skip divider */
                ਅਗਲਾ_ਰਸਤਾ ++;
            }
        }

        ਜੇ(ਰਸਤਾ_Len == 0) {
            ਜਾਰੀ;
        }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_ਰਸਤਾ);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            ਲਈ(qqq=0;qqq<ਰਸਤਾ_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#ਜੇ_ਅੰਤ
        ਰਸਤਾ_ਬਫਰ.clear().append(current_ਰਸਤਾ, ਰਸਤਾ_Len, *p_Error_ਕੋਡ);

        /* check for .dat files */
        ਰਸਤਾ_Basename = ਲੱਭੋ_Basename(ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ());

        ਜੇ(check_ਆਖਰੀ_Four && 
           (ਰਸਤਾ_Len>=4) &&
           uprv_strncmp(ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ() +(ਰਸਤਾ_Len-4), suffix.ਅੰਕੜੇ(), 4)==0 && /* suffix matches */
           uprv_strncmp(ਲੱਭੋ_Basename(ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(ਰਸਤਾ_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.ਅੰਕੜੇ(), ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
            /* do nothing */
        }
        ਨਹੀਂਤਾਂ 
        {       /* regular dir path */
            ਜੇ(ਰਸਤਾ_ਬਫਰ[ਰਸਤਾ_Len-1] != U_FILE_SEP_CHAR) {
                ਜੇ((ਰਸਤਾ_Len>=4) &&
                   uprv_strncmp(ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ()+(ਰਸਤਾ_Len-4), ".dat", 4) == 0)
                {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
                    ਜਾਰੀ;
                }

                /* Check if it is a directory with the same name as our package */
                ਜੇ(!packageStub.is_ਖਾਲੀ_ਥਾਵਾਂ() &&
                   (ਰਸਤਾ_Len > packageStub.length()) &&
                   !uprv_strcmp(ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ() + ਰਸਤਾ_Len - packageStub.length(), packageStub.ਅੰਕੜੇ())) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.ਅੰਕੜੇ(), basename, basenameLen);
#ਜੇ_ਅੰਤ
                  ਰਸਤਾ_ਬਫਰ.truncate(ਰਸਤਾ_Len - packageStub.length());
                }
                ਰਸਤਾ_ਬਫਰ.append(U_FILE_SEP_CHAR, *p_Error_ਕੋਡ);
            }

            /* + basename */
            ਰਸਤਾ_ਬਫਰ.append(packageStub.ਅੰਕੜੇ()+1, packageStub.length()-1, *p_Error_ਕੋਡ);

            ਜੇ (!suffix.empty())  /* tack on suffix */
            {
                ਜੇ (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    ਰਸਤਾ_ਬਫਰ.ensure_ਸਿਰੇ_With_File_Separator(*p_Error_ਕੋਡ);
                }
                ਰਸਤਾ_ਬਫਰ.append(suffix, *p_Error_ਕੋਡ);
            }
        }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ

        ਵਾਪਸ ਰਸਤਾ_ਬਫਰ.ਅੰਕੜੇ();

    } ਜਦੋਂ(ਰਸਤਾ);

    /* fell way off the end */
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_NAMESPACE_ਸਿਰਾ

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#ਜੇ !defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
ਬਾਹਰੀ "C" ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ U_ਅੰਕੜੇ_API U_ICUDATA_ਇੰਦਰਾਜ_ਬਿੰਦੂ;
#ਜੇ_ਅੰਤ

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
ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *
open_Common_ਅੰਕੜੇ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,          /*  Path from OpenChoice?          */
               int32_t common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    UData_ਸਮ੍ਰਿਤੀ t_ਅੰਕੜੇ;
    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ_ਬਫਰ;
    ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_Basename;

    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UData_ਸਮ੍ਰਿਤੀ_init(&t_ਅੰਕੜੇ);

    /* ??????? TODO revisit this */ 
    ਜੇ (common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ >= 0) {
        /* "mini-cache" for common ICU data */
        ਜੇ(common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        {
            Mutex ਤਾਲਾ;
            ਜੇ(gCommonICUDataArray[common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ] != ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ gCommonICUDataArray[common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ];
            }
#ਜੇ !defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            ਲਈ(i = 0; i < common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ; ++i) {
                ਜੇ(gCommonICUDataArray[i]->p_ਸਿਰਲੇਖ == &U_ICUDATA_ਇੰਦਰਾਜ_ਬਿੰਦੂ) {
                    /* The linked-in data is already in the list. */
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
            }
#ਜੇ_ਅੰਤ
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
#ਜੇ !defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        ਸੈੱਟ_Common_ICUData_ਸੂਚਕ(&U_ICUDATA_ਇੰਦਰਾਜ_ਬਿੰਦੂ, ਝੂਠ, p_Error_ਕੋਡ);
        {
            Mutex ਤਾਲਾ;
            ਵਾਪਸ gCommonICUDataArray[common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ];
        }
#ਜੇ_ਅੰਤ
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    ਵਿੱਚ_Basename = ਲੱਭੋ_Basename(ਰਸਤਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", ਵਿੱਚ_Basename);
#ਜੇ_ਅੰਤ

    ਜੇ(*ਵਿੱਚ_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", ਰਸਤਾ);
#ਜੇ_ਅੰਤ
        ਜੇ (U_SUCCESS(*p_Error_ਕੋਡ)) {
            *p_Error_ਕੋਡ=U_FILE_ACCESS_ERROR;
        }
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_ਸਮ੍ਰਿਤੀ  *ਅੰਕੜੇ_ਮੰਜ਼ਿਲ_Return = udata_ਲੱਭੋ_Cached_ਅੰਕੜੇ(ਵਿੱਚ_Basename, *p_Error_ਕੋਡ);
    ਜੇ (ਅੰਕੜੇ_ਮੰਜ਼ਿਲ_Return != ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਅੰਕੜੇ_ਮੰਜ਼ਿਲ_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_ਰਸਤਾ_Iterator iter(u_get_ਅੰਕੜੇ_Directory(), ਵਿੱਚ_Basename, ਰਸਤਾ, ".dat", ਸੱਚ, p_Error_ਕੋਡ);

    ਜਦੋਂ ((UData_ਸਮ੍ਰਿਤੀ_is_Loaded(&t_ਅੰਕੜੇ)==ਝੂਠ) && (ਰਸਤਾ_ਬਫਰ = iter.next(p_Error_ਕੋਡ)) != ਖਾਲੀ_ਸੂਚਕ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", ਰਸਤਾ_ਬਫਰ);
#ਜੇ_ਅੰਤ
        uprv_mapFile(&t_ਅੰਕੜੇ, ਰਸਤਾ_ਬਫਰ, p_Error_ਕੋਡ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_ਸਮ੍ਰਿਤੀ_is_Loaded(&t_ਅੰਕੜੇ)?"LOADED":"not loaded");
#ਜੇ_ਅੰਤ
    }
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (!UData_ਸਮ੍ਰਿਤੀ_is_Loaded(&t_ਅੰਕੜੇ)) {
        /* no common data */
        *p_Error_ਕੋਡ=U_FILE_ACCESS_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_ਅੰਕੜੇ(&t_ਅੰਕੜੇ, p_Error_ਕੋਡ);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    ਵਾਪਸ udata_cache_ਅੰਕੜੇ_ਵਸਤੂ(ਵਿੱਚ_Basename, &t_ਅੰਕੜੇ, p_Error_ਕੋਡ);
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
ਸਥਿਰ UBool extendICUData(UError_ਕੋਡ *pErr)
{
    UData_ਸਮ੍ਰਿਤੀ   *p_ਅੰਕੜੇ;
    UData_ਸਮ੍ਰਿਤੀ   copyPData;
    UBool         didUpdate = ਝੂਠ;

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
#ਜੇ MAP_IMPLEMENTATION==MAP_STDIO
    ਸਥਿਰ UMutex extendICUDataMutex;
    umtx_ਤਾਲਾ(&extendICUDataMutex);
#ਜੇ_ਅੰਤ
    ਜੇ(!umtx_ਲੱਦੋ_Acquire(g_Have_Tried_ਮੰਜ਼ਿਲ_ਲੱਦੋ_Common_ਅੰਕੜੇ)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_ਅੰਕੜੇ = open_Common_ਅੰਕੜੇ(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_ਸਮ੍ਰਿਤੀ_init(&copyPData);
       ਜੇ(p_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
          UDatamemory_ਸੌਂਪੋ(&copyPData, p_ਅੰਕੜੇ);
          copyPData.map = ਖਾਲੀ_ਸੂਚਕ;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = ਖਾਲੀ_ਸੂਚਕ; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              ਸੈੱਟ_Common_ICUData(&copyPData,/*  The new common data.                                */
                       ਝੂਠ,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_ਮੰਜ਼ਿਲ_ਲੱਦੋ_Common_ਅੰਕੜੇ, 1);
    }

    didUpdate = ਲੱਭੋ_Common_ICUData_By_Name(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#ਜੇ MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI ਖਾਲੀ U_EXPORT2
udata_ਸੈੱਟ_Common_ਅੰਕੜੇ(ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UData_ਸਮ੍ਰਿਤੀ ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    ਜੇ(ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    /* set the data pointer and test for validity */
    UData_ਸਮ੍ਰਿਤੀ_init(&ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ);
    UData_ਸਮ੍ਰਿਤੀ_ਸੈੱਟ_ਅੰਕੜੇ(&ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ, ਅੰਕੜੇ);
    udata_check_Common_ਅੰਕੜੇ(&ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ, p_Error_ਕੋਡ);
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {ਵਾਪਸ;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    ਸੈੱਟ_Common_ICUData(&ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ, ਸੱਚ, p_Error_ਕੋਡ);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI ਖਾਲੀ U_EXPORT2
udata_ਸੈੱਟ_App_ਅੰਕੜੇ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, UError_ਕੋਡ *err)
{
    UData_ਸਮ੍ਰਿਤੀ     udm;

    ਜੇ(err==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err)) {
        ਵਾਪਸ;
    }
    ਜੇ(ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    UData_ਸਮ੍ਰਿਤੀ_init(&udm);
    UData_ਸਮ੍ਰਿਤੀ_ਸੈੱਟ_ਅੰਕੜੇ(&udm, ਅੰਕੜੇ);
    udata_check_Common_ਅੰਕੜੇ(&udm, err);
    udata_cache_ਅੰਕੜੇ_ਵਸਤੂ(ਰਸਤਾ, &udm, err);
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
ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *
check_ਅੰਕੜੇ_ਵਸਤੂ
(
 ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ         *p_ਸਿਰਲੇਖ,         /* The data item to be checked.                */
 UData_ਸਮ੍ਰਿਤੀ_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 ਖਾਲੀ                     *context,         /*   pass-thru param for above.                */
 ਅਚਲ ਅੱਖਰ               *type,            /*   pass-thru param for above.                */
 ਅਚਲ ਅੱਖਰ               *name,            /*   pass-thru param for above.                */
 UError_ਕੋਡ               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_ਕੋਡ               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_ਸਮ੍ਰਿਤੀ  *r_ਅੰਕੜੇ_Mem = ਖਾਲੀ_ਸੂਚਕ;          /* the new UDataMemory, to be returned.        */

    ਜੇ (U_FAILURE(*fatalErr)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic1==0xda &&
        p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic2==0x27 &&
        (isAcceptable==ਖਾਲੀ_ਸੂਚਕ || isAcceptable(context, type, name, &p_ਸਿਰਲੇਖ->info))
    ) {
        r_ਅੰਕੜੇ_Mem=UData_ਸਮ੍ਰਿਤੀ_create_New_Instance(fatalErr);
        ਜੇ (U_FAILURE(*fatalErr)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        r_ਅੰਕੜੇ_Mem->p_ਸਿਰਲੇਖ = p_ਸਿਰਲੇਖ;
    } ਨਹੀਂਤਾਂ {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    ਵਾਪਸ r_ਅੰਕੜੇ_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *do_ਲੱਦੋ_From_Individual_Files(ਅਚਲ ਅੱਖਰ *pkgName, 
        ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ_ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *name,
             UData_ਸਮ੍ਰਿਤੀ_Is_Acceptable *isAcceptable, ਖਾਲੀ *context,
             UError_ਕੋਡ *sub_Error_ਕੋਡ,
             UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਅਚਲ ਅੱਖਰ         *ਰਸਤਾ_ਬਫਰ;
    UData_ਸਮ੍ਰਿਤੀ         ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ;
    UData_ਸਮ੍ਰਿਤੀ *p_ਇੰਦਰਾਜ_ਅੰਕੜੇ;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_ਰਸਤਾ_Iterator iter(ਅੰਕੜੇ_ਰਸਤਾ, pkgName, ਰਸਤਾ, toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix, ਝੂਠ, p_Error_ਕੋਡ);

    ਜਦੋਂ ((ਰਸਤਾ_ਬਫਰ = iter.next(p_Error_ਕੋਡ)) != ਖਾਲੀ_ਸੂਚਕ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", ਰਸਤਾ_ਬਫਰ);
#ਜੇ_ਅੰਤ
        ਜੇ (uprv_mapFile(&ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ, ਰਸਤਾ_ਬਫਰ, p_Error_ਕੋਡ))
        {
            p_ਇੰਦਰਾਜ_ਅੰਕੜੇ = check_ਅੰਕੜੇ_ਵਸਤੂ(ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ.p_ਸਿਰਲੇਖ, isAcceptable, context, type, name, sub_Error_ਕੋਡ, p_Error_ਕੋਡ);
            ਜੇ (p_ਇੰਦਰਾਜ_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_ਇੰਦਰਾਜ_ਅੰਕੜੇ->mapAddr = ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ.mapAddr;
                p_ਇੰਦਰਾਜ_ਅੰਕੜੇ->map     = ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ.map;
                p_ਇੰਦਰਾਜ_ਅੰਕੜੇ->length  = ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ.length;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", ਰਸਤਾ_ਬਫਰ);
#ਜੇ_ਅੰਤ
                ਵਾਪਸ p_ਇੰਦਰਾਜ_ਅੰਕੜੇ;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ);

            /* If we had a nasty error, bail out completely.  */
            ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_ਸਮ੍ਰਿਤੀ_is_Loaded(&ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ)?"LOADED":"not loaded");
#ਜੇ_ਅੰਤ
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *do_ਲੱਦੋ_From_Common_ਅੰਕੜੇ(UBool isICUData, ਅਚਲ ਅੱਖਰ * /*pkgName*/, 
        ਅਚਲ ਅੱਖਰ * /*dataPath*/, ਅਚਲ ਅੱਖਰ * /*tocEntryPathSuffix*/, ਅਚਲ ਅੱਖਰ *toc_ਇੰਦਰਾਜ_Name,
            /* following arguments are the same as doOpenChoice itself */
            ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *name,
             UData_ਸਮ੍ਰਿਤੀ_Is_Acceptable *isAcceptable, ਖਾਲੀ *context,
             UError_ਕੋਡ *sub_Error_ਕੋਡ,
             UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    UData_ਸਮ੍ਰਿਤੀ        *p_ਇੰਦਰਾਜ_ਅੰਕੜੇ;
    ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ   *p_ਸਿਰਲੇਖ;
    UData_ਸਮ੍ਰਿਤੀ        *p_Common_ਅੰਕੜੇ;
    int32_t            common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ;
    UBool              checkedExtendedICUData = ਝੂਠ;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    ਲਈ (common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ = isICUData ? 0 : -1;;) {
        p_Common_ਅੰਕੜੇ=open_Common_ਅੰਕੜੇ(ਰਸਤਾ, common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ, sub_Error_ਕੋਡ); /** search for pkg **/

        ਜੇ(U_SUCCESS(*sub_Error_ਕੋਡ) && p_Common_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ) {
            int32_t length;

            /* look up the data piece in the common data */
            p_ਸਿਰਲੇਖ=p_Common_ਅੰਕੜੇ->vFuncs->Lookup(p_Common_ਅੰਕੜੇ, toc_ਇੰਦਰਾਜ_Name, &length, sub_Error_ਕੋਡ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_ਇੰਦਰਾਜ_Name, (ਖਾਲੀ*) p_ਸਿਰਲੇਖ, u_errorName(*sub_Error_ਕੋਡ));
#ਜੇ_ਅੰਤ

            ਜੇ(p_ਸਿਰਲੇਖ!=ਖਾਲੀ_ਸੂਚਕ) {
                p_ਇੰਦਰਾਜ_ਅੰਕੜੇ = check_ਅੰਕੜੇ_ਵਸਤੂ(p_ਸਿਰਲੇਖ, isAcceptable, context, type, name, sub_Error_ਕੋਡ, p_Error_ਕੋਡ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (ਖਾਲੀ*) p_ਇੰਦਰਾਜ_ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
                ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
                ਜੇ (p_ਇੰਦਰਾਜ_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
                    p_ਇੰਦਰਾਜ_ਅੰਕੜੇ->length = length;
                    ਵਾਪਸ p_ਇੰਦਰਾਜ_ਅੰਕੜੇ;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        ਜੇ (*sub_Error_ਕੋਡ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
            *p_Error_ਕੋਡ = *sub_Error_ਕੋਡ;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        ਜੇ (!isICUData) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        } ਨਹੀਂਤਾਂ ਜੇ (p_Common_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
            ++common_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ;  /* try the next data package */
        } ਨਹੀਂਤਾਂ ਜੇ ((!checkedExtendedICUData) && extendICUData(sub_Error_ਕੋਡ)) {
            checkedExtendedICUData = ਸੱਚ;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
ਸਥਿਰ UBool is_ਸਮਾਂ_Zone_File(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *type) {
    ਵਾਪਸ ((uprv_strcmp(type, "res") == 0) &&
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
ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *
doOpenChoice(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *name,
             UData_ਸਮ੍ਰਿਤੀ_Is_Acceptable *isAcceptable, ਖਾਲੀ *context,
             UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    UData_ਸਮ੍ਰਿਤੀ         *retVal = ਖਾਲੀ_ਸੂਚਕ;

    ਅਚਲ ਅੱਖਰ         *ਅੰਕੜੇ_ਰਸਤਾ;

    int32_t             toc_ਇੰਦਰਾਜ_Suffix_ਸੂਚਕਾਂਕ;
    ਅਚਲ ਅੱਖਰ         *toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix;
    UError_ਕੋਡ          sub_Error_ਕੋਡ=U_ZERO_ERROR;
    ਅਚਲ ਅੱਖਰ         *ਰੁੱਖ_Char;

    UBool               isICUData = ਝੂਠ;


    FileTracer::traceOpen(ਰਸਤਾ, type, name);


    /* Is this path ICU data? */
    ਜੇ(ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ ||
       !strcmp(ਰਸਤਾ, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(ਰਸਤਾ, U_ICUDATA_NAME U_ਰੁੱਖ_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_ਰੁੱਖ_SEPARATOR_STRING)) ||  
       !uprv_strncmp(ਰਸਤਾ, U_ICUDATA_ALIAS U_ਰੁੱਖ_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_ਰੁੱਖ_SEPARATOR_STRING))) {
      isICUData = ਸੱਚ;
    }

#ਜੇ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_ਰਸਤਾ;
    ਜੇ(ਰਸਤਾ) {
        ਜੇ(uprv_strchr(ਰਸਤਾ,U_FILE_ALT_SEP_CHAR) != ਖਾਲੀ_ਸੂਚਕ) {
            alt_Sep_ਰਸਤਾ.append(ਰਸਤਾ, *p_Error_ਕੋਡ);
            ਅੱਖਰ *p;
            ਜਦੋਂ ((p = uprv_strchr(alt_Sep_ਰਸਤਾ.ਅੰਕੜੇ(), U_FILE_ALT_SEP_CHAR)) != ਖਾਲੀ_ਸੂਚਕ) {
                *p = U_FILE_SEP_CHAR;
            }
#ਜੇ defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", ਰਸਤਾ, alt_Sep_ਰਸਤਾ.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ
            ਰਸਤਾ = alt_Sep_ਰਸਤਾ.ਅੰਕੜੇ();
        }
    }
#ਜੇ_ਅੰਤ

    CharString toc_ਇੰਦਰਾਜ_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_ਇੰਦਰਾਜ_ਰਸਤਾ; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString ਰੁੱਖ_Name;

    /* ======= Set up strings */
    ਜੇ(ਰਸਤਾ==ਖਾਲੀ_ਸੂਚਕ) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ ਅੱਖਰ *pkg;
        ਅਚਲ ਅੱਖਰ *first;
        pkg = uprv_strrchr(ਰਸਤਾ, U_FILE_SEP_CHAR);
        first = uprv_strchr(ਰਸਤਾ, U_FILE_SEP_CHAR);
        ਜੇ(uprv_ਰਸਤਾ_Is_Absolute(ਰਸਤਾ) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            ਜੇ(pkg) {
                pkgName.append(pkg+1, *p_Error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                pkgName.append(ਰਸਤਾ, *p_Error_ਕੋਡ);
            }
        } ਨਹੀਂਤਾਂ {
            ਰੁੱਖ_Char = uprv_strchr(ਰਸਤਾ, U_ਰੁੱਖ_SEPARATOR);
            ਜੇ(ਰੁੱਖ_Char) { 
                ਰੁੱਖ_Name.append(ਰੁੱਖ_Char+1, *p_Error_ਕੋਡ); /* following '-' */
                ਜੇ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ {
                    pkgName.append(ਰਸਤਾ, static_cast<int32_t>(ਰੁੱਖ_Char - ਰਸਤਾ), *p_Error_ਕੋਡ);
                    ਜੇ (first == ਖਾਲੀ_ਸੂਚਕ) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        ਰਸਤਾ = pkgName.ਅੰਕੜੇ();
                    }
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ {
                    pkgName.append(ਰਸਤਾ, *p_Error_ਕੋਡ);
                }
            }
        }
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.ਅੰਕੜੇ(), ਰੁੱਖ_Name.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_ਇੰਦਰਾਜ_Name.append(pkgName, *p_Error_ਕੋਡ);
    toc_ਇੰਦਰਾਜ_ਰਸਤਾ.append(pkgName, *p_Error_ਕੋਡ);
    toc_ਇੰਦਰਾਜ_Suffix_ਸੂਚਕਾਂਕ = toc_ਇੰਦਰਾਜ_Name.length();

    ਜੇ(!ਰੁੱਖ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        toc_ਇੰਦਰਾਜ_Name.append(U_ਰੁੱਖ_ਇੰਦਰਾਜ_SEP_CHAR, *p_Error_ਕੋਡ).append(ਰੁੱਖ_Name, *p_Error_ਕੋਡ);
        toc_ਇੰਦਰਾਜ_ਰਸਤਾ.append(U_FILE_SEP_CHAR, *p_Error_ਕੋਡ).append(ਰੁੱਖ_Name, *p_Error_ਕੋਡ);
    }

    toc_ਇੰਦਰਾਜ_Name.append(U_ਰੁੱਖ_ਇੰਦਰਾਜ_SEP_CHAR, *p_Error_ਕੋਡ).append(name, *p_Error_ਕੋਡ);
    toc_ਇੰਦਰਾਜ_ਰਸਤਾ.append(U_FILE_SEP_CHAR, *p_Error_ਕੋਡ).append(name, *p_Error_ਕੋਡ);
    ਜੇ(type!=ਖਾਲੀ_ਸੂਚਕ && *type!=0) {
        toc_ਇੰਦਰਾਜ_Name.append(".", *p_Error_ਕੋਡ).append(type, *p_Error_ਕੋਡ);
        toc_ਇੰਦਰਾਜ_ਰਸਤਾ.append(".", *p_Error_ਕੋਡ).append(type, *p_Error_ਕੋਡ);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix = toc_ਇੰਦਰਾਜ_ਰਸਤਾ.ਅੰਕੜੇ() + toc_ਇੰਦਰਾਜ_Suffix_ਸੂਚਕਾਂਕ + 1; /* suffix starts here */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_ਇੰਦਰਾਜ_Name.ਅੰਕੜੇ());
    fprintf(stderr, " tocEntryPath = %s\n", toc_ਇੰਦਰਾਜ_ਰਸਤਾ.ਅੰਕੜੇ());
#ਜੇ_ਅੰਤ

#ਜੇ !defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)
    ਜੇ(ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
        ਰਸਤਾ = COMMON_ਅੰਕੜੇ_NAME; /* "icudt26e" */
    }
#ਨਹੀਂਤਾਂ
    // When using the Windows system data, we expects only a single data file.
    ਰਸਤਾ = COMMON_ਅੰਕੜੇ_NAME; /* "icudt26e" */
#ਜੇ_ਅੰਤ

    /************************ Begin loop looking for ind. files ***************/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_ਰਸਤਾ(ਰਸਤਾ));
#ਜੇ_ਅੰਤ

    /* End of dealing with a null basename */
    ਅੰਕੜੇ_ਰਸਤਾ = u_get_ਅੰਕੜੇ_Directory();

    /****    Time zone individual files override  */
    ਜੇ (isICUData && type != ਖਾਲੀ_ਸੂਚਕ && is_ਸਮਾਂ_Zone_File(name, type)) {
        ਅਚਲ ਅੱਖਰ *tzFilesDir = u_get_ਸਮਾਂ_Zone_Files_Directory(p_Error_ਕੋਡ);
        ਜੇ (tzFilesDir[0] != 0) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#ਜੇ_ਅੰਤ
            retVal = do_ਲੱਦੋ_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix,
                            /*ਰਸਤਾ*/ "", type, name, isAcceptable, context, &sub_Error_ਕੋਡ, p_Error_ਕੋਡ);
            ਜੇ((retVal != ਖਾਲੀ_ਸੂਚਕ) || U_FAILURE(*p_Error_ਕੋਡ)) {
                ਵਾਪਸ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    ਜੇ(g_ਅੰਕੜੇ_File_Access == UDATA_PACKAGES_ਪਹਿਲਾ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#ਜੇ_ਅੰਤ
        /* #2 */
        retVal = do_ਲੱਦੋ_From_Common_ਅੰਕੜੇ(isICUData, 
                            pkgName.ਅੰਕੜੇ(), ਅੰਕੜੇ_ਰਸਤਾ, toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix, toc_ਇੰਦਰਾਜ_Name.ਅੰਕੜੇ(),
                            ਰਸਤਾ, type, name, isAcceptable, context, &sub_Error_ਕੋਡ, p_Error_ਕੋਡ);
        ਜੇ((retVal != ਖਾਲੀ_ਸੂਚਕ) || U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    ਜੇ((g_ਅੰਕੜੇ_File_Access==UDATA_PACKAGES_ਪਹਿਲਾ) ||
       (g_ਅੰਕੜੇ_File_Access==UDATA_FILES_ਪਹਿਲਾ)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#ਜੇ_ਅੰਤ
        /* Check to make sure that there is a dataPath to iterate over */
        ਜੇ ((ਅੰਕੜੇ_ਰਸਤਾ && *ਅੰਕੜੇ_ਰਸਤਾ) || !isICUData) {
            retVal = do_ਲੱਦੋ_From_Individual_Files(pkgName.ਅੰਕੜੇ(), ਅੰਕੜੇ_ਰਸਤਾ, toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix,
                            ਰਸਤਾ, type, name, isAcceptable, context, &sub_Error_ਕੋਡ, p_Error_ਕੋਡ);
            ਜੇ((retVal != ਖਾਲੀ_ਸੂਚਕ) || U_FAILURE(*p_Error_ਕੋਡ)) {
                ਵਾਪਸ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    ਜੇ((g_ਅੰਕੜੇ_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_ਅੰਕੜੇ_File_Access==UDATA_FILES_ਪਹਿਲਾ)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#ਜੇ_ਅੰਤ
        retVal = do_ਲੱਦੋ_From_Common_ਅੰਕੜੇ(isICUData,
                            pkgName.ਅੰਕੜੇ(), ਅੰਕੜੇ_ਰਸਤਾ, toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix, toc_ਇੰਦਰਾਜ_Name.ਅੰਕੜੇ(),
                            ਰਸਤਾ, type, name, isAcceptable, context, &sub_Error_ਕੋਡ, p_Error_ਕੋਡ);
        ਜੇ((retVal != ਖਾਲੀ_ਸੂਚਕ) || U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    ਜੇ(g_ਅੰਕੜੇ_File_Access==UDATA_NO_FILES) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#ਜੇ_ਅੰਤ
        retVal = do_ਲੱਦੋ_From_Common_ਅੰਕੜੇ(isICUData,
                            pkgName.ਅੰਕੜੇ(), "", toc_ਇੰਦਰਾਜ_ਰਸਤਾ_Suffix, toc_ਇੰਦਰਾਜ_Name.ਅੰਕੜੇ(),
                            ਰਸਤਾ, type, name, isAcceptable, context, &sub_Error_ਕੋਡ, p_Error_ਕੋਡ);
        ਜੇ((retVal != ਖਾਲੀ_ਸੂਚਕ) || U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ retVal;
        }
    }

    /* data not found */
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        ਜੇ(U_SUCCESS(sub_Error_ਕੋਡ)) {
            /* file not found */
            *p_Error_ਕੋਡ=U_FILE_ACCESS_ERROR;
        } ਨਹੀਂਤਾਂ {
            /* entry point not found or rejected */
            *p_Error_ਕੋਡ=sub_Error_ਕੋਡ;
        }
    }
    ਵਾਪਸ retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_ਸਮ੍ਰਿਤੀ * U_EXPORT2
udata_open(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *name,
           UError_ਕੋਡ *p_Error_ਕੋਡ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (ਰਸਤਾ?ਰਸਤਾ:"nullptr"), name, type);
    fflush(stderr);
#ਜੇ_ਅੰਤ

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ(name==ਖਾਲੀ_ਸੂਚਕ || *name==0) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ doOpenChoice(ਰਸਤਾ, type, name, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
    }
}



U_CAPI UData_ਸਮ੍ਰਿਤੀ * U_EXPORT2
udata_openChoice(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *name,
                 UData_ਸਮ੍ਰਿਤੀ_Is_Acceptable *isAcceptable, ਖਾਲੀ *context,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (ਰਸਤਾ?ਰਸਤਾ:"nullptr"), name, type);
#ਜੇ_ਅੰਤ

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ(name==ਖਾਲੀ_ਸੂਚਕ || *name==0 || isAcceptable==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ doOpenChoice(ਰਸਤਾ, type, name, isAcceptable, context, p_Error_ਕੋਡ);
    }
}



U_CAPI ਖਾਲੀ U_EXPORT2
udata_getInfo(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ, UDataInfo *pInfo) {
    ਜੇ(pInfo!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(p_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ && p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ!=ਖਾਲੀ_ਸੂਚਕ) {
            ਅਚਲ UDataInfo *info=&p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ->info;
            uint16_t ਅੰਕੜੇ_Info_ਮਾਪ=udata_get_Info_ਮਾਪ(info);
            ਜੇ(pInfo->size>ਅੰਕੜੇ_Info_ਮਾਪ) {
                pInfo->size=ਅੰਕੜੇ_Info_ਮਾਪ;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (ਅਚਲ uint16_t *)info+1, pInfo->size-2);
            ਜੇ(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reserved_ਸ਼ਬਦ;
                pInfo->reserved_ਸ਼ਬਦ=(uint16_t)((x<<8)|(x>>8));
            }
        } ਨਹੀਂਤਾਂ {
            pInfo->size=0;
        }
    }
}


U_CAPI ਖਾਲੀ U_EXPORT2 udata_ਸੈੱਟ_File_Access(UDataFileAccess access, UError_ਕੋਡ * /*ਸਥਿਤੀ*/)
{
    // Note: this function is documented as not thread safe.
    g_ਅੰਕੜੇ_File_Access = access;
}
