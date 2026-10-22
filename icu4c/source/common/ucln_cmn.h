// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2001-2016, International Business Machines
*                Corporation and others. All Rights Reserved.
******************************************************************************
*   file name:  ucln_cmn.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001July05
*   created by: George Rhoten
*/

#నిర్వచితం_కాకపోతే __UCLN_CMN_H__
#నిర్వచించు __UCLN_CMN_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "ucln.h"

/* These are the cleanup functions for various APIs. */
/* @return true if cleanup complete successfully.*/
U_CFUNC UBool utrace_cleanup(శూన్య);

U_CFUNC UBool ucln_lib_cleanup(శూన్య);

/*
Please keep the order of enums declared in same order
as the cleanup functions are suppose to be called. */
రకనిర్వచనం గణన ECleanupCommonType {
    UCLN_COMMON_ప్రారంభం = -1,
    UCLN_COMMON_NUMPARSE_UNISETS,
    UCLN_COMMON_USPREP,
    UCLN_COMMON_BREAKITERATOR,
    UCLN_COMMON_RBBI,
    UCLN_COMMON_SERVICE,
    UCLN_COMMON_LOCALE_కీలకం_TYPE,
    UCLN_COMMON_LOCALE,
    UCLN_COMMON_LOCALE_ALIAS,
    UCLN_COMMON_LOCALE_తెలిసిన_CANONICALIZED,
    UCLN_COMMON_LOCALE_అందుబాటు,
    UCLN_COMMON_LIKELY_SUBTAGS,
    UCLN_COMMON_LOCALE_దూరం,
    UCLN_COMMON_ULOC,
    UCLN_COMMON_CURRENCY,
    UCLN_COMMON_LOADED_NORMALIZER2,
    UCLN_COMMON_NORMALIZER2,
    UCLN_COMMON_CHARACTERPROPERTIES,
    UCLN_COMMON_USET,
    UCLN_COMMON_UNAMES,
    UCLN_COMMON_UPROPS,
    UCLN_COMMON_EMOJIPROPS,
    UCLN_COMMON_UCNV,
    UCLN_COMMON_UCNV_IO,
    UCLN_COMMON_UDATA,
    UCLN_COMMON_PUTIL,
    UCLN_COMMON_UINIT,

    /*
       Unified caches caches collation stuff. Collation data structures
       contain resource bundles which means that unified cache cleanup
       must happen before resource bundle clean up.
    */
    UCLN_COMMON_UNIFIED_CACHE,
    UCLN_COMMON_URES,
    UCLN_COMMON_MUTEX,    // Mutexes should be the last to be cleaned up.
    UCLN_COMMON_లెక్క /* This must be last */
} ECleanupCommonType;

/* Main library cleanup registration function. */
/* See common/ucln.h for details on adding a cleanup function. */
/* Note: the global mutex must not be held when calling this function. */
U_CFUNC శూన్య U_EXPORT2 ucln_common_రిజిస్టరు_Cleanup(ECleanupCommonType type,
                                                   cleanupFunc *func);

#అయితే_ముగింపు
