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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "ustr_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

ಸ್ಥಿರ UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    ಯದಿ (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        ಯದಿ (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    ಯದಿ(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        ಯದಿ(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    ಪ್ರತ್ಯಾಗಮ converter;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    ಯದಿ(gDefaultConverter == nullptr) {
        ಯದಿ (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        ಯದಿ(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    ಯದಿ(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    ಯದಿ (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        ಯದಿ (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    ಯದಿ(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ಸ್ಥಿರ int32_t u_astrnlen(ಧ್ರುವ ವರ್ಣ *s1, int32_t n)
{
    int32_t len = 0;

    ಯದಿ (s1)
    {
        ಯಾವತ್ (n-- && *(s1++))
        {
            len++;
        }
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ಧ್ರುವ ವರ್ಣ* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ಯದಿ(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   ಸತ್ಯ,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ಯದಿ(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    ಯದಿ(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } ಅನ್ಯಥಾ {
    *ucs1 = 0;
  }
  ಪ್ರತ್ಯಾಗಮ ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ಧ್ರುವ ವರ್ಣ* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ಯದಿ(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    ಯದಿ(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } ಅನ್ಯಥಾ {
    *ucs1 = 0;
  }
  ಪ್ರತ್ಯಾಗಮ ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ಸ್ಥಿರ int32_t u_ustrnlen(ಧ್ರುವ char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    ಯದಿ (ucs1)
    {
        ಯಾವತ್ (n-- && *(ucs1++))
        {
            len++;
        }
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

U_CAPI ವರ್ಣ* U_EXPORT2
u_austrncpy(ವರ್ಣ* s1 U_LIFETIME_BOUND, ಧ್ರುವ char16_t* ucs2, int32_t n)
{
  ವರ್ಣ *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ಯದಿ(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  ಸತ್ಯ,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ಯದಿ(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    ಯದಿ(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } ಅನ್ಯಥಾ {
    *s1 = 0;
  }
  ಪ್ರತ್ಯಾಗಮ s1;
}

U_CAPI ವರ್ಣ* U_EXPORT2
u_austrcpy(ವರ್ಣ* s1 U_LIFETIME_BOUND, ಧ್ರುವ char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ಯದಿ(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } ಅನ್ಯಥಾ {
    *s1 = 0;
  }
  ಪ್ರತ್ಯಾಗಮ s1;
}

#endif
