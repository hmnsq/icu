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

#यदि defined(UDATA_DEBUG)
#   include <stdio.h>
#endif

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
स्थिर UDataMemory *udata_findCachedData(अचर वर्ण *path, UErrorCode &err);

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

#यदि !defined(ICU_DATA_DIR_WINDOWS)
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

    यदि (gCommonDataCache) {             /* Delete the cache of user data mappings.  */
        uhash_close(gCommonDataCache);  /*   Table owns the contents, and will delete them. */
        gCommonDataCache = nullptr;        /*   Cleanup is not thread safe.                */
    }
    gCommonDataCacheInitOnce.reset();

    लागि (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != nullptr; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = nullptr;
    }
    gHaveTriedToLoadCommonData = 0;

    फर्काऊ सत्य;                   /* Everything was cleaned up */
}

स्थिर UBool U_CALLCONV
findCommonICUDataByName(अचर वर्ण *inBasename, UErrorCode &err)
{
    UBool found = असत्य;
    int32_t i;

    UDataMemory  *pData = udata_findCachedData(inBasename, err);
    यदि (U_FAILURE(err) || pData == nullptr)
        फर्काऊ असत्य;

    {
        Mutex lock;
        लागि (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            यदि ((gCommonICUDataArray[i] != nullptr) && (gCommonICUDataArray[i]->pHeader == pData->pHeader)) {
                /* The data pointer is already in the array. */
                found = सत्य;
                रोक;
            }
        }
    }
    फर्काऊ found;
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
    यदि (U_FAILURE(*pErr)) {
        फर्काऊ असत्य;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_assign(newCommonData, pData);
    umtx_lock(nullptr);
    लागि (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        यदि (gCommonICUDataArray[i] == nullptr) {
            gCommonICUDataArray[i] = newCommonData;
            didUpdate = सत्य;
            रोक;
        } अन्यथा यदि (gCommonICUDataArray[i]->pHeader == pData->pHeader) {
            /* The same data pointer is already in the array. */
            रोक;
        }
    }
    umtx_unlock(nullptr);

    यदि (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    यदि (didUpdate) {
        ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } अन्यथा {
        uprv_free(newCommonData);
    }
    फर्काऊ didUpdate;
}

#यदि !defined(ICU_DATA_DIR_WINDOWS)

स्थिर UBool
setCommonICUDataPointer(अचर शून्य *pData, UBool /*warn*/, UErrorCode *pErrorCode) {
    UDataMemory tData;
    UDataMemory_init(&tData);
    UDataMemory_setData(&tData, pData);
    udata_checkCommonData(&tData, pErrorCode);
    फर्काऊ setCommonICUData(&tData, असत्य, pErrorCode);
}

#endif

स्थिर अचर वर्ण *
findBasename(अचर वर्ण *path) {
    अचर वर्ण *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    यदि(basename==nullptr) {
        फर्काऊ path;
    } अन्यथा {
        फर्काऊ basename+1;
    }
}

#ifdef UDATA_DEBUG
स्थिर अचर वर्ण *
packageNameFromPath(अचर वर्ण *path)
{
    यदि((path == nullptr) || (*path == 0)) {
        फर्काऊ U_ICUDATA_NAME;
    }

    path = findBasename(path);

    यदि((path == nullptr) || (*path == 0)) {
        फर्काऊ U_ICUDATA_NAME;
    }

    फर्काऊ path;
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

प्रकारनिर्देश संरचना DataCacheElement {
    वर्ण          *name;
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
    यदि (U_FAILURE(err)) {
       फर्काऊ;
    }
    U_ASSERT(gCommonDataCache != nullptr);
    uhash_set_मान_Deleter(gCommonDataCache, DataCacheElement_deleter);
    ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
स्थिर UHashtable *udata_getHashTable(UErrorCode &err) {
    umtx_initOnce(gCommonDataCacheInitOnce, &udata_initHashTable, err);
    फर्काऊ gCommonDataCache;
}



स्थिर UDataMemory *udata_findCachedData(अचर वर्ण *path, UErrorCode &err)
{
    UHashtable        *htable;
    UDataMemory       *retVal = nullptr;
    DataCacheElement  *el;
    अचर वर्ण        *baseName;

    htable = udata_getHashTable(err);
    यदि (U_FAILURE(err)) {
        फर्काऊ nullptr;
    }

    baseName = findBasename(path);   /* Cache remembers only the base name, not the full path. */
    umtx_lock(nullptr);
    el = static_cast<DataCacheElement*>(uhash_get(htable, baseName));
    umtx_unlock(nullptr);
    यदि (el != nullptr) {
        retVal = el->item;
    }
#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "Cache: [%s] -> %p\n", baseName, (शून्य*) retVal);
#endif
    फर्काऊ retVal;
}


स्थिर UDataMemory *udata_cacheDataItem(अचर वर्ण *path, UDataMemory *item, UErrorCode *pErr) {
    DataCacheElement *newElement;
    अचर वर्ण       *baseName;
    int32_t           nameLen;
    UHashtable       *htable;
    DataCacheElement *old_मान = nullptr;
    UErrorCode        subErr = U_ZERO_ERROR;

    htable = udata_getHashTable(*pErr);
    यदि (U_FAILURE(*pErr)) {
        फर्काऊ nullptr;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<DataCacheElement*>(uprv_malloc(परिमाण(DataCacheElement)));
    यदि (newElement == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    newElement->item = UDataMemory_createNewInstance(pErr);
    यदि (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        फर्काऊ nullptr;
    }
    UDatamemory_assign(newElement->item, item);

    baseName = findBasename(path);
    nameLen = static_cast<int32_t>(uprv_strlen(baseName));
    newElement->name = static_cast<वर्ण*>(uprv_malloc(nameLen + 1));
    यदि (newElement->name == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        फर्काऊ nullptr;
    }
    uprv_strcpy(newElement->name, baseName);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(nullptr);
    old_मान = static_cast<DataCacheElement*>(uhash_get(htable, path));
    यदि (old_मान != nullptr) {
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

    यदि (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        फर्काऊ old_मान ? old_मान->item : nullptr;
    }

    फर्काऊ newElement->item;
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
    UDataPathIterator(अचर वर्ण *path, अचर वर्ण *pkg,
                      अचर वर्ण *item, अचर वर्ण *suffix, UBool doCheckLastFour,
                      UErrorCode *pErrorCode);
    अचर वर्ण *next(UErrorCode *pErrorCode);

गुप्त:
    अचर वर्ण *path;                              /* working path (u_icudata_Dir) */
    अचर वर्ण *nextPath;                          /* path following this one */
    अचर वर्ण *basename;                          /* item's basename (icudt22e_mt.res)*/

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
UDataPathIterator::UDataPathIterator(अचर वर्ण *inPath, अचर वर्ण *pkg,
                                     अचर वर्ण *item, अचर वर्ण *inSuffix, UBool doCheckLastFour,
                                     UErrorCode *pErrorCode)
{
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "SUFFIX1=%s PATH=%s\n", inSuffix, inPath);
#endif
    /** Path **/
    यदि(inPath == nullptr) {
        path = u_getDataDirectory();
    } अन्यथा {
        path = inPath;
    }

    /** Package **/
    यदि(pkg != nullptr) {
      packageStub.append(U_FILE_SEP_CHAR, *pErrorCode).append(pkg, *pErrorCode);
#ifdef UDATA_DEBUG
      ख_लिखो(मानक_त्रुटि, "STUB=%s [%d]\n", packageStub.data(), packageStub.length());
#endif
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    यदि(basename == item) {
        nextPath = path;
    } अन्यथा {
        itemPath.append(item, static_cast<int32_t>(basename - item), *pErrorCode);
        nextPath = itemPath.data();
    }
#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "SUFFIX=%s [%p]\n", inSuffix, (शून्य*) inSuffix);
#endif

    /** Suffix  **/
    यदि(inSuffix != nullptr) {
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
अचर वर्ण *UDataPathIterator::next(UErrorCode *pErrorCode)
{
    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }

    अचर वर्ण *currentPath = nullptr;
    int32_t     pathLen = 0;
    अचर वर्ण *pathBasename;

    गर
    {
        यदि( nextPath == nullptr ) {
            रोक;
        }
        currentPath = nextPath;

        यदि(nextPath == itemPath.data()) { /* we were processing item's path. */
            nextPath = path; /* start with regular path next tm. */
            pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
        } अन्यथा {
            /* fix up next for next time */
            nextPath = uprv_strchr(currentPath, U_PATH_SEP_CHAR);
            यदि(nextPath == nullptr) {
                /* segment: entire path */
                pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
            } अन्यथा {
                /* segment: until next segment */
                pathLen = static_cast<int32_t>(nextPath - currentPath);
                /* skip divider */
                nextPath ++;
            }
        }

        यदि(pathLen == 0) {
            जारी;
        }

#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "rest of path (IDD) = %s\n", currentPath);
        ख_लिखो(मानक_त्रुटि, "                     ");
        { 
            int32_t qqq;
            लागि(qqq=0;qqq<pathLen;qqq++)
            {
                ख_लिखो(मानक_त्रुटि, " ");
            }

            ख_लिखो(मानक_त्रुटि, "^\n");
        }
#endif
        pathBuffer.clear().append(currentPath, pathLen, *pErrorCode);

        /* check for .dat files */
        pathBasename = findBasename(pathBuffer.data());

        यदि(checkLastFour && 
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
            यदि(pathBuffer[pathLen-1] != U_FILE_SEP_CHAR) {
                यदि((pathLen>=4) &&
                   uprv_strncmp(pathBuffer.data()+(pathLen-4), ".dat", 4) == 0)
                {
#ifdef UDATA_DEBUG
                    ख_लिखो(मानक_त्रुटि, "skipping non-directory .dat file %s\n", pathBuffer.data());
#endif
                    जारी;
                }

                /* Check if it is a directory with the same name as our package */
                यदि(!packageStub.isEmpty() &&
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

            यदि (!suffix.empty())  /* tack on suffix */
            {
                यदि (suffix.length() > 4) {
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

        फर्काऊ pathBuffer.data();

    } जबसम्म(path);

    /* fell way off the end */
    फर्काऊ nullptr;
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
#यदि !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
बाह्य "C" अचर DataHeader U_DATA_API U_ICUDATA_ENTRY_POINT;
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
openCommonData(अचर वर्ण *path,          /*  Path from OpenChoice?          */
               int32_t commonDataIndex,   /*  ICU Data (index >= 0) if path == nullptr */
               UErrorCode *pErrorCode)
{
    UDataMemory tData;
    अचर वर्ण *pathBuffer;
    अचर वर्ण *inBasename;

    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }

    UDataMemory_init(&tData);

    /* ??????? TODO revisit this */ 
    यदि (commonDataIndex >= 0) {
        /* "mini-cache" for common ICU data */
        यदि(commonDataIndex >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            फर्काऊ nullptr;
        }
        {
            Mutex lock;
            यदि(gCommonICUDataArray[commonDataIndex] != nullptr) {
                फर्काऊ gCommonICUDataArray[commonDataIndex];
            }
#यदि !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            लागि(i = 0; i < commonDataIndex; ++i) {
                यदि(gCommonICUDataArray[i]->pHeader == &U_ICUDATA_ENTRY_POINT) {
                    /* The linked-in data is already in the list. */
                    फर्काऊ nullptr;
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
#यदि !defined(ICU_DATA_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        setCommonICUDataPointer(&U_ICUDATA_ENTRY_POINT, असत्य, pErrorCode);
        {
            Mutex lock;
            फर्काऊ gCommonICUDataArray[commonDataIndex];
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

    यदि(*inBasename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "ocd: no basename in %s, bailing.\n", path);
#endif
        यदि (U_SUCCESS(*pErrorCode)) {
            *pErrorCode=U_FILE_ACCESS_ERROR;
        }
        फर्काऊ nullptr;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UDataMemory  *dataToReturn = udata_findCachedData(inBasename, *pErrorCode);
    यदि (dataToReturn != nullptr || U_FAILURE(*pErrorCode)) {
        फर्काऊ dataToReturn;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UDataPathIterator iter(u_getDataDirectory(), inBasename, path, ".dat", सत्य, pErrorCode);

    जबसम्म ((UDataMemory_isLoaded(&tData)==असत्य) && (pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "ocd: trying path %s - ", pathBuffer);
#endif
        uprv_mapFile(&tData, pathBuffer, pErrorCode);
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "%s\n", UDataMemory_isLoaded(&tData)?"LOADED":"not loaded");
#endif
    }
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }

    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }
    यदि (!UDataMemory_isLoaded(&tData)) {
        /* no common data */
        *pErrorCode=U_FILE_ACCESS_ERROR;
        फर्काऊ nullptr;
    }

    /* we have mapped a file, check its header */
    udata_checkCommonData(&tData, pErrorCode);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    फर्काऊ udata_cacheDataItem(inBasename, &tData, pErrorCode);
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
#यदि MAP_IMPLEMENTATION==MAP_STDIO
    स्थिर UMutex extendICUDataMutex;
    umtx_lock(&extendICUDataMutex);
#endif
    यदि(!umtx_loadAcquire(gHaveTriedToLoadCommonData)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        pData = openCommonData(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UDataMemory_init(&copyPData);
       यदि(pData != nullptr) {
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

#यदि MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#endif
    फर्काऊ didUpdate;               /* Return true if ICUData pointer was updated.   */
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
udata_setCommonData(अचर शून्य *data, UErrorCode *pErrorCode) {
    UDataMemory dataMemory;

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        फर्काऊ;
    }

    यदि(data==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    /* set the data pointer and test for validity */
    UDataMemory_init(&dataMemory);
    UDataMemory_setData(&dataMemory, data);
    udata_checkCommonData(&dataMemory, pErrorCode);
    यदि (U_FAILURE(*pErrorCode)) {फर्काऊ;}

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
udata_setAppData(अचर वर्ण *path, अचर शून्य *data, UErrorCode *err)
{
    UDataMemory     udm;

    यदि(err==nullptr || U_FAILURE(*err)) {
        फर्काऊ;
    }
    यदि(data==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
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
 अचर DataHeader         *pHeader,         /* The data item to be checked.                */
 UDataMemoryIsAcceptable  *isAcceptable,    /* App's call-back function                    */
 शून्य                     *context,         /*   pass-thru param for above.                */
 अचर वर्ण               *type,            /*   pass-thru param for above.                */
 अचर वर्ण               *name,            /*   pass-thru param for above.                */
 UErrorCode               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UErrorCode               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UDataMemory  *rDataMem = nullptr;          /* the new UDataMemory, to be returned.        */

    यदि (U_FAILURE(*fatalErr)) {
        फर्काऊ nullptr;
    }

    यदि(pHeader->dataHeader.magic1==0xda &&
        pHeader->dataHeader.magic2==0x27 &&
        (isAcceptable==nullptr || isAcceptable(context, type, name, &pHeader->info))
    ) {
        rDataMem=UDataMemory_createNewInstance(fatalErr);
        यदि (U_FAILURE(*fatalErr)) {
            फर्काऊ nullptr;
        }
        rDataMem->pHeader = pHeader;
    } अन्यथा {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    फर्काऊ rDataMem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
स्थिर UDataMemory *doLoadFromIndividualFiles(अचर वर्ण *pkgName, 
        अचर वर्ण *dataPath, अचर वर्ण *tocEntryPathSuffix,
            /* following arguments are the same as doOpenChoice itself */
            अचर वर्ण *path, अचर वर्ण *type, अचर वर्ण *name,
             UDataMemoryIsAcceptable *isAcceptable, शून्य *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    अचर वर्ण         *pathBuffer;
    UDataMemory         dataMemory;
    UDataMemory *pEntryData;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UDataPathIterator iter(dataPath, pkgName, path, tocEntryPathSuffix, असत्य, pErrorCode);

    जबसम्म ((pathBuffer = iter.next(pErrorCode)) != nullptr)
    {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "UDATA: trying individual file %s\n", pathBuffer);
#endif
        यदि (uprv_mapFile(&dataMemory, pathBuffer, pErrorCode))
        {
            pEntryData = checkDataItem(dataMemory.pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
            यदि (pEntryData != nullptr) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                pEntryData->mapAddr = dataMemory.mapAddr;
                pEntryData->map     = dataMemory.map;
                pEntryData->length  = dataMemory.length;

#ifdef UDATA_DEBUG
                ख_लिखो(मानक_त्रुटि, "** Mapped file: %s\n", pathBuffer);
#endif
                फर्काऊ pEntryData;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&dataMemory);

            /* If we had a nasty error, bail out completely.  */
            यदि (U_FAILURE(*pErrorCode)) {
                फर्काऊ nullptr;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *subErrorCode=U_INVALID_FORMAT_ERROR;
        }
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "%s\n", UDataMemory_isLoaded(&dataMemory)?"LOADED":"not loaded");
#endif
    }
    फर्काऊ nullptr;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
स्थिर UDataMemory *doLoadFromCommonData(UBool isICUData, अचर वर्ण * /*pkgName*/, 
        अचर वर्ण * /*dataPath*/, अचर वर्ण * /*tocEntryPathSuffix*/, अचर वर्ण *tocEntryName,
            /* following arguments are the same as doOpenChoice itself */
            अचर वर्ण *path, अचर वर्ण *type, अचर वर्ण *name,
             UDataMemoryIsAcceptable *isAcceptable, शून्य *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    UDataMemory        *pEntryData;
    अचर DataHeader   *pHeader;
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
    लागि (commonDataIndex = isICUData ? 0 : -1;;) {
        pCommonData=openCommonData(path, commonDataIndex, subErrorCode); /** search for pkg **/

        यदि(U_SUCCESS(*subErrorCode) && pCommonData!=nullptr) {
            int32_t length;

            /* look up the data piece in the common data */
            pHeader=pCommonData->vFuncs->Lookup(pCommonData, tocEntryName, &length, subErrorCode);
#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: pHeader=%p - %s\n", tocEntryName, (शून्य*) pHeader, u_errorName(*subErrorCode));
#endif

            यदि(pHeader!=nullptr) {
                pEntryData = checkDataItem(pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
#ifdef UDATA_DEBUG
                ख_लिखो(मानक_त्रुटि, "pEntryData=%p\n", (शून्य*) pEntryData);
#endif
                यदि (U_FAILURE(*pErrorCode)) {
                    फर्काऊ nullptr;
                }
                यदि (pEntryData != nullptr) {
                    pEntryData->length = length;
                    फर्काऊ pEntryData;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        यदि (*subErrorCode == U_MEMORY_ALLOCATION_ERROR) {
            *pErrorCode = *subErrorCode;
            फर्काऊ nullptr;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        यदि (!isICUData) {
            फर्काऊ nullptr;
        } अन्यथा यदि (pCommonData != nullptr) {
            ++commonDataIndex;  /* try the next data package */
        } अन्यथा यदि ((!checkedExtendedICUData) && extendICUData(subErrorCode)) {
            checkedExtendedICUData = सत्य;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } अन्यथा {
            फर्काऊ nullptr;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
स्थिर UBool isTimeZoneFile(अचर वर्ण *name, अचर वर्ण *type) {
    फर्काऊ ((uprv_strcmp(type, "res") == 0) &&
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
doOpenChoice(अचर वर्ण *path, अचर वर्ण *type, अचर वर्ण *name,
             UDataMemoryIsAcceptable *isAcceptable, शून्य *context,
             UErrorCode *pErrorCode)
{
    UDataMemory         *retVal = nullptr;

    अचर वर्ण         *dataPath;

    int32_t             tocEntrySuffixIndex;
    अचर वर्ण         *tocEntryPathSuffix;
    UErrorCode          subErrorCode=U_ZERO_ERROR;
    अचर वर्ण         *रूख_Char;

    UBool               isICUData = असत्य;


    FileTracer::traceOpen(path, type, name);


    /* Is this path ICU data? */
    यदि(path == nullptr ||
       !म_भेद(path, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(path, U_ICUDATA_NAME U_रूख_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_रूख_SEPARATOR_STRING)) ||  
       !uprv_strncmp(path, U_ICUDATA_ALIAS U_रूख_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_रूख_SEPARATOR_STRING))) {
      isICUData = सत्य;
    }

#यदि (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString altSepPath;
    यदि(path) {
        यदि(uprv_strchr(path,U_FILE_ALT_SEP_CHAR) != nullptr) {
            altSepPath.append(path, *pErrorCode);
            वर्ण *p;
            जबसम्म ((p = uprv_strchr(altSepPath.data(), U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
#यदि defined (UDATA_DEBUG)
            ख_लिखो(मानक_त्रुटि, "Changed path from [%s] to [%s]\n", path, altSepPath.data());
#endif
            path = altSepPath.data();
        }
    }
#endif

    CharString tocEntryName; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString tocEntryPath; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString रूख_Name;

    /* ======= Set up strings */
    यदि(path==nullptr) {
        pkgName.append(U_ICUDATA_NAME, *pErrorCode);
    } अन्यथा {
        अचर वर्ण *pkg;
        अचर वर्ण *first;
        pkg = uprv_strrchr(path, U_FILE_SEP_CHAR);
        first = uprv_strchr(path, U_FILE_SEP_CHAR);
        यदि(uprv_pathIsAbsolute(path) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            यदि(pkg) {
                pkgName.append(pkg+1, *pErrorCode);
            } अन्यथा {
                pkgName.append(path, *pErrorCode);
            }
        } अन्यथा {
            रूख_Char = uprv_strchr(path, U_रूख_SEPARATOR);
            यदि(रूख_Char) { 
                रूख_Name.append(रूख_Char+1, *pErrorCode); /* following '-' */
                यदि(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } अन्यथा {
                    pkgName.append(path, static_cast<int32_t>(रूख_Char - path), *pErrorCode);
                    यदि (first == nullptr) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        path = pkgName.data();
                    }
                }
            } अन्यथा {
                यदि(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } अन्यथा {
                    pkgName.append(path, *pErrorCode);
                }
            }
        }
    }

#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, " P=%s T=%s\n", pkgName.data(), रूख_Name.data());
#endif

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    tocEntryName.append(pkgName, *pErrorCode);
    tocEntryPath.append(pkgName, *pErrorCode);
    tocEntrySuffixIndex = tocEntryName.length();

    यदि(!रूख_Name.isEmpty()) {
        tocEntryName.append(U_रूख_ENTRY_SEP_CHAR, *pErrorCode).append(रूख_Name, *pErrorCode);
        tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(रूख_Name, *pErrorCode);
    }

    tocEntryName.append(U_रूख_ENTRY_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    यदि(type!=nullptr && *type!=0) {
        tocEntryName.append(".", *pErrorCode).append(type, *pErrorCode);
        tocEntryPath.append(".", *pErrorCode).append(type, *pErrorCode);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    tocEntryPathSuffix = tocEntryPath.data() + tocEntrySuffixIndex + 1; /* suffix starts here */

#ifdef UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, " tocEntryName = %s\n", tocEntryName.data());
    ख_लिखो(मानक_त्रुटि, " tocEntryPath = %s\n", tocEntryPath.data());
#endif

#यदि !defined(ICU_DATA_DIR_WINDOWS)
    यदि(path == nullptr) {
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
    यदि (isICUData && type != nullptr && isTimeZoneFile(name, type)) {
        अचर वर्ण *tzFilesDir = u_getTimeZoneFilesDirectory(pErrorCode);
        यदि (tzFilesDir[0] != 0) {
#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#endif
            retVal = doLoadFromIndividualFiles(/* pkgName.data() */ "", tzFilesDir, tocEntryPathSuffix,
                            /* path */ "", type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            यदि((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                फर्काऊ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    यदि(gDataFileAccess == UDATA_PACKAGES_FIRST) {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#endif
        /* #2 */
        retVal = doLoadFromCommonData(isICUData, 
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        यदि((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            फर्काऊ retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    यदि((gDataFileAccess==UDATA_PACKAGES_FIRST) ||
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying individual files\n");
#endif
        /* Check to make sure that there is a dataPath to iterate over */
        यदि ((dataPath && *dataPath) || !isICUData) {
            retVal = doLoadFromIndividualFiles(pkgName.data(), dataPath, tocEntryPathSuffix,
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            यदि((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                फर्काऊ retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    यदि((gDataFileAccess==UDATA_ONLY_PACKAGES) || 
       (gDataFileAccess==UDATA_FILES_FIRST)) {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), dataPath, tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        यदि((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            फर्काऊ retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    यदि(gDataFileAccess==UDATA_NO_FILES) {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying common data (UDATA_NO_FILES)\n");
#endif
        retVal = doLoadFromCommonData(isICUData,
                            pkgName.data(), "", tocEntryPathSuffix, tocEntryName.data(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        यदि((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            फर्काऊ retVal;
        }
    }

    /* data not found */
    यदि(U_SUCCESS(*pErrorCode)) {
        यदि(U_SUCCESS(subErrorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } अन्यथा {
            /* entry point not found or rejected */
            *pErrorCode=subErrorCode;
        }
    }
    फर्काऊ retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(अचर वर्ण *path, अचर वर्ण *type, अचर वर्ण *name,
           UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  ख_लिखो(मानक_त्रुटि, "udata_open(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
    ख_साफ(मानक_त्रुटि);
#endif

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    } अन्यथा यदि(name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    } अन्यथा {
        फर्काऊ doOpenChoice(path, type, name, nullptr, nullptr, pErrorCode);
    }
}



U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(अचर वर्ण *path, अचर वर्ण *type, अचर वर्ण *name,
                 UDataMemoryIsAcceptable *isAcceptable, शून्य *context,
                 UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  ख_लिखो(मानक_त्रुटि, "udata_openChoice(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
#endif

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    } अन्यथा यदि(name==nullptr || *name==0 || isAcceptable==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    } अन्यथा {
        फर्काऊ doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}



U_CAPI शून्य U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    यदि(pInfo!=nullptr) {
        यदि(pData!=nullptr && pData->pHeader!=nullptr) {
            अचर UDataInfo *info=&pData->pHeader->info;
            uint16_t dataInfoSize=udata_getInfoSize(info);
            यदि(pInfo->size>dataInfoSize) {
                pInfo->size=dataInfoSize;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (अचर uint16_t *)info+1, pInfo->size-2);
            यदि(info->isBigEndian!=U_IS_BIG_ENDIAN) {
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
