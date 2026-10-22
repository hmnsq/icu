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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_CONVERSION

#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "ustr_cnv.h"
#અન્તર્ભાવ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

સ્થિર UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    જો (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        જો (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    જો(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        જો(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    પાછા converter;
}

U_CAPI શૂન્ય U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    જો(gDefaultConverter == nullptr) {
        જો (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        જો(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    જો(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI શૂન્ય U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    જો (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        જો (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    જો(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
સ્થિર int32_t u_astrnlen(અચલ અક્ષર *s1, int32_t n)
{
    int32_t len = 0;

    જો (s1)
    {
        જ્યારે (n-- && *(s1++))
        {
            len++;
        }
    }
    પાછા len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, અચલ અક્ષર* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  જો(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   સાચું,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    જો(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    જો(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } નહીંતર {
    *ucs1 = 0;
  }
  પાછા ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, અચલ અક્ષર* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  જો(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    જો(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } નહીંતર {
    *ucs1 = 0;
  }
  પાછા ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
સ્થિર int32_t u_ustrnlen(અચલ char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    જો (ucs1)
    {
        જ્યારે (n-- && *(ucs1++))
        {
            len++;
        }
    }
    પાછા len;
}

U_CAPI અક્ષર* U_EXPORT2
u_austrncpy(અક્ષર* s1 U_LIFETIME_BOUND, અચલ char16_t* ucs2, int32_t n)
{
  અક્ષર *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  જો(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  સાચું,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    જો(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    જો(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } નહીંતર {
    *s1 = 0;
  }
  પાછા s1;
}

U_CAPI અક્ષર* U_EXPORT2
u_austrcpy(અક્ષર* s1 U_LIFETIME_BOUND, અચલ char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  જો(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } નહીંતર {
    *s1 = 0;
  }
  પાછા s1;
}

#endif
