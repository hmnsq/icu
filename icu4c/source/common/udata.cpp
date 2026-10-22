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

#அந்தர்பாவ "unicode/utypes.h"  /* U_PLATFORM etc. */

#ifdef __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#endif

#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/uversion.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "restrace.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "ucmndata.h"
#அந்தர்பாவ "udatamem.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "umapfile.h"
#அந்தர்பாவ "umutex.h"

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

#எனில் defined(UDATA_DEBUG)
#   include <stdio.h>
#endif

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
நிலையான UDataMemory *udata_findCachedData(மாறிலி எழுத்து *path, UErrorCode &err);

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
நிலையான UDataMemory *gCommonICUDataArray[10] = { nullptr };   // Access protected by icu global mutex.

நிலையான u_atomic_int32_t gHaveTriedToLoadCommonData {0};  //  See extendICUData().

நிலையான UHashtable  *gCommonDataCache = nullptr;  /* Global hash table of opened ICU data files.  */
நிலையான icu::UInitOnce gCommonDataCacheInitOnce {};

#எனில் !defined(ICU_DATA_DIR_WINDOWS)
நிலையான UDataFileAccess  gDataFileAccess = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#இல்லையெனில்
// If we are using the Windows data directory, then look in one spot only.
நிலையான UDataFileAccess  gDataFileAccess = UDATA_NO_FILES;
#endif

நிலையான UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    எனில் (gCommonDataCache) {             /* Delete the cache of user data mappings.  */
        uhash_close(gCommonDataCache);  /*   Table owns the contents, and will delete them. */
        gCommonDataCache = nullptr;        /*   Cleanup is not thread safe.                */
    }
    gCommonDataCacheInitOnce.reset();

    க்கு (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != nullptr; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = nullptr;
    }
    gHaveTriedToLoadCommonData = 0;

    திரும்பு உண்மை;                   /* Everything was cleaned up */
}

நிலையான UBool U_CALLCONV
findCommonICUDataByName(மாறிலி எழுத்து *inBasename, UErrorCode &err)
{
    UBool found = பொய்;
    int32_t i;

    UDataMemory  *pData = udata_findCachedData(inBasename, err);
    எனில் (U_FAILURE(err) || pData == nullptr)
        திரும்பு பொய்;

    {
        Mutex lock;
        க்கு (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            எனில் ((gCommonICUDataArray[i] != nullptr) && (gCommonICUDataArray[i]->pHeader == pData->pHeader)) {
                /* The data pointer is already in the array. */
                found = உண்மை;
                நிறுத்து;
            }
        }
    }
    திரும்பு found;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
நிலையான UBool
setCommonICUData(UDataMemory *pData,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UErrorCode *pErr)
{
    UDataMemory  *newCommonData = UDataMemory_createNewInstance(pErr);
    int32_t i;
    UBool didUpdate = பொய்;
    எனில் (U_FAILURE(*pErr)) {
        திரும்பு பொய்;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_assign(newCommonData, pData);
    umtx_lock(nullptr);
    க்கு (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        எனில் (gCommonICUDataArray[i] == nullptr) {
            gCommonICUDataArray[i] = newCommonData;
            didUpdate = உண்மை;
            நிறுத்து;
        } இல்லையெனில் எனில் (gCommonICUDataArray[i]->pHeader == pData->pHeader) {
            /* The same data pointer is already in the array. */
            நிறுத்து;
        }
    }
    umtx_unlock(nullptr);

    எனில் (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    எனில் (didUpdate) {
        ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } இல்லையெனில் {
        uprv_free(newCommonData);
    }
    திரும்பு didUpdate;
}

#எனில் !defined(ICU_DATA_DIR_WINDOWS)

நிலையான UBool
setCommonICUDataPointer(மாறிலி வெறுமை *pData, UBool /*warn*/, UErrorCode *pErrorCode) {
    UDataMemory tData;
    UDataMemory_init(&tData);
    UDataMemory_setData(&tData, pData);
    udata_checkCommonData(&tData, pErrorCode);
    திரும்பு setCommonICUData(&tData, பொய், pErrorCode);
}

#endif

நிலையான மாறிலி எழுத்து *
findBasename(மாறிலி எழுத்து *path) {
    மாறிலி எழுத்து *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    எனில்(basename==nullptr) {
        திரும்பு path;
    } இல்லையெனில் {
        திரும்பு basename+1;
    }
}

#ifdef UDATA_DEBUG
நிலையான மாறிலி எழுத்து *
packageNameFromPath(மாறிலி எழுத்து *path)
{
    எனில்((path == nullptr) || (*path == 0)) {
        திரும்பு U_ICUDATA_NAME;
    }

    path = findBasename(path);

    எனில்((path == nullptr) || (*path == 0)) {
        திரும்பு U_ICUDATA_NAME;
    }

    திரும்பு path;
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

வகைவரை கட்டமைப்பு DataCacheElement {
    எழுத்து          *name;
    UDataMemory   *item;
} DataCacheElement;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
நிலையான வெறுமை U_CALLCONV DataCacheElement_deleter(வெறுமை *pDCEl) {
    DataCacheElement* p = static_cast<DataCacheElement*>(pDCEl);
    udata_close(p->item);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

நிலையான வெறுமை U_CALLCONV udata_initHashTable(UErrorCode &err) {
    U_ASSERT(gCommonDataCache == nullptr);
    gCommonDataCache = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &err);
    எனில் (U_FAILURE(err)) {
       திரும்பு;
    }
    U_ASSERT(gCommonDataCache != nullptr);
    uhash_set_மதிப்பு_Deleter(gCommonDataCache, DataCacheElement_deleter);
    ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
நிலையான UHashtable *udata_getHashTable(UErrorCode &err) {
    umtx_initOnce(gCommonDataCacheInitOnce, &udata_initHashTable, err);
    திரும்பு gCommonDataCache;
}



நிலையான UDataMemory *udata_findCachedData(மாறிலி எழுத்து *path, UErrorCode &err)
{
    UHashtable        *htable;
    UDataMemory       *retVal = nullptr;
    DataCacheElement  *el;
    மாறிலி எழுத்து        *baseName;

    htable = udata_getHashTable(err);
    எனில் (U_FAILURE(err)) {
        திரும்பு nullptr;
    }

    baseName = findBasename(path);   /* Cache remembers only the base name, not the full path. */
    umtx_lock(nullptr);
    el = static_cast<DataCacheElement*>(uhash_get(htable, baseName));
    umtx_unlock(nullptr);
    எனில் (el != nullptr) {
        retVal = el->item;
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", baseName, (வெறுமை*) retVal);
#endif
    திரும்பு retVal;
}


நிலையான UDataMemory *udata_cacheDataItem(மாறிலி எழுத்து *path, UDataMemory *item, UErrorCode *pErr) {
    DataCacheElement *newElement;
    மாறிலி எழுத்து       *baseName;
    int32_t           nameLen;
    UHashtable       *htable;
    DataCacheElement *old_மதிப்பு = nullptr;
    UErrorCode        subErr = U_ZERO_ERROR;

    htable = udata_getHashTable(*pErr);
    எனில் (U_FAILURE(*pErr)) {
        திரும்பு nullptr;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<DataCacheElement*>(uprv_malloc(அளவு(DataCacheElement)));
    எனில் (newElement == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    newElement->item = UDataMemory_createNewInstance(pErr);
    எனில் (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        திரும்பு nullptr;
    }
    UDatamemory_assign(newElement->item, item);

    baseName = findBasename(path);
    nameLen = static_cast<int32_t>(uprv_strlen(baseName));
    newElement->name = static_cast<எழுத்து*>(uprv_malloc(nameLen + 1));
    எனில் (newElement->name == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        திரும்பு nullptr;
    }
    uprv_strcpy(newElement->name, baseName);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(nullptr);
    old_மதிப்பு = static_cast<DataCacheElement*>(uhash_get(htable, path));
    எனில் (old_மதிப்பு != nullptr) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    இல்லையெனில் {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(nullptr);

#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (வெறுமை*) newElement->item, u_errorName(subErr), (வெறுமை*) newElement->item->vFuncs);
#endif

    எனில் (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        திரும்பு old_மதிப்பு ? old_மதிப்பு->item : nullptr;
    }

    திரும்பு newElement->item;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

வகுப்பு UDataPathIterator
{
பொது:
    UDataPathIterator(மாறிலி எழுத்து *path, மாறிலி எழுத்து *pkg,
                      மாறிலி எழுத்து *item, மாறிலி எழுத்து *suffix, UBool doCheckLastFour,
                      UErrorCode *pErrorCode);
    மாறிலி எழுத்து *next(UErrorCode *pErrorCode);

தனிப்பட்ட:
    மாறிலி எழுத்து *path;                              /* working path (u_icudata_Dir) */
    மாறிலி எழுத்து *nextPath;                          /* path following this one */
    மாறிலி எழுத்து *basename;                          /* item's basename (icudt22e_mt.res)*/

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
UDataPathIterator::UDataPathIterator(மாறிலி எழுத்து *inPath, மாறிலி எழுத்து *pkg,
                                     மாறிலி எழுத்து *item, மாறிலி எழுத்து *inSuffix, UBool doCheckLastFour,
                                     UErrorCode *pErrorCode)
{
#ifdef UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", inSuffix, inPath);
#endif
    /** Path **/
    எனில்(inPath == nullptr) {
        path = u_getDataDirectory();
    } இல்லையெனில் {
        path = inPath;
    }

    /** Package **/
    எனில்(pkg != nullptr) {
      packageStub.append(U_FILE_SEP_CHAR, *pErrorCode).append(pkg, *pErrorCode);
#ifdef UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.data(), packageStub.length());
#endif
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    எனில்(basename == item) {
        nextPath = path;
    } இல்லையெனில் {
        itemPath.append(item, static_cast<int32_t>(basename - item), *pErrorCode);
        nextPath = itemPath.data();
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", inSuffix, (வெறுமை*) inSuffix);
#endif

    /** Suffix  **/
    எனில்(inSuffix != nullptr) {
        suffix = inSuffix;
    } இல்லையெனில் {
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
மாறிலி எழுத்து *UDataPathIterator::next(UErrorCode *pErrorCode)
{
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }

    மாறிலி எழுத்து *currentPath = nullptr;
    int32_t     pathLen = 0;
    மாறிலி எழுத்து *pathBasename;

    செய்
    {
        எனில்( nextPath == nullptr ) {
            நிறுத்து;
        }
        currentPath = nextPath;

        எனில்(nextPath == itemPath.data()) { /* we were processing item's path. */
            nextPath = path; /* start with regular path next tm. */
            pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
        } இல்லையெனில் {
            /* fix up next for next time */
            nextPath = uprv_strchr(currentPath, U_PATH_SEP_CHAR);
            எனில்(nextPath == nullptr) {
                /* segment: entire path */
                pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
            } இல்லையெனில் {
                /* segment: until next segment */
                pathLen = static_cast<int32_t>(nextPath - currentPath);
                /* skip divider */
                nextPath ++;
            }
        }

        எனில்(pathLen == 0) {
            தொடர்;
        }

#ifdef UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", currentPath);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            க்கு(qqq=0;qqq<pathLen;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#endif
        pathBuffer.clear().append(currentPath, pathLen, *pErrorCode);

        /* check for .dat files */
        pathBasename = findBasename(pathBuffer.data());

        எனில்(checkLastFour && 
           (pathLen>=4) &&
           uprv_strncmp(pathBuffer.data() +(pathLen-4), suffix.data(), 4)==0 && /* suffix matches */
           uprv_strncmp(findBasename(pathBuffer.data()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(pathBasename)==(basenameLen+4)) { /* base+suffix = full len */

#ifdef UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.data(), pathBuffer.data());
#endif
            /* do nothing */
        }
        இல்லையெனில் 
        {       /* regular dir path */
            எனில்(pathBuffer[pathLen-1] != U_FILE_SEP_CHAR) {
                எனில்((pathLen>=4) &&
                   uprv_strncmp(pathBuffer.data()+(pathLen-4), ".dat", 4) == 0)
                {
#ifdef UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", pathBuffer.data());
#endif
                    தொடர்;
                }

                /* Check if it is a directory with the same name as our package */
                எனில்(!packageStub.isEmpty() &&
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

            எனில் (!suffix.empty())  /* tack on suffix */
            {
                எனில் (suffix.length() > 4) {
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

        திரும்பு pathBuffer.data();

    } போது(path);

    /* fell way off the end */
    திரும்பு nullptr;
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
#எனில் !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
வெளி "C" மாறிலி DataHeader U_DATA_API U_ICUDATA_ENTRY_POINT;
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
நிலையான UDataMemory *
openCommonData(மாறிலி எழுத்து *path,          /*  Path from OpenChoice?          */
               int32_t commonDataIndex,   /*  ICU Data (index >= 0) if path == nullptr */
               UErrorCode *pErrorCode)
{
    UDataMemory tData;
    மாறிலி எழுத்து *pathBuffer;
    மாறிலி எழுத்து *inBasename;

    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }

    UDataMemory_init(&tData);

    /* ??????? TODO revisit this */ 
    எனில் (commonDataIndex >= 0) {
        /* "mini-cache" for common ICU data */
        எனில்(commonDataIndex >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            திரும்பு nullptr;
        }
        {
            Mutex lock;
            எனில்(gCommonICUDataArray[commonDataIndex] != nullptr) {
                திரும்பு gCommonICUDataArray[commonDataIndex];
            }
#எனில் !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            க்கு(i = 0; i < commonDataIndex; ++i) {
                எனில்(gCommonICUDataArray[i]->pHeader == &U_ICUDATA_ENTRY_POINT) {
                    /* The linked-in data is already in the list. */
                    திரும்பு nullptr;
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
#எனில் !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        setCommonICUDataPointer(&U_ICUDATA_ENTRY_POINT, பொய், pErrorCode);
        {
            Mutex lock;
            திரும்பு gCommonICUDataArray[commonDataIndex];
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

    எனில்(*inBasename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", path);
#endif
        எனில் (U_SUCCESS(*pErrorCode)) {
            *pErrorCode=U_FILE_ACCESS_ERROR;
        }
        திரும்பு nullptr;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UDataMemory  *dataToReturn = udata_findCachedData(inBasename, *pErrorCode);
    எனில் (dataToReturn != nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு dataToReturn;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UDataPathIterator iter(u_getDataDirectory(), inBasename, path, ".dat", உண்மை, pErrorCode);

    போது ((UDataMemory_isLoaded(&tData)==பொய்) && (pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", pathBuffer);
#endif
        uprv_mapFile(&tData, pathBuffer, pErrorCode);
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&tData)?"LOADED":"not loaded");
#endif
    }
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }

    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில் (!UDataMemory_isLoaded(&tData)) {
        /* no common data */
        *pErrorCode=U_FILE_ACCESS_ERROR;
        திரும்பு nullptr;
    }

    /* we have mapped a file, check its header */
    udata_checkCommonData(&tData, pErrorCode);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    திரும்பு udata_cacheDataItem(inBasename, &tData, pErrorCode);
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
நிலையான UBool extendICUData(UErrorCode *pErr)
{
    UDataMemory   *pData;
    UDataMemory   copyPData;
    UBool         didUpdate = பொய்;

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
#எனில் MAP_IMPLEMENTATION==MAP_STDIO
    நிலையான UMutex extendICUDataMutex;
    umtx_lock(&extendICUDataMutex);
#endif
    எனில்(!umtx_loadAcquire(gHaveTriedToLoadCommonData)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        pData = openCommonData(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UDataMemory_init(&copyPData);
       எனில்(pData != nullptr) {
          UDatamemory_assign(&copyPData, pData);
          copyPData.map = nullptr;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = nullptr; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              setCommonICUData(&copyPData,/*  The new common data.                                */
                       பொய்,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(gHaveTriedToLoadCommonData, 1);
    }

    didUpdate = findCommonICUDataByName(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#எனில் MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#endif
    திரும்பு didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI வெறுமை U_EXPORT2
udata_setCommonData(மாறிலி வெறுமை *data, UErrorCode *pErrorCode) {
    UDataMemory dataMemory;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }

    எனில்(data==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    /* set the data pointer and test for validity */
    UDataMemory_init(&dataMemory);
    UDataMemory_setData(&dataMemory, data);
    udata_checkCommonData(&dataMemory, pErrorCode);
    எனில் (U_FAILURE(*pErrorCode)) {திரும்பு;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    setCommonICUData(&dataMemory, உண்மை, pErrorCode);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI வெறுமை U_EXPORT2
udata_setAppData(மாறிலி எழுத்து *path, மாறிலி வெறுமை *data, UErrorCode *err)
{
    UDataMemory     udm;

    எனில்(err==nullptr || U_FAILURE(*err)) {
        திரும்பு;
    }
    எனில்(data==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
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
நிலையான UDataMemory *
checkDataItem
(
 மாறிலி DataHeader         *pHeader,         /* The data item to be checked.                */
 UDataMemoryIsAcceptable  *isAcceptable,    /* App's call-back function                    */
 வெறுமை                     *context,         /*   pass-thru param for above.                */
 மாறிலி எழுத்து               *type,            /*   pass-thru param for above.                */
 மாறிலி எழுத்து               *name,            /*   pass-thru param for above.                */
 UErrorCode               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UErrorCode               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UDataMemory  *rDataMem = nullptr;          /* the new UDataMemory, to be returned.        */

    எனில் (U_FAILURE(*fatalErr)) {
        திரும்பு nullptr;
    }

    எனில்(pHeader->dataHeader.magic1==0xda &&
        pHeader->dataHeader.magic2==0x27 &&
        (isAcceptable==nullptr || isAcceptable(context, type, name, &pHeader->info))
    ) {
        rDataMem=UDataMemory_createNewInstance(fatalErr);
        எனில் (U_FAILURE(*fatalErr)) {
            திரும்பு nullptr;
        }
        rDataMem->pHeader = pHeader;
    } இல்லையெனில் {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    திரும்பு rDataMem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
நிலையான UDataMemory *doLoadFromIndividualFiles(மாறிலி எழுத்து *pkgName, 
        மாறிலி எழுத்து *dataPath, மாறிலி எழுத்து *tocEntryPathSuffix,
            /* following arguments are the same as doOpenChoice itself */
            மாறிலி எழுத்து *path, மாறிலி எழுத்து *type, மாறிலி எழுத்து *name,
             UDataMemoryIsAcceptable *isAcceptable, வெறுமை *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    மாறிலி எழுத்து         *pathBuffer;
    UDataMemory         dataMemory;
    UDataMemory *pEntryData;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UDataPathIterator iter(dataPath, pkgName, path, tocEntryPathSuffix, பொய், pErrorCode);

    போது ((pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", pathBuffer);
#endif
        எனில் (uprv_mapFile(&dataMemory, pathBuffer, pErrorCode))
        {
            pEntryData = checkDataItem(dataMemory.pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
            எனில் (pEntryData != nullptr) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                pEntryData->mapAddr = dataMemory.mapAddr;
                pEntryData->map     = dataMemory.map;
                pEntryData->length  = dataMemory.length;

#ifdef UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", pathBuffer);
#endif
                திரும்பு pEntryData;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&dataMemory);

            /* If we had a nasty error, bail out completely.  */
            எனில் (U_FAILURE(*pErrorCode)) {
                திரும்பு nullptr;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *subErrorCode=U_INVALID_FORMAT_ERROR;
        }
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&dataMemory)?"LOADED":"not loaded");
#endif
    }
    திரும்பு nullptr;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
நிலையான UDataMemory *doLoadFromCommonData(UBool isICUData, மாறிலி எழுத்து * /*pkgName*/, 
        மாறிலி எழுத்து * /*dataPath*/, மாறிலி எழுத்து * /*tocEntryPathSuffix*/, மாறிலி எழுத்து *tocEntryName,
            /* following arguments are the same as doOpenChoice itself */
            மாறிலி எழுத்து *path, மாறிலி எழுத்து *type, மாறிலி எழுத்து *name,
             UDataMemoryIsAcceptable *isAcceptable, வெறுமை *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    UDataMemory        *pEntryData;
    மாறிலி DataHeader   *pHeader;
    UDataMemory        *pCommonData;
    int32_t            commonDataIndex;
    UBool              checkedExtendedICUData = பொய்;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    க்கு (commonDataIndex = isICUData ? 0 : -1;;) {
        pCommonData=openCommonData(path, commonDataIndex, subErrorCode); /** search for pkg **/

        எனில்(U_SUCCESS(*subErrorCode) && pCommonData!=nullptr) {
            int32_t length;

            /* look up the data piece in the common data */
            pHeader=pCommonData->vFuncs->Lookup(pCommonData, tocEntryName, &length, subErrorCode);
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", tocEntryName, (வெறுமை*) pHeader, u_errorName(*subErrorCode));
#endif

            எனில்(pHeader!=nullptr) {
                pEntryData = checkDataItem(pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
#ifdef UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (வெறுமை*) pEntryData);
#endif
                எனில் (U_FAILURE(*pErrorCode)) {
                    திரும்பு nullptr;
                }
                எனில் (pEntryData != nullptr) {
                    pEntryData->length = length;
                    திரும்பு pEntryData;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        எனில் (*subErrorCode == U_MEMORY_ALLOCATION_ERROR) {
            *pErrorCode = *subErrorCode;
            திரும்பு nullptr;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        எனில் (!isICUData) {
            திரும்பு nullptr;
        } இல்லையெனில் எனில் (pCommonData != nullptr) {
            ++commonDataIndex;  /* try the next data package */
        } இல்லையெனில் எனில் ((!checkedExtendedICUData) && extendICUData(subErrorCode)) {
            checkedExtendedICUData = உண்மை;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } இல்லையெனில் {
            திரும்பு nullptr;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
நிலையான UBool isTimeZoneFile(மாறிலி எழுத்து *name, மாறிலி எழுத்து *type) {
    திரும்பு ((uprv_strcmp(type, "res") == 0) &&
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
நிலையான UDataMemory *
doOpenChoice(மாறிலி எழுத்து *path, மாறிலி எழுத்து *type, மாறிலி எழுத்து *name,
             UDataMemoryIsAcceptable *isAcceptable, வெறுமை *context,
             UErrorCode *pErrorCode)
{
    UDataMemory         *retVal = nullptr;

    மாறிலி எழுத்து         *dataPath;

    int32_t             tocEntrySuffixIndex;
    மாறிலி எழுத்து         *tocEntryPathSuffix;
    UErrorCode          subErrorCode=U_ZERO_ERROR;
    மாறிலி எழுத்து         *மரம்_Char;

    UBool               isICUData = பொய்;


    FileTracer::traceOpen(path, type, name);


    /* Is this path ICU data? */
    எனில்(path == nullptr ||
       !strcmp(path, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(path, U_ICUDATA_NAME U_மரம்_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_மரம்_SEPARATOR_STRING)) ||  
       !uprv_strncmp(path, U_ICUDATA_ALIAS U_மரம்_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_மரம்_SEPARATOR_STRING))) {
      isICUData = உண்மை;
    }

#எனில் (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString altSepPath;
    எனில்(path) {
        எனில்(uprv_strchr(path,U_FILE_ALT_SEP_CHAR) != nullptr) {
            altSepPath.append(path, *pErrorCode);
            எழுத்து *p;
            போது ((p = uprv_strchr(altSepPath.data(), U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
#எனில் defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", path, altSepPath.data());
#endif
            path = altSepPath.data();
        }
    }
#endif

    CharString tocEntryName; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString tocEntryPath; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString மரம்_Name;

    /* ======= Set up strings */
    எனில்(path==nullptr) {
        pkgName.append(U_ICUDATA_NAME, *pErrorCode);
    } இல்லையெனில் {
        மாறிலி எழுத்து *pkg;
        மாறிலி எழுத்து *first;
        pkg = uprv_strrchr(path, U_FILE_SEP_CHAR);
        first = uprv_strchr(path, U_FILE_SEP_CHAR);
        எனில்(uprv_pathIsAbsolute(path) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            எனில்(pkg) {
                pkgName.append(pkg+1, *pErrorCode);
            } இல்லையெனில் {
                pkgName.append(path, *pErrorCode);
            }
        } இல்லையெனில் {
            மரம்_Char = uprv_strchr(path, U_மரம்_SEPARATOR);
            எனில்(மரம்_Char) { 
                மரம்_Name.append(மரம்_Char+1, *pErrorCode); /* following '-' */
                எனில்(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } இல்லையெனில் {
                    pkgName.append(path, static_cast<int32_t>(மரம்_Char - path), *pErrorCode);
                    எனில் (first == nullptr) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        path = pkgName.data();
                    }
                }
            } இல்லையெனில் {
                எனில்(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } இல்லையெனில் {
                    pkgName.append(path, *pErrorCode);
                }
            }
        }
    }

#ifdef UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.data(), மரம்_Name.data());
#endif

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    tocEntryName.append(pkgName, *pErrorCode);
    tocEntryPath.append(pkgName, *pErrorCode);
    tocEntrySuffixIndex = tocEntryName.length();

    எனில்(!மரம்_Name.isEmpty()) {
        tocEntryName.append(U_மரம்_ENTRY_SEP_CHAR, *pErrorCode).append(மரம்_Name, *pErrorCode);
        tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(மரம்_Name, *pErrorCode);
    }

    tocEntryName.append(U_மரம்_ENTRY_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    எனில்(type!=nullptr && *type!=0) {
        tocEntryName.append(".", *pErrorCode).append(type, *pErrorCode);
        tocEntryPath.append(".", *pErrorCode).append(type, *pErrorCode);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    tocEntryPathSuffix = tocEntryPath.data() + tocEntrySuffixIndex + 1; /* suffix starts here */

#ifdef UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", tocEntryName.data());
    fprintf(stderr, " tocEntryPath = %s\n", tocEntryPath.data());
#endif

#எனில் !defined(ICU_DATA_DIR_WINDOWS)
    எனில்(path == nullptr) {
        path = COMMON_DATA_NAME; /* "icudt26e" */
    }
#இல்லையெனில்
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
    எனில் (isICUData && type != nullptr && isTimeZoneFile(name, type)) {
        மாறிலி எழுத்து *tzFilesDir = u_getTimeZoneFilesDirectory(pErrorCode);
        எனில் (tzFilesDir[0] != 0) {
#ifdef UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#endif
            retVal = doLoadFromIndividualFiles(/* pkgName.data() */ "", tzFilesDir, tocEntryPathSuffix,
                            /* path */ "", type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            எனில்((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                திரும்பு retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    எனில்(gDataFileAccess == UDATA_PACKAGES_FIRST) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#endif
        /* #2 */
        retVal = doLoadFromCommonData(isICUData, 
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        எனில்((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            திரும்பு retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    எனில்((gDataFileAccess==UDATA_PACKAGES_FIRST) ||
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#endif
        /* Check to make sure that there is a dataPath to iterate over */
        எனில் ((dataPath && *dataPath) || !isICUData) {
            retVal = doLoadFromIndividualFiles(pkgName.data(), dataPath, tocEntryPathSuffix,
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            எனில்((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                திரும்பு retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    எனில்((gDataFileAccess==UDATA_ONLY_PACKAGES) || 
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        எனில்((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            திரும்பு retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    எனில்(gDataFileAccess==UDATA_NO_FILES) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), "", tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        எனில்((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            திரும்பு retVal;
        }
    }

    /* data not found */
    எனில்(U_SUCCESS(*pErrorCode)) {
        எனில்(U_SUCCESS(subErrorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } இல்லையெனில் {
            /* entry point not found or rejected */
            *pErrorCode=subErrorCode;
        }
    }
    திரும்பு retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(மாறிலி எழுத்து *path, மாறிலி எழுத்து *type, மாறிலி எழுத்து *name,
           UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
    fflush(stderr);
#endif

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    } இல்லையெனில் எனில்(name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    } இல்லையெனில் {
        திரும்பு doOpenChoice(path, type, name, nullptr, nullptr, pErrorCode);
    }
}



U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(மாறிலி எழுத்து *path, மாறிலி எழுத்து *type, மாறிலி எழுத்து *name,
                 UDataMemoryIsAcceptable *isAcceptable, வெறுமை *context,
                 UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
#endif

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    } இல்லையெனில் எனில்(name==nullptr || *name==0 || isAcceptable==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    } இல்லையெனில் {
        திரும்பு doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}



U_CAPI வெறுமை U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    எனில்(pInfo!=nullptr) {
        எனில்(pData!=nullptr && pData->pHeader!=nullptr) {
            மாறிலி UDataInfo *info=&pData->pHeader->info;
            uint16_t dataInfoSize=udata_getInfoSize(info);
            எனில்(pInfo->size>dataInfoSize) {
                pInfo->size=dataInfoSize;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (மாறிலி uint16_t *)info+1, pInfo->size-2);
            எனில்(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reservedWord;
                pInfo->reservedWord=(uint16_t)((x<<8)|(x>>8));
            }
        } இல்லையெனில் {
            pInfo->size=0;
        }
    }
}


U_CAPI வெறுமை U_EXPORT2 udata_setFileAccess(UDataFileAccess access, UErrorCode * /*status*/)
{
    // Note: this function is documented as not thread safe.
    gDataFileAccess = access;
}
