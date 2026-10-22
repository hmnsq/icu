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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "ustr_cnv.h"
#അന്തര്ഭാവ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

സ്ഥിതം UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    എങ്കിൽ (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        എങ്കിൽ (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    എങ്കിൽ(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        എങ്കിൽ(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    തിരികെ converter;
}

U_CAPI ശൂന്യം U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    എങ്കിൽ(gDefaultConverter == nullptr) {
        എങ്കിൽ (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        എങ്കിൽ(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    എങ്കിൽ(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI ശൂന്യം U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    എങ്കിൽ (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        എങ്കിൽ (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    എങ്കിൽ(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
സ്ഥിതം int32_t u_astrnlen(സ്ഥിരം അക്ഷരം *s1, int32_t n)
{
    int32_t len = 0;

    എങ്കിൽ (s1)
    {
        ആകുമ്പോൾ (n-- && *(s1++))
        {
            len++;
        }
    }
    തിരികെ len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, സ്ഥിരം അക്ഷരം* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  എങ്കിൽ(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   സത്യം,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    എങ്കിൽ(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    എങ്കിൽ(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } അല്ലെങ്കിൽ {
    *ucs1 = 0;
  }
  തിരികെ ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, സ്ഥിരം അക്ഷരം* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  എങ്കിൽ(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    എങ്കിൽ(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } അല്ലെങ്കിൽ {
    *ucs1 = 0;
  }
  തിരികെ ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
സ്ഥിതം int32_t u_ustrnlen(സ്ഥിരം char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    എങ്കിൽ (ucs1)
    {
        ആകുമ്പോൾ (n-- && *(ucs1++))
        {
            len++;
        }
    }
    തിരികെ len;
}

U_CAPI അക്ഷരം* U_EXPORT2
u_austrncpy(അക്ഷരം* s1 U_LIFETIME_BOUND, സ്ഥിരം char16_t* ucs2, int32_t n)
{
  അക്ഷരം *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  എങ്കിൽ(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  സത്യം,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    എങ്കിൽ(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    എങ്കിൽ(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } അല്ലെങ്കിൽ {
    *s1 = 0;
  }
  തിരികെ s1;
}

U_CAPI അക്ഷരം* U_EXPORT2
u_austrcpy(അക്ഷരം* s1 U_LIFETIME_BOUND, സ്ഥിരം char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  എങ്കിൽ(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } അല്ലെങ്കിൽ {
    *s1 = 0;
  }
  തിരികെ s1;
}

#endif
