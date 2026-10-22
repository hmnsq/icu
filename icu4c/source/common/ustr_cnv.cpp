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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION

#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "ustr_cnv.h"
#அந்தர்பாவ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

நிலையான UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    எனில் (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        எனில் (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    எனில்(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        எனில்(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    திரும்பு converter;
}

U_CAPI வெறுமை U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    எனில்(gDefaultConverter == nullptr) {
        எனில் (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        எனில்(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    எனில்(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI வெறுமை U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    எனில் (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        எனில் (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    எனில்(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
நிலையான int32_t u_astrnlen(மாறிலி எழுத்து *s1, int32_t n)
{
    int32_t len = 0;

    எனில் (s1)
    {
        போது (n-- && *(s1++))
        {
            len++;
        }
    }
    திரும்பு len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, மாறிலி எழுத்து* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  எனில்(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   உண்மை,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    எனில்(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    எனில்(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } இல்லையெனில் {
    *ucs1 = 0;
  }
  திரும்பு ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, மாறிலி எழுத்து* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  எனில்(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    எனில்(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } இல்லையெனில் {
    *ucs1 = 0;
  }
  திரும்பு ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
நிலையான int32_t u_ustrnlen(மாறிலி char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    எனில் (ucs1)
    {
        போது (n-- && *(ucs1++))
        {
            len++;
        }
    }
    திரும்பு len;
}

U_CAPI எழுத்து* U_EXPORT2
u_austrncpy(எழுத்து* s1 U_LIFETIME_BOUND, மாறிலி char16_t* ucs2, int32_t n)
{
  எழுத்து *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  எனில்(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  உண்மை,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    எனில்(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    எனில்(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } இல்லையெனில் {
    *s1 = 0;
  }
  திரும்பு s1;
}

U_CAPI எழுத்து* U_EXPORT2
u_austrcpy(எழுத்து* s1 U_LIFETIME_BOUND, மாறிலி char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  எனில்(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } இல்லையெனில் {
    *s1 = 0;
  }
  திரும்பு s1;
}

#endif
