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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "ustr_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

ಸ್ಥಿರ UConverter *gDefaultConverter = ಶೂನ್ಯಸೂಚಕ;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    UConverter *converter = ಶೂನ್ಯಸೂಚಕ;
    
    ಆದರೆ (gDefaultConverter != ಶೂನ್ಯಸೂಚಕ) {
        icu::umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
        
        /* need to check to make sure it wasn't taken out from under us */
        ಆದರೆ (gDefaultConverter != ಶೂನ್ಯಸೂಚಕ) {
            converter = gDefaultConverter;
            gDefaultConverter = ಶೂನ್ಯಸೂಚಕ;
        }
        icu::umtx_unlock(ಶೂನ್ಯಸೂಚಕ);
    }

    /* if the cache was empty, create a converter */
    ಆದರೆ(converter == ಶೂನ್ಯಸೂಚಕ) {
        converter = ucnv_open(ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ucnv_close(converter);
            converter = ಶೂನ್ಯಸೂಚಕ;
        }
    }

    ಹಿಂತಿರುಗಿ converter;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    ಆದರೆ(gDefaultConverter == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (converter != ಶೂನ್ಯಸೂಚಕ) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
        ಆದರೆ(gDefaultConverter == ಶೂನ್ಯಸೂಚಕ) {
            gDefaultConverter = converter;
            converter = ಶೂನ್ಯಸೂಚಕ;
        }
        icu::umtx_unlock(ಶೂನ್ಯಸೂಚಕ);
    }

    ಆದರೆ(converter != ಶೂನ್ಯಸೂಚಕ) {
        ucnv_close(converter);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = ಶೂನ್ಯಸೂಚಕ;
    
    ಆದರೆ (gDefaultConverter != ಶೂನ್ಯಸೂಚಕ) {
        icu::umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
        
        /* need to check to make sure it wasn't taken out from under us */
        ಆದರೆ (gDefaultConverter != ಶೂನ್ಯಸೂಚಕ) {
            converter = gDefaultConverter;
            gDefaultConverter = ಶೂನ್ಯಸೂಚಕ;
        }
        icu::umtx_unlock(ಶೂನ್ಯಸೂಚಕ);
    }

    /* if the cache was populated, flush it */
    ಆದರೆ(converter != ಶೂನ್ಯಸೂಚಕ) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ಸ್ಥಿರ int32_t u_astrnlen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1, int32_t n)
{
    int32_t len = 0;

    ಆದರೆ (s1)
    {
        ಆಗ (n-- && *(s1++))
        {
            len++;
        }
    }
    ಹಿಂತಿರುಗಿ len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s2, int32_t n)
{
  char16_t *ಗುರಿ = ucs1;
  UError_ಸಂಕೇತ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ಆದರೆ(U_SUCCESS(err) && cnv != ಶೂನ್ಯಸೂಚಕ) {
    ucnv_reset(cnv);
    ucnv_ಗಮ್ಯ_Unicode(cnv,
                   &ಗುರಿ,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   ಶೂನ್ಯಸೂಚಕ,
                   ನಿಜ,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ಆದರೆ(U_FAILURE(err) && (err != U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    ಆದರೆ(ಗುರಿ < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *ಗುರಿ = 0;  /* terminate */
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    *ucs1 = 0;
  }
  ಹಿಂತಿರುಗಿ ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s2)
{
  UError_ಸಂಕೇತ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ಆದರೆ(U_SUCCESS(err) && cnv != ಶೂನ್ಯಸೂಚಕ) {
    ucnv_ಗಮ್ಯ_UChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    ಆದರೆ(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    *ucs1 = 0;
  }
  ಹಿಂತಿರುಗಿ ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
ಸ್ಥಿರ int32_t u_ustrnlen(ಸ್ಥಿರಾಂಕ char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    ಆದರೆ (ucs1)
    {
        ಆಗ (n-- && *(ucs1++))
        {
            len++;
        }
    }
    ಹಿಂತಿರುಗಿ len;
}

U_CAPI ಅಕ್ಷರ* U_EXPORT2
u_austrncpy(ಅಕ್ಷರ* s1 U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* ucs2, int32_t n)
{
  ಅಕ್ಷರ *ಗುರಿ = s1;
  UError_ಸಂಕೇತ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ಆದರೆ(U_SUCCESS(err) && cnv != ಶೂನ್ಯಸೂಚಕ) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &ಗುರಿ,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  ಶೂನ್ಯಸೂಚಕ,
                  ನಿಜ,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ಆದರೆ(U_FAILURE(err) && (err != U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) ) {
      *s1 = 0; /* failure */
    }
    ಆದರೆ(ಗುರಿ < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *ಗುರಿ = 0;  /* terminate */
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    *s1 = 0;
  }
  ಹಿಂತಿರುಗಿ s1;
}

U_CAPI ಅಕ್ಷರ* U_EXPORT2
u_austrcpy(ಅಕ್ಷರ* s1 U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ char16_t* ucs2)
{
  UError_ಸಂಕೇತ err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ಆದರೆ(U_SUCCESS(err) && cnv != ಶೂನ್ಯಸೂಚಕ) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } ಇಲ್ಲದಿದ್ದರೆ {
    *s1 = 0;
  }
  ಹಿಂತಿರುಗಿ s1;
}

#ಆದರೆ_ಅಂತ್ಯ
