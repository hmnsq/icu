// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucol_swp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003sep10
*   created by: Markus W. Scherer
*
*   Swap collation binaries.
*/

#جيڪڏهن_وصف_نه_ٿيل __UCOL_SWP_H__
#وصف_ڪريو __UCOL_SWP_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_COLLATION

#ضمن "udataswp.h"

/*
 * Does the data look like a collation binary?
 * @internal
 */
U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ٻٽو(ثابت UDataSwapper *ds,
                              ثابت فراغ *اندريون_معلومات, int32_t ڊيگهائي);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_مٽا_سٽا(ثابت UDataSwapper *ds,
          ثابت فراغ *اندريون_معلومات, int32_t ڊيگهائي, فراغ *ٻاهريون_معلومات,
          UError_ڪوڊ *p_Error_ڪوڊ);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_مٽا_سٽا_Inverse_UCA(ثابت UDataSwapper *ds,
                    ثابت فراغ *اندريون_معلومات, int32_t ڊيگهائي, فراغ *ٻاهريون_معلومات,
                    UError_ڪوڊ *p_Error_ڪوڊ);

#جيڪڏهن_پورو /* #if !UCONFIG_NO_COLLATION */

#جيڪڏهن_پورو
