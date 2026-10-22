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

#شامل "unicode/utypes.h"  /* U_PLATFORM etc. */

#اگر_تعریف_شدہ __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#اگر_ختم

#شامل "unicode/putil.h"
#شامل "unicode/udata.h"
#شامل "unicode/uversion.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "putilimp.h"
#شامل "restrace.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "ucmndata.h"
#شامل "udatamem.h"
#شامل "uhash.h"
#شامل "umapfile.h"
#شامل "umutex.h"

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

#اگر defined(UDATA_DEBUG)
#   include <stdio.h>
#اگر_ختم

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
ساکن UData_حافظہ *udata_ڈھونڈو_Cached_ڈیٹا(ثابت حرف *راستہ, UError_رمز &err);

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
ساکن UData_حافظہ *gCommonICUDataArray[10] = { خالی_اشاریہ };   // Access protected by icu global mutex.

ساکن u_atomic_int32_t g_Have_Tried_مقصد_بھرو_Common_ڈیٹا {0};  //  See extendICUData().

ساکن UHashtable  *g_Common_ڈیٹا_Cache = خالی_اشاریہ;  /* Global hash table of opened ICU data files.  */
ساکن icu::UInitOnce g_Common_ڈیٹا_Cache_Init_Once {};

#اگر !defined(ICU_ڈیٹا_DIR_WINDOWS)
ساکن UDataFileAccess  g_ڈیٹا_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#ورنہ
// If we are using the Windows data directory, then look in one spot only.
ساکن UDataFileAccess  g_ڈیٹا_File_Access = UDATA_NO_FILES;
#اگر_ختم

ساکن UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    اگر (g_Common_ڈیٹا_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_ڈیٹا_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_ڈیٹا_Cache = خالی_اشاریہ;        /*   Cleanup is not thread safe.                */
    }
    g_Common_ڈیٹا_Cache_Init_Once.reset();

    برائے (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != خالی_اشاریہ; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = خالی_اشاریہ;
    }
    g_Have_Tried_مقصد_بھرو_Common_ڈیٹا = 0;

    واپس سچ;                   /* Everything was cleaned up */
}

ساکن UBool U_CALLCONV
ڈھونڈو_Common_ICUData_By_Name(ثابت حرف *اندر_Basename, UError_رمز &err)
{
    UBool ملا = جھوٹ;
    int32_t i;

    UData_حافظہ  *p_ڈیٹا = udata_ڈھونڈو_Cached_ڈیٹا(اندر_Basename, err);
    اگر (U_FAILURE(err) || p_ڈیٹا == خالی_اشاریہ)
        واپس جھوٹ;

    {
        Mutex تالا;
        برائے (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            اگر ((gCommonICUDataArray[i] != خالی_اشاریہ) && (gCommonICUDataArray[i]->p_سرخی == p_ڈیٹا->p_سرخی)) {
                /* The data pointer is already in the array. */
                ملا = سچ;
                توڑو;
            }
        }
    }
    واپس ملا;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
ساکن UBool
سیٹ_Common_ICUData(UData_حافظہ *p_ڈیٹا,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_رمز *pErr)
{
    UData_حافظہ  *new_Common_ڈیٹا = UData_حافظہ_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = جھوٹ;
    اگر (U_FAILURE(*pErr)) {
        واپس جھوٹ;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_تفویض(new_Common_ڈیٹا, p_ڈیٹا);
    umtx_تالا(خالی_اشاریہ);
    برائے (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        اگر (gCommonICUDataArray[i] == خالی_اشاریہ) {
            gCommonICUDataArray[i] = new_Common_ڈیٹا;
            didUpdate = سچ;
            توڑو;
        } ورنہ اگر (gCommonICUDataArray[i]->p_سرخی == p_ڈیٹا->p_سرخی) {
            /* The same data pointer is already in the array. */
            توڑو;
        }
    }
    umtx_unlock(خالی_اشاریہ);

    اگر (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    اگر (didUpdate) {
        ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } ورنہ {
        uprv_free(new_Common_ڈیٹا);
    }
    واپس didUpdate;
}

#اگر !defined(ICU_ڈیٹا_DIR_WINDOWS)

ساکن UBool
سیٹ_Common_ICUData_اشارہ_گر(ثابت خالی *p_ڈیٹا, UBool /*warn*/, UError_رمز *p_Error_رمز) {
    UData_حافظہ t_ڈیٹا;
    UData_حافظہ_init(&t_ڈیٹا);
    UData_حافظہ_سیٹ_ڈیٹا(&t_ڈیٹا, p_ڈیٹا);
    udata_check_Common_ڈیٹا(&t_ڈیٹا, p_Error_رمز);
    واپس سیٹ_Common_ICUData(&t_ڈیٹا, جھوٹ, p_Error_رمز);
}

#اگر_ختم

ساکن ثابت حرف *
ڈھونڈو_Basename(ثابت حرف *راستہ) {
    ثابت حرف *basename=uprv_strrchr(راستہ, U_FILE_SEP_CHAR);
    اگر(basename==خالی_اشاریہ) {
        واپس راستہ;
    } ورنہ {
        واپس basename+1;
    }
}

#اگر_تعریف_شدہ UDATA_DEBUG
ساکن ثابت حرف *
package_Name_From_راستہ(ثابت حرف *راستہ)
{
    اگر((راستہ == خالی_اشاریہ) || (*راستہ == 0)) {
        واپس U_ICUDATA_NAME;
    }

    راستہ = ڈھونڈو_Basename(راستہ);

    اگر((راستہ == خالی_اشاریہ) || (*راستہ == 0)) {
        واپس U_ICUDATA_NAME;
    }

    واپس راستہ;
}
#اگر_ختم

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

قسمتعریف ساخت ڈیٹا_Cache_Element {
    حرف          *name;
    UData_حافظہ   *شے;
} ڈیٹا_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
ساکن خالی U_CALLCONV ڈیٹا_Cache_Element_deleter(خالی *pDCEl) {
    ڈیٹا_Cache_Element* p = static_cast<ڈیٹا_Cache_Element*>(pDCEl);
    udata_close(p->شے);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

ساکن خالی U_CALLCONV udata_init_Hash_جدول(UError_رمز &err) {
    U_ASSERT(g_Common_ڈیٹا_Cache == خالی_اشاریہ);
    g_Common_ڈیٹا_Cache = uhash_open(uhash_hashChars, uhash_compareChars, خالی_اشاریہ, &err);
    اگر (U_FAILURE(err)) {
       واپس;
    }
    U_ASSERT(g_Common_ڈیٹا_Cache != خالی_اشاریہ);
    uhash_سیٹ_قدر_Deleter(g_Common_ڈیٹا_Cache, ڈیٹا_Cache_Element_deleter);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
ساکن UHashtable *udata_get_Hash_جدول(UError_رمز &err) {
    umtx_initOnce(g_Common_ڈیٹا_Cache_Init_Once, &udata_init_Hash_جدول, err);
    واپس g_Common_ڈیٹا_Cache;
}



ساکن UData_حافظہ *udata_ڈھونڈو_Cached_ڈیٹا(ثابت حرف *راستہ, UError_رمز &err)
{
    UHashtable        *htable;
    UData_حافظہ       *retVal = خالی_اشاریہ;
    ڈیٹا_Cache_Element  *el;
    ثابت حرف        *بنیاد_Name;

    htable = udata_get_Hash_جدول(err);
    اگر (U_FAILURE(err)) {
        واپس خالی_اشاریہ;
    }

    بنیاد_Name = ڈھونڈو_Basename(راستہ);   /* Cache remembers only the base name, not the full path. */
    umtx_تالا(خالی_اشاریہ);
    el = static_cast<ڈیٹا_Cache_Element*>(uhash_get(htable, بنیاد_Name));
    umtx_unlock(خالی_اشاریہ);
    اگر (el != خالی_اشاریہ) {
        retVal = el->شے;
    }
#اگر_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", بنیاد_Name, (خالی*) retVal);
#اگر_ختم
    واپس retVal;
}


ساکن UData_حافظہ *udata_cache_ڈیٹا_شے(ثابت حرف *راستہ, UData_حافظہ *شے, UError_رمز *pErr) {
    ڈیٹا_Cache_Element *newElement;
    ثابت حرف       *بنیاد_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    ڈیٹا_Cache_Element *old_قدر = خالی_اشاریہ;
    UError_رمز        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_جدول(*pErr);
    اگر (U_FAILURE(*pErr)) {
        واپس خالی_اشاریہ;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<ڈیٹا_Cache_Element*>(uprv_malloc(حجم(ڈیٹا_Cache_Element)));
    اگر (newElement == خالی_اشاریہ) {
        *pErr = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    newElement->شے = UData_حافظہ_create_New_Instance(pErr);
    اگر (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        واپس خالی_اشاریہ;
    }
    UDatamemory_تفویض(newElement->شے, شے);

    بنیاد_Name = ڈھونڈو_Basename(راستہ);
    nameLen = static_cast<int32_t>(uprv_strlen(بنیاد_Name));
    newElement->name = static_cast<حرف*>(uprv_malloc(nameLen + 1));
    اگر (newElement->name == خالی_اشاریہ) {
        *pErr = U_حافظہ_تخصیص_ERROR;
        uprv_free(newElement->شے);
        uprv_free(newElement);
        واپس خالی_اشاریہ;
    }
    uprv_strcpy(newElement->name, بنیاد_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_تالا(خالی_اشاریہ);
    old_قدر = static_cast<ڈیٹا_Cache_Element*>(uhash_get(htable, راستہ));
    اگر (old_قدر != خالی_اشاریہ) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    ورنہ {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(خالی_اشاریہ);

#اگر_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (خالی*) newElement->شے, u_errorName(subErr), (خالی*) newElement->شے->vFuncs);
#اگر_ختم

    اگر (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->شے);
        uprv_free(newElement);
        واپس old_قدر ? old_قدر->شے : خالی_اشاریہ;
    }

    واپس newElement->شے;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

جماعت UData_راستہ_Iterator
{
عوامی:
    UData_راستہ_Iterator(ثابت حرف *راستہ, ثابت حرف *pkg,
                      ثابت حرف *شے, ثابت حرف *suffix, UBool do_Check_آخری_Four,
                      UError_رمز *p_Error_رمز);
    ثابت حرف *next(UError_رمز *p_Error_رمز);

نجی:
    ثابت حرف *راستہ;                              /* working path (u_icudata_Dir) */
    ثابت حرف *اگلا_راستہ;                          /* path following this one */
    ثابت حرف *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  شے_راستہ;                          /* path passed in with item name */
    CharString  راستہ_بفر;                        /* output path for this it'ion */
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
UData_راستہ_Iterator::UData_راستہ_Iterator(ثابت حرف *اندر_راستہ, ثابت حرف *pkg,
                                     ثابت حرف *شے, ثابت حرف *اندر_Suffix, UBool do_Check_آخری_Four,
                                     UError_رمز *p_Error_رمز)
{
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", اندر_Suffix, اندر_راستہ);
#اگر_ختم
    /** Path **/
    اگر(اندر_راستہ == خالی_اشاریہ) {
        راستہ = u_get_ڈیٹا_Directory();
    } ورنہ {
        راستہ = اندر_راستہ;
    }

    /** Package **/
    اگر(pkg != خالی_اشاریہ) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_رمز).append(pkg, *p_Error_رمز);
#اگر_تعریف_شدہ UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.ڈیٹا(), packageStub.length());
#اگر_ختم
    }

    /** Item **/
    basename = ڈھونڈو_Basename(شے);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    اگر(basename == شے) {
        اگلا_راستہ = راستہ;
    } ورنہ {
        شے_راستہ.append(شے, static_cast<int32_t>(basename - شے), *p_Error_رمز);
        اگلا_راستہ = شے_راستہ.ڈیٹا();
    }
#اگر_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", اندر_Suffix, (خالی*) اندر_Suffix);
#اگر_ختم

    /** Suffix  **/
    اگر(اندر_Suffix != خالی_اشاریہ) {
        suffix = اندر_Suffix;
    } ورنہ {
        suffix = "";
    }

    check_آخری_Four = do_Check_آخری_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#اگر_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            شے,
            راستہ,
            basename,
            suffix.ڈیٹا(),
            شے_راستہ.ڈیٹا(),
            اگلا_راستہ,
            check_آخری_Four?"true":"false");
#اگر_ختم
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ثابت حرف *UData_راستہ_Iterator::next(UError_رمز *p_Error_رمز)
{
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }

    ثابت حرف *current_راستہ = خالی_اشاریہ;
    int32_t     راستہ_Len = 0;
    ثابت حرف *راستہ_Basename;

    کرو
    {
        اگر( اگلا_راستہ == خالی_اشاریہ ) {
            توڑو;
        }
        current_راستہ = اگلا_راستہ;

        اگر(اگلا_راستہ == شے_راستہ.ڈیٹا()) { /* we were processing item's path. */
            اگلا_راستہ = راستہ; /* start with regular path next tm. */
            راستہ_Len = static_cast<int32_t>(uprv_strlen(current_راستہ));
        } ورنہ {
            /* fix up next for next time */
            اگلا_راستہ = uprv_strchr(current_راستہ, U_راستہ_SEP_CHAR);
            اگر(اگلا_راستہ == خالی_اشاریہ) {
                /* segment: entire path */
                راستہ_Len = static_cast<int32_t>(uprv_strlen(current_راستہ));
            } ورنہ {
                /* segment: until next segment */
                راستہ_Len = static_cast<int32_t>(اگلا_راستہ - current_راستہ);
                /* skip divider */
                اگلا_راستہ ++;
            }
        }

        اگر(راستہ_Len == 0) {
            جاری;
        }

#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_راستہ);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            برائے(qqq=0;qqq<راستہ_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#اگر_ختم
        راستہ_بفر.clear().append(current_راستہ, راستہ_Len, *p_Error_رمز);

        /* check for .dat files */
        راستہ_Basename = ڈھونڈو_Basename(راستہ_بفر.ڈیٹا());

        اگر(check_آخری_Four && 
           (راستہ_Len>=4) &&
           uprv_strncmp(راستہ_بفر.ڈیٹا() +(راستہ_Len-4), suffix.ڈیٹا(), 4)==0 && /* suffix matches */
           uprv_strncmp(ڈھونڈو_Basename(راستہ_بفر.ڈیٹا()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(راستہ_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#اگر_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.ڈیٹا(), راستہ_بفر.ڈیٹا());
#اگر_ختم
            /* do nothing */
        }
        ورنہ 
        {       /* regular dir path */
            اگر(راستہ_بفر[راستہ_Len-1] != U_FILE_SEP_CHAR) {
                اگر((راستہ_Len>=4) &&
                   uprv_strncmp(راستہ_بفر.ڈیٹا()+(راستہ_Len-4), ".dat", 4) == 0)
                {
#اگر_تعریف_شدہ UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", راستہ_بفر.ڈیٹا());
#اگر_ختم
                    جاری;
                }

                /* Check if it is a directory with the same name as our package */
                اگر(!packageStub.is_خالی_خانے() &&
                   (راستہ_Len > packageStub.length()) &&
                   !uprv_strcmp(راستہ_بفر.ڈیٹا() + راستہ_Len - packageStub.length(), packageStub.ڈیٹا())) {
#اگر_تعریف_شدہ UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.ڈیٹا(), basename, basenameLen);
#اگر_ختم
                  راستہ_بفر.truncate(راستہ_Len - packageStub.length());
                }
                راستہ_بفر.append(U_FILE_SEP_CHAR, *p_Error_رمز);
            }

            /* + basename */
            راستہ_بفر.append(packageStub.ڈیٹا()+1, packageStub.length()-1, *p_Error_رمز);

            اگر (!suffix.empty())  /* tack on suffix */
            {
                اگر (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    راستہ_بفر.ensure_سرے_With_File_Separator(*p_Error_رمز);
                }
                راستہ_بفر.append(suffix, *p_Error_رمز);
            }
        }

#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", راستہ_بفر.ڈیٹا());
#اگر_ختم

        واپس راستہ_بفر.ڈیٹا();

    } جب(راستہ);

    /* fell way off the end */
    واپس خالی_اشاریہ;
}

U_NAMESPACE_انتہا

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#اگر !defined(ICU_ڈیٹا_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
خارجی "C" ثابت ڈیٹا_سرخی U_ڈیٹا_API U_ICUDATA_اندراج_نقطہ;
#اگر_ختم

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
ساکن UData_حافظہ *
open_Common_ڈیٹا(ثابت حرف *راستہ,          /*  Path from OpenChoice?          */
               int32_t common_ڈیٹا_اشاریہ,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_رمز *p_Error_رمز)
{
    UData_حافظہ t_ڈیٹا;
    ثابت حرف *راستہ_بفر;
    ثابت حرف *اندر_Basename;

    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }

    UData_حافظہ_init(&t_ڈیٹا);

    /* ??????? TODO revisit this */ 
    اگر (common_ڈیٹا_اشاریہ >= 0) {
        /* "mini-cache" for common ICU data */
        اگر(common_ڈیٹا_اشاریہ >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            واپس خالی_اشاریہ;
        }
        {
            Mutex تالا;
            اگر(gCommonICUDataArray[common_ڈیٹا_اشاریہ] != خالی_اشاریہ) {
                واپس gCommonICUDataArray[common_ڈیٹا_اشاریہ];
            }
#اگر !defined(ICU_ڈیٹا_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            برائے(i = 0; i < common_ڈیٹا_اشاریہ; ++i) {
                اگر(gCommonICUDataArray[i]->p_سرخی == &U_ICUDATA_اندراج_نقطہ) {
                    /* The linked-in data is already in the list. */
                    واپس خالی_اشاریہ;
                }
            }
#اگر_ختم
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
#اگر !defined(ICU_ڈیٹا_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        سیٹ_Common_ICUData_اشارہ_گر(&U_ICUDATA_اندراج_نقطہ, جھوٹ, p_Error_رمز);
        {
            Mutex تالا;
            واپس gCommonICUDataArray[common_ڈیٹا_اشاریہ];
        }
#اگر_ختم
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    اندر_Basename = ڈھونڈو_Basename(راستہ);
#اگر_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", اندر_Basename);
#اگر_ختم

    اگر(*اندر_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", راستہ);
#اگر_ختم
        اگر (U_SUCCESS(*p_Error_رمز)) {
            *p_Error_رمز=U_FILE_ACCESS_ERROR;
        }
        واپس خالی_اشاریہ;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_حافظہ  *ڈیٹا_مقصد_Return = udata_ڈھونڈو_Cached_ڈیٹا(اندر_Basename, *p_Error_رمز);
    اگر (ڈیٹا_مقصد_Return != خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس ڈیٹا_مقصد_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_راستہ_Iterator iter(u_get_ڈیٹا_Directory(), اندر_Basename, راستہ, ".dat", سچ, p_Error_رمز);

    جب ((UData_حافظہ_is_Loaded(&t_ڈیٹا)==جھوٹ) && (راستہ_بفر = iter.next(p_Error_رمز)) != خالی_اشاریہ)
    {
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", راستہ_بفر);
#اگر_ختم
        uprv_mapFile(&t_ڈیٹا, راستہ_بفر, p_Error_رمز);
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_حافظہ_is_Loaded(&t_ڈیٹا)?"LOADED":"not loaded");
#اگر_ختم
    }
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }

    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر (!UData_حافظہ_is_Loaded(&t_ڈیٹا)) {
        /* no common data */
        *p_Error_رمز=U_FILE_ACCESS_ERROR;
        واپس خالی_اشاریہ;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_ڈیٹا(&t_ڈیٹا, p_Error_رمز);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    واپس udata_cache_ڈیٹا_شے(اندر_Basename, &t_ڈیٹا, p_Error_رمز);
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
ساکن UBool extendICUData(UError_رمز *pErr)
{
    UData_حافظہ   *p_ڈیٹا;
    UData_حافظہ   copyPData;
    UBool         didUpdate = جھوٹ;

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
#اگر MAP_IMPLEMENTATION==MAP_STDIO
    ساکن UMutex extendICUDataMutex;
    umtx_تالا(&extendICUDataMutex);
#اگر_ختم
    اگر(!umtx_بھرو_Acquire(g_Have_Tried_مقصد_بھرو_Common_ڈیٹا)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_ڈیٹا = open_Common_ڈیٹا(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_حافظہ_init(&copyPData);
       اگر(p_ڈیٹا != خالی_اشاریہ) {
          UDatamemory_تفویض(&copyPData, p_ڈیٹا);
          copyPData.map = خالی_اشاریہ;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = خالی_اشاریہ; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              سیٹ_Common_ICUData(&copyPData,/*  The new common data.                                */
                       جھوٹ,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_مقصد_بھرو_Common_ڈیٹا, 1);
    }

    didUpdate = ڈھونڈو_Common_ICUData_By_Name(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#اگر MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#اگر_ختم
    واپس didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI خالی U_EXPORT2
udata_سیٹ_Common_ڈیٹا(ثابت خالی *ڈیٹا, UError_رمز *p_Error_رمز) {
    UData_حافظہ ڈیٹا_حافظہ;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس;
    }

    اگر(ڈیٹا==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    /* set the data pointer and test for validity */
    UData_حافظہ_init(&ڈیٹا_حافظہ);
    UData_حافظہ_سیٹ_ڈیٹا(&ڈیٹا_حافظہ, ڈیٹا);
    udata_check_Common_ڈیٹا(&ڈیٹا_حافظہ, p_Error_رمز);
    اگر (U_FAILURE(*p_Error_رمز)) {واپس;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    سیٹ_Common_ICUData(&ڈیٹا_حافظہ, سچ, p_Error_رمز);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI خالی U_EXPORT2
udata_سیٹ_App_ڈیٹا(ثابت حرف *راستہ, ثابت خالی *ڈیٹا, UError_رمز *err)
{
    UData_حافظہ     udm;

    اگر(err==خالی_اشاریہ || U_FAILURE(*err)) {
        واپس;
    }
    اگر(ڈیٹا==خالی_اشاریہ) {
        *err=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    UData_حافظہ_init(&udm);
    UData_حافظہ_سیٹ_ڈیٹا(&udm, ڈیٹا);
    udata_check_Common_ڈیٹا(&udm, err);
    udata_cache_ڈیٹا_شے(راستہ, &udm, err);
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
ساکن UData_حافظہ *
check_ڈیٹا_شے
(
 ثابت ڈیٹا_سرخی         *p_سرخی,         /* The data item to be checked.                */
 UData_حافظہ_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 خالی                     *context,         /*   pass-thru param for above.                */
 ثابت حرف               *type,            /*   pass-thru param for above.                */
 ثابت حرف               *name,            /*   pass-thru param for above.                */
 UError_رمز               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_رمز               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_حافظہ  *r_ڈیٹا_Mem = خالی_اشاریہ;          /* the new UDataMemory, to be returned.        */

    اگر (U_FAILURE(*fatalErr)) {
        واپس خالی_اشاریہ;
    }

    اگر(p_سرخی->ڈیٹا_سرخی_2.magic1==0xda &&
        p_سرخی->ڈیٹا_سرخی_2.magic2==0x27 &&
        (isAcceptable==خالی_اشاریہ || isAcceptable(context, type, name, &p_سرخی->info))
    ) {
        r_ڈیٹا_Mem=UData_حافظہ_create_New_Instance(fatalErr);
        اگر (U_FAILURE(*fatalErr)) {
            واپس خالی_اشاریہ;
        }
        r_ڈیٹا_Mem->p_سرخی = p_سرخی;
    } ورنہ {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    واپس r_ڈیٹا_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساکن UData_حافظہ *do_بھرو_From_Individual_Files(ثابت حرف *pkgName, 
        ثابت حرف *ڈیٹا_راستہ, ثابت حرف *toc_اندراج_راستہ_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *راستہ, ثابت حرف *type, ثابت حرف *name,
             UData_حافظہ_Is_Acceptable *isAcceptable, خالی *context,
             UError_رمز *sub_Error_رمز,
             UError_رمز *p_Error_رمز)
{
    ثابت حرف         *راستہ_بفر;
    UData_حافظہ         ڈیٹا_حافظہ;
    UData_حافظہ *p_اندراج_ڈیٹا;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_راستہ_Iterator iter(ڈیٹا_راستہ, pkgName, راستہ, toc_اندراج_راستہ_Suffix, جھوٹ, p_Error_رمز);

    جب ((راستہ_بفر = iter.next(p_Error_رمز)) != خالی_اشاریہ)
    {
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", راستہ_بفر);
#اگر_ختم
        اگر (uprv_mapFile(&ڈیٹا_حافظہ, راستہ_بفر, p_Error_رمز))
        {
            p_اندراج_ڈیٹا = check_ڈیٹا_شے(ڈیٹا_حافظہ.p_سرخی, isAcceptable, context, type, name, sub_Error_رمز, p_Error_رمز);
            اگر (p_اندراج_ڈیٹا != خالی_اشاریہ) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_اندراج_ڈیٹا->mapAddr = ڈیٹا_حافظہ.mapAddr;
                p_اندراج_ڈیٹا->map     = ڈیٹا_حافظہ.map;
                p_اندراج_ڈیٹا->length  = ڈیٹا_حافظہ.length;

#اگر_تعریف_شدہ UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", راستہ_بفر);
#اگر_ختم
                واپس p_اندراج_ڈیٹا;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&ڈیٹا_حافظہ);

            /* If we had a nasty error, bail out completely.  */
            اگر (U_FAILURE(*p_Error_رمز)) {
                واپس خالی_اشاریہ;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_رمز=U_INVALID_FORMAT_ERROR;
        }
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_حافظہ_is_Loaded(&ڈیٹا_حافظہ)?"LOADED":"not loaded");
#اگر_ختم
    }
    واپس خالی_اشاریہ;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ساکن UData_حافظہ *do_بھرو_From_Common_ڈیٹا(UBool isICUData, ثابت حرف * /*pkgName*/, 
        ثابت حرف * /*dataPath*/, ثابت حرف * /*tocEntryPathSuffix*/, ثابت حرف *toc_اندراج_Name,
            /* following arguments are the same as doOpenChoice itself */
            ثابت حرف *راستہ, ثابت حرف *type, ثابت حرف *name,
             UData_حافظہ_Is_Acceptable *isAcceptable, خالی *context,
             UError_رمز *sub_Error_رمز,
             UError_رمز *p_Error_رمز)
{
    UData_حافظہ        *p_اندراج_ڈیٹا;
    ثابت ڈیٹا_سرخی   *p_سرخی;
    UData_حافظہ        *p_Common_ڈیٹا;
    int32_t            common_ڈیٹا_اشاریہ;
    UBool              checkedExtendedICUData = جھوٹ;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    برائے (common_ڈیٹا_اشاریہ = isICUData ? 0 : -1;;) {
        p_Common_ڈیٹا=open_Common_ڈیٹا(راستہ, common_ڈیٹا_اشاریہ, sub_Error_رمز); /** search for pkg **/

        اگر(U_SUCCESS(*sub_Error_رمز) && p_Common_ڈیٹا!=خالی_اشاریہ) {
            int32_t length;

            /* look up the data piece in the common data */
            p_سرخی=p_Common_ڈیٹا->vFuncs->Lookup(p_Common_ڈیٹا, toc_اندراج_Name, &length, sub_Error_رمز);
#اگر_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_اندراج_Name, (خالی*) p_سرخی, u_errorName(*sub_Error_رمز));
#اگر_ختم

            اگر(p_سرخی!=خالی_اشاریہ) {
                p_اندراج_ڈیٹا = check_ڈیٹا_شے(p_سرخی, isAcceptable, context, type, name, sub_Error_رمز, p_Error_رمز);
#اگر_تعریف_شدہ UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (خالی*) p_اندراج_ڈیٹا);
#اگر_ختم
                اگر (U_FAILURE(*p_Error_رمز)) {
                    واپس خالی_اشاریہ;
                }
                اگر (p_اندراج_ڈیٹا != خالی_اشاریہ) {
                    p_اندراج_ڈیٹا->length = length;
                    واپس p_اندراج_ڈیٹا;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        اگر (*sub_Error_رمز == U_حافظہ_تخصیص_ERROR) {
            *p_Error_رمز = *sub_Error_رمز;
            واپس خالی_اشاریہ;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        اگر (!isICUData) {
            واپس خالی_اشاریہ;
        } ورنہ اگر (p_Common_ڈیٹا != خالی_اشاریہ) {
            ++common_ڈیٹا_اشاریہ;  /* try the next data package */
        } ورنہ اگر ((!checkedExtendedICUData) && extendICUData(sub_Error_رمز)) {
            checkedExtendedICUData = سچ;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } ورنہ {
            واپس خالی_اشاریہ;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
ساکن UBool is_وقت_Zone_File(ثابت حرف *name, ثابت حرف *type) {
    واپس ((uprv_strcmp(type, "res") == 0) &&
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
ساکن UData_حافظہ *
doOpenChoice(ثابت حرف *راستہ, ثابت حرف *type, ثابت حرف *name,
             UData_حافظہ_Is_Acceptable *isAcceptable, خالی *context,
             UError_رمز *p_Error_رمز)
{
    UData_حافظہ         *retVal = خالی_اشاریہ;

    ثابت حرف         *ڈیٹا_راستہ;

    int32_t             toc_اندراج_Suffix_اشاریہ;
    ثابت حرف         *toc_اندراج_راستہ_Suffix;
    UError_رمز          sub_Error_رمز=U_ZERO_ERROR;
    ثابت حرف         *درخت_Char;

    UBool               isICUData = جھوٹ;


    FileTracer::traceOpen(راستہ, type, name);


    /* Is this path ICU data? */
    اگر(راستہ == خالی_اشاریہ ||
       !strcmp(راستہ, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(راستہ, U_ICUDATA_NAME U_درخت_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_درخت_SEPARATOR_STRING)) ||  
       !uprv_strncmp(راستہ, U_ICUDATA_ALIAS U_درخت_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_درخت_SEPARATOR_STRING))) {
      isICUData = سچ;
    }

#اگر (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_راستہ;
    اگر(راستہ) {
        اگر(uprv_strchr(راستہ,U_FILE_ALT_SEP_CHAR) != خالی_اشاریہ) {
            alt_Sep_راستہ.append(راستہ, *p_Error_رمز);
            حرف *p;
            جب ((p = uprv_strchr(alt_Sep_راستہ.ڈیٹا(), U_FILE_ALT_SEP_CHAR)) != خالی_اشاریہ) {
                *p = U_FILE_SEP_CHAR;
            }
#اگر defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", راستہ, alt_Sep_راستہ.ڈیٹا());
#اگر_ختم
            راستہ = alt_Sep_راستہ.ڈیٹا();
        }
    }
#اگر_ختم

    CharString toc_اندراج_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_اندراج_راستہ; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString درخت_Name;

    /* ======= Set up strings */
    اگر(راستہ==خالی_اشاریہ) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_رمز);
    } ورنہ {
        ثابت حرف *pkg;
        ثابت حرف *first;
        pkg = uprv_strrchr(راستہ, U_FILE_SEP_CHAR);
        first = uprv_strchr(راستہ, U_FILE_SEP_CHAR);
        اگر(uprv_راستہ_Is_Absolute(راستہ) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            اگر(pkg) {
                pkgName.append(pkg+1, *p_Error_رمز);
            } ورنہ {
                pkgName.append(راستہ, *p_Error_رمز);
            }
        } ورنہ {
            درخت_Char = uprv_strchr(راستہ, U_درخت_SEPARATOR);
            اگر(درخت_Char) { 
                درخت_Name.append(درخت_Char+1, *p_Error_رمز); /* following '-' */
                اگر(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_رمز);
                } ورنہ {
                    pkgName.append(راستہ, static_cast<int32_t>(درخت_Char - راستہ), *p_Error_رمز);
                    اگر (first == خالی_اشاریہ) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        راستہ = pkgName.ڈیٹا();
                    }
                }
            } ورنہ {
                اگر(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_رمز);
                } ورنہ {
                    pkgName.append(راستہ, *p_Error_رمز);
                }
            }
        }
    }

#اگر_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.ڈیٹا(), درخت_Name.ڈیٹا());
#اگر_ختم

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_اندراج_Name.append(pkgName, *p_Error_رمز);
    toc_اندراج_راستہ.append(pkgName, *p_Error_رمز);
    toc_اندراج_Suffix_اشاریہ = toc_اندراج_Name.length();

    اگر(!درخت_Name.is_خالی_خانے()) {
        toc_اندراج_Name.append(U_درخت_اندراج_SEP_CHAR, *p_Error_رمز).append(درخت_Name, *p_Error_رمز);
        toc_اندراج_راستہ.append(U_FILE_SEP_CHAR, *p_Error_رمز).append(درخت_Name, *p_Error_رمز);
    }

    toc_اندراج_Name.append(U_درخت_اندراج_SEP_CHAR, *p_Error_رمز).append(name, *p_Error_رمز);
    toc_اندراج_راستہ.append(U_FILE_SEP_CHAR, *p_Error_رمز).append(name, *p_Error_رمز);
    اگر(type!=خالی_اشاریہ && *type!=0) {
        toc_اندراج_Name.append(".", *p_Error_رمز).append(type, *p_Error_رمز);
        toc_اندراج_راستہ.append(".", *p_Error_رمز).append(type, *p_Error_رمز);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_اندراج_راستہ_Suffix = toc_اندراج_راستہ.ڈیٹا() + toc_اندراج_Suffix_اشاریہ + 1; /* suffix starts here */

#اگر_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_اندراج_Name.ڈیٹا());
    fprintf(stderr, " tocEntryPath = %s\n", toc_اندراج_راستہ.ڈیٹا());
#اگر_ختم

#اگر !defined(ICU_ڈیٹا_DIR_WINDOWS)
    اگر(راستہ == خالی_اشاریہ) {
        راستہ = COMMON_ڈیٹا_NAME; /* "icudt26e" */
    }
#ورنہ
    // When using the Windows system data, we expects only a single data file.
    راستہ = COMMON_ڈیٹا_NAME; /* "icudt26e" */
#اگر_ختم

    /************************ Begin loop looking for ind. files ***************/
#اگر_تعریف_شدہ UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_راستہ(راستہ));
#اگر_ختم

    /* End of dealing with a null basename */
    ڈیٹا_راستہ = u_get_ڈیٹا_Directory();

    /****    Time zone individual files override  */
    اگر (isICUData && type != خالی_اشاریہ && is_وقت_Zone_File(name, type)) {
        ثابت حرف *tzFilesDir = u_get_وقت_Zone_Files_Directory(p_Error_رمز);
        اگر (tzFilesDir[0] != 0) {
#اگر_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#اگر_ختم
            retVal = do_بھرو_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_اندراج_راستہ_Suffix,
                            /*راستہ*/ "", type, name, isAcceptable, context, &sub_Error_رمز, p_Error_رمز);
            اگر((retVal != خالی_اشاریہ) || U_FAILURE(*p_Error_رمز)) {
                واپس retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    اگر(g_ڈیٹا_File_Access == UDATA_PACKAGES_پہلا) {
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#اگر_ختم
        /* #2 */
        retVal = do_بھرو_From_Common_ڈیٹا(isICUData, 
                            pkgName.ڈیٹا(), ڈیٹا_راستہ, toc_اندراج_راستہ_Suffix, toc_اندراج_Name.ڈیٹا(),
                            راستہ, type, name, isAcceptable, context, &sub_Error_رمز, p_Error_رمز);
        اگر((retVal != خالی_اشاریہ) || U_FAILURE(*p_Error_رمز)) {
            واپس retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    اگر((g_ڈیٹا_File_Access==UDATA_PACKAGES_پہلا) ||
       (g_ڈیٹا_File_Access==UDATA_FILES_پہلا)) {
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#اگر_ختم
        /* Check to make sure that there is a dataPath to iterate over */
        اگر ((ڈیٹا_راستہ && *ڈیٹا_راستہ) || !isICUData) {
            retVal = do_بھرو_From_Individual_Files(pkgName.ڈیٹا(), ڈیٹا_راستہ, toc_اندراج_راستہ_Suffix,
                            راستہ, type, name, isAcceptable, context, &sub_Error_رمز, p_Error_رمز);
            اگر((retVal != خالی_اشاریہ) || U_FAILURE(*p_Error_رمز)) {
                واپس retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    اگر((g_ڈیٹا_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_ڈیٹا_File_Access==UDATA_FILES_پہلا)) {
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#اگر_ختم
        retVal = do_بھرو_From_Common_ڈیٹا(isICUData,
                            pkgName.ڈیٹا(), ڈیٹا_راستہ, toc_اندراج_راستہ_Suffix, toc_اندراج_Name.ڈیٹا(),
                            راستہ, type, name, isAcceptable, context, &sub_Error_رمز, p_Error_رمز);
        اگر((retVal != خالی_اشاریہ) || U_FAILURE(*p_Error_رمز)) {
            واپس retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    اگر(g_ڈیٹا_File_Access==UDATA_NO_FILES) {
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#اگر_ختم
        retVal = do_بھرو_From_Common_ڈیٹا(isICUData,
                            pkgName.ڈیٹا(), "", toc_اندراج_راستہ_Suffix, toc_اندراج_Name.ڈیٹا(),
                            راستہ, type, name, isAcceptable, context, &sub_Error_رمز, p_Error_رمز);
        اگر((retVal != خالی_اشاریہ) || U_FAILURE(*p_Error_رمز)) {
            واپس retVal;
        }
    }

    /* data not found */
    اگر(U_SUCCESS(*p_Error_رمز)) {
        اگر(U_SUCCESS(sub_Error_رمز)) {
            /* file not found */
            *p_Error_رمز=U_FILE_ACCESS_ERROR;
        } ورنہ {
            /* entry point not found or rejected */
            *p_Error_رمز=sub_Error_رمز;
        }
    }
    واپس retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_حافظہ * U_EXPORT2
udata_open(ثابت حرف *راستہ, ثابت حرف *type, ثابت حرف *name,
           UError_رمز *p_Error_رمز) {
#اگر_تعریف_شدہ UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (راستہ?راستہ:"nullptr"), name, type);
    fflush(stderr);
#اگر_ختم

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    } ورنہ اگر(name==خالی_اشاریہ || *name==0) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    } ورنہ {
        واپس doOpenChoice(راستہ, type, name, خالی_اشاریہ, خالی_اشاریہ, p_Error_رمز);
    }
}



U_CAPI UData_حافظہ * U_EXPORT2
udata_openChoice(ثابت حرف *راستہ, ثابت حرف *type, ثابت حرف *name,
                 UData_حافظہ_Is_Acceptable *isAcceptable, خالی *context,
                 UError_رمز *p_Error_رمز) {
#اگر_تعریف_شدہ UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (راستہ?راستہ:"nullptr"), name, type);
#اگر_ختم

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    } ورنہ اگر(name==خالی_اشاریہ || *name==0 || isAcceptable==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    } ورنہ {
        واپس doOpenChoice(راستہ, type, name, isAcceptable, context, p_Error_رمز);
    }
}



U_CAPI خالی U_EXPORT2
udata_getInfo(UData_حافظہ *p_ڈیٹا, UDataInfo *pInfo) {
    اگر(pInfo!=خالی_اشاریہ) {
        اگر(p_ڈیٹا!=خالی_اشاریہ && p_ڈیٹا->p_سرخی!=خالی_اشاریہ) {
            ثابت UDataInfo *info=&p_ڈیٹا->p_سرخی->info;
            uint16_t ڈیٹا_Info_سائز=udata_get_Info_سائز(info);
            اگر(pInfo->size>ڈیٹا_Info_سائز) {
                pInfo->size=ڈیٹا_Info_سائز;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (ثابت uint16_t *)info+1, pInfo->size-2);
            اگر(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reserved_لفظ;
                pInfo->reserved_لفظ=(uint16_t)((x<<8)|(x>>8));
            }
        } ورنہ {
            pInfo->size=0;
        }
    }
}


U_CAPI خالی U_EXPORT2 udata_سیٹ_File_Access(UDataFileAccess access, UError_رمز * /*حالت*/)
{
    // Note: this function is documented as not thread safe.
    g_ڈیٹا_File_Access = access;
}
