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

#यदि_परिभाषितम् __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#यदि_अन्तः

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
#यदि_अन्तः

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
स्थिर UData_स्मृतिः *udata_find_Cached_दत्त(ध्रुव वर्ण *मार्गः, UError_संहिता &err);

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
स्थिर UData_स्मृतिः *gCommonICUDataArray[10] = { शून्यसूचकः };   // Access protected by icu global mutex.

स्थिर u_atomic_int32_t g_Have_Tried_गन्तव्यम्_आरोपय_Common_दत्त {0};  //  See extendICUData().

स्थिर UHashtable  *g_Common_दत्त_Cache = शून्यसूचकः;  /* Global hash table of opened ICU data files.  */
स्थिर icu::UInitOnce g_Common_दत्त_Cache_Init_Once {};

#यदि !defined(ICU_दत्त_DIR_WINDOWS)
स्थिर UDataFileAccess  g_दत्त_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#अन्यथा
// If we are using the Windows data directory, then look in one spot only.
स्थिर UDataFileAccess  g_दत्त_File_Access = UDATA_NO_FILES;
#यदि_अन्तः

स्थिर UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    यदि (g_Common_दत्त_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_दत्त_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_दत्त_Cache = शून्यसूचकः;        /*   Cleanup is not thread safe.                */
    }
    g_Common_दत्त_Cache_Init_Once.reset();

    प्रति (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != शून्यसूचकः; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = शून्यसूचकः;
    }
    g_Have_Tried_गन्तव्यम्_आरोपय_Common_दत्त = 0;

    प्रत्यागम सत्य;                   /* Everything was cleaned up */
}

स्थिर UBool U_CALLCONV
findCommonICUDataByName(ध्रुव वर्ण *अन्तर्_Basename, UError_संहिता &err)
{
    UBool प्राप्तम् = असत्य;
    int32_t i;

    UData_स्मृतिः  *p_दत्त = udata_find_Cached_दत्त(अन्तर्_Basename, err);
    यदि (U_FAILURE(err) || p_दत्त == शून्यसूचकः)
        प्रत्यागम असत्य;

    {
        Mutex तालकम्;
        प्रति (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            यदि ((gCommonICUDataArray[i] != शून्यसूचकः) && (gCommonICUDataArray[i]->p_शीर्षकम् == p_दत्त->p_शीर्षकम्)) {
                /* The data pointer is already in the array. */
                प्राप्तम् = सत्य;
                भङ्ग;
            }
        }
    }
    प्रत्यागम प्राप्तम्;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
स्थिर UBool
समुच्चयः_Common_ICUData(UData_स्मृतिः *p_दत्त,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_संहिता *pErr)
{
    UData_स्मृतिः  *new_Common_दत्त = UData_स्मृतिः_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = असत्य;
    यदि (U_FAILURE(*pErr)) {
        प्रत्यागम असत्य;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_नियोजय(new_Common_दत्त, p_दत्त);
    umtx_तालकम्(शून्यसूचकः);
    प्रति (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        यदि (gCommonICUDataArray[i] == शून्यसूचकः) {
            gCommonICUDataArray[i] = new_Common_दत्त;
            didUpdate = सत्य;
            भङ्ग;
        } अन्यथा यदि (gCommonICUDataArray[i]->p_शीर्षकम् == p_दत्त->p_शीर्षकम्) {
            /* The same data pointer is already in the array. */
            भङ्ग;
        }
    }
    umtx_unlock(शून्यसूचकः);

    यदि (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    यदि (didUpdate) {
        ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } अन्यथा {
        uprv_free(new_Common_दत्त);
    }
    प्रत्यागम didUpdate;
}

#यदि !defined(ICU_दत्त_DIR_WINDOWS)

स्थिर UBool
समुच्चयः_Common_ICUData_Pointer(ध्रुव शून्य *p_दत्त, UBool /*warn*/, UError_संहिता *p_Error_संहिता) {
    UData_स्मृतिः t_दत्त;
    UData_स्मृतिः_init(&t_दत्त);
    UData_स्मृतिः_समुच्चयः_दत्त(&t_दत्त, p_दत्त);
    udata_check_Common_दत्त(&t_दत्त, p_Error_संहिता);
    प्रत्यागम समुच्चयः_Common_ICUData(&t_दत्त, असत्य, p_Error_संहिता);
}

#यदि_अन्तः

स्थिर ध्रुव वर्ण *
findBasename(ध्रुव वर्ण *मार्गः) {
    ध्रुव वर्ण *basename=uprv_strrchr(मार्गः, U_FILE_SEP_CHAR);
    यदि(basename==शून्यसूचकः) {
        प्रत्यागम मार्गः;
    } अन्यथा {
        प्रत्यागम basename+1;
    }
}

#यदि_परिभाषितम् UDATA_DEBUG
स्थिर ध्रुव वर्ण *
package_Name_From_मार्गः(ध्रुव वर्ण *मार्गः)
{
    यदि((मार्गः == शून्यसूचकः) || (*मार्गः == 0)) {
        प्रत्यागम U_ICUDATA_NAME;
    }

    मार्गः = findBasename(मार्गः);

    यदि((मार्गः == शून्यसूचकः) || (*मार्गः == 0)) {
        प्रत्यागम U_ICUDATA_NAME;
    }

    प्रत्यागम मार्गः;
}
#यदि_अन्तः

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

प्रकारनिर्देश संरचना दत्त_Cache_Element {
    वर्ण          *name;
    UData_स्मृतिः   *item;
} दत्त_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
स्थिर शून्य U_CALLCONV दत्त_Cache_Element_deleter(शून्य *pDCEl) {
    दत्त_Cache_Element* p = static_cast<दत्त_Cache_Element*>(pDCEl);
    udata_close(p->item);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

स्थिर शून्य U_CALLCONV udata_init_Hash_सारणी(UError_संहिता &err) {
    U_ASSERT(g_Common_दत्त_Cache == शून्यसूचकः);
    g_Common_दत्त_Cache = uhash_open(uhash_hashChars, uhash_compareChars, शून्यसूचकः, &err);
    यदि (U_FAILURE(err)) {
       प्रत्यागम;
    }
    U_ASSERT(g_Common_दत्त_Cache != शून्यसूचकः);
    uhash_समुच्चयः_मानम्_Deleter(g_Common_दत्त_Cache, दत्त_Cache_Element_deleter);
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
स्थिर UHashtable *udata_get_Hash_सारणी(UError_संहिता &err) {
    umtx_initOnce(g_Common_दत्त_Cache_Init_Once, &udata_init_Hash_सारणी, err);
    प्रत्यागम g_Common_दत्त_Cache;
}



स्थिर UData_स्मृतिः *udata_find_Cached_दत्त(ध्रुव वर्ण *मार्गः, UError_संहिता &err)
{
    UHashtable        *htable;
    UData_स्मृतिः       *retVal = शून्यसूचकः;
    दत्त_Cache_Element  *el;
    ध्रुव वर्ण        *आधारः_Name;

    htable = udata_get_Hash_सारणी(err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम शून्यसूचकः;
    }

    आधारः_Name = findBasename(मार्गः);   /* Cache remembers only the base name, not the full path. */
    umtx_तालकम्(शून्यसूचकः);
    el = static_cast<दत्त_Cache_Element*>(uhash_get(htable, आधारः_Name));
    umtx_unlock(शून्यसूचकः);
    यदि (el != शून्यसूचकः) {
        retVal = el->item;
    }
#यदि_परिभाषितम् UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", आधारः_Name, (शून्य*) retVal);
#यदि_अन्तः
    प्रत्यागम retVal;
}


स्थिर UData_स्मृतिः *udata_cache_दत्त_Item(ध्रुव वर्ण *मार्गः, UData_स्मृतिः *item, UError_संहिता *pErr) {
    दत्त_Cache_Element *newElement;
    ध्रुव वर्ण       *आधारः_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    दत्त_Cache_Element *old_मानम् = शून्यसूचकः;
    UError_संहिता        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_सारणी(*pErr);
    यदि (U_FAILURE(*pErr)) {
        प्रत्यागम शून्यसूचकः;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<दत्त_Cache_Element*>(uprv_malloc(परिमाण(दत्त_Cache_Element)));
    यदि (newElement == शून्यसूचकः) {
        *pErr = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    newElement->item = UData_स्मृतिः_create_New_Instance(pErr);
    यदि (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        प्रत्यागम शून्यसूचकः;
    }
    UDatamemory_नियोजय(newElement->item, item);

    आधारः_Name = findBasename(मार्गः);
    nameLen = static_cast<int32_t>(uprv_strlen(आधारः_Name));
    newElement->name = static_cast<वर्ण*>(uprv_malloc(nameLen + 1));
    यदि (newElement->name == शून्यसूचकः) {
        *pErr = U_स्मृतिः_आवण्टनम्_ERROR;
        uprv_free(newElement->item);
        uprv_free(newElement);
        प्रत्यागम शून्यसूचकः;
    }
    uprv_strcpy(newElement->name, आधारः_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_तालकम्(शून्यसूचकः);
    old_मानम् = static_cast<दत्त_Cache_Element*>(uhash_get(htable, मार्गः));
    यदि (old_मानम् != शून्यसूचकः) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    अन्यथा {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(शून्यसूचकः);

#यदि_परिभाषितम् UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (शून्य*) newElement->item, u_errorName(subErr), (शून्य*) newElement->item->vFuncs);
#यदि_अन्तः

    यदि (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->item);
        uprv_free(newElement);
        प्रत्यागम old_मानम् ? old_मानम्->item : शून्यसूचकः;
    }

    प्रत्यागम newElement->item;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

वर्ग UData_मार्गः_Iterator
{
सार्वजनिक:
    UData_मार्गः_Iterator(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *pkg,
                      ध्रुव वर्ण *item, ध्रुव वर्ण *suffix, UBool do_Check_चरमः_Four,
                      UError_संहिता *p_Error_संहिता);
    ध्रुव वर्ण *next(UError_संहिता *p_Error_संहिता);

गुप्त:
    ध्रुव वर्ण *मार्गः;                              /* working path (u_icudata_Dir) */
    ध्रुव वर्ण *अग्र_मार्गः;                          /* path following this one */
    ध्रुव वर्ण *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  item_मार्गः;                          /* path passed in with item name */
    CharString  मार्गः_बफर;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       check_चरमः_Four;                     /* if true then allow paths such as '/foo/myapp.dat'
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
UData_मार्गः_Iterator::UData_मार्गः_Iterator(ध्रुव वर्ण *अन्तर्_मार्गः, ध्रुव वर्ण *pkg,
                                     ध्रुव वर्ण *item, ध्रुव वर्ण *अन्तर्_Suffix, UBool do_Check_चरमः_Four,
                                     UError_संहिता *p_Error_संहिता)
{
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", अन्तर्_Suffix, अन्तर्_मार्गः);
#यदि_अन्तः
    /** Path **/
    यदि(अन्तर्_मार्गः == शून्यसूचकः) {
        मार्गः = u_get_दत्त_Directory();
    } अन्यथा {
        मार्गः = अन्तर्_मार्गः;
    }

    /** Package **/
    यदि(pkg != शून्यसूचकः) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_संहिता).append(pkg, *p_Error_संहिता);
#यदि_परिभाषितम् UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.दत्त(), packageStub.length());
#यदि_अन्तः
    }

    /** Item **/
    basename = findBasename(item);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    यदि(basename == item) {
        अग्र_मार्गः = मार्गः;
    } अन्यथा {
        item_मार्गः.append(item, static_cast<int32_t>(basename - item), *p_Error_संहिता);
        अग्र_मार्गः = item_मार्गः.दत्त();
    }
#यदि_परिभाषितम् UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", अन्तर्_Suffix, (शून्य*) अन्तर्_Suffix);
#यदि_अन्तः

    /** Suffix  **/
    यदि(अन्तर्_Suffix != शून्यसूचकः) {
        suffix = अन्तर्_Suffix;
    } अन्यथा {
        suffix = "";
    }

    check_चरमः_Four = do_Check_चरमः_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#यदि_परिभाषितम् UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            item,
            मार्गः,
            basename,
            suffix.दत्त(),
            item_मार्गः.दत्त(),
            अग्र_मार्गः,
            check_चरमः_Four?"true":"false");
#यदि_अन्तः
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
ध्रुव वर्ण *UData_मार्गः_Iterator::next(UError_संहिता *p_Error_संहिता)
{
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    ध्रुव वर्ण *current_मार्गः = शून्यसूचकः;
    int32_t     मार्गः_Len = 0;
    ध्रुव वर्ण *मार्गः_Basename;

    कुरु
    {
        यदि( अग्र_मार्गः == शून्यसूचकः ) {
            भङ्ग;
        }
        current_मार्गः = अग्र_मार्गः;

        यदि(अग्र_मार्गः == item_मार्गः.दत्त()) { /* we were processing item's path. */
            अग्र_मार्गः = मार्गः; /* start with regular path next tm. */
            मार्गः_Len = static_cast<int32_t>(uprv_strlen(current_मार्गः));
        } अन्यथा {
            /* fix up next for next time */
            अग्र_मार्गः = uprv_strchr(current_मार्गः, U_मार्गः_SEP_CHAR);
            यदि(अग्र_मार्गः == शून्यसूचकः) {
                /* segment: entire path */
                मार्गः_Len = static_cast<int32_t>(uprv_strlen(current_मार्गः));
            } अन्यथा {
                /* segment: until next segment */
                मार्गः_Len = static_cast<int32_t>(अग्र_मार्गः - current_मार्गः);
                /* skip divider */
                अग्र_मार्गः ++;
            }
        }

        यदि(मार्गः_Len == 0) {
            अनुवर्त;
        }

#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_मार्गः);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            प्रति(qqq=0;qqq<मार्गः_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#यदि_अन्तः
        मार्गः_बफर.clear().append(current_मार्गः, मार्गः_Len, *p_Error_संहिता);

        /* check for .dat files */
        मार्गः_Basename = findBasename(मार्गः_बफर.दत्त());

        यदि(check_चरमः_Four && 
           (मार्गः_Len>=4) &&
           uprv_strncmp(मार्गः_बफर.दत्त() +(मार्गः_Len-4), suffix.दत्त(), 4)==0 && /* suffix matches */
           uprv_strncmp(findBasename(मार्गः_बफर.दत्त()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(मार्गः_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#यदि_परिभाषितम् UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.दत्त(), मार्गः_बफर.दत्त());
#यदि_अन्तः
            /* do nothing */
        }
        अन्यथा 
        {       /* regular dir path */
            यदि(मार्गः_बफर[मार्गः_Len-1] != U_FILE_SEP_CHAR) {
                यदि((मार्गः_Len>=4) &&
                   uprv_strncmp(मार्गः_बफर.दत्त()+(मार्गः_Len-4), ".dat", 4) == 0)
                {
#यदि_परिभाषितम् UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", मार्गः_बफर.दत्त());
#यदि_अन्तः
                    अनुवर्त;
                }

                /* Check if it is a directory with the same name as our package */
                यदि(!packageStub.is_रिक्तानि() &&
                   (मार्गः_Len > packageStub.length()) &&
                   !uprv_strcmp(मार्गः_बफर.दत्त() + मार्गः_Len - packageStub.length(), packageStub.दत्त())) {
#यदि_परिभाषितम् UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.दत्त(), basename, basenameLen);
#यदि_अन्तः
                  मार्गः_बफर.truncate(मार्गः_Len - packageStub.length());
                }
                मार्गः_बफर.append(U_FILE_SEP_CHAR, *p_Error_संहिता);
            }

            /* + basename */
            मार्गः_बफर.append(packageStub.दत्त()+1, packageStub.length()-1, *p_Error_संहिता);

            यदि (!suffix.empty())  /* tack on suffix */
            {
                यदि (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    मार्गः_बफर.ensure_अन्ताः_With_File_Separator(*p_Error_संहिता);
                }
                मार्गः_बफर.append(suffix, *p_Error_संहिता);
            }
        }

#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", मार्गः_बफर.दत्त());
#यदि_अन्तः

        प्रत्यागम मार्गः_बफर.दत्त();

    } यावत्(मार्गः);

    /* fell way off the end */
    प्रत्यागम शून्यसूचकः;
}

U_NAMESPACE_अन्तः

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#यदि !defined(ICU_दत्त_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
बाह्य "C" ध्रुव दत्त_शीर्षकम् U_दत्त_API U_ICUDATA_प्रविष्टिः_POINT;
#यदि_अन्तः

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
स्थिर UData_स्मृतिः *
open_Common_दत्त(ध्रुव वर्ण *मार्गः,          /*  Path from OpenChoice?          */
               int32_t common_दत्त_सूचकांक,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_संहिता *p_Error_संहिता)
{
    UData_स्मृतिः t_दत्त;
    ध्रुव वर्ण *मार्गः_बफर;
    ध्रुव वर्ण *अन्तर्_Basename;

    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    UData_स्मृतिः_init(&t_दत्त);

    /* ??????? TODO revisit this */ 
    यदि (common_दत्त_सूचकांक >= 0) {
        /* "mini-cache" for common ICU data */
        यदि(common_दत्त_सूचकांक >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            प्रत्यागम शून्यसूचकः;
        }
        {
            Mutex तालकम्;
            यदि(gCommonICUDataArray[common_दत्त_सूचकांक] != शून्यसूचकः) {
                प्रत्यागम gCommonICUDataArray[common_दत्त_सूचकांक];
            }
#यदि !defined(ICU_दत्त_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            प्रति(i = 0; i < common_दत्त_सूचकांक; ++i) {
                यदि(gCommonICUDataArray[i]->p_शीर्षकम् == &U_ICUDATA_प्रविष्टिः_POINT) {
                    /* The linked-in data is already in the list. */
                    प्रत्यागम शून्यसूचकः;
                }
            }
#यदि_अन्तः
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
#यदि !defined(ICU_दत्त_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        समुच्चयः_Common_ICUData_Pointer(&U_ICUDATA_प्रविष्टिः_POINT, असत्य, p_Error_संहिता);
        {
            Mutex तालकम्;
            प्रत्यागम gCommonICUDataArray[common_दत्त_सूचकांक];
        }
#यदि_अन्तः
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    अन्तर्_Basename = findBasename(मार्गः);
#यदि_परिभाषितम् UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", अन्तर्_Basename);
#यदि_अन्तः

    यदि(*अन्तर्_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", मार्गः);
#यदि_अन्तः
        यदि (U_SUCCESS(*p_Error_संहिता)) {
            *p_Error_संहिता=U_FILE_ACCESS_ERROR;
        }
        प्रत्यागम शून्यसूचकः;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_स्मृतिः  *दत्त_गन्तव्यम्_Return = udata_find_Cached_दत्त(अन्तर्_Basename, *p_Error_संहिता);
    यदि (दत्त_गन्तव्यम्_Return != शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम दत्त_गन्तव्यम्_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_मार्गः_Iterator iter(u_get_दत्त_Directory(), अन्तर्_Basename, मार्गः, ".dat", सत्य, p_Error_संहिता);

    यावत् ((UData_स्मृतिः_is_Loaded(&t_दत्त)==असत्य) && (मार्गः_बफर = iter.next(p_Error_संहिता)) != शून्यसूचकः)
    {
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", मार्गः_बफर);
#यदि_अन्तः
        uprv_mapFile(&t_दत्त, मार्गः_बफर, p_Error_संहिता);
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_स्मृतिः_is_Loaded(&t_दत्त)?"LOADED":"not loaded");
#यदि_अन्तः
    }
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (!UData_स्मृतिः_is_Loaded(&t_दत्त)) {
        /* no common data */
        *p_Error_संहिता=U_FILE_ACCESS_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_दत्त(&t_दत्त, p_Error_संहिता);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    प्रत्यागम udata_cache_दत्त_Item(अन्तर्_Basename, &t_दत्त, p_Error_संहिता);
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
स्थिर UBool extendICUData(UError_संहिता *pErr)
{
    UData_स्मृतिः   *p_दत्त;
    UData_स्मृतिः   copyPData;
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
    umtx_तालकम्(&extendICUDataMutex);
#यदि_अन्तः
    यदि(!umtx_आरोपय_Acquire(g_Have_Tried_गन्तव्यम्_आरोपय_Common_दत्त)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_दत्त = open_Common_दत्त(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_स्मृतिः_init(&copyPData);
       यदि(p_दत्त != शून्यसूचकः) {
          UDatamemory_नियोजय(&copyPData, p_दत्त);
          copyPData.map = शून्यसूचकः;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = शून्यसूचकः; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              समुच्चयः_Common_ICUData(&copyPData,/*  The new common data.                                */
                       असत्य,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_गन्तव्यम्_आरोपय_Common_दत्त, 1);
    }

    didUpdate = findCommonICUDataByName(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#यदि MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#यदि_अन्तः
    प्रत्यागम didUpdate;               /* Return true if ICUData pointer was updated.   */
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
udata_समुच्चयः_Common_दत्त(ध्रुव शून्य *दत्त, UError_संहिता *p_Error_संहिता) {
    UData_स्मृतिः दत्त_स्मृतिः;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }

    यदि(दत्त==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    /* set the data pointer and test for validity */
    UData_स्मृतिः_init(&दत्त_स्मृतिः);
    UData_स्मृतिः_समुच्चयः_दत्त(&दत्त_स्मृतिः, दत्त);
    udata_check_Common_दत्त(&दत्त_स्मृतिः, p_Error_संहिता);
    यदि (U_FAILURE(*p_Error_संहिता)) {प्रत्यागम;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    समुच्चयः_Common_ICUData(&दत्त_स्मृतिः, सत्य, p_Error_संहिता);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI शून्य U_EXPORT2
udata_समुच्चयः_App_दत्त(ध्रुव वर्ण *मार्गः, ध्रुव शून्य *दत्त, UError_संहिता *err)
{
    UData_स्मृतिः     udm;

    यदि(err==शून्यसूचकः || U_FAILURE(*err)) {
        प्रत्यागम;
    }
    यदि(दत्त==शून्यसूचकः) {
        *err=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    UData_स्मृतिः_init(&udm);
    UData_स्मृतिः_समुच्चयः_दत्त(&udm, दत्त);
    udata_check_Common_दत्त(&udm, err);
    udata_cache_दत्त_Item(मार्गः, &udm, err);
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
स्थिर UData_स्मृतिः *
check_दत्त_Item
(
 ध्रुव दत्त_शीर्षकम्         *p_शीर्षकम्,         /* The data item to be checked.                */
 UData_स्मृतिः_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 शून्य                     *context,         /*   pass-thru param for above.                */
 ध्रुव वर्ण               *type,            /*   pass-thru param for above.                */
 ध्रुव वर्ण               *name,            /*   pass-thru param for above.                */
 UError_संहिता               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_संहिता               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_स्मृतिः  *r_दत्त_Mem = शून्यसूचकः;          /* the new UDataMemory, to be returned.        */

    यदि (U_FAILURE(*fatalErr)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि(p_शीर्षकम्->दत्त_शीर्षकम्_2.magic1==0xda &&
        p_शीर्षकम्->दत्त_शीर्षकम्_2.magic2==0x27 &&
        (isAcceptable==शून्यसूचकः || isAcceptable(context, type, name, &p_शीर्षकम्->info))
    ) {
        r_दत्त_Mem=UData_स्मृतिः_create_New_Instance(fatalErr);
        यदि (U_FAILURE(*fatalErr)) {
            प्रत्यागम शून्यसूचकः;
        }
        r_दत्त_Mem->p_शीर्षकम् = p_शीर्षकम्;
    } अन्यथा {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    प्रत्यागम r_दत्त_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
स्थिर UData_स्मृतिः *do_आरोपय_From_Individual_Files(ध्रुव वर्ण *pkgName, 
        ध्रुव वर्ण *दत्त_मार्गः, ध्रुव वर्ण *toc_प्रविष्टिः_मार्गः_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *type, ध्रुव वर्ण *name,
             UData_स्मृतिः_Is_Acceptable *isAcceptable, शून्य *context,
             UError_संहिता *sub_Error_संहिता,
             UError_संहिता *p_Error_संहिता)
{
    ध्रुव वर्ण         *मार्गः_बफर;
    UData_स्मृतिः         दत्त_स्मृतिः;
    UData_स्मृतिः *p_प्रविष्टिः_दत्त;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_मार्गः_Iterator iter(दत्त_मार्गः, pkgName, मार्गः, toc_प्रविष्टिः_मार्गः_Suffix, असत्य, p_Error_संहिता);

    यावत् ((मार्गः_बफर = iter.next(p_Error_संहिता)) != शून्यसूचकः)
    {
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", मार्गः_बफर);
#यदि_अन्तः
        यदि (uprv_mapFile(&दत्त_स्मृतिः, मार्गः_बफर, p_Error_संहिता))
        {
            p_प्रविष्टिः_दत्त = check_दत्त_Item(दत्त_स्मृतिः.p_शीर्षकम्, isAcceptable, context, type, name, sub_Error_संहिता, p_Error_संहिता);
            यदि (p_प्रविष्टिः_दत्त != शून्यसूचकः) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_प्रविष्टिः_दत्त->mapAddr = दत्त_स्मृतिः.mapAddr;
                p_प्रविष्टिः_दत्त->map     = दत्त_स्मृतिः.map;
                p_प्रविष्टिः_दत्त->length  = दत्त_स्मृतिः.length;

#यदि_परिभाषितम् UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", मार्गः_बफर);
#यदि_अन्तः
                प्रत्यागम p_प्रविष्टिः_दत्त;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&दत्त_स्मृतिः);

            /* If we had a nasty error, bail out completely.  */
            यदि (U_FAILURE(*p_Error_संहिता)) {
                प्रत्यागम शून्यसूचकः;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_संहिता=U_INVALID_FORMAT_ERROR;
        }
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_स्मृतिः_is_Loaded(&दत्त_स्मृतिः)?"LOADED":"not loaded");
#यदि_अन्तः
    }
    प्रत्यागम शून्यसूचकः;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
स्थिर UData_स्मृतिः *do_आरोपय_From_Common_दत्त(UBool isICUData, ध्रुव वर्ण * /*pkgName*/, 
        ध्रुव वर्ण * /*dataPath*/, ध्रुव वर्ण * /*tocEntryPathSuffix*/, ध्रुव वर्ण *toc_प्रविष्टिः_Name,
            /* following arguments are the same as doOpenChoice itself */
            ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *type, ध्रुव वर्ण *name,
             UData_स्मृतिः_Is_Acceptable *isAcceptable, शून्य *context,
             UError_संहिता *sub_Error_संहिता,
             UError_संहिता *p_Error_संहिता)
{
    UData_स्मृतिः        *p_प्रविष्टिः_दत्त;
    ध्रुव दत्त_शीर्षकम्   *p_शीर्षकम्;
    UData_स्मृतिः        *p_Common_दत्त;
    int32_t            common_दत्त_सूचकांक;
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
    प्रति (common_दत्त_सूचकांक = isICUData ? 0 : -1;;) {
        p_Common_दत्त=open_Common_दत्त(मार्गः, common_दत्त_सूचकांक, sub_Error_संहिता); /** search for pkg **/

        यदि(U_SUCCESS(*sub_Error_संहिता) && p_Common_दत्त!=शून्यसूचकः) {
            int32_t length;

            /* look up the data piece in the common data */
            p_शीर्षकम्=p_Common_दत्त->vFuncs->Lookup(p_Common_दत्त, toc_प्रविष्टिः_Name, &length, sub_Error_संहिता);
#यदि_परिभाषितम् UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_प्रविष्टिः_Name, (शून्य*) p_शीर्षकम्, u_errorName(*sub_Error_संहिता));
#यदि_अन्तः

            यदि(p_शीर्षकम्!=शून्यसूचकः) {
                p_प्रविष्टिः_दत्त = check_दत्त_Item(p_शीर्षकम्, isAcceptable, context, type, name, sub_Error_संहिता, p_Error_संहिता);
#यदि_परिभाषितम् UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (शून्य*) p_प्रविष्टिः_दत्त);
#यदि_अन्तः
                यदि (U_FAILURE(*p_Error_संहिता)) {
                    प्रत्यागम शून्यसूचकः;
                }
                यदि (p_प्रविष्टिः_दत्त != शून्यसूचकः) {
                    p_प्रविष्टिः_दत्त->length = length;
                    प्रत्यागम p_प्रविष्टिः_दत्त;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        यदि (*sub_Error_संहिता == U_स्मृतिः_आवण्टनम्_ERROR) {
            *p_Error_संहिता = *sub_Error_संहिता;
            प्रत्यागम शून्यसूचकः;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        यदि (!isICUData) {
            प्रत्यागम शून्यसूचकः;
        } अन्यथा यदि (p_Common_दत्त != शून्यसूचकः) {
            ++common_दत्त_सूचकांक;  /* try the next data package */
        } अन्यथा यदि ((!checkedExtendedICUData) && extendICUData(sub_Error_संहिता)) {
            checkedExtendedICUData = सत्य;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } अन्यथा {
            प्रत्यागम शून्यसूचकः;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
स्थिर UBool isTimeZoneFile(ध्रुव वर्ण *name, ध्रुव वर्ण *type) {
    प्रत्यागम ((uprv_strcmp(type, "res") == 0) &&
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
स्थिर UData_स्मृतिः *
doOpenChoice(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *type, ध्रुव वर्ण *name,
             UData_स्मृतिः_Is_Acceptable *isAcceptable, शून्य *context,
             UError_संहिता *p_Error_संहिता)
{
    UData_स्मृतिः         *retVal = शून्यसूचकः;

    ध्रुव वर्ण         *दत्त_मार्गः;

    int32_t             toc_प्रविष्टिः_Suffix_सूचकांक;
    ध्रुव वर्ण         *toc_प्रविष्टिः_मार्गः_Suffix;
    UError_संहिता          sub_Error_संहिता=U_ZERO_ERROR;
    ध्रुव वर्ण         *वृक्षः_Char;

    UBool               isICUData = असत्य;


    FileTracer::traceOpen(मार्गः, type, name);


    /* Is this path ICU data? */
    यदि(मार्गः == शून्यसूचकः ||
       !वाक्यं_तुलय(मार्गः, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(मार्गः, U_ICUDATA_NAME U_वृक्षः_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_वृक्षः_SEPARATOR_STRING)) ||  
       !uprv_strncmp(मार्गः, U_ICUDATA_ALIAS U_वृक्षः_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_वृक्षः_SEPARATOR_STRING))) {
      isICUData = सत्य;
    }

#यदि (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_मार्गः;
    यदि(मार्गः) {
        यदि(uprv_strchr(मार्गः,U_FILE_ALT_SEP_CHAR) != शून्यसूचकः) {
            alt_Sep_मार्गः.append(मार्गः, *p_Error_संहिता);
            वर्ण *p;
            यावत् ((p = uprv_strchr(alt_Sep_मार्गः.दत्त(), U_FILE_ALT_SEP_CHAR)) != शून्यसूचकः) {
                *p = U_FILE_SEP_CHAR;
            }
#यदि defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", मार्गः, alt_Sep_मार्गः.दत्त());
#यदि_अन्तः
            मार्गः = alt_Sep_मार्गः.दत्त();
        }
    }
#यदि_अन्तः

    CharString toc_प्रविष्टिः_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_प्रविष्टिः_मार्गः; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString वृक्षः_Name;

    /* ======= Set up strings */
    यदि(मार्गः==शून्यसूचकः) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_संहिता);
    } अन्यथा {
        ध्रुव वर्ण *pkg;
        ध्रुव वर्ण *first;
        pkg = uprv_strrchr(मार्गः, U_FILE_SEP_CHAR);
        first = uprv_strchr(मार्गः, U_FILE_SEP_CHAR);
        यदि(uprv_मार्गः_Is_Absolute(मार्गः) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            यदि(pkg) {
                pkgName.append(pkg+1, *p_Error_संहिता);
            } अन्यथा {
                pkgName.append(मार्गः, *p_Error_संहिता);
            }
        } अन्यथा {
            वृक्षः_Char = uprv_strchr(मार्गः, U_वृक्षः_SEPARATOR);
            यदि(वृक्षः_Char) { 
                वृक्षः_Name.append(वृक्षः_Char+1, *p_Error_संहिता); /* following '-' */
                यदि(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_संहिता);
                } अन्यथा {
                    pkgName.append(मार्गः, static_cast<int32_t>(वृक्षः_Char - मार्गः), *p_Error_संहिता);
                    यदि (first == शून्यसूचकः) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        मार्गः = pkgName.दत्त();
                    }
                }
            } अन्यथा {
                यदि(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_संहिता);
                } अन्यथा {
                    pkgName.append(मार्गः, *p_Error_संहिता);
                }
            }
        }
    }

#यदि_परिभाषितम् UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.दत्त(), वृक्षः_Name.दत्त());
#यदि_अन्तः

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_प्रविष्टिः_Name.append(pkgName, *p_Error_संहिता);
    toc_प्रविष्टिः_मार्गः.append(pkgName, *p_Error_संहिता);
    toc_प्रविष्टिः_Suffix_सूचकांक = toc_प्रविष्टिः_Name.length();

    यदि(!वृक्षः_Name.is_रिक्तानि()) {
        toc_प्रविष्टिः_Name.append(U_वृक्षः_प्रविष्टिः_SEP_CHAR, *p_Error_संहिता).append(वृक्षः_Name, *p_Error_संहिता);
        toc_प्रविष्टिः_मार्गः.append(U_FILE_SEP_CHAR, *p_Error_संहिता).append(वृक्षः_Name, *p_Error_संहिता);
    }

    toc_प्रविष्टिः_Name.append(U_वृक्षः_प्रविष्टिः_SEP_CHAR, *p_Error_संहिता).append(name, *p_Error_संहिता);
    toc_प्रविष्टिः_मार्गः.append(U_FILE_SEP_CHAR, *p_Error_संहिता).append(name, *p_Error_संहिता);
    यदि(type!=शून्यसूचकः && *type!=0) {
        toc_प्रविष्टिः_Name.append(".", *p_Error_संहिता).append(type, *p_Error_संहिता);
        toc_प्रविष्टिः_मार्गः.append(".", *p_Error_संहिता).append(type, *p_Error_संहिता);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_प्रविष्टिः_मार्गः_Suffix = toc_प्रविष्टिः_मार्गः.दत्त() + toc_प्रविष्टिः_Suffix_सूचकांक + 1; /* suffix starts here */

#यदि_परिभाषितम् UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_प्रविष्टिः_Name.दत्त());
    fprintf(stderr, " tocEntryPath = %s\n", toc_प्रविष्टिः_मार्गः.दत्त());
#यदि_अन्तः

#यदि !defined(ICU_दत्त_DIR_WINDOWS)
    यदि(मार्गः == शून्यसूचकः) {
        मार्गः = COMMON_दत्त_NAME; /* "icudt26e" */
    }
#अन्यथा
    // When using the Windows system data, we expects only a single data file.
    मार्गः = COMMON_दत्त_NAME; /* "icudt26e" */
#यदि_अन्तः

    /************************ Begin loop looking for ind. files ***************/
#यदि_परिभाषितम् UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_मार्गः(मार्गः));
#यदि_अन्तः

    /* End of dealing with a null basename */
    दत्त_मार्गः = u_get_दत्त_Directory();

    /****    Time zone individual files override  */
    यदि (isICUData && type != शून्यसूचकः && isTimeZoneFile(name, type)) {
        ध्रुव वर्ण *tzFilesDir = u_getTimeZoneFilesDirectory(p_Error_संहिता);
        यदि (tzFilesDir[0] != 0) {
#यदि_परिभाषितम् UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#यदि_अन्तः
            retVal = do_आरोपय_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_प्रविष्टिः_मार्गः_Suffix,
                            /*मार्गः*/ "", type, name, isAcceptable, context, &sub_Error_संहिता, p_Error_संहिता);
            यदि((retVal != शून्यसूचकः) || U_FAILURE(*p_Error_संहिता)) {
                प्रत्यागम retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    यदि(g_दत्त_File_Access == UDATA_PACKAGES_FIRST) {
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#यदि_अन्तः
        /* #2 */
        retVal = do_आरोपय_From_Common_दत्त(isICUData, 
                            pkgName.दत्त(), दत्त_मार्गः, toc_प्रविष्टिः_मार्गः_Suffix, toc_प्रविष्टिः_Name.दत्त(),
                            मार्गः, type, name, isAcceptable, context, &sub_Error_संहिता, p_Error_संहिता);
        यदि((retVal != शून्यसूचकः) || U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    यदि((g_दत्त_File_Access==UDATA_PACKAGES_FIRST) ||
       (g_दत्त_File_Access==UDATA_FILES_FIRST)) {
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#यदि_अन्तः
        /* Check to make sure that there is a dataPath to iterate over */
        यदि ((दत्त_मार्गः && *दत्त_मार्गः) || !isICUData) {
            retVal = do_आरोपय_From_Individual_Files(pkgName.दत्त(), दत्त_मार्गः, toc_प्रविष्टिः_मार्गः_Suffix,
                            मार्गः, type, name, isAcceptable, context, &sub_Error_संहिता, p_Error_संहिता);
            यदि((retVal != शून्यसूचकः) || U_FAILURE(*p_Error_संहिता)) {
                प्रत्यागम retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    यदि((g_दत्त_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_दत्त_File_Access==UDATA_FILES_FIRST)) {
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#यदि_अन्तः
        retVal = do_आरोपय_From_Common_दत्त(isICUData,
                            pkgName.दत्त(), दत्त_मार्गः, toc_प्रविष्टिः_मार्गः_Suffix, toc_प्रविष्टिः_Name.दत्त(),
                            मार्गः, type, name, isAcceptable, context, &sub_Error_संहिता, p_Error_संहिता);
        यदि((retVal != शून्यसूचकः) || U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    यदि(g_दत्त_File_Access==UDATA_NO_FILES) {
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#यदि_अन्तः
        retVal = do_आरोपय_From_Common_दत्त(isICUData,
                            pkgName.दत्त(), "", toc_प्रविष्टिः_मार्गः_Suffix, toc_प्रविष्टिः_Name.दत्त(),
                            मार्गः, type, name, isAcceptable, context, &sub_Error_संहिता, p_Error_संहिता);
        यदि((retVal != शून्यसूचकः) || U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम retVal;
        }
    }

    /* data not found */
    यदि(U_SUCCESS(*p_Error_संहिता)) {
        यदि(U_SUCCESS(sub_Error_संहिता)) {
            /* file not found */
            *p_Error_संहिता=U_FILE_ACCESS_ERROR;
        } अन्यथा {
            /* entry point not found or rejected */
            *p_Error_संहिता=sub_Error_संहिता;
        }
    }
    प्रत्यागम retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_स्मृतिः * U_EXPORT2
udata_open(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *type, ध्रुव वर्ण *name,
           UError_संहिता *p_Error_संहिता) {
#यदि_परिभाषितम् UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (मार्गः?मार्गः:"nullptr"), name, type);
    fflush(stderr);
#यदि_अन्तः

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    } अन्यथा यदि(name==शून्यसूचकः || *name==0) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा {
        प्रत्यागम doOpenChoice(मार्गः, type, name, शून्यसूचकः, शून्यसूचकः, p_Error_संहिता);
    }
}



U_CAPI UData_स्मृतिः * U_EXPORT2
udata_openChoice(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *type, ध्रुव वर्ण *name,
                 UData_स्मृतिः_Is_Acceptable *isAcceptable, शून्य *context,
                 UError_संहिता *p_Error_संहिता) {
#यदि_परिभाषितम् UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (मार्गः?मार्गः:"nullptr"), name, type);
#यदि_अन्तः

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    } अन्यथा यदि(name==शून्यसूचकः || *name==0 || isAcceptable==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा {
        प्रत्यागम doOpenChoice(मार्गः, type, name, isAcceptable, context, p_Error_संहिता);
    }
}



U_CAPI शून्य U_EXPORT2
udata_getInfo(UData_स्मृतिः *p_दत्त, UDataInfo *pInfo) {
    यदि(pInfo!=शून्यसूचकः) {
        यदि(p_दत्त!=शून्यसूचकः && p_दत्त->p_शीर्षकम्!=शून्यसूचकः) {
            ध्रुव UDataInfo *info=&p_दत्त->p_शीर्षकम्->info;
            uint16_t दत्त_Info_आकार=udata_get_Info_आकार(info);
            यदि(pInfo->size>दत्त_Info_आकार) {
                pInfo->size=दत्त_Info_आकार;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (ध्रुव uint16_t *)info+1, pInfo->size-2);
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


U_CAPI शून्य U_EXPORT2 udata_समुच्चयः_File_Access(UDataFileAccess access, UError_संहिता * /*स्थितिः*/)
{
    // Note: this function is documented as not thread safe.
    g_दत्त_File_Access = access;
}
