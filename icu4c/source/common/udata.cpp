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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"  /* U_PLATFORM etc. */

#ifdef __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#endif

#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uversion.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "restrace.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "ucmndata.h"
#ଅନ୍ତର୍ଭାଵ "udatamem.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "umapfile.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"

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

#ଯଦି defined(UDATA_DEBUG)
#   include <stdio.h>
#endif

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
ସ୍ଥିର UDataMemory *udata_findCachedData(ଅଚଳ ଅକ୍ଷର *path, UErrorCode &err);

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
ସ୍ଥିର UDataMemory *gCommonICUDataArray[10] = { nullptr };   // Access protected by icu global mutex.

ସ୍ଥିର u_atomic_int32_t gHaveTriedToLoadCommonData {0};  //  See extendICUData().

ସ୍ଥିର UHashtable  *gCommonDataCache = nullptr;  /* Global hash table of opened ICU data files.  */
ସ୍ଥିର icu::UInitOnce gCommonDataCacheInitOnce {};

#ଯଦି !defined(ICU_DATA_DIR_WINDOWS)
ସ୍ଥିର UDataFileAccess  gDataFileAccess = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#ଅନ୍ୟଥା
// If we are using the Windows data directory, then look in one spot only.
ସ୍ଥିର UDataFileAccess  gDataFileAccess = UDATA_NO_FILES;
#endif

ସ୍ଥିର UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    ଯଦି (gCommonDataCache) {             /* Delete the cache of user data mappings.  */
        uhash_close(gCommonDataCache);  /*   Table owns the contents, and will delete them. */
        gCommonDataCache = nullptr;        /*   Cleanup is not thread safe.                */
    }
    gCommonDataCacheInitOnce.reset();

    ପାଇଁ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != nullptr; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = nullptr;
    }
    gHaveTriedToLoadCommonData = 0;

    ଫେରନ୍ତୁ ସତ୍ୟ;                   /* Everything was cleaned up */
}

ସ୍ଥିର UBool U_CALLCONV
findCommonICUDataByName(ଅଚଳ ଅକ୍ଷର *inBasename, UErrorCode &err)
{
    UBool found = ମିଥ୍ୟା;
    int32_t i;

    UDataMemory  *pData = udata_findCachedData(inBasename, err);
    ଯଦି (U_FAILURE(err) || pData == nullptr)
        ଫେରନ୍ତୁ ମିଥ୍ୟା;

    {
        Mutex lock;
        ପାଇଁ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            ଯଦି ((gCommonICUDataArray[i] != nullptr) && (gCommonICUDataArray[i]->pHeader == pData->pHeader)) {
                /* The data pointer is already in the array. */
                found = ସତ୍ୟ;
                ଭାଙ୍ଗ;
            }
        }
    }
    ଫେରନ୍ତୁ found;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
ସ୍ଥିର UBool
setCommonICUData(UDataMemory *pData,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UErrorCode *pErr)
{
    UDataMemory  *newCommonData = UDataMemory_createNewInstance(pErr);
    int32_t i;
    UBool didUpdate = ମିଥ୍ୟା;
    ଯଦି (U_FAILURE(*pErr)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_assign(newCommonData, pData);
    umtx_lock(nullptr);
    ପାଇଁ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        ଯଦି (gCommonICUDataArray[i] == nullptr) {
            gCommonICUDataArray[i] = newCommonData;
            didUpdate = ସତ୍ୟ;
            ଭାଙ୍ଗ;
        } ଅନ୍ୟଥା ଯଦି (gCommonICUDataArray[i]->pHeader == pData->pHeader) {
            /* The same data pointer is already in the array. */
            ଭାଙ୍ଗ;
        }
    }
    umtx_unlock(nullptr);

    ଯଦି (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    ଯଦି (didUpdate) {
        ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } ଅନ୍ୟଥା {
        uprv_free(newCommonData);
    }
    ଫେରନ୍ତୁ didUpdate;
}

#ଯଦି !defined(ICU_DATA_DIR_WINDOWS)

ସ୍ଥିର UBool
setCommonICUDataPointer(ଅଚଳ ଶୂନ୍ୟ *pData, UBool /*warn*/, UErrorCode *pErrorCode) {
    UDataMemory tData;
    UDataMemory_init(&tData);
    UDataMemory_setData(&tData, pData);
    udata_checkCommonData(&tData, pErrorCode);
    ଫେରନ୍ତୁ setCommonICUData(&tData, ମିଥ୍ୟା, pErrorCode);
}

#endif

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *
findBasename(ଅଚଳ ଅକ୍ଷର *path) {
    ଅଚଳ ଅକ୍ଷର *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    ଯଦି(basename==nullptr) {
        ଫେରନ୍ତୁ path;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ basename+1;
    }
}

#ifdef UDATA_DEBUG
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *
packageNameFromPath(ଅଚଳ ଅକ୍ଷର *path)
{
    ଯଦି((path == nullptr) || (*path == 0)) {
        ଫେରନ୍ତୁ U_ICUDATA_NAME;
    }

    path = findBasename(path);

    ଯଦି((path == nullptr) || (*path == 0)) {
        ଫେରନ୍ତୁ U_ICUDATA_NAME;
    }

    ଫେରନ୍ତୁ path;
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା DataCacheElement {
    ଅକ୍ଷର          *name;
    UDataMemory   *item;
} DataCacheElement;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV DataCacheElement_deleter(ଶୂନ୍ୟ *pDCEl) {
    DataCacheElement* p = static_cast<DataCacheElement*>(pDCEl);
    udata_close(p->item);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV udata_initHashTable(UErrorCode &err) {
    U_ASSERT(gCommonDataCache == nullptr);
    gCommonDataCache = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &err);
    ଯଦି (U_FAILURE(err)) {
       ଫେରନ୍ତୁ;
    }
    U_ASSERT(gCommonDataCache != nullptr);
    uhash_set_ମୂଲ୍ୟ_Deleter(gCommonDataCache, DataCacheElement_deleter);
    ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
ସ୍ଥିର UHashtable *udata_getHashTable(UErrorCode &err) {
    umtx_initOnce(gCommonDataCacheInitOnce, &udata_initHashTable, err);
    ଫେରନ୍ତୁ gCommonDataCache;
}



ସ୍ଥିର UDataMemory *udata_findCachedData(ଅଚଳ ଅକ୍ଷର *path, UErrorCode &err)
{
    UHashtable        *htable;
    UDataMemory       *retVal = nullptr;
    DataCacheElement  *el;
    ଅଚଳ ଅକ୍ଷର        *baseName;

    htable = udata_getHashTable(err);
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ nullptr;
    }

    baseName = findBasename(path);   /* Cache remembers only the base name, not the full path. */
    umtx_lock(nullptr);
    el = static_cast<DataCacheElement*>(uhash_get(htable, baseName));
    umtx_unlock(nullptr);
    ଯଦି (el != nullptr) {
        retVal = el->item;
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", baseName, (ଶୂନ୍ୟ*) retVal);
#endif
    ଫେରନ୍ତୁ retVal;
}


ସ୍ଥିର UDataMemory *udata_cacheDataItem(ଅଚଳ ଅକ୍ଷର *path, UDataMemory *item, UErrorCode *pErr) {
    DataCacheElement *newElement;
    ଅଚଳ ଅକ୍ଷର       *baseName;
    int32_t           nameLen;
    UHashtable       *htable;
    DataCacheElement *old_ମୂଲ୍ୟ = nullptr;
    UErrorCode        subErr = U_ZERO_ERROR;

    htable = udata_getHashTable(*pErr);
    ଯଦି (U_FAILURE(*pErr)) {
        ଫେରନ୍ତୁ nullptr;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<DataCacheElement*>(uprv_malloc(ଆକାର(DataCacheElement)));
    ଯଦି (newElement == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    newElement->item = UDataMemory_createNewInstance(pErr);
    ଯଦି (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        ଫେରନ୍ତୁ nullptr;
    }
    UDatamemory_assign(newElement->item, item);

    baseName = findBasename(path);
    nameLen = static_cast<int32_t>(uprv_strlen(baseName));
    newElement->name = static_cast<ଅକ୍ଷର*>(uprv_malloc(nameLen + 1));
    ଯଦି (newElement->name == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_strcpy(newElement->name, baseName);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(nullptr);
    old_ମୂଲ୍ୟ = static_cast<DataCacheElement*>(uhash_get(htable, path));
    ଯଦି (old_ମୂଲ୍ୟ != nullptr) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    ଅନ୍ୟଥା {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(nullptr);

#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (ଶୂନ୍ୟ*) newElement->item, u_errorName(subErr), (ଶୂନ୍ୟ*) newElement->item->vFuncs);
#endif

    ଯଦି (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        ଫେରନ୍ତୁ old_ମୂଲ୍ୟ ? old_ମୂଲ୍ୟ->item : nullptr;
    }

    ଫେରନ୍ତୁ newElement->item;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

ଶ୍ରେଣୀ UDataPathIterator
{
ସର୍ବସାଧାରଣ:
    UDataPathIterator(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *pkg,
                      ଅଚଳ ଅକ୍ଷର *item, ଅଚଳ ଅକ୍ଷର *suffix, UBool doCheckLastFour,
                      UErrorCode *pErrorCode);
    ଅଚଳ ଅକ୍ଷର *next(UErrorCode *pErrorCode);

ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ ଅକ୍ଷର *path;                              /* working path (u_icudata_Dir) */
    ଅଚଳ ଅକ୍ଷର *nextPath;                          /* path following this one */
    ଅଚଳ ଅକ୍ଷର *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  itemPath;                          /* path passed in with item name */
    CharString  pathBuffer;                        /* output path for this it'ion */
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
UDataPathIterator::UDataPathIterator(ଅଚଳ ଅକ୍ଷର *inPath, ଅଚଳ ଅକ୍ଷର *pkg,
                                     ଅଚଳ ଅକ୍ଷର *item, ଅଚଳ ଅକ୍ଷର *inSuffix, UBool doCheckLastFour,
                                     UErrorCode *pErrorCode)
{
#ifdef UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", inSuffix, inPath);
#endif
    /** Path **/
    ଯଦି(inPath == nullptr) {
        path = u_getDataDirectory();
    } ଅନ୍ୟଥା {
        path = inPath;
    }

    /** Package **/
    ଯଦି(pkg != nullptr) {
      packageStub.append(U_FILE_SEP_CHAR, *pErrorCode).append(pkg, *pErrorCode);
#ifdef UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.data(), packageStub.length());
#endif
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    ଯଦି(basename == item) {
        nextPath = path;
    } ଅନ୍ୟଥା {
        itemPath.append(item, static_cast<int32_t>(basename - item), *pErrorCode);
        nextPath = itemPath.data();
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", inSuffix, (ଶୂନ୍ୟ*) inSuffix);
#endif

    /** Suffix  **/
    ଯଦି(inSuffix != nullptr) {
        suffix = inSuffix;
    } ଅନ୍ୟଥା {
        suffix = "";
    }

    checkLastFour = doCheckLastFour;

    /* pathBuffer will hold the output path strings returned by this iterator */

#ifdef UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            item,
            path,
            basename,
            suffix.data(),
            itemPath.data(),
            nextPath,
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
ଅଚଳ ଅକ୍ଷର *UDataPathIterator::next(UErrorCode *pErrorCode)
{
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଅଚଳ ଅକ୍ଷର *currentPath = nullptr;
    int32_t     pathLen = 0;
    ଅଚଳ ଅକ୍ଷର *pathBasename;

    କର
    {
        ଯଦି( nextPath == nullptr ) {
            ଭାଙ୍ଗ;
        }
        currentPath = nextPath;

        ଯଦି(nextPath == itemPath.data()) { /* we were processing item's path. */
            nextPath = path; /* start with regular path next tm. */
            pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
        } ଅନ୍ୟଥା {
            /* fix up next for next time */
            nextPath = uprv_strchr(currentPath, U_PATH_SEP_CHAR);
            ଯଦି(nextPath == nullptr) {
                /* segment: entire path */
                pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
            } ଅନ୍ୟଥା {
                /* segment: until next segment */
                pathLen = static_cast<int32_t>(nextPath - currentPath);
                /* skip divider */
                nextPath ++;
            }
        }

        ଯଦି(pathLen == 0) {
            ଜାରି;
        }

#ifdef UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", currentPath);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            ପାଇଁ(qqq=0;qqq<pathLen;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#endif
        pathBuffer.clear().append(currentPath, pathLen, *pErrorCode);

        /* check for .dat files */
        pathBasename = findBasename(pathBuffer.data());

        ଯଦି(checkLastFour && 
           (pathLen>=4) &&
           uprv_strncmp(pathBuffer.data() +(pathLen-4), suffix.data(), 4)==0 && /* suffix matches */
           uprv_strncmp(findBasename(pathBuffer.data()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(pathBasename)==(basenameLen+4)) { /* base+suffix = full len */

#ifdef UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.data(), pathBuffer.data());
#endif
            /* do nothing */
        }
        ଅନ୍ୟଥା 
        {       /* regular dir path */
            ଯଦି(pathBuffer[pathLen-1] != U_FILE_SEP_CHAR) {
                ଯଦି((pathLen>=4) &&
                   uprv_strncmp(pathBuffer.data()+(pathLen-4), ".dat", 4) == 0)
                {
#ifdef UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", pathBuffer.data());
#endif
                    ଜାରି;
                }

                /* Check if it is a directory with the same name as our package */
                ଯଦି(!packageStub.isEmpty() &&
                   (pathLen > packageStub.length()) &&
                   !uprv_strcmp(pathBuffer.data() + pathLen - packageStub.length(), packageStub.data())) {
#ifdef UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.data(), basename, basenameLen);
#endif
                  pathBuffer.truncate(pathLen - packageStub.length());
                }
                pathBuffer.append(U_FILE_SEP_CHAR, *pErrorCode);
            }

            /* + basename */
            pathBuffer.append(packageStub.data()+1, packageStub.length()-1, *pErrorCode);

            ଯଦି (!suffix.empty())  /* tack on suffix */
            {
                ଯଦି (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    pathBuffer.ensureEndsWithFileSeparator(*pErrorCode);
                }
                pathBuffer.append(suffix, *pErrorCode);
            }
        }

#ifdef UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", pathBuffer.data());
#endif

        ଫେରନ୍ତୁ pathBuffer.data();

    } ଯେତେବେଳେ(path);

    /* fell way off the end */
    ଫେରନ୍ତୁ nullptr;
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
#ଯଦି !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
ବାହ୍ୟ "C" ଅଚଳ DataHeader U_DATA_API U_ICUDATA_ENTRY_POINT;
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
ସ୍ଥିର UDataMemory *
openCommonData(ଅଚଳ ଅକ୍ଷର *path,          /*  Path from OpenChoice?          */
               int32_t commonDataIndex,   /*  ICU Data (index >= 0) if path == nullptr */
               UErrorCode *pErrorCode)
{
    UDataMemory tData;
    ଅଚଳ ଅକ୍ଷର *pathBuffer;
    ଅଚଳ ଅକ୍ଷର *inBasename;

    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }

    UDataMemory_init(&tData);

    /* ??????? TODO revisit this */ 
    ଯଦି (commonDataIndex >= 0) {
        /* "mini-cache" for common ICU data */
        ଯଦି(commonDataIndex >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            ଫେରନ୍ତୁ nullptr;
        }
        {
            Mutex lock;
            ଯଦି(gCommonICUDataArray[commonDataIndex] != nullptr) {
                ଫେରନ୍ତୁ gCommonICUDataArray[commonDataIndex];
            }
#ଯଦି !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            ପାଇଁ(i = 0; i < commonDataIndex; ++i) {
                ଯଦି(gCommonICUDataArray[i]->pHeader == &U_ICUDATA_ENTRY_POINT) {
                    /* The linked-in data is already in the list. */
                    ଫେରନ୍ତୁ nullptr;
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
#ଯଦି !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        setCommonICUDataPointer(&U_ICUDATA_ENTRY_POINT, ମିଥ୍ୟା, pErrorCode);
        {
            Mutex lock;
            ଫେରନ୍ତୁ gCommonICUDataArray[commonDataIndex];
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

    ଯଦି(*inBasename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", path);
#endif
        ଯଦି (U_SUCCESS(*pErrorCode)) {
            *pErrorCode=U_FILE_ACCESS_ERROR;
        }
        ଫେରନ୍ତୁ nullptr;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UDataMemory  *dataToReturn = udata_findCachedData(inBasename, *pErrorCode);
    ଯଦି (dataToReturn != nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ dataToReturn;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UDataPathIterator iter(u_getDataDirectory(), inBasename, path, ".dat", ସତ୍ୟ, pErrorCode);

    ଯେତେବେଳେ ((UDataMemory_isLoaded(&tData)==ମିଥ୍ୟା) && (pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", pathBuffer);
#endif
        uprv_mapFile(&tData, pathBuffer, pErrorCode);
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&tData)?"LOADED":"not loaded");
#endif
    }
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (!UDataMemory_isLoaded(&tData)) {
        /* no common data */
        *pErrorCode=U_FILE_ACCESS_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    /* we have mapped a file, check its header */
    udata_checkCommonData(&tData, pErrorCode);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    ଫେରନ୍ତୁ udata_cacheDataItem(inBasename, &tData, pErrorCode);
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
ସ୍ଥିର UBool extendICUData(UErrorCode *pErr)
{
    UDataMemory   *pData;
    UDataMemory   copyPData;
    UBool         didUpdate = ମିଥ୍ୟା;

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
#ଯଦି MAP_IMPLEMENTATION==MAP_STDIO
    ସ୍ଥିର UMutex extendICUDataMutex;
    umtx_lock(&extendICUDataMutex);
#endif
    ଯଦି(!umtx_loadAcquire(gHaveTriedToLoadCommonData)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        pData = openCommonData(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UDataMemory_init(&copyPData);
       ଯଦି(pData != nullptr) {
          UDatamemory_assign(&copyPData, pData);
          copyPData.map = nullptr;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = nullptr; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              setCommonICUData(&copyPData,/*  The new common data.                                */
                       ମିଥ୍ୟା,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(gHaveTriedToLoadCommonData, 1);
    }

    didUpdate = findCommonICUDataByName(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#ଯଦି MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#endif
    ଫେରନ୍ତୁ didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI ଶୂନ୍ୟ U_EXPORT2
udata_setCommonData(ଅଚଳ ଶୂନ୍ୟ *data, UErrorCode *pErrorCode) {
    UDataMemory dataMemory;

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି(data==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    /* set the data pointer and test for validity */
    UDataMemory_init(&dataMemory);
    UDataMemory_setData(&dataMemory, data);
    udata_checkCommonData(&dataMemory, pErrorCode);
    ଯଦି (U_FAILURE(*pErrorCode)) {ଫେରନ୍ତୁ;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    setCommonICUData(&dataMemory, ସତ୍ୟ, pErrorCode);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI ଶୂନ୍ୟ U_EXPORT2
udata_setAppData(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଶୂନ୍ୟ *data, UErrorCode *err)
{
    UDataMemory     udm;

    ଯଦି(err==nullptr || U_FAILURE(*err)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(data==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    UDataMemory_init(&udm);
    UDataMemory_setData(&udm, data);
    udata_checkCommonData(&udm, err);
    udata_cacheDataItem(path, &udm, err);
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
ସ୍ଥିର UDataMemory *
checkDataItem
(
 ଅଚଳ DataHeader         *pHeader,         /* The data item to be checked.                */
 UDataMemoryIsAcceptable  *isAcceptable,    /* App's call-back function                    */
 ଶୂନ୍ୟ                     *context,         /*   pass-thru param for above.                */
 ଅଚଳ ଅକ୍ଷର               *type,            /*   pass-thru param for above.                */
 ଅଚଳ ଅକ୍ଷର               *name,            /*   pass-thru param for above.                */
 UErrorCode               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UErrorCode               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UDataMemory  *rDataMem = nullptr;          /* the new UDataMemory, to be returned.        */

    ଯଦି (U_FAILURE(*fatalErr)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି(pHeader->dataHeader.magic1==0xda &&
        pHeader->dataHeader.magic2==0x27 &&
        (isAcceptable==nullptr || isAcceptable(context, type, name, &pHeader->info))
    ) {
        rDataMem=UDataMemory_createNewInstance(fatalErr);
        ଯଦି (U_FAILURE(*fatalErr)) {
            ଫେରନ୍ତୁ nullptr;
        }
        rDataMem->pHeader = pHeader;
    } ଅନ୍ୟଥା {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    ଫେରନ୍ତୁ rDataMem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ସ୍ଥିର UDataMemory *doLoadFromIndividualFiles(ଅଚଳ ଅକ୍ଷର *pkgName, 
        ଅଚଳ ଅକ୍ଷର *dataPath, ଅଚଳ ଅକ୍ଷର *tocEntryPathSuffix,
            /* following arguments are the same as doOpenChoice itself */
            ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *type, ଅଚଳ ଅକ୍ଷର *name,
             UDataMemoryIsAcceptable *isAcceptable, ଶୂନ୍ୟ *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    ଅଚଳ ଅକ୍ଷର         *pathBuffer;
    UDataMemory         dataMemory;
    UDataMemory *pEntryData;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UDataPathIterator iter(dataPath, pkgName, path, tocEntryPathSuffix, ମିଥ୍ୟା, pErrorCode);

    ଯେତେବେଳେ ((pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", pathBuffer);
#endif
        ଯଦି (uprv_mapFile(&dataMemory, pathBuffer, pErrorCode))
        {
            pEntryData = checkDataItem(dataMemory.pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
            ଯଦି (pEntryData != nullptr) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                pEntryData->mapAddr = dataMemory.mapAddr;
                pEntryData->map     = dataMemory.map;
                pEntryData->length  = dataMemory.length;

#ifdef UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", pathBuffer);
#endif
                ଫେରନ୍ତୁ pEntryData;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&dataMemory);

            /* If we had a nasty error, bail out completely.  */
            ଯଦି (U_FAILURE(*pErrorCode)) {
                ଫେରନ୍ତୁ nullptr;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *subErrorCode=U_INVALID_FORMAT_ERROR;
        }
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&dataMemory)?"LOADED":"not loaded");
#endif
    }
    ଫେରନ୍ତୁ nullptr;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
ସ୍ଥିର UDataMemory *doLoadFromCommonData(UBool isICUData, ଅଚଳ ଅକ୍ଷର * /*pkgName*/, 
        ଅଚଳ ଅକ୍ଷର * /*dataPath*/, ଅଚଳ ଅକ୍ଷର * /*tocEntryPathSuffix*/, ଅଚଳ ଅକ୍ଷର *tocEntryName,
            /* following arguments are the same as doOpenChoice itself */
            ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *type, ଅଚଳ ଅକ୍ଷର *name,
             UDataMemoryIsAcceptable *isAcceptable, ଶୂନ୍ୟ *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    UDataMemory        *pEntryData;
    ଅଚଳ DataHeader   *pHeader;
    UDataMemory        *pCommonData;
    int32_t            commonDataIndex;
    UBool              checkedExtendedICUData = ମିଥ୍ୟା;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    ପାଇଁ (commonDataIndex = isICUData ? 0 : -1;;) {
        pCommonData=openCommonData(path, commonDataIndex, subErrorCode); /** search for pkg **/

        ଯଦି(U_SUCCESS(*subErrorCode) && pCommonData!=nullptr) {
            int32_t length;

            /* look up the data piece in the common data */
            pHeader=pCommonData->vFuncs->Lookup(pCommonData, tocEntryName, &length, subErrorCode);
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", tocEntryName, (ଶୂନ୍ୟ*) pHeader, u_errorName(*subErrorCode));
#endif

            ଯଦି(pHeader!=nullptr) {
                pEntryData = checkDataItem(pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
#ifdef UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (ଶୂନ୍ୟ*) pEntryData);
#endif
                ଯଦି (U_FAILURE(*pErrorCode)) {
                    ଫେରନ୍ତୁ nullptr;
                }
                ଯଦି (pEntryData != nullptr) {
                    pEntryData->length = length;
                    ଫେରନ୍ତୁ pEntryData;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        ଯଦି (*subErrorCode == U_MEMORY_ALLOCATION_ERROR) {
            *pErrorCode = *subErrorCode;
            ଫେରନ୍ତୁ nullptr;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        ଯଦି (!isICUData) {
            ଫେରନ୍ତୁ nullptr;
        } ଅନ୍ୟଥା ଯଦି (pCommonData != nullptr) {
            ++commonDataIndex;  /* try the next data package */
        } ଅନ୍ୟଥା ଯଦି ((!checkedExtendedICUData) && extendICUData(subErrorCode)) {
            checkedExtendedICUData = ସତ୍ୟ;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ nullptr;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
ସ୍ଥିର UBool isTimeZoneFile(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *type) {
    ଫେରନ୍ତୁ ((uprv_strcmp(type, "res") == 0) &&
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
ସ୍ଥିର UDataMemory *
doOpenChoice(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *type, ଅଚଳ ଅକ୍ଷର *name,
             UDataMemoryIsAcceptable *isAcceptable, ଶୂନ୍ୟ *context,
             UErrorCode *pErrorCode)
{
    UDataMemory         *retVal = nullptr;

    ଅଚଳ ଅକ୍ଷର         *dataPath;

    int32_t             tocEntrySuffixIndex;
    ଅଚଳ ଅକ୍ଷର         *tocEntryPathSuffix;
    UErrorCode          subErrorCode=U_ZERO_ERROR;
    ଅଚଳ ଅକ୍ଷର         *ବୃକ୍ଷ_Char;

    UBool               isICUData = ମିଥ୍ୟା;


    FileTracer::traceOpen(path, type, name);


    /* Is this path ICU data? */
    ଯଦି(path == nullptr ||
       !strcmp(path, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(path, U_ICUDATA_NAME U_ବୃକ୍ଷ_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_ବୃକ୍ଷ_SEPARATOR_STRING)) ||  
       !uprv_strncmp(path, U_ICUDATA_ALIAS U_ବୃକ୍ଷ_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_ବୃକ୍ଷ_SEPARATOR_STRING))) {
      isICUData = ସତ୍ୟ;
    }

#ଯଦି (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString altSepPath;
    ଯଦି(path) {
        ଯଦି(uprv_strchr(path,U_FILE_ALT_SEP_CHAR) != nullptr) {
            altSepPath.append(path, *pErrorCode);
            ଅକ୍ଷର *p;
            ଯେତେବେଳେ ((p = uprv_strchr(altSepPath.data(), U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
#ଯଦି defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", path, altSepPath.data());
#endif
            path = altSepPath.data();
        }
    }
#endif

    CharString tocEntryName; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString tocEntryPath; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString ବୃକ୍ଷ_Name;

    /* ======= Set up strings */
    ଯଦି(path==nullptr) {
        pkgName.append(U_ICUDATA_NAME, *pErrorCode);
    } ଅନ୍ୟଥା {
        ଅଚଳ ଅକ୍ଷର *pkg;
        ଅଚଳ ଅକ୍ଷର *first;
        pkg = uprv_strrchr(path, U_FILE_SEP_CHAR);
        first = uprv_strchr(path, U_FILE_SEP_CHAR);
        ଯଦି(uprv_pathIsAbsolute(path) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            ଯଦି(pkg) {
                pkgName.append(pkg+1, *pErrorCode);
            } ଅନ୍ୟଥା {
                pkgName.append(path, *pErrorCode);
            }
        } ଅନ୍ୟଥା {
            ବୃକ୍ଷ_Char = uprv_strchr(path, U_ବୃକ୍ଷ_SEPARATOR);
            ଯଦି(ବୃକ୍ଷ_Char) { 
                ବୃକ୍ଷ_Name.append(ବୃକ୍ଷ_Char+1, *pErrorCode); /* following '-' */
                ଯଦି(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } ଅନ୍ୟଥା {
                    pkgName.append(path, static_cast<int32_t>(ବୃକ୍ଷ_Char - path), *pErrorCode);
                    ଯଦି (first == nullptr) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        path = pkgName.data();
                    }
                }
            } ଅନ୍ୟଥା {
                ଯଦି(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } ଅନ୍ୟଥା {
                    pkgName.append(path, *pErrorCode);
                }
            }
        }
    }

#ifdef UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.data(), ବୃକ୍ଷ_Name.data());
#endif

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    tocEntryName.append(pkgName, *pErrorCode);
    tocEntryPath.append(pkgName, *pErrorCode);
    tocEntrySuffixIndex = tocEntryName.length();

    ଯଦି(!ବୃକ୍ଷ_Name.isEmpty()) {
        tocEntryName.append(U_ବୃକ୍ଷ_ENTRY_SEP_CHAR, *pErrorCode).append(ବୃକ୍ଷ_Name, *pErrorCode);
        tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(ବୃକ୍ଷ_Name, *pErrorCode);
    }

    tocEntryName.append(U_ବୃକ୍ଷ_ENTRY_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    ଯଦି(type!=nullptr && *type!=0) {
        tocEntryName.append(".", *pErrorCode).append(type, *pErrorCode);
        tocEntryPath.append(".", *pErrorCode).append(type, *pErrorCode);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    tocEntryPathSuffix = tocEntryPath.data() + tocEntrySuffixIndex + 1; /* suffix starts here */

#ifdef UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", tocEntryName.data());
    fprintf(stderr, " tocEntryPath = %s\n", tocEntryPath.data());
#endif

#ଯଦି !defined(ICU_DATA_DIR_WINDOWS)
    ଯଦି(path == nullptr) {
        path = COMMON_DATA_NAME; /* "icudt26e" */
    }
#ଅନ୍ୟଥା
    // When using the Windows system data, we expects only a single data file.
    path = COMMON_DATA_NAME; /* "icudt26e" */
#endif

    /************************ Begin loop looking for ind. files ***************/
#ifdef UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", packageNameFromPath(path));
#endif

    /* End of dealing with a null basename */
    dataPath = u_getDataDirectory();

    /****    Time zone individual files override  */
    ଯଦି (isICUData && type != nullptr && isTimeZoneFile(name, type)) {
        ଅଚଳ ଅକ୍ଷର *tzFilesDir = u_getTimeZoneFilesDirectory(pErrorCode);
        ଯଦି (tzFilesDir[0] != 0) {
#ifdef UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#endif
            retVal = doLoadFromIndividualFiles(/* pkgName.data() */ "", tzFilesDir, tocEntryPathSuffix,
                            /* path */ "", type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            ଯଦି((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                ଫେରନ୍ତୁ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    ଯଦି(gDataFileAccess == UDATA_PACKAGES_FIRST) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#endif
        /* #2 */
        retVal = doLoadFromCommonData(isICUData, 
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        ଯଦି((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ଫେରନ୍ତୁ retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    ଯଦି((gDataFileAccess==UDATA_PACKAGES_FIRST) ||
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#endif
        /* Check to make sure that there is a dataPath to iterate over */
        ଯଦି ((dataPath && *dataPath) || !isICUData) {
            retVal = doLoadFromIndividualFiles(pkgName.data(), dataPath, tocEntryPathSuffix,
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            ଯଦି((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                ଫେରନ୍ତୁ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    ଯଦି((gDataFileAccess==UDATA_ONLY_PACKAGES) || 
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        ଯଦି((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ଫେରନ୍ତୁ retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    ଯଦି(gDataFileAccess==UDATA_NO_FILES) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), "", tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        ଯଦି((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ଫେରନ୍ତୁ retVal;
        }
    }

    /* data not found */
    ଯଦି(U_SUCCESS(*pErrorCode)) {
        ଯଦି(U_SUCCESS(subErrorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } ଅନ୍ୟଥା {
            /* entry point not found or rejected */
            *pErrorCode=subErrorCode;
        }
    }
    ଫେରନ୍ତୁ retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *type, ଅଚଳ ଅକ୍ଷର *name,
           UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
    fflush(stderr);
#endif

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା ଯଦି(name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ doOpenChoice(path, type, name, nullptr, nullptr, pErrorCode);
    }
}



U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *type, ଅଚଳ ଅକ୍ଷର *name,
                 UDataMemoryIsAcceptable *isAcceptable, ଶୂନ୍ୟ *context,
                 UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
#endif

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା ଯଦି(name==nullptr || *name==0 || isAcceptable==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}



U_CAPI ଶୂନ୍ୟ U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    ଯଦି(pInfo!=nullptr) {
        ଯଦି(pData!=nullptr && pData->pHeader!=nullptr) {
            ଅଚଳ UDataInfo *info=&pData->pHeader->info;
            uint16_t dataInfoSize=udata_getInfoSize(info);
            ଯଦି(pInfo->size>dataInfoSize) {
                pInfo->size=dataInfoSize;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (ଅଚଳ uint16_t *)info+1, pInfo->size-2);
            ଯଦି(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reservedWord;
                pInfo->reservedWord=(uint16_t)((x<<8)|(x>>8));
            }
        } ଅନ୍ୟଥା {
            pInfo->size=0;
        }
    }
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2 udata_setFileAccess(UDataFileAccess access, UErrorCode * /*status*/)
{
    // Note: this function is documented as not thread safe.
    gDataFileAccess = access;
}
