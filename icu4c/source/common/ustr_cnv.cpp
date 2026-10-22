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

ساکن UConverter *gDefaultConverter = خالی_اشاریہ;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UError_رمز *حالت_2)
{
    UConverter *converter = خالی_اشاریہ;
    
    اگر (gDefaultConverter != خالی_اشاریہ) {
        icu::umtx_تالا(خالی_اشاریہ);
        
        /* need to check to make sure it wasn't taken out from under us */
        اگر (gDefaultConverter != خالی_اشاریہ) {
            converter = gDefaultConverter;
            gDefaultConverter = خالی_اشاریہ;
        }
        icu::umtx_unlock(خالی_اشاریہ);
    }

    /* if the cache was empty, create a converter */
    اگر(converter == خالی_اشاریہ) {
        converter = ucnv_open(خالی_اشاریہ, حالت_2);
        اگر(U_FAILURE(*حالت_2)) {
            ucnv_close(converter);
            converter = خالی_اشاریہ;
        }
    }

    واپس converter;
}

U_CAPI خالی U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    اگر(gDefaultConverter == خالی_اشاریہ) {
        اگر (converter != خالی_اشاریہ) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_تالا(خالی_اشاریہ);
        اگر(gDefaultConverter == خالی_اشاریہ) {
            gDefaultConverter = converter;
            converter = خالی_اشاریہ;
        }
        icu::umtx_unlock(خالی_اشاریہ);
    }

    اگر(converter != خالی_اشاریہ) {
        ucnv_close(converter);
    }
}

U_CAPI خالی U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = خالی_اشاریہ;
    
    اگر (gDefaultConverter != خالی_اشاریہ) {
        icu::umtx_تالا(خالی_اشاریہ);
        
        /* need to check to make sure it wasn't taken out from under us */
        اگر (gDefaultConverter != خالی_اشاریہ) {
            converter = gDefaultConverter;
            gDefaultConverter = خالی_اشاریہ;
        }
        icu::umtx_unlock(خالی_اشاریہ);
    }

    /* if the cache was populated, flush it */
    اگر(converter != خالی_اشاریہ) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#تعریف_کرو MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ساکن int32_t u_astrnlen(ثابت حرف *s1, int32_t n)
{
    int32_t len = 0;

    اگر (s1)
    {
        جب (n-- && *(s1++))
        {
            len++;
        }
    }
    واپس len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ثابت حرف* s2, int32_t n)
{
  char16_t *ہدف = ucs1;
  UError_رمز err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  اگر(U_SUCCESS(err) && cnv != خالی_اشاریہ) {
    ucnv_reset(cnv);
    ucnv_مقصد_Unicode(cnv,
                   &ہدف,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   خالی_اشاریہ,
                   سچ,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    اگر(U_FAILURE(err) && (err != U_بفر_حد_سے_تجاوز_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    اگر(ہدف < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *ہدف = 0;  /* terminate */
    }
  } ورنہ {
    *ucs1 = 0;
  }
  واپس ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ثابت حرف* s2)
{
  UError_رمز err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  اگر(U_SUCCESS(err) && cnv != خالی_اشاریہ) {
    ucnv_مقصد_UChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    اگر(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } ورنہ {
    *ucs1 = 0;
  }
  واپس ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ساکن int32_t u_ustrnlen(ثابت char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    اگر (ucs1)
    {
        جب (n-- && *(ucs1++))
        {
            len++;
        }
    }
    واپس len;
}

U_CAPI حرف* U_EXPORT2
u_austrncpy(حرف* s1 U_LIFETIME_BOUND, ثابت char16_t* ucs2, int32_t n)
{
  حرف *ہدف = s1;
  UError_رمز err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  اگر(U_SUCCESS(err) && cnv != خالی_اشاریہ) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &ہدف,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  خالی_اشاریہ,
                  سچ,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    اگر(U_FAILURE(err) && (err != U_بفر_حد_سے_تجاوز_ERROR) ) {
      *s1 = 0; /* failure */
    }
    اگر(ہدف < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *ہدف = 0;  /* terminate */
    }
  } ورنہ {
    *s1 = 0;
  }
  واپس s1;
}

U_CAPI حرف* U_EXPORT2
u_austrcpy(حرف* s1 U_LIFETIME_BOUND, ثابت char16_t* ucs2)
{
  UError_رمز err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  اگر(U_SUCCESS(err) && cnv != خالی_اشاریہ) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } ورنہ {
    *s1 = 0;
  }
  واپس s1;
}

#اگر_ختم
