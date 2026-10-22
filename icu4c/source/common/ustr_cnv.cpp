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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ustring.h"
#ضمن "unicode/ucnv.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "umutex.h"
#ضمن "ustr_cnv.h"
#ضمن "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

ساكن UConverter *gDefaultConverter = تش_اشاره;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UError_کوډ *حالت)
{
    UConverter *converter = تش_اشاره;
    
    إذا (gDefaultConverter != تش_اشاره) {
        icu::umtx_قلف(تش_اشاره);
        
        /* need to check to make sure it wasn't taken out from under us */
        إذا (gDefaultConverter != تش_اشاره) {
            converter = gDefaultConverter;
            gDefaultConverter = تش_اشاره;
        }
        icu::umtx_unlock(تش_اشاره);
    }

    /* if the cache was empty, create a converter */
    إذا(converter == تش_اشاره) {
        converter = ucnv_open(تش_اشاره, حالت);
        إذا(U_FAILURE(*حالت)) {
            ucnv_close(converter);
            converter = تش_اشاره;
        }
    }

    أرجع converter;
}

U_CAPI فراغ U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    إذا(gDefaultConverter == تش_اشاره) {
        إذا (converter != تش_اشاره) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_قلف(تش_اشاره);
        إذا(gDefaultConverter == تش_اشاره) {
            gDefaultConverter = converter;
            converter = تش_اشاره;
        }
        icu::umtx_unlock(تش_اشاره);
    }

    إذا(converter != تش_اشاره) {
        ucnv_close(converter);
    }
}

U_CAPI فراغ U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = تش_اشاره;
    
    إذا (gDefaultConverter != تش_اشاره) {
        icu::umtx_قلف(تش_اشاره);
        
        /* need to check to make sure it wasn't taken out from under us */
        إذا (gDefaultConverter != تش_اشاره) {
            converter = gDefaultConverter;
            gDefaultConverter = تش_اشاره;
        }
        icu::umtx_unlock(تش_اشاره);
    }

    /* if the cache was populated, flush it */
    إذا(converter != تش_اشاره) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#تعریف_کړه MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ساكن int32_t u_astrnlen(ثابت حرف *s1, int32_t n)
{
    int32_t len = 0;

    إذا (s1)
    {
        طالما (n-- && *(s1++))
        {
            len++;
        }
    }
    أرجع len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ثابت حرف* s2, int32_t n)
{
  char16_t *موخه = ucs1;
  UError_کوډ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  إذا(U_SUCCESS(err) && cnv != تش_اشاره) {
    ucnv_reset(cnv);
    ucnv_منزل_Unicode(cnv,
                   &موخه,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   تش_اشاره,
                   ریښتیا,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    إذا(U_FAILURE(err) && (err != U_بفر_ډکېدنه_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    إذا(موخه < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *موخه = 0;  /* terminate */
    }
  } وإلا {
    *ucs1 = 0;
  }
  أرجع ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ثابت حرف* s2)
{
  UError_کوډ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  إذا(U_SUCCESS(err) && cnv != تش_اشاره) {
    ucnv_منزل_UChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    إذا(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } وإلا {
    *ucs1 = 0;
  }
  أرجع ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ساكن int32_t u_ustrnlen(ثابت char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    إذا (ucs1)
    {
        طالما (n-- && *(ucs1++))
        {
            len++;
        }
    }
    أرجع len;
}

U_CAPI حرف* U_EXPORT2
u_austrncpy(حرف* s1 U_LIFETIME_BOUND, ثابت char16_t* ucs2, int32_t n)
{
  حرف *موخه = s1;
  UError_کوډ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  إذا(U_SUCCESS(err) && cnv != تش_اشاره) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &موخه,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  تش_اشاره,
                  ریښتیا,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    إذا(U_FAILURE(err) && (err != U_بفر_ډکېدنه_ERROR) ) {
      *s1 = 0; /* failure */
    }
    إذا(موخه < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *موخه = 0;  /* terminate */
    }
  } وإلا {
    *s1 = 0;
  }
  أرجع s1;
}

U_CAPI حرف* U_EXPORT2
u_austrcpy(حرف* s1 U_LIFETIME_BOUND, ثابت char16_t* ucs2)
{
  UError_کوډ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  إذا(U_SUCCESS(err) && cnv != تش_اشاره) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } وإلا {
    *s1 = 0;
  }
  أرجع s1;
}

#د_که_پای
