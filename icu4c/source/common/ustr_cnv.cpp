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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ustring.h"
#شامل "unicode/ucnv.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "umutex.h"
#شامل "ustr_cnv.h"
#شامل "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

ایستا UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    اگر (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        اگر (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    اگر(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        اگر(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    بازگشت converter;
}

U_CAPI تهی U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    اگر(gDefaultConverter == nullptr) {
        اگر (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        اگر(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    اگر(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI تهی U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    اگر (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        اگر (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    اگر(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ایستا int32_t u_astrnlen(ثابت نویسه *s1, int32_t n)
{
    int32_t len = 0;

    اگر (s1)
    {
        وقتی (n-- && *(s1++))
        {
            len++;
        }
    }
    بازگشت len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ثابت نویسه* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  اگر(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   درست,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    اگر(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    اگر(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } وگرنه {
    *ucs1 = 0;
  }
  بازگشت ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ثابت نویسه* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  اگر(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    اگر(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } وگرنه {
    *ucs1 = 0;
  }
  بازگشت ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ایستا int32_t u_ustrnlen(ثابت char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    اگر (ucs1)
    {
        وقتی (n-- && *(ucs1++))
        {
            len++;
        }
    }
    بازگشت len;
}

U_CAPI نویسه* U_EXPORT2
u_austrncpy(نویسه* s1 U_LIFETIME_BOUND, ثابت char16_t* ucs2, int32_t n)
{
  نویسه *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  اگر(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  درست,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    اگر(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    اگر(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } وگرنه {
    *s1 = 0;
  }
  بازگشت s1;
}

U_CAPI نویسه* U_EXPORT2
u_austrcpy(نویسه* s1 U_LIFETIME_BOUND, ثابت char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  اگر(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } وگرنه {
    *s1 = 0;
  }
  بازگشت s1;
}

#endif
