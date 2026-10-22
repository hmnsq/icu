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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "ustr_cnv.h"
#అన్తర్భావ "ucnv_bld.h"

/* mutexed access to a shared default converter ----------------------------- */

స్థిర UConverter *gDefaultConverter = శూన్యసూచిక;

U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UError_సంకేతం *స్థితి_2)
{
    UConverter *converter = శూన్యసూచిక;
    
    ఒకవేళ (gDefaultConverter != శూన్యసూచిక) {
        icu::umtx_తాళం(శూన్యసూచిక);
        
        /* need to check to make sure it wasn't taken out from under us */
        ఒకవేళ (gDefaultConverter != శూన్యసూచిక) {
            converter = gDefaultConverter;
            gDefaultConverter = శూన్యసూచిక;
        }
        icu::umtx_unlock(శూన్యసూచిక);
    }

    /* if the cache was empty, create a converter */
    ఒకవేళ(converter == శూన్యసూచిక) {
        converter = ucnv_open(శూన్యసూచిక, స్థితి_2);
        ఒకవేళ(U_FAILURE(*స్థితి_2)) {
            ucnv_close(converter);
            converter = శూన్యసూచిక;
        }
    }

    తిరిగి converter;
}

U_CAPI శూన్య U_EXPORT2
u_releaseDefaultConverter(UConverter *converter)
{
    ఒకవేళ(gDefaultConverter == శూన్యసూచిక) {
        ఒకవేళ (converter != శూన్యసూచిక) {
            ucnv_reset(converter);
        }
        ucnv_enableCleanup();
        icu::umtx_తాళం(శూన్యసూచిక);
        ఒకవేళ(gDefaultConverter == శూన్యసూచిక) {
            gDefaultConverter = converter;
            converter = శూన్యసూచిక;
        }
        icu::umtx_unlock(శూన్యసూచిక);
    }

    ఒకవేళ(converter != శూన్యసూచిక) {
        ucnv_close(converter);
    }
}

U_CAPI శూన్య U_EXPORT2
u_flushDefaultConverter()
{
    UConverter *converter = శూన్యసూచిక;
    
    ఒకవేళ (gDefaultConverter != శూన్యసూచిక) {
        icu::umtx_తాళం(శూన్యసూచిక);
        
        /* need to check to make sure it wasn't taken out from under us */
        ఒకవేళ (gDefaultConverter != శూన్యసూచిక) {
            converter = gDefaultConverter;
            gDefaultConverter = శూన్యసూచిక;
        }
        icu::umtx_unlock(శూన్యసూచిక);
    }

    /* if the cache was populated, flush it */
    ఒకవేళ(converter != శూన్యసూచిక) {
         ucnv_close(converter);
    }
}


/* conversions between char* and char16_t* ------------------------------------- */

/* maximum string length for u_uastrcpy() and u_austrcpy() implementations */
#నిర్వచించు MAX_STRLEN 0x0FFFFFFF

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
స్థిర int32_t u_astrnlen(స్థిరరాశి అక్షరం *s1, int32_t n)
{
    int32_t len = 0;

    ఒకవేళ (s1)
    {
        అయితే (n-- && *(s1++))
        {
            len++;
        }
    }
    తిరిగి len;
}

U_CAPI char16_t* U_EXPORT2
u_uastrncpy(char16_t* ucs1 U_LIFETIME_BOUND, స్థిరరాశి అక్షరం* s2, int32_t n)
{
  char16_t *లక్ష్యం = ucs1;
  UError_సంకేతం err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ఒకవేళ(U_SUCCESS(err) && cnv != శూన్యసూచిక) {
    ucnv_reset(cnv);
    ucnv_గమ్యం_Unicode(cnv,
                   &లక్ష్యం,
                   ucs1+n,
                   &s2,
                   s2+u_astrnlen(s2, n),
                   శూన్యసూచిక,
                   నిజం,
                   &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ఒకవేళ(U_FAILURE(err) && (err != U_బఫరు_అధికప్రవాహం_ERROR) ) {
      *ucs1 = 0; /* failure */
    }
    ఒకవేళ(లక్ష్యం < (ucs1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *లక్ష్యం = 0;  /* terminate */
    }
  } లేకపోతే {
    *ucs1 = 0;
  }
  తిరిగి ucs1;
}

U_CAPI char16_t* U_EXPORT2
u_uastrcpy(char16_t* ucs1 U_LIFETIME_BOUND, స్థిరరాశి అక్షరం* s2)
{
  UError_సంకేతం err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ఒకవేళ(U_SUCCESS(err) && cnv != శూన్యసూచిక) {
    ucnv_గమ్యం_UChars(cnv,
                    ucs1,
                    MAX_STRLEN,
                    s2,
                    (int32_t)uprv_strlen(s2),
                    &err);
    u_releaseDefaultConverter(cnv);
    ఒకవేళ(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } లేకపోతే {
    *ucs1 = 0;
  }
  తిరిగి ucs1;
}

/*
 returns the minimum of (the length of the null-terminated string) and n.
*/
స్థిర int32_t u_ustrnlen(స్థిరరాశి char16_t *ucs1, int32_t n)
{
    int32_t len = 0;

    ఒకవేళ (ucs1)
    {
        అయితే (n-- && *(ucs1++))
        {
            len++;
        }
    }
    తిరిగి len;
}

U_CAPI అక్షరం* U_EXPORT2
u_austrncpy(అక్షరం* s1 U_LIFETIME_BOUND, స్థిరరాశి char16_t* ucs2, int32_t n)
{
  అక్షరం *లక్ష్యం = s1;
  UError_సంకేతం err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ఒకవేళ(U_SUCCESS(err) && cnv != శూన్యసూచిక) {
    ucnv_reset(cnv);
    ucnv_fromUnicode(cnv,
                  &లక్ష్యం,
                  s1+n,
                  &ucs2,
                  ucs2+u_ustrnlen(ucs2, n),
                  శూన్యసూచిక,
                  నిజం,
                  &err);
    ucnv_reset(cnv); /* be good citizens */
    u_releaseDefaultConverter(cnv);
    ఒకవేళ(U_FAILURE(err) && (err != U_బఫరు_అధికప్రవాహం_ERROR) ) {
      *s1 = 0; /* failure */
    }
    ఒకవేళ(లక్ష్యం < (s1+n)) { /* U_BUFFER_OVERFLOW_ERROR isn't an err, just means no termination will happen. */
      *లక్ష్యం = 0;  /* terminate */
    }
  } లేకపోతే {
    *s1 = 0;
  }
  తిరిగి s1;
}

U_CAPI అక్షరం* U_EXPORT2
u_austrcpy(అక్షరం* s1 U_LIFETIME_BOUND, స్థిరరాశి char16_t* ucs2)
{
  UError_సంకేతం err = U_ZERO_ERROR;
  UConverter *cnv = u_getDefaultConverter(&err);
  ఒకవేళ(U_SUCCESS(err) && cnv != శూన్యసూచిక) {
    int32_t len = ucnv_fromUChars(cnv,
                  s1,
                  MAX_STRLEN,
                  ucs2,
                  -1,
                  &err);
    u_releaseDefaultConverter(cnv);
    s1[len] = 0;
  } లేకపోతే {
    *s1 = 0;
  }
  తిరిగి s1;
}

#అయితే_ముగింపు
