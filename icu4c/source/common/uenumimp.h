// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uenumimp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2002jul08
*   created by: Vladimir Weinstein
*/

#جے_بنا_تعریف __UENUMIMP_H
#تعریف_کرو __UENUMIMP_H

#ضمن "unicode/uenum.h"

U_CDECL_BEGIN

/** 
 * following are the type declarations for 
 * implementations of APIs. If any of these
 * functions are NULL, U_UNSUPPORTED_ERROR
 * is returned. If close is NULL, the enumeration
 * object is going to be released.
 * Initial error checking is done in the body
 * of API function, so the implementations 
 * need not to check the initial error condition.
 */

/**
 * Function type declaration for uenum_close().
 *
 * This function should cleanup the enumerator object
 *
 * @param en enumeration to be closed
 */
تعريفنوع فراغ U_CALLCONV
UEnumClose(UEnumeration *en);

/**
 * Function type declaration for uenum_count().
 *
 * This function should count the number of elements
 * in this enumeration
 *
 * @param en enumeration to be counted
 * @param status pointer to UErrorCode variable
 * @return number of elements in enumeration
 */
تعريفنوع int32_t U_CALLCONV
UEnum_گنتی(UEnumeration *en, UError_کوڈ *حالت);

/**
 * Function type declaration for uenum_unext().
 *
 * This function returns the next element as a UChar *,
 * or NULL after all elements haven been enumerated.
 *
 * @param en enumeration 
 * @param resultLength pointer to result length
 * @param status pointer to UErrorCode variable
 * @return next element as UChar *,
 *         or NULL after all elements haven been enumerated
 */
تعريفنوع ثابت UChar* U_CALLCONV 
UEnumUNext(UEnumeration* en,
            int32_t* نتیجہ_لمائی,
            UError_کوڈ* حالت);

/**
 * Function type declaration for uenum_next().
 *
 * This function returns the next element as a char *,
 * or NULL after all elements haven been enumerated.
 *
 * @param en enumeration 
 * @param resultLength pointer to result length
 * @param status pointer to UErrorCode variable
 * @return next element as char *,
 *         or NULL after all elements haven been enumerated
 */
تعريفنوع ثابت حرف* U_CALLCONV 
UEnum_اگلا(UEnumeration* en,
           int32_t* نتیجہ_لمائی,
           UError_کوڈ* حالت);

/**
 * Function type declaration for uenum_reset().
 *
 * This function should reset the enumeration 
 * object
 *
 * @param en enumeration 
 * @param status pointer to UErrorCode variable
 */
تعريفنوع فراغ U_CALLCONV 
UEnumReset(UEnumeration* en, 
            UError_کوڈ* حالت);


بنية UEnumeration {
    /* baseContext. For the base class only. Don't touch! */
    فراغ *ادھار_Context;

    /* context. Use it for what you need */
    فراغ *context;

    /** 
     * these are functions that will 
     * be used for APIs
     */
    /* called from uenum_close */
    UEnumClose *close;
    /* called from uenum_count */
    UEnum_گنتی *گنتی;
    /* called from uenum_unext */
    UEnumUNext *u_اگلا;
    /* called from uenum_next */
    UEnum_اگلا  *اگلا;
    /* called from uenum_reset */
    UEnumReset *reset;
};

U_CDECL_سرا

/* This is the default implementation for uenum_unext().
 * It automatically converts the char * string to UChar *.
 * Don't call this directly.  This is called internally by uenum_unext
 * when a UEnumeration is defined with 'uNext' pointing to this
 * function.
 */
U_CAPI ثابت UChar* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* نتیجہ_لمائی,
                   UError_کوڈ* حالت);

/* This is the default implementation for uenum_next().
 * It automatically converts the UChar * string to char *.
 * Don't call this directly.  This is called internally by uenum_next
 * when a UEnumeration is defined with 'next' pointing to this
 * function.
 */
U_CAPI ثابت حرف* U_EXPORT2
uenum_اگلا_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجہ_لمائی,
                  UError_کوڈ* حالت);

#جے_انت
