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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ustring.h"
#समावेश "unicode/ucnv.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "umutex.h"
#समावेश "ustr_cnv.h"
#समावेश "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

अटल UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    अगर (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        अगर (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    अगर(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        अगर(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    वापस converter;
}

U_CAPI व्योम U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    अगर(gDefaultConverter == nullptr) {
        अगर (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        अगर(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    अगर(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI व्योम U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    अगर (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        अगर (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    अगर(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#घोषणा MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
अटल int32_t u_astrnlen(स्थिर अक्षर *s1, int32_t n)
{
    int32_t len = 0;

    अगर (s1)
    {
        जबतक (n-- && *(s1++))
        {
            len++;
        }
    }
    वापस len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, स्थिर अक्षर* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  अगर(U_SUCCESS(err) && cnv != nullptr) {
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
    अगर(U_FAILURE(err) && (err != U_बफर_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    अगर(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } अन्यथा {
    *ucs1 = 0;
  }
  वापस ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, स्थिर अक्षर* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  अगर(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    अगर(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } अन्यथा {
    *ucs1 = 0;
  }
  वापस ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
अटल int32_t u_ustrnlen(स्थिर char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    अगर (ucs1)
    {
        जबतक (n-- && *(ucs1++))
        {
            len++;
        }
    }
    वापस len;
}

U_CAPI अक्षर* U_EXPORT2
u_austrncpy(अक्षर* s1 U_LIFETIME_BOUND, स्थिर char16_t* ucs2, int32_t n)
{
  अक्षर *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  अगर(U_SUCCESS(err) && cnv != nullptr) {
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
    अगर(U_FAILURE(err) && (err != U_बफर_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    अगर(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } अन्यथा {
    *s1 = 0;
  }
  वापस s1;
}

U_CAPI अक्षर* U_EXPORT2
u_austrcpy(अक्षर* s1 U_LIFETIME_BOUND, स्थिर char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  अगर(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } अन्यथा {
    *s1 = 0;
  }
  वापस s1;
}

#पूर्ण_अगर
