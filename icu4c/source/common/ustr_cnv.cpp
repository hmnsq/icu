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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ustr_cnv.h"
#अन्तर्भाव "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

स्थिर UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    यदि (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        यदि (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    यदि(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        यदि(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    प्रत्यागम converter;
}

U_CAPI शून्य U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    यदि(gDefaultConverter == nullptr) {
        यदि (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        यदि(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    यदि(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI शून्य U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    यदि (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        यदि (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    यदि(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
स्थिर int32_t u_astrnlen(ध्रुव वर्ण *s1, int32_t n)
{
    int32_t len = 0;

    यदि (s1)
    {
        यावत् (n-- && *(s1++))
        {
            len++;
        }
    }
    प्रत्यागम len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ध्रुव वर्ण* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  यदि(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   सत्य,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    यदि(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    यदि(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } अन्यथा {
    *ucs1 = 0;
  }
  प्रत्यागम ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ध्रुव वर्ण* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  यदि(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    यदि(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } अन्यथा {
    *ucs1 = 0;
  }
  प्रत्यागम ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
स्थिर int32_t u_ustrnlen(ध्रुव char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    यदि (ucs1)
    {
        यावत् (n-- && *(ucs1++))
        {
            len++;
        }
    }
    प्रत्यागम len;
}

U_CAPI वर्ण* U_EXPORT2
u_austrncpy(वर्ण* s1 U_LIFETIME_BOUND, ध्रुव char16_t* ucs2, int32_t n)
{
  वर्ण *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  यदि(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  सत्य,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    यदि(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    यदि(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } अन्यथा {
    *s1 = 0;
  }
  प्रत्यागम s1;
}

U_CAPI वर्ण* U_EXPORT2
u_austrcpy(वर्ण* s1 U_LIFETIME_BOUND, ध्रुव char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  यदि(U_SUCCESS(err) && cnv != nullptr) {
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
  प्रत्यागम s1;
}

#endif
