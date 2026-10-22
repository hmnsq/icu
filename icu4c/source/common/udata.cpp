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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"  /* U_PLATFORM etc. */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/uversion.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "restrace.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "ucmndata.h"
#ಅನ್ತರ್ಭಾವ "udatamem.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "umapfile.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"

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

#ಆದರೆ defined(UDATA_DEBUG)
#   include <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *udata_ಕಂಡುಹಿಡಿ_Cached_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ &err);

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
ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *gCommonICUDataArray[10] = { ಶೂನ್ಯಸೂಚಕ };   // Access protected by icu global mutex.

ಸ್ಥಿರ u_atomic_int32_t g_Have_Tried_ಗಮ್ಯ_ತುಂಬು_Common_ದತ್ತಾಂಶ {0};  //  See extendICUData().

ಸ್ಥಿರ UHashtable  *g_Common_ದತ್ತಾಂಶ_Cache = ಶೂನ್ಯಸೂಚಕ;  /* Global hash table of opened ICU data files.  */
ಸ್ಥಿರ icu::UInitOnce g_Common_ದತ್ತಾಂಶ_Cache_Init_Once {};

#ಆದರೆ !defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)
ಸ್ಥಿರ UDataFileAccess  g_ದತ್ತಾಂಶ_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#ಇಲ್ಲದಿದ್ದರೆ
// If we are using the Windows data directory, then look in one spot only.
ಸ್ಥಿರ UDataFileAccess  g_ದತ್ತಾಂಶ_File_Access = UDATA_NO_FILES;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    ಆದರೆ (g_Common_ದತ್ತಾಂಶ_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_ದತ್ತಾಂಶ_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_ದತ್ತಾಂಶ_Cache = ಶೂನ್ಯಸೂಚಕ;        /*   Cleanup is not thread safe.                */
    }
    g_Common_ದತ್ತಾಂಶ_Cache_Init_Once.reset();

    ಗಾಗಿ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != ಶೂನ್ಯಸೂಚಕ; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = ಶೂನ್ಯಸೂಚಕ;
    }
    g_Have_Tried_ಗಮ್ಯ_ತುಂಬು_Common_ದತ್ತಾಂಶ = 0;

    ಹಿಂತಿರುಗಿ ನಿಜ;                   /* Everything was cleaned up */
}

ಸ್ಥಿರ UBool U_CALLCONV
ಕಂಡುಹಿಡಿ_Common_ICUData_By_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_Basename, UError_ಸಂಕೇತ &err)
{
    UBool ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
    int32_t i;

    UData_ಸ್ಮರಣೆ  *p_ದತ್ತಾಂಶ = udata_ಕಂಡುಹಿಡಿ_Cached_ದತ್ತಾಂಶ(ಒಳ_Basename, err);
    ಆದರೆ (U_FAILURE(err) || p_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ)
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

    {
        Mutex ಬೀಗ;
        ಗಾಗಿ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            ಆದರೆ ((gCommonICUDataArray[i] != ಶೂನ್ಯಸೂಚಕ) && (gCommonICUDataArray[i]->p_ಶೀರ್ಷಿಕೆ == p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ)) {
                /* The data pointer is already in the array. */
                ಸಿಕ್ಕಿತು = ನಿಜ;
                ನಿಲ್ಲಿಸು;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
ಸ್ಥಿರ UBool
ಗಣ_Common_ICUData(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_ಸಂಕೇತ *pErr)
{
    UData_ಸ್ಮರಣೆ  *new_Common_ದತ್ತಾಂಶ = UData_ಸ್ಮರಣೆ_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = ಸುಳ್ಳು;
    ಆದರೆ (U_FAILURE(*pErr)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_ನಿಯೋಜಿಸು(new_Common_ದತ್ತಾಂಶ, p_ದತ್ತಾಂಶ);
    umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
    ಗಾಗಿ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        ಆದರೆ (gCommonICUDataArray[i] == ಶೂನ್ಯಸೂಚಕ) {
            gCommonICUDataArray[i] = new_Common_ದತ್ತಾಂಶ;
            didUpdate = ನಿಜ;
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gCommonICUDataArray[i]->p_ಶೀರ್ಷಿಕೆ == p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ) {
            /* The same data pointer is already in the array. */
            ನಿಲ್ಲಿಸು;
        }
    }
    umtx_unlock(ಶೂನ್ಯಸೂಚಕ);

    ಆದರೆ (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    ಆದರೆ (didUpdate) {
        ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } ಇಲ್ಲದಿದ್ದರೆ {
        uprv_free(new_Common_ದತ್ತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ didUpdate;
}

#ಆದರೆ !defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)

ಸ್ಥಿರ UBool
ಗಣ_Common_ICUData_ಸೂಚಕ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p_ದತ್ತಾಂಶ, UBool /*warn*/, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UData_ಸ್ಮರಣೆ t_ದತ್ತಾಂಶ;
    UData_ಸ್ಮರಣೆ_init(&t_ದತ್ತಾಂಶ);
    UData_ಸ್ಮರಣೆ_ಗಣ_ದತ್ತಾಂಶ(&t_ದತ್ತಾಂಶ, p_ದತ್ತಾಂಶ);
    udata_check_Common_ದತ್ತಾಂಶ(&t_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ ಗಣ_Common_ICUData(&t_ದತ್ತಾಂಶ, ಸುಳ್ಳು, p_Error_ಸಂಕೇತ);
}

#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
ಕಂಡುಹಿಡಿ_Basename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *basename=uprv_strrchr(ಮಾರ್ಗ, U_FILE_SEP_CHAR);
    ಆದರೆ(basename==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ basename+1;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
package_Name_From_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
    ಆದರೆ((ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) || (*ಮಾರ್ಗ == 0)) {
        ಹಿಂತಿರುಗಿ U_ICUDATA_NAME;
    }

    ಮಾರ್ಗ = ಕಂಡುಹಿಡಿ_Basename(ಮಾರ್ಗ);

    ಆದರೆ((ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) || (*ಮಾರ್ಗ == 0)) {
        ಹಿಂತಿರುಗಿ U_ICUDATA_NAME;
    }

    ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;
}
#ಆದರೆ_ಅಂತ್ಯ

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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ದತ್ತಾಂಶ_Cache_Element {
    ಅಕ್ಷರ          *name;
    UData_ಸ್ಮರಣೆ   *ಅಂಶ;
} ದತ್ತಾಂಶ_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV ದತ್ತಾಂಶ_Cache_Element_deleter(ಶೂನ್ಯ *pDCEl) {
    ದತ್ತಾಂಶ_Cache_Element* p = static_cast<ದತ್ತಾಂಶ_Cache_Element*>(pDCEl);
    udata_close(p->ಅಂಶ);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV udata_init_Hash_ಕೋಷ್ಟಕ(UError_ಸಂಕೇತ &err) {
    U_ASSERT(g_Common_ದತ್ತಾಂಶ_Cache == ಶೂನ್ಯಸೂಚಕ);
    g_Common_ದತ್ತಾಂಶ_Cache = uhash_open(uhash_hashChars, uhash_compareChars, ಶೂನ್ಯಸೂಚಕ, &err);
    ಆದರೆ (U_FAILURE(err)) {
       ಹಿಂತಿರುಗಿ;
    }
    U_ASSERT(g_Common_ದತ್ತಾಂಶ_Cache != ಶೂನ್ಯಸೂಚಕ);
    uhash_ಗಣ_ಮೌಲ್ಯ_Deleter(g_Common_ದತ್ತಾಂಶ_Cache, ದತ್ತಾಂಶ_Cache_Element_deleter);
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
ಸ್ಥಿರ UHashtable *udata_get_Hash_ಕೋಷ್ಟಕ(UError_ಸಂಕೇತ &err) {
    umtx_initOnce(g_Common_ದತ್ತಾಂಶ_Cache_Init_Once, &udata_init_Hash_ಕೋಷ್ಟಕ, err);
    ಹಿಂತಿರುಗಿ g_Common_ದತ್ತಾಂಶ_Cache;
}



ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *udata_ಕಂಡುಹಿಡಿ_Cached_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ &err)
{
    UHashtable        *htable;
    UData_ಸ್ಮರಣೆ       *retVal = ಶೂನ್ಯಸೂಚಕ;
    ದತ್ತಾಂಶ_Cache_Element  *el;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ        *ತಳ_Name;

    htable = udata_get_Hash_ಕೋಷ್ಟಕ(err);
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ತಳ_Name = ಕಂಡುಹಿಡಿ_Basename(ಮಾರ್ಗ);   /* Cache remembers only the base name, not the full path. */
    umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
    el = static_cast<ದತ್ತಾಂಶ_Cache_Element*>(uhash_get(htable, ತಳ_Name));
    umtx_unlock(ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ (el != ಶೂನ್ಯಸೂಚಕ) {
        retVal = el->ಅಂಶ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", ತಳ_Name, (ಶೂನ್ಯ*) retVal);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ retVal;
}


ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *udata_cache_ದತ್ತಾಂಶ_ಅಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UData_ಸ್ಮರಣೆ *ಅಂಶ, UError_ಸಂಕೇತ *pErr) {
    ದತ್ತಾಂಶ_Cache_Element *newElement;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ       *ತಳ_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    ದತ್ತಾಂಶ_Cache_Element *old_ಮೌಲ್ಯ = ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_ಕೋಷ್ಟಕ(*pErr);
    ಆದರೆ (U_FAILURE(*pErr)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<ದತ್ತಾಂಶ_Cache_Element*>(uprv_malloc(ಗಾತ್ರ(ದತ್ತಾಂಶ_Cache_Element)));
    ಆದರೆ (newElement == ಶೂನ್ಯಸೂಚಕ) {
        *pErr = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    newElement->ಅಂಶ = UData_ಸ್ಮರಣೆ_create_New_Instance(pErr);
    ಆದರೆ (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UDatamemory_ನಿಯೋಜಿಸು(newElement->ಅಂಶ, ಅಂಶ);

    ತಳ_Name = ಕಂಡುಹಿಡಿ_Basename(ಮಾರ್ಗ);
    nameLen = static_cast<int32_t>(uprv_strlen(ತಳ_Name));
    newElement->name = static_cast<ಅಕ್ಷರ*>(uprv_malloc(nameLen + 1));
    ಆದರೆ (newElement->name == ಶೂನ್ಯಸೂಚಕ) {
        *pErr = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        uprv_free(newElement->ಅಂಶ);
        uprv_free(newElement);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_strcpy(newElement->name, ತಳ_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
    old_ಮೌಲ್ಯ = static_cast<ದತ್ತಾಂಶ_Cache_Element*>(uhash_get(htable, ಮಾರ್ಗ));
    ಆದರೆ (old_ಮೌಲ್ಯ != ಶೂನ್ಯಸೂಚಕ) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(ಶೂನ್ಯಸೂಚಕ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (ಶೂನ್ಯ*) newElement->ಅಂಶ, u_errorName(subErr), (ಶೂನ್ಯ*) newElement->ಅಂಶ->vFuncs);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->ಅಂಶ);
        uprv_free(newElement);
        ಹಿಂತಿರುಗಿ old_ಮೌಲ್ಯ ? old_ಮೌಲ್ಯ->ಅಂಶ : ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ newElement->ಅಂಶ;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

ವರ್ಗ UData_ಮಾರ್ಗ_Iterator
{
ಸಾರ್ವಜನಿಕ:
    UData_ಮಾರ್ಗ_Iterator(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pkg,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, UBool do_Check_ಕೊನೆಯ_Four,
                      UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *next(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ;                              /* working path (u_icudata_Dir) */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮುಂದಿನ_ಮಾರ್ಗ;                          /* path following this one */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  ಅಂಶ_ಮಾರ್ಗ;                          /* path passed in with item name */
    CharString  ಮಾರ್ಗ_ಬಫರ್;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       check_ಕೊನೆಯ_Four;                     /* if true then allow paths such as '/foo/myapp.dat'
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
UData_ಮಾರ್ಗ_Iterator::UData_ಮಾರ್ಗ_Iterator(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pkg,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_Suffix, UBool do_Check_ಕೊನೆಯ_Four,
                                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", ಒಳ_Suffix, ಒಳ_ಮಾರ್ಗ);
#ಆದರೆ_ಅಂತ್ಯ
    /** Path **/
    ಆದರೆ(ಒಳ_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
        ಮಾರ್ಗ = u_get_ದತ್ತಾಂಶ_Directory();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಮಾರ್ಗ = ಒಳ_ಮಾರ್ಗ;
    }

    /** Package **/
    ಆದರೆ(pkg != ಶೂನ್ಯಸೂಚಕ) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_ಸಂಕೇತ).append(pkg, *p_Error_ಸಂಕೇತ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.ದತ್ತಾಂಶ(), packageStub.length());
#ಆದರೆ_ಅಂತ್ಯ
    }

    /** Item **/
    basename = ಕಂಡುಹಿಡಿ_Basename(ಅಂಶ);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    ಆದರೆ(basename == ಅಂಶ) {
        ಮುಂದಿನ_ಮಾರ್ಗ = ಮಾರ್ಗ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಅಂಶ_ಮಾರ್ಗ.append(ಅಂಶ, static_cast<int32_t>(basename - ಅಂಶ), *p_Error_ಸಂಕೇತ);
        ಮುಂದಿನ_ಮಾರ್ಗ = ಅಂಶ_ಮಾರ್ಗ.ದತ್ತಾಂಶ();
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", ಒಳ_Suffix, (ಶೂನ್ಯ*) ಒಳ_Suffix);
#ಆದರೆ_ಅಂತ್ಯ

    /** Suffix  **/
    ಆದರೆ(ಒಳ_Suffix != ಶೂನ್ಯಸೂಚಕ) {
        suffix = ಒಳ_Suffix;
    } ಇಲ್ಲದಿದ್ದರೆ {
        suffix = "";
    }

    check_ಕೊನೆಯ_Four = do_Check_ಕೊನೆಯ_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            ಅಂಶ,
            ಮಾರ್ಗ,
            basename,
            suffix.ದತ್ತಾಂಶ(),
            ಅಂಶ_ಮಾರ್ಗ.ದತ್ತಾಂಶ(),
            ಮುಂದಿನ_ಮಾರ್ಗ,
            check_ಕೊನೆಯ_Four?"true":"false");
#ಆದರೆ_ಅಂತ್ಯ
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *UData_ಮಾರ್ಗ_Iterator::next(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *current_ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
    int32_t     ಮಾರ್ಗ_Len = 0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ_Basename;

    ಮಾಡು
    {
        ಆದರೆ( ಮುಂದಿನ_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ ) {
            ನಿಲ್ಲಿಸು;
        }
        current_ಮಾರ್ಗ = ಮುಂದಿನ_ಮಾರ್ಗ;

        ಆದರೆ(ಮುಂದಿನ_ಮಾರ್ಗ == ಅಂಶ_ಮಾರ್ಗ.ದತ್ತಾಂಶ()) { /* we were processing item's path. */
            ಮುಂದಿನ_ಮಾರ್ಗ = ಮಾರ್ಗ; /* start with regular path next tm. */
            ಮಾರ್ಗ_Len = static_cast<int32_t>(uprv_strlen(current_ಮಾರ್ಗ));
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* fix up next for next time */
            ಮುಂದಿನ_ಮಾರ್ಗ = uprv_strchr(current_ಮಾರ್ಗ, U_ಮಾರ್ಗ_SEP_CHAR);
            ಆದರೆ(ಮುಂದಿನ_ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
                /* segment: entire path */
                ಮಾರ್ಗ_Len = static_cast<int32_t>(uprv_strlen(current_ಮಾರ್ಗ));
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* segment: until next segment */
                ಮಾರ್ಗ_Len = static_cast<int32_t>(ಮುಂದಿನ_ಮಾರ್ಗ - current_ಮಾರ್ಗ);
                /* skip divider */
                ಮುಂದಿನ_ಮಾರ್ಗ ++;
            }
        }

        ಆದರೆ(ಮಾರ್ಗ_Len == 0) {
            ಮುಂದುವರಿ;
        }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_ಮಾರ್ಗ);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            ಗಾಗಿ(qqq=0;qqq<ಮಾರ್ಗ_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಮಾರ್ಗ_ಬಫರ್.clear().append(current_ಮಾರ್ಗ, ಮಾರ್ಗ_Len, *p_Error_ಸಂಕೇತ);

        /* check for .dat files */
        ಮಾರ್ಗ_Basename = ಕಂಡುಹಿಡಿ_Basename(ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ());

        ಆದರೆ(check_ಕೊನೆಯ_Four && 
           (ಮಾರ್ಗ_Len>=4) &&
           uprv_strncmp(ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ() +(ಮಾರ್ಗ_Len-4), suffix.ದತ್ತಾಂಶ(), 4)==0 && /* suffix matches */
           uprv_strncmp(ಕಂಡುಹಿಡಿ_Basename(ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(ಮಾರ್ಗ_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.ದತ್ತಾಂಶ(), ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
            /* do nothing */
        }
        ಇಲ್ಲದಿದ್ದರೆ 
        {       /* regular dir path */
            ಆದರೆ(ಮಾರ್ಗ_ಬಫರ್[ಮಾರ್ಗ_Len-1] != U_FILE_SEP_CHAR) {
                ಆದರೆ((ಮಾರ್ಗ_Len>=4) &&
                   uprv_strncmp(ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ()+(ಮಾರ್ಗ_Len-4), ".dat", 4) == 0)
                {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
                    ಮುಂದುವರಿ;
                }

                /* Check if it is a directory with the same name as our package */
                ಆದರೆ(!packageStub.is_ಖಾಲಿಗಳು() &&
                   (ಮಾರ್ಗ_Len > packageStub.length()) &&
                   !uprv_strcmp(ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ() + ಮಾರ್ಗ_Len - packageStub.length(), packageStub.ದತ್ತಾಂಶ())) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.ದತ್ತಾಂಶ(), basename, basenameLen);
#ಆದರೆ_ಅಂತ್ಯ
                  ಮಾರ್ಗ_ಬಫರ್.truncate(ಮಾರ್ಗ_Len - packageStub.length());
                }
                ಮಾರ್ಗ_ಬಫರ್.append(U_FILE_SEP_CHAR, *p_Error_ಸಂಕೇತ);
            }

            /* + basename */
            ಮಾರ್ಗ_ಬಫರ್.append(packageStub.ದತ್ತಾಂಶ()+1, packageStub.length()-1, *p_Error_ಸಂಕೇತ);

            ಆದರೆ (!suffix.empty())  /* tack on suffix */
            {
                ಆದರೆ (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    ಮಾರ್ಗ_ಬಫರ್.ensure_ತುದಿಗಳು_With_File_Separator(*p_Error_ಸಂಕೇತ);
                }
                ಮಾರ್ಗ_ಬಫರ್.append(suffix, *p_Error_ಸಂಕೇತ);
            }
        }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ

        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_ಬಫರ್.ದತ್ತಾಂಶ();

    } ಆಗ(ಮಾರ್ಗ);

    /* fell way off the end */
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_NAMESPACE_ಕೊನೆ

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#ಆದರೆ !defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
ಬಾಹ್ಯ "C" ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ U_ದತ್ತಾಂಶ_API U_ICUDATA_ನಮೂದು_ಬಿಂದು;
#ಆದರೆ_ಅಂತ್ಯ

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
ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *
open_Common_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,          /*  Path from OpenChoice?          */
               int32_t common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    UData_ಸ್ಮರಣೆ t_ದತ್ತಾಂಶ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ_ಬಫರ್;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_Basename;

    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UData_ಸ್ಮರಣೆ_init(&t_ದತ್ತಾಂಶ);

    /* ??????? TODO revisit this */ 
    ಆದರೆ (common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ >= 0) {
        /* "mini-cache" for common ICU data */
        ಆದರೆ(common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        {
            Mutex ಬೀಗ;
            ಆದರೆ(gCommonICUDataArray[common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ] != ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ gCommonICUDataArray[common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ];
            }
#ಆದರೆ !defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            ಗಾಗಿ(i = 0; i < common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ; ++i) {
                ಆದರೆ(gCommonICUDataArray[i]->p_ಶೀರ್ಷಿಕೆ == &U_ICUDATA_ನಮೂದು_ಬಿಂದು) {
                    /* The linked-in data is already in the list. */
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
            }
#ಆದರೆ_ಅಂತ್ಯ
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
#ಆದರೆ !defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        ಗಣ_Common_ICUData_ಸೂಚಕ(&U_ICUDATA_ನಮೂದು_ಬಿಂದು, ಸುಳ್ಳು, p_Error_ಸಂಕೇತ);
        {
            Mutex ಬೀಗ;
            ಹಿಂತಿರುಗಿ gCommonICUDataArray[common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ];
        }
#ಆದರೆ_ಅಂತ್ಯ
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    ಒಳ_Basename = ಕಂಡುಹಿಡಿ_Basename(ಮಾರ್ಗ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", ಒಳ_Basename);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(*ಒಳ_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", ಮಾರ್ಗ);
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
            *p_Error_ಸಂಕೇತ=U_FILE_ACCESS_ERROR;
        }
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_ಸ್ಮರಣೆ  *ದತ್ತಾಂಶ_ಗಮ್ಯ_Return = udata_ಕಂಡುಹಿಡಿ_Cached_ದತ್ತಾಂಶ(ಒಳ_Basename, *p_Error_ಸಂಕೇತ);
    ಆದರೆ (ದತ್ತಾಂಶ_ಗಮ್ಯ_Return != ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_ಗಮ್ಯ_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_ಮಾರ್ಗ_Iterator iter(u_get_ದತ್ತಾಂಶ_Directory(), ಒಳ_Basename, ಮಾರ್ಗ, ".dat", ನಿಜ, p_Error_ಸಂಕೇತ);

    ಆಗ ((UData_ಸ್ಮರಣೆ_is_Loaded(&t_ದತ್ತಾಂಶ)==ಸುಳ್ಳು) && (ಮಾರ್ಗ_ಬಫರ್ = iter.next(p_Error_ಸಂಕೇತ)) != ಶೂನ್ಯಸೂಚಕ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", ಮಾರ್ಗ_ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
        uprv_mapFile(&t_ದತ್ತಾಂಶ, ಮಾರ್ಗ_ಬಫರ್, p_Error_ಸಂಕೇತ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_ಸ್ಮರಣೆ_is_Loaded(&t_ದತ್ತಾಂಶ)?"LOADED":"not loaded");
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (!UData_ಸ್ಮರಣೆ_is_Loaded(&t_ದತ್ತಾಂಶ)) {
        /* no common data */
        *p_Error_ಸಂಕೇತ=U_FILE_ACCESS_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_ದತ್ತಾಂಶ(&t_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    ಹಿಂತಿರುಗಿ udata_cache_ದತ್ತಾಂಶ_ಅಂಶ(ಒಳ_Basename, &t_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
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
ಸ್ಥಿರ UBool extendICUData(UError_ಸಂಕೇತ *pErr)
{
    UData_ಸ್ಮರಣೆ   *p_ದತ್ತಾಂಶ;
    UData_ಸ್ಮರಣೆ   copyPData;
    UBool         didUpdate = ಸುಳ್ಳು;

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
#ಆದರೆ MAP_IMPLEMENTATION==MAP_STDIO
    ಸ್ಥಿರ UMutex extendICUDataMutex;
    umtx_ಬೀಗ(&extendICUDataMutex);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(!umtx_ತುಂಬು_Acquire(g_Have_Tried_ಗಮ್ಯ_ತುಂಬು_Common_ದತ್ತಾಂಶ)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_ದತ್ತಾಂಶ = open_Common_ದತ್ತಾಂಶ(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_ಸ್ಮರಣೆ_init(&copyPData);
       ಆದರೆ(p_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
          UDatamemory_ನಿಯೋಜಿಸು(&copyPData, p_ದತ್ತಾಂಶ);
          copyPData.map = ಶೂನ್ಯಸೂಚಕ;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = ಶೂನ್ಯಸೂಚಕ; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              ಗಣ_Common_ICUData(&copyPData,/*  The new common data.                                */
                       ಸುಳ್ಳು,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_ಗಮ್ಯ_ತುಂಬು_Common_ದತ್ತಾಂಶ, 1);
    }

    didUpdate = ಕಂಡುಹಿಡಿ_Common_ICUData_By_Name(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#ಆದರೆ MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_ಗಣ_Common_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UData_ಸ್ಮರಣೆ ದತ್ತಾಂಶ_ಸ್ಮರಣೆ;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    /* set the data pointer and test for validity */
    UData_ಸ್ಮರಣೆ_init(&ದತ್ತಾಂಶ_ಸ್ಮರಣೆ);
    UData_ಸ್ಮರಣೆ_ಗಣ_ದತ್ತಾಂಶ(&ದತ್ತಾಂಶ_ಸ್ಮರಣೆ, ದತ್ತಾಂಶ);
    udata_check_Common_ದತ್ತಾಂಶ(&ದತ್ತಾಂಶ_ಸ್ಮರಣೆ, p_Error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {ಹಿಂತಿರುಗಿ;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    ಗಣ_Common_ICUData(&ದತ್ತಾಂಶ_ಸ್ಮರಣೆ, ನಿಜ, p_Error_ಸಂಕೇತ);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_ಗಣ_App_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, UError_ಸಂಕೇತ *err)
{
    UData_ಸ್ಮರಣೆ     udm;

    ಆದರೆ(err==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *err=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    UData_ಸ್ಮರಣೆ_init(&udm);
    UData_ಸ್ಮರಣೆ_ಗಣ_ದತ್ತಾಂಶ(&udm, ದತ್ತಾಂಶ);
    udata_check_Common_ದತ್ತಾಂಶ(&udm, err);
    udata_cache_ದತ್ತಾಂಶ_ಅಂಶ(ಮಾರ್ಗ, &udm, err);
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
ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *
check_ದತ್ತಾಂಶ_ಅಂಶ
(
 ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ         *p_ಶೀರ್ಷಿಕೆ,         /* The data item to be checked.                */
 UData_ಸ್ಮರಣೆ_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 ಶೂನ್ಯ                     *context,         /*   pass-thru param for above.                */
 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ               *type,            /*   pass-thru param for above.                */
 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ               *name,            /*   pass-thru param for above.                */
 UError_ಸಂಕೇತ               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_ಸಂಕೇತ               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_ಸ್ಮರಣೆ  *r_ದತ್ತಾಂಶ_Mem = ಶೂನ್ಯಸೂಚಕ;          /* the new UDataMemory, to be returned.        */

    ಆದರೆ (U_FAILURE(*fatalErr)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic1==0xda &&
        p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic2==0x27 &&
        (isAcceptable==ಶೂನ್ಯಸೂಚಕ || isAcceptable(context, type, name, &p_ಶೀರ್ಷಿಕೆ->info))
    ) {
        r_ದತ್ತಾಂಶ_Mem=UData_ಸ್ಮರಣೆ_create_New_Instance(fatalErr);
        ಆದರೆ (U_FAILURE(*fatalErr)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        r_ದತ್ತಾಂಶ_Mem->p_ಶೀರ್ಷಿಕೆ = p_ಶೀರ್ಷಿಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    ಹಿಂತಿರುಗಿ r_ದತ್ತಾಂಶ_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *do_ತುಂಬು_From_Individual_Files(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pkgName, 
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ_ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *toc_ನಮೂದು_ಮಾರ್ಗ_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
             UData_ಸ್ಮರಣೆ_Is_Acceptable *isAcceptable, ಶೂನ್ಯ *context,
             UError_ಸಂಕೇತ *sub_Error_ಸಂಕೇತ,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ         *ಮಾರ್ಗ_ಬಫರ್;
    UData_ಸ್ಮರಣೆ         ದತ್ತಾಂಶ_ಸ್ಮರಣೆ;
    UData_ಸ್ಮರಣೆ *p_ನಮೂದು_ದತ್ತಾಂಶ;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_ಮಾರ್ಗ_Iterator iter(ದತ್ತಾಂಶ_ಮಾರ್ಗ, pkgName, ಮಾರ್ಗ, toc_ನಮೂದು_ಮಾರ್ಗ_Suffix, ಸುಳ್ಳು, p_Error_ಸಂಕೇತ);

    ಆಗ ((ಮಾರ್ಗ_ಬಫರ್ = iter.next(p_Error_ಸಂಕೇತ)) != ಶೂನ್ಯಸೂಚಕ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", ಮಾರ್ಗ_ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (uprv_mapFile(&ದತ್ತಾಂಶ_ಸ್ಮರಣೆ, ಮಾರ್ಗ_ಬಫರ್, p_Error_ಸಂಕೇತ))
        {
            p_ನಮೂದು_ದತ್ತಾಂಶ = check_ದತ್ತಾಂಶ_ಅಂಶ(ದತ್ತಾಂಶ_ಸ್ಮರಣೆ.p_ಶೀರ್ಷಿಕೆ, isAcceptable, context, type, name, sub_Error_ಸಂಕೇತ, p_Error_ಸಂಕೇತ);
            ಆದರೆ (p_ನಮೂದು_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_ನಮೂದು_ದತ್ತಾಂಶ->mapAddr = ದತ್ತಾಂಶ_ಸ್ಮರಣೆ.mapAddr;
                p_ನಮೂದು_ದತ್ತಾಂಶ->map     = ದತ್ತಾಂಶ_ಸ್ಮರಣೆ.map;
                p_ನಮೂದು_ದತ್ತಾಂಶ->length  = ದತ್ತಾಂಶ_ಸ್ಮರಣೆ.length;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", ಮಾರ್ಗ_ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
                ಹಿಂತಿರುಗಿ p_ನಮೂದು_ದತ್ತಾಂಶ;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&ದತ್ತಾಂಶ_ಸ್ಮರಣೆ);

            /* If we had a nasty error, bail out completely.  */
            ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_ಸ್ಮರಣೆ_is_Loaded(&ದತ್ತಾಂಶ_ಸ್ಮರಣೆ)?"LOADED":"not loaded");
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *do_ತುಂಬು_From_Common_ದತ್ತಾಂಶ(UBool isICUData, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*pkgName*/, 
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*dataPath*/, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*tocEntryPathSuffix*/, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *toc_ನಮೂದು_Name,
            /* following arguments are the same as doOpenChoice itself */
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
             UData_ಸ್ಮರಣೆ_Is_Acceptable *isAcceptable, ಶೂನ್ಯ *context,
             UError_ಸಂಕೇತ *sub_Error_ಸಂಕೇತ,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    UData_ಸ್ಮರಣೆ        *p_ನಮೂದು_ದತ್ತಾಂಶ;
    ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ   *p_ಶೀರ್ಷಿಕೆ;
    UData_ಸ್ಮರಣೆ        *p_Common_ದತ್ತಾಂಶ;
    int32_t            common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ;
    UBool              checkedExtendedICUData = ಸುಳ್ಳು;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    ಗಾಗಿ (common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ = isICUData ? 0 : -1;;) {
        p_Common_ದತ್ತಾಂಶ=open_Common_ದತ್ತಾಂಶ(ಮಾರ್ಗ, common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ, sub_Error_ಸಂಕೇತ); /** search for pkg **/

        ಆದರೆ(U_SUCCESS(*sub_Error_ಸಂಕೇತ) && p_Common_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ) {
            int32_t length;

            /* look up the data piece in the common data */
            p_ಶೀರ್ಷಿಕೆ=p_Common_ದತ್ತಾಂಶ->vFuncs->Lookup(p_Common_ದತ್ತಾಂಶ, toc_ನಮೂದು_Name, &length, sub_Error_ಸಂಕೇತ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_ನಮೂದು_Name, (ಶೂನ್ಯ*) p_ಶೀರ್ಷಿಕೆ, u_errorName(*sub_Error_ಸಂಕೇತ));
#ಆದರೆ_ಅಂತ್ಯ

            ಆದರೆ(p_ಶೀರ್ಷಿಕೆ!=ಶೂನ್ಯಸೂಚಕ) {
                p_ನಮೂದು_ದತ್ತಾಂಶ = check_ದತ್ತಾಂಶ_ಅಂಶ(p_ಶೀರ್ಷಿಕೆ, isAcceptable, context, type, name, sub_Error_ಸಂಕೇತ, p_Error_ಸಂಕೇತ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (ಶೂನ್ಯ*) p_ನಮೂದು_ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
                ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
                ಆದರೆ (p_ನಮೂದು_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
                    p_ನಮೂದು_ದತ್ತಾಂಶ->length = length;
                    ಹಿಂತಿರುಗಿ p_ನಮೂದು_ದತ್ತಾಂಶ;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        ಆದರೆ (*sub_Error_ಸಂಕೇತ == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
            *p_Error_ಸಂಕೇತ = *sub_Error_ಸಂಕೇತ;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        ಆದರೆ (!isICUData) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_Common_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
            ++common_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ;  /* try the next data package */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((!checkedExtendedICUData) && extendICUData(sub_Error_ಸಂಕೇತ)) {
            checkedExtendedICUData = ನಿಜ;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
ಸ್ಥಿರ UBool is_ಸಮಯ_Zone_File(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type) {
    ಹಿಂತಿರುಗಿ ((uprv_strcmp(type, "res") == 0) &&
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
ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *
doOpenChoice(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
             UData_ಸ್ಮರಣೆ_Is_Acceptable *isAcceptable, ಶೂನ್ಯ *context,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    UData_ಸ್ಮರಣೆ         *retVal = ಶೂನ್ಯಸೂಚಕ;

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ         *ದತ್ತಾಂಶ_ಮಾರ್ಗ;

    int32_t             toc_ನಮೂದು_Suffix_ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ         *toc_ನಮೂದು_ಮಾರ್ಗ_Suffix;
    UError_ಸಂಕೇತ          sub_Error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ         *ವೃಕ್ಷ_Char;

    UBool               isICUData = ಸುಳ್ಳು;


    FileTracer::traceOpen(ಮಾರ್ಗ, type, name);


    /* Is this path ICU data? */
    ಆದರೆ(ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ ||
       !strcmp(ಮಾರ್ಗ, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(ಮಾರ್ಗ, U_ICUDATA_NAME U_ವೃಕ್ಷ_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_ವೃಕ್ಷ_SEPARATOR_STRING)) ||  
       !uprv_strncmp(ಮಾರ್ಗ, U_ICUDATA_ALIAS U_ವೃಕ್ಷ_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_ವೃಕ್ಷ_SEPARATOR_STRING))) {
      isICUData = ನಿಜ;
    }

#ಆದರೆ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_ಮಾರ್ಗ;
    ಆದರೆ(ಮಾರ್ಗ) {
        ಆದರೆ(uprv_strchr(ಮಾರ್ಗ,U_FILE_ALT_SEP_CHAR) != ಶೂನ್ಯಸೂಚಕ) {
            alt_Sep_ಮಾರ್ಗ.append(ಮಾರ್ಗ, *p_Error_ಸಂಕೇತ);
            ಅಕ್ಷರ *p;
            ಆಗ ((p = uprv_strchr(alt_Sep_ಮಾರ್ಗ.ದತ್ತಾಂಶ(), U_FILE_ALT_SEP_CHAR)) != ಶೂನ್ಯಸೂಚಕ) {
                *p = U_FILE_SEP_CHAR;
            }
#ಆದರೆ defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", ಮಾರ್ಗ, alt_Sep_ಮಾರ್ಗ.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ
            ಮಾರ್ಗ = alt_Sep_ಮಾರ್ಗ.ದತ್ತಾಂಶ();
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    CharString toc_ನಮೂದು_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_ನಮೂದು_ಮಾರ್ಗ; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString ವೃಕ್ಷ_Name;

    /* ======= Set up strings */
    ಆದರೆ(ಮಾರ್ಗ==ಶೂನ್ಯಸೂಚಕ) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pkg;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *first;
        pkg = uprv_strrchr(ಮಾರ್ಗ, U_FILE_SEP_CHAR);
        first = uprv_strchr(ಮಾರ್ಗ, U_FILE_SEP_CHAR);
        ಆದರೆ(uprv_ಮಾರ್ಗ_Is_Absolute(ಮಾರ್ಗ) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            ಆದರೆ(pkg) {
                pkgName.append(pkg+1, *p_Error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                pkgName.append(ಮಾರ್ಗ, *p_Error_ಸಂಕೇತ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ವೃಕ್ಷ_Char = uprv_strchr(ಮಾರ್ಗ, U_ವೃಕ್ಷ_SEPARATOR);
            ಆದರೆ(ವೃಕ್ಷ_Char) { 
                ವೃಕ್ಷ_Name.append(ವೃಕ್ಷ_Char+1, *p_Error_ಸಂಕೇತ); /* following '-' */
                ಆದರೆ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    pkgName.append(ಮಾರ್ಗ, static_cast<int32_t>(ವೃಕ್ಷ_Char - ಮಾರ್ಗ), *p_Error_ಸಂಕೇತ);
                    ಆದರೆ (first == ಶೂನ್ಯಸೂಚಕ) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        ಮಾರ್ಗ = pkgName.ದತ್ತಾಂಶ();
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    pkgName.append(ಮಾರ್ಗ, *p_Error_ಸಂಕೇತ);
                }
            }
        }
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.ದತ್ತಾಂಶ(), ವೃಕ್ಷ_Name.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_ನಮೂದು_Name.append(pkgName, *p_Error_ಸಂಕೇತ);
    toc_ನಮೂದು_ಮಾರ್ಗ.append(pkgName, *p_Error_ಸಂಕೇತ);
    toc_ನಮೂದು_Suffix_ಸೂಚ್ಯಂಕ = toc_ನಮೂದು_Name.length();

    ಆದರೆ(!ವೃಕ್ಷ_Name.is_ಖಾಲಿಗಳು()) {
        toc_ನಮೂದು_Name.append(U_ವೃಕ್ಷ_ನಮೂದು_SEP_CHAR, *p_Error_ಸಂಕೇತ).append(ವೃಕ್ಷ_Name, *p_Error_ಸಂಕೇತ);
        toc_ನಮೂದು_ಮಾರ್ಗ.append(U_FILE_SEP_CHAR, *p_Error_ಸಂಕೇತ).append(ವೃಕ್ಷ_Name, *p_Error_ಸಂಕೇತ);
    }

    toc_ನಮೂದು_Name.append(U_ವೃಕ್ಷ_ನಮೂದು_SEP_CHAR, *p_Error_ಸಂಕೇತ).append(name, *p_Error_ಸಂಕೇತ);
    toc_ನಮೂದು_ಮಾರ್ಗ.append(U_FILE_SEP_CHAR, *p_Error_ಸಂಕೇತ).append(name, *p_Error_ಸಂಕೇತ);
    ಆದರೆ(type!=ಶೂನ್ಯಸೂಚಕ && *type!=0) {
        toc_ನಮೂದು_Name.append(".", *p_Error_ಸಂಕೇತ).append(type, *p_Error_ಸಂಕೇತ);
        toc_ನಮೂದು_ಮಾರ್ಗ.append(".", *p_Error_ಸಂಕೇತ).append(type, *p_Error_ಸಂಕೇತ);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_ನಮೂದು_ಮಾರ್ಗ_Suffix = toc_ನಮೂದು_ಮಾರ್ಗ.ದತ್ತಾಂಶ() + toc_ನಮೂದು_Suffix_ಸೂಚ್ಯಂಕ + 1; /* suffix starts here */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_ನಮೂದು_Name.ದತ್ತಾಂಶ());
    fprintf(stderr, " tocEntryPath = %s\n", toc_ನಮೂದು_ಮಾರ್ಗ.ದತ್ತಾಂಶ());
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)
    ಆದರೆ(ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
        ಮಾರ್ಗ = COMMON_ದತ್ತಾಂಶ_NAME; /* "icudt26e" */
    }
#ಇಲ್ಲದಿದ್ದರೆ
    // When using the Windows system data, we expects only a single data file.
    ಮಾರ್ಗ = COMMON_ದತ್ತಾಂಶ_NAME; /* "icudt26e" */
#ಆದರೆ_ಅಂತ್ಯ

    /************************ Begin loop looking for ind. files ***************/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_ಮಾರ್ಗ(ಮಾರ್ಗ));
#ಆದರೆ_ಅಂತ್ಯ

    /* End of dealing with a null basename */
    ದತ್ತಾಂಶ_ಮಾರ್ಗ = u_get_ದತ್ತಾಂಶ_Directory();

    /****    Time zone individual files override  */
    ಆದರೆ (isICUData && type != ಶೂನ್ಯಸೂಚಕ && is_ಸಮಯ_Zone_File(name, type)) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tzFilesDir = u_get_ಸಮಯ_Zone_Files_Directory(p_Error_ಸಂಕೇತ);
        ಆದರೆ (tzFilesDir[0] != 0) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#ಆದರೆ_ಅಂತ್ಯ
            retVal = do_ತುಂಬು_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_ನಮೂದು_ಮಾರ್ಗ_Suffix,
                            /*ಮಾರ್ಗ*/ "", type, name, isAcceptable, context, &sub_Error_ಸಂಕೇತ, p_Error_ಸಂಕೇತ);
            ಆದರೆ((retVal != ಶೂನ್ಯಸೂಚಕ) || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    ಆದರೆ(g_ದತ್ತಾಂಶ_File_Access == UDATA_PACKAGES_ಮೊದಲ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#ಆದರೆ_ಅಂತ್ಯ
        /* #2 */
        retVal = do_ತುಂಬು_From_Common_ದತ್ತಾಂಶ(isICUData, 
                            pkgName.ದತ್ತಾಂಶ(), ದತ್ತಾಂಶ_ಮಾರ್ಗ, toc_ನಮೂದು_ಮಾರ್ಗ_Suffix, toc_ನಮೂದು_Name.ದತ್ತಾಂಶ(),
                            ಮಾರ್ಗ, type, name, isAcceptable, context, &sub_Error_ಸಂಕೇತ, p_Error_ಸಂಕೇತ);
        ಆದರೆ((retVal != ಶೂನ್ಯಸೂಚಕ) || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    ಆದರೆ((g_ದತ್ತಾಂಶ_File_Access==UDATA_PACKAGES_ಮೊದಲ) ||
       (g_ದತ್ತಾಂಶ_File_Access==UDATA_FILES_ಮೊದಲ)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#ಆದರೆ_ಅಂತ್ಯ
        /* Check to make sure that there is a dataPath to iterate over */
        ಆದರೆ ((ದತ್ತಾಂಶ_ಮಾರ್ಗ && *ದತ್ತಾಂಶ_ಮಾರ್ಗ) || !isICUData) {
            retVal = do_ತುಂಬು_From_Individual_Files(pkgName.ದತ್ತಾಂಶ(), ದತ್ತಾಂಶ_ಮಾರ್ಗ, toc_ನಮೂದು_ಮಾರ್ಗ_Suffix,
                            ಮಾರ್ಗ, type, name, isAcceptable, context, &sub_Error_ಸಂಕೇತ, p_Error_ಸಂಕೇತ);
            ಆದರೆ((retVal != ಶೂನ್ಯಸೂಚಕ) || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    ಆದರೆ((g_ದತ್ತಾಂಶ_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_ದತ್ತಾಂಶ_File_Access==UDATA_FILES_ಮೊದಲ)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#ಆದರೆ_ಅಂತ್ಯ
        retVal = do_ತುಂಬು_From_Common_ದತ್ತಾಂಶ(isICUData,
                            pkgName.ದತ್ತಾಂಶ(), ದತ್ತಾಂಶ_ಮಾರ್ಗ, toc_ನಮೂದು_ಮಾರ್ಗ_Suffix, toc_ನಮೂದು_Name.ದತ್ತಾಂಶ(),
                            ಮಾರ್ಗ, type, name, isAcceptable, context, &sub_Error_ಸಂಕೇತ, p_Error_ಸಂಕೇತ);
        ಆದರೆ((retVal != ಶೂನ್ಯಸೂಚಕ) || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    ಆದರೆ(g_ದತ್ತಾಂಶ_File_Access==UDATA_NO_FILES) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#ಆದರೆ_ಅಂತ್ಯ
        retVal = do_ತುಂಬು_From_Common_ದತ್ತಾಂಶ(isICUData,
                            pkgName.ದತ್ತಾಂಶ(), "", toc_ನಮೂದು_ಮಾರ್ಗ_Suffix, toc_ನಮೂದು_Name.ದತ್ತಾಂಶ(),
                            ಮಾರ್ಗ, type, name, isAcceptable, context, &sub_Error_ಸಂಕೇತ, p_Error_ಸಂಕೇತ);
        ಆದರೆ((retVal != ಶೂನ್ಯಸೂಚಕ) || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ retVal;
        }
    }

    /* data not found */
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        ಆದರೆ(U_SUCCESS(sub_Error_ಸಂಕೇತ)) {
            /* file not found */
            *p_Error_ಸಂಕೇತ=U_FILE_ACCESS_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* entry point not found or rejected */
            *p_Error_ಸಂಕೇತ=sub_Error_ಸಂಕೇತ;
        }
    }
    ಹಿಂತಿರುಗಿ retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_ಸ್ಮರಣೆ * U_EXPORT2
udata_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (ಮಾರ್ಗ?ಮಾರ್ಗ:"nullptr"), name, type);
    fflush(stderr);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(name==ಶೂನ್ಯಸೂಚಕ || *name==0) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ doOpenChoice(ಮಾರ್ಗ, type, name, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
    }
}



U_CAPI UData_ಸ್ಮರಣೆ * U_EXPORT2
udata_openChoice(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                 UData_ಸ್ಮರಣೆ_Is_Acceptable *isAcceptable, ಶೂನ್ಯ *context,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (ಮಾರ್ಗ?ಮಾರ್ಗ:"nullptr"), name, type);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(name==ಶೂನ್ಯಸೂಚಕ || *name==0 || isAcceptable==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ doOpenChoice(ಮಾರ್ಗ, type, name, isAcceptable, context, p_Error_ಸಂಕೇತ);
    }
}



U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_getInfo(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ, UDataInfo *pInfo) {
    ಆದರೆ(pInfo!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(p_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ && p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ!=ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿರಾಂಕ UDataInfo *info=&p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ->info;
            uint16_t ದತ್ತಾಂಶ_Info_ಗಾತ್ರ=udata_get_Info_ಗಾತ್ರ(info);
            ಆದರೆ(pInfo->size>ದತ್ತಾಂಶ_Info_ಗಾತ್ರ) {
                pInfo->size=ದತ್ತಾಂಶ_Info_ಗಾತ್ರ;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (ಸ್ಥಿರಾಂಕ uint16_t *)info+1, pInfo->size-2);
            ಆದರೆ(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reserved_ಶಬ್ದ;
                pInfo->reserved_ಶಬ್ದ=(uint16_t)((x<<8)|(x>>8));
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            pInfo->size=0;
        }
    }
}


U_CAPI ಶೂನ್ಯ U_EXPORT2 udata_ಗಣ_File_Access(UDataFileAccess access, UError_ಸಂಕೇತ * /*ಸ್ಥಿತಿ*/)
{
    // Note: this function is documented as not thread safe.
    g_ದತ್ತಾಂಶ_File_Access = access;
}
