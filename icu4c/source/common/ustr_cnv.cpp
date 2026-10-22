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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "ustr_cnv.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

ସ୍ଥିର UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    ଯଦି (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        ଯଦି (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    ଯଦି(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        ଯଦି(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    ଫେରନ୍ତୁ converter;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    ଯଦି(gDefaultConverter == nullptr) {
        ଯଦି (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        ଯଦି(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    ଯଦି(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    ଯଦି (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        ଯଦି (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    ଯଦି(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ସ୍ଥିର int32_t u_astrnlen(ଅଚଳ ଅକ୍ଷର *s1, int32_t n)
{
    int32_t len = 0;

    ଯଦି (s1)
    {
        ଯେତେବେଳେ (n-- && *(s1++))
        {
            len++;
        }
    }
    ଫେରନ୍ତୁ len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ଅଚଳ ଅକ୍ଷର* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ଯଦି(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   ସତ୍ୟ,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ଯଦି(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    ଯଦି(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } ଅନ୍ୟଥା {
    *ucs1 = 0;
  }
  ଫେରନ୍ତୁ ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ଅଚଳ ଅକ୍ଷର* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ଯଦି(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    ଯଦି(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } ଅନ୍ୟଥା {
    *ucs1 = 0;
  }
  ଫେରନ୍ତୁ ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ସ୍ଥିର int32_t u_ustrnlen(ଅଚଳ char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    ଯଦି (ucs1)
    {
        ଯେତେବେଳେ (n-- && *(ucs1++))
        {
            len++;
        }
    }
    ଫେରନ୍ତୁ len;
}

U_CAPI ଅକ୍ଷର* U_EXPORT2
u_austrncpy(ଅକ୍ଷର* s1 U_LIFETIME_BOUND, ଅଚଳ char16_t* ucs2, int32_t n)
{
  ଅକ୍ଷର *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ଯଦି(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  ସତ୍ୟ,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ଯଦି(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    ଯଦି(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } ଅନ୍ୟଥା {
    *s1 = 0;
  }
  ଫେରନ୍ତୁ s1;
}

U_CAPI ଅକ୍ଷର* U_EXPORT2
u_austrcpy(ଅକ୍ଷର* s1 U_LIFETIME_BOUND, ଅଚଳ char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ଯଦି(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } ଅନ୍ୟଥା {
    *s1 = 0;
  }
  ଫେରନ୍ତୁ s1;
}

#endif
