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

#समावेश "unicode/utypes.h"  /* U_PLATFORM etc. */

#अगर_घोषित __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#पूर्ण_अगर

#समावेश "unicode/putil.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/uversion.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "mutex.h"
#समावेश "putilimp.h"
#समावेश "restrace.h"
#समावेश "uassert.h"
#समावेश "ucln_cmn.h"
#समावेश "ucmndata.h"
#समावेश "udatamem.h"
#समावेश "uhash.h"
#समावेश "umapfile.h"
#समावेश "umutex.h"

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

#अगर defined(UDATA_DEBUG)
#   include <stdio.h>
#पूर्ण_अगर

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
अटल UDataMemory *udata_find_Cached_तथ्य(स्थिर अक्षर *path, UErrorCode &err);

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
अटल UDataMemory *gCommonICUDataArray[10] = { nullptr };   // Access protected by icu global mutex.

अटल u_atomic_int32_t g_Have_Tried_To_Load_Common_तथ्य {0};  //  See extendICUData().

अटल UHashtable  *g_Common_तथ्य_Cache = nullptr;  /* Global hash table of opened ICU data files.  */
अटल icu::UInitOnce g_Common_तथ्य_Cache_Init_Once {};

#अगर !defined(ICU_तथ्य_DIR_WINDOWS)
अटल UDataFileAccess  g_तथ्य_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#अन्यथा
// If we are using the Windows data directory, then look in one spot only.
अटल UDataFileAccess  g_तथ्य_File_Access = UDATA_NO_FILES;
#पूर्ण_अगर

अटल UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    अगर (g_Common_तथ्य_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_तथ्य_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_तथ्य_Cache = nullptr;        /*   Cleanup is not thread safe.                */
    }
    g_Common_तथ्य_Cache_Init_Once.reset();

    क्रम (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != nullptr; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = nullptr;
    }
    g_Have_Tried_To_Load_Common_तथ्य = 0;

    वापस true;                   /* Everything was cleaned up */
}

अटल UBool U_CALLCONV
findCommonICUDataByName(स्थिर अक्षर *inBasename, UErrorCode &err)
{
    UBool found = false;
    int32_t i;

    UDataMemory  *p_तथ्य = udata_find_Cached_तथ्य(inBasename, err);
    अगर (U_FAILURE(err) || p_तथ्य == nullptr)
        वापस false;

    {
        Mutex lock;
        क्रम (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            अगर ((gCommonICUDataArray[i] != nullptr) && (gCommonICUDataArray[i]->pHeader == p_तथ्य->pHeader)) {
                /* The data pointer is already in the array. */
                found = true;
                अवरोध;
            }
        }
    }
    वापस found;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
अटल UBool
setCommonICUData(UDataMemory *p_तथ्य,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UErrorCode *pErr)
{
    UDataMemory  *new_Common_तथ्य = UDataMemory_createNewInstance(pErr);
    int32_t i;
    UBool didUpdate = false;
    अगर (U_FAILURE(*pErr)) {
        वापस false;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_assign(new_Common_तथ्य, p_तथ्य);
    umtx_lock(nullptr);
    क्रम (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        अगर (gCommonICUDataArray[i] == nullptr) {
            gCommonICUDataArray[i] = new_Common_तथ्य;
            didUpdate = true;
            अवरोध;
        } अन्यथा अगर (gCommonICUDataArray[i]->pHeader == p_तथ्य->pHeader) {
            /* The same data pointer is already in the array. */
            अवरोध;
        }
    }
    umtx_unlock(nullptr);

    अगर (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    अगर (didUpdate) {
        ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } अन्यथा {
        uprv_free(new_Common_तथ्य);
    }
    वापस didUpdate;
}

#अगर !defined(ICU_तथ्य_DIR_WINDOWS)

अटल UBool
setCommonICUDataPointer(स्थिर व्योम *p_तथ्य, UBool /*warn*/, UErrorCode *pErrorCode) {
    UDataMemory t_तथ्य;
    UDataMemory_init(&t_तथ्य);
    UData_Memory_set_तथ्य(&t_तथ्य, p_तथ्य);
    udata_check_Common_तथ्य(&t_तथ्य, pErrorCode);
    वापस setCommonICUData(&t_तथ्य, false, pErrorCode);
}

#पूर्ण_अगर

अटल स्थिर अक्षर *
findBasename(स्थिर अक्षर *path) {
    स्थिर अक्षर *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    अगर(basename==nullptr) {
        वापस path;
    } अन्यथा {
        वापस basename+1;
    }
}

#अगर_घोषित UDATA_DEBUG
अटल स्थिर अक्षर *
packageNameFromPath(स्थिर अक्षर *path)
{
    अगर((path == nullptr) || (*path == 0)) {
        वापस U_ICUDATA_NAME;
    }

    path = findBasename(path);

    अगर((path == nullptr) || (*path == 0)) {
        वापस U_ICUDATA_NAME;
    }

    वापस path;
}
#पूर्ण_अगर

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

प्रकार काष्ठा तथ्य_Cache_Element {
    अक्षर          *name;
    UDataMemory   *item;
} तथ्य_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
अटल व्योम U_CALLCONV तथ्य_Cache_Element_deleter(व्योम *pDCEl) {
    तथ्य_Cache_Element* p = static_cast<तथ्य_Cache_Element*>(pDCEl);
    udata_close(p->item);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

अटल व्योम U_CALLCONV udata_initHashTable(UErrorCode &err) {
    U_ASSERT(g_Common_तथ्य_Cache == nullptr);
    g_Common_तथ्य_Cache = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &err);
    अगर (U_FAILURE(err)) {
       वापस;
    }
    U_ASSERT(g_Common_तथ्य_Cache != nullptr);
    uhash_set_मूल्य_Deleter(g_Common_तथ्य_Cache, तथ्य_Cache_Element_deleter);
    ucln_common_registerCleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
अटल UHashtable *udata_getHashTable(UErrorCode &err) {
    umtx_initOnce(g_Common_तथ्य_Cache_Init_Once, &udata_initHashTable, err);
    वापस g_Common_तथ्य_Cache;
}



अटल UDataMemory *udata_find_Cached_तथ्य(स्थिर अक्षर *path, UErrorCode &err)
{
    UHashtable        *htable;
    UDataMemory       *retVal = nullptr;
    तथ्य_Cache_Element  *el;
    स्थिर अक्षर        *baseName;

    htable = udata_getHashTable(err);
    अगर (U_FAILURE(err)) {
        वापस nullptr;
    }

    baseName = findBasename(path);   /* Cache remembers only the base name, not the full path. */
    umtx_lock(nullptr);
    el = static_cast<तथ्य_Cache_Element*>(uhash_get(htable, baseName));
    umtx_unlock(nullptr);
    अगर (el != nullptr) {
        retVal = el->item;
    }
#अगर_घोषित UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "Cache: [%s] -> %p\n", baseName, (व्योम*) retVal);
#पूर्ण_अगर
    वापस retVal;
}


अटल UDataMemory *udata_cache_तथ्य_Item(स्थिर अक्षर *path, UDataMemory *item, UErrorCode *pErr) {
    तथ्य_Cache_Element *newElement;
    स्थिर अक्षर       *baseName;
    int32_t           nameLen;
    UHashtable       *htable;
    तथ्य_Cache_Element *old_मूल्य = nullptr;
    UErrorCode        subErr = U_ZERO_ERROR;

    htable = udata_getHashTable(*pErr);
    अगर (U_FAILURE(*pErr)) {
        वापस nullptr;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<तथ्य_Cache_Element*>(uprv_malloc(माप(तथ्य_Cache_Element)));
    अगर (newElement == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    newElement->item = UDataMemory_createNewInstance(pErr);
    अगर (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        वापस nullptr;
    }
    UDatamemory_assign(newElement->item, item);

    baseName = findBasename(path);
    nameLen = static_cast<int32_t>(uprv_strlen(baseName));
    newElement->name = static_cast<अक्षर*>(uprv_malloc(nameLen + 1));
    अगर (newElement->name == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        वापस nullptr;
    }
    uprv_strcpy(newElement->name, baseName);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(nullptr);
    old_मूल्य = static_cast<तथ्य_Cache_Element*>(uhash_get(htable, path));
    अगर (old_मूल्य != nullptr) {
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

#अगर_घोषित UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (व्योम*) newElement->item, u_errorName(subErr), (व्योम*) newElement->item->vFuncs);
#पूर्ण_अगर

    अगर (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        वापस old_मूल्य ? old_मूल्य->item : nullptr;
    }

    वापस newElement->item;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

श्रेणी UDataPathIterator
{
खुला:
    UDataPathIterator(स्थिर अक्षर *path, स्थिर अक्षर *pkg,
                      स्थिर अक्षर *item, स्थिर अक्षर *suffix, UBool doCheckLastFour,
                      UErrorCode *pErrorCode);
    स्थिर अक्षर *next(UErrorCode *pErrorCode);

निजी:
    स्थिर अक्षर *path;                              /* working path (u_icudata_Dir) */
    स्थिर अक्षर *अग्र_Path;                          /* path following this one */
    स्थिर अक्षर *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  itemPath;                          /* path passed in with item name */
    CharString  path_बफर;                        /* output path for this it'ion */
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
UDataPathIterator::UDataPathIterator(स्थिर अक्षर *inPath, स्थिर अक्षर *pkg,
                                     स्थिर अक्षर *item, स्थिर अक्षर *inSuffix, UBool doCheckLastFour,
                                     UErrorCode *pErrorCode)
{
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "SUFFIX1=%s PATH=%s\n", inSuffix, inPath);
#पूर्ण_अगर
    /** Path **/
    अगर(inPath == nullptr) {
        path = u_get_तथ्य_Directory();
    } अन्यथा {
        path = inPath;
    }

    /** Package **/
    अगर(pkg != nullptr) {
      packageStub.append(U_FILE_SEP_CHAR, *pErrorCode).append(pkg, *pErrorCode);
#अगर_घोषित UDATA_DEBUG
      ख_लिखो(मानक_त्रुटि, "STUB=%s [%d]\n", packageStub.तथ्य(), packageStub.length());
#पूर्ण_अगर
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    अगर(basename == item) {
        अग्र_Path = path;
    } अन्यथा {
        itemPath.append(item, static_cast<int32_t>(basename - item), *pErrorCode);
        अग्र_Path = itemPath.तथ्य();
    }
#अगर_घोषित UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "SUFFIX=%s [%p]\n", inSuffix, (व्योम*) inSuffix);
#पूर्ण_अगर

    /** Suffix  **/
    अगर(inSuffix != nullptr) {
        suffix = inSuffix;
    } अन्यथा {
        suffix = "";
    }

    checkLastFour = doCheckLastFour;

    /* pathBuffer will hold the output path strings returned by this iterator */

#अगर_घोषित UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            item,
            path,
            basename,
            suffix.तथ्य(),
            itemPath.तथ्य(),
            अग्र_Path,
            checkLastFour?"true":"false");
#पूर्ण_अगर
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
स्थिर अक्षर *UDataPathIterator::next(UErrorCode *pErrorCode)
{
    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }

    स्थिर अक्षर *currentPath = nullptr;
    int32_t     pathLen = 0;
    स्थिर अक्षर *pathBasename;

    करो
    {
        अगर( अग्र_Path == nullptr ) {
            अवरोध;
        }
        currentPath = अग्र_Path;

        अगर(अग्र_Path == itemPath.तथ्य()) { /* we were processing item's path. */
            अग्र_Path = path; /* start with regular path next tm. */
            pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
        } अन्यथा {
            /* fix up next for next time */
            अग्र_Path = uprv_strchr(currentPath, U_PATH_SEP_CHAR);
            अगर(अग्र_Path == nullptr) {
                /* segment: entire path */
                pathLen = static_cast<int32_t>(uprv_strlen(currentPath));
            } अन्यथा {
                /* segment: until next segment */
                pathLen = static_cast<int32_t>(अग्र_Path - currentPath);
                /* skip divider */
                अग्र_Path ++;
            }
        }

        अगर(pathLen == 0) {
            जारी;
        }

#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "rest of path (IDD) = %s\n", currentPath);
        ख_लिखो(मानक_त्रुटि, "                     ");
        { 
            int32_t qqq;
            क्रम(qqq=0;qqq<pathLen;qqq++)
            {
                ख_लिखो(मानक_त्रुटि, " ");
            }

            ख_लिखो(मानक_त्रुटि, "^\n");
        }
#पूर्ण_अगर
        path_बफर.सफा().append(currentPath, pathLen, *pErrorCode);

        /* check for .dat files */
        pathBasename = findBasename(path_बफर.तथ्य());

        अगर(checkLastFour && 
           (pathLen>=4) &&
           uprv_strncmp(path_बफर.तथ्य() +(pathLen-4), suffix.तथ्य(), 4)==0 && /* suffix matches */
           uprv_strncmp(findBasename(path_बफर.तथ्य()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(pathBasename)==(basenameLen+4)) { /* base+suffix = full len */

#अगर_घोषित UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "Have %s file on the path: %s\n", suffix.तथ्य(), path_बफर.तथ्य());
#पूर्ण_अगर
            /* do nothing */
        }
        अन्यथा 
        {       /* regular dir path */
            अगर(path_बफर[pathLen-1] != U_FILE_SEP_CHAR) {
                अगर((pathLen>=4) &&
                   uprv_strncmp(path_बफर.तथ्य()+(pathLen-4), ".dat", 4) == 0)
                {
#अगर_घोषित UDATA_DEBUG
                    ख_लिखो(मानक_त्रुटि, "skipping non-directory .dat file %s\n", path_बफर.तथ्य());
#पूर्ण_अगर
                    जारी;
                }

                /* Check if it is a directory with the same name as our package */
                अगर(!packageStub.isEmpty() &&
                   (pathLen > packageStub.length()) &&
                   !uprv_strcmp(path_बफर.तथ्य() + pathLen - packageStub.length(), packageStub.तथ्य())) {
#अगर_घोषित UDATA_DEBUG
                  ख_लिखो(मानक_त्रुटि, "Found stub %s (will add package %s of len %d)\n", packageStub.तथ्य(), basename, basenameLen);
#पूर्ण_अगर
                  path_बफर.truncate(pathLen - packageStub.length());
                }
                path_बफर.append(U_FILE_SEP_CHAR, *pErrorCode);
            }

            /* + basename */
            path_बफर.append(packageStub.तथ्य()+1, packageStub.length()-1, *pErrorCode);

            अगर (!suffix.empty())  /* tack on suffix */
            {
                अगर (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    path_बफर.ensureEndsWithFileSeparator(*pErrorCode);
                }
                path_बफर.append(suffix, *pErrorCode);
            }
        }

#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, " -->  %s\n", path_बफर.तथ्य());
#पूर्ण_अगर

        वापस path_बफर.तथ्य();

    } जबतक(path);

    /* fell way off the end */
    वापस nullptr;
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
#अगर !defined(ICU_तथ्य_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
बाह्य "C" स्थिर तथ्य_Header U_तथ्य_API U_ICUDATA_ENTRY_POINT;
#पूर्ण_अगर

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
अटल UDataMemory *
open_Common_तथ्य(स्थिर अक्षर *path,          /*  Path from OpenChoice?          */
               int32_t common_तथ्य_सूचकांक,   /*  ICU Data (index >= 0) if path == nullptr */
               UErrorCode *pErrorCode)
{
    UDataMemory t_तथ्य;
    स्थिर अक्षर *path_बफर;
    स्थिर अक्षर *inBasename;

    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }

    UDataMemory_init(&t_तथ्य);

    /* ??????? TODO revisit this */ 
    अगर (common_तथ्य_सूचकांक >= 0) {
        /* "mini-cache" for common ICU data */
        अगर(common_तथ्य_सूचकांक >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            वापस nullptr;
        }
        {
            Mutex lock;
            अगर(gCommonICUDataArray[common_तथ्य_सूचकांक] != nullptr) {
                वापस gCommonICUDataArray[common_तथ्य_सूचकांक];
            }
#अगर !defined(ICU_तथ्य_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            क्रम(i = 0; i < common_तथ्य_सूचकांक; ++i) {
                अगर(gCommonICUDataArray[i]->pHeader == &U_ICUDATA_ENTRY_POINT) {
                    /* The linked-in data is already in the list. */
                    वापस nullptr;
                }
            }
#पूर्ण_अगर
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
#अगर !defined(ICU_तथ्य_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        setCommonICUDataPointer(&U_ICUDATA_ENTRY_POINT, false, pErrorCode);
        {
            Mutex lock;
            वापस gCommonICUDataArray[common_तथ्य_सूचकांक];
        }
#पूर्ण_अगर
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    inBasename = findBasename(path);
#अगर_घोषित UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "inBasename = %s\n", inBasename);
#पूर्ण_अगर

    अगर(*inBasename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "ocd: no basename in %s, bailing.\n", path);
#पूर्ण_अगर
        अगर (U_SUCCESS(*pErrorCode)) {
            *pErrorCode=U_FILE_ACCESS_ERROR;
        }
        वापस nullptr;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UDataMemory  *तथ्य_To_Return = udata_find_Cached_तथ्य(inBasename, *pErrorCode);
    अगर (तथ्य_To_Return != nullptr || U_FAILURE(*pErrorCode)) {
        वापस तथ्य_To_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UDataPathIterator iter(u_get_तथ्य_Directory(), inBasename, path, ".dat", true, pErrorCode);

    जबतक ((UDataMemory_isLoaded(&t_तथ्य)==false) && (path_बफर = iter.next(pErrorCode)) != nullptr)
    {
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "ocd: trying path %s - ", path_बफर);
#पूर्ण_अगर
        uprv_mapFile(&t_तथ्य, path_बफर, pErrorCode);
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "%s\n", UDataMemory_isLoaded(&t_तथ्य)?"LOADED":"not loaded");
#पूर्ण_अगर
    }
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }

    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर (!UDataMemory_isLoaded(&t_तथ्य)) {
        /* no common data */
        *pErrorCode=U_FILE_ACCESS_ERROR;
        वापस nullptr;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_तथ्य(&t_तथ्य, pErrorCode);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    वापस udata_cache_तथ्य_Item(inBasename, &t_तथ्य, pErrorCode);
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
अटल UBool extendICUData(UErrorCode *pErr)
{
    UDataMemory   *p_तथ्य;
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
#अगर MAP_IMPLEMENTATION==MAP_STDIO
    अटल UMutex extendICUDataMutex;
    umtx_lock(&extendICUDataMutex);
#पूर्ण_अगर
    अगर(!umtx_loadAcquire(g_Have_Tried_To_Load_Common_तथ्य)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_तथ्य = open_Common_तथ्य(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UDataMemory_init(&copyPData);
       अगर(p_तथ्य != nullptr) {
          UDatamemory_assign(&copyPData, p_तथ्य);
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

        umtx_storeRelease(g_Have_Tried_To_Load_Common_तथ्य, 1);
    }

    didUpdate = findCommonICUDataByName(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#अगर MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#पूर्ण_अगर
    वापस didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI व्योम U_EXPORT2
udata_set_Common_तथ्य(स्थिर व्योम *तथ्य, UErrorCode *pErrorCode) {
    UDataMemory तथ्य_Memory;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस;
    }

    अगर(तथ्य==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    /* set the data pointer and test for validity */
    UDataMemory_init(&तथ्य_Memory);
    UData_Memory_set_तथ्य(&तथ्य_Memory, तथ्य);
    udata_check_Common_तथ्य(&तथ्य_Memory, pErrorCode);
    अगर (U_FAILURE(*pErrorCode)) {वापस;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    setCommonICUData(&तथ्य_Memory, true, pErrorCode);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI व्योम U_EXPORT2
udata_set_App_तथ्य(स्थिर अक्षर *path, स्थिर व्योम *तथ्य, UErrorCode *err)
{
    UDataMemory     udm;

    अगर(err==nullptr || U_FAILURE(*err)) {
        वापस;
    }
    अगर(तथ्य==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    UDataMemory_init(&udm);
    UData_Memory_set_तथ्य(&udm, तथ्य);
    udata_check_Common_तथ्य(&udm, err);
    udata_cache_तथ्य_Item(path, &udm, err);
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
अटल UDataMemory *
check_तथ्य_Item
(
 स्थिर तथ्य_Header         *pHeader,         /* The data item to be checked.                */
 UDataMemoryIsAcceptable  *isAcceptable,    /* App's call-back function                    */
 व्योम                     *context,         /*   pass-thru param for above.                */
 स्थिर अक्षर               *type,            /*   pass-thru param for above.                */
 स्थिर अक्षर               *name,            /*   pass-thru param for above.                */
 UErrorCode               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UErrorCode               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UDataMemory  *r_तथ्य_Mem = nullptr;          /* the new UDataMemory, to be returned.        */

    अगर (U_FAILURE(*fatalErr)) {
        वापस nullptr;
    }

    अगर(pHeader->तथ्य_Header_2.magic1==0xda &&
        pHeader->तथ्य_Header_2.magic2==0x27 &&
        (isAcceptable==nullptr || isAcceptable(context, type, name, &pHeader->info))
    ) {
        r_तथ्य_Mem=UDataMemory_createNewInstance(fatalErr);
        अगर (U_FAILURE(*fatalErr)) {
            वापस nullptr;
        }
        r_तथ्य_Mem->pHeader = pHeader;
    } अन्यथा {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    वापस r_तथ्य_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
अटल UDataMemory *doLoadFromIndividualFiles(स्थिर अक्षर *pkgName, 
        स्थिर अक्षर *तथ्य_Path, स्थिर अक्षर *tocEntryPathSuffix,
            /* following arguments are the same as doOpenChoice itself */
            स्थिर अक्षर *path, स्थिर अक्षर *type, स्थिर अक्षर *name,
             UDataMemoryIsAcceptable *isAcceptable, व्योम *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    स्थिर अक्षर         *path_बफर;
    UDataMemory         तथ्य_Memory;
    UDataMemory *p_Entry_तथ्य;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UDataPathIterator iter(तथ्य_Path, pkgName, path, tocEntryPathSuffix, false, pErrorCode);

    जबतक ((path_बफर = iter.next(pErrorCode)) != nullptr)
    {
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "UDATA: trying individual file %s\n", path_बफर);
#पूर्ण_अगर
        अगर (uprv_mapFile(&तथ्य_Memory, path_बफर, pErrorCode))
        {
            p_Entry_तथ्य = check_तथ्य_Item(तथ्य_Memory.pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
            अगर (p_Entry_तथ्य != nullptr) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_Entry_तथ्य->mapAddr = तथ्य_Memory.mapAddr;
                p_Entry_तथ्य->map     = तथ्य_Memory.map;
                p_Entry_तथ्य->length  = तथ्य_Memory.length;

#अगर_घोषित UDATA_DEBUG
                ख_लिखो(मानक_त्रुटि, "** Mapped file: %s\n", path_बफर);
#पूर्ण_अगर
                वापस p_Entry_तथ्य;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&तथ्य_Memory);

            /* If we had a nasty error, bail out completely.  */
            अगर (U_FAILURE(*pErrorCode)) {
                वापस nullptr;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *subErrorCode=U_INVALID_FORMAT_ERROR;
        }
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "%s\n", UDataMemory_isLoaded(&तथ्य_Memory)?"LOADED":"not loaded");
#पूर्ण_अगर
    }
    वापस nullptr;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
अटल UDataMemory *do_Load_From_Common_तथ्य(UBool isICUData, स्थिर अक्षर * /*pkgName*/, 
        स्थिर अक्षर * /*dataPath*/, स्थिर अक्षर * /*tocEntryPathSuffix*/, स्थिर अक्षर *tocEntryName,
            /* following arguments are the same as doOpenChoice itself */
            स्थिर अक्षर *path, स्थिर अक्षर *type, स्थिर अक्षर *name,
             UDataMemoryIsAcceptable *isAcceptable, व्योम *context,
             UErrorCode *subErrorCode,
             UErrorCode *pErrorCode)
{
    UDataMemory        *p_Entry_तथ्य;
    स्थिर तथ्य_Header   *pHeader;
    UDataMemory        *p_Common_तथ्य;
    int32_t            common_तथ्य_सूचकांक;
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
    क्रम (common_तथ्य_सूचकांक = isICUData ? 0 : -1;;) {
        p_Common_तथ्य=open_Common_तथ्य(path, common_तथ्य_सूचकांक, subErrorCode); /** search for pkg **/

        अगर(U_SUCCESS(*subErrorCode) && p_Common_तथ्य!=nullptr) {
            int32_t length;

            /* look up the data piece in the common data */
            pHeader=p_Common_तथ्य->vFuncs->Lookup(p_Common_तथ्य, tocEntryName, &length, subErrorCode);
#अगर_घोषित UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: pHeader=%p - %s\n", tocEntryName, (व्योम*) pHeader, u_errorName(*subErrorCode));
#पूर्ण_अगर

            अगर(pHeader!=nullptr) {
                p_Entry_तथ्य = check_तथ्य_Item(pHeader, isAcceptable, context, type, name, subErrorCode, pErrorCode);
#अगर_घोषित UDATA_DEBUG
                ख_लिखो(मानक_त्रुटि, "pEntryData=%p\n", (व्योम*) p_Entry_तथ्य);
#पूर्ण_अगर
                अगर (U_FAILURE(*pErrorCode)) {
                    वापस nullptr;
                }
                अगर (p_Entry_तथ्य != nullptr) {
                    p_Entry_तथ्य->length = length;
                    वापस p_Entry_तथ्य;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        अगर (*subErrorCode == U_MEMORY_ALLOCATION_ERROR) {
            *pErrorCode = *subErrorCode;
            वापस nullptr;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        अगर (!isICUData) {
            वापस nullptr;
        } अन्यथा अगर (p_Common_तथ्य != nullptr) {
            ++common_तथ्य_सूचकांक;  /* try the next data package */
        } अन्यथा अगर ((!checkedExtendedICUData) && extendICUData(subErrorCode)) {
            checkedExtendedICUData = true;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } अन्यथा {
            वापस nullptr;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
अटल UBool isTimeZoneFile(स्थिर अक्षर *name, स्थिर अक्षर *type) {
    वापस ((uprv_strcmp(type, "res") == 0) &&
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
अटल UDataMemory *
doOpenChoice(स्थिर अक्षर *path, स्थिर अक्षर *type, स्थिर अक्षर *name,
             UDataMemoryIsAcceptable *isAcceptable, व्योम *context,
             UErrorCode *pErrorCode)
{
    UDataMemory         *retVal = nullptr;

    स्थिर अक्षर         *तथ्य_Path;

    int32_t             toc_Entry_Suffix_सूचकांक;
    स्थिर अक्षर         *tocEntryPathSuffix;
    UErrorCode          subErrorCode=U_ZERO_ERROR;
    स्थिर अक्षर         *वृक्ष_Char;

    UBool               isICUData = false;


    FileTracer::traceOpen(path, type, name);


    /* Is this path ICU data? */
    अगर(path == nullptr ||
       !म_भेद(path, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(path, U_ICUDATA_NAME U_वृक्ष_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_वृक्ष_SEPARATOR_STRING)) ||  
       !uprv_strncmp(path, U_ICUDATA_ALIAS U_वृक्ष_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_वृक्ष_SEPARATOR_STRING))) {
      isICUData = true;
    }

#अगर (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString altSepPath;
    अगर(path) {
        अगर(uprv_strchr(path,U_FILE_ALT_SEP_CHAR) != nullptr) {
            altSepPath.append(path, *pErrorCode);
            अक्षर *p;
            जबतक ((p = uprv_strchr(altSepPath.तथ्य(), U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
#अगर defined (UDATA_DEBUG)
            ख_लिखो(मानक_त्रुटि, "Changed path from [%s] to [%s]\n", path, altSepPath.तथ्य());
#पूर्ण_अगर
            path = altSepPath.तथ्य();
        }
    }
#पूर्ण_अगर

    CharString tocEntryName; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString tocEntryPath; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString वृक्ष_Name;

    /* ======= Set up strings */
    अगर(path==nullptr) {
        pkgName.append(U_ICUDATA_NAME, *pErrorCode);
    } अन्यथा {
        स्थिर अक्षर *pkg;
        स्थिर अक्षर *first;
        pkg = uprv_strrchr(path, U_FILE_SEP_CHAR);
        first = uprv_strchr(path, U_FILE_SEP_CHAR);
        अगर(uprv_pathIsAbsolute(path) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            अगर(pkg) {
                pkgName.append(pkg+1, *pErrorCode);
            } अन्यथा {
                pkgName.append(path, *pErrorCode);
            }
        } अन्यथा {
            वृक्ष_Char = uprv_strchr(path, U_वृक्ष_SEPARATOR);
            अगर(वृक्ष_Char) { 
                वृक्ष_Name.append(वृक्ष_Char+1, *pErrorCode); /* following '-' */
                अगर(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } अन्यथा {
                    pkgName.append(path, static_cast<int32_t>(वृक्ष_Char - path), *pErrorCode);
                    अगर (first == nullptr) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        path = pkgName.तथ्य();
                    }
                }
            } अन्यथा {
                अगर(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *pErrorCode);
                } अन्यथा {
                    pkgName.append(path, *pErrorCode);
                }
            }
        }
    }

#अगर_घोषित UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, " P=%s T=%s\n", pkgName.तथ्य(), वृक्ष_Name.तथ्य());
#पूर्ण_अगर

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    tocEntryName.append(pkgName, *pErrorCode);
    tocEntryPath.append(pkgName, *pErrorCode);
    toc_Entry_Suffix_सूचकांक = tocEntryName.length();

    अगर(!वृक्ष_Name.isEmpty()) {
        tocEntryName.append(U_वृक्ष_ENTRY_SEP_CHAR, *pErrorCode).append(वृक्ष_Name, *pErrorCode);
        tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(वृक्ष_Name, *pErrorCode);
    }

    tocEntryName.append(U_वृक्ष_ENTRY_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    tocEntryPath.append(U_FILE_SEP_CHAR, *pErrorCode).append(name, *pErrorCode);
    अगर(type!=nullptr && *type!=0) {
        tocEntryName.append(".", *pErrorCode).append(type, *pErrorCode);
        tocEntryPath.append(".", *pErrorCode).append(type, *pErrorCode);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    tocEntryPathSuffix = tocEntryPath.तथ्य() + toc_Entry_Suffix_सूचकांक + 1; /* suffix starts here */

#अगर_घोषित UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, " tocEntryName = %s\n", tocEntryName.तथ्य());
    ख_लिखो(मानक_त्रुटि, " tocEntryPath = %s\n", tocEntryPath.तथ्य());
#पूर्ण_अगर

#अगर !defined(ICU_तथ्य_DIR_WINDOWS)
    अगर(path == nullptr) {
        path = COMMON_तथ्य_NAME; /* "icudt26e" */
    }
#अन्यथा
    // When using the Windows system data, we expects only a single data file.
    path = COMMON_तथ्य_NAME; /* "icudt26e" */
#पूर्ण_अगर

    /************************ Begin loop looking for ind. files ***************/
#अगर_घोषित UDATA_DEBUG
    ख_लिखो(मानक_त्रुटि, "IND: inBasename = %s, pkg=%s\n", "(n/a)", packageNameFromPath(path));
#पूर्ण_अगर

    /* End of dealing with a null basename */
    तथ्य_Path = u_get_तथ्य_Directory();

    /****    Time zone individual files override  */
    अगर (isICUData && type != nullptr && isTimeZoneFile(name, type)) {
        स्थिर अक्षर *tzFilesDir = u_getTimeZoneFilesDirectory(pErrorCode);
        अगर (tzFilesDir[0] != 0) {
#अगर_घोषित UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#पूर्ण_अगर
            retVal = doLoadFromIndividualFiles(/* pkgName.data() */ "", tzFilesDir, tocEntryPathSuffix,
                            /* path */ "", type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            अगर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                वापस retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    अगर(g_तथ्य_File_Access == UDATA_PACKAGES_FIRST) {
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#पूर्ण_अगर
        /* #2 */
        retVal = do_Load_From_Common_तथ्य(isICUData, 
                            pkgName.तथ्य(), तथ्य_Path, tocEntryPathSuffix, tocEntryName.तथ्य(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        अगर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            वापस retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    अगर((g_तथ्य_File_Access==UDATA_PACKAGES_FIRST) ||
       (g_तथ्य_File_Access==UDATA_FILES_FIRST)) {
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying individual files\n");
#पूर्ण_अगर
        /* Check to make sure that there is a dataPath to iterate over */
        अगर ((तथ्य_Path && *तथ्य_Path) || !isICUData) {
            retVal = doLoadFromIndividualFiles(pkgName.तथ्य(), तथ्य_Path, tocEntryPathSuffix,
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
            अगर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
                वापस retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    अगर((g_तथ्य_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_तथ्य_File_Access==UDATA_FILES_FIRST)) {
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#पूर्ण_अगर
        retVal = do_Load_From_Common_तथ्य(isICUData,
                            pkgName.तथ्य(), तथ्य_Path, tocEntryPathSuffix, tocEntryName.तथ्य(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        अगर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            वापस retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    अगर(g_तथ्य_File_Access==UDATA_NO_FILES) {
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "Trying common data (UDATA_NO_FILES)\n");
#पूर्ण_अगर
        retVal = do_Load_From_Common_तथ्य(isICUData,
                            pkgName.तथ्य(), "", tocEntryPathSuffix, tocEntryName.तथ्य(),
                            path, type, name, isAcceptable, context, &subErrorCode, pErrorCode);
        अगर((retVal != nullptr) || U_FAILURE(*pErrorCode)) {
            वापस retVal;
        }
    }

    /* data not found */
    अगर(U_SUCCESS(*pErrorCode)) {
        अगर(U_SUCCESS(subErrorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } अन्यथा {
            /* entry point not found or rejected */
            *pErrorCode=subErrorCode;
        }
    }
    वापस retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(स्थिर अक्षर *path, स्थिर अक्षर *type, स्थिर अक्षर *name,
           UErrorCode *pErrorCode) {
#अगर_घोषित UDATA_DEBUG
  ख_लिखो(मानक_त्रुटि, "udata_open(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
    ख_साफ(मानक_त्रुटि);
#पूर्ण_अगर

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    } अन्यथा अगर(name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    } अन्यथा {
        वापस doOpenChoice(path, type, name, nullptr, nullptr, pErrorCode);
    }
}



U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(स्थिर अक्षर *path, स्थिर अक्षर *type, स्थिर अक्षर *name,
                 UDataMemoryIsAcceptable *isAcceptable, व्योम *context,
                 UErrorCode *pErrorCode) {
#अगर_घोषित UDATA_DEBUG
  ख_लिखो(मानक_त्रुटि, "udata_openChoice(): Opening: %s : %s . %s\n", (path?path:"nullptr"), name, type);
#पूर्ण_अगर

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    } अन्यथा अगर(name==nullptr || *name==0 || isAcceptable==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    } अन्यथा {
        वापस doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}



U_CAPI व्योम U_EXPORT2
udata_getInfo(UDataMemory *p_तथ्य, UDataInfo *pInfo) {
    अगर(pInfo!=nullptr) {
        अगर(p_तथ्य!=nullptr && p_तथ्य->pHeader!=nullptr) {
            स्थिर UDataInfo *info=&p_तथ्य->pHeader->info;
            uint16_t तथ्य_Info_आकार=udata_get_Info_आकार(info);
            अगर(pInfo->size>तथ्य_Info_आकार) {
                pInfo->size=तथ्य_Info_आकार;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (स्थिर uint16_t *)info+1, pInfo->size-2);
            अगर(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reservedWord;
                pInfo->reservedWord=(uint16_t)((x<<8)|(x>>8));
            }
        } अन्यथा {
            pInfo->size=0;
        }
    }
}


U_CAPI व्योम U_EXPORT2 udata_setFileAccess(UDataFileAccess access, UErrorCode * /*status*/)
{
    // Note: this function is documented as not thread safe.
    g_तथ्य_File_Access = access;
}
