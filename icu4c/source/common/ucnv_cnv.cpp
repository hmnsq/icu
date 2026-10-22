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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "cmemory.h"

U_CFUNC शून्य
ucnv_get_Complete_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                   ध्रुव USetAdder *sa,
                   UConverter_Unicode_समुच्चयः which,
                   UError_संहिता *p_Error_संहिता) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)p_Error_संहिता;
    sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0x10ffff);
}

U_CFUNC शून्य
ucnv_get_Non_Surrogate_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                               ध्रुव USetAdder *sa,
                               UConverter_Unicode_समुच्चयः which,
                               UError_संहिता *p_Error_संहिता) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)p_Error_संहिता;
    sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0xd7ff);
    sa->सङ्कलय_Range(sa->समुच्चयः, 0xe000, 0x10ffff);
}

U_CFUNC शून्य
ucnv_from_UWrite_अष्टकानि(UConverter *cnv,
                     ध्रुव वर्ण *अष्टकानि, int32_t length,
                     वर्ण **target, ध्रुव वर्ण *target_सीमा,
                     int32_t **offsets,
                     int32_t source_सूचकांक,
                     UError_संहिता *p_Error_संहिता) {
    वर्ण *t=*target;
    int32_t *o;

    /* write bytes */
    यदि(offsets==शून्यसूचकः || (o=*offsets)==शून्यसूचकः) {
        यावत्(length>0 && t<target_सीमा) {
            *t++=*अष्टकानि++;
            --length;
        }
    } अन्यथा {
        /* output with offsets */
        यावत्(length>0 && t<target_सीमा) {
            *t++=*अष्टकानि++;
            *o++=source_सूचकांक;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    यदि(length>0) {
        यदि(cnv!=शून्यसूचकः) {
            t=(वर्ण *)cnv->char_Error_बफर;
            cnv->char_Error_बफर_दीर्घता=(int8_t)length;
            कुरु {
                *t++=(uint8_t)*अष्टकानि++;
            } यावत्(--length>0);
        }
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }
}

U_CFUNC शून्य
ucnv_गन्तव्यम्_UWrite_UChars(UConverter *cnv,
                    ध्रुव char16_t *uchars, int32_t length,
                    char16_t **target, ध्रुव char16_t *target_सीमा,
                    int32_t **offsets,
                    int32_t source_सूचकांक,
                    UError_संहिता *p_Error_संहिता) {
    char16_t *t=*target;
    int32_t *o;

    /* write UChars */
    यदि(offsets==शून्यसूचकः || (o=*offsets)==शून्यसूचकः) {
        यावत्(length>0 && t<target_सीमा) {
            *t++=*uchars++;
            --length;
        }
    } अन्यथा {
        /* output with offsets */
        यावत्(length>0 && t<target_सीमा) {
            *t++=*uchars++;
            *o++=source_सूचकांक;
            --length;
        }
        *offsets=o;
    }
    *target=t;

    /* write overflow */
    यदि(length>0) {
        यदि(cnv!=शून्यसूचकः) {
            t=cnv->UChar_Error_बफर;
            cnv->UChar_Error_बफर_दीर्घता=(int8_t)length;
            कुरु {
                *t++=*uchars++;
            } यावत्(--length>0);
        }
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }
}

U_CFUNC शून्य
ucnv_गन्तव्यम्_UWrite_संहिता_Point(UConverter *cnv,
                       UChar32 c,
                       char16_t **target, ध्रुव char16_t *target_सीमा,
                       int32_t **offsets,
                       int32_t source_सूचकांक,
                       UError_संहिता *p_Error_संहिता) {
    char16_t *t;
    int32_t *o;

    t=*target;

    यदि(t<target_सीमा) {
        यदि(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } अन्यथा /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            यदि(t<target_सीमा) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        यदि(offsets!=शून्यसूचकः && (o=*offsets)!=शून्यसूचकः) {
            *o++=source_सूचकांक;
            यदि((*target+1)<t) {
                *o++=source_सूचकांक;
            }
            *offsets=o;
        }
    }

    *target=t;

    /* write overflow from c */
    यदि(c>=0) {
        यदि(cnv!=शून्यसूचकः) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_बफर, i, c);
            cnv->UChar_Error_बफर_दीर्घता=i;
        }
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }
}

#यदि_अन्तः
