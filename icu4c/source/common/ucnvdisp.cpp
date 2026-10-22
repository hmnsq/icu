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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(ਅਚਲ UConverter *cnv,
                    ਅਚਲ ਅੱਖਰ *displayLocale,
                    char16_t *displayName, int32_t display_Name_ਸਮਰੱਥਾ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UResourceBundle *rb;
    ਅਚਲ char16_t *name;
    int32_t length;
    UError_ਕੋਡ local_ਸਥਿਤੀ = U_ZERO_ERROR;

    /* check arguments */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ(cnv==ਖਾਲੀ_ਸੂਚਕ || display_Name_ਸਮਰੱਥਾ<0 || (display_Name_ਸਮਰੱਥਾ>0 && displayName==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(ਖਾਲੀ_ਸੂਚਕ, displayLocale, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_ਕੁੰਜੀ(rb, cnv->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name, &length, &local_ਸਥਿਤੀ);
    ures_close(rb);

    ਜੇ(U_SUCCESS(local_ਸਥਿਤੀ)) {
        /* copy the string */
        ਜੇ (*p_Error_ਕੋਡ == U_ZERO_ERROR) {
            *p_Error_ਕੋਡ = local_ਸਥਿਤੀ;
        }
        u_memcpy(displayName, name, uprv_min(length, display_Name_ਸਮਰੱਥਾ)*U_SIZEOF_UCHAR);
    } ਨਹੀਂਤਾਂ {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name);
        u_chars_ਮੰਜ਼ਿਲ_UChars(cnv->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name, displayName, uprv_min(length, display_Name_ਸਮਰੱਥਾ));
    }
    ਵਾਪਸ u_terminateUChars(displayName, display_Name_ਸਮਰੱਥਾ, length, p_Error_ਕੋਡ);
}

#ਜੇ_ਅੰਤ

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
