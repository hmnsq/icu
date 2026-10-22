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
                    char16_t *displayName, int32_t display_Name_ظرفیت,
                    UError_کد *p_Error_کد) {
    UResourceBundle *rb;
    ثابت char16_t *name;
    int32_t length;
    UError_کد local_وضعیت = U_ZERO_ERROR;

    /* check arguments */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    إذا(cnv==اشاره_تهی || display_Name_ظرفیت<0 || (display_Name_ظرفیت>0 && displayName==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(اشاره_تهی, displayLocale, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_کلید(rb, cnv->shared_داده->static_داده->name, &length, &local_وضعیت);
    ures_close(rb);

    إذا(U_SUCCESS(local_وضعیت)) {
        /* copy the string */
        إذا (*p_Error_کد == U_ZERO_ERROR) {
            *p_Error_کد = local_وضعیت;
        }
        u_memcpy(displayName, name, uprv_min(length, display_Name_ظرفیت)*U_SIZEOF_UCHAR);
    } وإلا {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_داده->static_داده->name);
        u_chars_مقصد_UChars(cnv->shared_داده->static_داده->name, displayName, uprv_min(length, display_Name_ظرفیت));
    }
    أرجع u_terminateUChars(displayName, display_Name_ظرفیت, length, p_Error_کد);
}

#پایان_اگر

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
