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

#అన్తర్భావ "unicode/utypes.h"  /* U_PLATFORM etc. */

#నిర్వచితమైతే __GNUC__
/* if gcc
#define ATTRIBUTE_WEAK __attribute__ ((weak))
might have to #include some other header
*/
#అయితే_ముగింపు

#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/uversion.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "restrace.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "ucmndata.h"
#అన్తర్భావ "udatamem.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "umapfile.h"
#అన్తర్భావ "umutex.h"

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

#ఒకవేళ defined(UDATA_DEBUG)
#   include <stdio.h>
#అయితే_ముగింపు

U_NAMESPACE_USE

/*
 *  Forward declarations
 */
స్థిర UData_జ్ఞాపకం *udata_కనుగొను_Cached_దత్తాంశం(స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం &err);

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
స్థిర UData_జ్ఞాపకం *gCommonICUDataArray[10] = { శూన్యసూచిక };   // Access protected by icu global mutex.

స్థిర u_atomic_int32_t g_Have_Tried_గమ్యం_ఎక్కించు_Common_దత్తాంశం {0};  //  See extendICUData().

స్థిర UHashtable  *g_Common_దత్తాంశం_Cache = శూన్యసూచిక;  /* Global hash table of opened ICU data files.  */
స్థిర icu::UInitOnce g_Common_దత్తాంశం_Cache_Init_Once {};

#ఒకవేళ !defined(ICU_దత్తాంశం_DIR_WINDOWS)
స్థిర UDataFileAccess  g_దత్తాంశం_File_Access = UDATA_DEFAULT_ACCESS;  // Access not synchronized.
                                                                 // Modifying is documented as thread-unsafe.
#లేకపోతే
// If we are using the Windows data directory, then look in one spot only.
స్థిర UDataFileAccess  g_దత్తాంశం_File_Access = UDATA_NO_FILES;
#అయితే_ముగింపు

స్థిర UBool U_CALLCONV
udata_cleanup()
{
    int32_t i;

    ఒకవేళ (g_Common_దత్తాంశం_Cache) {             /* Delete the cache of user data mappings.  */
        uhash_close(g_Common_దత్తాంశం_Cache);  /*   Table owns the contents, and will delete them. */
        g_Common_దత్తాంశం_Cache = శూన్యసూచిక;        /*   Cleanup is not thread safe.                */
    }
    g_Common_దత్తాంశం_Cache_Init_Once.reset();

    కొరకు (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray) && gCommonICUDataArray[i] != శూన్యసూచిక; ++i) {
        udata_close(gCommonICUDataArray[i]);
        gCommonICUDataArray[i] = శూన్యసూచిక;
    }
    g_Have_Tried_గమ్యం_ఎక్కించు_Common_దత్తాంశం = 0;

    తిరిగి నిజం;                   /* Everything was cleaned up */
}

స్థిర UBool U_CALLCONV
కనుగొను_Common_ICUData_By_Name(స్థిరరాశి అక్షరం *లోపల_Basename, UError_సంకేతం &err)
{
    UBool దొరికింది = అబద్ధం;
    int32_t i;

    UData_జ్ఞాపకం  *p_దత్తాంశం = udata_కనుగొను_Cached_దత్తాంశం(లోపల_Basename, err);
    ఒకవేళ (U_FAILURE(err) || p_దత్తాంశం == శూన్యసూచిక)
        తిరిగి అబద్ధం;

    {
        Mutex తాళం;
        కొరకు (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
            ఒకవేళ ((gCommonICUDataArray[i] != శూన్యసూచిక) && (gCommonICUDataArray[i]->p_శీర్షిక == p_దత్తాంశం->p_శీర్షిక)) {
                /* The data pointer is already in the array. */
                దొరికింది = నిజం;
                ఆపు;
            }
        }
    }
    తిరిగి దొరికింది;
}


/*
 * setCommonICUData.   Set a UDataMemory to be the global ICU Data
 */
స్థిర UBool
సమితి_Common_ICUData(UData_జ్ఞాపకం *p_దత్తాంశం,     /*  The new common data.  Belongs to caller, we copy it. */
                 UBool       warn,       /*  If true, set USING_DEFAULT warning if ICUData was    */
                                         /*    changed by another thread before we got to it.     */
                 UError_సంకేతం *pErr)
{
    UData_జ్ఞాపకం  *new_Common_దత్తాంశం = UData_జ్ఞాపకం_create_New_Instance(pErr);
    int32_t i;
    UBool didUpdate = అబద్ధం;
    ఒకవేళ (U_FAILURE(*pErr)) {
        తిరిగి అబద్ధం;
    }

    /*  For the assignment, other threads must cleanly see either the old            */
    /*    or the new, not some partially initialized new.  The old can not be        */
    /*    deleted - someone may still have a pointer to it lying around in           */
    /*    their locals.                                                              */
    UDatamemory_కేటాయించు(new_Common_దత్తాంశం, p_దత్తాంశం);
    umtx_తాళం(శూన్యసూచిక);
    కొరకు (i = 0; i < UPRV_LENGTHOF(gCommonICUDataArray); ++i) {
        ఒకవేళ (gCommonICUDataArray[i] == శూన్యసూచిక) {
            gCommonICUDataArray[i] = new_Common_దత్తాంశం;
            didUpdate = నిజం;
            ఆపు;
        } లేకపోతే ఒకవేళ (gCommonICUDataArray[i]->p_శీర్షిక == p_దత్తాంశం->p_శీర్షిక) {
            /* The same data pointer is already in the array. */
            ఆపు;
        }
    }
    umtx_unlock(శూన్యసూచిక);

    ఒకవేళ (i == UPRV_LENGTHOF(gCommonICUDataArray) && warn) {
        *pErr = U_USING_DEFAULT_WARNING;
    }
    ఒకవేళ (didUpdate) {
        ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
    } లేకపోతే {
        uprv_free(new_Common_దత్తాంశం);
    }
    తిరిగి didUpdate;
}

#ఒకవేళ !defined(ICU_దత్తాంశం_DIR_WINDOWS)

స్థిర UBool
సమితి_Common_ICUData_సూచిక(స్థిరరాశి శూన్య *p_దత్తాంశం, UBool /*warn*/, UError_సంకేతం *p_Error_సంకేతం) {
    UData_జ్ఞాపకం t_దత్తాంశం;
    UData_జ్ఞాపకం_init(&t_దత్తాంశం);
    UData_జ్ఞాపకం_సమితి_దత్తాంశం(&t_దత్తాంశం, p_దత్తాంశం);
    udata_check_Common_దత్తాంశం(&t_దత్తాంశం, p_Error_సంకేతం);
    తిరిగి సమితి_Common_ICUData(&t_దత్తాంశం, అబద్ధం, p_Error_సంకేతం);
}

#అయితే_ముగింపు

స్థిర స్థిరరాశి అక్షరం *
కనుగొను_Basename(స్థిరరాశి అక్షరం *మార్గం) {
    స్థిరరాశి అక్షరం *basename=uprv_strrchr(మార్గం, U_FILE_SEP_CHAR);
    ఒకవేళ(basename==శూన్యసూచిక) {
        తిరిగి మార్గం;
    } లేకపోతే {
        తిరిగి basename+1;
    }
}

#నిర్వచితమైతే UDATA_DEBUG
స్థిర స్థిరరాశి అక్షరం *
package_Name_From_మార్గం(స్థిరరాశి అక్షరం *మార్గం)
{
    ఒకవేళ((మార్గం == శూన్యసూచిక) || (*మార్గం == 0)) {
        తిరిగి U_ICUDATA_NAME;
    }

    మార్గం = కనుగొను_Basename(మార్గం);

    ఒకవేళ((మార్గం == శూన్యసూచిక) || (*మార్గం == 0)) {
        తిరిగి U_ICUDATA_NAME;
    }

    తిరిగి మార్గం;
}
#అయితే_ముగింపు

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

రకనిర్వచనం నిర్మాణం దత్తాంశం_Cache_Element {
    అక్షరం          *name;
    UData_జ్ఞాపకం   *అంశం;
} దత్తాంశం_Cache_Element;



/*
 * Deleter function for DataCacheElements.
 *         udata cleanup function closes the hash table; hash table in turn calls back to
 *         here for each entry.
 */
స్థిర శూన్య U_CALLCONV దత్తాంశం_Cache_Element_deleter(శూన్య *pDCEl) {
    దత్తాంశం_Cache_Element* p = static_cast<దత్తాంశం_Cache_Element*>(pDCEl);
    udata_close(p->అంశం);              /* unmaps storage */
    uprv_free(p->name);                /* delete the hash key string. */
    uprv_free(pDCEl);                  /* delete 'this'          */
}

స్థిర శూన్య U_CALLCONV udata_init_Hash_పట్టిక(UError_సంకేతం &err) {
    U_ASSERT(g_Common_దత్తాంశం_Cache == శూన్యసూచిక);
    g_Common_దత్తాంశం_Cache = uhash_open(uhash_hashChars, uhash_compareChars, శూన్యసూచిక, &err);
    ఒకవేళ (U_FAILURE(err)) {
       తిరిగి;
    }
    U_ASSERT(g_Common_దత్తాంశం_Cache != శూన్యసూచిక);
    uhash_సమితి_విలువ_Deleter(g_Common_దత్తాంశం_Cache, దత్తాంశం_Cache_Element_deleter);
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_UDATA, udata_cleanup);
}

 /*   udata_getCacheHashTable()
  *     Get the hash table used to store the data cache entries.
  *     Lazy create it if it doesn't yet exist.
  */
స్థిర UHashtable *udata_get_Hash_పట్టిక(UError_సంకేతం &err) {
    umtx_initOnce(g_Common_దత్తాంశం_Cache_Init_Once, &udata_init_Hash_పట్టిక, err);
    తిరిగి g_Common_దత్తాంశం_Cache;
}



స్థిర UData_జ్ఞాపకం *udata_కనుగొను_Cached_దత్తాంశం(స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం &err)
{
    UHashtable        *htable;
    UData_జ్ఞాపకం       *retVal = శూన్యసూచిక;
    దత్తాంశం_Cache_Element  *el;
    స్థిరరాశి అక్షరం        *పునాది_Name;

    htable = udata_get_Hash_పట్టిక(err);
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి శూన్యసూచిక;
    }

    పునాది_Name = కనుగొను_Basename(మార్గం);   /* Cache remembers only the base name, not the full path. */
    umtx_తాళం(శూన్యసూచిక);
    el = static_cast<దత్తాంశం_Cache_Element*>(uhash_get(htable, పునాది_Name));
    umtx_unlock(శూన్యసూచిక);
    ఒకవేళ (el != శూన్యసూచిక) {
        retVal = el->అంశం;
    }
#నిర్వచితమైతే UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] -> %p\n", పునాది_Name, (శూన్య*) retVal);
#అయితే_ముగింపు
    తిరిగి retVal;
}


స్థిర UData_జ్ఞాపకం *udata_cache_దత్తాంశం_అంశం(స్థిరరాశి అక్షరం *మార్గం, UData_జ్ఞాపకం *అంశం, UError_సంకేతం *pErr) {
    దత్తాంశం_Cache_Element *newElement;
    స్థిరరాశి అక్షరం       *పునాది_Name;
    int32_t           nameLen;
    UHashtable       *htable;
    దత్తాంశం_Cache_Element *old_విలువ = శూన్యసూచిక;
    UError_సంకేతం        subErr = U_ZERO_ERROR;

    htable = udata_get_Hash_పట్టిక(*pErr);
    ఒకవేళ (U_FAILURE(*pErr)) {
        తిరిగి శూన్యసూచిక;
    }

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = static_cast<దత్తాంశం_Cache_Element*>(uprv_malloc(పరిమాణం(దత్తాంశం_Cache_Element)));
    ఒకవేళ (newElement == శూన్యసూచిక) {
        *pErr = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    newElement->అంశం = UData_జ్ఞాపకం_create_New_Instance(pErr);
    ఒకవేళ (U_FAILURE(*pErr)) {
        uprv_free(newElement);
        తిరిగి శూన్యసూచిక;
    }
    UDatamemory_కేటాయించు(newElement->అంశం, అంశం);

    పునాది_Name = కనుగొను_Basename(మార్గం);
    nameLen = static_cast<int32_t>(uprv_strlen(పునాది_Name));
    newElement->name = static_cast<అక్షరం*>(uprv_malloc(nameLen + 1));
    ఒకవేళ (newElement->name == శూన్యసూచిక) {
        *pErr = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        uprv_free(newElement->అంశం);
        uprv_free(newElement);
        తిరిగి శూన్యసూచిక;
    }
    uprv_strcpy(newElement->name, పునాది_Name);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_తాళం(శూన్యసూచిక);
    old_విలువ = static_cast<దత్తాంశం_Cache_Element*>(uhash_get(htable, మార్గం));
    ఒకవేళ (old_విలువ != శూన్యసూచిక) {
        subErr = U_USING_DEFAULT_WARNING;
    }
    లేకపోతే {
        uhash_put(
            htable,
            newElement->name,               /* Key   */
            newElement,                     /* Value */
            &subErr);
    }
    umtx_unlock(శూన్యసూచిక);

#నిర్వచితమైతే UDATA_DEBUG
    fprintf(stderr, "Cache: [%s] <<< %p : %s. vFunc=%p\n", newElement->name, 
    (శూన్య*) newElement->అంశం, u_errorName(subErr), (శూన్య*) newElement->అంశం->vFuncs);
#అయితే_ముగింపు

    ఒకవేళ (subErr == U_USING_DEFAULT_WARNING || U_FAILURE(subErr)) {
        *pErr = subErr; /* copy sub err unto fillin ONLY if something happens. */
        uprv_free(newElement->name);
        uprv_free(newElement->అంశం);
        uprv_free(newElement);
        తిరిగి old_విలువ ? old_విలువ->అంశం : శూన్యసూచిక;
    }

    తిరిగి newElement->అంశం;
}

/*----------------------------------------------------------------------*==============
 *                                                                      *
 *  Path management.  Could be shared with other tools/etc if need be   *
 * later on.                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*/

U_NAMESPACE_BEGIN

తరగతి UData_మార్గం_Iterator
{
బహిరంగ:
    UData_మార్గం_Iterator(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *pkg,
                      స్థిరరాశి అక్షరం *అంశం, స్థిరరాశి అక్షరం *suffix, UBool do_Check_చివరి_Four,
                      UError_సంకేతం *p_Error_సంకేతం);
    స్థిరరాశి అక్షరం *next(UError_సంకేతం *p_Error_సంకేతం);

అంతరంగ:
    స్థిరరాశి అక్షరం *మార్గం;                              /* working path (u_icudata_Dir) */
    స్థిరరాశి అక్షరం *తదుపరి_మార్గం;                          /* path following this one */
    స్థిరరాశి అక్షరం *basename;                          /* item's basename (icudt22e_mt.res)*/

    StringPiece suffix;                            /* item suffix (can be null) */

    uint32_t    basenameLen;                       /* length of basename */

    CharString  అంశం_మార్గం;                          /* path passed in with item name */
    CharString  మార్గం_బఫరు;                        /* output path for this it'ion */
    CharString  packageStub;                       /* example:  "/icudt28b". Will ignore that leaf in set paths. */

    UBool       check_చివరి_Four;                     /* if true then allow paths such as '/foo/myapp.dat'
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
UData_మార్గం_Iterator::UData_మార్గం_Iterator(స్థిరరాశి అక్షరం *లోపల_మార్గం, స్థిరరాశి అక్షరం *pkg,
                                     స్థిరరాశి అక్షరం *అంశం, స్థిరరాశి అక్షరం *లోపల_Suffix, UBool do_Check_చివరి_Four,
                                     UError_సంకేతం *p_Error_సంకేతం)
{
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "SUFFIX1=%s PATH=%s\n", లోపల_Suffix, లోపల_మార్గం);
#అయితే_ముగింపు
    /** Path **/
    ఒకవేళ(లోపల_మార్గం == శూన్యసూచిక) {
        మార్గం = u_get_దత్తాంశం_Directory();
    } లేకపోతే {
        మార్గం = లోపల_మార్గం;
    }

    /** Package **/
    ఒకవేళ(pkg != శూన్యసూచిక) {
      packageStub.append(U_FILE_SEP_CHAR, *p_Error_సంకేతం).append(pkg, *p_Error_సంకేతం);
#నిర్వచితమైతే UDATA_DEBUG
      fprintf(stderr, "STUB=%s [%d]\n", packageStub.దత్తాంశం(), packageStub.length());
#అయితే_ముగింపు
    }

    /** Item **/
    basename = కనుగొను_Basename(అంశం);
    basenameLen = static_cast<int32_t>(uprv_strlen(basename));

    /** Item path **/
    ఒకవేళ(basename == అంశం) {
        తదుపరి_మార్గం = మార్గం;
    } లేకపోతే {
        అంశం_మార్గం.append(అంశం, static_cast<int32_t>(basename - అంశం), *p_Error_సంకేతం);
        తదుపరి_మార్గం = అంశం_మార్గం.దత్తాంశం();
    }
#నిర్వచితమైతే UDATA_DEBUG
    fprintf(stderr, "SUFFIX=%s [%p]\n", లోపల_Suffix, (శూన్య*) లోపల_Suffix);
#అయితే_ముగింపు

    /** Suffix  **/
    ఒకవేళ(లోపల_Suffix != శూన్యసూచిక) {
        suffix = లోపల_Suffix;
    } లేకపోతే {
        suffix = "";
    }

    check_చివరి_Four = do_Check_చివరి_Four;

    /* pathBuffer will hold the output path strings returned by this iterator */

#నిర్వచితమైతే UDATA_DEBUG
    fprintf(stderr, "0: init %s -> [path=%s], [base=%s], [suff=%s], [itempath=%s], [nextpath=%s], [checklast4=%s]\n",
            అంశం,
            మార్గం,
            basename,
            suffix.దత్తాంశం(),
            అంశం_మార్గం.దత్తాంశం(),
            తదుపరి_మార్గం,
            check_చివరి_Four?"true":"false");
#అయితే_ముగింపు
}

/**
 * Get the next path on the list.
 *
 * @param iter The Iter to be used 
 * @param len  If set, pointer to the length of the returned path, for convenience. 
 * @return Pointer to the next path segment, or nullptr if there are no more.
 */
స్థిరరాశి అక్షరం *UData_మార్గం_Iterator::next(UError_సంకేతం *p_Error_సంకేతం)
{
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    స్థిరరాశి అక్షరం *current_మార్గం = శూన్యసూచిక;
    int32_t     మార్గం_Len = 0;
    స్థిరరాశి అక్షరం *మార్గం_Basename;

    చేయి
    {
        ఒకవేళ( తదుపరి_మార్గం == శూన్యసూచిక ) {
            ఆపు;
        }
        current_మార్గం = తదుపరి_మార్గం;

        ఒకవేళ(తదుపరి_మార్గం == అంశం_మార్గం.దత్తాంశం()) { /* we were processing item's path. */
            తదుపరి_మార్గం = మార్గం; /* start with regular path next tm. */
            మార్గం_Len = static_cast<int32_t>(uprv_strlen(current_మార్గం));
        } లేకపోతే {
            /* fix up next for next time */
            తదుపరి_మార్గం = uprv_strchr(current_మార్గం, U_మార్గం_SEP_CHAR);
            ఒకవేళ(తదుపరి_మార్గం == శూన్యసూచిక) {
                /* segment: entire path */
                మార్గం_Len = static_cast<int32_t>(uprv_strlen(current_మార్గం));
            } లేకపోతే {
                /* segment: until next segment */
                మార్గం_Len = static_cast<int32_t>(తదుపరి_మార్గం - current_మార్గం);
                /* skip divider */
                తదుపరి_మార్గం ++;
            }
        }

        ఒకవేళ(మార్గం_Len == 0) {
            కొనసాగు;
        }

#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "rest of path (IDD) = %s\n", current_మార్గం);
        fprintf(stderr, "                     ");
        { 
            int32_t qqq;
            కొరకు(qqq=0;qqq<మార్గం_Len;qqq++)
            {
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
        }
#అయితే_ముగింపు
        మార్గం_బఫరు.clear().append(current_మార్గం, మార్గం_Len, *p_Error_సంకేతం);

        /* check for .dat files */
        మార్గం_Basename = కనుగొను_Basename(మార్గం_బఫరు.దత్తాంశం());

        ఒకవేళ(check_చివరి_Four && 
           (మార్గం_Len>=4) &&
           uprv_strncmp(మార్గం_బఫరు.దత్తాంశం() +(మార్గం_Len-4), suffix.దత్తాంశం(), 4)==0 && /* suffix matches */
           uprv_strncmp(కనుగొను_Basename(మార్గం_బఫరు.దత్తాంశం()), basename, basenameLen)==0  && /* base matches */
           uprv_strlen(మార్గం_Basename)==(basenameLen+4)) { /* base+suffix = full len */

#నిర్వచితమైతే UDATA_DEBUG
            fprintf(stderr, "Have %s file on the path: %s\n", suffix.దత్తాంశం(), మార్గం_బఫరు.దత్తాంశం());
#అయితే_ముగింపు
            /* do nothing */
        }
        లేకపోతే 
        {       /* regular dir path */
            ఒకవేళ(మార్గం_బఫరు[మార్గం_Len-1] != U_FILE_SEP_CHAR) {
                ఒకవేళ((మార్గం_Len>=4) &&
                   uprv_strncmp(మార్గం_బఫరు.దత్తాంశం()+(మార్గం_Len-4), ".dat", 4) == 0)
                {
#నిర్వచితమైతే UDATA_DEBUG
                    fprintf(stderr, "skipping non-directory .dat file %s\n", మార్గం_బఫరు.దత్తాంశం());
#అయితే_ముగింపు
                    కొనసాగు;
                }

                /* Check if it is a directory with the same name as our package */
                ఒకవేళ(!packageStub.is_ఖాళీలు() &&
                   (మార్గం_Len > packageStub.length()) &&
                   !uprv_strcmp(మార్గం_బఫరు.దత్తాంశం() + మార్గం_Len - packageStub.length(), packageStub.దత్తాంశం())) {
#నిర్వచితమైతే UDATA_DEBUG
                  fprintf(stderr, "Found stub %s (will add package %s of len %d)\n", packageStub.దత్తాంశం(), basename, basenameLen);
#అయితే_ముగింపు
                  మార్గం_బఫరు.truncate(మార్గం_Len - packageStub.length());
                }
                మార్గం_బఫరు.append(U_FILE_SEP_CHAR, *p_Error_సంకేతం);
            }

            /* + basename */
            మార్గం_బఫరు.append(packageStub.దత్తాంశం()+1, packageStub.length()-1, *p_Error_సంకేతం);

            ఒకవేళ (!suffix.empty())  /* tack on suffix */
            {
                ఒకవేళ (suffix.length() > 4) {
                    // If the suffix is actually an item ("ibm-5348_P100-1997.cnv") and not an extension (".res")
                    // then we need to ensure that the path ends with a separator.
                    మార్గం_బఫరు.ensure_చివర్లు_With_File_Separator(*p_Error_సంకేతం);
                }
                మార్గం_బఫరు.append(suffix, *p_Error_సంకేతం);
            }
        }

#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, " -->  %s\n", మార్గం_బఫరు.దత్తాంశం());
#అయితే_ముగింపు

        తిరిగి మార్గం_బఫరు.దత్తాంశం();

    } అయితే(మార్గం);

    /* fell way off the end */
    తిరిగి శూన్యసూచిక;
}

U_NAMESPACE_చివర

/* ==================================================================================*/


/*----------------------------------------------------------------------*
 *                                                                      *
 *  Add a static reference to the common data library                   *
 *   Unless overridden by an explicit udata_setCommonData, this will be *
 *      our common data.                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
#ఒకవేళ !defined(ICU_దత్తాంశం_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
బాహ్య "C" స్థిరరాశి దత్తాంశం_శీర్షిక U_దత్తాంశం_API U_ICUDATA_నమోదు_బిందువు;
#అయితే_ముగింపు

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
స్థిర UData_జ్ఞాపకం *
open_Common_దత్తాంశం(స్థిరరాశి అక్షరం *మార్గం,          /*  Path from OpenChoice?          */
               int32_t common_దత్తాంశం_సూచీ,   /*  ICU Data (index >= 0) if path == nullptr */
               UError_సంకేతం *p_Error_సంకేతం)
{
    UData_జ్ఞాపకం t_దత్తాంశం;
    స్థిరరాశి అక్షరం *మార్గం_బఫరు;
    స్థిరరాశి అక్షరం *లోపల_Basename;

    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    UData_జ్ఞాపకం_init(&t_దత్తాంశం);

    /* ??????? TODO revisit this */ 
    ఒకవేళ (common_దత్తాంశం_సూచీ >= 0) {
        /* "mini-cache" for common ICU data */
        ఒకవేళ(common_దత్తాంశం_సూచీ >= UPRV_LENGTHOF(gCommonICUDataArray)) {
            తిరిగి శూన్యసూచిక;
        }
        {
            Mutex తాళం;
            ఒకవేళ(gCommonICUDataArray[common_దత్తాంశం_సూచీ] != శూన్యసూచిక) {
                తిరిగి gCommonICUDataArray[common_దత్తాంశం_సూచీ];
            }
#ఒకవేళ !defined(ICU_దత్తాంశం_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
            int32_t i;
            కొరకు(i = 0; i < common_దత్తాంశం_సూచీ; ++i) {
                ఒకవేళ(gCommonICUDataArray[i]->p_శీర్షిక == &U_ICUDATA_నమోదు_బిందువు) {
                    /* The linked-in data is already in the list. */
                    తిరిగి శూన్యసూచిక;
                }
            }
#అయితే_ముగింపు
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
#ఒకవేళ !defined(ICU_దత్తాంశం_DIR_WINDOWS)
// When using the Windows system data, we expect only a single data file.
        సమితి_Common_ICUData_సూచిక(&U_ICUDATA_నమోదు_బిందువు, అబద్ధం, p_Error_సంకేతం);
        {
            Mutex తాళం;
            తిరిగి gCommonICUDataArray[common_దత్తాంశం_సూచీ];
        }
#అయితే_ముగింపు
    }


    /* request is NOT for ICU Data.  */

    /* Find the base name portion of the supplied path.   */
    /*   inBasename will be left pointing somewhere within the original path string.      */
    లోపల_Basename = కనుగొను_Basename(మార్గం);
#నిర్వచితమైతే UDATA_DEBUG
    fprintf(stderr, "inBasename = %s\n", లోపల_Basename);
#అయితే_ముగింపు

    ఒకవేళ(*లోపల_Basename==0) {
        /* no basename.     This will happen if the original path was a directory name,   */
        /*    like  "a/b/c/".   (Fallback to separate files will still work.)             */
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "ocd: no basename in %s, bailing.\n", మార్గం);
#అయితే_ముగింపు
        ఒకవేళ (U_SUCCESS(*p_Error_సంకేతం)) {
            *p_Error_సంకేతం=U_FILE_ACCESS_ERROR;
        }
        తిరిగి శూన్యసూచిక;
    }

   /* Is the requested common data file already open and cached?                     */
   /*   Note that the cache is keyed by the base name only.  The rest of the path,   */
   /*     if any, is not considered.                                                 */
    UData_జ్ఞాపకం  *దత్తాంశం_గమ్యం_Return = udata_కనుగొను_Cached_దత్తాంశం(లోపల_Basename, *p_Error_సంకేతం);
    ఒకవేళ (దత్తాంశం_గమ్యం_Return != శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి దత్తాంశం_గమ్యం_Return;
    }

    /* Requested item is not in the cache.
     * Hunt it down, trying all the path locations
     */

    UData_మార్గం_Iterator iter(u_get_దత్తాంశం_Directory(), లోపల_Basename, మార్గం, ".dat", నిజం, p_Error_సంకేతం);

    అయితే ((UData_జ్ఞాపకం_is_Loaded(&t_దత్తాంశం)==అబద్ధం) && (మార్గం_బఫరు = iter.next(p_Error_సంకేతం)) != శూన్యసూచిక)
    {
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "ocd: trying path %s - ", మార్గం_బఫరు);
#అయితే_ముగింపు
        uprv_mapFile(&t_దత్తాంశం, మార్గం_బఫరు, p_Error_సంకేతం);
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_జ్ఞాపకం_is_Loaded(&t_దత్తాంశం)?"LOADED":"not loaded");
#అయితే_ముగింపు
    }
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (!UData_జ్ఞాపకం_is_Loaded(&t_దత్తాంశం)) {
        /* no common data */
        *p_Error_సంకేతం=U_FILE_ACCESS_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* we have mapped a file, check its header */
    udata_check_Common_దత్తాంశం(&t_దత్తాంశం, p_Error_సంకేతం);


    /* Cache the UDataMemory struct for this .dat file,
     *   so we won't need to hunt it down and map it again next time
     *   something is needed from it.                */
    తిరిగి udata_cache_దత్తాంశం_అంశం(లోపల_Basename, &t_దత్తాంశం, p_Error_సంకేతం);
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
స్థిర UBool extendICUData(UError_సంకేతం *pErr)
{
    UData_జ్ఞాపకం   *p_దత్తాంశం;
    UData_జ్ఞాపకం   copyPData;
    UBool         didUpdate = అబద్ధం;

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
#ఒకవేళ MAP_IMPLEMENTATION==MAP_STDIO
    స్థిర UMutex extendICUDataMutex;
    umtx_తాళం(&extendICUDataMutex);
#అయితే_ముగింపు
    ఒకవేళ(!umtx_ఎక్కించు_Acquire(g_Have_Tried_గమ్యం_ఎక్కించు_Common_దత్తాంశం)) {
        /* See if we can explicitly open a .dat file for the ICUData. */
        p_దత్తాంశం = open_Common_దత్తాంశం(
                   U_ICUDATA_NAME,            /*  "icudt20l" , for example.          */
                   -1,                        /*  Pretend we're not opening ICUData  */
                   pErr);

        /* How about if there is no pData, eh... */

       UData_జ్ఞాపకం_init(&copyPData);
       ఒకవేళ(p_దత్తాంశం != శూన్యసూచిక) {
          UDatamemory_కేటాయించు(&copyPData, p_దత్తాంశం);
          copyPData.map = శూన్యసూచిక;     /* The mapping for this data is owned by the hash table */
          copyPData.mapAddr = శూన్యసూచిక; /*   which will unmap it when ICU is shut down.         */
                                       /* CommonICUData is also unmapped when ICU is shut down.*/
                                       /* To avoid unmapping the data twice, zero out the map  */
                                       /*   fields in the UDataMemory that we're assigning     */
                                       /*   to CommonICUData.                                  */

          didUpdate = /* no longer using this result */
              సమితి_Common_ICUData(&copyPData,/*  The new common data.                                */
                       అబద్ధం,             /*  No warnings if write didn't happen                  */
                       pErr);             /*  setCommonICUData honors errors; NOP if error set    */
        }

        umtx_storeRelease(g_Have_Tried_గమ్యం_ఎక్కించు_Common_దత్తాంశం, 1);
    }

    didUpdate = కనుగొను_Common_ICUData_By_Name(U_ICUDATA_NAME, *pErr);  /* Return 'true' when a racing writes out the extended                 */
                                                          /* data after another thread has failed to see it (in openCommonData), so     */
                                                          /* extended data can be examined.                                             */
                                                          /* Also handles a race through here before gHaveTriedToLoadCommonData is set. */

#ఒకవేళ MAP_IMPLEMENTATION==MAP_STDIO
    umtx_unlock(&extendICUDataMutex);
#అయితే_ముగింపు
    తిరిగి didUpdate;               /* Return true if ICUData pointer was updated.   */
                                    /*   (Could potentially have been done by another thread racing */
                                    /*   us through here, but that's fine, we still return true    */
                                    /*   so that current thread will also examine extended data.   */
}

/*----------------------------------------------------------------------*
 *                                                                      *
 *   udata_setCommonData                                                *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CAPI శూన్య U_EXPORT2
udata_సమితి_Common_దత్తాంశం(స్థిరరాశి శూన్య *దత్తాంశం, UError_సంకేతం *p_Error_సంకేతం) {
    UData_జ్ఞాపకం దత్తాంశం_జ్ఞాపకం;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }

    ఒకవేళ(దత్తాంశం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    /* set the data pointer and test for validity */
    UData_జ్ఞాపకం_init(&దత్తాంశం_జ్ఞాపకం);
    UData_జ్ఞాపకం_సమితి_దత్తాంశం(&దత్తాంశం_జ్ఞాపకం, దత్తాంశం);
    udata_check_Common_దత్తాంశం(&దత్తాంశం_జ్ఞాపకం, p_Error_సంకేతం);
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {తిరిగి;}

    /* we have good data */
    /* Set it up as the ICU Common Data.  */
    సమితి_Common_ICUData(&దత్తాంశం_జ్ఞాపకం, నిజం, p_Error_సంకేతం);
}

/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */
U_CAPI శూన్య U_EXPORT2
udata_సమితి_App_దత్తాంశం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి శూన్య *దత్తాంశం, UError_సంకేతం *err)
{
    UData_జ్ఞాపకం     udm;

    ఒకవేళ(err==శూన్యసూచిక || U_FAILURE(*err)) {
        తిరిగి;
    }
    ఒకవేళ(దత్తాంశం==శూన్యసూచిక) {
        *err=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    UData_జ్ఞాపకం_init(&udm);
    UData_జ్ఞాపకం_సమితి_దత్తాంశం(&udm, దత్తాంశం);
    udata_check_Common_దత్తాంశం(&udm, err);
    udata_cache_దత్తాంశం_అంశం(మార్గం, &udm, err);
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
స్థిర UData_జ్ఞాపకం *
check_దత్తాంశం_అంశం
(
 స్థిరరాశి దత్తాంశం_శీర్షిక         *p_శీర్షిక,         /* The data item to be checked.                */
 UData_జ్ఞాపకం_Is_Acceptable  *isAcceptable,    /* App's call-back function                    */
 శూన్య                     *context,         /*   pass-thru param for above.                */
 స్థిరరాశి అక్షరం               *type,            /*   pass-thru param for above.                */
 స్థిరరాశి అక్షరం               *name,            /*   pass-thru param for above.                */
 UError_సంకేతం               *nonFatalErr,     /* Error code if this data was not acceptable  */
                                            /*   but openChoice should continue with       */
                                            /*   trying to get data from fallback path.    */
 UError_సంకేతం               *fatalErr         /* Bad error, caller should return immediately */
 )
{
    UData_జ్ఞాపకం  *r_దత్తాంశం_Mem = శూన్యసూచిక;          /* the new UDataMemory, to be returned.        */

    ఒకవేళ (U_FAILURE(*fatalErr)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic1==0xda &&
        p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic2==0x27 &&
        (isAcceptable==శూన్యసూచిక || isAcceptable(context, type, name, &p_శీర్షిక->info))
    ) {
        r_దత్తాంశం_Mem=UData_జ్ఞాపకం_create_New_Instance(fatalErr);
        ఒకవేళ (U_FAILURE(*fatalErr)) {
            తిరిగి శూన్యసూచిక;
        }
        r_దత్తాంశం_Mem->p_శీర్షిక = p_శీర్షిక;
    } లేకపోతే {
        /* the data is not acceptable, look further */
        /* If we eventually find something good, this errorcode will be */
        /*    cleared out.                                              */
        *nonFatalErr=U_INVALID_FORMAT_ERROR;
    }
    తిరిగి r_దత్తాంశం_Mem;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
స్థిర UData_జ్ఞాపకం *do_ఎక్కించు_From_Individual_Files(స్థిరరాశి అక్షరం *pkgName, 
        స్థిరరాశి అక్షరం *దత్తాంశం_మార్గం, స్థిరరాశి అక్షరం *toc_నమోదు_మార్గం_Suffix,
            /* following arguments are the same as doOpenChoice itself */
            స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *type, స్థిరరాశి అక్షరం *name,
             UData_జ్ఞాపకం_Is_Acceptable *isAcceptable, శూన్య *context,
             UError_సంకేతం *sub_Error_సంకేతం,
             UError_సంకేతం *p_Error_సంకేతం)
{
    స్థిరరాశి అక్షరం         *మార్గం_బఫరు;
    UData_జ్ఞాపకం         దత్తాంశం_జ్ఞాపకం;
    UData_జ్ఞాపకం *p_నమోదు_దత్తాంశం;

    /* look in ind. files: package\nam.typ  ========================= */
    /* init path iterator for individual files */
    UData_మార్గం_Iterator iter(దత్తాంశం_మార్గం, pkgName, మార్గం, toc_నమోదు_మార్గం_Suffix, అబద్ధం, p_Error_సంకేతం);

    అయితే ((మార్గం_బఫరు = iter.next(p_Error_సంకేతం)) != శూన్యసూచిక)
    {
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "UDATA: trying individual file %s\n", మార్గం_బఫరు);
#అయితే_ముగింపు
        ఒకవేళ (uprv_mapFile(&దత్తాంశం_జ్ఞాపకం, మార్గం_బఫరు, p_Error_సంకేతం))
        {
            p_నమోదు_దత్తాంశం = check_దత్తాంశం_అంశం(దత్తాంశం_జ్ఞాపకం.p_శీర్షిక, isAcceptable, context, type, name, sub_Error_సంకేతం, p_Error_సంకేతం);
            ఒకవేళ (p_నమోదు_దత్తాంశం != శూన్యసూచిక) {
                /* Data is good.
                *  Hand off ownership of the backing memory to the user's UDataMemory.
                *  and return it.   */
                p_నమోదు_దత్తాంశం->mapAddr = దత్తాంశం_జ్ఞాపకం.mapAddr;
                p_నమోదు_దత్తాంశం->map     = దత్తాంశం_జ్ఞాపకం.map;
                p_నమోదు_దత్తాంశం->length  = దత్తాంశం_జ్ఞాపకం.length;

#నిర్వచితమైతే UDATA_DEBUG
                fprintf(stderr, "** Mapped file: %s\n", మార్గం_బఫరు);
#అయితే_ముగింపు
                తిరిగి p_నమోదు_దత్తాంశం;
            }

            /* the data is not acceptable, or some error occurred.  Either way, unmap the memory */
            udata_close(&దత్తాంశం_జ్ఞాపకం);

            /* If we had a nasty error, bail out completely.  */
            ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
                తిరిగి శూన్యసూచిక;
            }

            /* Otherwise remember that we found data but didn't like it for some reason  */
            *sub_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        }
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "%s\n", UData_జ్ఞాపకం_is_Loaded(&దత్తాంశం_జ్ఞాపకం)?"LOADED":"not loaded");
#అయితే_ముగింపు
    }
    తిరిగి శూన్యసూచిక;
}

/**
 * @return 0 if not loaded, 1 if loaded or err 
 */
స్థిర UData_జ్ఞాపకం *do_ఎక్కించు_From_Common_దత్తాంశం(UBool isICUData, స్థిరరాశి అక్షరం * /*pkgName*/, 
        స్థిరరాశి అక్షరం * /*dataPath*/, స్థిరరాశి అక్షరం * /*tocEntryPathSuffix*/, స్థిరరాశి అక్షరం *toc_నమోదు_Name,
            /* following arguments are the same as doOpenChoice itself */
            స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *type, స్థిరరాశి అక్షరం *name,
             UData_జ్ఞాపకం_Is_Acceptable *isAcceptable, శూన్య *context,
             UError_సంకేతం *sub_Error_సంకేతం,
             UError_సంకేతం *p_Error_సంకేతం)
{
    UData_జ్ఞాపకం        *p_నమోదు_దత్తాంశం;
    స్థిరరాశి దత్తాంశం_శీర్షిక   *p_శీర్షిక;
    UData_జ్ఞాపకం        *p_Common_దత్తాంశం;
    int32_t            common_దత్తాంశం_సూచీ;
    UBool              checkedExtendedICUData = అబద్ధం;
    /* try to get common data.  The loop is for platforms such as the 390 that do
     *  not initially load the full set of ICU data.  If the lookup of an ICU data item
     *  fails, the full (but slower to load) set is loaded, the and the loop repeats,
     *  trying the lookup again.  Once the full set of ICU data is loaded, the loop wont
     *  repeat because the full set will be checked the first time through.
     *
     *  The loop also handles the fallback to a .dat file if the application linked
     *   to the stub data library rather than a real library.
     */
    కొరకు (common_దత్తాంశం_సూచీ = isICUData ? 0 : -1;;) {
        p_Common_దత్తాంశం=open_Common_దత్తాంశం(మార్గం, common_దత్తాంశం_సూచీ, sub_Error_సంకేతం); /** search for pkg **/

        ఒకవేళ(U_SUCCESS(*sub_Error_సంకేతం) && p_Common_దత్తాంశం!=శూన్యసూచిక) {
            int32_t length;

            /* look up the data piece in the common data */
            p_శీర్షిక=p_Common_దత్తాంశం->vFuncs->Lookup(p_Common_దత్తాంశం, toc_నమోదు_Name, &length, sub_Error_సంకేతం);
#నిర్వచితమైతే UDATA_DEBUG
            fprintf(stderr, "%s: pHeader=%p - %s\n", toc_నమోదు_Name, (శూన్య*) p_శీర్షిక, u_errorName(*sub_Error_సంకేతం));
#అయితే_ముగింపు

            ఒకవేళ(p_శీర్షిక!=శూన్యసూచిక) {
                p_నమోదు_దత్తాంశం = check_దత్తాంశం_అంశం(p_శీర్షిక, isAcceptable, context, type, name, sub_Error_సంకేతం, p_Error_సంకేతం);
#నిర్వచితమైతే UDATA_DEBUG
                fprintf(stderr, "pEntryData=%p\n", (శూన్య*) p_నమోదు_దత్తాంశం);
#అయితే_ముగింపు
                ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
                    తిరిగి శూన్యసూచిక;
                }
                ఒకవేళ (p_నమోదు_దత్తాంశం != శూన్యసూచిక) {
                    p_నమోదు_దత్తాంశం->length = length;
                    తిరిగి p_నమోదు_దత్తాంశం;
                }
            }
        }
        // If we failed due to being out-of-memory, then stop early and report the error.
        ఒకవేళ (*sub_Error_సంకేతం == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
            *p_Error_సంకేతం = *sub_Error_సంకేతం;
            తిరిగి శూన్యసూచిక;
        }
        /* Data wasn't found.  If we were looking for an ICUData item and there is
         * more data available, load it and try again,
         * otherwise break out of this loop. */
        ఒకవేళ (!isICUData) {
            తిరిగి శూన్యసూచిక;
        } లేకపోతే ఒకవేళ (p_Common_దత్తాంశం != శూన్యసూచిక) {
            ++common_దత్తాంశం_సూచీ;  /* try the next data package */
        } లేకపోతే ఒకవేళ ((!checkedExtendedICUData) && extendICUData(sub_Error_సంకేతం)) {
            checkedExtendedICUData = నిజం;
            /* try this data package slot again: it changed from nullptr to non-nullptr */
        } లేకపోతే {
            తిరిగి శూన్యసూచిక;
        }
    }
}

/*
 * Identify the Time Zone resources that are subject to special override data loading.
 */
స్థిర UBool is_సమయం_Zone_File(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *type) {
    తిరిగి ((uprv_strcmp(type, "res") == 0) &&
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
స్థిర UData_జ్ఞాపకం *
doOpenChoice(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *type, స్థిరరాశి అక్షరం *name,
             UData_జ్ఞాపకం_Is_Acceptable *isAcceptable, శూన్య *context,
             UError_సంకేతం *p_Error_సంకేతం)
{
    UData_జ్ఞాపకం         *retVal = శూన్యసూచిక;

    స్థిరరాశి అక్షరం         *దత్తాంశం_మార్గం;

    int32_t             toc_నమోదు_Suffix_సూచీ;
    స్థిరరాశి అక్షరం         *toc_నమోదు_మార్గం_Suffix;
    UError_సంకేతం          sub_Error_సంకేతం=U_ZERO_ERROR;
    స్థిరరాశి అక్షరం         *వృక్షం_Char;

    UBool               isICUData = అబద్ధం;


    FileTracer::traceOpen(మార్గం, type, name);


    /* Is this path ICU data? */
    ఒకవేళ(మార్గం == శూన్యసూచిక ||
       !strcmp(మార్గం, U_ICUDATA_ALIAS) ||  /* "ICUDATA" */
       !uprv_strncmp(మార్గం, U_ICUDATA_NAME U_వృక్షం_SEPARATOR_STRING, /* "icudt26e-" */
                     uprv_strlen(U_ICUDATA_NAME U_వృక్షం_SEPARATOR_STRING)) ||  
       !uprv_strncmp(మార్గం, U_ICUDATA_ALIAS U_వృక్షం_SEPARATOR_STRING, /* "ICUDATA-" */
                     uprv_strlen(U_ICUDATA_ALIAS U_వృక్షం_SEPARATOR_STRING))) {
      isICUData = నిజం;
    }

#ఒకవేళ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)  /* Windows:  try "foo\bar" and "foo/bar" */
    /* remap from alternate path char to the main one */
    CharString alt_Sep_మార్గం;
    ఒకవేళ(మార్గం) {
        ఒకవేళ(uprv_strchr(మార్గం,U_FILE_ALT_SEP_CHAR) != శూన్యసూచిక) {
            alt_Sep_మార్గం.append(మార్గం, *p_Error_సంకేతం);
            అక్షరం *p;
            అయితే ((p = uprv_strchr(alt_Sep_మార్గం.దత్తాంశం(), U_FILE_ALT_SEP_CHAR)) != శూన్యసూచిక) {
                *p = U_FILE_SEP_CHAR;
            }
#ఒకవేళ defined (UDATA_DEBUG)
            fprintf(stderr, "Changed path from [%s] to [%s]\n", మార్గం, alt_Sep_మార్గం.దత్తాంశం());
#అయితే_ముగింపు
            మార్గం = alt_Sep_మార్గం.దత్తాంశం();
        }
    }
#అయితే_ముగింపు

    CharString toc_నమోదు_Name; /* entry name in tree format. ex:  'icudt28b/coll/ar.res' */
    CharString toc_నమోదు_మార్గం; /* entry name in path format. ex:  'icudt28b\\coll\\ar.res' */

    CharString pkgName;
    CharString వృక్షం_Name;

    /* ======= Set up strings */
    ఒకవేళ(మార్గం==శూన్యసూచిక) {
        pkgName.append(U_ICUDATA_NAME, *p_Error_సంకేతం);
    } లేకపోతే {
        స్థిరరాశి అక్షరం *pkg;
        స్థిరరాశి అక్షరం *first;
        pkg = uprv_strrchr(మార్గం, U_FILE_SEP_CHAR);
        first = uprv_strchr(మార్గం, U_FILE_SEP_CHAR);
        ఒకవేళ(uprv_మార్గం_Is_Absolute(మార్గం) || (pkg != first)) { /* more than one slash in the path- not a tree name */
            /* see if this is an /absolute/path/to/package  path */
            ఒకవేళ(pkg) {
                pkgName.append(pkg+1, *p_Error_సంకేతం);
            } లేకపోతే {
                pkgName.append(మార్గం, *p_Error_సంకేతం);
            }
        } లేకపోతే {
            వృక్షం_Char = uprv_strchr(మార్గం, U_వృక్షం_SEPARATOR);
            ఒకవేళ(వృక్షం_Char) { 
                వృక్షం_Name.append(వృక్షం_Char+1, *p_Error_సంకేతం); /* following '-' */
                ఒకవేళ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_సంకేతం);
                } లేకపోతే {
                    pkgName.append(మార్గం, static_cast<int32_t>(వృక్షం_Char - మార్గం), *p_Error_సంకేతం);
                    ఒకవేళ (first == శూన్యసూచిక) {
                        /*
                        This user data has no path, but there is a tree name.
                        Look up the correct path from the data cache later.
                        */
                        మార్గం = pkgName.దత్తాంశం();
                    }
                }
            } లేకపోతే {
                ఒకవేళ(isICUData) {
                    pkgName.append(U_ICUDATA_NAME, *p_Error_సంకేతం);
                } లేకపోతే {
                    pkgName.append(మార్గం, *p_Error_సంకేతం);
                }
            }
        }
    }

#నిర్వచితమైతే UDATA_DEBUG
    fprintf(stderr, " P=%s T=%s\n", pkgName.దత్తాంశం(), వృక్షం_Name.దత్తాంశం());
#అయితే_ముగింపు

    /* setting up the entry name and file name 
     * Make up a full name by appending the type to the supplied
     *  name, assuming that a type was supplied.
     */

    /* prepend the package */
    toc_నమోదు_Name.append(pkgName, *p_Error_సంకేతం);
    toc_నమోదు_మార్గం.append(pkgName, *p_Error_సంకేతం);
    toc_నమోదు_Suffix_సూచీ = toc_నమోదు_Name.length();

    ఒకవేళ(!వృక్షం_Name.is_ఖాళీలు()) {
        toc_నమోదు_Name.append(U_వృక్షం_నమోదు_SEP_CHAR, *p_Error_సంకేతం).append(వృక్షం_Name, *p_Error_సంకేతం);
        toc_నమోదు_మార్గం.append(U_FILE_SEP_CHAR, *p_Error_సంకేతం).append(వృక్షం_Name, *p_Error_సంకేతం);
    }

    toc_నమోదు_Name.append(U_వృక్షం_నమోదు_SEP_CHAR, *p_Error_సంకేతం).append(name, *p_Error_సంకేతం);
    toc_నమోదు_మార్గం.append(U_FILE_SEP_CHAR, *p_Error_సంకేతం).append(name, *p_Error_సంకేతం);
    ఒకవేళ(type!=శూన్యసూచిక && *type!=0) {
        toc_నమోదు_Name.append(".", *p_Error_సంకేతం).append(type, *p_Error_సంకేతం);
        toc_నమోదు_మార్గం.append(".", *p_Error_సంకేతం).append(type, *p_Error_సంకేతం);
    }
    // The +1 is for the U_FILE_SEP_CHAR that is always appended above.
    toc_నమోదు_మార్గం_Suffix = toc_నమోదు_మార్గం.దత్తాంశం() + toc_నమోదు_Suffix_సూచీ + 1; /* suffix starts here */

#నిర్వచితమైతే UDATA_DEBUG
    fprintf(stderr, " tocEntryName = %s\n", toc_నమోదు_Name.దత్తాంశం());
    fprintf(stderr, " tocEntryPath = %s\n", toc_నమోదు_మార్గం.దత్తాంశం());
#అయితే_ముగింపు

#ఒకవేళ !defined(ICU_దత్తాంశం_DIR_WINDOWS)
    ఒకవేళ(మార్గం == శూన్యసూచిక) {
        మార్గం = COMMON_దత్తాంశం_NAME; /* "icudt26e" */
    }
#లేకపోతే
    // When using the Windows system data, we expects only a single data file.
    మార్గం = COMMON_దత్తాంశం_NAME; /* "icudt26e" */
#అయితే_ముగింపు

    /************************ Begin loop looking for ind. files ***************/
#నిర్వచితమైతే UDATA_DEBUG
    fprintf(stderr, "IND: inBasename = %s, pkg=%s\n", "(n/a)", package_Name_From_మార్గం(మార్గం));
#అయితే_ముగింపు

    /* End of dealing with a null basename */
    దత్తాంశం_మార్గం = u_get_దత్తాంశం_Directory();

    /****    Time zone individual files override  */
    ఒకవేళ (isICUData && type != శూన్యసూచిక && is_సమయం_Zone_File(name, type)) {
        స్థిరరాశి అక్షరం *tzFilesDir = u_get_సమయం_Zone_Files_Directory(p_Error_సంకేతం);
        ఒకవేళ (tzFilesDir[0] != 0) {
#నిర్వచితమైతే UDATA_DEBUG
            fprintf(stderr, "Trying Time Zone Files directory = %s\n", tzFilesDir);
#అయితే_ముగింపు
            retVal = do_ఎక్కించు_From_Individual_Files(/* pkgName.data() */ "", tzFilesDir, toc_నమోదు_మార్గం_Suffix,
                            /*మార్గం*/ "", type, name, isAcceptable, context, &sub_Error_సంకేతం, p_Error_సంకేతం);
            ఒకవేళ((retVal != శూన్యసూచిక) || U_FAILURE(*p_Error_సంకేతం)) {
                తిరిగి retVal;
            }
        }
    }

    /****    COMMON PACKAGE  - only if packages are first. */
    ఒకవేళ(g_దత్తాంశం_File_Access == UDATA_PACKAGES_మొదటి) {
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_PACKAGES_FIRST)\n");
#అయితే_ముగింపు
        /* #2 */
        retVal = do_ఎక్కించు_From_Common_దత్తాంశం(isICUData, 
                            pkgName.దత్తాంశం(), దత్తాంశం_మార్గం, toc_నమోదు_మార్గం_Suffix, toc_నమోదు_Name.దత్తాంశం(),
                            మార్గం, type, name, isAcceptable, context, &sub_Error_సంకేతం, p_Error_సంకేతం);
        ఒకవేళ((retVal != శూన్యసూచిక) || U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి retVal;
        }
    }

    /****    INDIVIDUAL FILES  */
    ఒకవేళ((g_దత్తాంశం_File_Access==UDATA_PACKAGES_మొదటి) ||
       (g_దత్తాంశం_File_Access==UDATA_FILES_మొదటి)) {
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "Trying individual files\n");
#అయితే_ముగింపు
        /* Check to make sure that there is a dataPath to iterate over */
        ఒకవేళ ((దత్తాంశం_మార్గం && *దత్తాంశం_మార్గం) || !isICUData) {
            retVal = do_ఎక్కించు_From_Individual_Files(pkgName.దత్తాంశం(), దత్తాంశం_మార్గం, toc_నమోదు_మార్గం_Suffix,
                            మార్గం, type, name, isAcceptable, context, &sub_Error_సంకేతం, p_Error_సంకేతం);
            ఒకవేళ((retVal != శూన్యసూచిక) || U_FAILURE(*p_Error_సంకేతం)) {
                తిరిగి retVal;
            }
        }
    }

    /****    COMMON PACKAGE  */
    ఒకవేళ((g_దత్తాంశం_File_Access==UDATA_ONLY_PACKAGES) || 
       (g_దత్తాంశం_File_Access==UDATA_FILES_మొదటి)) {
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "Trying packages (UDATA_ONLY_PACKAGES || UDATA_FILES_FIRST)\n");
#అయితే_ముగింపు
        retVal = do_ఎక్కించు_From_Common_దత్తాంశం(isICUData,
                            pkgName.దత్తాంశం(), దత్తాంశం_మార్గం, toc_నమోదు_మార్గం_Suffix, toc_నమోదు_Name.దత్తాంశం(),
                            మార్గం, type, name, isAcceptable, context, &sub_Error_సంకేతం, p_Error_సంకేతం);
        ఒకవేళ((retVal != శూన్యసూచిక) || U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి retVal;
        }
    }
    
    /* Load from DLL.  If we haven't attempted package load, we also haven't had any chance to
        try a DLL (static or setCommonData/etc)  load.
         If we ever have a "UDATA_ONLY_FILES", add it to the or list here.  */  
    ఒకవేళ(g_దత్తాంశం_File_Access==UDATA_NO_FILES) {
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "Trying common data (UDATA_NO_FILES)\n");
#అయితే_ముగింపు
        retVal = do_ఎక్కించు_From_Common_దత్తాంశం(isICUData,
                            pkgName.దత్తాంశం(), "", toc_నమోదు_మార్గం_Suffix, toc_నమోదు_Name.దత్తాంశం(),
                            మార్గం, type, name, isAcceptable, context, &sub_Error_సంకేతం, p_Error_సంకేతం);
        ఒకవేళ((retVal != శూన్యసూచిక) || U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి retVal;
        }
    }

    /* data not found */
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        ఒకవేళ(U_SUCCESS(sub_Error_సంకేతం)) {
            /* file not found */
            *p_Error_సంకేతం=U_FILE_ACCESS_ERROR;
        } లేకపోతే {
            /* entry point not found or rejected */
            *p_Error_సంకేతం=sub_Error_సంకేతం;
        }
    }
    తిరిగి retVal;
}



/* API ---------------------------------------------------------------------- */

U_CAPI UData_జ్ఞాపకం * U_EXPORT2
udata_open(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *type, స్థిరరాశి అక్షరం *name,
           UError_సంకేతం *p_Error_సంకేతం) {
#నిర్వచితమైతే UDATA_DEBUG
  fprintf(stderr, "udata_open(): Opening: %s : %s . %s\n", (మార్గం?మార్గం:"nullptr"), name, type);
    fflush(stderr);
#అయితే_ముగింపు

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    } లేకపోతే ఒకవేళ(name==శూన్యసూచిక || *name==0) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        తిరిగి doOpenChoice(మార్గం, type, name, శూన్యసూచిక, శూన్యసూచిక, p_Error_సంకేతం);
    }
}



U_CAPI UData_జ్ఞాపకం * U_EXPORT2
udata_openChoice(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *type, స్థిరరాశి అక్షరం *name,
                 UData_జ్ఞాపకం_Is_Acceptable *isAcceptable, శూన్య *context,
                 UError_సంకేతం *p_Error_సంకేతం) {
#నిర్వచితమైతే UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s : %s . %s\n", (మార్గం?మార్గం:"nullptr"), name, type);
#అయితే_ముగింపు

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    } లేకపోతే ఒకవేళ(name==శూన్యసూచిక || *name==0 || isAcceptable==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        తిరిగి doOpenChoice(మార్గం, type, name, isAcceptable, context, p_Error_సంకేతం);
    }
}



U_CAPI శూన్య U_EXPORT2
udata_getInfo(UData_జ్ఞాపకం *p_దత్తాంశం, UDataInfo *pInfo) {
    ఒకవేళ(pInfo!=శూన్యసూచిక) {
        ఒకవేళ(p_దత్తాంశం!=శూన్యసూచిక && p_దత్తాంశం->p_శీర్షిక!=శూన్యసూచిక) {
            స్థిరరాశి UDataInfo *info=&p_దత్తాంశం->p_శీర్షిక->info;
            uint16_t దత్తాంశం_Info_కొలత=udata_get_Info_కొలత(info);
            ఒకవేళ(pInfo->size>దత్తాంశం_Info_కొలత) {
                pInfo->size=దత్తాంశం_Info_కొలత;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (స్థిరరాశి uint16_t *)info+1, pInfo->size-2);
            ఒకవేళ(info->isBigEndian!=U_IS_BIG_ENDIAN) {
                /* opposite endianness */
                uint16_t x=info->reserved_మాట;
                pInfo->reserved_మాట=(uint16_t)((x<<8)|(x>>8));
            }
        } లేకపోతే {
            pInfo->size=0;
        }
    }
}


U_CAPI శూన్య U_EXPORT2 udata_సమితి_File_Access(UDataFileAccess access, UError_సంకేతం * /*స్థితి*/)
{
    // Note: this function is documented as not thread safe.
    g_దత్తాంశం_File_Access = access;
}
