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

#അന്തര്ഭാവ "unicode/utypes.h"  /* U_PLATFORM etc. */

#ifdef __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#endif

#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/uversion.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "restrace.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "ucmndata.h"
#അന്തര്ഭാവ "udatamem.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "umapfile.h"
#അന്തര്ഭാവ "umutex.h"

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

#എങ്കിൽ defined(UDATA_DEBUG)
#   include <stdio.h>
#endif

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
സ്ഥിതം UDataMemory *udata_findCachedData(സ്ഥിരം അക്ഷരം *path, UErrorCode &err);

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
സ്ഥിതം UDataMemory *gCommonICUDataArray[10] = { nullptr };   // Access protected by icu global mutex.

സ്ഥിതം u_atomic_int32_t gHaveTriedToLoadCommonData {0};  //  See extendICUData().

സ്ഥിതം UHashtable  *gCommonDataCache = nullptr;  /* Global hash table of opened ICU data files.  */
സ്ഥിതം icu::UInitOnce gCommonDataCacheInitOnce {};

#എങ്കിൽ !defined(ICU_DATA_DIR_WINDOWS)
സ്ഥിതം UDataFileAccess  gDataFileAccess = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#അല്ലെങ്കിൽ
// If we are using the Windows data directory, then look in one spot only.
സ്ഥിതം UDataFileAccess  gDataFileAccess = UDATA_NO_FILES;
#endif

സ്ഥിതം UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    എങ്കിൽ (gCommonDataCache) {             /* Delete the cache of user data mappings.  */
        uhash_close(gCommonDataCache);  /*   Table owns the contents, and will delete them. */
        gCommonDataCache = nullptr;        /*   Cleanup is not thread safe.                */
    }
    gCommonDataCacheInitOnce.reset();

    വേണ്ടി (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != nullptr; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = nullptr;
    }
    gHaveTriedToLoadCommonData = 0;

    തിരികെ സത്യം;                   /* Everything was cleaned up */
}

സ്ഥിതം UBool U_CALLCONV
findCommonICUDataByName(സ്ഥിരം അക്ഷരം *inBasename, UErrorCode &err)
{
    UBool found = അസത്യം;
    int32_t i;

    UDataMemory  *pData = udata_findCachedData(inBasename, err);
    എങ്കിൽ (U_FAILURE(err) || pData == nullptr)
        തിരികെ അസത്യം;

    {
        Mutex lock;
        വേണ്ടി (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            എങ്കിൽ ((gCommonICUDataArray[i] != nullptr) && (gCommonICUDataArray[i]->pHeader == pData->pHeader)) {
                /* The data pointer is already in the array. */
                found = സത്യം;
                നിർത്തുക;
            }
        }
    }
    തിരികെ found;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
സ്ഥിതം UBool
setCommonICUData(UDataMemory *pData,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UErrorCode *pErr)
{
    UDataMemory  *newCommonData = UDataMemory_createNewInstance(pErr);
    int32_t i;
    UBool didUpdate = അസത്യം;
    എങ്കിൽ (U_FAILURE(*pErr)) {
        തിരികെ അസത്യം;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_assign(newCommonData, pData);
    umtx_lock(nullptr);
    വേണ്ടി (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        എങ്കിൽ (gCommonICUDataArray[i] == nullptr) {
            gCommonICUDataArray[i] = newCommonData;
            didUpdate = സത്യം;
            നിർത്തുക;
        } അല്ലെങ്കിൽ എങ്കിൽ (gCommonICUDataArray[i]->pHeader == pData->pHeader) {
            /* The same data pointer is already in the array. */
            നിർത്തുക;
        }
    }
    umtx_unlock(nullptr);

    എങ്കിൽ (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    എങ്കിൽ (didUpdate) {
        ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } അല്ലെങ്കിൽ {
        uprv_free(newCommonData);
    }
    തിരികെ didUpdate;
}

#എങ്കിൽ !defined(ICU_DATA_DIR_WINDOWS)

സ്ഥിതം UBool
setCommonICUDataPointer(സ്ഥിരം ശൂന്യം *pData, UBool /*warn*/, UErrorCode *pErrorCode) {
    UDataMemory tData;
    UDataMemory_init(&tData);
    UDataMemory_setData(&tData, pData);
    udata_checkCommonData(&tData, pErrorCode);
    തിരികെ setCommonICUData(&tData, അസത്യം, pErrorCode);
}

#endif

സ്ഥിതം സ്ഥിരം അക്ഷരം *
findBasename(സ്ഥിരം അക്ഷരം *path) {
    സ്ഥിരം അക്ഷരം *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    എങ്കിൽ(basename==nullptr) {
        തിരികെ path;
    } അല്ലെങ്കിൽ {
        തിരികെ basename+1;
    }
}

#ifdef UDATA_DEBUG
സ്ഥിതം സ്ഥിരം അക്ഷരം *
packageNameFromPath(സ്ഥിരം അക്ഷരം *path)
{
    എങ്കിൽ((path == nullptr) || (*path == 0)) {
        തിരികെ U_ICUDATA_NAME;
    }

    path = findBasename(path);

    എങ്കിൽ((path == nullptr) || (*path == 0)) {
        തിരികെ U_ICUDATA_NAME;
    }

    തിരികെ path;
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

തരനിർവചനം ഘടന DataCacheElement {
    അക്ഷരം          *name;
    UDataMemory   *item;
} DataCacheElement;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
സ്ഥിതം ശൂന്യം U_CALLCONV DataCacheElement_deleter(ശൂന്യം *pDCEl) {
    DataCacheElement* p = static_cast<DataCacheElement*>(pDCEl);
    udata_close(p->item);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

സ്ഥിതം ശൂന്യം U_CALLCONV udata_initHashTable(UErrorCode &err) {
    U_ASSERT(gCommonDataCache == nullptr);
    gCommonDataCache = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &err);
    എങ്കിൽ (U_FAILURE(err)) {
       തിരികെ;
    }
    U_ASSERT(gCommonDataCache != nullptr);
    uhash_set_മൂല്യം_Deleter(gCommonDataCache, DataCacheElement_deleter);
    ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
സ്ഥിതം UHashtable *udata_getHashTable(UErrorCode &err) {
    umtx_initOnce(gCommonDataCacheInitOnce, &udata_initHashTable, err);
    തിരികെ gCommonDataCache;
}



സ്ഥിതം UDataMemory *udata_findCachedData(സ്ഥിരം അക്ഷരം *path, UErrorCode &err)
{
    UHashtable        *htable;
    UDataMemory       *retVal = nullptr;
    DataCacheElement  *el;
    സ്ഥിരം അക്ഷരം        *baseName;

    htable = udata_getHashTable(err);
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ nullptr;
    }

    baseName = findBasename(path);   /* Cache remembers only the base name, not the full path. */
    umtx_lock(nullptr);
    el = static_cast<DataCacheElement*>(uhash_get(htable, baseName));
    umtx_unlock(nullptr);
    എങ്കിൽ (el != nullptr) {
        retVal = el->item;
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", baseName, (ശൂന്യം*) retVal);
#endif
    തിരികെ retVal;
}


സ്ഥിതം UDataMemory *udata_cacheDataItem(സ്ഥിരം അക്ഷരം *path, UDataMemory *item, UErrorCode *pErr) {
    DataCacheElement *newElement;
    സ്ഥിരം അക്ഷരം       *baseName;
    int32_t           nameLen;
    UHashtable       *htable;
    DataCacheElement *old_മൂല്യം = nullptr;
    UErrorCode        subErr = U_ZERO_ERROR;

    htable = udata_getHashTable(*pErr);
    എങ്കിൽ (U_FAILURE(*pErr)) {
        തിരികെ nullptr;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<DataCacheElement*>(uprv_malloc(വലുപ്പം(DataCacheElement)));
    എങ്കിൽ (newElement == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    newElement->item = UDataMemory_createNewInstance(pErr);
    എങ്കിൽ (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        തിരികെ nullptr;
    }
    UDatamemory_assign(newElement->item, item);

    baseName = findBasename(path);
    nameLen = static_cast<int32_t>(uprv_strlen(baseName));
    newElement->name = static_cast<അക്ഷരം*>(uprv_malloc(nameLen + 1));
    എങ്കിൽ (newElement->name == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        തിരികെ nullptr;
    }
    uprv_strcpy(newElement->name, baseName);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(nullptr);
    old_മൂല്യം = static_cast<DataCacheElement*>(uhash_get(htable, path));
    എങ്കിൽ (old_മൂല്യം != nullptr) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    അല്ലെങ്കിൽ {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(nullptr);

#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (ശൂന്യം*) newElement->item, u_errorName(subErr), (ശൂന്യം*) newElement->item->vFuncs);
#endif

    എങ്കിൽ (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        തിരികെ old_മൂല്യം ? old_മൂല്യം->item : nullptr;
    }

    തിരികെ newElement->item;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

വർഗ്ഗം UDataPathIterator
{
പൊതു:
    UDataPathIterator(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *pkg,
                      സ്ഥിരം അക്ഷരം *item, സ്ഥിരം അക്ഷരം *suffix, UBool doCheckLastFour,
                      UErrorCode *pErrorCode);
    സ്ഥിരം അക്ഷരം *next(UErrorCode *pErrorCode);

സ്വകാര്യ:
    സ്ഥിരം അക്ഷരം *path;                              /* working path (u_icudata_Dir) */
    സ്ഥിരം അക്ഷരം *nextPath;                          /* path following this one */
    സ്ഥിരം അക്ഷരം *basename;                          /* item's basename (icudt22e_mt.res)*/

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
UDataPathIterator::UDataPathIterator(സ്ഥിരം അക്ഷരം *inPath, സ്ഥിരം അക്ഷരം *pkg,
                                     സ്ഥിരം അക്ഷരം *item, സ്ഥിരം അക്ഷരം *inSuffix, UBool doCheckLastFour,
                                     UErrorCode *pErrorCode)
{
#ifdef UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", inSuffix, inPath);
#endif
    /** Path **/
    എങ്കിൽ(inPath == nullptr) {
        path = u_getDataDirectory();
    } അല്ലെങ്കിൽ {
        path = inPath;
    }

    /** Package **/
    എങ്കിൽ(pkg != nullptr) {
      packageStub.append(U_FILE_SEP_CHAR, *pErrorCode).append(pkg, *pErrorCode);
#ifdef UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.data(), packageStub.length());
#endif
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    എങ്കിൽ(basename == item) {
        nextPath = path;
    } അല്ലെങ്കിൽ {
        itemPath.append(item, static_cast<int32_t>(basename - item), *pErrorCode);
        nextPath = itemPath.data();
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", inSuffix, (ശൂന്യം*) inSuffix);
#endif

    /** Suffix  **/
    എങ്കിൽ(inSuffix != nullptr) {
        suffix = inSuffix;
    } അല്ലെങ്കിൽ {
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
സ്ഥിരം അക്ഷരം *UDataPathIterator::next(UErrorCode *pErrorCode)
{
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }

    സ്ഥിരം അക്ഷരം *currentPath = nullptr;
    int32_t     pathLen = 0;
    സ്ഥിരം അക്ഷരം *pathBasename;

    ചെയ്യുക
    {
        എങ്കിൽ( nextPath == nullptr ) {
            നിർത്തുക;
        }
        currentPath = nextPath;

        എങ്കിൽ(nextPath == itemPath.data()) { /* we were processing item's path. */
            nextPath = path; /* start with regular path next tm. */
            pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
        } അല്ലെങ്കിൽ {
            /* fix up next for next time */
            nextPath = uprv_strchr(currentPath, U_PATH_SEP_CHAR);
            എങ്കിൽ(nextPath == nullptr) {
                /* segment: entire path */
                pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
            } അല്ലെങ്കിൽ {
                /* segment: until next segment */
                pathLen = static_cast<int32_t>(nextPath - currentPath);
                /* skip divider */
                nextPath ++;
            }
        }

        എങ്കിൽ(pathLen == 0) {
            തുടരുക;
        }

#ifdef UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", currentPath);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            വേണ്ടി(qqq=0;qqq<pathLen;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#endif
        pathBuffer.clear().append(currentPath, pathLen, *pErrorCode);

        /* check for .dat files */
        pathBasename = findBasename(pathBuffer.data());

        എങ്കിൽ(checkLastFour && 
           (pathLen>=4) &&
           uprv_strncmp(pathBuffer.data() +(pathLen-4), suffix.data(), 4)==0 && /* suffix matches */
           uprv_strncmp(findBasename(pathBuffer.data()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(pathBasename)==(basenameLen+4)) { /* base+suffix = full len */

#ifdef UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.data(), pathBuffer.data());
#endif
            /* do nothing */
        }
        അല്ലെങ്കിൽ 
        {       /* regular dir path */
            എങ്കിൽ(pathBuffer[pathLen-1] != U_FILE_SEP_CHAR) {
                എങ്കിൽ((pathLen>=4) &&
                   uprv_strncmp(pathBuffer.data()+(pathLen-4), ".dat", 4) == 0)
                {
#ifdef UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", pathBuffer.data());
#endif
                    തുടരുക;
                }

                /* Check if it is a directory with the same name as our package */
                എങ്കിൽ(!packageStub.isEmpty() &&
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

            എങ്കിൽ (!suffix.empty())  /* tack on suffix */
            {
                എങ്കിൽ (suffix.length() > 4) {
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

        തിരികെ pathBuffer.data();

    } ആകുമ്പോൾ(path);

    /* fell way off the end */
    തിരികെ nullptr;
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
#എങ്കിൽ !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
ബാഹ്യ "C" സ്ഥിരം DataHeader U_DATA_API U_ICUDATA_ENTRY_POINT;
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
സ്ഥിതം UDataMemory *
openCommonData(സ്ഥിരം അക്ഷരം *path,          /*  Path from OpenChoice?          */
               int32_t commonDataIndex,   /*  ICU Data (index >= 0) if path == nullptr */
               UErrorCode *pErrorCode)
{
    UDataMemory tData;
    സ്ഥിരം അക്ഷരം *pathBuffer;
    സ്ഥിരം അക്ഷരം *inBasename;

    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }

    UDataMemory_init(&tData);

    /* ??????? TODO revisit this */ 
    എങ്കിൽ (commonDataIndex >= 0) {
        /* "mini-cache" for common ICU data */
        എങ്കിൽ(commonDataIndex >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            തിരികെ nullptr;
        }
        {
            Mutex lock;
            എങ്കിൽ(gCommonICUDataArray[commonDataIndex] != nullptr) {
                തിരികെ gCommonICUDataArray[commonDataIndex];
            }
#എങ്കിൽ !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            വേണ്ടി(i = 0; i < commonDataIndex; ++i) {
                എങ്കിൽ(gCommonICUDataArray[i]->pHeader == &U_ICUDATA_ENTRY_POINT) {
                    /* The linked-in data is already in the list. */
                    തിരികെ nullptr;
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
#എങ്കിൽ !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        setCommonICUDataPointer(&U_ICUDATA_ENTRY_POINT, അസത്യം, pErrorCode);
        {
            Mutex lock;
            തിരികെ gCommonICUDataArray[commonDataIndex];
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

    എങ്കിൽ(*inBasename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", path);
#endif
        എങ്കിൽ (U_SUCCESS(*pErrorCode)) {
            *pErrorCode=U_FILE_ACCESS_ERROR;
        }
        തിരികെ nullptr;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UDataMemory  *dataToReturn = udata_findCachedData(inBasename, *pErrorCode);
    എങ്കിൽ (dataToReturn != nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ dataToReturn;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UDataPathIterator iter(u_getDataDirectory(), inBasename, path, ".dat", സത്യം, pErrorCode);

    ആകുമ്പോൾ ((UDataMemory_isLoaded(&tData)==അസത്യം) && (pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", pathBuffer);
#endif
        uprv_mapFile(&tData, pathBuffer, pErrorCode);
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&tData)?"LOADED":"not loaded");
#endif
    }
    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }

    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (!UDataMemory_isLoaded(&tData)) {
        /* no common data */
        *pErrorCode=U_FILE_ACCESS_ERROR;
        തിരികെ nullptr;
    }

    /* we have mapped a file, check its header */
    udata_checkCommonData(&tData, pErrorCode);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    തിരികെ udata_cacheDataItem(inBasename, &tData, pErrorCode);
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
സ്ഥിതം UBool extendICUData(UErrorCode *pErr)
{
    UDataMemory   *pData;
    UDataMemory   copyPData;
    UBool         didUpdate = അസത്യം;

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
#എങ്കിൽ MAP_IMPLEMENTATION==MAP_STDIO
    സ്ഥിതം UMutex extendICUDataMutex;
    umtx_lock(&extendICUDataMutex);
#endif
    എങ്കിൽ(!umtx_loadAcquire(gHaveTriedToLoadCommonData)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        pData = openCommonData(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UDataMemory_init(&copyPData);
       എങ്കിൽ(pData != nullptr) {
          UDatamemory_assign(&copyPData, pData);
          copyPData.map = nullptr;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = nullptr; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              setCommonICUData(&copyPData,/*  The new common data.                                */
                       അസത്യം,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(gHaveTriedToLoadCommonData, 1);
    }

    didUpdate = findCommonICUDataByName(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#എങ്കിൽ MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#endif
    തിരികെ didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI ശൂന്യം U_EXPORT2
udata_setCommonData(സ്ഥിരം ശൂന്യം *data, UErrorCode *pErrorCode) {
    UDataMemory dataMemory;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }

    എങ്കിൽ(data==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    /* set the data pointer and test for validity */
    UDataMemory_init(&dataMemory);
    UDataMemory_setData(&dataMemory, data);
    udata_checkCommonData(&dataMemory, pErrorCode);
    എങ്കിൽ (U_FAILURE(*pErrorCode)) {തിരികെ;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    setCommonICUData(&dataMemory, സത്യം, pErrorCode);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI ശൂന്യം U_EXPORT2
udata_setAppData(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം ശൂന്യം *data, UErrorCode *err)
{
    UDataMemory     udm;

    എങ്കിൽ(err==nullptr || U_FAILURE(*err)) {
        തിരികെ;
    }
    എങ്കിൽ(data==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
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
സ്ഥിതം UDataMemory *
checkDataItem
(
 സ്ഥിരം DataHeader         *pHeader,         /* The data item to be checked.                */
 UDataMemoryIsAcceptable  *isAcceptable,    /* App's call-back function                    */
 ശൂന്യം                     *context,         /*   pass-thru param for above.                */
 സ്ഥിരം അക്ഷരം               *type,            /*   pass-thru param for above.                */
 സ്ഥിരം അക്ഷരം               *name,            /*   pass-thru param for above.                */
 UErrorCode               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UErrorCode               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UDataMemory  *rDataMem = nullptr;          /* the new UDataMemory, to be returned.        */

    എങ്കിൽ (U_FAILURE(*fatalErr)) {
        തിരികെ nullptr;
    }

    എങ്കിൽ(pHeader->dataHeader.magic1==0xda &&
        pHeader->dataHeader.magic2==0x27 &&
        (isAcceptable==nullptr || isAcceptable(context, type, name, &pHeader->info))
    ) {
        rDataMem=UDataMemory_createNewInstance(fatalErr);
        എങ്കിൽ (U_FAILURE(*fatalErr)) {
            തിരികെ nullptr;
        }
        rDataMem->pHeader = pHeader;
    } അല്ലെങ്കിൽ {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    തിരികെ rDataMem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
സ്ഥിതം UDataMemory *doLoadFromIndividualFiles(സ്ഥിരം അക്ഷരം *pkgName, 
        സ്ഥിരം അക്ഷരം *dataPath, സ്ഥിരം അക്ഷരം *tocEntryPathSuffix,
            /* following arguments are the same as doOpenChoice itself */
            സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *type, സ്ഥിരം അക്ഷരം *name,
             UDataMemoryIsAcceptable *isAcceptable, ശൂന്യം *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    സ്ഥിരം അക്ഷരം         *pathBuffer;
    UDataMemory         dataMemory;
    UDataMemory *pEntryData;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UDataPathIterator iter(dataPath, pkgName, path, tocEntryPathSuffix, അസത്യം, pErrorCode);

    ആകുമ്പോൾ ((pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", pathBuffer);
#endif
        എങ്കിൽ (uprv_mapFile(&dataMemory, pathBuffer, pErrorCode))
        {
            pEntryData = checkDataItem(dataMemory.pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
            എങ്കിൽ (pEntryData != nullptr) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                pEntryData->mapAddr = dataMemory.mapAddr;
                pEntryData->map     = dataMemory.map;
                pEntryData->length  = dataMemory.length;

#ifdef UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", pathBuffer);
#endif
                തിരികെ pEntryData;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&dataMemory);

            /* If we had a nasty error, bail out completely.  */
            എങ്കിൽ (U_FAILURE(*pErrorCode)) {
                തിരികെ nullptr;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *subErrorCode=U_INVALID_FORMAT_ERROR;
        }
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&dataMemory)?"LOADED":"not loaded");
#endif
    }
    തിരികെ nullptr;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
സ്ഥിതം UDataMemory *doLoadFromCommonData(UBool isICUData, സ്ഥിരം അക്ഷരം * /*pkgName*/, 
        സ്ഥിരം അക്ഷരം * /*dataPath*/, സ്ഥിരം അക്ഷരം * /*tocEntryPathSuffix*/, സ്ഥിരം അക്ഷരം *tocEntryName,
            /* following arguments are the same as doOpenChoice itself */
            സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *type, സ്ഥിരം അക്ഷരം *name,
             UDataMemoryIsAcceptable *isAcceptable, ശൂന്യം *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    UDataMemory        *pEntryData;
    സ്ഥിരം DataHeader   *pHeader;
    UDataMemory        *pCommonData;
    int32_t            commonDataIndex;
    UBool              checkedExtendedICUData = അസത്യം;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    വേണ്ടി (commonDataIndex = isICUData ? 0 : -1;;) {
        pCommonData=openCommonData(path, commonDataIndex, subErrorCode); /** search for pkg **/

        എങ്കിൽ(U_SUCCESS(*subErrorCode) && pCommonData!=nullptr) {
            int32_t length;

            /* look up the data piece in the common data */
            pHeader=pCommonData->vFuncs->Lookup(pCommonData, tocEntryName, &length, subErrorCode);
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", tocEntryName, (ശൂന്യം*) pHeader, u_errorName(*subErrorCode));
#endif

            എങ്കിൽ(pHeader!=nullptr) {
                pEntryData = checkDataItem(pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
#ifdef UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (ശൂന്യം*) pEntryData);
#endif
                എങ്കിൽ (U_FAILURE(*pErrorCode)) {
                    തിരികെ nullptr;
                }
                എങ്കിൽ (pEntryData != nullptr) {
                    pEntryData->length = length;
                    തിരികെ pEntryData;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        എങ്കിൽ (*subErrorCode == U_MEMORY_ALLOCATION_ERROR) {
            *pErrorCode = *subErrorCode;
            തിരികെ nullptr;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        എങ്കിൽ (!isICUData) {
            തിരികെ nullptr;
        } അല്ലെങ്കിൽ എങ്കിൽ (pCommonData != nullptr) {
            ++commonDataIndex;  /* try the next data package */
        } അല്ലെങ്കിൽ എങ്കിൽ ((!checkedExtendedICUData) && extendICUData(subErrorCode)) {
            checkedExtendedICUData = സത്യം;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } അല്ലെങ്കിൽ {
            തിരികെ nullptr;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
സ്ഥിതം UBool isTimeZoneFile(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *type) {
    തിരികെ ((uprv_strcmp(type, "res") == 0) &&
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
സ്ഥിതം UDataMemory *
doOpenChoice(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *type, സ്ഥിരം അക്ഷരം *name,
             UDataMemoryIsAcceptable *isAcceptable, ശൂന്യം *context,
             UErrorCode *pErrorCode)
{
    UDataMemory         *retVal = nullptr;

    സ്ഥിരം അക്ഷരം         *dataPath;

    int32_t             tocEntrySuffixIndex;
    സ്ഥിരം അക്ഷരം         *tocEntryPathSuffix;
    UErrorCode          subErrorCode=U_ZERO_ERROR;
    സ്ഥിരം അക്ഷരം         *വൃക്ഷം_Char;

    UBool               isICUData = അസത്യം;


    FileTracer::traceOpen(path, type, name);


    /* Is this path ICU data? */
    എങ്കിൽ(path == nullptr ||
       !strcmp(path, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(path, U_ICUDATA_NAME U_വൃക്ഷം_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_വൃക്ഷം_SEPARATOR_STRING)) ||  
       !uprv_strncmp(path, U_ICUDATA_ALIAS U_വൃക്ഷം_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_വൃക്ഷം_SEPARATOR_STRING))) {
      isICUData = സത്യം;
    }

#എങ്കിൽ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString altSepPath;
    എങ്കിൽ(path) {
        എങ്കിൽ(uprv_strchr(path,U_FILE_ALT_SEP_CHAR) != nullptr) {
            altSepPath.append(path, *pErrorCode);
            അക്ഷരം *p;
            ആകുമ്പോൾ ((p = uprv_strchr(altSepPath.data(), U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
#എങ്കിൽ defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", path, altSepPath.data());
#endif
            path = altSepPath.data();
        }
    }
#endif

    CharString tocEntryName; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString tocEntryPath; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString വൃക്ഷം_Name;

    /* ======= Set up strings */
    എങ്കിൽ(path==nullptr) {
        pkgName.append(U_ICUDATA_NAME, *pErrorCode);
    } അല്ലെങ്കിൽ {
        സ്ഥിരം അക്ഷരം *pkg;
        സ്ഥിരം അക്ഷരം *first;
        pkg = uprv_strrchr(path, U_FILE_SEP_CHAR);
        first = uprv_strchr(path, U_FILE_SEP_CHAR);
        എങ്കിൽ(uprv_pathIsAbsolute(path) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            എങ്കിൽ(pkg) {
                pkgName.append(pkg+1, *pErrorCode);
            } അല്ലെങ്കിൽ {
                pkgName.append(path, *pErrorCode);
            }
        } അല്ലെങ്കിൽ {
            വൃക്ഷം_Char = uprv_strchr(path, U_വൃക്ഷം_SEPARATOR);
            എങ്കിൽ(വൃക്ഷം_Char) { 
                വൃക്ഷം_Name.append(വൃക്ഷം_Char+1, *pErrorCode); /* following '-' */
                എങ്കിൽ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } അല്ലെങ്കിൽ {
                    pkgName.append(path, static_cast<int32_t>(വൃക്ഷം_Char - path), *pErrorCode);
                    എങ്കിൽ (first == nullptr) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        path = pkgName.data();
                    }
                }
            } അല്ലെങ്കിൽ {
                എങ്കിൽ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } അല്ലെങ്കിൽ {
                    pkgName.append(path, *pErrorCode);
                }
            }
        }
    }

#ifdef UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.data(), വൃക്ഷം_Name.data());
#endif

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    tocEntryName.append(pkgName, *pErrorCode);
    tocEntryPath.append(pkgName, *pErrorCode);
    tocEntrySuffixIndex = tocEntryName.length();

    എങ്കിൽ(!വൃക്ഷം_Name.isEmpty()) {
        tocEntryName.append(U_വൃക്ഷം_ENTRY_SEP_CHAR, *pErrorCode).append(വൃക്ഷം_Name, *pErrorCode);
        tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(വൃക്ഷം_Name, *pErrorCode);
    }

    tocEntryName.append(U_വൃക്ഷം_ENTRY_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    എങ്കിൽ(type!=nullptr && *type!=0) {
        tocEntryName.append(".", *pErrorCode).append(type, *pErrorCode);
        tocEntryPath.append(".", *pErrorCode).append(type, *pErrorCode);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    tocEntryPathSuffix = tocEntryPath.data() + tocEntrySuffixIndex + 1; /* suffix starts here */

#ifdef UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", tocEntryName.data());
    fprintf(stderr, " tocEntryPath = %s\n", tocEntryPath.data());
#endif

#എങ്കിൽ !defined(ICU_DATA_DIR_WINDOWS)
    എങ്കിൽ(path == nullptr) {
        path = COMMON_DATA_NAME; /* "icudt26e" */
    }
#അല്ലെങ്കിൽ
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
    എങ്കിൽ (isICUData && type != nullptr && isTimeZoneFile(name, type)) {
        സ്ഥിരം അക്ഷരം *tzFilesDir = u_getTimeZoneFilesDirectory(pErrorCode);
        എങ്കിൽ (tzFilesDir[0] != 0) {
#ifdef UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#endif
            retVal = doLoadFromIndividualFiles(/* pkgName.data() */ "", tzFilesDir, tocEntryPathSuffix,
                            /* path */ "", type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            എങ്കിൽ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                തിരികെ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    എങ്കിൽ(gDataFileAccess == UDATA_PACKAGES_FIRST) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#endif
        /* #2 */
        retVal = doLoadFromCommonData(isICUData, 
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        എങ്കിൽ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            തിരികെ retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    എങ്കിൽ((gDataFileAccess==UDATA_PACKAGES_FIRST) ||
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#endif
        /* Check to make sure that there is a dataPath to iterate over */
        എങ്കിൽ ((dataPath && *dataPath) || !isICUData) {
            retVal = doLoadFromIndividualFiles(pkgName.data(), dataPath, tocEntryPathSuffix,
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            എങ്കിൽ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                തിരികെ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    എങ്കിൽ((gDataFileAccess==UDATA_ONLY_PACKAGES) || 
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        എങ്കിൽ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            തിരികെ retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    എങ്കിൽ(gDataFileAccess==UDATA_NO_FILES) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), "", tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        എങ്കിൽ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            തിരികെ retVal;
        }
    }

    /* data not found */
    എങ്കിൽ(U_SUCCESS(*pErrorCode)) {
        എങ്കിൽ(U_SUCCESS(subErrorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } അല്ലെങ്കിൽ {
            /* entry point not found or rejected */
            *pErrorCode=subErrorCode;
        }
    }
    തിരികെ retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *type, സ്ഥിരം അക്ഷരം *name,
           UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
    fflush(stderr);
#endif

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ(name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        തിരികെ doOpenChoice(path, type, name, nullptr, nullptr, pErrorCode);
    }
}



U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *type, സ്ഥിരം അക്ഷരം *name,
                 UDataMemoryIsAcceptable *isAcceptable, ശൂന്യം *context,
                 UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
#endif

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ(name==nullptr || *name==0 || isAcceptable==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        തിരികെ doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}



U_CAPI ശൂന്യം U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    എങ്കിൽ(pInfo!=nullptr) {
        എങ്കിൽ(pData!=nullptr && pData->pHeader!=nullptr) {
            സ്ഥിരം UDataInfo *info=&pData->pHeader->info;
            uint16_t dataInfoSize=udata_getInfoSize(info);
            എങ്കിൽ(pInfo->size>dataInfoSize) {
                pInfo->size=dataInfoSize;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (സ്ഥിരം uint16_t *)info+1, pInfo->size-2);
            എങ്കിൽ(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reservedWord;
                pInfo->reservedWord=(uint16_t)((x<<8)|(x>>8));
            }
        } അല്ലെങ്കിൽ {
            pInfo->size=0;
        }
    }
}


U_CAPI ശൂന്യം U_EXPORT2 udata_setFileAccess(UDataFileAccess access, UErrorCode * /*status*/)
{
    // Note: this function is documented as not thread safe.
    gDataFileAccess = access;
}
