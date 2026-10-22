// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1998-2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  ucnvdisp.c:
*  Implements APIs for the ICU's codeset conversion library display names.
*
* Modification History:
*
*   Date        Name        Description
*   04/04/99    helena      Fixed internal header inclusion.
*   05/09/00    helena      Added implementation to handle fallback mappings.
*   06/20/2000  helena      OS/400 port changes; mostly typecast.
*   09/08/2004  grhoten     split from ucnv.c
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "ucnv_imp.h"
#అన్తర్భావ "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(స్థిరరాశి UConverter *cnv,
                    స్థిరరాశి అక్షరం *displayLocale,
                    char16_t *displayName, int32_t display_Name_సామర్థ్యం,
                    UError_సంకేతం *p_Error_సంకేతం) {
    UResourceBundle *rb;
    స్థిరరాశి char16_t *name;
    int32_t length;
    UError_సంకేతం local_స్థితి = U_ZERO_ERROR;

    /* check arguments */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ(cnv==శూన్యసూచిక || display_Name_సామర్థ్యం<0 || (display_Name_సామర్థ్యం>0 && displayName==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(శూన్యసూచిక, displayLocale, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_కీలకం(rb, cnv->shared_దత్తాంశం->static_దత్తాంశం->name, &length, &local_స్థితి);
    ures_close(rb);

    ఒకవేళ(U_SUCCESS(local_స్థితి)) {
        /* copy the string */
        ఒకవేళ (*p_Error_సంకేతం == U_ZERO_ERROR) {
            *p_Error_సంకేతం = local_స్థితి;
        }
        u_memcpy(displayName, name, uprv_min(length, display_Name_సామర్థ్యం)*U_SIZEOF_UCHAR);
    } లేకపోతే {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_దత్తాంశం->static_దత్తాంశం->name);
        u_chars_గమ్యం_UChars(cnv->shared_దత్తాంశం->static_దత్తాంశం->name, displayName, uprv_min(length, display_Name_సామర్థ్యం));
    }
    తిరిగి u_terminateUChars(displayName, display_Name_సామర్థ్యం, length, p_Error_సంకేతం);
}

#అయితే_ముగింపు

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
