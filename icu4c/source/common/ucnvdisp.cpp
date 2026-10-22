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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "ucnv_imp.h"
#अन्तर्भाव "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(ध्रुव UConverter *cnv,
                    ध्रुव वर्ण *displayLocale,
                    char16_t *displayName, int32_t display_Name_क्षमता,
                    UError_संहिता *p_Error_संहिता) {
    UResourceBundle *rb;
    ध्रुव char16_t *name;
    int32_t length;
    UError_संहिता local_स्थितिः = U_ZERO_ERROR;

    /* check arguments */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि(cnv==शून्यसूचकः || display_Name_क्षमता<0 || (display_Name_क्षमता>0 && displayName==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(शून्यसूचकः, displayLocale, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_कुञ्चिका(rb, cnv->shared_दत्त->static_दत्त->name, &length, &local_स्थितिः);
    ures_close(rb);

    यदि(U_SUCCESS(local_स्थितिः)) {
        /* copy the string */
        यदि (*p_Error_संहिता == U_ZERO_ERROR) {
            *p_Error_संहिता = local_स्थितिः;
        }
        u_memcpy(displayName, name, uprv_min(length, display_Name_क्षमता)*U_SIZEOF_UCHAR);
    } अन्यथा {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_दत्त->static_दत्त->name);
        u_chars_गन्तव्यम्_UChars(cnv->shared_दत्त->static_दत्त->name, displayName, uprv_min(length, display_Name_क्षमता));
    }
    प्रत्यागम u_terminateUChars(displayName, display_Name_क्षमता, length, p_Error_संहिता);
}

#यदि_अन्तः

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
