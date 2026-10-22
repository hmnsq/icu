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

#include "unicode/utypes.h"  /* U_PLATFORM etc. */

#ifdef __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#endif

#include "unicode/putil.h"
#include "unicode/udata.h"
#include "unicode/uversion.h"
#include "charstr.h"
#include "cmemory.h"
#include "cstring.h"
#include "mutex.h"
#include "putilimp.h"
#include "restrace.h"
#include "uassert.h"
#include "ucln_cmn.h"
#include "ucmndata.h"
#include "udatamem.h"
#include "uhash.h"
#include "umapfile.h"
#include "umutex.h"

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

#ប្រសិនបើ defined(UDATA_DEBUG)
#   include <stdio.h>
#endif

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
static UDataMemory *udata_findCachedData(const var_na *path, UErrorCode &err);

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
static UDataMemory *gCommonICUDataArray[10] = { nullptr };   // Access protected by icu global mutex.

static u_atomic_int32_t gHaveTriedToLoadCommonData {0};  //  See extendICUData().

static UHashtable  *gCommonDataCache = nullptr;  /* Global hash table of opened ICU data files.  */
static icu::UInitOnce gCommonDataCacheInitOnce {};

#ប្រសិនបើ !defined(ICU_DATA_DIR_WINDOWS)
static UDataFileAccess  gDataFileAccess = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#បើមិនដូច្នេះ
// If we are using the Windows data directory, then look in one spot only.
static UDataFileAccess  gDataFileAccess = UDATA_NO_FILES;
#endif

static UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    ប្រសិនបើ (gCommonDataCache) {             /* Delete the cache of user data mappings.  */
        uhash_close(gCommonDataCache);  /*   Table owns the contents, and will delete them. */
        gCommonDataCache = nullptr;        /*   Cleanup is not thread safe.                */
    }
    gCommonDataCacheInitOnce.reset();

    សម្រាប់ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != nullptr; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = nullptr;
    }
    gHaveTriedToLoadCommonData = 0;

    ត្រឡប់ true;                   /* Everything was cleaned up */
}

static UBool U_CALLCONV
findCommonICUDataByName(const var_na *inBasename, UErrorCode &err)
{
    UBool found = false;
    int32_t i;

    UDataMemory  *pData = udata_findCachedData(inBasename, err);
    ប្រសិនបើ (U_FAILURE(err) || pData == nullptr)
        ត្រឡប់ false;

    {
        Mutex lock;
        សម្រាប់ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            ប្រសិនបើ ((gCommonICUDataArray[i] != nullptr) && (gCommonICUDataArray[i]->pHeader == pData->pHeader)) {
                /* The data pointer is already in the array. */
                found = true;
                bha_nga;
            }
        }
    }
    ត្រឡប់ found;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
static UBool
setCommonICUData(UDataMemory *pData,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UErrorCode *pErr)
{
    UDataMemory  *newCommonData = UDataMemory_createNewInstance(pErr);
    int32_t i;
    UBool didUpdate = false;
    ប្រសិនបើ (U_FAILURE(*pErr)) {
        ត្រឡប់ false;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_assign(newCommonData, pData);
    umtx_lock(nullptr);
    សម្រាប់ (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        ប្រសិនបើ (gCommonICUDataArray[i] == nullptr) {
            gCommonICUDataArray[i] = newCommonData;
            didUpdate = true;
            bha_nga;
        } បើមិនដូច្នេះ ប្រសិនបើ (gCommonICUDataArray[i]->pHeader == pData->pHeader) {
            /* The same data pointer is already in the array. */
            bha_nga;
        }
    }
    umtx_unlock(nullptr);

    ប្រសិនបើ (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    ប្រសិនបើ (didUpdate) {
        ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } បើមិនដូច្នេះ {
        uprv_free(newCommonData);
    }
    ត្រឡប់ didUpdate;
}

#ប្រសិនបើ !defined(ICU_DATA_DIR_WINDOWS)

static UBool
setCommonICUDataPointer(const void *pData, UBool /*warn*/, UErrorCode *pErrorCode) {
    UDataMemory tData;
    UDataMemory_init(&tData);
    UDataMemory_setData(&tData, pData);
    udata_checkCommonData(&tData, pErrorCode);
    ត្រឡប់ setCommonICUData(&tData, false, pErrorCode);
}

#endif

static const var_na *
findBasename(const var_na *path) {
    const var_na *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    ប្រសិនបើ(basename==nullptr) {
        ត្រឡប់ path;
    } បើមិនដូច្នេះ {
        ត្រឡប់ basename+1;
    }
}

#ifdef UDATA_DEBUG
static const var_na *
packageNameFromPath(const var_na *path)
{
    ប្រសិនបើ((path == nullptr) || (*path == 0)) {
        ត្រឡប់ U_ICUDATA_NAME;
    }

    path = findBasename(path);

    ប្រសិនបើ((path == nullptr) || (*path == 0)) {
        ត្រឡប់ U_ICUDATA_NAME;
    }

    ត្រឡប់ path;
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

typedef sa_nracanaa DataCacheElement {
    var_na          *name;
    UDataMemory   *item;
} DataCacheElement;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
static void U_CALLCONV DataCacheElement_deleter(void *pDCEl) {
    DataCacheElement* p = static_cast<DataCacheElement*>(pDCEl);
    udata_close(p->item);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

static void U_CALLCONV udata_initHashTable(UErrorCode &err) {
    U_ASSERT(gCommonDataCache == nullptr);
    gCommonDataCache = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &err);
    ប្រសិនបើ (U_FAILURE(err)) {
       ត្រឡប់;
    }
    U_ASSERT(gCommonDataCache != nullptr);
    uhash_setValueDeleter(gCommonDataCache, DataCacheElement_deleter);
    ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
static UHashtable *udata_getHashTable(UErrorCode &err) {
    umtx_initOnce(gCommonDataCacheInitOnce, &udata_initHashTable, err);
    ត្រឡប់ gCommonDataCache;
}



static UDataMemory *udata_findCachedData(const var_na *path, UErrorCode &err)
{
    UHashtable        *htable;
    UDataMemory       *retVal = nullptr;
    DataCacheElement  *el;
    const var_na        *baseName;

    htable = udata_getHashTable(err);
    ប្រសិនបើ (U_FAILURE(err)) {
        ត្រឡប់ nullptr;
    }

    baseName = findBasename(path);   /* Cache remembers only the base name, not the full path. */
    umtx_lock(nullptr);
    el = static_cast<DataCacheElement*>(uhash_get(htable, baseName));
    umtx_unlock(nullptr);
    ប្រសិនបើ (el != nullptr) {
        retVal = el->item;
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", baseName, (void*) retVal);
#endif
    ត្រឡប់ retVal;
}


static UDataMemory *udata_cacheDataItem(const var_na *path, UDataMemory *item, UErrorCode *pErr) {
    DataCacheElement *newElement;
    const var_na       *baseName;
    int32_t           nameLen;
    UHashtable       *htable;
    DataCacheElement *oldValue = nullptr;
    UErrorCode        subErr = U_ZERO_ERROR;

    htable = udata_getHashTable(*pErr);
    ប្រសិនបើ (U_FAILURE(*pErr)) {
        ត្រឡប់ nullptr;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<DataCacheElement*>(uprv_malloc(parimaa_na(DataCacheElement)));
    ប្រសិនបើ (newElement == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ nullptr;
    }
    newElement->item = UDataMemory_createNewInstance(pErr);
    ប្រសិនបើ (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        ត្រឡប់ nullptr;
    }
    UDatamemory_assign(newElement->item, item);

    baseName = findBasename(path);
    nameLen = static_cast<int32_t>(uprv_strlen(baseName));
    newElement->name = static_cast<var_na*>(uprv_malloc(nameLen + 1));
    ប្រសិនបើ (newElement->name == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        ត្រឡប់ nullptr;
    }
    uprv_strcpy(newElement->name, baseName);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(nullptr);
    oldValue = static_cast<DataCacheElement*>(uhash_get(htable, path));
    ប្រសិនបើ (oldValue != nullptr) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    បើមិនដូច្នេះ {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(nullptr);

#ifdef UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (void*) newElement->item, u_errorName(subErr), (void*) newElement->item->vFuncs);
#endif

    ប្រសិនបើ (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        ត្រឡប់ oldValue ? oldValue->item : nullptr;
    }

    ត្រឡប់ newElement->item;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

class UDataPathIterator
{
public:
    UDataPathIterator(const var_na *path, const var_na *pkg,
                      const var_na *item, const var_na *suffix, UBool doCheckLastFour,
                      UErrorCode *pErrorCode);
    const var_na *next(UErrorCode *pErrorCode);

private:
    const var_na *path;                              /* working path (u_icudata_Dir) */
    const var_na *nextPath;                          /* path following this one */
    const var_na *basename;                          /* item's basename (icudt22e_mt.res)*/

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
UDataPathIterator::UDataPathIterator(const var_na *inPath, const var_na *pkg,
                                     const var_na *item, const var_na *inSuffix, UBool doCheckLastFour,
                                     UErrorCode *pErrorCode)
{
#ifdef UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", inSuffix, inPath);
#endif
    /** Path **/
    ប្រសិនបើ(inPath == nullptr) {
        path = u_getDataDirectory();
    } បើមិនដូច្នេះ {
        path = inPath;
    }

    /** Package **/
    ប្រសិនបើ(pkg != nullptr) {
      packageStub.append(U_FILE_SEP_CHAR, *pErrorCode).append(pkg, *pErrorCode);
#ifdef UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.data(), packageStub.length());
#endif
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    ប្រសិនបើ(basename == item) {
        nextPath = path;
    } បើមិនដូច្នេះ {
        itemPath.append(item, static_cast<int32_t>(basename - item), *pErrorCode);
        nextPath = itemPath.data();
    }
#ifdef UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", inSuffix, (void*) inSuffix);
#endif

    /** Suffix  **/
    ប្រសិនបើ(inSuffix != nullptr) {
        suffix = inSuffix;
    } បើមិនដូច្នេះ {
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
const var_na *UDataPathIterator::next(UErrorCode *pErrorCode)
{
    ប្រសិនបើ(U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }

    const var_na *currentPath = nullptr;
    int32_t     pathLen = 0;
    const var_na *pathBasename;

    do
    {
        ប្រសិនបើ( nextPath == nullptr ) {
            bha_nga;
        }
        currentPath = nextPath;

        ប្រសិនបើ(nextPath == itemPath.data()) { /* we were processing item's path. */
            nextPath = path; /* start with regular path next tm. */
            pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
        } បើមិនដូច្នេះ {
            /* fix up next for next time */
            nextPath = uprv_strchr(currentPath, U_PATH_SEP_CHAR);
            ប្រសិនបើ(nextPath == nullptr) {
                /* segment: entire path */
                pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
            } បើមិនដូច្នេះ {
                /* segment: until next segment */
                pathLen = static_cast<int32_t>(nextPath - currentPath);
                /* skip divider */
                nextPath ++;
            }
        }

        ប្រសិនបើ(pathLen == 0) {
            continue;
        }

#ifdef UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", currentPath);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            សម្រាប់(qqq=0;qqq<pathLen;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#endif
        pathBuffer.clear().append(currentPath, pathLen, *pErrorCode);

        /* check for .dat files */
        pathBasename = findBasename(pathBuffer.data());

        ប្រសិនបើ(checkLastFour && 
           (pathLen>=4) &&
           uprv_strncmp(pathBuffer.data() +(pathLen-4), suffix.data(), 4)==0 && /* suffix matches */
           uprv_strncmp(findBasename(pathBuffer.data()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(pathBasename)==(basenameLen+4)) { /* base+suffix = full len */

#ifdef UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.data(), pathBuffer.data());
#endif
            /* do nothing */
        }
        បើមិនដូច្នេះ 
        {       /* regular dir path */
            ប្រសិនបើ(pathBuffer[pathLen-1] != U_FILE_SEP_CHAR) {
                ប្រសិនបើ((pathLen>=4) &&
                   uprv_strncmp(pathBuffer.data()+(pathLen-4), ".dat", 4) == 0)
                {
#ifdef UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", pathBuffer.data());
#endif
                    continue;
                }

                /* Check if it is a directory with the same name as our package */
                ប្រសិនបើ(!packageStub.isEmpty() &&
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

            ប្រសិនបើ (!suffix.empty())  /* tack on suffix */
            {
                ប្រសិនបើ (suffix.length() > 4) {
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

        ត្រឡប់ pathBuffer.data();

    } ខណៈពេល(path);

    /* fell way off the end */
    ត្រឡប់ nullptr;
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
#ប្រសិនបើ !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
extern "C" const DataHeader U_DATA_API U_ICUDATA_ENTRY_POINT;
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
static UDataMemory *
openCommonData(const var_na *path,          /*  Path from OpenChoice?          */
               int32_t commonDataIndex,   /*  ICU Data (index >= 0) if path == nullptr */
               UErrorCode *pErrorCode)
{
    UDataMemory tData;
    const var_na *pathBuffer;
    const var_na *inBasename;

    ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }

    UDataMemory_init(&tData);

    /* ??????? TODO revisit this */ 
    ប្រសិនបើ (commonDataIndex >= 0) {
        /* "mini-cache" for common ICU data */
        ប្រសិនបើ(commonDataIndex >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            ត្រឡប់ nullptr;
        }
        {
            Mutex lock;
            ប្រសិនបើ(gCommonICUDataArray[commonDataIndex] != nullptr) {
                ត្រឡប់ gCommonICUDataArray[commonDataIndex];
            }
#ប្រសិនបើ !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            សម្រាប់(i = 0; i < commonDataIndex; ++i) {
                ប្រសិនបើ(gCommonICUDataArray[i]->pHeader == &U_ICUDATA_ENTRY_POINT) {
                    /* The linked-in data is already in the list. */
                    ត្រឡប់ nullptr;
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
#ប្រសិនបើ !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        setCommonICUDataPointer(&U_ICUDATA_ENTRY_POINT, false, pErrorCode);
        {
            Mutex lock;
            ត្រឡប់ gCommonICUDataArray[commonDataIndex];
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

    ប្រសិនបើ(*inBasename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", path);
#endif
        ប្រសិនបើ (U_SUCCESS(*pErrorCode)) {
            *pErrorCode=U_FILE_ACCESS_ERROR;
        }
        ត្រឡប់ nullptr;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UDataMemory  *dataToReturn = udata_findCachedData(inBasename, *pErrorCode);
    ប្រសិនបើ (dataToReturn != nullptr || U_FAILURE(*pErrorCode)) {
        ត្រឡប់ dataToReturn;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UDataPathIterator iter(u_getDataDirectory(), inBasename, path, ".dat", true, pErrorCode);

    ខណៈពេល ((UDataMemory_isLoaded(&tData)==false) && (pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", pathBuffer);
#endif
        uprv_mapFile(&tData, pathBuffer, pErrorCode);
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&tData)?"LOADED":"not loaded");
#endif
    }
    ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ (!UDataMemory_isLoaded(&tData)) {
        /* no common data */
        *pErrorCode=U_FILE_ACCESS_ERROR;
        ត្រឡប់ nullptr;
    }

    /* we have mapped a file, check its header */
    udata_checkCommonData(&tData, pErrorCode);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    ត្រឡប់ udata_cacheDataItem(inBasename, &tData, pErrorCode);
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
static UBool extendICUData(UErrorCode *pErr)
{
    UDataMemory   *pData;
    UDataMemory   copyPData;
    UBool         didUpdate = false;

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
#ប្រសិនបើ MAP_IMPLEMENTATION==MAP_STDIO
    static UMutex extendICUDataMutex;
    umtx_lock(&extendICUDataMutex);
#endif
    ប្រសិនបើ(!umtx_loadAcquire(gHaveTriedToLoadCommonData)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        pData = openCommonData(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UDataMemory_init(&copyPData);
       ប្រសិនបើ(pData != nullptr) {
          UDatamemory_assign(&copyPData, pData);
          copyPData.map = nullptr;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = nullptr; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              setCommonICUData(&copyPData,/*  The new common data.                                */
                       false,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(gHaveTriedToLoadCommonData, 1);
    }

    didUpdate = findCommonICUDataByName(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#ប្រសិនបើ MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#endif
    ត្រឡប់ didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI void U_EXPORT2
udata_setCommonData(const void *data, UErrorCode *pErrorCode) {
    UDataMemory dataMemory;

    ប្រសិនបើ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ត្រឡប់;
    }

    ប្រសិនបើ(data==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់;
    }

    /* set the data pointer and test for validity */
    UDataMemory_init(&dataMemory);
    UDataMemory_setData(&dataMemory, data);
    udata_checkCommonData(&dataMemory, pErrorCode);
    ប្រសិនបើ (U_FAILURE(*pErrorCode)) {ត្រឡប់;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    setCommonICUData(&dataMemory, true, pErrorCode);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI void U_EXPORT2
udata_setAppData(const var_na *path, const void *data, UErrorCode *err)
{
    UDataMemory     udm;

    ប្រសិនបើ(err==nullptr || U_FAILURE(*err)) {
        ត្រឡប់;
    }
    ប្រសិនបើ(data==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់;
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
static UDataMemory *
checkDataItem
(
 const DataHeader         *pHeader,         /* The data item to be checked.                */
 UDataMemoryIsAcceptable  *isAcceptable,    /* App's call-back function                    */
 void                     *context,         /*   pass-thru param for above.                */
 const var_na               *type,            /*   pass-thru param for above.                */
 const var_na               *name,            /*   pass-thru param for above.                */
 UErrorCode               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UErrorCode               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UDataMemory  *rDataMem = nullptr;          /* the new UDataMemory, to be returned.        */

    ប្រសិនបើ (U_FAILURE(*fatalErr)) {
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ(pHeader->dataHeader.magic1==0xda &&
        pHeader->dataHeader.magic2==0x27 &&
        (isAcceptable==nullptr || isAcceptable(context, type, name, &pHeader->info))
    ) {
        rDataMem=UDataMemory_createNewInstance(fatalErr);
        ប្រសិនបើ (U_FAILURE(*fatalErr)) {
            ត្រឡប់ nullptr;
        }
        rDataMem->pHeader = pHeader;
    } បើមិនដូច្នេះ {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    ត្រឡប់ rDataMem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
static UDataMemory *doLoadFromIndividualFiles(const var_na *pkgName, 
        const var_na *dataPath, const var_na *tocEntryPathSuffix,
            /* following arguments are the same as doOpenChoice itself */
            const var_na *path, const var_na *type, const var_na *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    const var_na         *pathBuffer;
    UDataMemory         dataMemory;
    UDataMemory *pEntryData;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UDataPathIterator iter(dataPath, pkgName, path, tocEntryPathSuffix, false, pErrorCode);

    ខណៈពេល ((pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", pathBuffer);
#endif
        ប្រសិនបើ (uprv_mapFile(&dataMemory, pathBuffer, pErrorCode))
        {
            pEntryData = checkDataItem(dataMemory.pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
            ប្រសិនបើ (pEntryData != nullptr) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                pEntryData->mapAddr = dataMemory.mapAddr;
                pEntryData->map     = dataMemory.map;
                pEntryData->length  = dataMemory.length;

#ifdef UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", pathBuffer);
#endif
                ត្រឡប់ pEntryData;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&dataMemory);

            /* If we had a nasty error, bail out completely.  */
            ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
                ត្រឡប់ nullptr;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *subErrorCode=U_INVALID_FORMAT_ERROR;
        }
#ifdef UDATA_DEBUG
        fprintf(stderr, "%s\n", UDataMemory_isLoaded(&dataMemory)?"LOADED":"not loaded");
#endif
    }
    ត្រឡប់ nullptr;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
static UDataMemory *doLoadFromCommonData(UBool isICUData, const var_na * /*pkgName*/, 
        const var_na * /*dataPath*/, const var_na * /*tocEntryPathSuffix*/, const var_na *tocEntryName,
            /* following arguments are the same as doOpenChoice itself */
            const var_na *path, const var_na *type, const var_na *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    UDataMemory        *pEntryData;
    const DataHeader   *pHeader;
    UDataMemory        *pCommonData;
    int32_t            commonDataIndex;
    UBool              checkedExtendedICUData = false;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    សម្រាប់ (commonDataIndex = isICUData ? 0 : -1;;) {
        pCommonData=openCommonData(path, commonDataIndex, subErrorCode); /** search for pkg **/

        ប្រសិនបើ(U_SUCCESS(*subErrorCode) && pCommonData!=nullptr) {
            int32_t length;

            /* look up the data piece in the common data */
            pHeader=pCommonData->vFuncs->Lookup(pCommonData, tocEntryName, &length, subErrorCode);
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", tocEntryName, (void*) pHeader, u_errorName(*subErrorCode));
#endif

            ប្រសិនបើ(pHeader!=nullptr) {
                pEntryData = checkDataItem(pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
#ifdef UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (void*) pEntryData);
#endif
                ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
                    ត្រឡប់ nullptr;
                }
                ប្រសិនបើ (pEntryData != nullptr) {
                    pEntryData->length = length;
                    ត្រឡប់ pEntryData;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        ប្រសិនបើ (*subErrorCode == U_MEMORY_ALLOCATION_ERROR) {
            *pErrorCode = *subErrorCode;
            ត្រឡប់ nullptr;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        ប្រសិនបើ (!isICUData) {
            ត្រឡប់ nullptr;
        } បើមិនដូច្នេះ ប្រសិនបើ (pCommonData != nullptr) {
            ++commonDataIndex;  /* try the next data package */
        } បើមិនដូច្នេះ ប្រសិនបើ ((!checkedExtendedICUData) && extendICUData(subErrorCode)) {
            checkedExtendedICUData = true;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } បើមិនដូច្នេះ {
            ត្រឡប់ nullptr;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
static UBool isTimeZoneFile(const var_na *name, const var_na *type) {
    ត្រឡប់ ((uprv_strcmp(type, "res") == 0) &&
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
static UDataMemory *
doOpenChoice(const var_na *path, const var_na *type, const var_na *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode)
{
    UDataMemory         *retVal = nullptr;

    const var_na         *dataPath;

    int32_t             tocEntrySuffixIndex;
    const var_na         *tocEntryPathSuffix;
    UErrorCode          subErrorCode=U_ZERO_ERROR;
    const var_na         *treeChar;

    UBool               isICUData = false;


    FileTracer::traceOpen(path, type, name);


    /* Is this path ICU data? */
    ប្រសិនបើ(path == nullptr ||
       !strcmp(path, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(path, U_ICUDATA_NAME U_TREE_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_TREE_SEPARATOR_STRING)) ||  
       !uprv_strncmp(path, U_ICUDATA_ALIAS U_TREE_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_TREE_SEPARATOR_STRING))) {
      isICUData = true;
    }

#ប្រសិនបើ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString altSepPath;
    ប្រសិនបើ(path) {
        ប្រសិនបើ(uprv_strchr(path,U_FILE_ALT_SEP_CHAR) != nullptr) {
            altSepPath.append(path, *pErrorCode);
            var_na *p;
            ខណៈពេល ((p = uprv_strchr(altSepPath.data(), U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
#ប្រសិនបើ defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", path, altSepPath.data());
#endif
            path = altSepPath.data();
        }
    }
#endif

    CharString tocEntryName; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString tocEntryPath; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString treeName;

    /* ======= Set up strings */
    ប្រសិនបើ(path==nullptr) {
        pkgName.append(U_ICUDATA_NAME, *pErrorCode);
    } បើមិនដូច្នេះ {
        const var_na *pkg;
        const var_na *first;
        pkg = uprv_strrchr(path, U_FILE_SEP_CHAR);
        first = uprv_strchr(path, U_FILE_SEP_CHAR);
        ប្រសិនបើ(uprv_pathIsAbsolute(path) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            ប្រសិនបើ(pkg) {
                pkgName.append(pkg+1, *pErrorCode);
            } បើមិនដូច្នេះ {
                pkgName.append(path, *pErrorCode);
            }
        } បើមិនដូច្នេះ {
            treeChar = uprv_strchr(path, U_TREE_SEPARATOR);
            ប្រសិនបើ(treeChar) { 
                treeName.append(treeChar+1, *pErrorCode); /* following '-' */
                ប្រសិនបើ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } បើមិនដូច្នេះ {
                    pkgName.append(path, static_cast<int32_t>(treeChar - path), *pErrorCode);
                    ប្រសិនបើ (first == nullptr) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        path = pkgName.data();
                    }
                }
            } បើមិនដូច្នេះ {
                ប្រសិនបើ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } បើមិនដូច្នេះ {
                    pkgName.append(path, *pErrorCode);
                }
            }
        }
    }

#ifdef UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.data(), treeName.data());
#endif

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    tocEntryName.append(pkgName, *pErrorCode);
    tocEntryPath.append(pkgName, *pErrorCode);
    tocEntrySuffixIndex = tocEntryName.length();

    ប្រសិនបើ(!treeName.isEmpty()) {
        tocEntryName.append(U_TREE_ENTRY_SEP_CHAR, *pErrorCode).append(treeName, *pErrorCode);
        tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(treeName, *pErrorCode);
    }

    tocEntryName.append(U_TREE_ENTRY_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    ប្រសិនបើ(type!=nullptr && *type!=0) {
        tocEntryName.append(".", *pErrorCode).append(type, *pErrorCode);
        tocEntryPath.append(".", *pErrorCode).append(type, *pErrorCode);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    tocEntryPathSuffix = tocEntryPath.data() + tocEntrySuffixIndex + 1; /* suffix starts here */

#ifdef UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", tocEntryName.data());
    fprintf(stderr, " tocEntryPath = %s\n", tocEntryPath.data());
#endif

#ប្រសិនបើ !defined(ICU_DATA_DIR_WINDOWS)
    ប្រសិនបើ(path == nullptr) {
        path = COMMON_DATA_NAME; /* "icudt26e" */
    }
#បើមិនដូច្នេះ
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
    ប្រសិនបើ (isICUData && type != nullptr && isTimeZoneFile(name, type)) {
        const var_na *tzFilesDir = u_getTimeZoneFilesDirectory(pErrorCode);
        ប្រសិនបើ (tzFilesDir[0] != 0) {
#ifdef UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#endif
            retVal = doLoadFromIndividualFiles(/* pkgName.data() */ "", tzFilesDir, tocEntryPathSuffix,
                            /* path */ "", type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            ប្រសិនបើ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                ត្រឡប់ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    ប្រសិនបើ(gDataFileAccess == UDATA_PACKAGES_FIRST) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#endif
        /* #2 */
        retVal = doLoadFromCommonData(isICUData, 
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        ប្រសិនបើ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ត្រឡប់ retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    ប្រសិនបើ((gDataFileAccess==UDATA_PACKAGES_FIRST) ||
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#endif
        /* Check to make sure that there is a dataPath to iterate over */
        ប្រសិនបើ ((dataPath && *dataPath) || !isICUData) {
            retVal = doLoadFromIndividualFiles(pkgName.data(), dataPath, tocEntryPathSuffix,
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            ប្រសិនបើ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                ត្រឡប់ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    ប្រសិនបើ((gDataFileAccess==UDATA_ONLY_PACKAGES) || 
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        ប្រសិនបើ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ត្រឡប់ retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    ប្រសិនបើ(gDataFileAccess==UDATA_NO_FILES) {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), "", tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        ប្រសិនបើ((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            ត្រឡប់ retVal;
        }
    }

    /* data not found */
    ប្រសិនបើ(U_SUCCESS(*pErrorCode)) {
        ប្រសិនបើ(U_SUCCESS(subErrorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } បើមិនដូច្នេះ {
            /* entry point not found or rejected */
            *pErrorCode=subErrorCode;
        }
    }
    ត្រឡប់ retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(const var_na *path, const var_na *type, const var_na *name,
           UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
    fflush(stderr);
#endif

    ប្រសិនបើ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    } បើមិនដូច្នេះ ប្រសិនបើ(name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    } បើមិនដូច្នេះ {
        ត្រឡប់ doOpenChoice(path, type, name, nullptr, nullptr, pErrorCode);
    }
}



U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(const var_na *path, const var_na *type, const var_na *name,
                 UDataMemoryIsAcceptable *isAcceptable, void *context,
                 UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
#endif

    ប្រសិនបើ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    } បើមិនដូច្នេះ ប្រសិនបើ(name==nullptr || *name==0 || isAcceptable==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    } បើមិនដូច្នេះ {
        ត្រឡប់ doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}



U_CAPI void U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    ប្រសិនបើ(pInfo!=nullptr) {
        ប្រសិនបើ(pData!=nullptr && pData->pHeader!=nullptr) {
            const UDataInfo *info=&pData->pHeader->info;
            uint16_t dataInfoSize=udata_getInfoSize(info);
            ប្រសិនបើ(pInfo->size>dataInfoSize) {
                pInfo->size=dataInfoSize;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (const uint16_t *)info+1, pInfo->size-2);
            ប្រសិនបើ(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reservedWord;
                pInfo->reservedWord=(uint16_t)((x<<8)|(x>>8));
            }
        } បើមិនដូច្នេះ {
            pInfo->size=0;
        }
    }
}


U_CAPI void U_EXPORT2 udata_setFileAccess(UDataFileAccess access, UErrorCode * /*status*/)
{
    // Note: this function is documented as not thread safe.
    gDataFileAccess = access;
}
