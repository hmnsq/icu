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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "ucnv_imp.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *displayLocale,
                    char16_t *displayName, int32_t display_Name_ಸಾಮರ್ಥ್ಯ,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UResourceBundle *rb;
    ಸ್ಥಿರಾಂಕ char16_t *name;
    int32_t length;
    UError_ಸಂಕೇತ local_ಸ್ಥಿತಿ = U_ZERO_ERROR;

    /* check arguments */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(cnv==ಶೂನ್ಯಸೂಚಕ || display_Name_ಸಾಮರ್ಥ್ಯ<0 || (display_Name_ಸಾಮರ್ಥ್ಯ>0 && displayName==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(ಶೂನ್ಯಸೂಚಕ, displayLocale, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_ಕೀಲಿ(rb, cnv->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name, &length, &local_ಸ್ಥಿತಿ);
    ures_close(rb);

    ಆದರೆ(U_SUCCESS(local_ಸ್ಥಿತಿ)) {
        /* copy the string */
        ಆದರೆ (*p_Error_ಸಂಕೇತ == U_ZERO_ERROR) {
            *p_Error_ಸಂಕೇತ = local_ಸ್ಥಿತಿ;
        }
        u_memcpy(displayName, name, uprv_min(length, display_Name_ಸಾಮರ್ಥ್ಯ)*U_SIZEOF_UCHAR);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name);
        u_chars_ಗಮ್ಯ_UChars(cnv->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name, displayName, uprv_min(length, display_Name_ಸಾಮರ್ಥ್ಯ));
    }
    ಹಿಂತಿರುಗಿ u_terminateUChars(displayName, display_Name_ಸಾಮರ್ಥ್ಯ, length, p_Error_ಸಂಕೇತ);
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
