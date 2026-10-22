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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ustring.h"
#شامل "unicode/ures.h"
#شامل "unicode/ucnv.h"
#شامل "cstring.h"
#شامل "ustr_imp.h"
#شامل "ucnv_imp.h"
#شامل "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(ثابت UConverter *cnv,
                    ثابت حرف *displayLocale,
                    char16_t *displayName, int32_t display_Name_استعداد,
                    UError_رمز *p_Error_رمز) {
    UResourceBundle *rb;
    ثابت char16_t *name;
    int32_t length;
    UError_رمز local_حالت = U_ZERO_ERROR;

    /* check arguments */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر(cnv==خالی_اشاریہ || display_Name_استعداد<0 || (display_Name_استعداد>0 && displayName==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(خالی_اشاریہ, displayLocale, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_کلید(rb, cnv->shared_ڈیٹا->static_ڈیٹا->name, &length, &local_حالت);
    ures_close(rb);

    اگر(U_SUCCESS(local_حالت)) {
        /* copy the string */
        اگر (*p_Error_رمز == U_ZERO_ERROR) {
            *p_Error_رمز = local_حالت;
        }
        u_memcpy(displayName, name, uprv_min(length, display_Name_استعداد)*U_SIZEOF_UCHAR);
    } ورنہ {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_ڈیٹا->static_ڈیٹا->name);
        u_chars_مقصد_UChars(cnv->shared_ڈیٹا->static_ڈیٹا->name, displayName, uprv_min(length, display_Name_استعداد));
    }
    واپس u_terminateUChars(displayName, display_Name_استعداد, length, p_Error_رمز);
}

#اگر_ختم

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
