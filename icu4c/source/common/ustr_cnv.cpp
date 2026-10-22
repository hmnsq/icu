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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/ucnv.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "ustr_cnv.h"
#অন্তর্ভাৱ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

স্থির UConverter *gDefaultConverter = nullptr;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UErrorCode *status)
{
    UConverter *converter = nullptr;
    
    যদি (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        যদি (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was empty, create a converter */
    যদি(converter == nullptr) {
        converter = ucnv_open(nullptr, status);
        যদি(U_FAILURE(*status)) {
            ucnv_close(converter);
            converter = nullptr;
        }
    }

    ঘূৰাই converter;
}

U_CAPI শূন্য U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    যদি(gDefaultConverter == nullptr) {
        যদি (converter != nullptr) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_lock(nullptr);
        যদি(gDefaultConverter == nullptr) {
            gDefaultConverter = converter;
            converter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    যদি(converter != nullptr) {
        ucnv_close(converter);
    }
}

U_CAPI শূন্য U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = nullptr;
    
    যদি (gDefaultConverter != nullptr) {
        icu::umtx_lock(nullptr);
        
        /* need to check to make sure it wasn't taken out from under us */
        যদি (gDefaultConverter != nullptr) {
            converter = gDefaultConverter;
            gDefaultConverter = nullptr;
        }
        icu::umtx_unlock(nullptr);
    }

    /* if the cache was populated, flush it */
    যদি(converter != nullptr) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#define MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
স্থির int32_t u_astrnlen(ধ্রুৱ ৱর্ণ *s1, int32_t n)
{
    int32_t len = 0;

    যদি (s1)
    {
        যেতিয়ালৈ (n-- && *(s1++))
        {
            len++;
        }
    }
    ঘূৰাই len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ধ্রুৱ ৱর্ণ* s2, int32_t n)
{
  char16_t *target = ucs1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  যদি(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_toUnicode(cnv,
                   &target,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   nullptr,
                   সঁচা,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    যদি(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    যদি(target < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } অন্যথা {
    *ucs1 = 0;
  }
  ঘূৰাই ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ধ্রুৱ ৱর্ণ* s2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  যদি(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_toUChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    যদি(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } অন্যথা {
    *ucs1 = 0;
  }
  ঘূৰাই ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
স্থির int32_t u_ustrnlen(ধ্রুৱ char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    যদি (ucs1)
    {
        যেতিয়ালৈ (n-- && *(ucs1++))
        {
            len++;
        }
    }
    ঘূৰাই len;
}

U_CAPI ৱর্ণ* U_EXPORT2
u_austrncpy(ৱর্ণ* s1 U_LIFETIME_BOUND, ধ্রুৱ char16_t* ucs2, int32_t n)
{
  ৱর্ণ *target = s1;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  যদি(U_SUCCESS(err) && cnv != nullptr) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &target,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  nullptr,
                  সঁচা,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    যদি(U_FAILURE(err) && (err != U_BUFFER_OVERFLOW_ERROR) ) {
      *s1 = 0; /* failure */
    }
    যদি(target < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *target = 0;  /* terminate */
    }
  } অন্যথা {
    *s1 = 0;
  }
  ঘূৰাই s1;
}

U_CAPI ৱর্ণ* U_EXPORT2
u_austrcpy(ৱর্ণ* s1 U_LIFETIME_BOUND, ধ্রুৱ char16_t* ucs2)
{
  UErrorCode err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  যদি(U_SUCCESS(err) && cnv != nullptr) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } অন্যথা {
    *s1 = 0;
  }
  ঘূৰাই s1;
}

#endif
