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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ustring.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/ucnv.h"
#ضمن "cstring.h"
#ضمن "ustr_imp.h"
#ضمن "ucnv_imp.h"
#ضمن "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(ثابت UConverter *cnv,
                    ثابت حرف *displayLocale,
                    char16_t *displayName, int32_t display_Name_صلاحيت,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
    UResourceBundle *rb;
    ثابت char16_t *name;
    int32_t length;
    UError_ڪوڊ local_حالت = U_ZERO_ERROR;

    /* check arguments */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    إذا(cnv==خالي_اشارو || display_Name_صلاحيت<0 || (display_Name_صلاحيت>0 && displayName==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(خالي_اشارو, displayLocale, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_ڪنجي(rb, cnv->shared_معلومات->static_معلومات->name, &length, &local_حالت);
    ures_close(rb);

    إذا(U_SUCCESS(local_حالت)) {
        /* copy the string */
        إذا (*p_Error_ڪوڊ == U_ZERO_ERROR) {
            *p_Error_ڪوڊ = local_حالت;
        }
        u_memcpy(displayName, name, uprv_min(length, display_Name_صلاحيت)*U_SIZEOF_UCHAR);
    } وإلا {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_معلومات->static_معلومات->name);
        u_chars_منزل_UChars(cnv->shared_معلومات->static_معلومات->name, displayName, uprv_min(length, display_Name_صلاحيت));
    }
    أرجع u_terminateUChars(displayName, display_Name_صلاحيت, length, p_Error_ڪوڊ);
}

#جيڪڏهن_پورو

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
