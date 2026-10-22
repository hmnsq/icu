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

#অন্তর্ভাব "unicode/utypes.h"  /* U_PLATFORM etc. */

#ifdef __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#endif

#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/uversion.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "restrace.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "ucmndata.h"
#অন্তর্ভাব "udatamem.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "umapfile.h"
#অন্তর্ভাব "umutex.h"

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

#যদি defined(UDATA_DEBUG)
#   include <stdio.h>
#endif

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
স্থির UDataMemory *udata_find_Cached_তথ্য(ধ্রুবক অক্ষর *path, UErrorCode &err);

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
স্থির UDataMemory *gCommonICUDataArray[10] = { nullptr };   // Access protected by icu global mutex.

স্থির u_atomic_int32_t g_Have_Tried_To_Load_Common_তথ্য {0};  //  See extendICUData().

স্থির UHashtable  *g_Common_তথ্য_Cache = nullptr;  /* Global hash table of opened ICU data files.  */
স্থির icu::UInitOnce g_Common_তথ্য_Cache_Init_Once {};

#যদি !defined(ICU_তথ্য_DIR_WINDOWS)
স্থির UDataFileAccess  g_তথ্য_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#অন্যথা
// If we are using the Windows data directory, then look in one spot only.
স্থির UDataFileAccess  g_তথ্য_File_Access = UDATA_NO_FILES;
#endif

স্থির UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    যদি (g_Common_তথ্য_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_তথ্য_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_তথ্য_Cache = nullptr;        /*   Cleanup is not thread safe.                */
    }
    g_Common_তথ্য_Cache_Init_Once.reset();

    জন্য (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != nullptr; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = nullptr;
    }
    g_Have_Tried_To_Load_Common_তথ্য = 0;

    ফেরত সত্য;                   /* Everything was cleaned up */
}

স্থির UBool U_CALLCONV
findCommonICUDataByName(ধ্রুবক অক্ষর *inBasename, UErrorCode &err)
{
    UBool found = মিথ্যা;
    int32_t i;

    UDataMemory  *p_তথ্য = udata_find_Cached_তথ্য(inBasename, err);
    যদি (U_FAILURE(err) || p_তথ্য == nullptr)
        ফেরত মিথ্যা;

    {
        Mutex lock;
        জন্য (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            যদি ((gCommonICUDataArray[i] != nullptr) && (gCommonICUDataArray[i]->pHeader == p_তথ্য->pHeader)) {
                /* The data pointer is already in the array. */
                found = সত্য;
                বিরতি;
            }
        }
    }
    ফেরত found;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
স্থির UBool
setCommonICUData(UDataMemory *p_তথ্য,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UErrorCode *pErr)
{
    UDataMemory  *new_Common_তথ্য = UDataMemory_createNewInstance(pErr);
    int32_t i;
    UBool didUpdate = মিথ্যা;
    যদি (U_FAILURE(*pErr)) {
        ফেরত মিথ্যা;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_assign(new_Common_তথ্য, p_তথ্য);
    umtx_lock(nullptr);
    জন্য (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        যদি (gCommonICUDataArray[i] == nullptr) {
            gCommonICUDataArray[i] = new_Common_তথ্য;
            didUpdate = সত্য;
            বিরতি;
        } অন্যথা যদি (gCommonICUDataArray[i]->pHeader == p_তথ্য->pHeader) {
            /* The same data pointer is already in the array. */
            বিরতি;
        }
    }
    umtx_unlock(nullptr);

    যদি (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    যদি (didUpdate) {
        ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } অন্যথা {
        uprv_free(new_Common_তথ্য);
    }
    ফেরত didUpdate;
}

#যদি !defined(ICU_তথ্য_DIR_WINDOWS)

স্থির UBool
setCommonICUDataPointer(ধ্রুবক শূন্য *p_তথ্য, UBool /*warn*/, UErrorCode *pErrorCode) {
    UDataMemory t_তথ্য;
    UDataMemory_init(&t_তথ্য);
    UData_Memory_set_তথ্য(&t_তথ্য, p_তথ্য);
    udata_check_Common_তথ্য(&t_তথ্য, pErrorCode);
    ফেরত setCommonICUData(&t_তথ্য, মিথ্যা, pErrorCode);
}

#endif

স্থির ধ্রুবক অক্ষর *
findBasename(ধ্রুবক অক্ষর *path) {
    ধ্রুবক অক্ষর *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    যদি(basename==nullptr) {
        ফেরত path;
    } অন্যথা {
        ফেরত basename+1;
    }
}

#ifdef UDATA_DEBUG
স্থির ধ্রুবক অক্ষর *
packageNameFromPath(ধ্রুবক অক্ষর *path)
{
    যদি((path == nullptr) || (*path == 0)) {
        ফেরত U_ICUDATA_NAME;
    }

    path = findBasename(path);

    যদি((path == nullptr) || (*path == 0)) {
        ফেরত U_ICUDATA_NAME;
    }

    ফেরত path;
}
#endif

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

প্রকারলেখ কাঠামো তথ্য_Cache_Element {
    অক্ষর          *name;
    UDataMemory   *item;
} তথ্য_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
স্থির শূন্য U_CALLCONV তথ্য_Cache_Element_deleter(শূন্য *pDCEl) {
    তথ্য_Cache_Element* p = static_cast<তথ্য_Cache_Element*>(pDCEl);
    udata_close(p->item);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

স্থির শূন্য U_CALLCONV udata_initHashTable(UErrorCode &err) {
    U_ASSERT(g_Common_তথ্য_Cache == nullptr);
    g_Common_তথ্য_Cache = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &err);
    যদি (U_FAILURE(err)) {
       ফেরত;
    }
    U_ASSERT(g_Common_তথ্য_Cache != nullptr);
    uhash_set_মান_Deleter(g_Common_তথ্য_Cache, তথ্য_Cache_Element_deleter);
    ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
স্থির UHashtable *udata_getHashTable(UErrorCode &err) {
    umtx_initOnce(g_Common_তথ্য_Cache_Init_Once, &udata_initHashTable, err);
    ফেরত g_Common_তথ্য_Cache;
}



স্থির UDataMemory *udata_find_Cached_তথ্য(ধ্রুবক অক্ষর *path, UErrorCode &err)
{
    UHashtable        *htable;
    UDataMemory       *retVal = nullptr;
    তথ্য_Cache_Element  *el;
    ধ্রুবক অক্ষর        *baseName;

    htable = udata_getHashTable(err);
    যদি (U_FAILURE(err)) {
        ফেরত nullptr;
    }

    baseName = findBasename(path);   /* Cache remembers only the base name, not the full path. */
    umtx_lock(nullptr);
    el = static_cast<তথ্য_Cache_Element*>(uhash_get(htable, baseName));
    umtx_unlock(nullptr);
    যদি (el != nullptr) {
        retVal = el->item;
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", baseName, (শূন্য*) retVal);
#endif
    ফেরত retVal;
}


স্থির UDataMemory *udata_cache_তথ্য_Item(ধ্রুবক অক্ষর *path, UDataMemory *item, UErrorCode *pErr) {
    তথ্য_Cache_Element *newElement;
    ধ্রুবক অক্ষর       *baseName;
    int32_t           nameLen;
    UHashtable       *htable;
    তথ্য_Cache_Element *old_মান = nullptr;
    UErrorCode        subErr = U_ZERO_ERROR;

    htable = udata_getHashTable(*pErr);
    যদি (U_FAILURE(*pErr)) {
        ফেরত nullptr;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<তথ্য_Cache_Element*>(uprv_malloc(আকার(তথ্য_Cache_Element)));
    যদি (newElement == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    newElement->item = UDataMemory_createNewInstance(pErr);
    যদি (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        ফেরত nullptr;
    }
    UDatamemory_assign(newElement->item, item);

    baseName = findBasename(path);
    nameLen = static_cast<int32_t>(uprv_strlen(baseName));
    newElement->name = static_cast<অক্ষর*>(uprv_malloc(nameLen + 1));
    যদি (newElement->name == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        ফেরত nullptr;
    }
    uprv_strcpy(newElement->name, baseName);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(nullptr);
    old_মান = static_cast<তথ্য_Cache_Element*>(uhash_get(htable, path));
    যদি (old_মান != nullptr) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    অন্যথা {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(nullptr);

#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (শূন্য*) newElement->item, u_errorName(subErr), (শূন্য*) newElement->item->vFuncs);
#endif

    যদি (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        ফেরত old_মান ? old_মান->item : nullptr;
    }

    ফেরত newElement->item;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

শ্রেণী UDataPathIterator
{
সর্বজনীন:
    UDataPathIterator(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *pkg,
                      ধ্রুবক অক্ষর *item, ধ্রুবক অক্ষর *suffix, UBool doCheckLastFour,
                      UErrorCode *pErrorCode);
    ধ্রুবক অক্ষর *next(UErrorCode *pErrorCode);

ব্যক্তিগত:
    ধ্রুবক অক্ষর *path;                              /* working path (u_icudata_Dir) */
    ধ্রুবক অক্ষর *পরবর্তী_Path;                          /* path following this one */
    ধ্রুবক অক্ষর *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  itemPath;                          /* path passed in with item name */
    CharString  path_বাফার;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       checkLastFour;                     /* if true then allow paths such as '/foo/myapp.dat'
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
UDataPathIterator::UDataPathIterator(ধ্রুবক অক্ষর *inPath, ধ্রুবক অক্ষর *pkg,
                                     ধ্রুবক অক্ষর *item, ধ্রুবক অক্ষর *inSuffix, UBool doCheckLastFour,
                                     UErrorCode *pErrorCode)
{
#ifdef UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", inSuffix, inPath);
#endif
    /** Path **/
    যদি(inPath == nullptr) {
        path = u_get_তথ্য_Directory();
    } অন্যথা {
        path = inPath;
    }

    /** Package **/
    যদি(pkg != nullptr) {
      packageStub.append(U_FILE_SEP_CHAR, *pErrorCode).append(pkg, *pErrorCode);
#ifdef UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.তথ্য(), packageStub.length());
#endif
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    যদি(basename == item) {
        পরবর্তী_Path = path;
    } অন্যথা {
        itemPath.append(item, static_cast<int32_t>(basename - item), *pErrorCode);
        পরবর্তী_Path = itemPath.তথ্য();
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", inSuffix, (শূন্য*) inSuffix);
#endif

    /** Suffix  **/
    যদি(inSuffix != nullptr) {
        suffix = inSuffix;
    } অন্যথা {
        suffix = "";
    }

    checkLastFour = doCheckLastFour;

    /* pathBuffer will hold the output path strings returned by this iterator */

#ifdef UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            item,
            path,
            basename,
            suffix.তথ্য(),
            itemPath.তথ্য(),
            পরবর্তী_Path,
            checkLastFour?"true":"false");
#endif
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ধ্রুবক অক্ষর *UDataPathIterator::next(UErrorCode *pErrorCode)
{
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }

    ধ্রুবক অক্ষর *currentPath = nullptr;
    int32_t     pathLen = 0;
    ধ্রুবক অক্ষর *pathBasename;

    করো
    {
        যদি( পরবর্তী_Path == nullptr ) {
            বিরতি;
        }
        currentPath = পরবর্তী_Path;

        যদি(পরবর্তী_Path == itemPath.তথ্য()) { /* we were processing item's path. */
            পরবর্তী_Path = path; /* start with regular path next tm. */
            pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
        } অন্যথা {
            /* fix up next for next time */
            পরবর্তী_Path = uprv_strchr(currentPath, U_PATH_SEP_CHAR);
            যদি(পরবর্তী_Path == nullptr) {
                /* segment: entire path */
                pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
            } অন্যথা {
                /* segment: until next segment */
                pathLen = static_cast<int32_t>(পরবর্তী_Path - currentPath);
                /* skip divider */
                পরবর্তী_Path ++;
            }
        }

        যদি(pathLen == 0) {
            চালু;
        }

#ifdef UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", currentPath);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            জন্য(qqq=0;qqq<pathLen;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#endif
        path_বাফার.clear().append(currentPath, pathLen, *pErrorCode);

        /* check for .dat files */
        pathBasename = findBasename(path_বাফার.তথ্য());

        যদি(checkLastFour && 
           (pathLen>=4) &&
           uprv_strncmp(path_বাফার.তথ্য() +(pathLen-4), suffix.তথ্য(), 4)==0 && /* suffix matches */
           uprv_strncmp(findBasename(path_বাফার.তথ্য()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(pathBasename)==(basenameLen+4)) { /* base+suffix = full len */

#ifdef UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.তথ্য(), path_বাফার.তথ্য());
#endif
            /* do nothing */
        }
        অন্যথা 
        {       /* regular dir path */
            যদি(path_বাফার[pathLen-1] != U_FILE_SEP_CHAR) {
                যদি((pathLen>=4) &&
                   uprv_strncmp(path_বাফার.তথ্য()+(pathLen-4), ".dat", 4) == 0)
                {
#ifdef UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", path_বাফার.তথ্য());
#endif
                    চালু;
                }

                /* Check if it is a directory with the same name as our package */
                যদি(!packageStub.isEmpty() &&
                   (pathLen > packageStub.length()) &&
                   !uprv_strcmp(path_বাফার.তথ্য() + pathLen - packageStub.length(), packageStub.তথ্য())) {
#ifdef UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.তথ্য(), basename, basenameLen);
#endif
                  path_বাফার.truncate(pathLen - packageStub.length());
                }
                path_বাফার.append(U_FILE_SEP_CHAR, *pErrorCode);
            }

            /* + basename */
            path_বাফার.append(packageStub.তথ্য()+1, packageStub.length()-1, *pErrorCode);

            যদি (!suffix.empty())  /* tack on suffix */
            {
                যদি (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    path_বাফার.ensureEndsWithFileSeparator(*pErrorCode);
                }
                path_বাফার.append(suffix, *pErrorCode);
            }
        }

#ifdef UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", path_বাফার.তথ্য());
#endif

        ফেরত path_বাফার.তথ্য();

    } যখন(path);

    /* fell way off the end */
    ফেরত nullptr;
}

U_NAMESPACE_END

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#যদি !defined(ICU_তথ্য_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
বাহ্যিক "C" ধ্রুবক তথ্য_Header U_তথ্য_API U_ICUDATA_ENTRY_POINT;
#endif

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
স্থির UDataMemory *
open_Common_তথ্য(ধ্রুবক অক্ষর *path,          /*  Path from OpenChoice?          */
               int32_t common_তথ্য_সূচক,   /*  ICU Data (index >= 0) if path == nullptr */
               UErrorCode *pErrorCode)
{
    UDataMemory t_তথ্য;
    ধ্রুবক অক্ষর *path_বাফার;
    ধ্রুবক অক্ষর *inBasename;

    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }

    UDataMemory_init(&t_তথ্য);

    /* ??????? TODO revisit this */ 
    যদি (common_তথ্য_সূচক >= 0) {
        /* "mini-cache" for common ICU data */
        যদি(common_তথ্য_সূচক >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            ফেরত nullptr;
        }
        {
            Mutex lock;
            যদি(gCommonICUDataArray[common_তথ্য_সূচক] != nullptr) {
                ফেরত gCommonICUDataArray[common_তথ্য_সূচক];
            }
#যদি !defined(ICU_তথ্য_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            জন্য(i = 0; i < common_তথ্য_সূচক; ++i) {
                যদি(gCommonICUDataArray[i]->pHeader == &U_ICUDATA_ENTRY_POINT) {
                    /* The linked-in data is already in the list. */
                    ফেরত nullptr;
                }
            }
#endif
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
#যদি !defined(ICU_তথ্য_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        setCommonICUDataPointer(&U_ICUDATA_ENTRY_POINT, মিথ্যা, pErrorCode);
        {
            Mutex lock;
            ফেরত gCommonICUDataArray[common_তথ্য_সূচক];
        }
#endif
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    inBasename = findBasename(path);
#ifdef UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", inBasename);
#endif

    যদি(*inBasename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", path);
#endif
        যদি (U_SUCCESS(*pErrorCode)) {
            *pErrorCode=U_FILE_ACCESS_ERROR;
        }
        ফেরত nullptr;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UDataMemory  *তথ্য_To_Return = udata_find_Cached_তথ্য(inBasename, *pErrorCode);
    যদি (তথ্য_To_Return != nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত তথ্য_To_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UDataPathIterator iter(u_get_তথ্য_Directory(), inBasename, path, ".dat", সত্য, pErrorCode);

    যখন ((UDataMemory_isLoaded(&t_তথ্য)==মিথ্যা) && (path_বাফার = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", path_বাফার);
#endif
        uprv_mapFile(&t_তথ্য, path_বাফার, pErrorCode);
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&t_তথ্য)?"LOADED":"not loaded");
#endif
    }
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }

    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি (!UDataMemory_isLoaded(&t_তথ্য)) {
        /* no common data */
        *pErrorCode=U_FILE_ACCESS_ERROR;
        ফেরত nullptr;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_তথ্য(&t_তথ্য, pErrorCode);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    ফেরত udata_cache_তথ্য_Item(inBasename, &t_তথ্য, pErrorCode);
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
স্থির UBool extendICUData(UErrorCode *pErr)
{
    UDataMemory   *p_তথ্য;
    UDataMemory   copyPData;
    UBool         didUpdate = মিথ্যা;

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
#যদি MAP_IMPLEMENTATION==MAP_STDIO
    স্থির UMutex extendICUDataMutex;
    umtx_lock(&extendICUDataMutex);
#endif
    যদি(!umtx_loadAcquire(g_Have_Tried_To_Load_Common_তথ্য)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_তথ্য = open_Common_তথ্য(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UDataMemory_init(&copyPData);
       যদি(p_তথ্য != nullptr) {
          UDatamemory_assign(&copyPData, p_তথ্য);
          copyPData.map = nullptr;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = nullptr; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              setCommonICUData(&copyPData,/*  The new common data.                                */
                       মিথ্যা,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_To_Load_Common_তথ্য, 1);
    }

    didUpdate = findCommonICUDataByName(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#যদি MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#endif
    ফেরত didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI শূন্য U_EXPORT2
udata_set_Common_তথ্য(ধ্রুবক শূন্য *তথ্য, UErrorCode *pErrorCode) {
    UDataMemory তথ্য_Memory;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত;
    }

    যদি(তথ্য==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    /* set the data pointer and test for validity */
    UDataMemory_init(&তথ্য_Memory);
    UData_Memory_set_তথ্য(&তথ্য_Memory, তথ্য);
    udata_check_Common_তথ্য(&তথ্য_Memory, pErrorCode);
    যদি (U_FAILURE(*pErrorCode)) {ফেরত;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    setCommonICUData(&তথ্য_Memory, সত্য, pErrorCode);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI শূন্য U_EXPORT2
udata_set_App_তথ্য(ধ্রুবক অক্ষর *path, ধ্রুবক শূন্য *তথ্য, UErrorCode *err)
{
    UDataMemory     udm;

    যদি(err==nullptr || U_FAILURE(*err)) {
        ফেরত;
    }
    যদি(তথ্য==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    UDataMemory_init(&udm);
    UData_Memory_set_তথ্য(&udm, তথ্য);
    udata_check_Common_তথ্য(&udm, err);
    udata_cache_তথ্য_Item(path, &udm, err);
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
স্থির UDataMemory *
check_তথ্য_Item
(
 ধ্রুবক তথ্য_Header         *pHeader,         /* The data item to be checked.                */
 UDataMemoryIsAcceptable  *isAcceptable,    /* App's call-back function                    */
 শূন্য                     *context,         /*   pass-thru param for above.                */
 ধ্রুবক অক্ষর               *type,            /*   pass-thru param for above.                */
 ধ্রুবক অক্ষর               *name,            /*   pass-thru param for above.                */
 UErrorCode               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UErrorCode               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UDataMemory  *r_তথ্য_Mem = nullptr;          /* the new UDataMemory, to be returned.        */

    যদি (U_FAILURE(*fatalErr)) {
        ফেরত nullptr;
    }

    যদি(pHeader->তথ্য_Header_2.magic1==0xda &&
        pHeader->তথ্য_Header_2.magic2==0x27 &&
        (isAcceptable==nullptr || isAcceptable(context, type, name, &pHeader->info))
    ) {
        r_তথ্য_Mem=UDataMemory_createNewInstance(fatalErr);
        যদি (U_FAILURE(*fatalErr)) {
            ফেরত nullptr;
        }
        r_তথ্য_Mem->pHeader = pHeader;
    } অন্যথা {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    ফেরত r_তথ্য_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
স্থির UDataMemory *doLoadFromIndividualFiles(ধ্রুবক অক্ষর *pkgName, 
        ধ্রুবক অক্ষর *তথ্য_Path, ধ্রুবক অক্ষর *tocEntryPathSuffix,
            /* following arguments are the same as doOpenChoice itself */
            ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *type, ধ্রুবক অক্ষর *name,
             UDataMemoryIsAcceptable *isAcceptable, শূন্য *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    ধ্রুবক অক্ষর         *path_বাফার;
    UDataMemory         তথ্য_Memory;
    UDataMemory *p_Entry_তথ্য;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UDataPathIterator iter(তথ্য_Path, pkgName, path, tocEntryPathSuffix, মিথ্যা, pErrorCode);

    যখন ((path_বাফার = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", path_বাফার);
#endif
        যদি (uprv_mapFile(&তথ্য_Memory, path_বাফার, pErrorCode))
        {
            p_Entry_তথ্য = check_তথ্য_Item(তথ্য_Memory.pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
            যদি (p_Entry_তথ্য != nullptr) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_Entry_তথ্য->mapAddr = তথ্য_Memory.mapAddr;
                p_Entry_তথ্য->map     = তথ্য_Memory.map;
                p_Entry_তথ্য->length  = তথ্য_Memory.length;

#ifdef UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", path_বাফার);
#endif
                ফেরত p_Entry_তথ্য;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&তথ্য_Memory);

            /* If we had a nasty error, bail out completely.  */
            যদি (U_FAILURE(*pErrorCode)) {
                ফেরত nullptr;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *subErrorCode=U_INVALID_FORMAT_ERROR;
        }
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&তথ্য_Memory)?"LOADED":"not loaded");
#endif
    }
    ফেরত nullptr;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
স্থির UDataMemory *do_Load_From_Common_তথ্য(UBool isICUData, ধ্রুবক অক্ষর * /*pkgName*/, 
        ধ্রুবক অক্ষর * /*dataPath*/, ধ্রুবক অক্ষর * /*tocEntryPathSuffix*/, ধ্রুবক অক্ষর *tocEntryName,
            /* following arguments are the same as doOpenChoice itself */
            ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *type, ধ্রুবক অক্ষর *name,
             UDataMemoryIsAcceptable *isAcceptable, শূন্য *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    UDataMemory        *p_Entry_তথ্য;
    ধ্রুবক তথ্য_Header   *pHeader;
    UDataMemory        *p_Common_তথ্য;
    int32_t            common_তথ্য_সূচক;
    UBool              checkedExtendedICUData = মিথ্যা;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    জন্য (common_তথ্য_সূচক = isICUData ? 0 : -1;;) {
        p_Common_তথ্য=open_Common_তথ্য(path, common_তথ্য_সূচক, subErrorCode); /** search for pkg **/

        যদি(U_SUCCESS(*subErrorCode) && p_Common_তথ্য!=nullptr) {
            int32_t length;

            /* look up the data piece in the common data */
            pHeader=p_Common_তথ্য->vFuncs->Lookup(p_Common_তথ্য, tocEntryName, &length, subErrorCode);
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", tocEntryName, (শূন্য*) pHeader, u_errorName(*subErrorCode));
#endif

            যদি(pHeader!=nullptr) {
                p_Entry_তথ্য = check_তথ্য_Item(pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
#ifdef UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (শূন্য*) p_Entry_তথ্য);
#endif
                যদি (U_FAILURE(*pErrorCode)) {
                    ফেরত nullptr;
                }
                যদি (p_Entry_তথ্য != nullptr) {
                    p_Entry_তথ্য->length = length;
                    ফেরত p_Entry_তথ্য;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        যদি (*subErrorCode == U_MEMORY_ALLOCATION_ERROR) {
            *pErrorCode = *subErrorCode;
            ফেরত nullptr;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        যদি (!isICUData) {
            ফেরত nullptr;
        } অন্যথা যদি (p_Common_তথ্য != nullptr) {
            ++common_তথ্য_সূচক;  /* try the next data package */
        } অন্যথা যদি ((!checkedExtendedICUData) && extendICUData(subErrorCode)) {
            checkedExtendedICUData = সত্য;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } অন্যথা {
            ফেরত nullptr;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
স্থির UBool isTimeZoneFile(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *type) {
    ফেরত ((uprv_strcmp(type, "res") == 0) &&
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
স্থির UDataMemory *
doOpenChoice(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *type, ধ্রুবক অক্ষর *name,
             UDataMemoryIsAcceptable *isAcceptable, শূন্য *context,
             UErrorCode *pErrorCode)
{
    UDataMemory         *retVal = nullptr;

    ধ্রুবক অক্ষর         *তথ্য_Path;

    int32_t             toc_Entry_Suffix_সূচক;
    ধ্রুবক অক্ষর         *tocEntryPathSuffix;
    UErrorCode          subErrorCode=U_ZERO_ERROR;
    ধ্রুবক অক্ষর         *বৃক্ষ_Char;

    UBool               isICUData = মিথ্যা;


    FileTracer::traceOpen(path, type, name);


    /* Is this path ICU data? */
    যদি(path == nullptr ||
       !strcmp(path, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(path, U_ICUDATA_NAME U_বৃক্ষ_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_বৃক্ষ_SEPARATOR_STRING)) ||  
       !uprv_strncmp(path, U_ICUDATA_ALIAS U_বৃক্ষ_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_বৃক্ষ_SEPARATOR_STRING))) {
      isICUData = সত্য;
    }

#যদি (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString altSepPath;
    যদি(path) {
        যদি(uprv_strchr(path,U_FILE_ALT_SEP_CHAR) != nullptr) {
            altSepPath.append(path, *pErrorCode);
            অক্ষর *p;
            যখন ((p = uprv_strchr(altSepPath.তথ্য(), U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
#যদি defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", path, altSepPath.তথ্য());
#endif
            path = altSepPath.তথ্য();
        }
    }
#endif

    CharString tocEntryName; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString tocEntryPath; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString বৃক্ষ_Name;

    /* ======= Set up strings */
    যদি(path==nullptr) {
        pkgName.append(U_ICUDATA_NAME, *pErrorCode);
    } অন্যথা {
        ধ্রুবক অক্ষর *pkg;
        ধ্রুবক অক্ষর *first;
        pkg = uprv_strrchr(path, U_FILE_SEP_CHAR);
        first = uprv_strchr(path, U_FILE_SEP_CHAR);
        যদি(uprv_pathIsAbsolute(path) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            যদি(pkg) {
                pkgName.append(pkg+1, *pErrorCode);
            } অন্যথা {
                pkgName.append(path, *pErrorCode);
            }
        } অন্যথা {
            বৃক্ষ_Char = uprv_strchr(path, U_বৃক্ষ_SEPARATOR);
            যদি(বৃক্ষ_Char) { 
                বৃক্ষ_Name.append(বৃক্ষ_Char+1, *pErrorCode); /* following '-' */
                যদি(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } অন্যথা {
                    pkgName.append(path, static_cast<int32_t>(বৃক্ষ_Char - path), *pErrorCode);
                    যদি (first == nullptr) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        path = pkgName.তথ্য();
                    }
                }
            } অন্যথা {
                যদি(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } অন্যথা {
                    pkgName.append(path, *pErrorCode);
                }
            }
        }
    }

#ifdef UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.তথ্য(), বৃক্ষ_Name.তথ্য());
#endif

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    tocEntryName.append(pkgName, *pErrorCode);
    tocEntryPath.append(pkgName, *pErrorCode);
    toc_Entry_Suffix_সূচক = tocEntryName.length();

    যদি(!বৃক্ষ_Name.isEmpty()) {
        tocEntryName.append(U_বৃক্ষ_ENTRY_SEP_CHAR, *pErrorCode).append(বৃক্ষ_Name, *pErrorCode);
        tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(বৃক্ষ_Name, *pErrorCode);
    }

    tocEntryName.append(U_বৃক্ষ_ENTRY_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    যদি(type!=nullptr && *type!=0) {
        tocEntryName.append(".", *pErrorCode).append(type, *pErrorCode);
        tocEntryPath.append(".", *pErrorCode).append(type, *pErrorCode);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    tocEntryPathSuffix = tocEntryPath.তথ্য() + toc_Entry_Suffix_সূচক + 1; /* suffix starts here */

#ifdef UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", tocEntryName.তথ্য());
    fprintf(stderr, " tocEntryPath = %s\n", tocEntryPath.তথ্য());
#endif

#যদি !defined(ICU_তথ্য_DIR_WINDOWS)
    যদি(path == nullptr) {
        path = COMMON_তথ্য_NAME; /* "icudt26e" */
    }
#অন্যথা
    // When using the Windows system data, we expects only a single data file.
    path = COMMON_তথ্য_NAME; /* "icudt26e" */
#endif

    /************************ Begin loop looking for ind. files ***************/
#ifdef UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", packageNameFromPath(path));
#endif

    /* End of dealing with a null basename */
    তথ্য_Path = u_get_তথ্য_Directory();

    /****    Time zone individual files override  */
    যদি (isICUData && type != nullptr && isTimeZoneFile(name, type)) {
        ধ্রুবক অক্ষর *tzFilesDir = u_getTimeZoneFilesDirectory(pErrorCode);
        যদি (tzFilesDir[0] != 0) {
#ifdef UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#endif
            retVal = doLoadFromIndividualFiles(/* pkgName.data() */ "", tzFilesDir, tocEntryPathSuffix,
                            /* path */ "", type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            যদি((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                ফেরত retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    যদি(g_তথ্য_File_Access == UDATA_PACKAGES_FIRST) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#endif
        /* #2 */
        retVal = do_Load_From_Common_তথ্য(isICUData, 
                            pkgName.তথ্য(), তথ্য_Path, tocEntryPathSuffix, tocEntryName.তথ্য(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        যদি((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ফেরত retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    যদি((g_তথ্য_File_Access==UDATA_PACKAGES_FIRST) ||
       (g_তথ্য_File_Access==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#endif
        /* Check to make sure that there is a dataPath to iterate over */
        যদি ((তথ্য_Path && *তথ্য_Path) || !isICUData) {
            retVal = doLoadFromIndividualFiles(pkgName.তথ্য(), তথ্য_Path, tocEntryPathSuffix,
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            যদি((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                ফেরত retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    যদি((g_তথ্য_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_তথ্য_File_Access==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#endif
        retVal = do_Load_From_Common_তথ্য(isICUData,
                            pkgName.তথ্য(), তথ্য_Path, tocEntryPathSuffix, tocEntryName.তথ্য(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        যদি((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ফেরত retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    যদি(g_তথ্য_File_Access==UDATA_NO_FILES) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#endif
        retVal = do_Load_From_Common_তথ্য(isICUData,
                            pkgName.তথ্য(), "", tocEntryPathSuffix, tocEntryName.তথ্য(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        যদি((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ফেরত retVal;
        }
    }

    /* data not found */
    যদি(U_SUCCESS(*pErrorCode)) {
        যদি(U_SUCCESS(subErrorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } অন্যথা {
            /* entry point not found or rejected */
            *pErrorCode=subErrorCode;
        }
    }
    ফেরত retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *type, ধ্রুবক অক্ষর *name,
           UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
    fflush(stderr);
#endif

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    } অন্যথা যদি(name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    } অন্যথা {
        ফেরত doOpenChoice(path, type, name, nullptr, nullptr, pErrorCode);
    }
}



U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *type, ধ্রুবক অক্ষর *name,
                 UDataMemoryIsAcceptable *isAcceptable, শূন্য *context,
                 UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
#endif

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    } অন্যথা যদি(name==nullptr || *name==0 || isAcceptable==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    } অন্যথা {
        ফেরত doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}



U_CAPI শূন্য U_EXPORT2
udata_getInfo(UDataMemory *p_তথ্য, UDataInfo *pInfo) {
    যদি(pInfo!=nullptr) {
        যদি(p_তথ্য!=nullptr && p_তথ্য->pHeader!=nullptr) {
            ধ্রুবক UDataInfo *info=&p_তথ্য->pHeader->info;
            uint16_t তথ্য_Info_আকার=udata_get_Info_আকার(info);
            যদি(pInfo->size>তথ্য_Info_আকার) {
                pInfo->size=তথ্য_Info_আকার;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (ধ্রুবক uint16_t *)info+1, pInfo->size-2);
            যদি(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reservedWord;
                pInfo->reservedWord=(uint16_t)((x<<8)|(x>>8));
            }
        } অন্যথা {
            pInfo->size=0;
        }
    }
}


U_CAPI শূন্য U_EXPORT2 udata_setFileAccess(UDataFileAccess access, UErrorCode * /*status*/)
{
    // Note: this function is documented as not thread safe.
    g_তথ্য_File_Access = access;
}
