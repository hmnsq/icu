// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*   uconv_cnv.c:
*   Implements all the low level conversion functions
*   T_UnicodeConverter_{to,from}Unicode_$ConversionType
*
*   Change history:
*
*   06/29/2000  helena      Major rewrite of the callback APIs.
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv_err.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "cmemory.h"

U_CFUNC శూన్య
ucnv_get_Complete_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                   స్థిరరాశి USetAdder *sa,
                   UConverter_Unicode_సమితి which,
                   UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)cnv;
    (శూన్య)which;
    (శూన్య)p_Error_సంకేతం;
    sa->కూడు_Range(sa->సమితి, 0, 0x10ffff);
}

U_CFUNC శూన్య
ucnv_get_Non_Surrogate_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                               స్థిరరాశి USetAdder *sa,
                               UConverter_Unicode_సమితి which,
                               UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)cnv;
    (శూన్య)which;
    (శూన్య)p_Error_సంకేతం;
    sa->కూడు_Range(sa->సమితి, 0, 0xd7ff);
    sa->కూడు_Range(sa->సమితి, 0xe000, 0x10ffff);
}

U_CFUNC శూన్య
ucnv_from_UWrite_బైట్లు(UConverter *cnv,
                     స్థిరరాశి అక్షరం *బైట్లు, int32_t length,
                     అక్షరం **లక్ష్యం, స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు,
                     int32_t **offsets,
                     int32_t ఆధారం_సూచీ,
                     UError_సంకేతం *p_Error_సంకేతం) {
    అక్షరం *t=*లక్ష్యం;
    int32_t *o;

    /* write bytes */
    ఒకవేళ(offsets==శూన్యసూచిక || (o=*offsets)==శూన్యసూచిక) {
        అయితే(length>0 && t<లక్ష్యం_హద్దు) {
            *t++=*బైట్లు++;
            --length;
        }
    } లేకపోతే {
        /* output with offsets */
        అయితే(length>0 && t<లక్ష్యం_హద్దు) {
            *t++=*బైట్లు++;
            *o++=ఆధారం_సూచీ;
            --length;
        }
        *offsets=o;
    }
    *లక్ష్యం=t;

    /* write overflow */
    ఒకవేళ(length>0) {
        ఒకవేళ(cnv!=శూన్యసూచిక) {
            t=(అక్షరం *)cnv->char_Error_బఫరు;
            cnv->char_Error_బఫరు_నిడివి=(int8_t)length;
            చేయి {
                *t++=(uint8_t)*బైట్లు++;
            } అయితే(--length>0);
        }
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }
}

U_CFUNC శూన్య
ucnv_గమ్యం_UWrite_UChars(UConverter *cnv,
                    స్థిరరాశి char16_t *uchars, int32_t length,
                    char16_t **లక్ష్యం, స్థిరరాశి char16_t *లక్ష్యం_హద్దు,
                    int32_t **offsets,
                    int32_t ఆధారం_సూచీ,
                    UError_సంకేతం *p_Error_సంకేతం) {
    char16_t *t=*లక్ష్యం;
    int32_t *o;

    /* write UChars */
    ఒకవేళ(offsets==శూన్యసూచిక || (o=*offsets)==శూన్యసూచిక) {
        అయితే(length>0 && t<లక్ష్యం_హద్దు) {
            *t++=*uchars++;
            --length;
        }
    } లేకపోతే {
        /* output with offsets */
        అయితే(length>0 && t<లక్ష్యం_హద్దు) {
            *t++=*uchars++;
            *o++=ఆధారం_సూచీ;
            --length;
        }
        *offsets=o;
    }
    *లక్ష్యం=t;

    /* write overflow */
    ఒకవేళ(length>0) {
        ఒకవేళ(cnv!=శూన్యసూచిక) {
            t=cnv->UChar_Error_బఫరు;
            cnv->UChar_Error_బఫరు_నిడివి=(int8_t)length;
            చేయి {
                *t++=*uchars++;
            } అయితే(--length>0);
        }
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }
}

U_CFUNC శూన్య
ucnv_గమ్యం_UWrite_సంకేతం_బిందువు(UConverter *cnv,
                       UChar32 c,
                       char16_t **లక్ష్యం, స్థిరరాశి char16_t *లక్ష్యం_హద్దు,
                       int32_t **offsets,
                       int32_t ఆధారం_సూచీ,
                       UError_సంకేతం *p_Error_సంకేతం) {
    char16_t *t;
    int32_t *o;

    t=*లక్ష్యం;

    ఒకవేళ(t<లక్ష్యం_హద్దు) {
        ఒకవేళ(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } లేకపోతే /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            ఒకవేళ(t<లక్ష్యం_హద్దు) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        ఒకవేళ(offsets!=శూన్యసూచిక && (o=*offsets)!=శూన్యసూచిక) {
            *o++=ఆధారం_సూచీ;
            ఒకవేళ((*లక్ష్యం+1)<t) {
                *o++=ఆధారం_సూచీ;
            }
            *offsets=o;
        }
    }

    *లక్ష్యం=t;

    /* write overflow from c */
    ఒకవేళ(c>=0) {
        ఒకవేళ(cnv!=శూన్యసూచిక) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_బఫరు, i, c);
            cnv->UChar_Error_బఫరు_నిడివి=i;
        }
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }
}

#అయితే_ముగింపు
