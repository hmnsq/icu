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

#अन्तर्भाव "unicode/utypes.h"  /* U_PLATFORM etc. */

#ifdef __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#endif

#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/uversion.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "restrace.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "ucmndata.h"
#अन्तर्भाव "udatamem.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "umapfile.h"
#अन्तर्भाव "umutex.h"

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

#जर defined(UDATA_DEBUG)
#   include <stdio.h>
#endif

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
स्थिर UDataMemory *udata_findCachedData(स्थिरांक अक्षर *path, UErrorCode &err);

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
स्थिर UDataMemory *gCommonICUDataArray[10] = { nullptr };   // Access protected by icu global mutex.

स्थिर u_atomic_int32_t gHaveTriedToLoadCommonData {0};  //  See extendICUData().

स्थिर UHashtable  *gCommonDataCache = nullptr;  /* Global hash table of opened ICU data files.  */
स्थिर icu::UInitOnce gCommonDataCacheInitOnce {};

#जर !defined(ICU_DATA_DIR_WINDOWS)
स्थिर UDataFileAccess  gDataFileAccess = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#अन्यथा
// If we are using the Windows data directory, then look in one spot only.
स्थिर UDataFileAccess  gDataFileAccess = UDATA_NO_FILES;
#endif

स्थिर UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    जर (gCommonDataCache) {             /* Delete the cache of user data mappings.  */
        uhash_close(gCommonDataCache);  /*   Table owns the contents, and will delete them. */
        gCommonDataCache = nullptr;        /*   Cleanup is not thread safe.                */
    }
    gCommonDataCacheInitOnce.reset();

    साठी (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != nullptr; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = nullptr;
    }
    gHaveTriedToLoadCommonData = 0;

    परत सत्य;                   /* Everything was cleaned up */
}

स्थिर UBool U_CALLCONV
findCommonICUDataByName(स्थिरांक अक्षर *inBasename, UErrorCode &err)
{
    UBool found = असत्य;
    int32_t i;

    UDataMemory  *pData = udata_findCachedData(inBasename, err);
    जर (U_FAILURE(err) || pData == nullptr)
        परत असत्य;

    {
        Mutex lock;
        साठी (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            जर ((gCommonICUDataArray[i] != nullptr) && (gCommonICUDataArray[i]->pHeader == pData->pHeader)) {
                /* The data pointer is already in the array. */
                found = सत्य;
                थांबा;
            }
        }
    }
    परत found;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
स्थिर UBool
setCommonICUData(UDataMemory *pData,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UErrorCode *pErr)
{
    UDataMemory  *newCommonData = UDataMemory_createNewInstance(pErr);
    int32_t i;
    UBool didUpdate = असत्य;
    जर (U_FAILURE(*pErr)) {
        परत असत्य;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_assign(newCommonData, pData);
    umtx_lock(nullptr);
    साठी (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        जर (gCommonICUDataArray[i] == nullptr) {
            gCommonICUDataArray[i] = newCommonData;
            didUpdate = सत्य;
            थांबा;
        } अन्यथा जर (gCommonICUDataArray[i]->pHeader == pData->pHeader) {
            /* The same data pointer is already in the array. */
            थांबा;
        }
    }
    umtx_unlock(nullptr);

    जर (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    जर (didUpdate) {
        ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } अन्यथा {
        uprv_free(newCommonData);
    }
    परत didUpdate;
}

#जर !defined(ICU_DATA_DIR_WINDOWS)

स्थिर UBool
setCommonICUDataPointer(स्थिरांक शून्य *pData, UBool /*warn*/, UErrorCode *pErrorCode) {
    UDataMemory tData;
    UDataMemory_init(&tData);
    UDataMemory_setData(&tData, pData);
    udata_checkCommonData(&tData, pErrorCode);
    परत setCommonICUData(&tData, असत्य, pErrorCode);
}

#endif

स्थिर स्थिरांक अक्षर *
findBasename(स्थिरांक अक्षर *path) {
    स्थिरांक अक्षर *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    जर(basename==nullptr) {
        परत path;
    } अन्यथा {
        परत basename+1;
    }
}

#ifdef UDATA_DEBUG
स्थिर स्थिरांक अक्षर *
packageNameFromPath(स्थिरांक अक्षर *path)
{
    जर((path == nullptr) || (*path == 0)) {
        परत U_ICUDATA_NAME;
    }

    path = findBasename(path);

    जर((path == nullptr) || (*path == 0)) {
        परत U_ICUDATA_NAME;
    }

    परत path;
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

प्रकारव्याख्या संरचना DataCacheElement {
    अक्षर          *name;
    UDataMemory   *item;
} DataCacheElement;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
स्थिर शून्य U_CALLCONV DataCacheElement_deleter(शून्य *pDCEl) {
    DataCacheElement* p = static_cast<DataCacheElement*>(pDCEl);
    udata_close(p->item);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

स्थिर शून्य U_CALLCONV udata_initHashTable(UErrorCode &err) {
    U_ASSERT(gCommonDataCache == nullptr);
    gCommonDataCache = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &err);
    जर (U_FAILURE(err)) {
       परत;
    }
    U_ASSERT(gCommonDataCache != nullptr);
    uhash_set_मूल्य_Deleter(gCommonDataCache, DataCacheElement_deleter);
    ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
स्थिर UHashtable *udata_getHashTable(UErrorCode &err) {
    umtx_initOnce(gCommonDataCacheInitOnce, &udata_initHashTable, err);
    परत gCommonDataCache;
}



स्थिर UDataMemory *udata_findCachedData(स्थिरांक अक्षर *path, UErrorCode &err)
{
    UHashtable        *htable;
    UDataMemory       *retVal = nullptr;
    DataCacheElement  *el;
    स्थिरांक अक्षर        *baseName;

    htable = udata_getHashTable(err);
    जर (U_FAILURE(err)) {
        परत nullptr;
    }

    baseName = findBasename(path);   /* Cache remembers only the base name, not the full path. */
    umtx_lock(nullptr);
    el = static_cast<DataCacheElement*>(uhash_get(htable, baseName));
    umtx_unlock(nullptr);
    जर (el != nullptr) {
        retVal = el->item;
    }
#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "Cache: [%s] -> %p\n", baseName, (शून्य*) retVal);
#endif
    परत retVal;
}


स्थिर UDataMemory *udata_cacheDataItem(स्थिरांक अक्षर *path, UDataMemory *item, UErrorCode *pErr) {
    DataCacheElement *newElement;
    स्थिरांक अक्षर       *baseName;
    int32_t           nameLen;
    UHashtable       *htable;
    DataCacheElement *old_मूल्य = nullptr;
    UErrorCode        subErr = U_ZERO_ERROR;

    htable = udata_getHashTable(*pErr);
    जर (U_FAILURE(*pErr)) {
        परत nullptr;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<DataCacheElement*>(uprv_malloc(आकार(DataCacheElement)));
    जर (newElement == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    newElement->item = UDataMemory_createNewInstance(pErr);
    जर (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        परत nullptr;
    }
    UDatamemory_assign(newElement->item, item);

    baseName = findBasename(path);
    nameLen = static_cast<int32_t>(uprv_strlen(baseName));
    newElement->name = static_cast<अक्षर*>(uprv_malloc(nameLen + 1));
    जर (newElement->name == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        परत nullptr;
    }
    uprv_strcpy(newElement->name, baseName);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(nullptr);
    old_मूल्य = static_cast<DataCacheElement*>(uhash_get(htable, path));
    जर (old_मूल्य != nullptr) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    अन्यथा {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(nullptr);

#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (शून्य*) newElement->item, u_errorName(subErr), (शून्य*) newElement->item->vFuncs);
#endif

    जर (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        परत old_मूल्य ? old_मूल्य->item : nullptr;
    }

    परत newElement->item;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

वर्ग UDataPathIterator
{
सार्वजनिक:
    UDataPathIterator(स्थिरांक अक्षर *path, स्थिरांक अक्षर *pkg,
                      स्थिरांक अक्षर *item, स्थिरांक अक्षर *suffix, UBool doCheckLastFour,
                      UErrorCode *pErrorCode);
    स्थिरांक अक्षर *next(UErrorCode *pErrorCode);

खाजगी:
    स्थिरांक अक्षर *path;                              /* working path (u_icudata_Dir) */
    स्थिरांक अक्षर *nextPath;                          /* path following this one */
    स्थिरांक अक्षर *basename;                          /* item's basename (icudt22e_mt.res)*/

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
UDataPathIterator::UDataPathIterator(स्थिरांक अक्षर *inPath, स्थिरांक अक्षर *pkg,
                                     स्थिरांक अक्षर *item, स्थिरांक अक्षर *inSuffix, UBool doCheckLastFour,
                                     UErrorCode *pErrorCode)
{
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "SUFFIX1=%s PATH=%s\n", inSuffix, inPath);
#endif
    /** Path **/
    जर(inPath == nullptr) {
        path = u_getDataDirectory();
    } अन्यथा {
        path = inPath;
    }

    /** Package **/
    जर(pkg != nullptr) {
      packageStub.append(U_FILE_SEP_CHAR, *pErrorCode).append(pkg, *pErrorCode);
#ifdef UDATA_DEBUG
      ख_लिखो(मानक_त्रुटि, "STUB=%s [%d]\n", packageStub.data(), packageStub.length());
#endif
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    जर(basename == item) {
        nextPath = path;
    } अन्यथा {
        itemPath.append(item, static_cast<int32_t>(basename - item), *pErrorCode);
        nextPath = itemPath.data();
    }
#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "SUFFIX=%s [%p]\n", inSuffix, (शून्य*) inSuffix);
#endif

    /** Suffix  **/
    जर(inSuffix != nullptr) {
        suffix = inSuffix;
    } अन्यथा {
        suffix = "";
    }

    checkLastFour = doCheckLastFour;

    /* pathBuffer will hold the output path strings returned by this iterator */

#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
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
स्थिरांक अक्षर *UDataPathIterator::next(UErrorCode *pErrorCode)
{
    जर(U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }

    स्थिरांक अक्षर *currentPath = nullptr;
    int32_t     pathLen = 0;
    स्थिरांक अक्षर *pathBasename;

    करा
    {
        जर( nextPath == nullptr ) {
            थांबा;
        }
        currentPath = nextPath;

        जर(nextPath == itemPath.data()) { /* we were processing item's path. */
            nextPath = path; /* start with regular path next tm. */
            pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
        } अन्यथा {
            /* fix up next for next time */
            nextPath = uprv_strchr(currentPath, U_PATH_SEP_CHAR);
            जर(nextPath == nullptr) {
                /* segment: entire path */
                pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
            } अन्यथा {
                /* segment: until next segment */
                pathLen = static_cast<int32_t>(nextPath - currentPath);
                /* skip divider */
                nextPath ++;
            }
        }

        जर(pathLen == 0) {
            सुरूठेवा;
        }

#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "rest of path (IDD) = %s\n", currentPath);
        ख_लिखो(मानक_त्रुटि, "                     ");
        { 
            int32_t qqq;
            साठी(qqq=0;qqq<pathLen;qqq++)
            {
                ख_लिखो(मानक_त्रुटि, " ");
            }

            ख_लिखो(मानक_त्रुटि, "^\n");
        }
#endif
        pathBuffer.clear().append(currentPath, pathLen, *pErrorCode);

        /* check for .dat files */
        pathBasename = findBasename(pathBuffer.data());

        जर(checkLastFour && 
           (pathLen>=4) &&
           uprv_strncmp(pathBuffer.data() +(pathLen-4), suffix.data(), 4)==0 && /* suffix matches */
           uprv_strncmp(findBasename(pathBuffer.data()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(pathBasename)==(basenameLen+4)) { /* base+suffix = full len */

#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "Have %s file on the path: %s\n", suffix.data(), pathBuffer.data());
#endif
            /* do nothing */
        }
        अन्यथा 
        {       /* regular dir path */
            जर(pathBuffer[pathLen-1] != U_FILE_SEP_CHAR) {
                जर((pathLen>=4) &&
                   uprv_strncmp(pathBuffer.data()+(pathLen-4), ".dat", 4) == 0)
                {
#ifdef UDATA_DEBUG
                    ख_लिखो(मानक_त्रुटि, "skipping non-directory .dat file %s\n", pathBuffer.data());
#endif
                    सुरूठेवा;
                }

                /* Check if it is a directory with the same name as our package */
                जर(!packageStub.isEmpty() &&
                   (pathLen > packageStub.length()) &&
                   !uprv_strcmp(pathBuffer.data() + pathLen - packageStub.length(), packageStub.data())) {
#ifdef UDATA_DEBUG
                  ख_लिखो(मानक_त्रुटि, "Found stub %s (will add package %s of len %d)\n", packageStub.data(), basename, basenameLen);
#endif
                  pathBuffer.truncate(pathLen - packageStub.length());
                }
                pathBuffer.append(U_FILE_SEP_CHAR, *pErrorCode);
            }

            /* + basename */
            pathBuffer.append(packageStub.data()+1, packageStub.length()-1, *pErrorCode);

            जर (!suffix.empty())  /* tack on suffix */
            {
                जर (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    pathBuffer.ensureEndsWithFileSeparator(*pErrorCode);
                }
                pathBuffer.append(suffix, *pErrorCode);
            }
        }

#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, " -->  %s\n", pathBuffer.data());
#endif

        परत pathBuffer.data();

    } पर्यंत(path);

    /* fell way off the end */
    परत nullptr;
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
#जर !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
बाह्य "C" स्थिरांक DataHeader U_DATA_API U_ICUDATA_ENTRY_POINT;
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
स्थिर UDataMemory *
openCommonData(स्थिरांक अक्षर *path,          /*  Path from OpenChoice?          */
               int32_t commonDataIndex,   /*  ICU Data (index >= 0) if path == nullptr */
               UErrorCode *pErrorCode)
{
    UDataMemory tData;
    स्थिरांक अक्षर *pathBuffer;
    स्थिरांक अक्षर *inBasename;

    जर (U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }

    UDataMemory_init(&tData);

    /* ??????? TODO revisit this */ 
    जर (commonDataIndex >= 0) {
        /* "mini-cache" for common ICU data */
        जर(commonDataIndex >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            परत nullptr;
        }
        {
            Mutex lock;
            जर(gCommonICUDataArray[commonDataIndex] != nullptr) {
                परत gCommonICUDataArray[commonDataIndex];
            }
#जर !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            साठी(i = 0; i < commonDataIndex; ++i) {
                जर(gCommonICUDataArray[i]->pHeader == &U_ICUDATA_ENTRY_POINT) {
                    /* The linked-in data is already in the list. */
                    परत nullptr;
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
#जर !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        setCommonICUDataPointer(&U_ICUDATA_ENTRY_POINT, असत्य, pErrorCode);
        {
            Mutex lock;
            परत gCommonICUDataArray[commonDataIndex];
        }
#endif
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    inBasename = findBasename(path);
#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "inBasename = %s\n", inBasename);
#endif

    जर(*inBasename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "ocd: no basename in %s, bailing.\n", path);
#endif
        जर (U_SUCCESS(*pErrorCode)) {
            *pErrorCode=U_FILE_ACCESS_ERROR;
        }
        परत nullptr;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UDataMemory  *dataToReturn = udata_findCachedData(inBasename, *pErrorCode);
    जर (dataToReturn != nullptr || U_FAILURE(*pErrorCode)) {
        परत dataToReturn;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UDataPathIterator iter(u_getDataDirectory(), inBasename, path, ".dat", सत्य, pErrorCode);

    पर्यंत ((UDataMemory_isLoaded(&tData)==असत्य) && (pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "ocd: trying path %s - ", pathBuffer);
#endif
        uprv_mapFile(&tData, pathBuffer, pErrorCode);
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "%s\n", UDataMemory_isLoaded(&tData)?"LOADED":"not loaded");
#endif
    }
    जर (U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }

    जर (U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }
    जर (!UDataMemory_isLoaded(&tData)) {
        /* no common data */
        *pErrorCode=U_FILE_ACCESS_ERROR;
        परत nullptr;
    }

    /* we have mapped a file, check its header */
    udata_checkCommonData(&tData, pErrorCode);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    परत udata_cacheDataItem(inBasename, &tData, pErrorCode);
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
स्थिर UBool extendICUData(UErrorCode *pErr)
{
    UDataMemory   *pData;
    UDataMemory   copyPData;
    UBool         didUpdate = असत्य;

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
#जर MAP_IMPLEMENTATION==MAP_STDIO
    स्थिर UMutex extendICUDataMutex;
    umtx_lock(&extendICUDataMutex);
#endif
    जर(!umtx_loadAcquire(gHaveTriedToLoadCommonData)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        pData = openCommonData(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UDataMemory_init(&copyPData);
       जर(pData != nullptr) {
          UDatamemory_assign(&copyPData, pData);
          copyPData.map = nullptr;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = nullptr; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              setCommonICUData(&copyPData,/*  The new common data.                                */
                       असत्य,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(gHaveTriedToLoadCommonData, 1);
    }

    didUpdate = findCommonICUDataByName(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#जर MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#endif
    परत didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI शून्य U_EXPORT2
udata_setCommonData(स्थिरांक शून्य *data, UErrorCode *pErrorCode) {
    UDataMemory dataMemory;

    जर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        परत;
    }

    जर(data==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }

    /* set the data pointer and test for validity */
    UDataMemory_init(&dataMemory);
    UDataMemory_setData(&dataMemory, data);
    udata_checkCommonData(&dataMemory, pErrorCode);
    जर (U_FAILURE(*pErrorCode)) {परत;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    setCommonICUData(&dataMemory, सत्य, pErrorCode);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI शून्य U_EXPORT2
udata_setAppData(स्थिरांक अक्षर *path, स्थिरांक शून्य *data, UErrorCode *err)
{
    UDataMemory     udm;

    जर(err==nullptr || U_FAILURE(*err)) {
        परत;
    }
    जर(data==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        परत;
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
स्थिर UDataMemory *
checkDataItem
(
 स्थिरांक DataHeader         *pHeader,         /* The data item to be checked.                */
 UDataMemoryIsAcceptable  *isAcceptable,    /* App's call-back function                    */
 शून्य                     *context,         /*   pass-thru param for above.                */
 स्थिरांक अक्षर               *type,            /*   pass-thru param for above.                */
 स्थिरांक अक्षर               *name,            /*   pass-thru param for above.                */
 UErrorCode               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UErrorCode               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UDataMemory  *rDataMem = nullptr;          /* the new UDataMemory, to be returned.        */

    जर (U_FAILURE(*fatalErr)) {
        परत nullptr;
    }

    जर(pHeader->dataHeader.magic1==0xda &&
        pHeader->dataHeader.magic2==0x27 &&
        (isAcceptable==nullptr || isAcceptable(context, type, name, &pHeader->info))
    ) {
        rDataMem=UDataMemory_createNewInstance(fatalErr);
        जर (U_FAILURE(*fatalErr)) {
            परत nullptr;
        }
        rDataMem->pHeader = pHeader;
    } अन्यथा {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    परत rDataMem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
स्थिर UDataMemory *doLoadFromIndividualFiles(स्थिरांक अक्षर *pkgName, 
        स्थिरांक अक्षर *dataPath, स्थिरांक अक्षर *tocEntryPathSuffix,
            /* following arguments are the same as doOpenChoice itself */
            स्थिरांक अक्षर *path, स्थिरांक अक्षर *type, स्थिरांक अक्षर *name,
             UDataMemoryIsAcceptable *isAcceptable, शून्य *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    स्थिरांक अक्षर         *pathBuffer;
    UDataMemory         dataMemory;
    UDataMemory *pEntryData;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UDataPathIterator iter(dataPath, pkgName, path, tocEntryPathSuffix, असत्य, pErrorCode);

    पर्यंत ((pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "UDATA: trying individual file %s\n", pathBuffer);
#endif
        जर (uprv_mapFile(&dataMemory, pathBuffer, pErrorCode))
        {
            pEntryData = checkDataItem(dataMemory.pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
            जर (pEntryData != nullptr) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                pEntryData->mapAddr = dataMemory.mapAddr;
                pEntryData->map     = dataMemory.map;
                pEntryData->length  = dataMemory.length;

#ifdef UDATA_DEBUG
                ख_लिखो(मानक_त्रुटि, "** Mapped file: %s\n", pathBuffer);
#endif
                परत pEntryData;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&dataMemory);

            /* If we had a nasty error, bail out completely.  */
            जर (U_FAILURE(*pErrorCode)) {
                परत nullptr;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *subErrorCode=U_INVALID_FORMAT_ERROR;
        }
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "%s\n", UDataMemory_isLoaded(&dataMemory)?"LOADED":"not loaded");
#endif
    }
    परत nullptr;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
स्थिर UDataMemory *doLoadFromCommonData(UBool isICUData, स्थिरांक अक्षर * /*pkgName*/, 
        स्थिरांक अक्षर * /*dataPath*/, स्थिरांक अक्षर * /*tocEntryPathSuffix*/, स्थिरांक अक्षर *tocEntryName,
            /* following arguments are the same as doOpenChoice itself */
            स्थिरांक अक्षर *path, स्थिरांक अक्षर *type, स्थिरांक अक्षर *name,
             UDataMemoryIsAcceptable *isAcceptable, शून्य *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    UDataMemory        *pEntryData;
    स्थिरांक DataHeader   *pHeader;
    UDataMemory        *pCommonData;
    int32_t            commonDataIndex;
    UBool              checkedExtendedICUData = असत्य;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    साठी (commonDataIndex = isICUData ? 0 : -1;;) {
        pCommonData=openCommonData(path, commonDataIndex, subErrorCode); /** search for pkg **/

        जर(U_SUCCESS(*subErrorCode) && pCommonData!=nullptr) {
            int32_t length;

            /* look up the data piece in the common data */
            pHeader=pCommonData->vFuncs->Lookup(pCommonData, tocEntryName, &length, subErrorCode);
#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: pHeader=%p - %s\n", tocEntryName, (शून्य*) pHeader, u_errorName(*subErrorCode));
#endif

            जर(pHeader!=nullptr) {
                pEntryData = checkDataItem(pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
#ifdef UDATA_DEBUG
                ख_लिखो(मानक_त्रुटि, "pEntryData=%p\n", (शून्य*) pEntryData);
#endif
                जर (U_FAILURE(*pErrorCode)) {
                    परत nullptr;
                }
                जर (pEntryData != nullptr) {
                    pEntryData->length = length;
                    परत pEntryData;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        जर (*subErrorCode == U_MEMORY_ALLOCATION_ERROR) {
            *pErrorCode = *subErrorCode;
            परत nullptr;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        जर (!isICUData) {
            परत nullptr;
        } अन्यथा जर (pCommonData != nullptr) {
            ++commonDataIndex;  /* try the next data package */
        } अन्यथा जर ((!checkedExtendedICUData) && extendICUData(subErrorCode)) {
            checkedExtendedICUData = सत्य;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } अन्यथा {
            परत nullptr;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
स्थिर UBool isTimeZoneFile(स्थिरांक अक्षर *name, स्थिरांक अक्षर *type) {
    परत ((uprv_strcmp(type, "res") == 0) &&
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
स्थिर UDataMemory *
doOpenChoice(स्थिरांक अक्षर *path, स्थिरांक अक्षर *type, स्थिरांक अक्षर *name,
             UDataMemoryIsAcceptable *isAcceptable, शून्य *context,
             UErrorCode *pErrorCode)
{
    UDataMemory         *retVal = nullptr;

    स्थिरांक अक्षर         *dataPath;

    int32_t             tocEntrySuffixIndex;
    स्थिरांक अक्षर         *tocEntryPathSuffix;
    UErrorCode          subErrorCode=U_ZERO_ERROR;
    स्थिरांक अक्षर         *वृक्ष_Char;

    UBool               isICUData = असत्य;


    FileTracer::traceOpen(path, type, name);


    /* Is this path ICU data? */
    जर(path == nullptr ||
       !म_भेद(path, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(path, U_ICUDATA_NAME U_वृक्ष_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_वृक्ष_SEPARATOR_STRING)) ||  
       !uprv_strncmp(path, U_ICUDATA_ALIAS U_वृक्ष_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_वृक्ष_SEPARATOR_STRING))) {
      isICUData = सत्य;
    }

#जर (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString altSepPath;
    जर(path) {
        जर(uprv_strchr(path,U_FILE_ALT_SEP_CHAR) != nullptr) {
            altSepPath.append(path, *pErrorCode);
            अक्षर *p;
            पर्यंत ((p = uprv_strchr(altSepPath.data(), U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
#जर defined (UDATA_DEBUG)
            ख_लिखो(मानक_त्रुटि, "Changed path from [%s] to [%s]\n", path, altSepPath.data());
#endif
            path = altSepPath.data();
        }
    }
#endif

    CharString tocEntryName; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString tocEntryPath; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString वृक्ष_Name;

    /* ======= Set up strings */
    जर(path==nullptr) {
        pkgName.append(U_ICUDATA_NAME, *pErrorCode);
    } अन्यथा {
        स्थिरांक अक्षर *pkg;
        स्थिरांक अक्षर *first;
        pkg = uprv_strrchr(path, U_FILE_SEP_CHAR);
        first = uprv_strchr(path, U_FILE_SEP_CHAR);
        जर(uprv_pathIsAbsolute(path) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            जर(pkg) {
                pkgName.append(pkg+1, *pErrorCode);
            } अन्यथा {
                pkgName.append(path, *pErrorCode);
            }
        } अन्यथा {
            वृक्ष_Char = uprv_strchr(path, U_वृक्ष_SEPARATOR);
            जर(वृक्ष_Char) { 
                वृक्ष_Name.append(वृक्ष_Char+1, *pErrorCode); /* following '-' */
                जर(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } अन्यथा {
                    pkgName.append(path, static_cast<int32_t>(वृक्ष_Char - path), *pErrorCode);
                    जर (first == nullptr) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        path = pkgName.data();
                    }
                }
            } अन्यथा {
                जर(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } अन्यथा {
                    pkgName.append(path, *pErrorCode);
                }
            }
        }
    }

#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, " P=%s T=%s\n", pkgName.data(), वृक्ष_Name.data());
#endif

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    tocEntryName.append(pkgName, *pErrorCode);
    tocEntryPath.append(pkgName, *pErrorCode);
    tocEntrySuffixIndex = tocEntryName.length();

    जर(!वृक्ष_Name.isEmpty()) {
        tocEntryName.append(U_वृक्ष_ENTRY_SEP_CHAR, *pErrorCode).append(वृक्ष_Name, *pErrorCode);
        tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(वृक्ष_Name, *pErrorCode);
    }

    tocEntryName.append(U_वृक्ष_ENTRY_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    जर(type!=nullptr && *type!=0) {
        tocEntryName.append(".", *pErrorCode).append(type, *pErrorCode);
        tocEntryPath.append(".", *pErrorCode).append(type, *pErrorCode);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    tocEntryPathSuffix = tocEntryPath.data() + tocEntrySuffixIndex + 1; /* suffix starts here */

#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, " tocEntryName = %s\n", tocEntryName.data());
    ख_लिखो(मानक_त्रुटि, " tocEntryPath = %s\n", tocEntryPath.data());
#endif

#जर !defined(ICU_DATA_DIR_WINDOWS)
    जर(path == nullptr) {
        path = COMMON_DATA_NAME; /* "icudt26e" */
    }
#अन्यथा
    // When using the Windows system data, we expects only a single data file.
    path = COMMON_DATA_NAME; /* "icudt26e" */
#endif

    /************************ Begin loop looking for ind. files ***************/
#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "IND: inBasename = %s, pkg=%s\n", "(n/a)", packageNameFromPath(path));
#endif

    /* End of dealing with a null basename */
    dataPath = u_getDataDirectory();

    /****    Time zone individual files override  */
    जर (isICUData && type != nullptr && isTimeZoneFile(name, type)) {
        स्थिरांक अक्षर *tzFilesDir = u_getTimeZoneFilesDirectory(pErrorCode);
        जर (tzFilesDir[0] != 0) {
#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#endif
            retVal = doLoadFromIndividualFiles(/* pkgName.data() */ "", tzFilesDir, tocEntryPathSuffix,
                            /* path */ "", type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            जर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                परत retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    जर(gDataFileAccess == UDATA_PACKAGES_FIRST) {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#endif
        /* #2 */
        retVal = doLoadFromCommonData(isICUData, 
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        जर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            परत retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    जर((gDataFileAccess==UDATA_PACKAGES_FIRST) ||
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying individual files\n");
#endif
        /* Check to make sure that there is a dataPath to iterate over */
        जर ((dataPath && *dataPath) || !isICUData) {
            retVal = doLoadFromIndividualFiles(pkgName.data(), dataPath, tocEntryPathSuffix,
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            जर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                परत retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    जर((gDataFileAccess==UDATA_ONLY_PACKAGES) || 
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        जर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            परत retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    जर(gDataFileAccess==UDATA_NO_FILES) {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying common data (UDATA_NO_FILES)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), "", tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        जर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            परत retVal;
        }
    }

    /* data not found */
    जर(U_SUCCESS(*pErrorCode)) {
        जर(U_SUCCESS(subErrorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } अन्यथा {
            /* entry point not found or rejected */
            *pErrorCode=subErrorCode;
        }
    }
    परत retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(स्थिरांक अक्षर *path, स्थिरांक अक्षर *type, स्थिरांक अक्षर *name,
           UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  ख_लिखो(मानक_त्रुटि, "udata_open(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
    ख_साफ(मानक_त्रुटि);
#endif

    जर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        परत nullptr;
    } अन्यथा जर(name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    } अन्यथा {
        परत doOpenChoice(path, type, name, nullptr, nullptr, pErrorCode);
    }
}



U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(स्थिरांक अक्षर *path, स्थिरांक अक्षर *type, स्थिरांक अक्षर *name,
                 UDataMemoryIsAcceptable *isAcceptable, शून्य *context,
                 UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  ख_लिखो(मानक_त्रुटि, "udata_openChoice(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
#endif

    जर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        परत nullptr;
    } अन्यथा जर(name==nullptr || *name==0 || isAcceptable==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    } अन्यथा {
        परत doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}



U_CAPI शून्य U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    जर(pInfo!=nullptr) {
        जर(pData!=nullptr && pData->pHeader!=nullptr) {
            स्थिरांक UDataInfo *info=&pData->pHeader->info;
            uint16_t dataInfoSize=udata_getInfoSize(info);
            जर(pInfo->size>dataInfoSize) {
                pInfo->size=dataInfoSize;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (स्थिरांक uint16_t *)info+1, pInfo->size-2);
            जर(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reservedWord;
                pInfo->reservedWord=(uint16_t)((x<<8)|(x>>8));
            }
        } अन्यथा {
            pInfo->size=0;
        }
    }
}


U_CAPI शून्य U_EXPORT2 udata_setFileAccess(UDataFileAccess access, UErrorCode * /*status*/)
{
    // Note: this function is documented as not thread safe.
    gDataFileAccess = access;
}
