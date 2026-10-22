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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

ਸਥਿਰ UConverter *gDefaultConverter = ਖਾਲੀ_ਸੂਚਕ;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UError_ਕੋਡ *ਸਥਿਤੀ)
{
    UConverter *converter = ਖਾਲੀ_ਸੂਚਕ;
    
    ਜੇ (gDefaultConverter != ਖਾਲੀ_ਸੂਚਕ) {
        icu::umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
        
        /* need to check to make sure it wasn't taken out from under us */
        ਜੇ (gDefaultConverter != ਖਾਲੀ_ਸੂਚਕ) {
            converter = gDefaultConverter;
            gDefaultConverter = ਖਾਲੀ_ਸੂਚਕ;
        }
        icu::umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);
    }

    /* if the cache was empty, create a converter */
    ਜੇ(converter == ਖਾਲੀ_ਸੂਚਕ) {
        converter = ucnv_open(ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
            ucnv_close(converter);
            converter = ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    ਵਾਪਸ converter;
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    ਜੇ(gDefaultConverter == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (converter != ਖਾਲੀ_ਸੂਚਕ) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
        ਜੇ(gDefaultConverter == ਖਾਲੀ_ਸੂਚਕ) {
            gDefaultConverter = converter;
            converter = ਖਾਲੀ_ਸੂਚਕ;
        }
        icu::umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);
    }

    ਜੇ(converter != ਖਾਲੀ_ਸੂਚਕ) {
        ucnv_close(converter);
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = ਖਾਲੀ_ਸੂਚਕ;
    
    ਜੇ (gDefaultConverter != ਖਾਲੀ_ਸੂਚਕ) {
        icu::umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
        
        /* need to check to make sure it wasn't taken out from under us */
        ਜੇ (gDefaultConverter != ਖਾਲੀ_ਸੂਚਕ) {
            converter = gDefaultConverter;
            gDefaultConverter = ਖਾਲੀ_ਸੂਚਕ;
        }
        icu::umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);
    }

    /* if the cache was populated, flush it */
    ਜੇ(converter != ਖਾਲੀ_ਸੂਚਕ) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ਸਥਿਰ int32_t u_astrnlen(ਅਚਲ ਅੱਖਰ *s1, int32_t n)
{
    int32_t len = 0;

    ਜੇ (s1)
    {
        ਜਦੋਂ (n-- && *(s1++))
        {
            len++;
        }
    }
    ਵਾਪਸ len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ਅਚਲ ਅੱਖਰ* s2, int32_t n)
{
  char16_t *ਨਿਸ਼ਾਨਾ = ucs1;
  UError_ਕੋਡ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ਜੇ(U_SUCCESS(err) && cnv != ਖਾਲੀ_ਸੂਚਕ) {
    ucnv_reset(cnv);
    ucnv_ਮੰਜ਼ਿਲ_Unicode(cnv,
                   &ਨਿਸ਼ਾਨਾ,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   ਖਾਲੀ_ਸੂਚਕ,
                   ਸੱਚ,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ਜੇ(U_FAILURE(err) && (err != U_ਬਫਰ_ਉਛਾਲ_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    ਜੇ(ਨਿਸ਼ਾਨਾ < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *ਨਿਸ਼ਾਨਾ = 0;  /* terminate */
    }
  } ਨਹੀਂਤਾਂ {
    *ucs1 = 0;
  }
  ਵਾਪਸ ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ਅਚਲ ਅੱਖਰ* s2)
{
  UError_ਕੋਡ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ਜੇ(U_SUCCESS(err) && cnv != ਖਾਲੀ_ਸੂਚਕ) {
    ucnv_ਮੰਜ਼ਿਲ_UChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    ਜੇ(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } ਨਹੀਂਤਾਂ {
    *ucs1 = 0;
  }
  ਵਾਪਸ ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ਸਥਿਰ int32_t u_ustrnlen(ਅਚਲ char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    ਜੇ (ucs1)
    {
        ਜਦੋਂ (n-- && *(ucs1++))
        {
            len++;
        }
    }
    ਵਾਪਸ len;
}

U_CAPI ਅੱਖਰ* U_EXPORT2
u_austrncpy(ਅੱਖਰ* s1 U_LIFETIME_BOUND, ਅਚਲ char16_t* ucs2, int32_t n)
{
  ਅੱਖਰ *ਨਿਸ਼ਾਨਾ = s1;
  UError_ਕੋਡ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ਜੇ(U_SUCCESS(err) && cnv != ਖਾਲੀ_ਸੂਚਕ) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &ਨਿਸ਼ਾਨਾ,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  ਖਾਲੀ_ਸੂਚਕ,
                  ਸੱਚ,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ਜੇ(U_FAILURE(err) && (err != U_ਬਫਰ_ਉਛਾਲ_ERROR) ) {
      *s1 = 0; /* failure */
    }
    ਜੇ(ਨਿਸ਼ਾਨਾ < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *ਨਿਸ਼ਾਨਾ = 0;  /* terminate */
    }
  } ਨਹੀਂਤਾਂ {
    *s1 = 0;
  }
  ਵਾਪਸ s1;
}

U_CAPI ਅੱਖਰ* U_EXPORT2
u_austrcpy(ਅੱਖਰ* s1 U_LIFETIME_BOUND, ਅਚਲ char16_t* ucs2)
{
  UError_ਕੋਡ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ਜੇ(U_SUCCESS(err) && cnv != ਖਾਲੀ_ਸੂਚਕ) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } ਨਹੀਂਤਾਂ {
    *s1 = 0;
  }
  ਵਾਪਸ s1;
}

#ਜੇ_ਅੰਤ
