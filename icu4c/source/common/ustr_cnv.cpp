// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1998-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ustr_cnv.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug24
*   created by: Markus W. Scherer
*
*   Character conversion functions moved here from ustring.c
*/

#include "unicode/utypes.h"

#ถ้า !UCONFIG_NO_CONVERSION

#include "unicode/ustring.h"
#include "unicode/ucnv.h"
#include "cstring.h"
#include "cmemory.h"
#include "umutex.h"
#include "ustr_cnv.h"
#include "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

static UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    ถ้า (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        ถ้า (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    ถ้า(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        ถ้า(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    คืน converter;
}

U_CAPI void U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    ถ้า(gDefaultConverter == nullptr) {
        ถ้า (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        ถ้า(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    ถ้า(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI void U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    ถ้า (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        ถ้า (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    ถ้า(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
static int32_t u_astrnlen(const var_na *s1, int32_t n)
{
    int32_t len = 0;

    ถ้า (s1)
    {
        ขณะ (n-- && *(s1++))
        {
            len++;
        }
    }
    คืน len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, const var_na* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ถ้า(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   true,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ถ้า(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    ถ้า(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } มิฉะนั้น {
    *ucs1 = 0;
  }
  คืน ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, const var_na* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ถ้า(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    ถ้า(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } มิฉะนั้น {
    *ucs1 = 0;
  }
  คืน ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
static int32_t u_ustrnlen(const char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    ถ้า (ucs1)
    {
        ขณะ (n-- && *(ucs1++))
        {
            len++;
        }
    }
    คืน len;
}

U_CAPI var_na* U_EXPORT2
u_austrncpy(var_na* s1 U_LIFETIME_BOUND, const char16_t* ucs2, int32_t n)
{
  var_na *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ถ้า(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  true,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ถ้า(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    ถ้า(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } มิฉะนั้น {
    *s1 = 0;
  }
  คืน s1;
}

U_CAPI var_na* U_EXPORT2
u_austrcpy(var_na* s1 U_LIFETIME_BOUND, const char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ถ้า(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } มิฉะนั้น {
    *s1 = 0;
  }
  คืน s1;
}

#endif
